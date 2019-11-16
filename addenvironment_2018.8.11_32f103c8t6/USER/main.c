#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "car.h"
#include "camera.h"
#include "uart2.h"
#include "led.h"
#include "common.h" 
#include "malloc.h"

//72Mhz

int main(void)
{	
	  int led=0,ledflag=0;
		LED_Init();
		delay_init();	    	 //延时函数初始化	  
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
		uart_init(115200);	 				//	 PA9 PA10 UART1串口初始化为115200
		uart2init(9600);					//	 PA2 PA3  UART2 温度，湿度，气压，光强
	 	Camera_Init(19200);   			//	 PB11 USART3  WiFi摄像头串口波特率19200
	
		TIM3_PWM_Init(2000-1,720-1);//   PB1	TIM3CH4 50Hz的电调，1ms-2ms脉宽         总共20ms
		TIM4_PWM_Init(2000-1,720-1);//	 PB9	TIM4CH4 50Hz的舵机，0.5ms-2.5ms的脉宽   总共20ms
		
		atk_8266_test();		//进入ATK_ESP8266测试

		while(1)
		{				  	 
				if(ledflag==0)
				{
						TIM_SetCompare3(TIM3,led++);//中点值
						if(led==1600)ledflag=1;	
				}
				if(ledflag==1)
				{			
						TIM_SetCompare3(TIM3,led--);
						if(led==0)ledflag=0;	
				}					   
		} 
}

