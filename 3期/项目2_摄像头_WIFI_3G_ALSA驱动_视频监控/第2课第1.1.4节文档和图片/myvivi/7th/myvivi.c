
/* ����vivi.c */
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/random.h>
#include <linux/version.h>
#include <linux/mutex.h>
#include <linux/videodev2.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/highmem.h>
#include <linux/freezer.h>
#include <media/videobuf-vmalloc.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>


static struct v4l2_format myvivi_format;

/* ���в���1: ���� */
static struct videobuf_queue myvivi_vb_vidqueue;
static spinlock_t myvivi_queue_slock;

static struct list_head myvivi_vb_local_queue;


static struct timer_list myvivi_timer;

#include "fillbuf.c"

/* �ο�documentations/video4linux/v4l2-framework.txt:
 *     drivers\media\video\videobuf-core.c 
 ops->buf_setup   - calculates the size of the video buffers and avoid they
            to waste more than some maximum limit of RAM;
 ops->buf_prepare - fills the video buffer structs and calls
            videobuf_iolock() to alloc and prepare mmaped memory;
 ops->buf_queue   - advices the driver that another buffer were
            requested (by read() or by QBUF);
 ops->buf_release - frees any buffer that were allocated.
 
 *
 */


/* ------------------------------------------------------------------
	Videobuf operations
   ------------------------------------------------------------------*/
/* APP����ioctl VIDIOC_REQBUFSʱ�ᵼ�´˺���������,
 * �����µ���count��size
 */
static int myvivi_buffer_setup(struct videobuf_queue *vq, unsigned int *count, unsigned int *size)
{

	*size = myvivi_format.fmt.pix.sizeimage;

	if (0 == *count)
		*count = 32;

	return 0;
}

/* APP����ioctlVIDIOC_QBUFʱ���´˺���������,
 * �������video_buffer�ṹ�岢����videobuf_iolock�������ڴ�
 * 
 */
static int myvivi_buffer_prepare(struct videobuf_queue *vq, struct videobuf_buffer *vb,
						enum v4l2_field field)
{
    /* 0. ����videobuf */
	vb->size = myvivi_format.fmt.pix.sizeimage;
    vb->bytesperline = myvivi_format.fmt.pix.bytesperline;
	vb->width  = myvivi_format.fmt.pix.width;
	vb->height = myvivi_format.fmt.pix.height;
	vb->field  = field;
    
    
    /* 1. ��Щ׼������ */
    myvivi_precalculate_bars(0);

#if 0
    /* 2. ����videobuf_iolockΪ����ΪV4L2_MEMORY_USERPTR��videobuf�����ڴ� */
	if (VIDEOBUF_NEEDS_INIT == buf->vb.state) {
		rc = videobuf_iolock(vq, &buf->vb, NULL);
		if (rc < 0)
			goto fail;
	}
#endif
    /* 3. ����״̬ */
	vb->state = VIDEOBUF_PREPARED;

	return 0;
}


/* APP����ioctl VIDIOC_QBUFʱ:
 * 1. �ȵ���buf_prepare����һЩ׼������
 * 2. ��buf����stream����
 * 3. ����buf_queue(��֪ͨ����¼����)
 */
static void myvivi_buffer_queue(struct videobuf_queue *vq, struct videobuf_buffer *vb)
{
	vb->state = VIDEOBUF_QUEUED;

    /* ��videobuf���뱾��һ������β��
     * ��ʱ���������Ϳ��Դӱ��ض���ȡ��videobuf
     */
    list_add_tail(&vb->queue, &myvivi_vb_local_queue);
}

/* APP����ʹ�ö���ʱ, �������ͷ��ڴ� */
static void myvivi_buffer_release(struct videobuf_queue *vq,
			   struct videobuf_buffer *vb)
{
	videobuf_vmalloc_free(vb);
	vb->state = VIDEOBUF_NEEDS_INIT;
}

static struct videobuf_queue_ops myvivi_video_qops = {
	.buf_setup      = myvivi_buffer_setup, /* �����С�����˷� */
	.buf_prepare    = myvivi_buffer_prepare,
	.buf_queue      = myvivi_buffer_queue,
	.buf_release    = myvivi_buffer_release,
};

/* ------------------------------------------------------------------
	File operations for the device
   ------------------------------------------------------------------*/

static int myvivi_open(struct file *file)
{
    /* ���в���2: ��ʼ�� */
	videobuf_queue_vmalloc_init(&myvivi_vb_vidqueue, &myvivi_video_qops,
			NULL, &myvivi_queue_slock, V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED,
			sizeof(struct videobuf_buffer), NULL); /* ������2��������buffer��ͷ����С */

    myvivi_timer.expires = jiffies + 1;
    add_timer(&myvivi_timer);

	return 0;
}


static int myvivi_close(struct file *file)
{
    del_timer(&myvivi_timer);
	videobuf_stop(&myvivi_vb_vidqueue);
	videobuf_mmap_free(&myvivi_vb_vidqueue);
    
	return 0;
}

static int myvivi_mmap(struct file *file, struct vm_area_struct *vma)
{
	return videobuf_mmap_mapper(&myvivi_vb_vidqueue, vma);
}

static unsigned int myvivi_poll(struct file *file, struct poll_table_struct *wait)
{
	return videobuf_poll_stream(file, &myvivi_vb_vidqueue, wait);
}

static int myvivi_vidioc_querycap(struct file *file, void  *priv,
					struct v4l2_capability *cap)
{
	strcpy(cap->driver, "myvivi");
	strcpy(cap->card, "myvivi");
	cap->version = 0x0001;
	cap->capabilities =	V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
	return 0;
}

/* �о�֧�����ָ�ʽ */
static int myvivi_vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
					struct v4l2_fmtdesc *f)
{
	if (f->index >= 1)
		return -EINVAL;

	strcpy(f->description, "4:2:2, packed, YUYV");
	f->pixelformat = V4L2_PIX_FMT_YUYV;
	return 0;
}

/* ���ص�ǰ��ʹ�õĸ�ʽ */
static int myvivi_vidioc_g_fmt_vid_cap(struct file *file, void *priv,
					struct v4l2_format *f)
{
    memcpy(f, &myvivi_format, sizeof(myvivi_format));
	return (0);
}

/* �������������Ƿ�֧��ĳ�ָ�ʽ */
static int myvivi_vidioc_try_fmt_vid_cap(struct file *file, void *priv,
			struct v4l2_format *f)
{
	unsigned int maxw, maxh;
    enum v4l2_field field;

    if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV)
        return -EINVAL;

	field = f->fmt.pix.field;

	if (field == V4L2_FIELD_ANY) {
		field = V4L2_FIELD_INTERLACED;
	} else if (V4L2_FIELD_INTERLACED != field) {
		return -EINVAL;
	}

	maxw  = 1024;
	maxh  = 768;

    /* ����format��width, height, 
     * ����bytesperline, sizeimage
     */
	v4l_bound_align_image(&f->fmt.pix.width, 48, maxw, 2,
			      &f->fmt.pix.height, 32, maxh, 0, 0);
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * 16) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;

	return 0;
}

static int myvivi_vidioc_s_fmt_vid_cap(struct file *file, void *priv,
					struct v4l2_format *f)
{
	int ret = myvivi_vidioc_try_fmt_vid_cap(file, NULL, f);
	if (ret < 0)
		return ret;

    memcpy(&myvivi_format, f, sizeof(myvivi_format));
    
	return ret;
}

static int myvivi_vidioc_reqbufs(struct file *file, void *priv,
			  struct v4l2_requestbuffers *p)
{
	return (videobuf_reqbufs(&myvivi_vb_vidqueue, p));
}

static int myvivi_vidioc_querybuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
	return (videobuf_querybuf(&myvivi_vb_vidqueue, p));
}

static int myvivi_vidioc_qbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
	return (videobuf_qbuf(&myvivi_vb_vidqueue, p));
}

static int myvivi_vidioc_dqbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
	return (videobuf_dqbuf(&myvivi_vb_vidqueue, p,
				file->f_flags & O_NONBLOCK));
}

static int myvivi_vidioc_streamon(struct file *file, void *priv, enum v4l2_buf_type i)
{
	return videobuf_streamon(&myvivi_vb_vidqueue);
}

static int myvivi_vidioc_streamoff(struct file *file, void *priv, enum v4l2_buf_type i)
{
	videobuf_streamoff(&myvivi_vb_vidqueue);
    return 0;
}


static const struct v4l2_ioctl_ops myvivi_ioctl_ops = {
        // ��ʾ����һ������ͷ�豸
        .vidioc_querycap      = myvivi_vidioc_querycap,

        /* �����о١���á����ԡ���������ͷ�����ݵĸ�ʽ */
        .vidioc_enum_fmt_vid_cap  = myvivi_vidioc_enum_fmt_vid_cap,
        .vidioc_g_fmt_vid_cap     = myvivi_vidioc_g_fmt_vid_cap,
        .vidioc_try_fmt_vid_cap   = myvivi_vidioc_try_fmt_vid_cap,
        .vidioc_s_fmt_vid_cap     = myvivi_vidioc_s_fmt_vid_cap,
        
        /* ����������: ����/��ѯ/�������/ȡ������ */
        .vidioc_reqbufs       = myvivi_vidioc_reqbufs,
        .vidioc_querybuf      = myvivi_vidioc_querybuf,
        .vidioc_qbuf          = myvivi_vidioc_qbuf,
        .vidioc_dqbuf         = myvivi_vidioc_dqbuf,
        
        // ����/ֹͣ
        .vidioc_streamon      = myvivi_vidioc_streamon,
        .vidioc_streamoff     = myvivi_vidioc_streamoff,   
};


static const struct v4l2_file_operations myvivi_fops = {
	.owner		= THIS_MODULE,
    .open       = myvivi_open,
    .release    = myvivi_close,
    .mmap       = myvivi_mmap,
    .ioctl      = video_ioctl2, /* V4L2 ioctl handler */
    .poll       = myvivi_poll,
};


static struct video_device *myvivi_device;

static void myvivi_release(struct video_device *vdev)
{
}

static void myvivi_timer_function(unsigned long data)
{
    struct videobuf_buffer *vb;
	void *vbuf;
	struct timeval ts;
    
    /* 1. ��������: �Ӷ���ͷ��ȡ����1��videobuf, �������
     */

    /* 1.1 �ӱ��ض���ȡ����1��videobuf */
    if (list_empty(&myvivi_vb_local_queue)) {
        goto out;
    }
    
    vb = list_entry(myvivi_vb_local_queue.next,
             struct videobuf_buffer, queue);
    
    /* Nobody is waiting on this buffer, return */
    if (!waitqueue_active(&vb->done))
        goto out;
    

    /* 1.2 ������� */
    vbuf = videobuf_to_vmalloc(vb);
    //memset(vbuf, 0xff, vb->size);
    myvivi_fillbuff(vb);
    
    vb->field_count++;
    do_gettimeofday(&ts);
    vb->ts = ts;
    vb->state = VIDEOBUF_DONE;

    /* 1.3 ��videobuf�ӱ��ض�����ɾ�� */
    list_del(&vb->queue);

    /* 2. ���ѽ���: ����videobuf->done�ϵĽ��� */
    wake_up(&vb->done);
    
out:
    /* 3. �޸�timer�ĳ�ʱʱ�� : 30fps, 1������30֡����
     *    ÿ1/30 �����һ֡����
     */
    mod_timer(&myvivi_timer, jiffies + HZ/30);
}

static int myvivi_init(void)
{
    int error;
    
    /* 1. ����һ��video_device�ṹ�� */
    myvivi_device = video_device_alloc();

    /* 2. ���� */

    /* 2.1 */
    myvivi_device->release = myvivi_release;

    /* 2.2 */
    myvivi_device->fops    = &myvivi_fops;

    /* 2.3 */
    myvivi_device->ioctl_ops = &myvivi_ioctl_ops;

    /* 2.4 ���в���
     *  a. ����/��ʼ��һ������(���õ�һ��spinlock)
     */
    spin_lock_init(&myvivi_queue_slock);

    /* 3. ע�� */
    error = video_register_device(myvivi_device, VFL_TYPE_GRABBER, -1);

    /* �ö�ʱ���������ݲ����ѽ��� */
	init_timer(&myvivi_timer);
    myvivi_timer.function  = myvivi_timer_function;

    INIT_LIST_HEAD(&myvivi_vb_local_queue);
    
    return error;
}

static void myvivi_exit(void)
{
    video_unregister_device(myvivi_device);
    video_device_release(myvivi_device);
}

module_init(myvivi_init);
module_exit(myvivi_exit);
MODULE_LICENSE("GPL");

