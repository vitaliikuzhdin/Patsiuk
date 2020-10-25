#include <NewPing.h>
#include <SoftwareSerial.h>
#include <GyverMotor.h>//documentation: https://alexgyver.ru/gyvermotor/

#define timeBetweenCommands 1000//must be 10 cm

#define RIGHT_FRONT_D 4
#define RIGHT_FRONT_PWM 5

#define RIGHT_BACK_PWM 9
#define RIGHT_BACK_D A4

#define LEFT_FRONT_PWM 6
#define LEFT_FRONT_D 7

#define LEFT_BACK_PWM 10
#define LEFT_BACK_D A3

#define RIGHT_TRIG 11
#define RIGHT_ECHO 12
#define RIGHT_SONAR_VCC 8

#define LEFT_TRIG A2
#define LEFT_ECHO A1
#define LEFT_SONAR_VCC 13

#define rx 3
#define tx 2

#define MAX_DISTANCE 100

#define metal_input A5

NewPing RIGHT_SONAR(RIGHT_TRIG, RIGHT_ECHO, MAX_DISTANCE);
NewPing LEFT_SONAR(LEFT_TRIG, LEFT_ECHO, MAX_DISTANCE);
SoftwareSerial BTserial(rx, tx);

String strCommands;
bool BTstop = false;
bool finished_ride = false;
byte commandSerialNum = 0;

void setup(){
  pinMode(metal_input, INPUT);
  
  pinMode(RIGHT_FRONT_1, OUTPUT);
  pinMode(RIGHT_FRONT_2, OUTPUT);
   
  pinMode(RIGHT_BACK_1, OUTPUT);
  pinMode(RIGHT_BACK_2, OUTPUT);

  pinMode(LEFT_FRONT_1, OUTPUT);
  pinMode(LEFT_FRONT_2, OUTPUT);

  pinMode(LEFT_BACK_1, OUTPUT);
  pinMode(LEFT_BACK_2, OUTPUT);
  
  pinMode(LEFT_SONAR_VCC, OUTPUT);
  pinMode(RIGHT_SONAR_VCC, OUTPUT);
  
  digitalWrite(RIGHT_SONAR_VCC, HIGH);//power for right sonar
  digitalWrite(LEFT_SONAR_VCC, HIGH);//power for left sonar
  
  Serial.begin(9600);
  BTserial.begin(9600);

  while(!BTserial){};
  while(!Serial){};
}

void loop(){
  read_commands();
  
  while(finished_ride == false){
    if (getRightUS() > 20 and getLeftUS() > 20){//checks if there is any obstacles
      //guides car
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
      else{
        finished_ride = true;
      }
    }
    else{//avoid obstacles
      right();
      forward();
      left();
    }
    for (int i = 0; i < timeBetweenCommands; i++){
      while(!BTserial){};
      BTserial.print(analogRead(metal_input));//send feedback of md
      delay(10);
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

//needs a fix
void right(){
  digitalWrite(RIGHT_FRONT_1, HIGH); 
  digitalWrite(RIGHT_FRONT_2, LOW);

  digitalWrite(RIGHT_BACK_1, HIGH);
  digitalWrite(RIGHT_BACK_2, LOW);

  digitalWrite(LEFT_FRONT_1, LOW);
  digitalWrite(LEFT_FRONT_2, HIGH);

  digitalWrite(LEFT_BACK_1, LOW);
  digitalWrite(LEFT_BACK_2, HIGH);

  BTserial.print('r');
}

void left(){
  digitalWrite(RIGHT_FRONT_1, HIGH); 
  digitalWrite(RIGHT_FRONT_2, LOW);

  digitalWrite(RIGHT_BACK_1, HIGH);
  digitalWrite(RIGHT_BACK_2, LOW);

  digitalWrite(LEFT_FRONT_1, LOW);
  digitalWrite(LEFT_FRONT_2, HIGH);

  digitalWrite(LEFT_BACK_1, LOW);
  digitalWrite(LEFT_BACK_2, HIGH);

  BTserial.print('l');
}

void forward(){
  digitalWrite(RIGHT_FRONT_1, HIGH); 
  digitalWrite(RIGHT_FRONT_2, LOW);

  digitalWrite(RIGHT_BACK_1, HIGH);
  digitalWrite(RIGHT_BACK_2, LOW);

  digitalWrite(LEFT_FRONT_1, LOW);
  digitalWrite(LEFT_FRONT_2, HIGH);

  digitalWrite(LEFT_BACK_1, LOW);
  digitalWrite(LEFT_BACK_2, HIGH);

  BTserial.print('f');
}

void back(){
  digitalWrite(RIGHT_FRONT_1, HIGH); 
  digitalWrite(RIGHT_FRONT_2, LOW);

  digitalWrite(RIGHT_BACK_1, HIGH);
  digitalWrite(RIGHT_BACK_2, LOW);

  digitalWrite(LEFT_FRONT_1, LOW);
  digitalWrite(LEFT_FRONT_2, HIGH);

  digitalWrite(LEFT_BACK_1, LOW);
  digitalWrite(LEFT_BACK_2, HIGH);

  BTserial.print('b');
}
