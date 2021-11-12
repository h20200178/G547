#include <linux/version.h> 
#include <linux/blk-mq.h>	
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>
#include <linux/bio.h>

static int major_num = 0;	//dynamic allocation of major number 
static int sect_size = 512;	//sector size
static int nsectors = 1024;	//number of sectors
static int mode = 0;		//Simple transfer

//Partition info
#define KERNEL_SECTOR_SIZE 512
#define MBR_SIZE KERNEL_SECTOR_SIZE
#define MBR_DISK_SIGNATURE_OFFSET 440
#define MBR_DISK_SIGNATURE_SIZE 4
#define PARTITION_TABLE_OFFSET 446
#define PARTITION_ENTRY_SIZE 16 
#define PARTITION_TABLE_SIZE 64 
#define MBR_SIGNATURE_OFFSET 510
#define MBR_SIGNATURE_SIZE 2
#define MBR_SIGNATURE 0xAA55
#define DISK_MINORS 2	//Minor number

typedef struct
{
	unsigned char boot_type; 
	unsigned char start_head;
	unsigned char start_sec:6;
	unsigned char start_cyl_hi:2;
	unsigned char start_cyl;
	unsigned char part_type;
	unsigned char end_head;
	unsigned char end_sec:6;
	unsigned char end_cyl_hi:2;
	unsigned char end_cyl;
	unsigned int abs_start_sec;
	unsigned int sec_in_part;
} PartEntry;

typedef PartEntry PartTable[4];

static PartTable def_part_table =
{
	{
		boot_type: 0x00,
		start_head: 0x00,
		start_sec: 0x2,
		start_cyl: 0x00,
		part_type: 0x83,
		end_head: 0x00,
		end_sec: 0x20,
		end_cyl: 0x09,
		abs_start_sec: 0x00000001,
		sec_in_part: 0x0000013F
	},
	{
		boot_type: 0x00,
		start_head: 0x00,
		start_sec: 0x1,
		start_cyl: 0x14,
		part_type: 0x83,
		end_head: 0x00,
		end_sec: 0x20,
		end_cyl: 0x1F,
		abs_start_sec: 0x00000280,
		sec_in_part: 0x00000180
	},
	{	
	},
	{
	}
};

struct disk_dev {
	int size;                       
	u8 *data;                      
	spinlock_t lock;                
	struct blk_mq_tag_set tag_set;	
	struct request_queue *queue;
	struct gendisk *gd;        
}disk_device;


static void copy_mbr(u8 *disk)
{
	memset(disk, 0x0, MBR_SIZE);
	*(unsigned long *)(disk + MBR_DISK_SIGNATURE_OFFSET) = 0x36E5756D;
	memcpy(disk + PARTITION_TABLE_OFFSET, &def_part_table, PARTITION_TABLE_SIZE);
	*(unsigned short *)(disk + MBR_SIGNATURE_OFFSET) = MBR_SIGNATURE;
}

static int disk_open(struct block_device *bdev, fmode_t mode)	 
{
	printk(KERN_INFO "mydiskdrive : open \n");
	return 0;
}

static void disk_release(struct gendisk *disk, fmode_t mode)
{
	printk(KERN_INFO "mydiskdrive : closed \n");
}

static struct block_device_operations fops =
{
	.owner = THIS_MODULE,
	.open = disk_open,
	.release = disk_release,
};

static void disk_transfer(struct disk_dev *dev, unsigned long sector,
		unsigned long nsect, char *buffer, int dir)
{
	unsigned long offset = sector*KERNEL_SECTOR_SIZE;
	unsigned long nbytes = nsect*KERNEL_SECTOR_SIZE;

	if ((offset + nbytes) > dev->size) {
		printk (KERN_NOTICE "Beyond-end write (%ld %ld)\n", offset, nbytes);
		return;
	}
	if (dir)
		memcpy(dev->data + offset, buffer, nbytes);
	else
		memcpy(buffer, dev->data + offset, nbytes);
}


#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 9, 0))
static inline struct request_queue *
blk_generic_alloc_queue(make_request_fn make_request, int node_id)
#else
static inline struct request_queue *
blk_generic_alloc_queue(int node_id)
#endif
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 7, 0))
	struct request_queue *q = blk_alloc_queue(GFP_KERNEL);
	if (q != NULL)
		blk_queue_make_request(q, make_request);

	return (q);
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(5, 9, 0))
	return (blk_alloc_queue(make_request, node_id));
#else
	return (blk_alloc_queue(node_id));
#endif
}

static blk_status_t disk_request(struct blk_mq_hw_ctx *hctx, const struct blk_mq_queue_data* bd)  
{
	struct request *req = bd->rq;
	struct disk_dev *dev = req->rq_disk->private_data;
        struct bio_vec bvec;
        struct req_iterator iter;
        sector_t pos_sector = blk_rq_pos(req);
	void	*buffer;
	blk_status_t  ret;

	blk_mq_start_request (req);

	if (blk_rq_is_passthrough(req)) {
		printk (KERN_NOTICE "Skip non-fs request\n");
                ret = BLK_STS_IOERR;  //-EIO
			goto done;
	}
	rq_for_each_segment(bvec, req, iter)
	{
		size_t num_sector = blk_rq_cur_sectors(req);
		printk (KERN_NOTICE "dir %d sec %lld, nr %ld\n",
                        rq_data_dir(req),
                        pos_sector, num_sector);
		buffer = page_address(bvec.bv_page) + bvec.bv_offset;
		disk_transfer(dev, pos_sector, num_sector,
				buffer, rq_data_dir(req) == WRITE);
		pos_sector += num_sector;
	}
	ret = BLK_STS_OK;
done:
	blk_mq_end_request (req, ret);
	return ret;
}



static int disk_xfer_bio(struct disk_dev *dev, struct bio *bio)
{
	struct bio_vec bvec;
	struct bvec_iter iter;
	sector_t sector = bio->bi_iter.bi_sector;

	bio_for_each_segment(bvec, bio, iter) {
		char *buffer = kmap_atomic(bvec.bv_page) + bvec.bv_offset;
		disk_transfer(dev, sector, (bio_cur_bytes(bio) / KERNEL_SECTOR_SIZE),
				buffer, bio_data_dir(bio) == WRITE);
		sector += (bio_cur_bytes(bio) / KERNEL_SECTOR_SIZE);
		kunmap_atomic(buffer);
	}
	return 0; 
}


static int disk_xfer_request(struct disk_dev *dev, struct request *req)
{
	struct bio *bio;
	int nsect = 0;
    
	__rq_for_each_bio(bio, req) {
		disk_xfer_bio(dev, bio);
		nsect += bio->bi_iter.bi_size/KERNEL_SECTOR_SIZE;
	}
	return nsect;
}

static blk_status_t disk_full_request(struct blk_mq_hw_ctx * hctx, const struct blk_mq_queue_data * bd)
{
	struct request *req = bd->rq;
	int sectors_xferred;
	struct disk_dev *dev = req->q->queuedata;
	blk_status_t  ret;

	blk_mq_start_request (req);
		if (blk_rq_is_passthrough(req)) {
			printk (KERN_NOTICE "Skip non-fs request\n");
			ret = BLK_STS_IOERR; //-EIO;
			goto done;
		}
		sectors_xferred = disk_xfer_request(dev, req);
		ret = BLK_STS_OK; 
	done:
		blk_mq_end_request (req, ret);
	return ret;
}

static struct blk_mq_ops mq_ops_simple = {
    .queue_rq = disk_request,
};

static struct blk_mq_ops mq_ops_full = {
    .queue_rq = disk_full_request,
};

static int __init disk_init(void)
{
	major_num = register_blkdev(major_num, "dof");
	if (major_num <= 0) {
		printk(KERN_INFO "disk: unable to get major number\n");
		return -EBUSY;
	}
        struct disk_dev* dev = &disk_device;
	
	disk_device.size = nsectors*sect_size;
	disk_device.data = vmalloc(disk_device.size);
	copy_mbr(disk_device.data);
	
	spin_lock_init(&disk_device.lock);	
	switch (mode) {
	   case 0 :
		disk_device.queue = blk_mq_init_sq_queue(&disk_device.tag_set, &mq_ops_simple, 128, BLK_MQ_F_SHOULD_MERGE);
		break;
	   case 1 :
		disk_device.queue = blk_mq_init_sq_queue(&disk_device.tag_set, &mq_ops_full, 128, BLK_MQ_F_SHOULD_MERGE);
		break;
	}
	
	blk_queue_logical_block_size(disk_device.queue, sect_size);
	(disk_device.queue)->queuedata = dev;
	
	disk_device.gd = alloc_disk(DISK_MINORS);
	disk_device.gd->major = major_num;
	disk_device.gd->first_minor = 0;
	disk_device.gd->minors = DISK_MINORS;
	disk_device.gd->fops = &fops;
	disk_device.gd->queue = dev->queue;
	disk_device.gd->private_data = dev;
	sprintf(disk_device.gd->disk_name,"dof");
	
	set_capacity(disk_device.gd, nsectors*(sect_size/KERNEL_SECTOR_SIZE));
	
	add_disk(disk_device.gd);	    
	return 0;
}

static void disk_exit(void)
{
	del_gendisk(disk_device.gd);
	unregister_blkdev(major_num, "dof");
	put_disk(disk_device.gd);	
	blk_cleanup_queue(disk_device.queue);
	vfree(disk_device.data);
	spin_unlock(&disk_device.lock);	
	printk(KERN_ALERT "dof unregistered");
}
	
module_init(disk_init);
module_exit(disk_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AYUSH KAPUR");
MODULE_DESCRIPTION("Block device driver : Disk on RAM");
