#include "uart2.h"
#include "usart.h"
#include "led.h"
#include "common.h" 
#include "malloc.h"
Bme bme={0,0,0,0,0};
u8 *p2;

void uart2init(uint32_t bound)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		 
		//USART1_RX	  GPIOA.10��ʼ��
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
		GPIO_Init(GPIOA, &GPIO_InitStructure); 

		//Usart1 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		
		 //USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
		USART_Init(USART2, &USART_InitStructure); //��ʼ������1
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
		USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1 
		p2=mymalloc(SRAMIN,64);							//����32�ֽ��ڴ�
}
//////////////
void USART2_IRQHandler(void)
{
	
	static uint8_t i=0,rebuf[20]={0};
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//�жϽ��ձ�־
	{
		rebuf[i++]=USART_ReceiveData(USART2);//��ȡ�������ݣ�ͬʱ����ձ�־
		if (rebuf[0]!=0x5A)i=0;	
		if ((i==2)&&(rebuf[1]!=0x5A))i=0;
		if(i>2)//i����4ʱ���Ѿ����յ��������ֽ�rebuf[3]
		{				
			if(i!=(rebuf[3]+4))return;	//�ж��Ƿ����һ֡�������
			//for(j=0;j<(rebuf[3]);j++)sum+=rebuf[j+4];
			//	USART_SendData(USART1,rebuf[8]);//�򴮿�1��������
				//USART_SendData(USART1,sum);//�򴮿�1��������
			switch(rebuf[2])//������Ϻ���
			{
				case 0x15:
							bme.Lux=((rebuf[4]<<24)|(rebuf[5]<<16)|(rebuf[6]<<8)|rebuf[7])/100;
					
					break;
				case 0x45:
							bme.Temp=((rebuf[4]<<8)|rebuf[5])/100-6;
							bme.P=(rebuf[6]<<24|rebuf[7]<<16|rebuf[8]<<8|rebuf[9])/100+430;
							bme.Hum=(rebuf[10]<<8|rebuf[11])/100;
							bme.Alt=(rebuf[12]<<8|rebuf[13]-24);
							sprintf((char*)p2,"%03d%03d%03d%04d%06f%04d",bme.Lux,bme.Temp,bme.Hum,bme.Alt,bme.P,2000);//��������
							atk_8266_send_cmd("AT+CIPSEND=0,24","OK",24);  //����ָ�����ȵ�����
							delay_ms(100);
							atk_8266_send_data(p2,"OK",100);  //����ָ�����ȵ�����
				break;//ԭʼ���ݽ��գ���ģ��0x45�ķ�ʽ
				case 0x35:break;
			}	
				
//			printf("  %d",bme.Lux);
//			printf("  %d",bme.Temp);
//			printf("  %d",bme.P);
//			printf("  %d",bme.Hum);
//  		printf("  %d",bme.Alt);
//			printf("\r\n\r\n");
			
//			USART_SendData(USART1,bme.Lux);//�򴮿�1��������
//			USART_SendData(USART1,bme.Temp);//�򴮿�1��������
//			USART_SendData(USART1,bme.P);//�򴮿�1��������
//			USART_SendData(USART1,bme.Hum);//�򴮿�1��������
//			USART_SendData(USART1,bme.Alt);//�򴮿�1��������				
			i=0;//������0
		}
	}
}
