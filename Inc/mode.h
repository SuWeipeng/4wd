#ifndef __MODE_H
#define __MODE_H

#include <stm32f4xx_hal.h>

typedef enum {
  OPEN_LOOP_CONTROL,
  CLOSE_LOOP_CONTROL
}control_mode;

void update_mode(void);

#endif
