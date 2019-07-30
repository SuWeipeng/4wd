#ifndef __MODE_H
#define __MODE_H

#include <stm32f4xx_hal.h>

typedef enum {
  LOW_SPEED_MODE,
  MIDDLE_SPEED_MODE,
  HIGH_SPEED_MODE
}control_mode;

void Mode_Control(void);

#endif