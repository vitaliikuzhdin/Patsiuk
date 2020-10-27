#include <NewPing.h>//documentation: https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home
#include <NeoSWSerial.h>//documentation: https://github.com/SlashDevin/NeoSWSerial
#include <GyverMotor.h>//documentation: https://alexgyver.ru/gyvermotor/

//test and fix
#define timeForRiding 100//must be 10 cm
#define timeForTurning 100//must be 90 degrees
#define minDuty 200

#define RIGHT_FRONT_PWM 5
#define RIGHT_FRONT_D 4

#define RIGHT_BACK_PWM 9
#define RIGHT_BACK_D A4

#define LEFT_FRONT_PWM 6
#define LEFT_FRONT_D 7

#define LEFT_BACK_PWM 10
#define LEFT_BACK_D A3

#define RIGHT_TRIG 3
#define RIGHT_ECHO 8
#define RIGHT_SONAR_VCC 2

#define LEFT_TRIG 13
#define LEFT_ECHO A0
#define LEFT_SONAR_VCC 12

#define rx A1
#define tx A2

#define MAX_DISTANCE 100

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

int angle;
int xTravel;
int yTravel;

void setup(){
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
  
  Serial.begin(9600);//only for tests
  BTserial.begin(9600); 
}

void loop(){
  read_commands();
  bool finished_ride = false;
  while(finished_ride == false){
    if (getRightUS() > 20 and getLeftUS() > 20){//checks if there is any obstacles
      //guides car
      if (currentCommandChar() == 'f'){
        forward();
        for (int i = 0; i < timeForRiding; i++){
          BTserial.print(analogRead(metal_input));//send feedback of md
          delay(1);
        }
      }
      else if (currentCommandChar() == 'b'){
        back();
        for (int i = 0; i < timeForRiding; i++){
          BTserial.print(analogRead(metal_input));//send feedback of md
          delay(1);
        }
      }
      else if (currentCommandChar() == 'r'){
        right();
        for (int i = 0; i < timeForTurning; i++){
          BTserial.print(analogRead(metal_input));//send feedback of md
          delay(1);
        }
      }
      else if (currentCommandChar() == 'l'){
        left();
        for (int i = 0; i < timeForTurning; i++){
          BTserial.print(analogRead(metal_input));//send feedback of md
          delay(1);
        }
      }
      else{
        //return home
      }
    }
    else{//avoid obstacles
      right();
      forward();
      left();
    }
  }
}

char currentCommandChar(){
  return strCommands.charAt(commandSerialNum++);
}

byte getLeftUS(){
  int leftSonarSumm = 0;
  for(byte i = 0; i < 20; i++){
    leftSonarSumm += LEFT_SONAR.ping_cm(); 
  }
  return leftSonarSumm / 20;
}

byte getRightUS(){
  int rightSonarSumm = 0;
  for(byte i = 0; i < 20; i++){
    rightSonarSumm += RIGHT_SONAR.ping_cm(); 
  }
  return rightSonarSumm / 20;
}

void read_commands(){
  bool BTstop = false;
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
        strCommands += BTserial.read();
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

  BTserial.print('r');

  angle += 90;
  if (angle == 360){
    angle = 0;  
  }
}

void left(){
  RIGHT_FRONT.smoothTick(255);
  RIGHT_BACK.smoothTick(255);
  LEFT_FRONT.smoothTick(-255);
  LEFT_BACK.smoothTick(-255);

  BTserial.print('l');

  angle -= 90;
  if (angle == 360){
    angle = 0;  
  }
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
}
