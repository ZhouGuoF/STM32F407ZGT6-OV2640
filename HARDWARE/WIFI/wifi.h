/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ����Wifi���ܵ�ͷ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __WIFI_H
#define __WIFI_H

#include "usart2.h"	    //������Ҫ��ͷ�ļ�

#define RESET_IO(x)    GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)x)  //PA4����WiFi�ĸ�λ

#define WiFi_printf       u3_printf           //����2���� WiFi
#define WiFi_RxCounter    Usart3_RxCounter    //����2���� WiFi
#define WiFi_RX_BUF       Usart3_RxBuff       //����2���� WiFi
#define WiFi_RXBUFF_SIZE  USART3_RXBUFF_SIZE  //����2���� WiFi

#define SSID   "ljyyy"                     //·����SSID����
#define PASS   "ljy20000403"                 //·��������

void WiFi_ResetIO_Init(void);
char WiFi_SendCmd(char *cmd, int timeout);
char WiFi_Reset(int timeout);
char WiFi_JoinAP(int timeout);
char WiFi_Connect_Server(int timeout);
char WiFi_Smartconfig(int timeout);
char WiFi_WaitAP(int timeout);
char WiFi_Connect_IoTServer(void);


#endif


