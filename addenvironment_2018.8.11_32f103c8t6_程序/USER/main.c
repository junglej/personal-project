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
		delay_init();	    	 //��ʱ������ʼ��	  
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
		uart_init(115200);	 				//	 PA9 PA10 UART1���ڳ�ʼ��Ϊ115200
		uart2init(9600);					//	 PA2 PA3  UART2 �¶ȣ�ʪ�ȣ���ѹ����ǿ
	 	Camera_Init(19200);   			//	 PB11 USART3  WiFi����ͷ���ڲ�����19200
	
		TIM3_PWM_Init(2000-1,720-1);//   PB1	TIM3CH4 50Hz�ĵ����1ms-2ms����         �ܹ�20ms
		TIM4_PWM_Init(2000-1,720-1);//	 PB9	TIM4CH4 50Hz�Ķ����0.5ms-2.5ms������   �ܹ�20ms
		
		atk_8266_test();		//����ATK_ESP8266����

		while(1)
		{				  	 
				if(ledflag==0)
				{
						TIM_SetCompare3(TIM3,led++);//�е�ֵ
						if(led==1600)ledflag=1;	
				}
				if(ledflag==1)
				{			
						TIM_SetCompare3(TIM3,led--);
						if(led==0)ledflag=0;	
				}					   
		} 
}

