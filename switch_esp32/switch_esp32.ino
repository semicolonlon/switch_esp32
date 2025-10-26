#include <WiFi.h>
#include <WiFiUdp.h>
#include <M5Unified.h>
#include <Avatar.h>
#include <faces/DogFace.h>

#include "SwitchControllerESP32.h"
#include "Arduino.h"

const char* ssid = "HR01a-DC45C3";
const char* password = "864a9ac3c3";

WiFiUDP udp;
unsigned int localUdpPort = 12345;
char packetBuffer[255];

// Mrs.Stack
m5avatar::Avatar avatar;
m5avatar::Face* faces[2];
m5avatar::ColorPalette* palettes[3];
int faceIdx = 0;

uint16_t pressed_bottons = 0;

// (この関数は元のコードから変更ありません)
void execution_command(String query) {
  int first_colon = query.indexOf(":");
  int second_colon = query.indexOf(":", first_colon + 1);

  String cmd = query.substring(0, first_colon);
  String arg1 = query.substring(first_colon + 1, second_colon);
  String arg2 = query.substring(second_colon + 1);

  if (cmd == "button") {
    int btn_val = arg1.toInt();
    int push_time = arg2.toInt();
    pushButton2(static_cast<Button>(btn_val), push_time ,0, 1);
  } else if(cmd == "press"){
    int btn_val = arg1.toInt();
    pressButton(static_cast<Button>(btn_val)); 
    pressed_bottons |= btn_val;
  } else if(cmd == "release"){
    int btn_val = arg1.toInt();
    releaseButton(static_cast<Button>(btn_val)); 
    pressed_bottons &= ~btn_val;
  } else if (cmd == "hat") {
    int hat_val = arg1.toInt();
    int delay_ms = arg2.length() > 0 ? arg2.toInt() : 40;
    pushHatButton(static_cast<Hat>(hat_val), delay_ms, 1);
  } else if (cmd == "stick") {
    int colon1 = arg1.indexOf('.');
    int rx = arg1.substring(0, colon1).toInt();
    int ry = arg1.substring(colon1 + 1).toInt();

    int colon2 = arg2.indexOf('.');
    int lx = arg2.substring(0, colon2).toInt();
    int ly = arg2.substring(colon2 + 1).toInt();

    tiltJoystick(lx, ly, rx, ry);
  }else{
    releaseButton(static_cast<Button>(pressed_bottons));
    pressed_bottons = 0;
  }
}
void setupAvatar() {
  faces[0] = avatar.getFace();
  faces[1] = avatar.getFace();

  palettes[0] = new m5avatar::ColorPalette();
  palettes[1] = new m5avatar::ColorPalette();
  palettes[2] = new m5avatar::ColorPalette();

  // (0) 待機中
  palettes[0]->set("primary", TFT_YELLOW);
  palettes[0]->set("background", TFT_DARKCYAN);
  // (1) 接続成功
  palettes[1]->set("primary", TFT_WHITE);
  palettes[1]->set("background", TFT_RED);
  // (2) 接続失敗
  palettes[2]->set("primary", TFT_DARKGREY);
  palettes[2]->set("background", TFT_NAVY);

  avatar.init();
  avatar.setColorPalette(*palettes[0]); // 待機中パレット
  avatar.setSpeechText("Waiting for WiFi...");
  avatar.setExpression(m5avatar::Expression::Neutral);

}

void setup() {
  switchcontrolleresp32_init();
  USB.begin();
  switchcontrolleresp32_reset();

  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Speaker.begin();
  M5.Speaker.setVolume(128);
  setupAvatar();

  Serial.print("Connecting to WiFi ");
  Serial.println(ssid);
  avatar.setSpeechText("Connecting WiFi...");

  WiFi.begin(ssid, password);
  
  int try_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.update();
    Serial.print(".");
    try_count++;
    
    if (try_count > 20) {
        Serial.println("\nWiFi connection failed!");
        avatar.setFace(faces[0]);
        avatar.setExpression(m5avatar::Expression::Sad);
        avatar.setColorPalette(*palettes[2]);
        avatar.setSpeechText("WiFi Failed!");
        while(1) { M5.update(); }
    }
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  avatar.setFace(faces[1]);
  avatar.setExpression(m5avatar::Expression::Happy);
  avatar.setColorPalette(*palettes[1]);
  avatar.setSpeechText(WiFi.localIP().toString().c_str());

  udp.begin(localUdpPort);
  Serial.printf("UDP Listening on port %d\n", localUdpPort);
  avatar.setSpeechText(WiFi.localIP().toString().c_str());

}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, 254);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    
    String commandStr = String(packetBuffer);

    execution_command(commandStr);
  }
}