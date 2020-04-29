#define GPFCON      (*(volatile unsigned long *)0x56000050)
#define GPFDAT      (*(volatile unsigned long *)0x56000054)

#define GPBCON      (*(volatile unsigned long *)0x56000010)
#define GPBDAT      (*(volatile unsigned long *)0x56000014)

/*
 * LED1,LED2,LED4��ӦGPB5��GPB6��GPB7��GPB8
 */
#define	GPB5_out	(1<<(5*2))
#define	GPB6_out	(1<<(6*2))
#define	GPB7_out	(1<<(7*2))
#define	GPB8_out	(1<<(8*2))

#define	GPB5_msk	(3<<(5*2))
#define	GPB6_msk	(3<<(6*2))
#define	GPB7_msk	(3<<(7*2))
#define	GPB8_msk	(3<<(8*2))

/*
 * K1,K2,K3,K4��ӦGPF1��GPF4��GPF2��GPF0
 */
#define GPF0_in     (0<<(0*2))
#define GPF1_in     (0<<(1*2))
#define GPF2_in     (0<<(2*2))
#define GPF4_in     (0<<(4*2))

#define GPF0_msk    (3<<(0*2))
#define GPF1_msk    (3<<(1*2))
#define GPF2_msk    (3<<(2*2))
#define GPF4_msk    (3<<(4*2))

int main()
{
        unsigned long dwDat;
        // LED1,LED2,LED3,LED4��Ӧ��4��������Ϊ���
        GPBCON &= ~(GPB5_msk | GPB6_msk | GPB7_msk | GPB8_msk);
        GPBCON |= GPB5_out | GPB6_out | GPB7_out | GPB8_out;
        
        // K1,K2,K3,K4��Ӧ��4��������Ϊ����
        GPFCON &= ~(GPF0_msk | GPF1_msk | GPF2_msk | GPF4_msk);
        GPFCON |= GPF0_in | GPF1_in | GPF2_in | GPF4_in;


        while(1){
            //��KnΪ0(��ʾ����)������LEDnΪ0(��ʾ����)
            dwDat = GPFDAT;             // ��ȡGPF�ܽŵ�ƽ״̬
        
            if (dwDat & (1<<1))        // K1û�а���
                GPBDAT |= (1<<5);       // LED1Ϩ��
            else    
                GPBDAT &= ~(1<<5);      // LED1����
                
            if (dwDat & (1<<4))         // K2û�а���
                GPBDAT |= (1<<6);       // LED2Ϩ��
            else    
                GPBDAT &= ~(1<<6);      // LED2����
    
			if (dwDat & (1<<2)) 	   // K3û�а���
				GPBDAT |= (1<<7);		// LED3Ϩ��
			else	
				GPBDAT &= ~(1<<7);		// LED3����
				
			if (dwDat & (1<<0)) 		// K4û�а���
				GPBDAT |= (1<<8);		// LED4Ϩ��
			else	
				GPBDAT &= ~(1<<8);		// LED4����
    }

    return 0;
}
