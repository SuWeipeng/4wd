#include "mode.h"
#include "motors.h"

extern uint16_t speed;
extern uint8_t  key_value;

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
      speed = 35;
      break;
    }
    case MIDDLE_SPEED_MODE:{
      speed = 40;
      break;
    }
    case HIGH_SPEED_MODE:{
      speed = 49;
      break;
    }
  }  
  
  switch(key_value){
  case 0:{
    Turn_Right();
    break;
  }
  case 1:{
    Go_Back();
    break;
  }
  case 2:{
    Turn_Left();
    break;
  }
  case 3:{
    Go_Front();
    break;
  }
  default:{
    speed = 0;
    Go_Front();
    break;
  }
  }
}