int MOSI_pin = 11;
int SlaveSelect_pin = 10;
int Clock_pin = 13;

byte max7219_reg_noop        = 0x00;
byte max7219_reg_decodeMode  = 0x09;
byte max7219_reg_intensity   = 0x0a;
byte max7219_reg_scanLimit   = 0x0b;
byte max7219_reg_shutdown    = 0x0c;
byte max7219_reg_displayTest = 0x0f;

// imaginea logica 16x8: bit15 = x=0 ... bit0 = x=15
uint16_t frame[8];

void putByte(byte data) {
  for (int i = 7; i >= 0; i--) {
    digitalWrite(Clock_pin, LOW);
    digitalWrite(MOSI_pin, (data >> i) & 1);
    digitalWrite(Clock_pin, HIGH);
  }
}

void toMax2(byte reg, byte left, byte right) {
  digitalWrite(SlaveSelect_pin, LOW);
  putByte(reg); putByte(left);    // primul  -> modulul 2 (fizic stanga)
  putByte(reg); putByte(right);   // ultimul -> modulul 1 (fizic dreapta)
  digitalWrite(SlaveSelect_pin, HIGH);
}

void clearFrame() {
  for (int y = 0; y < 8; y++) frame[y] = 0;
}

void setPixel(int x, int y, bool on = true) {
  if (x < 0 || x > 15 || y < 0 || y > 7) return;
  uint16_t mask = (uint16_t)1 << (15 - x);
  if (on) frame[y] |= mask;
  else    frame[y] &= ~mask;
}

void drawRectangle() {
  frame[0] = 0xFFFF;
  frame[7] = 0xFFFF;
  for (int y = 1; y < 7; y++) frame[y] = 0x8001;
}

void flush() {
  for (int y = 0; y < 8; y++) {
    toMax2(y + 1, frame[y] >> 8, frame[y] & 0xFF);
  }
}

// val circular care porneste din (cx, cy)
void drawRipple(int cx, int cy) {
  for (int r = 1; r <= 17; r++) {
    int r2 = r * r;
    int inner = (r - 1) * (r - 1);

    clearFrame();
    // drawRectangle();   // decomenteaza daca vrei sa ramana dreptunghiul

    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 16; x++) {
        int dx = x - cx;
        int dy = y - cy;
        int d2 = dx * dx + dy * dy;

        if (d2 <= r2 && d2 > inner) {
          setPixel(x, y);
        }
      }
    }

    flush();
    delay(45);
  }
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
  static int x = 0, y = 0;
  static int dx = 1, dy = 1;
  static bool wasNearCorner = true;

  // deseneaza dreptunghiul + punctul
  clearFrame();
  drawRectangle();
  setPixel(x, y);
  flush();

  delay(80);

  // misca punctul
  x += dx;
  y += dy;

  // bounce pe margini
  if (x <= 0)  { x = 0;  dx =  1; }
  if (x >= 15) { x = 15; dx = -1; }
  if (y <= 0)  { y = 0;  dy =  1; }
  if (y >= 7)  { y = 7;  dy = -1; }

  // a lovit efectiv o margine?
  bool hitX = (x == 0 || x == 15);
  bool hitY = (y == 0 || y == 7);

  // e colt doar daca loveste o margine si cealalta coordonata e langa colt
  bool cornerHit = (hitX && (y <= 1 || y >= 6)) ||
                   (hitY && (x <= 1 || x >= 14));

  if (cornerHit && !wasNearCorner) {
    int cx = (x < 8) ? 0 : 15;
    int cy = (y < 4) ? 0 : 7;
    drawRipple(cx, cy);
  }

  wasNearCorner = cornerHit;
}