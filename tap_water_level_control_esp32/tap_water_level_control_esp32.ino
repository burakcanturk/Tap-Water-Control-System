#if defined(ESP32)
#include "FirebaseESP32.h"
#include <WiFi.h>

#elif defined(ESP8266)
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>

#endif

#include "Tap_Water_Control.h"
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#define WIFI_SSID "Your WiFi SSID"
#define WIFI_PASSWORD "Your WiFi Password"

#define FIREBASE_HOST "tap-verileri-default-rtdb.Firebaseio.com"
#define FIREBASE_AUTH "Od8AqgY4L8nwXgnf9TeIgLuZyUJ8yLIUocQWG6Uj"

#define selenoid1 4
#define flow1 5

#define selenoid2 12
#define flow2 13

#define selenoid3 14
#define flow3 15

#define selenoid4 16
#define flow4 17

#define selenoid5 18
#define flow5 19

#define selenoid6 23
#define flow6 25

#define wifi_waiting_time 5000

bool time_set = false;

unsigned long begin_time_save = 0;

LiquidCrystal_I2C lcdMainScreen(0x27, 16, 2);
LiquidCrystal_I2C lcd1(0x26, 16, 2);
LiquidCrystal_I2C lcd2(0x25, 16, 2);
LiquidCrystal_I2C lcd3(0x24, 16, 2);
LiquidCrystal_I2C lcd4(0x23, 16, 2);
LiquidCrystal_I2C lcd5(0x22, 16, 2);
LiquidCrystal_I2C lcd6(0x21, 16, 2);

Tap_Water_Control tap1(&lcd1, selenoid1, flow1);
Tap_Water_Control tap2(&lcd2, selenoid2, flow2);
Tap_Water_Control tap3(&lcd3, selenoid3, flow3);
Tap_Water_Control tap4(&lcd4, selenoid4, flow4);
Tap_Water_Control tap5(&lcd5, selenoid5, flow5);
Tap_Water_Control tap6(&lcd6, selenoid6, flow6);

unsigned long tap1_spent_milliliters = 0;
unsigned long tap2_spent_milliliters = 0;
unsigned long tap3_spent_milliliters = 0;
unsigned long tap4_spent_milliliters = 0;
unsigned long tap5_spent_milliliters = 0;
unsigned long tap6_spent_milliliters = 0;

unsigned long tap1_remaining_milliliters = 0;
unsigned long tap2_remaining_milliliters = 0;
unsigned long tap3_remaining_milliliters = 0;
unsigned long tap4_remaining_milliliters = 0;
unsigned long tap5_remaining_milliliters = 0;
unsigned long tap6_remaining_milliliters = 0;

unsigned long tap1_remaining_milliliters_before = 0;
unsigned long tap2_remaining_milliliters_before = 0;
unsigned long tap3_remaining_milliliters_before = 0;
unsigned long tap4_remaining_milliliters_before = 0;
unsigned long tap5_remaining_milliliters_before = 0;
unsigned long tap6_remaining_milliliters_before = 0;

bool tap0_close = false;
bool tap1_close = false;
bool tap2_close = false;
bool tap3_close = false;
bool tap4_close = false;
bool tap5_close = false;
bool tap6_close = false;

bool tap0_time_set = false;
bool tap1_time_set = false;
bool tap2_time_set = false;
bool tap3_time_set = false;
bool tap4_time_set = false;
bool tap5_time_set = false;
bool tap6_time_set = false;

unsigned int tap1_total_milliliters = 500;
unsigned int tap2_total_milliliters = 500;
unsigned int tap3_total_milliliters = 500;
unsigned int tap4_total_milliliters = 500;
unsigned int tap5_total_milliliters = 500;
unsigned int tap6_total_milliliters = 500;

unsigned int time_day = 0, time_hour = 0, time_minute = 3, time_second = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
RTC_DS1307 rtc;

DateTime tap_next_repeat;

byte save_row = 0;
byte total_saves = 0;

unsigned long time_ = 0;

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;

bool wifi_connected = true;

void setup() {

  lcdMainScreen.begin();
  lcdMainScreen.backlight();

  lcdMainScreen.home();
  lcdMainScreen.print("WiFi connecting");
  lcdMainScreen.setCursor(0, 1);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long begin_time = millis();

  while (WiFi.status() != WL_CONNECTED and millis() - begin_time < wifi_waiting_time)
  {
    lcdMainScreen.print(".");
    delay(500);
  }

  //wifi_connected = millis() - begin_time_ = < wifi_waiting_time_ =;
  wifi_connected = WiFi.status() == WL_CONNECTED;

  lcdMainScreen.clear();

  if (wifi_connected) {
    lcdMainScreen.home();
    lcdMainScreen.print("Connected!");
  }

  else {

    rtc.begin();

    lcdMainScreen.home();
    lcdMainScreen.print("Not Connected!");
    lcdMainScreen.setCursor(0, 1);
    lcdMainScreen.print("Started no wifi");
  }

  delay(1000);

  lcdMainScreen.clear();

  if (wifi_connected) {
    firebaseConfig.database_url = FIREBASE_HOST; // Replace with your Firebase host
    firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH; // Replace with your Firebase Databegine Secret
    Firebase.begin(&firebaseConfig, NULL);
    Firebase.reconnectWiFi(true);
  }

  Serial.begin(115200);

  timeClient.begin();
  timeClient.setTimeOffset(2); // RTC +2
  //rtc.begin();

  tap1.lcdStart();
  tap2.lcdStart();
  tap3.lcdStart();
  tap4.lcdStart();
  tap5.lcdStart();
  tap6.lcdStart();

  tap1.flowStart();
  tap2.flowStart();
  tap3.flowStart();
  tap4.flowStart();
  tap5.flowStart();
  tap6.flowStart();

  tap1.solenoidStart();
  tap2.solenoidStart();
  tap3.solenoidStart();
  tap4.solenoidStart();
  tap5.solenoidStart();
  tap6.solenoidStart();

  tap1.selenoidClose();
  tap2.selenoidClose();
  tap3.selenoidClose();
  tap4.selenoidClose();
  tap5.selenoidClose();
  tap6.selenoidClose();

  tap1.lcdSetPosition(0, 0);
  tap1.lcdPrint("Tap");
  tap1.lcdSetPosition(0, 1);
  tap1.lcdPrint(1);

  tap2.lcdSetPosition(0, 0);
  tap2.lcdPrint("Tap");
  tap2.lcdSetPosition(0, 1);
  tap2.lcdPrint(2);

  tap3.lcdSetPosition(0, 0);
  tap3.lcdPrint("Tap");
  tap3.lcdSetPosition(0, 1);
  tap3.lcdPrint(3);

  tap4.lcdSetPosition(0, 0);
  tap4.lcdPrint("Tap");
  tap4.lcdSetPosition(0, 1);
  tap4.lcdPrint(4);

  tap5.lcdSetPosition(0, 0);
  tap5.lcdPrint("Tap");
  tap5.lcdSetPosition(0, 1);
  tap5.lcdPrint(5);

  tap6.lcdSetPosition(0, 0);
  tap6.lcdPrint("Tap");
  tap6.lcdSetPosition(0, 1);
  tap6.lcdPrint(6);

  lcdMainScreen.clear();
  tap1.lcdClear();
  tap2.lcdClear();
  tap3.lcdClear();
  tap4.lcdClear();
  tap5.lcdClear();
  tap6.lcdClear();

  if (wifi_connected) {

    if (Firebase.getInt(firebaseData, "/tap1_Total")) {
      tap1_total_milliliters = firebaseData.intData();
      //Serial.println("tap1 Alindi");
    }

    if (Firebase.getInt(firebaseData, "/tap2_Total")) {
      tap2_total_milliliters = firebaseData.intData();
      //Serial.println("tap2 Alindi");
    }

    if (Firebase.getInt(firebaseData, "/tap3_Total")) {
      tap3_total_milliliters = firebaseData.intData();
      //Serial.println("tap3 Alindi");
    }

    if (Firebase.getInt(firebaseData, "/tap4_Total")) {
      tap4_total_milliliters = firebaseData.intData();
      //Serial.println("tap4 Alindi");
    }

    if (Firebase.getInt(firebaseData, "/tap5_Total")) {
      tap5_total_milliliters = firebaseData.intData();
      //Serial.println("tap5 Alindi");
    }

    if (Firebase.getInt(firebaseData, "/tap6_Total")) {
      tap6_total_milliliters = firebaseData.intData();
      //Serial.println("tap6 Alindi");
    }

    if (Firebase.getInt(firebaseData, "/tap1_remaining")) {
      tap1_remaining_milliliters = firebaseData.intData();
      tap1_spent_milliliters = tap1_total_milliliters - tap1_remaining_milliliters;
    }

    if (Firebase.getInt(firebaseData, "/tap2_remaining")) {
      tap2_remaining_milliliters = firebaseData.intData();
      tap2_spent_milliliters = tap2_total_milliliters - tap2_remaining_milliliters;
    }

    if (Firebase.getInt(firebaseData, "/tap3_remaining")) {
      tap3_remaining_milliliters = firebaseData.intData();
      tap3_spent_milliliters = tap3_total_milliliters - tap3_remaining_milliliters;
    }

    if (Firebase.getInt(firebaseData, "/tap4_remaining")) {
      tap4_remaining_milliliters = firebaseData.intData();
      tap4_spent_milliliters = tap4_total_milliliters - tap4_remaining_milliliters;
    }

    if (Firebase.getInt(firebaseData, "/tap5_remaining")) {
      tap5_remaining_milliliters = firebaseData.intData();
      tap5_spent_milliliters = tap5_total_milliliters - tap5_remaining_milliliters;
    }

    if (Firebase.getInt(firebaseData, "/tap6_remaining")) {
      tap6_remaining_milliliters = firebaseData.intData();
      tap6_spent_milliliters = tap6_total_milliliters - tap6_remaining_milliliters;
    }

    timeClient.update();
    unsigned long epochtime = timeClient.getEpochTime();
    DateTime now_ = DateTime(epochtime);

    if (Firebase.getInt(firebaseData, "/time")) {
      time_ = firebaseData.intData();
    }

    if (time_ == 0 or time_ < now_.unixtime()) {

      tap_next_repeat = now_ + TimeSpan(time_day, time_hour, time_minute, time_second);

      //Serial.println(tap_next_repeat.unixtime());

      time_ = tap_next_repeat.unixtime();

      Firebase.setInt(firebaseData, "/time", time_);

      /*if (Firebase.getInt(FirebaseData, "/time")) {
        time = FirebaseData.intData();
        }*/

      //Serial.println(tap6_total_milliliters);

      tap1_spent_milliliters = 0;
      tap2_spent_milliliters = 0;
      tap3_spent_milliliters = 0;
      tap4_spent_milliliters = 0;
      tap5_spent_milliliters = 0;
      tap6_spent_milliliters = 0;

      tap1_remaining_milliliters = tap1_total_milliliters;
      tap2_remaining_milliliters = tap2_total_milliliters;
      tap3_remaining_milliliters = tap3_total_milliliters;
      tap4_remaining_milliliters = tap4_total_milliliters;
      tap5_remaining_milliliters = tap5_total_milliliters;
      tap6_remaining_milliliters = tap6_total_milliliters;

      Firebase.setInt(firebaseData, "/tap1_remaining", tap1_remaining_milliliters);
      Firebase.setInt(firebaseData, "/tap2_remaining", tap2_remaining_milliliters);
      Firebase.setInt(firebaseData, "/tap3_remaining", tap3_remaining_milliliters);
      Firebase.setInt(firebaseData, "/tap4_remaining", tap4_remaining_milliliters);
      Firebase.setInt(firebaseData, "/tap5_remaining", tap5_remaining_milliliters);
      Firebase.setInt(firebaseData, "/tap6_remaining", tap6_remaining_milliliters);
    }

    else {
      tap_next_repeat = DateTime(time_);
    }
  }

  tap1.solenoidOpen();
  tap2.solenoidOpen();
  tap3.solenoidOpen();
  tap4.solenoidOpen();
  tap5.solenoidOpen();
  tap6.solenoidOpen();
}

void loop() {

  DateTime now_;

  if (wifi_connected) {
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    now_ = DateTime(epochTime);
  }

  else {
    now_ = rtc.now();
  }

  if (now_ >= tap_next_repeat or now_.unixtime() == 0) {

    tap1.selenoidClose();
    tap2.selenoidClose();
    tap3.selenoidClose();
    tap4.selenoidClose();
    tap5.selenoidClose();
    tap6.selenoidClose();

    lcdMainScreen.clear();
    tap1.lcdClear();
    tap2.lcdClear();
    tap3.lcdClear();
    tap4.lcdClear();
    tap5.lcdClear();
    tap6.lcdClear();

    unsigned long time_ = now_.unixtime();

    String save_part = "/Kayitlar/" + String(time_);

    if (wifi_connected) {

      Firebase.setInt(firebaseData, save_part + "/tap_sayisi", 6);

      Firebase.setInt(firebaseData, save_part + "/time", time_);

      Firebase.setInt(firebaseData, save_part + "/tap1_remaining", tap1_remaining_milliliters);
      Firebase.setInt(firebaseData, save_part + "/tap1_Total", tap1_total_milliliters);

      Firebase.setInt(firebaseData, save_part + "/tap2_remaining", tap2_remaining_milliliters);
      Firebase.setInt(firebaseData, save_part + "/tap2_Total", tap2_total_milliliters);

      Firebase.setInt(firebaseData, save_part + "/tap3_remaining", tap3_remaining_milliliters);
      Firebase.setInt(firebaseData, save_part + "/tap3_Total", tap3_total_milliliters);

      Firebase.setInt(firebaseData, save_part + "/tap4_remaining", tap4_remaining_milliliters);
      Firebase.setInt(firebaseData, save_part + "/tap4_Total", tap4_total_milliliters);

      Firebase.setInt(firebaseData, save_part + "/tap5_remaining", tap5_remaining_milliliters);
      Firebase.setInt(firebaseData, save_part + "/tap5_Total", tap5_total_milliliters);

      Firebase.setInt(firebaseData, save_part + "/tap6_remaining", tap6_remaining_milliliters);
      Firebase.setInt(firebaseData, save_part + "/tap6_Total", tap6_total_milliliters);
    }

    tap1_spent_milliliters = 0;
    tap2_spent_milliliters = 0;
    tap3_spent_milliliters = 0;
    tap4_spent_milliliters = 0;
    tap5_spent_milliliters = 0;
    tap6_spent_milliliters = 0;

    tap1_remaining_milliliters = tap1_total_milliliters;
    tap2_remaining_milliliters = tap2_total_milliliters;
    tap3_remaining_milliliters = tap3_total_milliliters;
    tap4_remaining_milliliters = tap4_total_milliliters;
    tap5_remaining_milliliters = tap5_total_milliliters;
    tap6_remaining_milliliters = tap6_total_milliliters;

    tap_next_repeat = now_ + TimeSpan(time_day, time_hour, time_minute, time_second);
    if (wifi_connected)
      Firebase.setInt(firebaseData, "/time", tap_next_repeat.unixtime());

    tap1.solenoidOpen();
    tap2.solenoidOpen();
    tap3.solenoidOpen();
    tap4.solenoidOpen();
    tap5.solenoidOpen();
    tap6.solenoidOpen();
  }

  TimeSpan tap_remaining_time = tap_next_repeat - now_;

  bool flow1_value = tap1.flowValue();
  bool flow2_value = tap2.flowValue();
  bool flow3_value = tap3.flowValue();
  bool flow4_value = tap4.flowValue();
  bool flow5_value = tap5.flowValue();
  bool flow6_value = tap6.flowValue();

  unsigned int flow1_increase = 0;
  unsigned int flow2_increase = 0;
  unsigned int flow3_increase = 0;
  unsigned int flow4_increase = 0;
  unsigned int flow5_increase = 0;
  unsigned int flow6_increase = 0;

  bool flow1_value_before = false;
  bool flow2_value_before = false;
  bool flow3_value_before = false;
  bool flow4_value_before = false;
  bool flow5_value_before = false;
  bool flow6_value_before = false;

  unsigned long beginlangic_timei = millis();

  while (millis() - beginlangic_timei < 1000) {

    flow1_value = tap1.flowValue();
    flow2_value = tap2.flowValue();
    flow3_value = tap3.flowValue();
    flow4_value = tap4.flowValue();
    flow5_value = tap5.flowValue();
    flow6_value = tap6.flowValue();

    flow1_increase += flow1_value_before and not flow1_value;
    flow2_increase += flow2_value_before and not flow2_value;
    flow3_increase += flow3_value_before and not flow3_value;
    flow4_increase += flow4_value_before and not flow4_value;
    flow5_increase += flow5_value_before and not flow5_value;
    flow6_increase += flow6_value_before and not flow6_value;

    flow1_value_before = flow1_value;
    flow2_value_before = flow2_value;
    flow3_value_before = flow3_value;
    flow4_value_before = flow4_value;
    flow5_value_before = flow5_value;
    flow6_value_before = flow6_value;
  }

  float water_flow_speed1 = flow1_increase / tap1.calibrationFactor;
  float water_flow_speed2 = flow2_increase / tap2.calibrationFactor;
  float water_flow_speed3 = flow3_increase / tap3.calibrationFactor;
  float water_flow_speed4 = flow4_increase / tap4.calibrationFactor;
  float water_flow_speed5 = flow5_increase / tap5.calibrationFactor;
  float water_flow_speed6 = flow6_increase / tap6.calibrationFactor;

  tap1_spent_milliliters += (water_flow_speed1 / 60) * 1000;
  tap2_spent_milliliters += (water_flow_speed2 / 60) * 1000;
  tap3_spent_milliliters += (water_flow_speed3 / 60) * 1000;
  tap4_spent_milliliters += (water_flow_speed4 / 60) * 1000;
  tap5_spent_milliliters += (water_flow_speed5 / 60) * 1000;
  tap6_spent_milliliters += (water_flow_speed6 / 60) * 1000;

  tap1_spent_milliliters = tap1_spent_milliliters < tap1_total_milliliters ? tap1_spent_milliliters : tap1_total_milliliters;
  tap2_spent_milliliters = tap2_spent_milliliters < tap2_total_milliliters ? tap2_spent_milliliters : tap2_total_milliliters;
  tap3_spent_milliliters = tap3_spent_milliliters < tap3_total_milliliters ? tap3_spent_milliliters : tap3_total_milliliters;
  tap4_spent_milliliters = tap4_spent_milliliters < tap4_total_milliliters ? tap4_spent_milliliters : tap4_total_milliliters;
  tap5_spent_milliliters = tap5_spent_milliliters < tap5_total_milliliters ? tap5_spent_milliliters : tap5_total_milliliters;
  tap6_spent_milliliters = tap6_spent_milliliters < tap6_total_milliliters ? tap6_spent_milliliters : tap6_total_milliliters;

  tap1.setSolenoidValue(tap1_spent_milliliters < tap1_total_milliliters);
  tap2.setSolenoidValue(tap2_spent_milliliters < tap2_total_milliliters);
  tap3.setSolenoidValue(tap3_spent_milliliters < tap3_total_milliliters);
  tap4.setSolenoidValue(tap4_spent_milliliters < tap4_total_milliliters);
  tap5.setSolenoidValue(tap5_spent_milliliters < tap5_total_milliliters);
  tap6.setSolenoidValue(tap6_spent_milliliters < tap6_total_milliliters);

  lcdMainScreen.setCursor(0, 0);
  lcdMainScreen.print("Yenilenmeye");
  lcdMainScreen.setCursor(0, 1);
  lcdMainScreen.print("remaining: ");
  lcdMainScreen.print(tap_remaining_time.minutes() < 10 ? "0" : "");
  lcdMainScreen.print(tap_remaining_time.minutes());
  lcdMainScreen.print(":");
  lcdMainScreen.print(tap_remaining_time.seconds() < 10 ? "0" : "");
  lcdMainScreen.print(tap_remaining_time.seconds());

  tap1_remaining_milliliters = tap1_total_milliliters - tap1_spent_milliliters;

  if (String(tap1_remaining_milliliters).length() < String(tap1_remaining_milliliters_before).length()) {
    tap1.lcdClear();
  }

  tap1.lcdSetPosition(0, 0);
  tap1.lcdPrint("Total: ");
  tap1.lcdPrint(tap1_spent_milliliters);
  tap1.lcdPrint(" mL");
  tap1.lcdSetPosition(0, 1);
  tap1.lcdPrint("remaining: ");
  tap1.lcdPrint(tap1_remaining_milliliters);
  tap1.lcdPrint(" mL");
  tap1.lcdSetPosition(14, 1);
  tap1.lcdPrint("M1");

  //----------------------------------------------------------------------------

  tap2_remaining_milliliters = tap2_total_milliliters - tap2_spent_milliliters;

  if (String(tap2_remaining_milliliters).length() < String(tap2_remaining_milliliters_before).length()) {
    tap2.lcdClear();
  }

  tap2.lcdSetPosition(0, 0);
  tap2.lcdPrint("Total: ");
  tap2.lcdPrint(tap2_spent_milliliters);
  tap2.lcdPrint(" mL");
  tap2.lcdSetPosition(0, 1);
  tap2.lcdPrint("remaining: ");
  tap2.lcdPrint(tap2_remaining_milliliters);
  tap2.lcdPrint(" mL");
  tap2.lcdSetPosition(14, 1);
  tap2.lcdPrint("M2");

  //----------------------------------------------------------------------------

  tap3_remaining_milliliters = tap3_total_milliliters - tap3_spent_milliliters;

  if (String(tap3_remaining_milliliters).length() < String(tap3_remaining_milliliters_before).length()) {
    tap3.lcdClear();
  }

  tap3.lcdSetPosition(0, 0);
  tap3.lcdPrint("Total: ");
  tap3.lcdPrint(tap3_spent_milliliters);
  tap3.lcdPrint(" mL");
  tap3.lcdSetPosition(0, 1);
  tap3.lcdPrint("remaining: ");
  tap3.lcdPrint(tap3_remaining_milliliters);
  tap3.lcdPrint(" mL");
  tap3.lcdSetPosition(14, 1);
  tap3.lcdPrint("M3");

  //----------------------------------------------------------------------------

  tap4_remaining_milliliters = tap4_total_milliliters - tap4_spent_milliliters;

  if (String(tap4_remaining_milliliters).length() < String(tap4_remaining_milliliters_before).length()) {
    tap4.lcdClear();
  }

  tap4.lcdSetPosition(0, 0);
  tap4.lcdPrint("Total: ");
  tap4.lcdPrint(tap4_spent_milliliters);
  tap4.lcdPrint(" mL");
  tap4.lcdSetPosition(0, 1);
  tap4.lcdPrint("remaining: ");
  tap4.lcdPrint(tap4_remaining_milliliters);
  tap4.lcdPrint(" mL");
  tap4.lcdSetPosition(14, 1);
  tap4.lcdPrint("M4");

  //----------------------------------------------------------------------------

  tap5_remaining_milliliters = tap5_total_milliliters - tap5_spent_milliliters;

  if (String(tap5_remaining_milliliters).length() < String(tap5_remaining_milliliters_before).length()) {
    tap5.lcdClear();
  }

  tap5.lcdSetPosition(0, 0);
  tap5.lcdPrint("Total: ");
  tap5.lcdPrint(tap5_spent_milliliters);
  tap5.lcdPrint(" mL");
  tap5.lcdSetPosition(0, 1);
  tap5.lcdPrint("remaining: ");
  tap5.lcdPrint(tap5_remaining_milliliters);
  tap5.lcdPrint(" mL");
  tap5.lcdSetPosition(14, 1);
  tap5.lcdPrint("M5");

  //----------------------------------------------------------------------------

  tap6_remaining_milliliters = tap6_total_milliliters - tap6_spent_milliliters;

  if (String(tap6_remaining_milliliters).length() < String(tap6_remaining_milliliters_before).length()) {
    tap6.lcdClear();
  }

  tap6.lcdSetPosition(0, 0);
  tap6.lcdPrint("Total: ");
  tap6.lcdPrint(tap6_spent_milliliters);
  tap6.lcdPrint(" mL");
  tap6.lcdSetPosition(0, 1);
  tap6.lcdPrint("remaining: ");
  tap6.lcdPrint(tap6_remaining_milliliters);
  tap6.lcdPrint(" mL");
  tap6.lcdSetPosition(14, 1);
  tap6.lcdPrint("M6");

  //----------------------------------------------------------------------------

  if (wifi_connected) {

    if (tap1_remaining_milliliters_before != tap1_remaining_milliliters) {
      Firebase.setInt(firebaseData, "/tap1_remaining", tap1_remaining_milliliters);
    }

    if (tap2_remaining_milliliters_before != tap2_remaining_milliliters) {
      Firebase.setInt(firebaseData, "/tap2_remaining", tap2_remaining_milliliters);
    }

    if (tap3_remaining_milliliters_before != tap3_remaining_milliliters) {
      Firebase.setInt(firebaseData, "/tap3_remaining", tap3_remaining_milliliters);
    }

    if (tap4_remaining_milliliters_before != tap4_remaining_milliliters) {
      Firebase.setInt(firebaseData, "/tap4_remaining", tap4_remaining_milliliters);
    }

    if (tap5_remaining_milliliters_before != tap5_remaining_milliliters) {
      Firebase.setInt(firebaseData, "/tap5_remaining", tap5_remaining_milliliters);
    }

    if (tap6_remaining_milliliters_before != tap6_remaining_milliliters) {
      Firebase.setInt(firebaseData, "/tap6_remaining", tap6_remaining_milliliters);
    }
  }

  tap1_remaining_milliliters_before = tap1_remaining_milliliters;
  tap2_remaining_milliliters_before = tap2_remaining_milliliters;
  tap3_remaining_milliliters_before = tap3_remaining_milliliters;
  tap4_remaining_milliliters_before = tap4_remaining_milliliters;
  tap5_remaining_milliliters_before = tap5_remaining_milliliters;
  tap6_remaining_milliliters_before = tap6_remaining_milliliters;
}
