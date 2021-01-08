/*
outline shop test motor controller
*/

#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <elapsedMillis.h>

LiquidCrystal_I2C display(0x3F,20,4);

int kPIN_THROTTLE[] = {A2,A3,A6,A7};
int kPIN_THROTTLE_ALT[] = {A2,A3,A6,A6};
int kPIN_DIRECTION[] = {2,3,4,7};
int kPIN_ON[] = {11,12,8,8};
int kPIN_PWM_OUT[] = {5,6,9,10};
//int kDISPLAY_LINE[] = {0,1,2,3};
char cDisplayLine[4][21];
int kPIN_THROTTLE_SAME = A0;
int kPIN_SPEED_SERVO = A1;

Servo SpeedControl[4];

int kI_THROTTLE_MAX = 500;
double kD_THROTTLE_MAX = kI_THROTTLE_MAX;
int kI_SERVO_MAX = 180;

int iThrottleVal = 0;
double dThrottleVal = 0.0;
int k_iTHROTTLE_ZERO = 1500;
char cThrottleVal[6];

int iIsFwd[4];
String sFwdRev;
int iIsOn = 0;
String sOnOff;
int iControlIndex = 0;
int iIsAltSpeed = 0;
int iIsSpeed = HIGH;

int kI_ROBORIO_DELAY = 20;
int kI_LCD_DELAY = 250;
elapsedMillis iRoboRIOCount;
elapsedMillis iLCDCount;

void setup() {
 
    display.init();
    display.clear();
    display.setBacklight(200);

    for (iControlIndex=0; iControlIndex < 4; iControlIndex++){
        SpeedControl[iControlIndex].attach(kPIN_PWM_OUT[iControlIndex]);
        pinMode(kPIN_DIRECTION[iControlIndex], INPUT_PULLUP);
        pinMode(kPIN_ON[iControlIndex], INPUT_PULLUP);
    }
    pinMode(kPIN_THROTTLE_SAME,INPUT_PULLUP);
    pinMode(kPIN_SPEED_SERVO,INPUT_PULLUP);

    iRoboRIOCount = 0;
    iLCDCount = 0;

}

void loop() {
    /* 
    read speed control on/off switch
    set the appropriate literal
    and if speed control is off, read the direction switch
    DO NOT CHANGE MOTOR DIRECTION WHILE RUNNING
    */

    iIsSpeed = digitalRead(kPIN_SPEED_SERVO);
    iIsAltSpeed = !digitalRead(kPIN_THROTTLE_SAME);

    if (iRoboRIOCount >= kI_ROBORIO_DELAY) {
        iRoboRIOCount = 0;
        for (iControlIndex=0; iControlIndex < 4; iControlIndex++){

            iIsOn = !digitalRead(kPIN_ON[iControlIndex]);

            if (iIsSpeed == HIGH) {

                if (iIsOn != HIGH) {
                    iIsFwd[iControlIndex] = digitalRead(kPIN_DIRECTION[iControlIndex]);
                }

                /*
                read speed control potentiometer and convert to throttle range limit
                convert to double for calcs
                */
                if (iIsAltSpeed == LOW){
                    iThrottleVal = map(analogRead(kPIN_THROTTLE[iControlIndex]),0,1023,0,kI_THROTTLE_MAX);
                } else {
                    iThrottleVal = map(analogRead(kPIN_THROTTLE_ALT[iControlIndex]),0,1023,0,kI_THROTTLE_MAX);
                }
                dThrottleVal = iThrottleVal;
                
                /*
                PWM speed control signal is in the range of 1000 to 2000
                where 1000 is full reverse, 1500 is stop, and 2000 is full forward
                use integers for the PWM control
                translate to range of 1 to -1 for use in WPILib speed range programming
                */
                if (iIsFwd[iControlIndex] == HIGH) {
                    iThrottleVal = k_iTHROTTLE_ZERO + iThrottleVal;
                    dThrottleVal = 0.0 - (dThrottleVal / kD_THROTTLE_MAX);
                    sFwdRev = "FWD";
                } else {
                    iThrottleVal = k_iTHROTTLE_ZERO - iThrottleVal;
                    dThrottleVal = 0.0 + (dThrottleVal / kD_THROTTLE_MAX);
                    sFwdRev = "REV";
                }

                if (iIsOn == HIGH) {
                    sOnOff = "ON ";
                } else {
                    iThrottleVal = k_iTHROTTLE_ZERO;
                    sOnOff = "OFF";
                }

                SpeedControl[iControlIndex].writeMicroseconds(iThrottleVal);

                dtostrf(dThrottleVal, 2, 2, cThrottleVal);
                sprintf(cDisplayLine[iControlIndex],"Speed%1u %3s %5s %3s", iControlIndex+1, sFwdRev.c_str(), cThrottleVal, sOnOff.c_str());

/*
                display.setCursor(0,kDISPLAY_LINE[iControlIndex]);
                display.print("Speed");
                display.print(iControlIndex+1);
                display.print(" ");
                display.print(dThrottleVal);
                display.print(" ");
                display.print(sOnOff);
*/

            } else {
                            
                /*
                read speed control potentiometer and convert to servo range limit
                */
                if (iIsAltSpeed == LOW){
                    iThrottleVal = map(analogRead(kPIN_THROTTLE[iControlIndex]),0,1023,0,kI_SERVO_MAX);
                } else {
                    iThrottleVal = map(analogRead(kPIN_THROTTLE_ALT[iControlIndex]),0,1023,0,kI_SERVO_MAX);
                }

                if (iIsOn == HIGH) {
                    sOnOff= "ON ";
                } else {
                    sOnOff = "OFF";
                }

                SpeedControl[iControlIndex].write(iThrottleVal);

                sprintf(cDisplayLine[iControlIndex],"Servo%1u %3u %3s      ", iControlIndex+1, iThrottleVal, sOnOff.c_str());
                /*
                display.setCursor(0,iControlIndex);
                display.print("Servo");
                display.print(iControlIndex+1);
                display.print(" ");
                display.print(iThrottleVal);
                display.print(" ");
                display.print(sOnOff);
                */
            }
        }
    }

    if (iLCDCount >= kI_LCD_DELAY) {
        iLCDCount = 0;
        for (iControlIndex=0; iControlIndex < 4; iControlIndex++){
            display.setCursor(0,iControlIndex);
            display.print(cDisplayLine[iControlIndex]);
        }
    }


//    delay(20);

}