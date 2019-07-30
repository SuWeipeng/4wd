#ifndef __MOTORS_H
#define __MOTORS_H

#include <stm32f4xx_hal.h>
    
typedef struct stm32_motor
{
  TIM_HandleTypeDef *tim;
  GPIO_TypeDef      *dir_port;
  uint16_t          pin_1;      // l298n in1
  uint16_t          pin_2;      // l298n in2
  uint8_t           channel;
}stm32_motor;

void Motors_Init();
void Go_Front();
void Go_Back();
void Turn_Left();
void Turn_Right();

#endif