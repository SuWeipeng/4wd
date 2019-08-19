#include "main.h"
#include "fatfs.h"
#include "usb_device.h"
#include "nrf_mavlink.h"
#include "Logger.h"
#include "MY_NRF24.h"
#include <AC_PID.h>
#include <vectorN.h>
#include <AP_Motors.h>

typedef VectorN<float,3> Vector3f;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim12;

AC_PID     *pid_1, *pid_2, *pid_3, *pid_4;
AP_Motors* motor1_fr;
AP_Motors* motor2_fl;
AP_Motors* motor3_bl;
AP_Motors* motor4_br;

#if defined(__GNUC__) && defined(__cplusplus)
extern "C" {
#endif
void setup(void)
{
  pid_1 = new AC_PID(0.045f, 
               0.07f, 
               0.0014f, 
               0.0f,
               95.0f,
               1.0f,
               1.0f,
               0.85f,
               0.02f);
  pid_2 = new AC_PID(0.045f, 
               0.07f, 
               0.0014f, 
               0.0f,
               95.0f,
               1.0f,
               1.0f,
               0.85f,
               0.02f);
  pid_3 = new AC_PID(0.045f, 
               0.07f, 
               0.0014f, 
               0.0f,
               95.0f,
               1.0f,
               1.0f,
               0.85f,
               0.02f);
  pid_4 = new AC_PID(0.045f, 
               0.07f, 
               0.0014f, 
               0.0f,
               95.0f,
               1.0f,
               1.0f,
               0.85f,
               0.02f);
  motor1_fr = new AP_Motors(&htim1,
                            -1,
                            GPIOD,
                            GPIO_PIN_10,
                            GPIO_PIN_8,
                            &htim12,
                            TIM_CHANNEL_1,
                            99,
                            pid_1
                            );
  motor2_fl = new AP_Motors(&htim5,
                            -1,
                            GPIOE,
                            GPIO_PIN_14,
                            GPIO_PIN_12,
                            &htim2,
                            TIM_CHANNEL_3,
                            99,
                            pid_2
                            );
  motor3_bl = new AP_Motors(&htim3,
                            -1,
                            GPIOE,
                            GPIO_PIN_15,
                            GPIO_PIN_13,
                            &htim2,
                            TIM_CHANNEL_4,
                            99,
                            pid_3
                            );
  motor4_br = new AP_Motors(&htim4,
                            -1,
                            GPIOD,
                            GPIO_PIN_11,
                            GPIO_PIN_9,
                            &htim12,
                            TIM_CHANNEL_2,
                            99,
                            pid_4
                            );
}

void loop(void)
{
  motor1_fr->set_rpm(-1500);
//  motor2_fl->set_rpm(-1500);
//  motor3_bl->set_rpm(-1500);
//  motor4_br->set_rpm(-1500);
}

#if defined(__GNUC__) && defined(__cplusplus)
}
#endif
