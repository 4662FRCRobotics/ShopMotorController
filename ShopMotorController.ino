/*
outline shop test motor controller
*/

#include <Servo.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C display(0x3F,20,4);

int kPIN_THROTTLE = A0;
int kPIN_DIRECTION = 2;
int kPIN_ON = 8;
int kPIN_PWM_OUT = 5;
int kDISPLAY_LINE = 0;

Servo SpeedControl;

int kI_THROTTLE_MAX = 500;
double kD_THROTTLE_MAX = kI_THROTTLE_MAX;

int iThrottleVal = 0;
double dThrottleVal = 0.0;
int k_iTHROTTLE_ZERO = 1500;

int iIsFwd = 0;
int iIsOn = 0;

void setup() {
 
    display.init();
    display.clear();
    display.setBacklight(200);

    SpeedControl.attach(kPIN_PWM_OUT);
    pinMode(kPIN_DIRECTION, INPUT_PULLUP);
    pinMode(kPIN_ON, INPUT_PULLUP);

}

void loop() {
    /* 
    read speed control on/off switch
    set the appropriate literal
    and if speed control is off, read the direction switch
    DO NOT CHANGE MOTOR DIRECTION WHILE RUNNING
    */
    iIsOn = digitalRead(kPIN_ON);
    if (iIsOn == LOW) {
        iIsFwd = digitalRead(kPIN_DIRECTION);
    } else {

    }

    /*
    read speed control potentiometer and convert to throttle range limit
    convert to double for calcs
    */
    iThrottleVal = map(analogRead(kPIN_THROTTLE),0,1023,0,kI_THROTTLE_MAX);
    dThrottleVal = iThrottleVal;
    
    /*
    PWM speed control signal is in the range of 1000 to 2000
    where 1000 is full reverse, 1500 is stop, and 2000 is full forward
    use integers for the PWM control
    translate to range of 1 to -1 for use in WPILib speed range programming
    */
    if (iIsFwd == HIGH) {
        iThrottleVal = k_iTHROTTLE_ZERO + iThrottleVal;
        dThrottleVal = 0.0 - (dThrottleVal / kD_THROTTLE_MAX);
    } else {
        iThrottleVal = k_iTHROTTLE_ZERO - iThrottleVal;
        dThrottleVal = 0.0 + (dThrottleVal / kD_THROTTLE_MAX);
    }

    if (iIsOn == HIGH) {
        SpeedControl.write(iThrottleVal);
    } else {
        SpeedControl.write(k_iTHROTTLE_ZERO);
    }

    display.setCursor(0,kDISPLAY_LINE);
    display.print("S1: ");
    display.print(dThrottleVal);
    display.print(" ");

}