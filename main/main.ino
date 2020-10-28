#include <NewPing.h>//documentation: https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home
#include <NeoSWSerial.h>//documentation: https://github.com/SlashDevin/NeoSWSerial
#include <GyverMotor.h>//documentation: https://alexgyver.ru/gyvermotor/

//test and fix
#define timeForRiding 100//must be 10 cm
#define timeForTurning 100//must be 90 degrees
#define minDuty 200
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

#define rx A1
#define tx A2

#define metal_input A5

NewPing RIGHT_SONAR(RIGHT_TRIG, RIGHT_ECHO, MAX_DISTANCE);
NewPing LEFT_SONAR(LEFT_TRIG, LEFT_ECHO, MAX_DISTANCE);

NeoSWSerial BTserial(rx, tx);

GMotor RIGHT_FRONT(DRIVER2WIRE, RIGHT_FRONT_D, RIGHT_FRONT_PWM, HIGH);
GMotor RIGHT_BACK(DRIVER2WIRE, RIGHT_BACK_D, RIGHT_BACK_PWM, HIGH);
GMotor LEFT_FRONT(DRIVER2WIRE, LEFT_FRONT_D, LEFT_FRONT_PWM, HIGH);
GMotor LEFT_BACK(DRIVER2WIRE, LEFT_BACK_D, LEFT_BACK_PWM, HIGH);

String strCommands;
byte commandSerialNum;
bool BTstop;
bool finished_ride;

int angle;
int xTravel;
int yTravel;

void setup(){
    BTserial.begin(9600);
       
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

    RIGHT_FRONT.setDirection(NORMAL);
    RIGHT_BACK.setDirection(NORMAL);
    LEFT_FRONT.setDirection(REVERSE);
    LEFT_BACK.setDirection(NORMAL);

    RIGHT_FRONT.setMinDuty(minDuty);
    RIGHT_BACK.setMinDuty(minDuty);
    LEFT_FRONT.setMinDuty(minDuty);
    LEFT_BACK.setMinDuty(minDuty);
    
    RIGHT_FRONT.setMode(AUTO);
    RIGHT_BACK.setMode(AUTO);
    LEFT_FRONT.setMode(AUTO);
    LEFT_BACK.setMode(AUTO); 
}

void loop() {
    read_commands();
    while (finished_ride == false){
        if (getRightUS() > 20 and getLeftUS() > 20){
            if (currentCommandChar() == 'f'){
                forward();
            }
            else if (currentCommandChar() == 'b'){
                back();
            }
            else if (currentCommandChar() == 'r'){
                right();
            }
            else if (currentCommandChar() == 'l'){
                left();
            }
            else{//currentCommandChar() == 's'
            finished_ride = true;    
            }
        }
        else{
            //avoid obstacles
            right();
            forward();
            left(); 
        }
    }
    //if finished_ride == true
    //return home Y
    if (yTravel > 0){
        while (angle != 180){
            right();    
        }
        for (byte i = 0; i < yTravel; i++){
            forward();    
        }
    }
    else{
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
    BTserial.println('d');
}

char currentCommandChar(){
    return strCommands.charAt(commandSerialNum++);
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

char getMDFeedback(){
    if (analogRead(metal_input) >= 800){
        return 'h';  
    }
    else if (analogRead(metal_input) < 800 and analogRead(metal_input) > 300){
        return 'm';  
    }
    else{
        return 'l';  
    }
}

void read_commands(){
    while (BTstop == false){
        while (BTserial.available() > 0){
            char current_command = BTserial.read();
            if (current_command == 'c'){
                strCommands = "";
            }
            else if (current_command != 's'){
                for(byte i = 0; i < 10; i++){//because 1 command = 10 cm
                    strCommands += current_command;
                }
            }
            else{
                strCommands += current_command;
                BTstop = true;
            }
        }
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
    
    BTserial.println(angle);
    
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

    BTserial.println(angle);
    
    delay(timeForTurning);
}

void forward(){
    RIGHT_FRONT.smoothTick(255);
    RIGHT_BACK.smoothTick(255);
    LEFT_FRONT.smoothTick(255);
    LEFT_BACK.smoothTick(255);
    
    BTserial.print('f');

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
    for (int i = 0; i < timeForRiding; i++){
        BTserial.println(getMDFeedback());
        delay(50);
    }
}

void back(){
    RIGHT_FRONT.smoothTick(-255);
    RIGHT_BACK.smoothTick(-255);
    LEFT_FRONT.smoothTick(-255);
    LEFT_BACK.smoothTick(-255);

    BTserial.print('b');

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
    for (int i = 0; i < timeForRiding; i++){
        BTserial.println(getMDFeedback());
        delay(50);
    }
}
