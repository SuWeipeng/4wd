#include <stdlib.h>
#include "encoder.h"
#include "usb_device.h"

#ifndef NULL
#define NULL                                            0U
#endif /* NULL */

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

static int32_t encoder_cnt[4];
static int32_t last_cnt[4];
static int32_t delta_cnt[4];
static int8_t  direct[4];
static float   circles[4];

void update_delta(void);

// 50Hz update
void update_encoder(void)
{
  encoder_cnt[0] = (uint32_t)(__HAL_TIM_GET_COUNTER(&htim1));
  encoder_cnt[1] = (uint32_t)(__HAL_TIM_GET_COUNTER(&htim3));
  encoder_cnt[2] = (uint32_t)(__HAL_TIM_GET_COUNTER(&htim4));
  encoder_cnt[3] = (uint32_t)(__HAL_TIM_GET_COUNTER(&htim5));

  update_delta();
}

void get_circles(uint8_t who, encoder *encoder_obj)
{
  if(encoder_obj != NULL) {
    switch(who) {
    case 0: {
      encoder_obj->circles = circles[0];
      break;
    }
    case 1: {
      encoder_obj->circles = circles[1];
      break;
    }
    case 2: {
      encoder_obj->circles = circles[2];
      break;
    }
    case 3: {
      encoder_obj->circles = circles[3];
      break;
    }
    default: {
      encoder_obj->circles = 0;
      break;
    }
    }
  }
}

void get_direct(uint8_t who, encoder *encoder_obj)
{
  if(encoder_obj != NULL) {
    switch(who) {
    case 0: {
      encoder_obj->direct = direct[0];
      break;
    }
    case 1: {
      encoder_obj->direct = direct[1];
      break;
    }
    case 2: {
      encoder_obj->direct = direct[2];
      break;
    }
    case 3: {
      encoder_obj->direct = direct[3];
      break;
    }
    default: {
      encoder_obj->direct = 0;
      break;
    }
    }
  }
}

// 50Hz update
void update_delta(void)
{
  uint8_t i;

  for(i=0; i<4; i++) {
    delta_cnt[i] = encoder_cnt[i] - last_cnt[i];

    if(abs(delta_cnt[i]) >= MAX_DELTA_50HZ) {
      int32_t temp;
      if(delta_cnt[i] > 0) {
        temp = COUNTER_PERIOD - encoder_cnt[i];
        delta_cnt[i] = -(last_cnt[i] + temp);
      } else {
        temp = COUNTER_PERIOD - last_cnt[i];
        delta_cnt[i] = encoder_cnt[i] + temp;
      }
    }

    last_cnt[i]  = encoder_cnt[i];

    if(delta_cnt[i] == 0) {
      direct[i] = 0;
    } else if(delta_cnt[i] > 0) {
      direct[i] = 1;
    } else {
      direct[i] = -1;
    }
    
    circles[i] += delta_cnt[i]/ENCODER_LINE;
    if(circles[i] > MAX_CIRCLES){
      circles[i] = 0.0f;
    }
    if(circles[i] < 0.0f){
      circles[i] = MAX_CIRCLES;
    }
    
#if VCP_DEBUG
    char TxBuf[40];
    sprintf(TxBuf, "i:%d, delta:%d, direct:%d, circles:%.2f\r\n", i,  delta_cnt[i], direct[i], circles[i]);
    VCPSend((uint8_t *)TxBuf, strlen(TxBuf));
#endif
  }
}
