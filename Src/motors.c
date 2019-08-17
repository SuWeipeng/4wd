#include "motors.h"
#include "usb_device.h"

#define L298N_PWM_MIN   30
#define MOTOR_VCP_DEBUG 0

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim12;

stm32_motor motor_fl = {.tim      = &htim2,
                        .dir_port = GPIOE,
                        .pin_1    = GPIO_PIN_12,
                        .pin_2    = GPIO_PIN_14,
                        .channel  = TIM_CHANNEL_3}; // front left
stm32_motor motor_fr = {.tim      = &htim2,
                        .dir_port = GPIOE,
                        .pin_1    = GPIO_PIN_13,
                        .pin_2    = GPIO_PIN_15,
                        .channel  = TIM_CHANNEL_4}; // front right
stm32_motor motor_bl = {.tim      = &htim12,
                        .dir_port = GPIOD,
                        .pin_1    = GPIO_PIN_8,
                        .pin_2    = GPIO_PIN_10,
                        .channel  = TIM_CHANNEL_1}; // back left
stm32_motor motor_br = {.tim      = &htim12,
                        .dir_port = GPIOD,
                        .pin_1    = GPIO_PIN_9,
                        .pin_2    = GPIO_PIN_11,
                        .channel  = TIM_CHANNEL_2}; // back right

uint16_t speed_fl = 0;
uint16_t speed_fr = 0;
uint16_t speed_bl = 0;
uint16_t speed_br = 0;

static void spin        (stm32_motor *motor, uint16_t speed, uint8_t inv);
static void go_front    (void);
static void go_back     (void);
static void turn_left   (void);
static void turn_right  (void);
static void move_left   (void);
static void move_right  (void);
static void read_encoder(void);

void Motors_Init()
{
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_2);
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

void go_front()
{
  spin(&motor_fl, speed_fl, 1);
  spin(&motor_bl, speed_bl, 1);
  spin(&motor_fr, speed_fr, 1);
  spin(&motor_br, speed_br, 1);
}

void go_back()
{
  spin(&motor_fl, speed_fl, 0);
  spin(&motor_bl, speed_bl, 0);
  spin(&motor_fr, speed_fr, 0);
  spin(&motor_br, speed_br, 0);
}

void turn_left()
{
  spin(&motor_fl, speed_fl, 0);
  spin(&motor_bl, speed_bl, 0);
  spin(&motor_fr, speed_fr, 1);
  spin(&motor_br, speed_br, 1);
}

void turn_right()
{
  spin(&motor_fl, speed_fl, 1);
  spin(&motor_bl, speed_bl, 1);
  spin(&motor_fr, speed_fr, 0);
  spin(&motor_br, speed_br, 0);
}

void move_left()
{
  spin(&motor_fl, speed_fl, 0);
  spin(&motor_bl, speed_bl, 1);
  spin(&motor_fr, speed_fr, 1);
  spin(&motor_br, speed_br, 0);
}

void move_right()
{
  spin(&motor_fl, speed_fl, 1);
  spin(&motor_bl, speed_bl, 0);
  spin(&motor_fr, speed_fr, 0);
  spin(&motor_br, speed_br, 1);
}

void read_encoder(void)
{
  get_circles(0,&motor_fr.enc);
  get_circles(1,&motor_bl.enc);
  get_circles(2,&motor_br.enc);
  get_circles(3,&motor_fl.enc);
  
  get_direct(0,&motor_fr.enc);
  get_direct(1,&motor_bl.enc);
  get_direct(2,&motor_br.enc);
  get_direct(3,&motor_fl.enc);
}

void update_motors(MOTOR_STATUS *status)
{
  if (status == NULL)
    return;
  
  switch(*status){
  case MOTORS_STOP:
    {
      spin(&motor_fl, 0, 0);
      spin(&motor_bl, 0, 0);
      spin(&motor_fr, 0, 0);
      spin(&motor_br, 0, 0);
      break;
    }
  case GO_FRONT:
    {
      go_front();
      break;
    }
  case GO_BACK:
    {
      go_back();
      break;
    }
  case TURN_LEFT:
    {
      turn_left();
      break;
    }
  case TURN_RIGHT:
    {
      turn_right();
      break;
    }
  case MOVE_LEFT:
    {
      move_left();
      break;
    }
  case MOVE_RIGHT:
    {
      move_right();
      break;
    }
  default:
    break;
  }
  
  read_encoder();
  
#if MOTOR_VCP_DEBUG
  char uartTxBuf[32];  
  sprintf(uartTxBuf, "status:%d\r\n", (int)*status);
  VCPSend((uint8_t *)uartTxBuf, strlen(uartTxBuf));
#endif
}
