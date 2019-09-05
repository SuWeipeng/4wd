#ifndef __MECANUM_4WD_H__
#define __MECANUM_4WD_H__

#include <stm32f4xx_hal.h>
#include <AP_Motors.h>
#include <AC_PID.h>

#if defined(USE_RTTHREAD)
#include "Semaphore.h"

using namespace rtthread;
#endif

#define HALF_BASE_LENGTH_M  0.075f
#define HALF_BASE_WIDTH_M   0.070f
#define WHEEL_DIAMETER_M    0.096f   
#define WHEEL_RADIUS_M      (WHEEL_DIAMETER_M / 2.0f)
#define MECANUM_VCP_DEBUG   0

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim12;

class Mecanum_4wd
{
public:
  Mecanum_4wd();
  ~Mecanum_4wd();
  
  void run();
  void stop();
  void vel2rpm(float& vel_x, float& vel_y, float& rad_z);
  
#if defined(USE_RTTHREAD)
  void log_write_base();
#endif

private:
  AP_Motors _motor1_fr{&htim1, -1, GPIOD, GPIO_PIN_10, GPIO_PIN_8 , &htim12, TIM_CHANNEL_1, 99, &_pid_1};
  AP_Motors _motor2_fl{&htim5, -1, GPIOE, GPIO_PIN_14, GPIO_PIN_12, &htim2 , TIM_CHANNEL_3, 99, &_pid_2};
  AP_Motors _motor3_bl{&htim3, -1, GPIOE, GPIO_PIN_15, GPIO_PIN_13, &htim2 , TIM_CHANNEL_4, 99, &_pid_3};
  AP_Motors _motor4_br{&htim4, -1, GPIOD, GPIO_PIN_11, GPIO_PIN_9 , &htim12, TIM_CHANNEL_2, 99, &_pid_4};
  AC_PID    _pid_1{1.2f, 1.2f, 0.0f, 0.0f, 200.0f, 1.0f, 1.0f, 0.85f, 0.02f};
  AC_PID    _pid_2{1.2f, 1.2f, 0.0f, 0.0f, 200.0f, 1.0f, 1.0f, 0.85f, 0.02f};
  AC_PID    _pid_3{1.2f, 1.2f, 0.0f, 0.0f, 200.0f, 1.0f, 1.0f, 0.85f, 0.02f};
  AC_PID    _pid_4{1.2f, 1.2f, 0.0f, 0.0f, 200.0f, 1.0f, 1.0f, 0.85f, 0.02f};
  
  float     _motor1_fr_rpm, _motor2_fl_rpm, _motor3_bl_rpm, _motor4_br_rpm;
  
#if defined(USE_RTTHREAD)
  Semaphore _log_sem;
#endif
};
#endif /*__MECANUM_4WD_H__*/
