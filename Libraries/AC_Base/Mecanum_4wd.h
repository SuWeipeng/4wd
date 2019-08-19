#ifndef __MECANUM_4WD_H__
#define __MECANUM_4WD_H__

#define HALF_BASE_LENGTH_M  0.075f
#define HALF_BASE_WIDTH_M   0.070f
#define WHEEL_DIAMETER_M    0.096f   
#define WHEEL_RADIUS_M      (WHEEL_DIAMETER_M / 2.0f)

class AP_Motors;
class AC_PID;

class Mecanum_4wd
{
public:
  Mecanum_4wd();
  ~Mecanum_4wd();
  
  void run();
  void stop();
  void vel2rpm(float& vel_x, float& vel_y, float& rad_z);

private:
  AP_Motors *_motor1_fr, *_motor2_fl, *_motor3_bl, *_motor4_br;
  AC_PID    *_pid_1, *_pid_2, *_pid_3, *_pid_4;
  
  float     _motor1_fr_rpm, _motor2_fl_rpm, _motor3_bl_rpm, _motor4_br_rpm;
};
#endif /*__MECANUM_4WD_H__*/
