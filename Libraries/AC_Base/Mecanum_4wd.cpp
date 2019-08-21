#include <stm32f4xx_hal.h>
#include <AP_Motors.h>
#include <AC_PID.h>
#include <vectorN.h>
#include "Mecanum_4wd.h"
#include "usb_device.h"

typedef VectorN<float,3> Vector3f;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim12;

static float r1[3] = { 1,  1,  (HALF_BASE_LENGTH_M+HALF_BASE_WIDTH_M)};
static float r2[3] = { 1, -1, -(HALF_BASE_LENGTH_M+HALF_BASE_WIDTH_M)};
static float r3[3] = { 1,  1, -(HALF_BASE_LENGTH_M+HALF_BASE_WIDTH_M)};
static float r4[3] = { 1, -1,  (HALF_BASE_LENGTH_M+HALF_BASE_WIDTH_M)};

static Vector3f _r1(r1),_r2(r2),_r3(r3),_r4(r4);

Mecanum_4wd::Mecanum_4wd()
{
  _pid_1 = new AC_PID(0.045f, 
                      0.07f, 
                      0.0014f, 
                      0.0f,
                      95.0f,
                      1.0f,
                      1.0f,
                      0.85f,
                      0.02f);
  _pid_2 = new AC_PID(0.045f, 
                      0.07f, 
                      0.0014f, 
                      0.0f,
                      95.0f,
                      1.0f,
                      1.0f,
                      0.85f,
                      0.02f);
  _pid_3 = new AC_PID(0.045f, 
                      0.07f, 
                      0.0014f, 
                      0.0f,
                      95.0f,
                      1.0f,
                      1.0f,
                      0.85f,
                      0.02f);
  _pid_4 = new AC_PID(0.045f, 
                      0.07f, 
                      0.0014f, 
                      0.0f,
                      95.0f,
                      1.0f,
                      1.0f,
                      0.85f,
                      0.02f);
  
  _motor1_fr = new AP_Motors(&htim1,
                             -1,
                             GPIOD,
                             GPIO_PIN_10,
                             GPIO_PIN_8,
                             &htim12,
                             TIM_CHANNEL_1,
                             99,
                             _pid_1
                               );
  _motor2_fl = new AP_Motors(&htim5,
                             -1,
                             GPIOE,
                             GPIO_PIN_14,
                             GPIO_PIN_12,
                             &htim2,
                             TIM_CHANNEL_3,
                             99,
                             _pid_2
                               );
  _motor3_bl = new AP_Motors(&htim3,
                             -1,
                             GPIOE,
                             GPIO_PIN_15,
                             GPIO_PIN_13,
                             &htim2,
                             TIM_CHANNEL_4,
                             99,
                             _pid_3
                               );
  _motor4_br = new AP_Motors(&htim4,
                             -1,
                             GPIOD,
                             GPIO_PIN_11,
                             GPIO_PIN_9,
                             &htim12,
                             TIM_CHANNEL_2,
                             99,
                             _pid_4
                               ); 
}

Mecanum_4wd::~Mecanum_4wd()
{
  delete _motor1_fr;
  delete _motor2_fl;
  delete _motor3_bl;
  delete _motor4_br;
}

void Mecanum_4wd::vel2rpm(float& vel_x, float& vel_y, float& rad_z)
{  
  float scale = 1;
  float vel[3] = {vel_x * 60, vel_y * 60, rad_z * 60}; // m/s -> m/min, rad/s -> rad/min
  Vector3f _vel(vel);
  
  /* check rpm max  */
  while(1)
  {
    _motor1_fr_rpm = (_r1 * _vel) / WHEEL_RADIUS_M;
    _motor2_fl_rpm = (_r2 * _vel) / WHEEL_RADIUS_M;
    _motor3_bl_rpm = (_r3 * _vel) / WHEEL_RADIUS_M;
    _motor4_br_rpm = (_r4 * _vel) / WHEEL_RADIUS_M;
    if(abs(_motor1_fr_rpm) <= MOTORS_MAX_RPM 
    && abs(_motor2_fl_rpm) <= MOTORS_MAX_RPM
    && abs(_motor3_bl_rpm) <= MOTORS_MAX_RPM
    && abs(_motor4_br_rpm) <= MOTORS_MAX_RPM)
    {
      break;
    }
    scale -= 0.0333334;
    _vel  *= scale;
  }
  
  run();
}

void Mecanum_4wd::run()
{
  _motor1_fr->set_rpm(_motor1_fr_rpm);
  _motor2_fl->set_rpm(_motor2_fl_rpm);
  _motor3_bl->set_rpm(_motor3_bl_rpm);
  _motor4_br->set_rpm(_motor4_br_rpm);
}

void Mecanum_4wd::stop()
{
  _motor1_fr->set_rpm(0);
  _motor2_fl->set_rpm(0);
  _motor3_bl->set_rpm(0);
  _motor4_br->set_rpm(0);
}