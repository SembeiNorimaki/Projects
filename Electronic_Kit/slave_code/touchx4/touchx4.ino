#include <Wire.h>
#include <EEPROM.h>


/**** Pin definitions *******/
#define P_TOUCH1
#define P_TOUCH2
#define P_TOUCH3
#define P_TOUCH4

/**** End of Pin definitions *******/

/*** Instructions ***/
#define I_NONE        0
#define I_GET_STATE   1
#define I_PING      254
#define I_ERR       255
/*** End of Instructions ***/

byte addr;
byte instr = I_NONE;
byte touch = 0;

void setup() {
  addr = EEPROM.read(0);
  Wire.begin(addr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
}

void read_touch() {
  bitWrite(touch, 0, digitalRead(P_TOUCH0));
  bitWrite(touch, 1, digitalRead(P_TOUCH1));
  bitWrite(touch, 2, digitalRead(P_TOUCH2));
  bitWrite(touch, 3, digitalRead(P_TOUCH3));
}

int16_t read16() {
  byte h = Wire.read();  
  byte l = Wire.read();
  int16_t result = h << 8 + l;
  return result;
}

void receiveEvent(int n) {
  instr = Wire.read();

}
void requestEvent() {
  switch(instr) {
    case(I_PING):
      Wire.write(I_PING);
    break;
    case(I_GETSTATE):
      Wire.write(touch);
    break;

    default:
      Wire.write(I_ERR);
    break;
  }
  instr = I_NONE;  
}
