#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//��ʱ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
extern TIM_HandleTypeDef TIM3_Handler;      //��ʱ��3��� 
extern TIM_IC_InitTypeDef TIM3_CH1Config;   
//extern TIM_OC_InitTypeDef TIM3_CH4Handler;  //��ʱ��3ͨ��4���

extern TIM_HandleTypeDef TIM5_Handler;      //��ʱ��5���
extern TIM_Encoder_InitTypeDef TIM5_Encoder_Config; //�������ӿ�

extern TIM_HandleTypeDef TIM2_Handler;              //��ʱ��2��� 
extern TIM_Encoder_InitTypeDef TIM2_Encoder_Config; //�������ӿ�

//void TIM3_Init(u16 arr,u16 psc);
//void TIM3_PWM_Init(u16 arr,u16 psc);
//void TIM_SetTIM3Compare4(u32 compare);
void TIM3_CH2_CAP_Init(u16 arr,u16 psc);

void TIM5_Encoder_Init(u32 arr,u16 psc); //TIM5 ������ģʽ��ʼ��
void TIM2_Encoder_Init(u32 arr,u16 psc);
#endif

