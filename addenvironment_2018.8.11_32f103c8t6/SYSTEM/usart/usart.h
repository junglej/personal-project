#ifndef __USART_H
#define __USART_H
#include "sys.h" 
#include "stdio.h"	
#include "delay.h"
#include "stdarg.h"	 	  
#include "common.h"

#define USART_REC_LEN  			600  	//定义最大接收字节数 200
#define USART_SEND_LEN		  600		//最大发送缓存字节数
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收 

extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 

extern u8  USART_TX_BUF[USART_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节

extern u16 USART_RX_STA;         		//接收状态标记	

void uart_init(u32 bound);

void u1_printf(char* fmt,...);

#endif


