#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
//#include "usmart.h"  
#include "usart2.h"  
#include "timer.h" 
#include "ov2640.h" 
#include "dcmi.h" 
#include "usart3.h" 
#include "common.h" 
#include "moter.h" 
//ALIENTEK STM32F407������
//����ͷ ʵ�� -�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK

//u8 ov2640_mode=1;						//����ģʽ:0,RGB565ģʽ;1,JPEGģʽ

#define jpeg_buf_size 31*1024  			//����JPEG���ݻ���jpeg_buf�Ĵ�С(*4�ֽ�)
__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG���ݻ���buf
volatile u32 jpeg_data_len=0; 			//buf�е�JPEG��Ч���ݳ��� 
volatile u8 jpeg_data_ok=0;				//JPEG���ݲɼ���ɱ�־ 
										//0,����û�вɼ���;
										//1,���ݲɼ�����,���ǻ�û����;
										//2,�����Ѿ����������,���Կ�ʼ��һ֡����
//JPEG�ߴ�֧���б�
const u16 jpeg_img_size_tbl[][2]=
{
	176,144,	//QCIF
	160,120,	//QQVGA
	352,288,	//CIF
	320,240,	//QVGA
	640,480,	//VGA
	800,600,	//SVGA
	1024,768,	//XGA
	1280,1024,	//SXGA
	1600,1200,	//UXGA
}; 
const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7����Ч 
const u8*JPEG_SIZE_TBL[9]={"QCIF","QQVGA","CIF","QVGA","VGA","SVGA","XGA","SXGA","UXGA"};	//JPEGͼƬ 9�ֳߴ� 

//����JPEG����
//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.
void jpeg_data_process(void)
{
		if(jpeg_data_ok==0)	//jpeg���ݻ�δ�ɼ���?
		{	
			DMA_Cmd(DMA2_Stream1, DISABLE);//ֹͣ��ǰ���� 
			while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//�ȴ�DMA2_Stream1������  
			jpeg_data_len=jpeg_buf_size-DMA_GetCurrDataCounter(DMA2_Stream1);//�õ��˴����ݴ���ĳ���
				
			jpeg_data_ok=1; 				//���JPEG���ݲɼ��갴��,�ȴ�������������
		}
		if(jpeg_data_ok==2)	//��һ�ε�jpeg�����Ѿ���������
		{
			DMA2_Stream1->NDTR=jpeg_buf_size;	
			DMA_SetCurrDataCounter(DMA2_Stream1,jpeg_buf_size);//���䳤��Ϊjpeg_buf_size*4�ֽ�
			DMA_Cmd(DMA2_Stream1, ENABLE);			//���´���
			jpeg_data_ok=0;						//�������δ�ɼ�
		}
} 
//JPEG����
//JPEG����,ͨ������1���͸�����.
void jpeg_test(void)
{
	u8 *p;
	u8 size=3;		//Ĭ����QVGA 320*240�ߴ�
	//u8 s[4];
//	u32 c;
	 u32 i,jpgstart,jpglen;
	u8 headok=0;
	usart2_init(42,115200);
 	OV2640_JPEG_Mode();		//JPEGģʽ
	My_DCMI_Init();			//DCMI����
	DCMI_DMA_Init((u32)&jpeg_buf,jpeg_buf_size,DMA_MemoryDataSize_Word,DMA_MemoryInc_Enable);//DCMI DMA����   
	OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ�    �þ����320*240
	DCMI_Start(); 		//��������
	delay_ms(200);
	while(1)
	{
		if(jpeg_data_ok==1)	//�Ѿ��ɼ���һ֡ͼ����
		{  
			p=(u8*)jpeg_buf;
		//printf("Sending JPEG data\r\n");//��ʾ���ڴ�������Sending JPEG data
/*		
			for(i=0;i<jpeg_data_len*4;i++)		//dma����1�ε���4�ֽ�,���Գ���4.
			{
				while((USART3->SR&0X40)==RESET);	//ѭ������,ֱ���������  		
				USART3->DR=p[i];  
				
			} 
			
			*/
			
			
						jpglen=0;	//����jpg�ļ���СΪ0
            headok=0;	//���jpgͷ���
            for(i=0;i<jpeg_data_len*4;i++)//����0XFF,0XD8��0XFF,0XD9,��ȡjpg�ļ���С
            {
                if((p[i]==0XFF)&&(p[i+1]==0XD8))//�ҵ�FF D8
                {
                    jpgstart=i;
                    headok=1;	//����ҵ�jpgͷ(FF D8)
                }
                if((p[i]==0XFF)&&(p[i+1]==0XD9)&&headok)//�ҵ�ͷ�Ժ�,����FF D9
                {
                    jpglen=i-jpgstart+2;
                    break;
                }
            }
						printf("jpglen:%d",jpglen);
           // if(jpglen)	//������jpeg����
          //  {
            p+=jpgstart;			//ƫ�Ƶ�0XFF,0XD8��
						
						/*
						s[0]=(u8)(((jpglen)&0xff000000)>>24);
				    s[1]=(u8)(((jpglen)&0xff0000)>>16);
				    s[2]=(u8)(((jpglen)&0xff00)>>8);
				    s[3]=(u8)((jpglen)&0xff);
						for(c=0;c<4;c++)      //���ͱ��ĳ���
			     {
				   USART3->DR=s[c];
				    while((USART3->SR&0X40)==0);
			      }
						*/
						
                for(i=0;i<jpglen;i++)	//��������jpg�ļ�
                {
                    while((USART3->SR&0X40)==0);	//ѭ������,ֱ���������
                    USART3->DR=p[i];
                    //key=KEY_Scan(0);
                    //if(key)break;
                }
           // }
			
			jpeg_data_ok=2;	//���jpeg���ݴ�������,������DMAȥ�ɼ���һ֡��.
		}		
	}    
} 

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usart2_init(42,115200);		//��ʼ������2������Ϊ115200
	usart3_init(115200);  //��ʼ������3������Ϊ115200
	LED_Init();					//��ʼ��LED 
	TIM4_Int_Init(10000-1,8400-1);//10Khz����,1�����ж�һ��
	//MOTOR_Init();
	TIM3_PWM_Init();
 	//usmart_dev.init(84);		//��ʼ��USMART 
	
	atk_8266_wifista_test();
	while(OV2640_Init())//��ʼ��OV2640
	{
		printf("OV2640 ����!\r\n");
		delay_ms(200);
	}
}
