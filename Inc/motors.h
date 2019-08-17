#ifndef __MOTORS_H
#define __MOTORS_H

#include <stm32f4xx_hal.h>
#include "encoder.h"
  
typedef struct stm32_motor
{
  TIM_HandleTypeDef *tim;
  GPIO_TypeDef      *dir_port;
  uint16_t          pin_1;      // l298n in1
  uint16_t          pin_2;      // l298n in2
  uint8_t           channel;
  encoder           enc;
}stm32_motor;

typedef enum {
  MOTORS_STOP = 0,
  GO_FRONT,
  GO_BACK,
  TURN_LEFT,
  TURN_RIGHT,
  MOVE_LEFT,
  MOVE_RIGHT
}MOTOR_STATUS;
  
void Motors_Init(void);
void update_motors(MOTOR_STATUS *status);
  
#endif
