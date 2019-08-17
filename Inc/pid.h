#ifndef __PID_H
#define __PID_H
#include <stdint.h>

void  pid_control(void);
float update_all(uint8_t who, float target, float measurement, uint8_t limit);
float constrain_float(float input, float lim_down, float lim_up);
uint8_t is_positive(float input);
uint8_t is_negative(float input);
uint8_t is_zero(float input);

#endif
