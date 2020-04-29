
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
    /* 1. ��Щ׼������ */

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


/* APP����ioctlVIDIOC_QBUFʱ:
 * 1. �ȵ���buf_prepare����һЩ׼������
 * 2. ��buf�������
 * 3. ����buf_queue(��֪ͨ����)
 */
static void myvivi_buffer_queue(struct videobuf_queue *vq, struct videobuf_buffer *vb)
{
	vb->state = VIDEOBUF_QUEUED;
	//list_add_tail(&buf->vb.queue, &vidq->active);
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

	return 0;
}


static int myvivi_close(struct file *file)
{
	videobuf_stop(&myvivi_vb_vidqueue);
	videobuf_mmap_free(&myvivi_vb_vidqueue);
    
	return 0;
}

static int myvivi_mmap(struct file *file, struct vm_area_struct *vma)
{
	return videobuf_mmap_mapper(&myvivi_vb_vidqueue, vma);
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

#if 0    
        
        // ����/ֹͣ
        .vidioc_streamon      = myvivi_vidioc_streamon,
        .vidioc_streamoff     = myvivi_vidioc_streamoff,   
#endif
};


static const struct v4l2_file_operations myvivi_fops = {
	.owner		= THIS_MODULE,
    .open       = myvivi_open,
    .release    = myvivi_close,
    .mmap       = myvivi_mmap,
    .ioctl      = video_ioctl2, /* V4L2 ioctl handler */
};


static struct video_device *myvivi_device;

static void myvivi_release(struct video_device *vdev)
{
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

