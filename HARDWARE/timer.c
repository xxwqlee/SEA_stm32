#include "timer.h"
#include "led.h"
#include "can.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F7������
//��ʱ���ж���������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//�޸�˵��
//V1.1 20160711
//����TIM3_PWM_Init����,����PWM���
////////////////////////////////////////////////////////////////////////////////// 	 

TIM_HandleTypeDef TIM3_Handler;              //��ʱ��3��� 
TIM_IC_InitTypeDef TIM3_CH1Config; 
//TIM_OC_InitTypeDef TIM3_CH4Handler;          //��ʱ��3ͨ��4���

TIM_HandleTypeDef TIM5_Handler;              //��ʱ��5��� 
TIM_Encoder_InitTypeDef TIM5_Encoder_Config; //�������ӿ�

TIM_HandleTypeDef TIM2_Handler;              //��ʱ��2��� 
TIM_Encoder_InitTypeDef TIM2_Encoder_Config; //�������ӿ�
static double count1,count2,count1_0=0.0f,count2_0=0.0f;
static double dangle1,dangle2=0.0f;
//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_CH2_CAP_Init(u16 arr,u16 psc)
{  
     
    
    TIM3_Handler.Instance=TIM3;                          //ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM3_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM3_Handler);
    
    TIM3_CH1Config.ICPolarity=TIM_ICPOLARITY_FALLING;    //�����ز���
    TIM3_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
    TIM3_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM3_CH1Config.ICFilter=0;                          //���������˲��������˲�
    HAL_TIM_IC_ConfigChannel(&TIM3_Handler,&TIM3_CH1Config,TIM_CHANNEL_2);//����TIM5ͨ��2
    HAL_TIM_IC_Start_IT(&TIM3_Handler,TIM_CHANNEL_2);   //��ʼ����TIM3��ͨ��2
    __HAL_TIM_ENABLE_IT(&TIM3_Handler,TIM_IT_UPDATE);   //ʹ�ܸ����ж�
}

//��ʱ��3�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_IC_Init()����
//htim:��ʱ��3���
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_7;            //PA7
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //�����������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    GPIO_Initure.Alternate=GPIO_AF2_TIM3;   //PA7����ΪTIM3ͨ��2
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM3_IRQn,1,0);    //�����ж����ȼ�����ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�   
}



//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);//��ʱ�����ô�������
}
 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//�����жϣ����������ʱִ��Tout= ((arr+1)*(psc+1))/Tclk
{
//	double count1,count2,angle1_b=0.0f,angle2_b=0.0f;
//	
//	double speed1,speed2;
//	count1=__HAL_TIM_GET_COUNTER(&TIM5_Handler);
//	count2=__HAL_TIM_GET_COUNTER(&TIM2_Handler);
//	angle1_b=count1/20000*360;
//	angle2_b=count2/320000*360;
//	speed1=(angle1_b-angle1_a)*100000;
//	speed2=(angle2_b-angle2_a)*100000;
////	printf("ANGLE1:%lf,ANGLE2:%lf\r\nSPEED1:%f,SPEED2:%f\r\n",angle1_b,angle2_b,speed1,speed2);	
//	angle1_a=angle1_b;
//	angle2_a=angle2_b; 
}
//��ʱ�����벶���жϴ����ص��������ú�����HAL_TIM_IRQHandler�лᱻ����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//�����жϷ���ʱִ��
{
	
	count1=__HAL_TIM_GET_COUNTER(&TIM5_Handler);
	count2=__HAL_TIM_GET_COUNTER(&TIM2_Handler);
	dangle1=(count1-count1_0)/20000*360;
	dangle2=(count2-count2_0)/320000*360;
	printf("%f    %f\r\n",dangle1,dangle2);
	
	count1_0=count1;
	count2_0=count2;
}


//TIM5 ������ģʽ��ʼ��

void TIM5_Encoder_Init(u32 arr,u16 psc) 
{	 
	TIM5_Handler.Instance=TIM5;                               //��ʱ��5
    TIM5_Handler.Init.Prescaler=psc;                          //��ʱ����Ƶ
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;         //���ϼ���ģʽ
    TIM5_Handler.Init.Period=arr;                             //�Զ���װ��ֵ
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	TIM5_Encoder_Config.EncoderMode=TIM_ENCODERMODE_TI12;     //TI1��TI2������
	TIM5_Encoder_Config.IC1Polarity=TIM_ICPOLARITY_FALLING;    //˫���ز���
    TIM5_Encoder_Config.IC1Selection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
    TIM5_Encoder_Config.IC1Prescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM5_Encoder_Config.IC1Filter=0;                          //���������˲��������˲�
	TIM5_Encoder_Config.IC2Polarity=TIM_ICPOLARITY_FALLING;    //˫���ز���
    TIM5_Encoder_Config.IC2Selection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI2��
    TIM5_Encoder_Config.IC2Prescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM5_Encoder_Config.IC2Filter=0;                          //���������˲��������˲� 
    HAL_TIM_Encoder_Init(&TIM5_Handler,&TIM5_Encoder_Config); //��ʼ��TIM5�������ӿ�	
	HAL_TIM_Encoder_Start(&TIM5_Handler,TIM_CHANNEL_ALL);     //��TIM5��ͨ��
}

//TIM2 ������ģʽ��ʼ��
void TIM2_Encoder_Init(u32 arr,u16 psc) 
{	 
	TIM2_Handler.Instance=TIM2;                               //��ʱ��2
    TIM2_Handler.Init.Prescaler=psc;                          //��ʱ����Ƶ
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;         //���ϼ���ģʽ
    TIM2_Handler.Init.Period=arr;                             //�Զ���װ��ֵ
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	TIM2_Encoder_Config.EncoderMode=TIM_ENCODERMODE_TI12;     //TI1��TI2������
	TIM2_Encoder_Config.IC1Polarity=TIM_ICPOLARITY_FALLING;    //˫���ز���
    TIM2_Encoder_Config.IC1Selection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
    TIM2_Encoder_Config.IC1Prescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM2_Encoder_Config.IC1Filter=0;                          //���������˲��������˲�
	TIM2_Encoder_Config.IC2Polarity=TIM_ICPOLARITY_FALLING;    //˫���ز���
    TIM2_Encoder_Config.IC2Selection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI2��
    TIM2_Encoder_Config.IC2Prescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM2_Encoder_Config.IC2Filter=0;                          //���������˲��������˲�
    HAL_TIM_Encoder_Init(&TIM2_Handler,&TIM2_Encoder_Config); //��ʼ��TIM2�������ӿ�	
	HAL_TIM_Encoder_Start(&TIM2_Handler,TIM_CHANNEL_ALL);     //��TIM2��ͨ��
}

//������ģʽ��ʼ���ص�����
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
{
	
  GPIO_InitTypeDef GPIO_Initure;                       //��ʼ��IO
  if(htim->Instance==TIM5)
  {
	__HAL_RCC_TIM5_CLK_ENABLE();                    //ʹ�ܶ�ʱ��5
	__HAL_RCC_GPIOH_CLK_ENABLE();		            //����GPIOHʱ��
	
	GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;       //PH10,PH11
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	        //��������
    GPIO_Initure.Pull=GPIO_PULLUP;        		   	//����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     		//����
	GPIO_Initure.Alternate=GPIO_AF2_TIM5;			//PH10,11����ΪTIM5_CH1,2
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
  }
  else if(htim->Instance==TIM2)
  {
	__HAL_RCC_TIM2_CLK_ENABLE();                    //ʹ�ܶ�ʱ��2
	__HAL_RCC_GPIOA_CLK_ENABLE();		            //����GPIOAʱ��
	__HAL_RCC_GPIOB_CLK_ENABLE();                   //����GPIOBʱ��
	
	GPIO_Initure.Pin=GPIO_PIN_3;                    //PB3
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	        //��������
    GPIO_Initure.Pull=GPIO_PULLUP;        		    //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     	    //����
	GPIO_Initure.Alternate=GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	  
	GPIO_Initure.Pin=GPIO_PIN_15;          		    //PA15
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
  }
}