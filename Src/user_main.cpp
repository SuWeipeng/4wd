#include "main.h"
#include "fatfs.h"
#include "usb_device.h"
#include "nrf_mavlink.h"
#include "Logger.h"
#include "MY_NRF24.h"

#include <AP_Math/vectorN.h>

typedef VectorN<float,3> Vector3f;

#if defined(__GNUC__) && defined(__cplusplus)
extern "C" {
#endif
void setup(void)
{

}

void loop(void)
{
  Vector3f a,b;
  for(uint8_t i=0; i<3; i++){
    a[i] = i;
    b[i] = i;
  }
  
  float res = a*b;
  
  char uartTxBuf[100];  
  sprintf(uartTxBuf, "res: %.2f\r\n", res);
  VCPSend((uint8_t *)uartTxBuf, strlen(uartTxBuf));
}

#if defined(__GNUC__) && defined(__cplusplus)
}
#endif
