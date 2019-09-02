#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdlib.h>
#include "nrf_mavlink.h"

extern vel_target vel;

extern "C" {

void setup(void)
{

}

void loop(void)
{

}

} // extern "C"

using namespace rtthread;

class tran
{
public:
  void getnumber(int a, int b)
  {
    x = a;
    y = b;
  }
  void out(tran & s)
  {
    rt_kprintf("x = %d, y = %d\n", x, y);
  }
private:
  int x, y;
};

tran s;

int test_cpp(void)
{
  s.getnumber(13, 54);
  s.out(s);

  return 0;
}

MSH_CMD_EXPORT(test_cpp, test cpp);



#define MIN_VALUE                 (1e-4)
#define IS_DOUBLE_ZERO(d)         (abs((int)d) < MIN_VALUE)

double div_func(double x, double y)
{
  if (IS_DOUBLE_ZERO(y)) {
    throw y;                                           /* throw exception */
  }

  return x / y;
}

void throw_exceptions(void *args)
{
  try {
    div_func(6, 3);
    rt_kprintf("there is no err\n");
    div_func(4, 0);                                   /* create exception*/
    rt_kprintf("you can run here\n");
  } catch(double) {                                     /* catch exception */
    rt_kprintf("error of dividing zero\n");
  }
}

MSH_CMD_EXPORT(throw_exceptions, throw cpp exceptions);
