#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h"
#include "key.h"
#include "exti.h"
#include "can.h"
#include "timer.h"

int main(void)
{
	u8 M01[2]={0x80,0x01};  
	u8 M02[2]={0x01,0x01};  
	u8 M1[1];               //SYNC
	u16 ID=0x000;
//	u8 res=0;               //can���ͱ�ʶ
//	u8 sen=0;               //can���ձ�ʶ
//	u8 canbuf[8];           //can����
	double count1,count2=0.0f;
	double angle1,angle2=0.0f;
	

	
    Cache_Enable();                 //��L1-Cache
    HAL_Init();				        //��ʼ��HAL��
    Stm32_Clock_Init(432,25,2,9);   //����ʱ��,216Mhz 
    delay_init(216);                //��ʱ��ʼ��
	uart_init(115200);		        //���ڳ�ʼ��
	LED_Init();                     //LED��ʼ��
	KEY_Init();                     //��ʼ������
	//������=Fpclk1/((tbs1+tbs2+1)*brp)=54/(6+11+1)/3=1Mbps
	EXTI_Init();                    //�ⲿ�жϳ�ʼ��
	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_6TQ,CAN_BS1_11TQ,3,CAN_MODE_NORMAL);//CAN��ʼ������ģʽ,������1Mbps               
	TIM5_Encoder_Init(0xFFFFFFFF-1,1-1);	
	TIM2_Encoder_Init(0xFFFFFFFF-1,1-1);
    TIM3_CH2_CAP_Init(1080-1,1-1);  //���ʱ��Ϊ10us
	
	//NMT��������
	CAN1_Send_Msg(M01,2,ID);        //����Ԥ����״̬
	delay_ms(1);
	CAN1_Send_Msg(M02,2,ID);        //����Զ�̽ڵ�1�������״̬ 
		
	while(1)
	{
		       
		//����SYNC����
		ID=0x080;                       
		CAN1_Send_Msg(M1,0,ID);        //����0���ֽ� 
		
//		if(res)printf("N\r\n");		//��ʾ����ʧ��
//		else printf("Y\r\n");		    //��ʾ���ͳɹ�
//		sen=CAN1_Receive_Msg(canbuf);
//		if(sen)printf("\r\n can����\r\n");
		
//		count1=__HAL_TIM_GET_COUNTER(&TIM5_Handler);
//		count2=__HAL_TIM_GET_COUNTER(&TIM2_Handler);
//		angle1=count1/20000*360;
//		angle2=count2/320000*360;
//		printf("ANGLE1:%lf,ANGLE2:%lf\r\n\r\n",angle1,angle2);
		
		delay_ms(10);
	}
}
