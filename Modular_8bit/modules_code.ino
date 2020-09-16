#include <Wire.h>

// MCP23017 ports A and B addresses
#define ADDR_PORTA    0x12
#define ADDR_PORTB    0x13

#define CLK_PIN 2   // only pins 2 and 3 have interrupt capabilities

// physical address of the boards
#define ADDR_ALU_I2C 1
#define ADDR_RAM_I2C 1   // ALU and RAM share same physical I2C address
#define ADDR_BANK_I2C 2
#define ADDR_PC_I2C 3
#define ADDR_OUT_I2C 3   // PC and OUT share same physical I2C address
#define ADDR_DECO_I2C 4

// Physical PORTB pin of each group in the boards
#define EN_ALU_INA 0
#define EN_ALU_INB 1
#define EN_ALU_OUT 2
#define EN_ALU_OP_LEDS 3
#define EN_ALU_FLAGS_CTRL 4
#define EN_RAM_OUT 5
#define EN_RAM_CTRL 6
#define EN_RAM_MAR 7

#define EN_BANK_REGA 0
#define EN_BANK_REGB 1
#define EN_BANK_REGC 2
#define EN_BANK_REGD 3
#define EN_BANK_IN_CTRL 4
#define EN_BANK_AB 5

#define EN_PC_PC 0
#define EN_PC_SEL 1
#define EN_PC_CTRL 2
#define EN_OUT_SEG4 3
#define EN_OUT_SEG3 4
#define EN_OUT_SEG2 5
#define EN_OUT_SEG1 6
#define EN_OUT_OUT 7

#define EN_DECO_IR 7
#define EN_DECO_CTRL_UINSTR 6
#define EN_DECO_DECO1 5
#define EN_DECO_DECO2 4
#define EN_DECO_DECO3 3
#define EN_DECO_DECO4 2

// indexes of each LED group inside LEDS 
#define LEDS_ALU_INA_IDX 0
#define LEDS_ALU_INB_IDX 1
#define LEDS_ALU_OUT_IDX 2
#define LEDS_ALU_OP_LEDS_IDX 3
#define LEDS_ALU_FLAGS_CTRL_IDX 4
#define LEDS_RAM_OUT_IDX 5
#define LEDS_RAM_CTRL_IDX 6
#define LEDS_RAM_MAR_IDX 7

#define LEDS_BANK_REGA_IDX 5
#define LEDS_BANK_REGB_IDX 6
#define LEDS_BANK_REGC_IDX 7
#define LEDS_BANK_REGD_IDX 8
#define LEDS_BANK_IN_CTRL_IDX 9
#define LEDS_BANK_AB_IDX 10

#define LEDS_PC_PC_IDX 11
#define LEDS_PC_SEL_IDX 12
#define LEDS_PC_CTRL_IDX 13

#define LEDS_OUT_SEG4_IDX 14
#define LEDS_OUT_SEG3_IDX 15
#define LEDS_OUT_SEG2_IDX 16
#define LEDS_OUT_SEG1_IDX 17
#define LEDS_OUT_OUT_IDX 18

#define LEDS_DECO_IR_IDX 19
#define LEDS_DECO_CTRL_UINSTR_IDX 20
#define LEDS_DECO_DECO1_IDX 21
#define LEDS_DECO_DECO2_IDX 22
#define LEDS_DECO_DECO3_IDX 23
#define LEDS_DECO_DECO4_IDX 24

// specific ctrl bits
#define LEDS_ALU_FLAGS_CTRL_FLAG_Z_BIT 0
#define LEDS_ALU_FLAGS_CTRL_FLAG_N_BIT 1
#define LEDS_ALU_FLAGS_CTRL_FLAG_C_BIT 2
#define LEDS_ALU_FLAGS_CTRL_RD_ALU_BIT 3

#define LEDS_RAM_CTRL_WR_MAR_BIT 5
#define LEDS_RAM_CTRL_WR_RAM_BIT 6
#define LEDS_RAM_CTRL_RD_RAM_BIT 7

#define LEDS_BANK_IN_CTRL_WR_REG_BIT 0
#define LEDS_BANK_IN_CTRL_RD_REG_BIT 1
#define LEDS_BANK_IN_CTRL_IN0_BIT 2
#define LEDS_BANK_IN_CTRL_IN1_BIT 3
#define LEDS_BANK_IN_CTRL_IN2_BIT 4
#define LEDS_BANK_IN_CTRL_IN3_BIT 5

#define LEDS_BANK_AB_A0_BIT 0
#define LEDS_BANK_AB_A1_BIT 1
#define LEDS_BANK_AB_A2_BIT 2
#define LEDS_BANK_AB_A3_BIT 3
#define LEDS_BANK_AB_B0_BIT 4
#define LEDS_BANK_AB_B1_BIT 5
#define LEDS_BANK_AB_B2_BIT 6
#define LEDS_BANK_AB_B3_BIT 7

#define LEDS_PC_SEL_NJMP_BIT 0
#define LEDS_PC_SEL_JZ_BIT 1
#define LEDS_PC_SEL_JNZ_BIT 2
#define LEDS_PC_SEL_JN_BIT 3
#define LEDS_PC_SEL_JNN_BIT 4
#define LEDS_PC_SEL_JC_BIT 5
#define LEDS_PC_SEL_JNC_BIT 6
#define LEDS_PC_SEL_JMP_BIT 7

#define LEDS_PC_CTRL_FLAG_Z_BIT 0
#define LEDS_PC_CTRL_FLAG_N_BIT 1
#define LEDS_PC_CTRL_FLAG_C_BIT 2
#define LEDS_PC_CTRL_INC_BIT 3
#define LEDS_PC_CTRL_JMP_BIT 4
#define LEDS_PC_CTRL_COUNT_BIT 5
#define LEDS_PC_CTRL_RD_PC_BIT 6

#define LEDS_DECO_CTRL_UINSTR0_BIT 0
#define LEDS_DECO_CTRL_UINSTR1_BIT 1
#define LEDS_DECO_CTRL_UINSTR2_BIT 2
#define LEDS_DECO_CTRL_UINSTR3_BIT 3
#define LEDS_DECO_CTRL_UINSTR4_BIT 4
#define LEDS_RAM_CTRL_WR_IR_BIT 5

// indexes of each registers inside registers array
#define REG_A_IDX 0
#define REG_B_IDX 1
#define REG_C_IDX 2
#define REG_D_IDX 3
#define REG_MAR_IDX 4
#define REG_PC_IDX 5
#define REG_OUT_IDX 6

// indexes of each flag inside flags array
#define FLAG_Z_IDX 0 
#define FLAG_N_IDX 1 
#define FLAG_C_IDX 2 

// ALU operations
#define ALU_OP_ADD 0
#define ALU_OP_SUB 1
#define ALU_OP_ADDC 2
#define ALU_OP_SUBC 3
#define ALU_OP_INC 4
#define ALU_OP_DEC 5
#define ALU_OP_AND 6
#define ALU_OP_OR 7


uint8_t registers[7];
bool flags[3];
uint8_t ram[256];
uint8_t leds[25];
String rx = ""; 
uint8_t uinstr_val = 0;

// when a clock ticks we advance to the next micro instruction
void on_clock() {
  uinstr_val++;
}

// fetching the instruction is the same procedure for all instructions:
// uinstr 0: RD_PC, WR_MAR
// uinstr 1: RD_RAM, WR_IR, (COUNT??)
void fetch() {
  while(1) {
    if (uinstr_val == 0) {
      bitSet(leds[LEDS_PC_CTRL_IDX], LEDS_PC_CTRL_RD_PC_BIT);
      bitSet(leds[LEDS_RAM_CTRL_IDX], LEDS_RAM_CTRL_WR_MAR_BIT);
    } else if (uinstr_val == 1) {
      bitSet(leds[LEDS_RAM_CTRL_IDX], LEDS_RAM_CTRL_RD_RAM_BIT);
      bitSet(leds[LEDS_DECO_CTRL_UINSTR_IDX], LEDS_RAM_CTRL_WR_IR_BIT);
    } else {
      return;
    }  
  }
}

void update_register_leds() {
  leds[LEDS_BANK_REGA_IDX] = registers[REG_A_IDX];
  leds[LEDS_BANK_REGB_IDX] = registers[REG_B_IDX];
  leds[LEDS_BANK_REGC_IDX] = registers[REG_C_IDX];
  leds[LEDS_BANK_REGD_IDX] = registers[REG_D_IDX];
  leds[LEDS_PC_PC_IDX] = registers[REG_PC_IDX];
  leds[LEDS_RAM_MAR_IDX] = registers[REG_MAR_IDX];
  leds[LEDS_OUT_OUT_IDX] = registers[REG_OUT_IDX];
}
void update_leds(int board_addr, int group_addr, int val) {
    // group addr is the enable pin of the group
    // enable is active low, so we have to set all the other pins to HIGH
    // group addr=0  --> 11111110
    // group addr=2  --> 11111011
    uint8_t aux = ~(1 << group_addr);
    Wire.beginTransmission(0x20 + board_addr);         // Select Board
    Wire.write(ADDR_PORTB);                            // Port B selects a group of LEDS
    Wire.write(aux);                                   // Select the group of leds to update
    Wire.endTransmission();
  
    Wire.beginTransmission(0x20 + board_addr);
    Wire.write(ADDR_PORTA);                            // Port A selects the value to write
    Wire.write(val);                                   // Update the value
    Wire.endTransmission();
}

void populate_leds() {
  // registers
  leds[LEDS_BANK_REGA_IDX] = registers[REG_A_IDX];
  leds[LEDS_BANK_REGB_IDX] = registers[REG_B_IDX];
  leds[LEDS_BANK_REGC_IDX] = registers[REG_C_IDX];
  leds[LEDS_BANK_REGD_IDX] = registers[REG_D_IDX];
  leds[LEDS_PC_PC_IDX] = registers[REG_PC_IDX];
  leds[LEDS_RAM_MAR_IDX] = registers[REG_MAR_IDX];
  leds[LEDS_OUT_OUT_IDX] = registers[REG_OUT_IDX];

  bitWrite(leds[LEDS_ALU_FLAGS_CTRL_IDX], LEDS_ALU_FLAGS_CTRL_FLAG_Z_BIT, flags[FLAG_Z_IDX]);
  bitWrite(leds[LEDS_ALU_FLAGS_CTRL_IDX], LEDS_ALU_FLAGS_CTRL_FLAG_N_BIT, flags[FLAG_N_IDX]);
  bitWrite(leds[LEDS_ALU_FLAGS_CTRL_IDX], LEDS_ALU_FLAGS_CTRL_FLAG_C_BIT, flags[FLAG_C_IDX]);
  
}

void main_update() {
  update_leds(ADDR_ALU_I2C, EN_ALU_INA, leds[LEDS_ALU_INA_IDX]);
  update_leds(ADDR_ALU_I2C, EN_ALU_INB, leds[LEDS_ALU_INB_IDX]);
  update_leds(ADDR_ALU_I2C, EN_ALU_INA, leds[LEDS_ALU_OUT_IDX]);
  update_leds(ADDR_ALU_I2C, EN_ALU_OP_LEDS, leds[LEDS_ALU_OP_LEDS_IDX]);
  update_leds(ADDR_ALU_I2C, EN_ALU_FLAGS_CTRL, leds[LEDS_ALU_FLAGS_CTRL_IDX]);

  update_leds(ADDR_RAM_I2C, EN_RAM_OUT, leds[LEDS_ALU_FLAGS_CTRL_IDX]);
  update_leds(ADDR_RAM_I2C, EN_RAM_CTRL, leds[LEDS_ALU_FLAGS_CTRL_IDX]);
  update_leds(ADDR_RAM_I2C, EN_RAM_MAR, leds[LEDS_ALU_FLAGS_CTRL_IDX]);

  update_leds(ADDR_BANK_I2C, EN_BANK_REGA, leds[LEDS_BANK_REGA_IDX]);
  update_leds(ADDR_BANK_I2C, EN_BANK_REGB, leds[LEDS_BANK_REGB_IDX]);
  update_leds(ADDR_BANK_I2C, EN_BANK_REGC, leds[LEDS_BANK_REGC_IDX]);
  update_leds(ADDR_BANK_I2C, EN_BANK_REGD, leds[LEDS_BANK_REGD_IDX]);
  update_leds(ADDR_BANK_I2C, EN_BANK_IN_CTRL, leds[LEDS_BANK_IN_CTRL_IDX]);
  update_leds(ADDR_BANK_I2C, EN_BANK_AB, leds[LEDS_BANK_AB_IDX]);

  update_leds(ADDR_PC_I2C, EN_PC_PC, leds[LEDS_PC_PC_IDX]);
  update_leds(ADDR_PC_I2C, EN_PC_SEL, leds[LEDS_PC_SEL_IDX]);
  update_leds(ADDR_PC_I2C, EN_PC_CTRL, leds[LEDS_PC_CTRL_IDX]);
  
  update_leds(ADDR_OUT_I2C, EN_OUT_SEG4, leds[LEDS_OUT_SEG4_IDX]);
  update_leds(ADDR_OUT_I2C, EN_OUT_SEG3, leds[LEDS_OUT_SEG3_IDX]);
  update_leds(ADDR_OUT_I2C, EN_OUT_SEG2, leds[LEDS_OUT_SEG2_IDX]);
  update_leds(ADDR_OUT_I2C, EN_OUT_SEG1, leds[LEDS_OUT_SEG1_IDX]);
  update_leds(ADDR_OUT_I2C, EN_OUT_OUT, leds[LEDS_OUT_OUT_IDX]);  

  update_leds(ADDR_DECO_I2C, EN_DECO_IR, leds[LEDS_DECO_IR_IDX]);
  update_leds(ADDR_DECO_I2C, EN_DECO_CTRL_UINSTR, leds[LEDS_DECO_CTRL_UINSTR_IDX]);
  update_leds(ADDR_DECO_I2C, EN_DECO_DECO1, leds[LEDS_DECO_DECO1_IDX]);
  update_leds(ADDR_DECO_I2C, EN_DECO_DECO2, leds[LEDS_DECO_DECO2_IDX]);
  update_leds(ADDR_DECO_I2C, EN_DECO_DECO3, leds[LEDS_DECO_DECO3_IDX]);
  update_leds(ADDR_DECO_I2C, EN_DECO_DECO4, leds[LEDS_DECO_DECO4_IDX]);  
}



void clear_flags() {
  for (int i=0; i<3; ++i) 
    flags[i] = false;
}

// allowed registers are: a, b, c, d, out, stack
int string_to_reg_id(String reg) {
  reg.toLowerCase();
  if (reg.length() == 1) {
    if (reg[0] == 'a') return REG_A_IDX;
    else if (reg[0] == 'b') return REG_B_IDX;
    else if (reg[0] == 'c') return REG_C_IDX;
    else if (reg[0] == 'd') return REG_D_IDX;
    else return -1;
  } else if (reg == "pc") {
    return REG_PC_IDX;
  } else if (reg == "mar") {
    return REG_MAR_IDX;
  } else if (reg == "out") {
    return REG_OUT_IDX; 
  } else {
    return -1;
  }
}


// copies the value of register orig into register dest
void mov(String dest, String orig) {
  int ori_idx = string_to_reg_id(orig);
  int dst_idx = string_to_reg_id(dest);
  if ((ori_idx != -1) && (dst_idx != -1)) {
    registers[dst_idx] = registers[ori_idx];
  }else {
    Serial.println("Invalid origin or destination register");
  }

  update_register_leds();

  // Low level mov
  // 1) set  sel_in=dest
  //         sel_b=orig
  //         rd_reg=1
  //         wr_reg=1
  // optionally bus=reg[orig] value

  bitWrite(leds[LEDS_BANK_IN_CTRL_IDX], LEDS_BANK_IN_CTRL_IN0_BIT, dst_idx==0);
  bitWrite(leds[LEDS_BANK_IN_CTRL_IDX], LEDS_BANK_IN_CTRL_IN1_BIT, dst_idx==1);
  bitWrite(leds[LEDS_BANK_IN_CTRL_IDX], LEDS_BANK_IN_CTRL_IN2_BIT, dst_idx==2);
  bitWrite(leds[LEDS_BANK_IN_CTRL_IDX], LEDS_BANK_IN_CTRL_IN3_BIT, dst_idx==3);

  bitWrite(leds[LEDS_BANK_AB_IDX], LEDS_BANK_AB_B0_BIT, ori_idx==0);
  bitWrite(leds[LEDS_BANK_AB_IDX], LEDS_BANK_AB_B1_BIT, ori_idx==1);
  bitWrite(leds[LEDS_BANK_AB_IDX], LEDS_BANK_AB_B2_BIT, ori_idx==2);
  bitWrite(leds[LEDS_BANK_AB_IDX], LEDS_BANK_AB_B3_BIT, ori_idx==3);

  bitSet(leds[LEDS_BANK_IN_CTRL_IDX], LEDS_BANK_IN_CTRL_WR_REG_BIT);
  bitSet(leds[LEDS_BANK_IN_CTRL_IDX], LEDS_BANK_IN_CTRL_RD_REG_BIT);
}

// loads value k into register reg
void ldi(String dest, String k) {
  int dst_idx = string_to_reg_id(dest); 
  if (dst_idx != -1) {
    registers[dst_idx] = k.toInt(); 
  } else {
    Serial.println("Invalid destination register");
  }

  update_register_leds();
}

// loads memory value at address addr into register reg
void ld(String dest, String addr) {
  int dst_idx = string_to_reg_id(dest);
  if (dst_idx != -1) {
    registers[dst_idx] = ram[addr.toInt()]; 
  } else {
    Serial.println("Invalid destination register");
  }

  update_register_leds();
}

// stores the value of register reg into memory at address addr
void sto(String orig, String addr) {
  int ori_idx = string_to_reg_id(orig);
  if (ori_idx != -1) {
    ram[addr.toInt()] = registers[ori_idx];
  } else {
    Serial.println("Invalid origin register");
  }
}

// jumps to addr by setting PC=addr
void jmp(String addr) {
  registers[REG_PC_IDX] = addr.toInt();
  clear_flags();

  update_register_leds();
}

void jz(String addr) {
  if (flags[FLAG_Z_IDX]) {
    registers[REG_PC_IDX] = addr.toInt();
  } else {
    registers[REG_PC_IDX]++;
  }
  clear_flags();

  update_register_leds();
}

void jnz(String addr) {
  if (!flags[FLAG_Z_IDX]) {
    registers[REG_PC_IDX] = addr.toInt();
  } else {
    registers[REG_PC_IDX]++;
  }  
  clear_flags();

  update_register_leds();
}

void jn(String addr) {
  if (flags[FLAG_N_IDX]) {
    registers[REG_PC_IDX] = addr.toInt();
  } else {
    registers[REG_PC_IDX]++;
  }
  clear_flags();

  update_register_leds();
}

void jnn(String addr) {
  if (!flags[FLAG_N_IDX]) {
    registers[REG_PC_IDX] = addr.toInt();
  } else {
    registers[REG_PC_IDX]++;
  }
  clear_flags();

  update_register_leds();
}

void jc(String addr) {
  if (flags[FLAG_C_IDX]) {
    registers[REG_PC_IDX] = addr.toInt();
  } else {
    registers[REG_PC_IDX]++;
  }
  clear_flags();

  update_register_leds();
}

void jnc(String addr) {
  if (!flags[FLAG_C_IDX]) {
    registers[REG_PC_IDX] = addr.toInt();
  } else {
    registers[REG_PC_IDX]++;
  }
  clear_flags();

  update_register_leds();
}

void inc(String dest) {
  int dst_idx = string_to_reg_id(dest); 
  if (dst_idx != -1) {
    registers[dst_idx]++;
  } else {
    Serial.println("Invalid destination register");
  }

  update_register_leds();
}
void dec(String dest) {
  int dst_idx = string_to_reg_id(dest);  
  if (dst_idx != -1) {
    registers[dst_idx]--;
  } else {
    Serial.println("Invalid destination register");
  }

  update_register_leds();
}

void and_op(String dest, String orig) {
  int ori_idx = string_to_reg_id(orig);
  int dst_idx = string_to_reg_id(dest);  
  if ((ori_idx != -1) && (dst_idx != -1)) {
    registers[dst_idx] &= registers[ori_idx];
  } else {
    // handle error
  }
}
void or_op(String dest, String orig) {
  int ori_idx = string_to_reg_id(orig);
  int dst_idx = string_to_reg_id(dest);  
  if ((ori_idx != -1) && (dst_idx != -1)) {
    registers[dst_idx] |= registers[ori_idx];
  } else {
    // handle error
  }
}
void xor_op(String dest, String orig) {
  int ori_idx = string_to_reg_id(orig);
  int dst_idx = string_to_reg_id(dest);  
  if ((ori_idx != -1) && (dst_idx != -1)) {
    registers[dst_idx] ^= registers[ori_idx];
  } else {
    // handle error
  }
}
// dest = dest + orig
void add(String dest, String orig) {
  int ori_idx = string_to_reg_id(orig);
  int dst_idx = string_to_reg_id(dest);  
  if ((ori_idx != -1) && (dst_idx != -1)) {
    registers[dst_idx] += registers[ori_idx];
  } else {
    // handle error
  }
  
  // update flags
  clear_flags();
  if (registers[dst_idx] == 0) {
    flags[FLAG_Z_IDX] = true;
  } else if (registers[dst_idx] < 0){
    flags[FLAG_N_IDX] = true;
  } // carry flag is tricky, I have to calculate it BEFORE doing the operation, 
    // also I have to check if we work with signed integers (-127..128) or unsigned  (0..255)
}

// dest = dest + orig + carry
void addc(String dest, String orig) {
  int ori_idx = string_to_reg_id(orig);
  int dst_idx = string_to_reg_id(dest);  
  if ((ori_idx != -1) && (dst_idx != -1)) {
    registers[dst_idx] += registers[ori_idx] + flags[FLAG_C_IDX];
  } else {
    // handle error
  }
  // update flags
  clear_flags();
  if (registers[dst_idx] == 0) {
    flags[FLAG_Z_IDX] = true;
  } else if (registers[dst_idx] < 0){
    flags[FLAG_N_IDX] = true;
  } // carry flag is tricky, I have to calculate it BEFORE doing the operation, 
    // also I have to check if we work with signed integers (-127..128) or unsigned  (0..255)
}

// dest = dest - orig
void sub(String dest, String orig) {
  int ori_idx = string_to_reg_id(orig);
  int dst_idx = string_to_reg_id(dest);  
  if ((ori_idx != -1) && (dst_idx != -1)) {
    registers[dst_idx] -= registers[ori_idx];
  } else {
    // handle error
  }  
  
  // update flags
  clear_flags();
  if (registers[dst_idx] == 0) {
    flags[FLAG_Z_IDX] = true;
  } else if (registers[dst_idx] < 0){
    flags[FLAG_N_IDX] = true;
  } // carry flag is tricky, I have to calculate it BEFORE doing the operation, 
    // also I have to check if we work with signed integers (-127..128) or unsigned  (0..255)
}

// dest = dest - orig - carry
void subc(String dest, String orig) {
  int ori_idx = string_to_reg_id(orig);
  int dst_idx = string_to_reg_id(dest);  
  if ((ori_idx != -1) && (dst_idx != -1)) {
    registers[dst_idx] -= registers[ori_idx] + flags[FLAG_C_IDX];
  } else {
    // handle error
  }
  
  // update flags
  clear_flags();
  if (registers[dst_idx] == 0) {
    flags[FLAG_Z_IDX] = true;
  } else if (registers[dst_idx] < 0){
    flags[FLAG_N_IDX] = true;
  } // carry flag is tricky, I have to calculate it BEFORE doing the operation, 
    // also I have to check if we work with signed integers (-127..128) or unsigned  (0..255)
}

void print_registers() {
  Serial.print("A:    ");
  Serial.println(registers[REG_A_IDX]);
  Serial.print("B:    ");
  Serial.println(registers[REG_B_IDX]);
  Serial.print("C:    ");
  Serial.println(registers[REG_C_IDX]);
  Serial.print("D:    ");
  Serial.println(registers[REG_D_IDX]);
  Serial.print("MAR:  ");
  Serial.println(registers[REG_MAR_IDX]);
  Serial.print("OUT:  ");
  Serial.print("PC:   ");
  Serial.println(registers[REG_PC_IDX]);
  Serial.print("uINS: ");
  Serial.println(uinstr_val);
  Serial.println(registers[REG_OUT_IDX]);
  Serial.print("FLAGS (Z, N, C): ");
  Serial.print(flags[FLAG_Z_IDX]);
  Serial.print(" ");
  Serial.print(flags[FLAG_N_IDX]);
  Serial.print(" ");
  Serial.println(flags[FLAG_C_IDX]);
}

void process_instruction() {
  int first_space_idx = rx.indexOf(" ");
  int second_space_idx = rx.indexOf(" ", first_space_idx);

  String instr = rx.substring(0, first_space_idx);
  String op1 = rx.substring(first_space_idx, second_space_idx);
  String op2 = rx.substring(second_space_idx, rx.length());

  instr.toLowerCase();
  
  if (instr == "mov") {          // move value from one register to another register
    mov(op1, op2);  
  } else if (instr == "ld") {    // load value from ram address to register
    ld(op1, op2);
  } else if (instr == "ldi") {   // load constant to register
    ldi(op1, op2);
  } else if (instr == "jmp") {   // unconditional jump
    jmp(op1);
  } else if (instr == "jz") {    // jump if zero
    jz(op1);
  } else if (instr == "jnz") {   // jump if not zero
    jnz(op1);
  } else if (instr == "jn") {    // jump if negative
    jn(op1);
  } else if (instr == "jnn") {   // jump if not negative
    jnn(op1);
  } else if (instr == "jc") {    // jump if carry
    jc(op1);
  } else if (instr == "jnc") {   // jump if not carry
    jnc(op1);
  } else if (instr == "add") {   // op1 = op1 + op2
    add(op1, op2);
  } else if (instr == "addc") {  // op1 = op1 + op2 + carry
    addc(op1, op2);
  } else if (instr == "sub") {   // op1 = op1 - op2
    sub(op1, op2);
  } else if (instr == "subc") {  // op1 = op1 - op2 - carry
    subc(op1, op2);
  } else if (instr == "inc") {  // op1 = op1 + 1
    inc(op1);
  } else if (instr == "dec") {  // op1 = op1 - 1
    dec(op1);
  } else if (instr == "and") {  // op1 = op1 and op2
    and_op(op1, op2);      
  } else if (instr == "or") {  // op1 = op1 or op2
    or_op(op1, op2);      
  } else if (instr == "xor") {  // op1 = op1 xor op2
    xor_op(op1, op2);      
  } else if (rx[0] == '.') {     // a dot will be considered like a digital push to clk
    uinstr_val++;
  } else {
    Serial.println("ERROR");
    return;
  }
  Serial.println("OK");
  rx = "";
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  rx.reserve(20);

  // needs to be debounced
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), on_clock, RISING);
  
}

void loop() {
  
}

// this is a test for the boards,
// the received string must be in the format:
// i2c_addr block_num value
// eg:  1 0 24 will write 24 to the block 0 of the board 1
void test_process_rx() {
  int first_space_idx = rx.indexOf(" ");
  int second_space_idx = rx.indexOf(" ", first_space_idx);

  uint8_t board_addr = rx.substring(0, first_space_idx).toInt();
  uint8_t block_num = rx.substring(first_space_idx, second_space_idx).toInt();
  uint8_t val = rx.substring(second_space_idx, rx.length()).toInt();

  Serial.print("Board address: ");
  Serial.println(board_addr);
  Serial.print("Block num:     ");
  Serial.println(block_num);
  Serial.print("Value:         ");
  Serial.println(val);
  Serial.println("");

  update_leds(board_addr, block_num, val);
}
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    rx += inChar;
    if (inChar == '\n') {
      test_process_rx();
    }
  }
}
