#include "mode.h"
#include "motors.h"

extern uint16_t speed;
extern uint8_t  key_value;

static MOTOR_STATUS status;
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

void update_mode()
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
    status = TURN_RIGHT;
    break;
  }
  case 1:{
    status = GO_BACK;
    break;
  }
  case 2:{
    status = TURN_LEFT;
    break;
  }
  case 3:{
    status = GO_FRONT;
    break;
  }
  default:{
    status = MOTORS_STOP;
    break;
  }
  }
  
  update_motors(&status);
}