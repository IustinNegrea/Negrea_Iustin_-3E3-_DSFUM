int MOSI_pin = 11;
int SlaveSelect_pin = 10;
int Clock_pin = 13;

byte max7219_reg_noop        = 0x00;
byte max7219_reg_decodeMode  = 0x09;
byte max7219_reg_intensity   = 0x0a;
byte max7219_reg_scanLimit   = 0x0b;
byte max7219_reg_shutdown    = 0x0c;
byte max7219_reg_displayTest = 0x0f;

void putByte(byte data) {
  for (int i = 7; i >= 0; i--) {
    digitalWrite(Clock_pin, LOW);
    digitalWrite(MOSI_pin, (data >> i) & 1);
    digitalWrite(Clock_pin, HIGH);
  }
}

// left  -> modulul 2 (fizic stanga)
// right -> modulul 1 (fizic dreapta)
void toMax2(byte reg, byte left, byte right) {
  digitalWrite(SlaveSelect_pin, LOW);
  putByte(reg); putByte(left);    // primii 16 biti  -> modulul 2
  putByte(reg); putByte(right);   // ultimii 16 biti -> modulul 1
  digitalWrite(SlaveSelect_pin, HIGH);
}

void setup() {
  pinMode(MOSI_pin, OUTPUT);
  pinMode(Clock_pin, OUTPUT);
  pinMode(SlaveSelect_pin, OUTPUT);

  digitalWrite(Clock_pin, HIGH);

  toMax2(max7219_reg_scanLimit,   0x07, 0x07);
  toMax2(max7219_reg_decodeMode,  0x00, 0x00);
  toMax2(max7219_reg_shutdown,    0x01, 0x01);
  toMax2(max7219_reg_displayTest, 0x00, 0x00);

  for (int i = 1; i <= 8; i++) {
    toMax2(i, 0, 0);
  }

  toMax2(max7219_reg_intensity, 0x05, 0x05);
}

void loop() {
  static int y  = 0;   // randul logic de sus al liniei (0..15)
  static int dy = 1;
  const int thickness = 3;   // cate randuri groase

  for (int row = 1; row <= 8; row++) {
    byte left = 0;
    byte right = 0;

    for (int t = 0; t < thickness; t++) {
      int ly = y + t;              // randul logic aprins
      if (ly < 0 || ly > 15) continue;

      if (ly < 8) {
        if (row == ly + 1) left = B00011000;
      } else {
        if (row == (ly - 8) + 1) right = B00110000;
      }
    }

    toMax2(row, left, right);
  }

  delay(100);

  y += dy;

  // limita de jos scade cu grosimea, altfel iese din matrice
  if (y > 15 - (thickness - 1)) { y = 15 - (thickness - 1); dy = -1; }
  else if (y < 0)               { y = 0;  dy =  1; }
}

/*
void loop() {
  static int y  = 0;   // 0..15
  static int dy = 1;   // +1 coboara, -1 urca
  const int col = 3;   // coloana fixa in interiorul matricei (0..7)

  for (int row = 1; row <= 8; row++) {
    byte left = 0;
    byte right = 0;
if (y < 8) {
  if (row == y + 1) left = B00111100;          // linie pe primul ecran
} else {
  if (row == (y - 8) + 1) right = 0x3c;   // linie pe al doilea ecran
}

    toMax2(row, left, right);
  }

  delay(100);

  y += dy;

  if (y > 15)      { y = 15; dy = -1; }
  else if (y < 0)  { y = 0;  dy =  1; }
}
*/
/*
void loop() {
  static int x = 0;
  static int y = 0;
  static int dir = 1;

  for (int row = 1; row <= 8; row++) {
    byte left = 0;
    byte right = 0;

    // dreptunghiul
    //if (row == 1 || row == 8) {
   //   left = 0xFF;
   //   right = 0xFF;
   // } else {
   //   left = 0x80;
   //   right = 0x01;
   // }

    // punctul care se misca pe randul 4
    if (row == 4| row == 5) {
      if (x < 16) left  |= (0x80 >> y);
      else       right |= (0x80 >> (x - 8));
    }

    toMax2(row, left, right);
  }

  delay(50);

  x += dir;
  if (x >= 15) { x = 15; dir = -1; }
  if (x <= 0)  { x = 0;  dir =  1; }
}*/

