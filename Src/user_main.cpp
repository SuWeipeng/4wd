#include "main.h"
#include "fatfs.h"
#include "usb_device.h"
#include "nrf_mavlink.h"
#include "Logger.h"
#include "MY_NRF24.h"
#include "AC_Base.h"

Mecanum_4wd *base;

#if defined(__GNUC__) && defined(__cplusplus)
extern "C" {
#endif
void setup(void)
{
  base = new Mecanum_4wd();
}

void loop(void)
{
  float vel_x = 0.0f;  // x max 1.33f
  float vel_y = 0.0f;  // y max 1.33f
  float rad_z = 0.1f;  // z max 0.7f
  
  base->vel2rpm(vel_x, vel_y, rad_z);
}

#if defined(__GNUC__) && defined(__cplusplus)
}
#endif
