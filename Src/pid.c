#include <math.h>
#include "pid.h"
#include "motors.h"
#include "usb_device.h"

#define REDUCTION_RATIO_MUL    (48.0f)
#define REDUCTION_RATIO        (1/REDUCTION_RATIO_MUL)
#define DELTA_T                (0.02f)
#define TARGET_20HZ_SPEED      (2.0f)
#define PID_VCP_DEBUG          1

extern uint8_t     key_value;
extern stm32_motor motor_fl; // front left
extern stm32_motor motor_fr; // front right
extern stm32_motor motor_bl; // back left
extern stm32_motor motor_br; // back right

extern uint16_t speed_fl;
extern uint16_t speed_fr;
extern uint16_t speed_bl;
extern uint16_t speed_br;

static float last_circles[4];
static float _dt = DELTA_T;
static float error_last = 0.0f;
static float _target;
static float _error;
static float _derivative;
static float _integrator;
static float _kp = 0.12f;
static float _ki = 0.5f;
static float _kd = 0.0f;

void  update_i(uint8_t limit);
float get_filt_T_alpha(void);
float get_filt_E_alpha(void);
float get_filt_D_alpha(void);
float get_speed (uint8_t who, float circles);
float calc_speed(uint8_t who, float *circles, float *last_circles);

// 50Hz update
void pid_control()
{
  speed_fl = (uint16_t)(update_all(TARGET_20HZ_SPEED, get_speed(0, motor_fl.enc.circles), 0)*REDUCTION_RATIO_MUL+0.5f);
  speed_fr = (uint16_t)(update_all(TARGET_20HZ_SPEED, get_speed(1, motor_fr.enc.circles), 0)*REDUCTION_RATIO_MUL+0.5f);
  speed_bl = (uint16_t)(update_all(TARGET_20HZ_SPEED, get_speed(2, motor_bl.enc.circles), 0)*REDUCTION_RATIO_MUL+0.5f);
  speed_br = (uint16_t)(update_all(TARGET_20HZ_SPEED, get_speed(3, motor_br.enc.circles), 0)*REDUCTION_RATIO_MUL+0.5f);
}

float update_all(float target, float measurement, uint8_t limit)
{
  // don't process inf or NaN
  if (!isfinite(target) || !isfinite(measurement)) {
    return 0.0f;
  }

  error_last = _error;
  _target += get_filt_T_alpha() * (target - _target);
  _error += get_filt_E_alpha() * ((_target - measurement) - _error);

  // calculate and filter derivative
  if (_dt > 0.0f) {
    float derivative = (_error - error_last) / _dt;
    _derivative += get_filt_D_alpha() * (derivative - _derivative);
  }

  // update I term
  update_i(limit);

  float P_out = (_error * _kp);
  float D_out = (_derivative * _kd);

#if PID_VCP_DEBUG
  char uartTxBuf[100];  
  sprintf(uartTxBuf, "p:%.2f, i:%.2f, d:%.2f\r\ntar:%.2f, measure:%.2f\r\n", P_out, _integrator, D_out, _target, measurement);
  VCPSend((uint8_t *)uartTxBuf, strlen(uartTxBuf));
#endif
  
  return P_out + _integrator + D_out;
}

//  update_i - update the integral
//  If the limit flag is set the integral is only allowed to shrink
void update_i(uint8_t limit)
{
  float _kimax = 49.0f;
  
  if (!is_zero(_ki) && is_positive(_dt) && key_value != 7) {
    // Ensure that integrator can only be reduced if the output is saturated
    if (!limit || ((is_positive(_integrator) && is_negative(_error)) || (is_negative(_integrator) && is_positive(_error)))) {
      _integrator += ((float)_error * _ki) * _dt;
      _integrator = constrain_float(_integrator, -_kimax, _kimax);
    }
  } else {
    _integrator = 0.0f;
  }
}

// get_filt_T_alpha - get the target filter alpha
float get_filt_T_alpha()
{
  return 1.0f;
}

// get_filt_E_alpha - get the error filter alpha
float get_filt_E_alpha()
{
  return 1.0f;
}

// get_filt_D_alpha - get the derivative filter alpha
float get_filt_D_alpha()
{
  return 1.0f;
}

float constrain_float(float input, float lim_down, float lim_up)
{
  if(input < lim_down)
    return lim_down;
  if(input > lim_up)
    return lim_up;
  
  return input;
}

uint8_t is_positive(float input)
{
  if(input > 1.0e-6f)
    return 1;
  
  return 0;
}

uint8_t is_negative(float input)
{
  if(input < -1.0e-6f)
    return 1;
  
  return 0;
}

uint8_t is_zero(float input)
{
  if(input > -1.0e-6f && input < 1.0e-6f)
    return 1;
  
  return 0;
}

float get_speed(uint8_t who, float circles)
{
  switch(who){
  case 0:{
    return calc_speed(0, &motor_fl.enc.circles, &last_circles[0]);
  }
  case 1:{
    return calc_speed(1, &motor_fr.enc.circles, &last_circles[1]);
  }
  case 2:{
    return calc_speed(2, &motor_bl.enc.circles, &last_circles[2]);
  }
  case 3:{
    return calc_speed(3, &motor_br.enc.circles, &last_circles[3]);
  }
  default:
    return 0.0f;
  }
}

float calc_speed(uint8_t who, float *circles, float *last_circles)
{
  (void)who; // UNUSED(who)
  
  float speed;
  
  speed = *circles - *last_circles;
  
  if(fabsf(speed) >= (MAX_DELTA*REDUCTION_RATIO)) {
    float temp;
    if(speed > 0) {
      temp  = MAX_CIRCLES - *circles;
      speed = -(*last_circles + temp);
    } else {
      temp  = MAX_CIRCLES - *last_circles;
      speed = *circles + temp;
    }
  }
  
  *last_circles = *circles;

  return fabsf(speed);
}
