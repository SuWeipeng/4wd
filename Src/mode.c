#include "mode.h"

extern uint16_t speed;

typedef enum {
  LOW_SPEED_MODE,
  MIDDLE_SPEED_MODE,
  HIGH_SPEED_MODE
}control_mode;

control_mode current_mode = MIDDLE_SPEED_MODE;

control_mode read_mode()
{
  if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9) == GPIO_PIN_SET){
    return LOW_SPEED_MODE;
  } else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10) == GPIO_PIN_SET){
    return MIDDLE_SPEED_MODE;
  } else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == GPIO_PIN_SET){
    return HIGH_SPEED_MODE;
  }
  
  return MIDDLE_SPEED_MODE;
}

void Mode_Control()
{
  switch(read_mode()){
    case LOW_SPEED_MODE:{
      speed = 39;
      break;
    }
    case MIDDLE_SPEED_MODE:{
      speed = 43;
      break;
    }
    case HIGH_SPEED_MODE:{
      speed = 49;
      break;
    }
  }  
}