#include "mode.h"
#include "motors.h"

extern uint16_t speed_fl;
extern uint16_t speed_fr;
extern uint16_t speed_bl;
extern uint16_t speed_br;
extern uint8_t  key_value;

static MOTOR_STATUS status;
control_mode current_mode = CLOSE_LOOP_CONTROL;

void pid_control(void);

control_mode read_mode()
{
  if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9) == GPIO_PIN_SET){
    return OPEN_LOOP_CONTROL;
  } else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10) == GPIO_PIN_SET){
    return CLOSE_LOOP_CONTROL;
  }
  
  return CLOSE_LOOP_CONTROL;
}

void update_mode()
{
  switch(read_mode()){
    case OPEN_LOOP_CONTROL:{
      speed_fl = 49;
      speed_fr = 49;
      speed_bl = 49;
      speed_br = 49;
      break;
    }
    case CLOSE_LOOP_CONTROL:{
      pid_control();
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