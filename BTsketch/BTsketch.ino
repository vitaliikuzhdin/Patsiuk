#include <SoftwareSerial.h>

#define rx 3
#define tx 2

SoftwareSerial BTserial(rx, tx);//rx, tx

char current_command;
String commands;
bool BTstop = false;

void setup() {
  BTserial.begin(9600);
  Serial.begin(9600);
}

void loop() {
  while (BTstop == false){
    while (BTserial.available() > 0){
      current_command = BTserial.read();
      if (current_command != 's')
        commands += current_command;
      else
        BTstop = true;
    }
  }
  Serial.println(commands);//only for tests
}
