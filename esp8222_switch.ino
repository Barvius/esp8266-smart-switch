#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#define BTN_1 13
#define LED_1 15
#define BTN_2 14
#define LED_2 12

#define MAX_FADE_VALUE 512

unsigned int valLed1 = 0;
boolean dirLed1 = true;
long lastTimeLed1 = 0;
boolean fadeLed1 = false;

unsigned int valLed2 = 0;
boolean dirLed2 = true;
long lastTimeLed2 = 0;
boolean fadeLed2 = false;




void ConnectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void UpdateFirmware() {
  t_httpUpdate_return ret = ESPhttpUpdate.update("192.168.1.141", 80, "/esp8266/", String(ESP.getSketchSize()));
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}

void setFadeLed1(boolean val) {
  fadeLed1 = val;
  if (val) {
    valLed1 = MAX_FADE_VALUE;
  } else {
    analogWrite(LED_1, MAX_FADE_VALUE);
  }
}

boolean isFadeLed1() {
  return fadeLed1;
}

void setFadeLed2(boolean val) {
  fadeLed2 = val;
  if (val) {
    valLed2 = MAX_FADE_VALUE;
  } else {
    analogWrite(LED_2, MAX_FADE_VALUE);
  }
}

boolean isFadeLed2() {
  return fadeLed2;
}

void sendReq(int i, int val) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  //Declare an object of class HTTPClient
    String resp = "set=";
    resp += String(i);
    resp += "&val=";
    resp += String(val);

    http.begin("http://192.168.1.233/io?" + resp);
    http.GET();
    http.end();
  }
}

//int pressedN = 0;
//int button;
//int value;
//boolean sendTo = false;
//
//void btn1() {
//  pressedN++;
//  Serial.println("btn1");
//  if (!isFadeLed1()) {
//    setFadeLed1(true);
//    value = 1;
//  } else {
//    setFadeLed1(false);
//    value = 0;
//  }
//  button = 0;
//  sendTo = true;
//}
//
//void btn2() {
//  pressedN++;
//  Serial.println("btn2");
//  if (!isFadeLed2()) {
//    setFadeLed2(true);
//    value = 1;
//  } else {
//    setFadeLed2(false);
//    value = 0;
//  }
//  button = 1;
//  sendTo = true;
//}



void fadeLed() {
  if (millis() - lastTimeLed1 > 5 && fadeLed1) {
    analogWrite(LED_1, valLed1);
    if (valLed1 <  MAX_FADE_VALUE && dirLed1) {
      valLed1++;
    } else {
      dirLed1 = false;
    }
    if (valLed1 >  0 && !dirLed1) {
      valLed1--;
    } else {
      dirLed1 = true;
    }
    lastTimeLed1 = millis();
  }

  if (millis() - lastTimeLed2 > 5 && fadeLed2) {
    analogWrite(LED_2, valLed2);
    if (valLed2 <  MAX_FADE_VALUE && dirLed2) {
      valLed2++;
    } else {
      dirLed2 = false;
    }
    if (valLed2 >  0 && !dirLed2) {
      valLed2--;
    } else {
      dirLed2 = true;
    }
    lastTimeLed2 = millis();
  }

}
//
//long lastTimeLed = 0;
//long lastBlinkLed = 0;
//
//void blinkLed(){
//  if (millis() - lastTimeLed > 1000) {
//    if (lastBlinkLed == 0 ){
//      analogWrite(LED_2, 0);
//      analogWrite(LED_1, MAX_FADE_VALUE);
//      lastBlinkLed = 1;
//    }
//    if (lastBlinkLed == 1 ){
//      analogWrite(LED_1, 0);
//      analogWrite(LED_2, MAX_FADE_VALUE);
//      lastBlinkLed = 0;
//    }
//    lastTimeLed = millis();
//  }
//}

void getIO() {
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    HTTPClient http;  //Declare an object of class HTTPClient

    http.begin("http://192.168.1.233/io");
    int httpCode = http.GET();                                                                  //Send the request

    if (httpCode == 200 ) { //Check the returning code
      String payload = http.getString();   //Get the request response payload

      Serial.println(payload.substring(1, 2));                    //Print the response payload
      Serial.println(payload.substring(3, 4));

      if (payload.substring(1, 2).equals("1")) {
        if (!isFadeLed1()) {
          setFadeLed1(true);
        }
      } else {
        setFadeLed1(false);
      }

      if (payload.substring(3, 4).equals("1")) {
        if (!isFadeLed2()) {
          setFadeLed2(true);
        }
      } else {
        setFadeLed2(false);
      }

    }

    http.end();   //Close connection

  }
}

void setup() {
  Serial.begin(115200);
  //  blinkLed();
  ConnectWiFi();
  UpdateFirmware();
  getIO();
  pinMode(BTN_1, INPUT);
  //attachInterrupt(digitalPinToInterrupt(BTN_1), btn1, FALLING);

  pinMode(BTN_2, INPUT);
  //attachInterrupt(digitalPinToInterrupt(BTN_2), btn2, FALLING);

  pinMode(LED_1, OUTPUT);
  analogWrite(LED_1, MAX_FADE_VALUE);

  pinMode(LED_2, OUTPUT);
  analogWrite(LED_2, MAX_FADE_VALUE);

}

long lastTimeUpdate = 0;
long lastTimeChesk = 0;


unsigned long timeButton1;
unsigned long durationButton1;
boolean timerButton1;

unsigned long timeButton2;
unsigned long durationButton2;
boolean timerButton2;

void loop() {
  fadeLed();
  if (timerButton1 == 0 && digitalRead(BTN_1) == HIGH) { // button pressed & timer not running already
    timeButton1 = millis();
    timerButton1 = 1;
  }
  if (timerButton1 == 1 && digitalRead(BTN_1) == LOW) { // timer running, button released
    timerButton1 = 0;
    durationButton1 = millis() - timeButton1;
    Serial.print ("button 1 press time in milliseconds: ");
    Serial.println (durationButton1);
  }

  if (timerButton2 == 0 && digitalRead(BTN_2) == HIGH) { // button pressed & timer not running already
    timeButton2 = millis();
    timerButton2 = 1;
  }
  if (timerButton2 == 1 && digitalRead(BTN_2) == LOW) { // timer running, button released
    timerButton2 = 0;
    durationButton2 = millis() - timeButton2;
    Serial.print ("button 2 press time in milliseconds: ");
    Serial.println (durationButton2);
  }

  if (durationButton1 > 5000 && !timerButton1 && durationButton2 > 5000 && !timerButton2) {
    Serial.println("btn");
    durationButton1 = 0;
    durationButton2 = 0;
    delay(500);
    ESP.restart();
  }
  if (durationButton1 > 50 && durationButton1 < 500 && !timerButton1) {
    Serial.println("btn1");
    if (!isFadeLed1()) {
      setFadeLed1(true);
      sendReq(0, 1);
    } else {
      setFadeLed1(false);
      sendReq(0, 0);
    }
    durationButton1 = 0;
  }
  if (durationButton2 > 50 && durationButton2 < 500 && !timerButton2) {
    Serial.println("btn2");
    if (!isFadeLed2()) {
      setFadeLed2(true);
      sendReq(1, 1);
    } else {
      setFadeLed2(false);
      sendReq(1, 0);
    }
    durationButton2 = 0;
  }


  if (millis() - lastTimeUpdate > 10000) {
    getIO();
    lastTimeUpdate = millis();
  }



}
