int MOSI_pin = 11;
int SlaveSelect_pin = 10;
int Clock_pin = 13;


// apoi stabilim cateva zone memorabile din max:
byte max7219_reg_noop        = 0x00; // nu face nimic
byte max7219_reg_digit0      = 0x01; // seteaza primul rand
byte max7219_reg_digit1      = 0x02;
byte max7219_reg_digit2      = 0x03;
byte max7219_reg_digit3      = 0x04;
byte max7219_reg_digit4      = 0x05;
byte max7219_reg_digit5      = 0x06;
byte max7219_reg_digit6      = 0x07;
byte max7219_reg_digit7      = 0x08;
byte max7219_reg_digit8      = 0x09;
byte max7219_reg_digit9      = 0x10;
byte max7219_reg_digit10     = 0x11;
byte max7219_reg_digit11     = 0x12;
byte max7219_reg_digit12     = 0x13;
byte max7219_reg_digit13     = 0x14;
byte max7219_reg_digit14     = 0x15;
byte max7219_reg_digit15     = 0x16;
byte max7219_reg_decodeMode  = 0x09;
byte max7219_reg_intensity   = 0x0a;
byte max7219_reg_scanLimit   = 0x0b;
byte max7219_reg_shutdown    = 0x0c;
byte max7219_reg_displayTest = 0x0f;

void putByte(byte data) { // trebuie sa scriem bitii din data pe rand, de la stg la dreapta, sincronizat
  for (int i=7; i>=0; i--)
  {  
    digitalWrite(Clock_pin, LOW);   // incepe o scriere sincronizata
    digitalWrite(MOSI_pin, data & (0x01 << i)); // cand i=7 scriu cel mai din stg bit din data...
    digitalWrite(Clock_pin, HIGH);   // a terminat sincronismul
  }
}


void toMax( byte reg, byte col) {    
  digitalWrite(SlaveSelect_pin, LOW);       // selectez slaveul    
  putByte(reg);                  // ii zic in ce zona de memorie scriu
  putByte(col);                  // si scriu valoarea
  digitalWrite(SlaveSelect_pin,HIGH);       // deselectez slaveul ca sa activez ce am scris
}

void toMax2(byte reg, byte left, byte right) {
  digitalWrite(SlaveSelect_pin, LOW);

  putByte(reg);  putByte(left);   // primii 16 biți  -> modulul 2 (dreapta)
  putByte(reg);  putByte(right);    // ultimii 16 biți -> modulul 1 (stânga)

  digitalWrite(SlaveSelect_pin, HIGH);
}

void setup() {

  pinMode(MOSI_pin, OUTPUT);
  pinMode(Clock_pin,  OUTPUT);
  pinMode(SlaveSelect_pin,   OUTPUT);

  digitalWrite(Clock_pin, HIGH);  

// cateva setari:
  toMax(max7219_reg_scanLimit, 0x07);      
  toMax(max7219_reg_decodeMode, 0x00);  
  toMax(max7219_reg_shutdown, 0x01);    // nu e in shutdown
  toMax(max7219_reg_displayTest, 0x00); // nu testez nimic
   for (int i=1; i<=8; i++) {    
    toMax(i,0);                         // curat fiecare rand
  }
  toMax(max7219_reg_intensity, 0x00);   // setez intensitatea la minim                                                 
}

void loop() {
   toMax2(1,B11111111, B11111111);
   toMax2(2,B10000000, B00000001);
   toMax2(3,B10000000, B00000001);
   toMax2(4,B10000000, B00000001);
   toMax2(5,B10000000, B00000001);
   toMax2(6,B10000000, B00000001);
   toMax2(7,B10000000, B00000001);
   toMax2(8,B11111111, B11111111);
  
}
