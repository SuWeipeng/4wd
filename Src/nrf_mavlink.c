#include "nrf_mavlink.h"
#include "MY_NRF24.h"
#include "usb_device.h"
#include "mode.h"

#define NRF_VCP_DEBUG 0

extern control_mode current_mode;

uint8_t key_value = 7;
ap_t    mav_data;

void update_mavlink(void)
{
  uint8_t myTxData[32];
  uint8_t AckPayload[32];
  mavlink_message_t msg;

  mav_data.mode = (uint8_t)current_mode;
  mavlink_msg_simple_pack(0,0,&msg,mav_data.value);
  int len = mavlink_msg_to_send_buffer(myTxData, &msg);

  if(NRF24_write(myTxData, len)) {
    NRF24_read(AckPayload, len);
    uint8_t i;
    mavlink_message_t msg_receive;
    mavlink_status_t mav_status;
    for(i=0; i<len; i++) {
      if(mavlink_parse_char(0, AckPayload[i], &msg_receive, &mav_status)) {
        switch (msg_receive.msgid) {

        case MAVLINK_MSG_ID_SIMPLE: {
          mavlink_simple_t packet;
          mavlink_msg_simple_decode(&msg_receive, &packet);

          key_value = packet.data & 0x07;
          
#if NRF_VCP_DEBUG
          char uartTxBuf[32];
          sprintf(uartTxBuf, "%d\r\n", key_value);
          VCPSend((uint8_t *)uartTxBuf, strlen(uartTxBuf));
#endif
          break;
        }
        }
      }
    }
  }
}