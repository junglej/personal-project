#include "common.h"
 #include "led.h"   
void TIM2_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		USART_RX_STA|=1<<15;	//��ǽ������
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIM7�����жϱ�־    
		TIM_Cmd(TIM2, DISABLE);  //�ر�TIM7 
	}	    
}

void TIM2_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//TIM7ʱ��ʹ��    

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�
	
	TIM_Cmd(TIM2,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}
	 
u8* atk_8266_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART_RX_BUF[USART_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//��ATK-ESP8266��������
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
u8 res=0; 
USART_RX_STA=0;
u1_printf("%s\r\n",cmd);	//��������
if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
{
	while(--waittime)	//�ȴ�����ʱ
	{
		delay_ms(10);
		if(USART_RX_STA&0X8000)//���յ��ڴ���Ӧ����
		{
			if(atk_8266_check_cmd(ack))
			{
//				printf("ack:%s\r\n",(u8*)ack);
				break;//�õ���Ч���� 
			}
				USART_RX_STA=0;
		} 
	}
	if(waittime==0)res=1; 
}
return res;
} 
//��ATK-ESP8266����ָ������
//data:���͵�����(����Ҫ��ӻس���)
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
		u8 res=0; 
		USART_RX_STA=0;
		u1_printf("%s",data);	//��������
		if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
		{
			while(--waittime)	//�ȴ�����ʱ
			{
				delay_ms(10);
				if(USART_RX_STA&0X8000)//���յ��ڴ���Ӧ����
				{
					if(atk_8266_check_cmd(ack))break;//�õ���Ч���� 
					USART_RX_STA=0;
				} 
			}
			if(waittime==0)res=1; 
		}
		return res;
}

//��ȡClient ip��ַ
//ipbuf:ip��ַ���������
void atk_8266_get_wanip(u8* ipbuf)
{
	u8 *p,*p1;
	if(atk_8266_send_cmd("AT+CIFSR","OK",50))//��ȡWAN IP��ַʧ��
	{
		ipbuf[0]=0;
		return;
	}		
	p=atk_8266_check_cmd("\"");
	p1=(u8*)strstr((const char*)(p+1),"\"");
	*p1=0;
	sprintf((char*)ipbuf,"%s",p+1);	
}


//ATK-ESP8266ģ�����������
void atk_8266_test(void)
	{ 
	u8 *p;	
	while(atk_8266_send_cmd("AT","OK",20))//���WIFIģ���Ƿ�����
			atk_8266_send_cmd("AT+CIPMODE=0","OK",200); //�ر�͸��ģʽ	
	while(atk_8266_send_cmd("ATE0","OK",20));//�رջ���
	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0�������ӣ�1��������
	sprintf((char*)p,"AT+CIPSERVER=1,%s","8080");
	atk_8266_send_cmd(p,"OK",20);     //����Serverģʽ���˿ں�Ϊ8080		
	USART_RX_STA=0;
}



