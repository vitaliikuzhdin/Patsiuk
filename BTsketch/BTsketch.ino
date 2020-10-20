#include <SoftwareSerial.h>

#define rx 3
#define tx 2

SoftwareSerial BTserial(rx, tx);//rx, tx

char current_command;
String strCommands;
bool BTstop = false;

void setup() {
  BTserial.begin(9600);
  Serial.begin(9600);//only for tests
}

void loop() {
  while (BTstop == false){
    while (BTserial.available() > 0){
      current_command = BTserial.read();
      if (current_command != 's'){
        commands += current_command;}
      else{
        BTstop = true;
        commands += current_command;}
    }
  }
  Serial.println(commands);//only for tests
}
