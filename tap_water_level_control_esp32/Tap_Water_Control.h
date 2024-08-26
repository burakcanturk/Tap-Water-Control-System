#ifndef _Tap_Water_Control_H_
#define _Tap_Water_Control_H_

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Tap_Water_Control {
  
  public:
  
    Tap_Water_Control(LiquidCrystal_I2C *lcd_input, int flow_meter_pin, int solenoid_valve_pin);
    
    void lcdStart();
    void lcdSetPosition(int column, int row);
    void lcdPrint(String text);
    void lcdPrint(int text);
    void lcdPrint(unsigned long text);
    void lcdPrint(float text);
    void lcdPrint(char text);
    void lcdClear();
    
    void flowStart();

    bool flowValue();
    float flowRateLevel(int duration = 1000);
    
    void solenoidStart();
    
    void solenoidOpen();
    void selenoidClose();
    void setSolenoidValue(bool value);

    float calibrationFactor = 4.5;
    
  private:
    LiquidCrystal_I2C *lcd;
    int flow_pin;
    int solenoid_pin;
    unsigned long flow_increment;
};

#endif
