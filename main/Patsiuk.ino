/*Sketch for SAM "Patsiuk" project
 * Fully written by Vitalii Kuzhdin (@vitaliy172), 2020
 * For more information, look at shematic
 */

/*=============SETTINGS=============*/
#define timeForRiding 100             //must be 10 cm
#define timeForTurning 100            //must be 90 degrees
#define minDuty 50                    //motors should start at this speed
#define smoothSpeed 50
#define RIGHT_FRONT_DIRECTION NORMAL  //NORMAL or REVERSE
#define RIGHT_BACK_DIRECTION  NORMAL
#define LEFT_FRONT_DIRECTION  NORMAL
#define LEFT_BACK_DIRECTION   NORMAL
#define MAX_SONAR_DISTANCE 100

/*==========PINS=========*/
#define RIGHT_FRONT_PWM 5
#define RIGHT_FRONT_D 2

#define RIGHT_BACK_PWM 9
#define RIGHT_BACK_D A4

#define LEFT_FRONT_PWM 6
#define LEFT_FRONT_D 3

#define LEFT_BACK_PWM 10
#define LEFT_BACK_D A3

#define RIGHT_TRIG 7
#define RIGHT_ECHO 8
#define RIGHT_SONAR_VCC 4

#define LEFT_TRIG 12
#define LEFT_ECHO 13
#define LEFT_SONAR_VCC 11

#define metal_input A5

/*============================LIBRARIES===============================*/
#include <NewPing.h>//documentation: https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home
NewPing RIGHT_SONAR(RIGHT_TRIG, RIGHT_ECHO, MAX_SONAR_DISTANCE);
NewPing LEFT_SONAR(LEFT_TRIG, LEFT_ECHO, MAX_SONAR_DISTANCE);

#include <GyverMotor.h>//documentation: https://alexgyver.ru/gyvermotor/
GMotor RIGHT_FRONT(DRIVER2WIRE, RIGHT_FRONT_D, RIGHT_FRONT_PWM, HIGH);
GMotor RIGHT_BACK(DRIVER2WIRE, RIGHT_BACK_D, RIGHT_BACK_PWM, HIGH);
GMotor LEFT_FRONT(DRIVER2WIRE, LEFT_FRONT_D, LEFT_FRONT_PWM, HIGH);
GMotor LEFT_BACK(DRIVER2WIRE, LEFT_BACK_D, LEFT_BACK_PWM, HIGH);

/*==============GLOBAL VARIABLES=============*/
boolean joystickMode, doneParsing, stopCarBool;
int angle, xTravel, yTravel, X, Y;

void setup(){
    Serial.begin(9600);
       
    pinMode(metal_input, INPUT);
    
    pinMode(RIGHT_FRONT_D, OUTPUT);
    pinMode(RIGHT_FRONT_PWM, OUTPUT);
     
    pinMode(RIGHT_BACK_D, OUTPUT);
    pinMode(RIGHT_BACK_PWM, OUTPUT);

    pinMode(LEFT_FRONT_D, OUTPUT);
    pinMode(LEFT_FRONT_PWM, OUTPUT);

    pinMode(LEFT_BACK_D, OUTPUT);
    pinMode(LEFT_BACK_PWM, OUTPUT);
    
    pinMode(RIGHT_SONAR_VCC, OUTPUT);
    pinMode(LEFT_SONAR_VCC, OUTPUT);

    digitalWrite(RIGHT_SONAR_VCC, HIGH);//power for right sonar
    digitalWrite(LEFT_SONAR_VCC, HIGH); //power for left sonar

    RIGHT_FRONT.setResolution(8);
    RIGHT_BACK.setResolution(8);
    LEFT_FRONT.setResolution(8);
    LEFT_BACK.setResolution(8);

    RIGHT_FRONT.setDirection(RIGHT_FRONT_DIRECTION);
    RIGHT_BACK.setDirection(RIGHT_BACK_DIRECTION);
    LEFT_FRONT.setDirection(LEFT_FRONT_DIRECTION);
    LEFT_BACK.setDirection(LEFT_BACK_DIRECTION);

    RIGHT_FRONT.setMinDuty(minDuty);
    RIGHT_BACK.setMinDuty(minDuty);
    LEFT_FRONT.setMinDuty(minDuty);
    LEFT_BACK.setMinDuty(minDuty);
    
    RIGHT_FRONT.setMode(AUTO);
    RIGHT_BACK.setMode(AUTO);
    LEFT_FRONT.setMode(AUTO);
    LEFT_BACK.setMode(AUTO);

    RIGHT_FRONT.setSmoothSpeed(smoothSpeed);
    RIGHT_BACK.setSmoothSpeed(smoothSpeed);
    LEFT_FRONT.setSmoothSpeed(smoothSpeed);
    LEFT_BACK.setSmoothSpeed(smoothSpeed);
}

void loop(){
    parsing();
    if (doneParsing){
        doneParsing = false;
        if (joystickMode){ 
            RIGHT_FRONT.smoothTick(Y - X);
            RIGHT_BACK.smoothTick(Y - X);
            LEFT_FRONT.smoothTick(Y + X);
            LEFT_BACK.smoothTick(Y + X);
            
            X = 0;
            Y = 0;

            if (analogRead(metal_input) >= 400){
                Serial.flush();
                Serial.println('Y');  
            }
            else{
                Serial.flush();
                Serial.println('n');
            }
        }
        else{//(joystickMode == false)
            Serial.println('n');
            stopCarBool = false;
            xTravel = 0;
            yTravel = 0;
            angle = 0;
            String commands = "";
            boolean rightTurn = false;
            
            for (unsigned int i = Y * 10 + 1; i > 0; i--){
                for (unsigned int o = X * 10 + 1; o > 0; o--){
                    commands += 'f';
                }
                if (rightTurn){
                    rightTurn = false;
                    commands += "rfr";
                }
                else{//(rightTurn == false)
                    rightTurn = true;
                    commands += "lfl";
                }
            }

            right();
            for (unsigned int currentCommand = 0; currentCommand < commands.length(); currentCommand++){
                if (stopCarBool == false){
                    byte timesAvoidedX = 0;
                    if (noObstacles()){
                        if (commands.charAt(currentCommand) == 'f'){
                            if (timesAvoidedX == 0){
                                forward();
                            }
                            else{
                                timesAvoidedX--;
                            }
                        }
                        else if (commands.charAt(currentCommand) == 'r'){
                            right();
                            timesAvoidedX = 0;
                        }
                        else if (commands.charAt(currentCommand) == 'l'){
                            left();
                            timesAvoidedX = 0;
                        }
                    }
                    else{//(noObstacles() == false)
                        byte timesAvoidedY = 0;
                        avoidObstacles:
                            //avoid obstacles X
                            while (noObstacles() == false){
                                right();
                                forward();
                                left();
                                timesAvoidedX++;
                            }
                            left();
                            //avoid obstacles Y
                            while (noObstacles() == false){
                                right();
                                forward();
                                left();
                                timesAvoidedY++;
                            }
                            right();
                            
                            if (noObstacles() == false){
                                goto avoidObstacles;
                            }
                            else{//done avoiding, return to original Y
                                left();
                                for (byte i = 0; i < timesAvoidedY; i++){
                                    forward();
                                }
                                right();
                            }
                    }
                }
                else{//(stopCarBool == true)
                    stopCar();
                }  
            }
            if (stopCarBool == false){
                returnHome();  
            }
        }
    }    
}

boolean noObstacles(){
    unsigned int rightSonarSumm = 0;
    for (byte i = 0; i < 10; i++){
        rightSonarSumm += RIGHT_SONAR.ping_cm();
    }
    rightSonarSumm /= 10;
    
    unsigned int leftSonarSumm = 0;
    for (byte i = 0; i < 10; i++){
        leftSonarSumm += LEFT_SONAR.ping_cm();
    }
    leftSonarSumm /= 10;
    
    if (rightSonarSumm <= 11 or leftSonarSumm <= 11){
        return false;
    }
    else{
        return true;
    }
}

void right(){
    RIGHT_FRONT.smoothTick(-255);
    RIGHT_BACK.smoothTick(-255);
    LEFT_FRONT.smoothTick(255);
    LEFT_BACK.smoothTick(255);

    angle += 90;
    if (angle == 360){
        angle = 0;
    }
    
    delay(timeForTurning);
}

void left(){
    RIGHT_FRONT.smoothTick(255);
    RIGHT_BACK.smoothTick(255);
    LEFT_FRONT.smoothTick(-255);
    LEFT_BACK.smoothTick(-255);

    angle -= 90;
    if (angle == 360){
        angle = 0;
    }
    
    delay(timeForTurning);
}

void forward(){
    RIGHT_FRONT.smoothTick(255);
    RIGHT_BACK.smoothTick(255);
    LEFT_FRONT.smoothTick(255);
    LEFT_BACK.smoothTick(255);

    if (angle == 0 or angle == 90){
        yTravel++;
    }  
    else{//(angle == 180 or angle == 270)
         xTravel--;
    }
    
    for (unsigned int i = 0; i < timeForRiding; i++){
        if (analogRead(metal_input) > 500){
            Serial.println('Y');//Found!
            stopCarBool = true;
        }
        else{
            delay(10);
        }
    }
}

void stopCar(){
    RIGHT_FRONT.smoothTick(0);
    RIGHT_BACK.smoothTick(0);
    LEFT_FRONT.smoothTick(0);
    LEFT_BACK.smoothTick(0);
}

void returnHome(){
    boolean doneReturning = false;
    while (doneReturning == false){
        if (noObstacles()){
            //return home Y
            if (yTravel > 0){
                while (angle != 180){
                    right();  
                }
                forward();
            }
            else if (yTravel < 0){
                while (angle != 0){
                    right();
                }
                forward();
            }
            //return home X
            else if (xTravel > 0){
                while (angle != 270){
                    right();  
                }
                forward();
            }
            else if (xTravel < 0){
                while (angle != 90){
                    right();
                }
                forward();
            }
            // set angle to 0
            else if (angle != 0){
                right();
            }
            if (angle == 0){
                doneReturning = true;
            }
        }
        else{//(noObstacles == false)
            avoidObstacles:
                //avoid obstacles X
                while (noObstacles() == false){
                    right();
                    forward();
                    left();
                }
                left();
                //avoid obstacles Y
                while (noObstacles() == false){
                    right();
                    forward();
                    left();
                }
                right();
                            
                if (noObstacles() == false){
                    goto avoidObstacles;
                }
        }
    }
    
    Serial.println('e');//Mission accomplished, but nothing was found
}

void parsing(){
    if (Serial.available() > 0){
        static boolean startParsing, readMod;
        static String string_convert;
        char incomingChar = Serial.read();  
        if (readMod){
            readMod = false;
            if (incomingChar == '1'){
                joystickMode = true;
            }
            else if (incomingChar == '2'){
                joystickMode = false;
            }
        }
        if (startParsing){
            if (incomingChar == ','){
                X = string_convert.toInt();
                string_convert = "";
            }
            else if (incomingChar == ';'){
                startParsing = false;
                doneParsing = true;
                Y = string_convert.toInt();
                string_convert = "";
            }
            else{
                string_convert += incomingChar;
            }
        }
        if (incomingChar == '$'){
            readMod = true;
        }
        if (incomingChar == ' '){
             startParsing = true;
        }
    }
}
