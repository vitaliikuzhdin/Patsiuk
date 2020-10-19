#include <NewPing.h>

#define RIGHT_TRIG 9
#define RIGHT_ECHO 10
#define RIGHT_SONAR_VCC 8

#define LEFT_TRIG 12
#define LEFT_ECHO 13
#define LEFT_SONAR_VCC 11

#define MAX_DISTANCE 200

int leftSonarSumm;

NewPing RIGHT_SONAR(RIGHT_TRIG, RIGHT_ECHO, MAX_DISTANCE);
NewPing LEFT_SONAR(LEFT_TRIG, LEFT_ECHO, MAX_DISTANCE);

void setup() {
  pinMode(LEFT_SONAR_VCC, OUTPUT);
  pinMode(RIGHT_SONAR_VCC, OUTPUT);
  
  digitalWrite(RIGHT_SONAR_VCC, HIGH);//питание правому ультразвуку
  digitalWrite(LEFT_SONAR_VCC, HIGH);//питание левому ультразвуку
  
  Serial.begin(9600);
  while(!Serial){};
}

void loop() {
  Serial.println(getLeftUS());
  
}
byte getLeftUS(){
  leftSonarSumm = 0;
  for(byte i = 0; i < 10; i++){
    leftSonarSumm += LEFT_SONAR.ping_cm(); 
  }
  return leftSonarSumm / 10;
}
