
/* ����usbģʽ�л�: ����usb_modeswitch
 */
int do_switch(int argc, char **argv)
{
    /* ��ʹ��mdev��Ƕ��ʽϵͳ��, ����һ��USB�豸��, 
     * ��/dev/Ŀ¼�½�����/dev/usbdevM.N�豸 
     * M��ʾ����, N��ʾ�豸��ַ    
     */

    /* ����usbdevM.N�ҳ�����VID,PID */

    /* ����VID,PID��/usr/share/usb_modeswitch�ҵ���Ϊ"VID:PID"���ļ� */

    /* ����"usb_modeswitch -V VID -P PID -f "�ļ�����"" */
    
}

/* 
 * ����/dev/gsmmodem����, ָ��ĳ��/dev/ttyUSBX
 */
int do_link(int argc, char **argv)
{
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

