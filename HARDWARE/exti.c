#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "can.h"
#include "usart.h"
#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//�ⲿ�ж���������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
static double count1,count2,count1_0=0.0f,count2_0=0.0f;
static double dangle1,dangle2=0.0f;
//�ⲿ�жϳ�ʼ��
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();               //����GPIOAʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();               //����GPIOCʱ��
    __HAL_RCC_GPIOH_CLK_ENABLE();               //����GPIOHʱ��
	__HAL_RCC_CAN1_CLK_ENABLE();                //ʹ��CAN1ʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_0;                //PA0
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //�����ش���
    GPIO_Initure.Pull=GPIO_PULLDOWN;			//����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_13;               //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_PULLUP;				//����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3;     //PH2,3  �½��ش���������
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    //�ж���0
    HAL_NVIC_SetPriority(EXTI0_IRQn,1,0);       //��ռ���ȼ�Ϊ1�����ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);             //ʹ���ж���0
    
    //�ж���2
    HAL_NVIC_SetPriority(EXTI2_IRQn,1,1);       //��ռ���ȼ�Ϊ1�������ȼ�Ϊ1
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);             //ʹ���ж���2
    
    //�ж���3
    HAL_NVIC_SetPriority(EXTI3_IRQn,1,2);       //��ռ���ȼ�Ϊ1�������ȼ�Ϊ2
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //ʹ���ж���2
	
	//�ж���5
	HAL_NVIC_SetPriority(EXTI9_5_IRQn,0,2);       //��ռ���ȼ�Ϊ0�������ȼ�Ϊ2
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);             //ʹ���ж���5
    
    //�ж���13
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,1,3);   //��ռ���ȼ�Ϊ1�������ȼ�Ϊ3
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //ʹ���ж���15  
}


//�жϷ�����
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);   //�����жϴ����ú���
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);   //�����жϴ����ú���
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);   //�����жϴ����ú���
}

void EXTI5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);   //�����жϴ����ú���
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);  //�����жϴ����ú���
}

//�жϷ����������Ҫ��������
//��HAL�������е��ⲿ�жϷ�����������ô˺���
//GPIO_Pin:�ж����ź�
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static u8 led0sta=1,led1sta=1;
	u16 ID;
    u8 res;
	
	//SDO�ٶȻ�����
	u8 M11[8]={0x2f,0x60,0x60,0x00,0x03};                //�����ٶȻ�
	u8 M12[8]={0x2b,0x40,0x60,0x00,0x05,0x00};           //������disable
	u8 M13[8]={0x23,0xff,0x60,0x00,0x40,0x0D,0x03,0x00}; //�����ٶ�
	u8 M14[8]={0x23,0x00,0x21,0x00,0xa0,0x86,0x01,0x00}; //���ü��ٶ�
	u8 M15[8]={0x2b,0x40,0x60,0x00,0x0f,0x00};           //�����ٶȻ�
	
	//TxPDO2 ӳ��
	u8 M21[8]={0x2f,0x01,0x1A,0x00,0x00};                		//ӳ��Disable
	u8 M22[8]={0x2f,0x01,0x18,0x02,0x01};                		//����ͨѶ����Ϊͬ����1��SYNC�źţ�
	u8 M23[8]={0x23,0x01,0x1A,0x01,0x10,0x00,0x60,0x22}; 		//�����λ��
	u8 M24[8]={0x23,0x01,0x1A,0x02,0x10,0x00,0x63,0x22}; 		//��������
	u8 M25[8]={0x23,0x01,0x1A,0x03,0x20,0x00,0x69,0x60};		//���ת��
	u8 M26[8]={0x2f,0x01,0x1A,0x00,0x03};                		//ӳ��Enable
	
	//PDO�ٶȿ���
//	u8 M3[6]={0x00,0xF6,0xA0,0x86,0x01,0x00};			//�����ٶ�
	u8 M3[6]={0x00,0xF6,0xA0,0x86,0x00,0x00};			//�����ٶ�
	
	//PDO�ٶȿ���
	u8 M2[6]={0x00,0xF6,0xE0,0x93,0x04,0x00};			//�����ٶ�
		
	//	//Heartbeat����
	//	u8 M20[8]={0x2b,0x17,0x10,0x00,0xE8,0x03};//����Ϊ1000ms
    delay_ms(50);      //����
	
    switch(GPIO_Pin)
    {
        case GPIO_PIN_0:
            if(WK_UP==1)	
            {
				//SDO�ٶȻ�����			
				ID=0x601;		
				
				res=CAN1_Send_Msg(M11,8,ID);        
				delay_ms(1);  
				res=CAN1_Send_Msg(M12,8,ID);       
				delay_ms(1);    
				res=CAN1_Send_Msg(M13,8,ID);       
				delay_ms(1);    
				res=CAN1_Send_Msg(M14,8,ID);       
				delay_ms(1);    
				res=CAN1_Send_Msg(M15,8,ID);        
				delay_ms(1);    
				
			    //����LED0,LED1�������
                led1sta=!led1sta;
                led0sta=!led1sta;
                LED1(led1sta);
                LED0(led0sta);
            }
            break;
        case GPIO_PIN_2:
            if(KEY1==0) 	
            {
				//PDO�ٶȿ���
				ID=0x501;
				
				res=CAN1_Send_Msg(M3,6,ID);        
				
				//����LED1��ת	
                led1sta=!led1sta;
                LED1(led1sta);					  
            };
            break;
        case GPIO_PIN_3:
            if(KEY0==0)  	
            {
				//PDO�ٶȿ���
				ID=0x501;
				
				res=CAN1_Send_Msg(M2,6,ID);  
				
				//ͬʱ����LED0,LED1��ת 
                led1sta=!led1sta;
                led0sta=!led0sta;
                LED1(led1sta);
                LED0(led0sta); 			  
            }
            break;
		case GPIO_PIN_5:
            count1=__HAL_TIM_GET_COUNTER(&TIM5_Handler);
			count2=__HAL_TIM_GET_COUNTER(&TIM2_Handler);
			dangle1=(count1-count1_0)/20000*360;
			dangle2=(count2-count2_0)/320000*360;
			printf("%f    %f\r\n",dangle1,dangle2);

			count1_0=count1;
			count2_0=count2;
            break;
        case GPIO_PIN_13:
            if(KEY2==0)  	
            {
				//TxPDO2 ӳ��
                ID=0x601;	
				
				res=CAN1_Send_Msg(M21,8,ID);        
				delay_ms(1);    
				res=CAN1_Send_Msg(M22,8,ID);       
				delay_ms(1);    
				res=CAN1_Send_Msg(M23,8,ID);         
				delay_ms(1);    
				res=CAN1_Send_Msg(M24,8,ID);        
				delay_ms(1);    
				res=CAN1_Send_Msg(M25,8,ID);        
				delay_ms(1);    
				res=CAN1_Send_Msg(M26,8,ID);        
				delay_ms(1);   
				
				//����LED0��ת
				led0sta=!led0sta;
                LED0(led0sta);
            }
            break;
    }
}
