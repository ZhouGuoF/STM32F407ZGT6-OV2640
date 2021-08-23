#include "common.h"
#include "stdlib.h"
#include "wifista.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-ESP8266 WIFI STA����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������


void atk_8266_wifista_test(void)
{
	if(atk_8266_send_cmd("AT+CWMODE=1","OK",50))
	{
		printf("����STAģʽʧ�ܣ�׼������\r\n");
	}
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 
	delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
	delay_ms(1000);
	delay_ms(1000);
	while(atk_8266_send_cmd("AT+CWJAP=\"HUAWEI\",\"123456789\"","WIFI GOT IP",600)); //����WIFIָ��
	atk_8266_send_cmd("AT+CIPMUX=0","OK",200);   //0�������ӣ�1��������
	delay_ms(200);
	
	while(atk_8266_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.43.84\",8086","OK",1000));

	atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��
	delay_ms(200);
	printf("%s\r\n","connect success");
	atk_8266_send_cmd("AT+CIPSEND","OK",200); 
	delay_ms(200);	
	}



























