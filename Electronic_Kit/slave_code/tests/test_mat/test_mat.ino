/* pin equivalence
PD0..PD7  0..7
PB0       8
PB1       9
PB2       10
PB3       11
PB4       12
PB5       13

PC0..5    A0..A5


*/

#define P_COL_ADDR0  7
#define P_COL_ADDR1  6
#define P_COL_ADDR2  5
#define P_ROW_ADDR0  4
#define P_ROW_ADDR1  3
#define P_ROW_ADDR2  2
#define P_EN         'A5'

void setup() {
    pinMode(P_COL_ADDR0, OUTPUT);
    pinMode(P_COL_ADDR1, OUTPUT);
    pinMode(P_COL_ADDR2, OUTPUT);
    pinMode(P_ROW_ADDR0, OUTPUT);
    pinMode(P_ROW_ADDR1, OUTPUT);
    pinMode(P_ROW_ADDR2, OUTPUT);
    pinMode(P_EN, OUTPUT);
    digitalWrite(P_EN, HIGH);
    digitalWrite(P_COL_ADDR0, LOW);
    digitalWrite(P_COL_ADDR1, LOW);
    digitalWrite(P_COL_ADDR2, LOW);
    digitalWrite(P_ROW_ADDR0, LOW);
    digitalWrite(P_ROW_ADDR1, LOW);
    digitalWrite(P_ROW_ADDR2, LOW);

}

void loop() {
    digitalWrite(P_EN, HIGH);
    digitalWrite(P_COL_ADDR0, LOW);
    digitalWrite(P_COL_ADDR1, LOW);
    digitalWrite(P_COL_ADDR2, LOW);
    delay(1000);
    digitalWrite(P_COL_ADDR0, HIGH);
    digitalWrite(P_COL_ADDR1, HIGH);
    digitalWrite(P_COL_ADDR2, HIGH);
    delay(1000);
    
    digitalWrite(P_EN, LOW);
    digitalWrite(P_COL_ADDR0, LOW);
    digitalWrite(P_COL_ADDR1, LOW);
    digitalWrite(P_COL_ADDR2, LOW);
    delay(1000);
    digitalWrite(P_COL_ADDR0, HIGH);
    digitalWrite(P_COL_ADDR1, HIGH);
    digitalWrite(P_COL_ADDR2, HIGH);
    delay(1000);
    // for (int i=0; i<8; i++) {
    //     digitalWrite(P_ROW_ADDR0, bitRead(i, 0));
    //     digitalWrite(P_ROW_ADDR1, bitRead(i, 1));
    //     digitalWrite(P_ROW_ADDR2, bitRead(i, 2));
    //     for (int j=0; j<8; j++) {
    //         digitalWrite(P_COL_ADDR0, bitRead(j, 0));
    //         digitalWrite(P_COL_ADDR1, bitRead(j, 1));
    //         digitalWrite(P_COL_ADDR2, bitRead(j, 2));
    //         delay(100);
    //     }
    // }
}

