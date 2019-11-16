#ifndef __USART_H
#define __USART_H
#include "sys.h" 
#include "stdio.h"	
#include "delay.h"
#include "stdarg.h"	 	  
#include "common.h"

#define USART_REC_LEN  			600  	//�����������ֽ��� 200
#define USART_SEND_LEN		  600		//����ͻ����ֽ���
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1���� 

extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 

extern u8  USART_TX_BUF[USART_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�

extern u16 USART_RX_STA;         		//����״̬���	

void uart_init(u32 bound);

void u1_printf(char* fmt,...);

#endif


