#include <Wire.h>
#include <EEPROM.h>


/**** Pin definitions *******/
/**** End of Pin definitions *******/

/*** Instructions ***/
#define I_NONE        0

#define I_PING      254
#define I_ERR       255
/*** End of Instructions ***/

byte addr;
byte instr = I_NONE;

void setup() {
  addr = EEPROM.read(0);
  Wire.begin(addr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
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

    default:
      Wire.write(I_ERR);
    break;
  }
  instr = I_NONE;
  
}
