#ifndef __COMMON_H__
#define __COMMON_H__	 
#include "sys.h"
#include "usart.h"		
#include "delay.h"	 	 
#include "malloc.h"
#include "string.h"    

void atk_8266_test(void);

void TIM2_Int_Init(u16 arr,u16 psc);//通用定时器2中断初始化

u8* atk_8266_check_cmd(u8 *str);

u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);

u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime);


#endif





