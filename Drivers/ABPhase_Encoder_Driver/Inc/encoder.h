#ifndef __ENCODER_H
#define __ENCODER_H

#include <stm32f4xx_hal.h>

#define ENCODER_LINE    30.0f
#define MAX_DELTA_50HZ  200
#define COUNTER_PERIOD  65535
#define MAX_CIRCLES     1.0e5f
#define VCP_DEBUG       0

typedef struct ab_phase_encoder
{
  float   circles;
  int8_t  direct;
}encoder;

void     update_encoder     (void);
void     get_circles        (uint8_t who, encoder *encoder_obj);
void     get_direct         (uint8_t who, encoder *encoder_obj);

#endif
