//test and fix
#define timeForRiding 100//must be 10 cm
#define timeForTurning 100//must be 90 degrees
#define minDuty 50
#define smoothSpeed 50
#define MAX_DISTANCE 100

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

#include <NewPing.h>//documentation: https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home
NewPing RIGHT_SONAR(RIGHT_TRIG, RIGHT_ECHO, MAX_DISTANCE);
NewPing LEFT_SONAR(LEFT_TRIG, LEFT_ECHO, MAX_DISTANCE);

#include <GyverMotor.h>//documentation: https://alexgyver.ru/gyvermotor/
GMotor RIGHT_FRONT(DRIVER2WIRE, RIGHT_FRONT_D, RIGHT_FRONT_PWM, HIGH);
GMotor RIGHT_BACK(DRIVER2WIRE, RIGHT_BACK_D, RIGHT_BACK_PWM, HIGH);
GMotor LEFT_FRONT(DRIVER2WIRE, LEFT_FRONT_D, LEFT_FRONT_PWM, HIGH);
GMotor LEFT_BACK(DRIVER2WIRE, LEFT_BACK_D, LEFT_BACK_PWM, HIGH);

boolean joystickMode, doneParsing, startParsing, readMod, finished_ride, rightTurn;
int angle, xTravel, yTravel, X, Y;
String string_convert = "";

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
    digitalWrite(LEFT_SONAR_VCC, HIGH);//power for left sonar

    RIGHT_FRONT.setResolution(8);
    RIGHT_BACK.setResolution(8);
    LEFT_FRONT.setResolution(8);
    LEFT_BACK.setResolution(8);

    //test and fix
    RIGHT_FRONT.setDirection(NORMAL);
    RIGHT_BACK.setDirection(NORMAL);
    LEFT_FRONT.setDirection(NORMAL);
    LEFT_BACK.setDirection(NORMAL);

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

void loop() {
    parsing();
    if (doneParsing){
        if (joystickMode){
            X *= 2;//because joystick max is 127
            Y *= 2;
            
            RIGHT_FRONT.smoothTick(Y - X);
            RIGHT_BACK.smoothTick(Y - X);
            LEFT_FRONT.smoothTick(Y + X);
            LEFT_BACK.smoothTick(Y + X);

            if (analogRead(metal_input) >= 400){
                Serial.flush();
                Serial.println('Y');  
            }
            else{
                Serial.flush();
                Serial.println('n');
            }
        }
        else if (joystickMode == false){
            if (getRightUS() > 20 and getLeftUS() > 20){
                while (finished_ride == false){
                    //do way stuff
                    right();
                    for (unsigned int i = Y * 10 + 1; i > 0; i--){
                        for (unsigned int i = X * 10 + 1; i > 0; i--){
                            forward();  
                        }
                        if (rightTurn){
                            right();
                            forward();
                            right();
                            rightTurn = false;
                        }
                        else{
                            left();
                            forward();
                            left();
                            rightTurn = true; 
                        }
                    } 
                }  
                if (finished_ride){    
                    return_home();
                    Serial.println('e');
                }
            }
            else{
                right();
                forward();
                left();  
            }
        }
    }    
}

byte getLeftUS(){
    unsigned int leftSonarSumm = 0;
    for (byte i = 0; i < 20; i++){
        leftSonarSumm += LEFT_SONAR.ping_cm(); 
    }
    return leftSonarSumm / 20;
}

byte getRightUS(){
    unsigned int rightSonarSumm = 0;
    for (byte i = 0; i < 20; i++){
        rightSonarSumm += RIGHT_SONAR.ping_cm(); 
    }
    return rightSonarSumm / 20;
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

    if (angle == 0){
     yTravel++;    
    }
    else if (angle == 90){
        xTravel++;    
    }
    else if (angle == 180){
        yTravel--;    
    }
    else{//angle == 270
         xTravel--; 
    }
    for (unsigned int i = 0; i < timeForRiding; i++){
        if (analogRead(metal_input) > 500){
            Serial.println('Y');
            stopCar();
        }
        else{
            delay(10);
        }
    }
}

void back(){
    RIGHT_FRONT.smoothTick(-255);
    RIGHT_BACK.smoothTick(-255);
    LEFT_FRONT.smoothTick(-255);
    LEFT_BACK.smoothTick(-255);

    if (angle == 0){
        yTravel--;    
    }
    else if (angle == 90){
        xTravel--;    
    }
    else if (angle == 180){
        yTravel++;    
    }
    else{//angle == 270
         xTravel++; 
    }
    for (unsigned int i = 0; i < timeForRiding; i++){
        if (analogRead(metal_input) > 500){
            Serial.println('Y');
            stopCar();
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

void return_home(){
    if (yTravel > 0){
        while (angle != 180){
            right();    
        }
        for (byte i = 0; i < yTravel; i++){
            forward();    
        }
    }
    else{//yTravel < 0
        while (angle != 0){
            right();    
        }
        for (byte i = 0; i > yTravel; i--){
            forward();    
        }
    }
    //return home X
    if (xTravel > 0){
        while (angle != 270){
            right();  
        }
        for (byte i = 0; i < xTravel; i++){
            forward();  
        } 
    }
    else{//xTravel < 0
        while (angle != 90){
            right();  
        }
        for (byte i = 0; i > xTravel; i--){
            forward();  
        }
    }
    //set angle to 0
    while (angle != 0){
        right();  
    }
}

void parsing(){
    if (Serial.available() > 0){
        unsigned char incomingChar = Serial.read();  
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
