#include <Wire.h>
#include <EEPROM.h>


/**** Pin definitions *******/
#define P_COL_ADDR0  1
#define P_COL_ADDR1  2
#define P_COL_ADDR2  3
#define P_ROW_ADDR0  1
#define P_ROW_ADDR1  2
#define P_ROW_ADDR2  3
#define P_EN         4

/**** End of Pin definitions *******/

/*** Instructions ***/
#define I_NONE    0
#define I_SETPX   1
#define I_SETROW  2
#define I_SETMAT  3
#define I_GETDATA 4

#define I_PING  254
#define I_ERR   255
/*** End of Instructions ***/

byte addr;
byte instr = I_NONE;
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};  // each index is a row

void setup() {
  addr = EEPROM.read(0);
  Wire.begin(addr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  refresh();
}

void refresh() {
  for (byte i=0; i<8; i++) {
    digitalWrite(P_ROW_ADDR0, bitRead(i, 0));
    digitalWrite(P_ROW_ADDR1, bitRead(i, 1));
    digitalWrite(P_ROW_ADDR2, bitRead(i, 2));

    for (byte j=0; j<8; j++) {
      digitalWrite(P_COL_ADDR0, bitRead(j, 0));
      digitalWrite(P_COL_ADDR1, bitRead(j, 1));
      digitalWrite(P_COL_ADDR2, bitRead(j, 2));
      digitalWrite(P_EN, bitRead(data[i], j));
    }
  }
}
int16_t read16() {
  byte h = Wire.read();  
  byte l = Wire.read();
  int16_t result = h << 8 + l;
  return result;
}

void receiveEvent(int n) {
  instr = Wire.read();
  if (n > 1) {
    byte row, col, val;
    switch(instr) {
      case(I_SETPX):
        row = Wire.read();
        col = Wire.read();
        val = Wire.read();
        bitWrite(data[row], col, bool(val));
      break;
      case(I_SETROW):
        row = Wire.read();
        val = Wire.read();
        data[row] = val;
      break;
      case(I_SETMAT):
      for (int i=0; i<8; i++)
        data[i] = Wire.read();
      break;
    }
    instr = I_NONE;
  }
}
void requestEvent() {
  switch(instr) {
    case(I_PING):
      Wire.write(I_PING);
    break;
    case(I_GETDATA):
      for (int i=0; i<8; i++)
        Wire.write(data[i]);
    break;
    

    default:
      Wire.write(I_ERR);
    break;
  }
  instr = I_NONE;
  
}
