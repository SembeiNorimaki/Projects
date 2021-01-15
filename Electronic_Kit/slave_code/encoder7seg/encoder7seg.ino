#include <Wire.h>
#include <EEPROM.h>

/**** Pin definitions *******/
// Encoder pins
#define P_ENC_LEFT   2
#define P_ENC_RIGHT  3
#define P_ENC_BUTTON 4

// 7Seg pins
#define P_ADDR0  1
#define P_ADDR1  2
#define P_ADDR2  3
#define P_EN     4
#define P_CS0  1
#define P_CS1  2
#define P_CS2  3
#define P_CS3  3

/**** End of Pin definitions *******/

/*** Instructions ***/
#define I_NONE        0
#define I_GETVAL      1
#define I_GETMINMAX   2
#define I_SETVAL      3
#define I_SETMINMAX   4
#define I_PING      254
#define I_ERR       255

/*** End of Instructions ***/

byte numbers[] = {
  B10001000,  //  0
  B11101011,  //  1
  B01001100,  //  2
  B01001001,  //  3
  B00101011,  //  4
  B00011001,  //  5
  B00011000,  //  6
  B11001011,  //  7
  B00001000,  //  8
  B00001011,  //  9
};

byte addr;
byte instr = I_NONE;
int16_t val = 0;
int16_t minVal = 0;
int16_t maxVal = 100;



void setup() {
  addr = EEPROM.read(0);

  attachInterrupt(digitalPinToInterrupt(P_ENC_LEFT), onPulseLeft, HIGH);
  attachInterrupt(digitalPinToInterrupt(P_ENC_RIGHT), onPulseRight, HIGH);
    
  Wire.begin(addr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(115200);
  
}

void loop() {
  updateVal();
}

void updateVal() {
  // split val into digits
  byte units = byte(val % 10);
  byte tens = byte(floor(val / 10));
  byte hundreds = byte(floor(val / 100));
  byte thousands = byte(floor(val / 1000));
  
  displayDigit(units, 0);
  displayDigit(tens, 1);
  displayDigit(hundreds, 2);
  displayDigit(thousands, 3);
}

// TODO: Code only the segments to be illuminated
void displayDigit(byte digit, byte segment) {
  // digit must be [0..9]
  // segment must be [0..3]
  digitalWrite(P_CS0, segment==0);
  digitalWrite(P_CS1, segment==1);
  digitalWrite(P_CS2, segment==2);
  digitalWrite(P_CS3, segment==3);
  
  for (int i=0; i<8; i++) {
    if(bitRead(numbers[digit], i)) {
      // output i to mux address
      digitalWrite(P_ADDR0, bitRead(i, 0));
      digitalWrite(P_ADDR1, bitRead(i, 1));
      digitalWrite(P_ADDR2, bitRead(i, 2));
      digitalWrite(P_EN, HIGH);
    }
  }
}

void onPulseLeft() {
  val--;
  if (val < minVal)
    val = minVal;
}

void onPulseRight() {
  val++;
  if (val > maxVal)
    val = maxVal;
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
    switch(instr) {
      case(I_SETVAL):
        val = read16();
      break;
      case(I_SETMINMAX):
        minVal = read16();
        maxVal = read16();
      break;
    }
  }
//  while(Wire.available() > 1) {
//    char c = Wire.read();
//  }
}

void requestEvent() {
  switch(instr) {
    case(I_PING):
    Wire.write(I_PING);
    break;
    case(I_GETVAL):
      Wire.write(highByte(val));
      Wire.write(lowByte(val));
    break;
    case(I_GETMINMAX):
      Wire.write(highByte(minVal));
      Wire.write(lowByte(minVal));
      Wire.write(highByte(maxVal));
      Wire.write(lowByte(maxVal));
    break;
    default:
      Wire.write(I_ERR);
    break;
  }
  instr = I_NONE;
}
