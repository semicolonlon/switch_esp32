#include "SwitchControllerESP32.h"

const uint16_t BUTTON_PUSHING_MSEC = 40;

void switchcontrolleresp32_init(void)
{
  _button_pushing_msec = BUTTON_PUSHING_MSEC;
  SwitchController().begin();
}

void switchcontrolleresp32_init(uint16_t button_pushing_msec)
{
  _button_pushing_msec = button_pushing_msec;
  SwitchController().begin();
}

void switchcontrolleresp32_reset(void)
{
  SwitchController().setStickTiltRatio(0,0,0,0);
  SwitchController().releaseHatButton();
}

void pushButton(Button button, int delay_after_pushing_msec, int loop_num = 1)
{
  for (int i = 0; i < loop_num; i++)
  {
    SwitchController().pressButton(button);
    delay(_button_pushing_msec);
    SwitchController().releaseButton(button);
    delay(delay_after_pushing_msec);
  }
}

void pushButton2(Button button, int pushing_time_msec, int delay_after_pushing_msec, int loop_num = 1)
{
  for (int i = 0; i < loop_num; i++)
  {
    SwitchController().pressButton(button);
    delay(pushing_time_msec);
    SwitchController().releaseButton(button);
    delay(delay_after_pushing_msec);
  }
}

void pushHatButton(Hat button, int delay_after_pushing_msec, int loop_num = 1)
{
  for (int i = 0; i < loop_num; i++)
  {
    SwitchController().pressHatButton(button);
    delay(_button_pushing_msec);
    SwitchController().releaseHatButton();
    delay(delay_after_pushing_msec);
  }
}

void pushHatButtonContinuous(Hat button, int pushing_time_msec)
{
  SwitchController().pressHatButton(button);
  delay(pushing_time_msec);
  SwitchController().releaseHatButton();
  delay(_button_pushing_msec);
}

void UseLStick(LS Lstick, int tilt_time_msec, int delay_after_tilt_msec)
{
  int lx_per = 0;
  int ly_per = 0;
  int rx_per = 0;
  int ry_per = 0;
  switch (Lstick)
  {
    case LS::LS_DOWN:
      ly_per = 100;
      break;
    case LS::LS_UP:
      ly_per = -100;
      break;
    case LS::LS_LEFT:
      lx_per = -100;
      break;
    case LS::LS_RIGHT:
      lx_per = 100;
      break;
    default:
      break;
  }
  tiltJoystick(lx_per, ly_per, rx_per, ry_per, tilt_time_msec, delay_after_tilt_msec);
}

void UseRStick(RS Rstick, int tilt_time_msec, int delay_after_tilt_msec)
{
  int lx_per = 0;
  int ly_per = 0;
  int rx_per = 0;
  int ry_per = 0;
  switch (Rstick)
  {
    case RS::RS_DOWN:
      ry_per = 100;
      break;
    case RS::RS_UP:
      ry_per = -100;
      break;
    case RS::RS_LEFT:
      rx_per = -100;
      break;
    case RS::RS_RIGHT:
      rx_per = 100;
      break;
    default:
      break;
  }
  tiltJoystick(lx_per, ly_per, rx_per, ry_per, tilt_time_msec, delay_after_tilt_msec);
}

void TiltLeftStick(int direction_deg, double power, int holdtime, int delaytime)
{
  double rad = (double)direction_deg * PI / 180.0; // 弧度法(ラジアン)変換
  //int x, y;
  int lx_per;
  int ly_per;
  lx_per = (double)sin(rad) * power * 100;
  ly_per = (double) - 1 * cos(rad) * power * 100;
  SwitchController().setStickTiltRatio(lx_per, ly_per, 0, 0);
  if (holdtime > 0) { // holdtime=0のときは押しっぱなし。
    delay(holdtime);
    //SwitchControlLibrary().MoveLeftStick(128, 128); // 傾きを直す
    SwitchController().setStickTiltRatio(0, 0, 0, 0);
  }
  if (delaytime > 0) delay(delaytime);
  return;
}

/**
  @brief Switchのジョイスティックの倒し量を設定する

  @param lx_per                LスティックのX方向倒し量[％] -100~100の範囲で設定
  @param ly_per                LスティックのY方向倒し量[％] -100~100の範囲で設定
  @param rx_per                RスティックのX方向倒し量[％] -100~100の範囲で設定
  @param ry_per                RスティックのY方向倒し量[％] -100~100の範囲で設定
  @param tilt_time_msec        スティックを倒し続ける時間
  @param delay_after_tilt_msec スティックを倒した後の待ち時間
*/
void tiltJoystick(int lx_per, int ly_per, int rx_per, int ry_per, int tilt_time_msec, int delay_after_tilt_msec)
{
  SwitchController().setStickTiltRatio(lx_per, ly_per, rx_per, ry_per);
  delay(tilt_time_msec);
  if (delay_after_tilt_msec > 0)
  {
    SwitchController().setStickTiltRatio(0, 0, 0, 0);
    delay(delay_after_tilt_msec);
  }
}

void sendReportOnly(USB_JoystickReport_Input_t t_joystickInputData)
{
  SwitchController().sendReportOnly(t_joystickInputData);
}

void pressButton(Button button) {
    SwitchController().pressButton(button);
}

void releaseButton(Button button) {
    SwitchController().releaseButton(button);
}
