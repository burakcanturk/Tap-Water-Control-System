#include <Arduino.h>
#include "Tap_Water_Control.h"
#include <LiquidCrystal_I2C.h>

Tap_Water_Control::Tap_Water_Control(LiquidCrystal_I2C *lcd_input, int solenoid_valve_pin, int flow_meter_pin) {
  lcd = lcd_input;
  flow_pin = flow_meter_pin;
  solenoid_pin = solenoid_valve_pin;
}

void Tap_Water_Control::lcdStart() {
  lcd->begin();
  lcd->backlight();
}

void Tap_Water_Control::lcdSetPosition(int column, int row) {
  lcd->setCursor(column, row);
}

void Tap_Water_Control::lcdPrint(String text) {
  lcd->print(text);
}

void Tap_Water_Control::lcdPrint(int text) {
  lcd->print(text);
}

void Tap_Water_Control::lcdPrint(unsigned long text) {
  lcd->print(text);
}

void Tap_Water_Control::lcdPrint(float text) {
  lcd->print(text);
}

void Tap_Water_Control::lcdPrint(char text) {
  lcd->print(text);
}

void Tap_Water_Control::lcdClear() {
  lcd->clear();
}

void Tap_Water_Control::flowStart() {
  pinMode(flow_pin, INPUT);
}

bool Tap_Water_Control::flowValue() {
  return digitalRead(flow_pin);
}

/*float Tap_Water_Control::flowRateLevel(int duration = 1000) {

  flow_increment = 0;
  
  unsigned long start_time = millis();

  while (millis() - start_time < duration) {
    bool flow_value = digitalRead(flow_pin);
    flow_increment += flow_value;
  }
  
  float water_flow_rate = flow_increment / calibrationFactor;
  
  return water_flow_rate;
}*/

void Tap_Water_Control::solenoidStart() {
  pinMode(solenoid_pin, OUTPUT);
  digitalWrite(solenoid_pin, HIGH);
}

void Tap_Water_Control::solenoidOpen() {
  digitalWrite(solenoid_pin, LOW);
}

void Tap_Water_Control::selenoidClose() {
  digitalWrite(solenoid_pin, HIGH);
}

void Tap_Water_Control::setSolenoidValue(bool value) {
  digitalWrite(solenoid_pin, !value);
}
