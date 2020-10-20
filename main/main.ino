#include <NewPing.h>
#include <SoftwareSerial.h>

#define timeBetweenCommands 1000//test and fix

#define RIGHT_FRONT_1 4
#define RIGHT_FRONT_2 5

#define RIGHT_BACK_1 A4
#define RIGHT_BACK_2 A3

#define LEFT_FRONT_1 6
#define LEFT_FRONT_2 7

#define LEFT_BACK_1 A2
#define LEFT_BACK_2 A1

#define RIGHT_TRIG 9
#define RIGHT_ECHO 10
#define RIGHT_SONAR_VCC 8

#define LEFT_TRIG 12
#define LEFT_ECHO 13
#define LEFT_SONAR_VCC 11

#define rx 3
#define tx 2

#define MAX_DISTANCE 200

#define metal_input A5

NewPing RIGHT_SONAR(RIGHT_TRIG, RIGHT_ECHO, MAX_DISTANCE);
NewPing LEFT_SONAR(LEFT_TRIG, LEFT_ECHO, MAX_DISTANCE);
SoftwareSerial BTserial(rx, tx);

String strCommands = "";
bool BTstop = false;
char current_command;
int rightSonarSumm;
int leftSonarSumm;
bool finished_ride = false;

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
  
  digitalWrite(RIGHT_SONAR_VCC, HIGH);//питание правому ультразвуку
  digitalWrite(LEFT_SONAR_VCC, HIGH);//питание левому ультразвуку
  
  Serial.begin(9600);
  BTserial.begin(9600);

  while(!BTserial){};
  while(!Serial){};
  BTserial.print('r'); //r satnds for ready
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
      delay(1);
    }
  }
}

char currentCommandChar(){
  for (byte i = 0; i < strCommands.length(); i++){
    return strCommands.charAt(i);  
  }
}

byte getLeftUS(){
  leftSonarSumm = 0;
  for(byte i = 0; i < 10; i++){
    leftSonarSumm += LEFT_SONAR.ping_cm(); 
  }
  return leftSonarSumm / 10;
}

byte getRightUS(){
  rightSonarSumm = 0;
  for(byte i = 0; i < 10; i++){
    rightSonarSumm += RIGHT_SONAR.ping_cm(); 
  }
  return rightSonarSumm / 10;
}

void read_commands(){
  while (BTstop == false){
    while (BTserial.available() > 0){
      current_command = BTserial.read();
      if (current_command != 's'){
        strCommands += current_command;
      }
      else{
        strCommands += current_command;
        BTstop = true;
      }
    }
  }
}

//НАДО ПРОТЕСТИРОВАТЬ И ПОФИКСИТЬ!
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
