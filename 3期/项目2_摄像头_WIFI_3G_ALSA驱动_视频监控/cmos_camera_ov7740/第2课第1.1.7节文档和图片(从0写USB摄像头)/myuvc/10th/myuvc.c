#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/videodev2.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>
#include <asm/atomic.h>
#include <asm/unaligned.h>

#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>

static struct video_device *myuvc_vdev;

/* A2 */
static int myuvc_vidioc_querycap(struct file *file, void  *priv,
					struct v4l2_capability *cap)
{
	return 0;
}

/* A3 �о�֧�����ָ�ʽ */
static int myuvc_vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
					struct v4l2_fmtdesc *f)
{
	return 0;
}

/* A4 ���ص�ǰ��ʹ�õĸ�ʽ */
static int myuvc_vidioc_g_fmt_vid_cap(struct file *file, void *priv,
					struct v4l2_format *f)
{
	return (0);
}

/* A5 �������������Ƿ�֧��ĳ�ָ�ʽ */
static int myuvc_vidioc_try_fmt_vid_cap(struct file *file, void *priv,
			struct v4l2_format *f)
{
    return 0;
}

/* A6 */
static int myuvc_vidioc_s_fmt_vid_cap(struct file *file, void *priv,
					struct v4l2_format *f)
{
    return 0;
}

/* A7 APP���ø�ioctl����������������ɸ�����, APP������Щ�����ж�����Ƶ���� */
static int myuvc_vidioc_reqbufs(struct file *file, void *priv,
			  struct v4l2_requestbuffers *p)
{
	return 0;
}

/* A8 ��ѯ����״̬, �����ַ��Ϣ(APP������mmap����ӳ��) */
static int myuvc_vidioc_querybuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
	return 0;
}

/* A10 �ѻ������������, �ײ��Ӳ������������������ݷ���������еĻ��� */
static int myuvc_vidioc_qbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
	return 0;
}

/* A11 �������� */
static int myuvc_vidioc_streamon(struct file *file, void *priv, enum v4l2_buf_type i)
{
	return 0;
}

/* A13 APPͨ��poll/selectȷ�������ݺ�, �ѻ���Ӷ�����ȡ����
 */
static int myuvc_vidioc_dqbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
	return 0;
}

/*
 * A14 ֮ǰ�Ѿ�ͨ��mmapӳ���˻���, APP����ֱ�Ӷ�����
 * A15 �ٴε���myuvc_vidioc_qbuf�ѻ���������
 * A16 poll...
 */

/* A17 ֹͣ */
static int myuvc_vidioc_streamoff(struct file *file, void *priv, enum v4l2_buf_type i)
{
    return 0;
}



static const struct v4l2_ioctl_ops myuvc_ioctl_ops = {
        // ��ʾ����һ������ͷ�豸
        .vidioc_querycap      = myuvc_vidioc_querycap,

        /* �����о١���á����ԡ���������ͷ�����ݵĸ�ʽ */
        .vidioc_enum_fmt_vid_cap  = myuvc_vidioc_enum_fmt_vid_cap,
        .vidioc_g_fmt_vid_cap     = myuvc_vidioc_g_fmt_vid_cap,
        .vidioc_try_fmt_vid_cap   = myuvc_vidioc_try_fmt_vid_cap,
        .vidioc_s_fmt_vid_cap     = myuvc_vidioc_s_fmt_vid_cap,
        
        /* ����������: ����/��ѯ/�������/ȡ������ */
        .vidioc_reqbufs       = myuvc_vidioc_reqbufs,
        .vidioc_querybuf      = myuvc_vidioc_querybuf,
        .vidioc_qbuf          = myuvc_vidioc_qbuf,
        .vidioc_dqbuf         = myuvc_vidioc_dqbuf,
        
        // ����/ֹͣ
        .vidioc_streamon      = myuvc_vidioc_streamon,
        .vidioc_streamoff     = myuvc_vidioc_streamoff,   
};

/* A1 */
static int myuvc_open(struct file *file)
{
	return 0;
}


/* A9 �ѻ���ӳ�䵽APP�Ŀռ�,�Ժ�APP�Ϳ���ֱ�Ӳ�����黺�� */
static int myuvc_mmap(struct file *file, struct vm_area_struct *vma)
{
	return 0;
}

/* A12 APP����POLL/select��ȷ�������Ƿ����(������) */
static unsigned int myuvc_poll(struct file *file, struct poll_table_struct *wait)
{
	return 0;
}

/* A18 �ر� */
static int myuvc_close(struct file *file)
{
    
	return 0;
}

static const struct v4l2_file_operations myuvc_fops = {
	.owner		= THIS_MODULE,
    .open       = myuvc_open,
    .release    = myuvc_close,
    .mmap       = myuvc_mmap,
    .ioctl      = video_ioctl2, /* V4L2 ioctl handler */
    .poll       = myuvc_poll,
};

static void myuvc_release(struct video_device *vdev)
{
}


static int myuvc_probe(struct usb_interface *intf,
		     const struct usb_device_id *id)
{
    static int cnt = 0;
	struct usb_device *dev = interface_to_usbdev(intf);
    struct usb_device_descriptor *descriptor = &dev->descriptor;
    struct usb_host_config *hostconfig;
    struct usb_config_descriptor *config;
	struct usb_interface_assoc_descriptor *assoc_desc;
    struct usb_interface_descriptor	*interface;
    struct usb_endpoint_descriptor  *endpoint;
    int i, j, k, l, m;
	unsigned char *buffer;
	int buflen;
    int desc_len;
    int desc_cnt;

    printk("myuvc_probe : cnt = %d\n", cnt++);

    if (cnt == 2)
    {
        /* 1. ����һ��video_device�ṹ�� */
        myuvc_vdev = video_device_alloc();

        /* 2. ���� */
        /* 2.1 */
        myuvc_vdev->release = myuvc_release;
        
        /* 2.2 */
        myuvc_vdev->fops    = &myuvc_fops;
        
        /* 2.3 */
        myuvc_vdev->ioctl_ops = &myuvc_ioctl_ops;

        /* 3. ע�� */
        video_register_device(myuvc_vdev, VFL_TYPE_GRABBER, -1);
    }
    
    
    return 0;
}

static void myuvc_disconnect(struct usb_interface *intf)
{
    static int cnt = 0;
    printk("myuvc_disconnect : cnt = %d\n", cnt++);

    if (cnt == 2)
    {
        video_unregister_device(myuvc_vdev);
        video_device_release(myuvc_vdev);
    }
    
}

static struct usb_device_id myuvc_ids[] = {
	/* Generic USB Video Class */
	{ USB_INTERFACE_INFO(USB_CLASS_VIDEO, 1, 0) },  /* VideoControl Interface */
    { USB_INTERFACE_INFO(USB_CLASS_VIDEO, 2, 0) },  /* VideoStreaming Interface */
	{}
};

/* 1. ����usb_driver */
/* 2. ���� */
static struct usb_driver myuvc_driver = {
    .name       = "myuvc",
    .probe      = myuvc_probe,
    .disconnect = myuvc_disconnect,
    .id_table   = myuvc_ids,
};

static int myuvc_init(void)
{
    /* 3. ע�� */
    usb_register(&myuvc_driver);
    return 0;
}

static void myuvc_exit(void)
{
    usb_deregister(&myuvc_driver);
}

module_init(myuvc_init);
module_exit(myuvc_exit);

MODULE_LICENSE("GPL");



