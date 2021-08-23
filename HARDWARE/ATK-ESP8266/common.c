#include "common.h"
#include "usart.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 公用驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//用户配置区

//连接端口号:8086,可自行修改为其他端口.
const u8* portnum="8080";		 

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const u8* wifista_ssid="俊霸天";			//路由器SSID号
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifista_password="**ljy2000"; 	//连接密码
const u8* ip_address="192.168.43.204"; 	//远端ip地址




//WIFI AP模式,模块对外的无线参数,可自行修改.
const u8* wifiap_ssid="ATK-ESP8266";			//对外SSID号
const u8* wifiap_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifiap_password="12345678"; 		//连接密码 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//4个网络模式
const u8 *ATK_ESP8266_CWMODE_TBL[3]={"STA模式 ","AP模式 ","AP&STA模式 "};	//ATK-ESP8266,3种网络模式,默认为路由器(ROUTER)模式 
//4种工作模式
const u8 *ATK_ESP8266_WORKMODE_TBL[3]={"TCP服务器","TCP客户端"," UDP 模式"};	//ATK-ESP8266,4种工作模式
//5种加密方式
const u8 *ATK_ESP8266_ECN_TBL[5]={"OPEN","WEP","WPA_PSK","WPA2_PSK","WPA_WAP2_PSK"};
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		printf("%s",USART3_RX_BUF);	//发送到串口
		if(mode)USART3_RX_STA=0;
	} 
}
//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* atk_8266_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))
				{
					printf("ack:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s",data);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))break;//得到有效数据 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}
//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 atk_8266_quit_trans(void)
{
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(1000);					//等待500ms
	return atk_8266_send_cmd("AT","OK",20);//退出透传判断.
	
}












