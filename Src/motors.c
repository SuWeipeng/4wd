#include "motors.h"

#define L298N_PWM_MIN 35

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim12;

stm32_motor motor_fl; // front left
stm32_motor motor_fr; // front right
stm32_motor motor_bl; // back left
stm32_motor motor_br; // back right

uint16_t speed = 0;

void Motors_Init()
{
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_2);
  
  motor_fl = (stm32_motor){.tim      = &htim2,
                           .dir_port = GPIOE,
                           .pin_1    = GPIO_PIN_12,
                           .pin_2    = GPIO_PIN_14,
                           .channel  = TIM_CHANNEL_3};
  motor_bl = (stm32_motor){.tim      = &htim2,
                           .dir_port = GPIOE,
                           .pin_1    = GPIO_PIN_13,
                           .pin_2    = GPIO_PIN_15,
                           .channel  = TIM_CHANNEL_4};
  motor_fr = (stm32_motor){.tim      = &htim12,
                           .dir_port = GPIOD,
                           .pin_1    = GPIO_PIN_8,
                           .pin_2    = GPIO_PIN_10,
                           .channel  = TIM_CHANNEL_1};
  motor_br = (stm32_motor){.tim      = &htim12,
                           .dir_port = GPIOD,
                           .pin_1    = GPIO_PIN_9,
                           .pin_2    = GPIO_PIN_11,
                           .channel  = TIM_CHANNEL_2};
}

void spin(stm32_motor *motor, uint16_t speed, uint8_t inv)
{
  if(speed <= L298N_PWM_MIN) {
    HAL_GPIO_WritePin(motor->dir_port, motor->pin_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->dir_port, motor->pin_2, GPIO_PIN_RESET);
  } else if (inv == 0) {
    HAL_GPIO_WritePin(motor->dir_port, motor->pin_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(motor->dir_port, motor->pin_2, GPIO_PIN_RESET);
  } else {
    HAL_GPIO_WritePin(motor->dir_port, motor->pin_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->dir_port, motor->pin_2, GPIO_PIN_SET);
  }
  
  __HAL_TIM_SET_COMPARE(motor->tim, motor->channel, speed);
}

void Go_Front()
{
  spin(&motor_fl, speed, 1);
  spin(&motor_bl, speed, 1);
  spin(&motor_fr, speed, 1);
  spin(&motor_br, speed, 1);
}

void Go_Back()
{
  spin(&motor_fl, speed, 0);
  spin(&motor_bl, speed, 0);
  spin(&motor_fr, speed, 0);
  spin(&motor_br, speed, 0);
}

void Turn_Left()
{
  spin(&motor_fl, speed, 0);
  spin(&motor_bl, speed, 0);
  spin(&motor_fr, speed, 1);
  spin(&motor_br, speed, 1);
}

void Turn_Right()
{
  spin(&motor_fl, speed, 1);
  spin(&motor_bl, speed, 1);
  spin(&motor_fr, speed, 0);
  spin(&motor_br, speed, 0);
}