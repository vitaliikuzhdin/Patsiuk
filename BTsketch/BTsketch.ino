#include <SoftwareSerial.h>

#define rx 3
#define tx 2

SoftwareSerial BTserial(rx, tx);//rx, tx

char BTcurrent_command;
String strCommands;
bool BTstop = false;

void setup() {
  BTserial.begin(9600);
  Serial.begin(9600);//only for tests
  while(!Serial){};//only for tests
  while(!BTserial){};
}

void loop() {
  while (BTstop == false){
    while (BTserial.available() > 0){
      BTcurrent_command = BTserial.read();
      if (BTcurrent_command != 's'){
        strCommands += BTcurrent_command;}
      else{
        BTstop = true;
        strCommands += BTcurrent_command;}
    }
  }
  Serial.println(strCommands);//only for tests
}
