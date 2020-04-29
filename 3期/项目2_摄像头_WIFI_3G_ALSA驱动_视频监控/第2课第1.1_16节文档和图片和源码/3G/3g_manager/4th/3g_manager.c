#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include <libusb.h>
       
#define USB_MODESWITCH_ETC_DIR "/usr/share/usb_modeswitch"

/* ����usbģʽ�л�: ����usb_modeswitch
 */
int do_switch(int argc, char **argv)
{
	libusb_device **devs;
	int r;
	ssize_t cnt;
	libusb_device *dev;
	int i = 0;
	struct libusb_device_descriptor desc;

    int bus;
    int address;

    int vid;
    int pid;
    char tmpBuf[1024];
    char *cmdBuf;

    DIR *dir;
    struct dirent *entry;

    int fd;
    struct stat stat;
    
    /* ��ʹ��mdev��Ƕ��ʽϵͳ��, ����һ��USB�豸��, 
     * ��/dev/Ŀ¼�½�����/dev/usbdevM.N�豸 
     * M��ʾ����, N��ʾ�豸��ַ    
     */
    sscanf(argv[2]+6, "%d.%d", &bus, &address);
    
    /* ����usbdevM.N�ҳ�����VID,PID */
	r = libusb_init(NULL);
	if (r < 0)
		return r;

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0)
		return (int) cnt;

	while ((dev = devs[i++]) != NULL) {
        if ((bus == libusb_get_bus_number(dev)) && (address == libusb_get_device_address(dev)))
        {
		    r = libusb_get_device_descriptor(dev, &desc);
    		if (r < 0) {
    			printf("failed to get device descriptor");
    			return -1;
    		}
			vid = desc.idVendor;
            pid = desc.idProduct;
            break;
        }
	}
    if (!dev)
    {
        printf("there is not this usbdev!\n");
        return -1;
    }

    /* ����VID,PID��/usr/share/usb_modeswitch�ҵ����к���"VID:PID"���ļ� */
    sprintf(tmpBuf, "%04x:%04x", vid, pid);
    dir = opendir(USB_MODESWITCH_ETC_DIR);
    if (!dir)
    {
        printf("can not open %s\n", USB_MODESWITCH_ETC_DIR);
        return -1;
    }
    while((entry = readdir(dir)))
    {
        if (strncasecmp(entry->d_name, tmpBuf, 9) == 0)
        {
            break;
        }
    }
    if (!entry)
    {
        printf("there is cfg file for %04x:%04x\n", vid, pid);
        return -1;
    }
    sprintf(tmpBuf, "%s/%s", USB_MODESWITCH_ETC_DIR, entry->d_name);

    /* �������ļ��򿪶������� */
    fd = open(tmpBuf, O_RDONLY);
    if (fd < 0)
    {
        printf("can not open %s\n", tmpBuf);
        return -1;
    }
    r = fstat(fd, &stat);
    if (r == -1)
    {
        printf("can not get stat for %s\n", tmpBuf);
        return -1;
    }
    
    /* ����"usb_modeswitch -v VID -p PID -f "�ļ�����"" */
    cmdBuf = malloc(stat.st_size + 1024);
    sprintf(cmdBuf, "usb_modeswitch -v %x -p %x -f \"", vid, pid);
    cnt = strlen(cmdBuf);
    read(fd, cmdBuf+cnt, stat.st_size);
    cnt += stat.st_size;
    cmdBuf[cnt++] = '\"';
    cmdBuf[cnt++] = '\0';

    printf("Cmd: %s\n", cmdBuf);
    r = system(cmdBuf);
    free(cmdBuf);
    close(fd);
    return r;
}

/*
 * �ж�/dev/ttyUSB'index'�Ƿ����ж����͵�endpoint
 */
int hasInterruptEndpoint(int index)
{
    
}

/* 
 * ����/dev/gsmmodem����, ָ��ĳ��/dev/ttyUSBX
 * 3g_manager link ttyUSB0,1,2
 */
int do_link(int argc, char **argv)
{
    char cmdBuf[1024];
    int index;
    int i;
    
    /*
     * ����ж��/dev/ttyUSBX���ж����Ͷ˵�, 
     * ��/dev/gsmmodemָ����С��/dev/ttyUSBX
     */
    sscanf(argv[2]+6, "%d", &index);
    for (i = 0; i <= index; i++)
    {
        if (hasInterruptEndpoint(i))
        {
            sprintf(cmdBuf, "ln -s /dev/ttyUSB%d /dev/gsmmodem", i);
            system("rm -f /dev/gsmmodem");
            system(cmdBuf);
            return 0;
        }
    }
    return -1;
}

/* ����usbģʽ�л�: ����usb_modeswitch
 * ����/dev/gsmmodem����, ָ��ĳ��/dev/ttyUSBX
 */

/* �÷�:
 * 3g_manager switch usbdev1.17
 * 3g_manager link   ttyUSB1
 */
int main(int argc, char **argv)
{
    if ((argc != 3) || \
        ((strcmp(argv[1], "switch") && strcmp(argv[1], "link"))))
    {
        printf("Usage:\n");
        printf("%s switch <usbdevM.N> : switch 3G Modem\n", argv[0]);
        printf("%s link   <ttyUSBX>   : Create Link to /dev/ttyUSBX if it has interrupt endpoint\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "switch") == 0)
        return do_switch(argc, argv);
    else if (strcmp(argv[1], "link") == 0)
        return do_link(argc, argv);
    return -1;
}

