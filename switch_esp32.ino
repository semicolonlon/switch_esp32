#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <M5Unified.h>
#include <Avatar.h>
#include <faces/DogFace.h>

#include "SwitchControllerESP32.h"
#include "Arduino.h"

// Bluetooth
BLEServer* pServer;
bool deviceConnected = false;
#define DEVICE_NAME "Atom 3S"
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

// Mrs.Stack
m5avatar::Avatar avatar;
m5avatar::Face* faces[2];
m5avatar::ColorPalette* palettes[3];
int faceIdx = 0;

uint16_t pressed_bottons = 0;

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

    tiltJoystick(lx, ly, rx, ry, 100, 0);
  }else{
    releaseButton(static_cast<Button>(pressed_bottons));
    pressed_bottons = 0;
  }
}

class Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) override {
    String value = String((char*)pCharacteristic->getValue().c_str());
    avatar.setSpeechText(value.c_str());
    execution_command(value); 
  }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
    avatar.setFace(faces[1]);
    avatar.setExpression(m5avatar::Expression::Happy);

    palettes[1]->set("primary", TFT_WHITE);
    palettes[1]->set("background", TFT_RED);
    avatar.setColorPalette(*palettes[1]);
    avatar.setSpeechText("Connected!");
    avatar.setExpression(m5avatar::Expression::Happy);
    avatar.setScale(0.4);
    avatar.setPosition(-56, -96);
  }

  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    avatar.setFace(faces[0]);
    avatar.setExpression(m5avatar::Expression::Sad);

    palettes[2]->set("primary", TFT_DARKGREY);
    palettes[2]->set("background", TFT_NAVY);
    avatar.setColorPalette(*palettes[2]);
    avatar.setSpeechText("Oh no…");
    avatar.setScale(0.4);
    avatar.setPosition(-56, -96);

    delay(500);
    pServer->startAdvertising();
  }
};

void setupAvatar() {
  faces[0] = avatar.getFace();
  faces[1] = avatar.getFace();

  palettes[0] = new m5avatar::ColorPalette();
  palettes[1] = new m5avatar::ColorPalette();
  palettes[2] = new m5avatar::ColorPalette();

  palettes[1]->set("primary", TFT_YELLOW);
  palettes[1]->set("background", TFT_DARKCYAN);
  palettes[2]->set("primary", TFT_DARKGREY);
  palettes[2]->set("background", TFT_NAVY);

  avatar.init();
  avatar.setColorPalette(*palettes[0]);
  avatar.setSpeechText("Waiting for connection...");
  avatar.setExpression(m5avatar::Expression::Neutral);
  avatar.setScale(0.4);
  avatar.setPosition(-56, -96);
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

  BLEDevice::init(DEVICE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic* pWriteCharacteristic = pService->createCharacteristic(
    UUID_RX,
    BLECharacteristic::PROPERTY_WRITE);
  pWriteCharacteristic->setCallbacks(new Callbacks());
  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  avatar.setSpeechText("Come on!");
}

void loop() {
  M5.update();
}