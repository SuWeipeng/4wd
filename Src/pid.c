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
static float _target[4];
static float _error[4];
static float _derivative[4];
static float _integrator[4];
static float _kp = 0.12f;
static float _ki = 0.5f;
static float _kd = 0.0f;

void  update_i(uint8_t who, uint8_t limit);
float get_filt_T_alpha(void);
float get_filt_E_alpha(void);
float get_filt_D_alpha(void);
float get_speed (uint8_t who, float circles);
float calc_speed(uint8_t who, float *circles, float *last_circles);

// 50Hz update
void pid_control()
{
  speed_fl = (uint16_t)(update_all(0, TARGET_20HZ_SPEED, get_speed(0, motor_fl.enc.circles), 0)*REDUCTION_RATIO_MUL+0.5f);
  speed_fr = (uint16_t)(update_all(1, TARGET_20HZ_SPEED, get_speed(1, motor_fr.enc.circles), 0)*REDUCTION_RATIO_MUL+0.5f);
  speed_bl = (uint16_t)(update_all(2, TARGET_20HZ_SPEED, get_speed(2, motor_bl.enc.circles), 0)*REDUCTION_RATIO_MUL+0.5f);
  speed_br = (uint16_t)(update_all(3, TARGET_20HZ_SPEED, get_speed(3, motor_br.enc.circles), 0)*REDUCTION_RATIO_MUL+0.5f);
#if PID_VCP_DEBUG
  char uartTxBuf[100];  
  sprintf(uartTxBuf, "fl:%d, fr:%d, bl:%d, br:%d\r\n", speed_fl, speed_fr, speed_bl, speed_br);
  VCPSend((uint8_t *)uartTxBuf, strlen(uartTxBuf));
#endif
}

float update_all(uint8_t who, float target, float measurement, uint8_t limit)
{
  // don't process inf or NaN
  if (!isfinite(target) || !isfinite(measurement)) {
    return 0.0f;
  }

  error_last = _error[who];
  _target[who] += get_filt_T_alpha() * (target - _target[who]);
  _error[who] += get_filt_E_alpha() * ((_target[who] - measurement) - _error[who]);

  // calculate and filter derivative
  if (_dt > 0.0f) {
    float derivative = (_error[who] - error_last) / _dt;
    _derivative[who] += get_filt_D_alpha() * (derivative - _derivative[who]);
  }

  // update I term
  update_i(who, limit);

  float P_out = (_error[who] * _kp);
  float D_out = (_derivative[who] * _kd);

#if PID_VCP_DEBUG && 0
  char uartTxBuf[100];  
  sprintf(uartTxBuf, "who:%d, p:%.2f, i:%.2f, d:%.2f\r\ntar:%.2f, measure:%.2f\r\n", who, P_out, _integrator, D_out, _target, measurement);
  VCPSend((uint8_t *)uartTxBuf, strlen(uartTxBuf));
#endif
  
  return constrain_float(P_out + _integrator[who] + D_out, -50.0f*REDUCTION_RATIO, 50.0f*REDUCTION_RATIO);
}

//  update_i - update the integral
//  If the limit flag is set the integral is only allowed to shrink
void update_i(uint8_t who, uint8_t limit)
{
  float _kimax = 50.0f*REDUCTION_RATIO;
  
  if (!is_zero(_ki) && is_positive(_dt) && key_value != 7) {
    // Ensure that integrator can only be reduced if the output is saturated
    if (!limit || ((is_positive(_integrator[who]) && is_negative(_error[who])) || (is_negative(_integrator[who]) && is_positive(_error[who])))) {
      _integrator[who] += ((float)_error[who] * _ki) * _dt;
      _integrator[who] = constrain_float(_integrator[who], -_kimax, _kimax);
    }
  } else {
    _integrator[who] = 0.0f;
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
