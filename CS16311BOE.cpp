#include "CS16311BOE.h"

// =================== BOE segment bits (confirmados) ===================
// BASE
static const uint8_t BOE_d = 0x01;
static const uint8_t BOE_e = 0x02;
static const uint8_t BOE_r = 0x04;  // standard L
static const uint8_t BOE_p = 0x08;  // standard M
static const uint8_t BOE_n = 0x10;  // standard N
static const uint8_t BOE_c = 0x20;
static const uint8_t BOE_m = 0x40;  // standard G2
static const uint8_t BOE_g = 0x80;  // standard G1

// EXTRA
static const uint8_t BOE_f    = 0x01;
static const uint8_t BOE_h    = 0x02;
static const uint8_t BOE_j    = 0x04;
static const uint8_t BOE_k    = 0x08;
static const uint8_t BOE_b    = 0x10;
static const uint8_t BOE_a    = 0x20;
static const uint8_t BOE_icon = 0x40; // icono por carácter (EXTRA)

// =================== Tabla David Madison (ASCII 32..127) en PROGMEM ===================
// bits: A B C D E F G1 G2 H J K L M N DP
static const uint16_t FourteenSegmentASCII[96] PROGMEM = {
  0b000000000000000,0b100000000000110,0b000001000000010,0b001001011001110,
  0b001001011101101,0b011111111100100,0b010001101011001,0b000001000000000,
  0b010010000000000,0b000100100000000,0b011111111000000,0b001001011000000,
  0b000100000000000,0b000000011000000,0b100000000000000,0b000110000000000,
  0b000110000111111,0b000010000000110,0b000000011011011,0b000000010001111,
  0b000000011100110,0b010000001101001,0b000000011111101,0b000000000000111,
  0b000000011111111,0b000000011101111,0b001001000000000,0b000101000000000,
  0b010010001000000,0b000000011001000,0b000100110000000,0b101000010000011,
  0b000001010111011,0b000000011110111,0b001001010001111,0b000000000111001,
  0b001001000001111,0b000000001111001,0b000000001110001,0b000000010111101,
  0b000000011110110,0b001001000001001,0b000000000011110,0b010010001110000,
  0b000000000111000,0b000010100110110,0b010000100110110,0b000000000111111,
  0b000000011110011,0b010000000111111,0b010000011110011,0b000000011101101,
  0b001001000000001,0b000000000111110,0b000110000110000,0b010100000110110,
  0b010110100000000,0b000000011101110,0b000110000001001,0b000000000111001,
  0b010000100000000,0b000000000001111,0b010100000000000,0b000000000001000,
  0b000000100000000,0b001000001011000,0b010000001111000,0b000000011011000,
  0b000100010001110,0b000100001011000,0b001010011000000,0b000010010001110,
  0b001000001110000,0b001000000000000,0b000101000010000,0b011011000000000,
  0b000000000110000,0b001000011010100,0b001000001010000,0b000000011011100,
  0b000000101110000,0b000010010000110,0b000000001010000,0b010000010001000,
  0b000000001111000,0b000000000011100,0b000100000010000,0b010100000010100,
  0b010110100000000,0b000001010001110,0b000100001001000,0b000100101001001,
  0b001001000000000,0b010010010001001,0b000110011000000,0b000000000000000
};

CS16311BOE::CS16311BOE()
: _stb(10), _clk(13), _din(11), _dout(12),
  _brightness(15), _icon36Shadow(0), _barsShadow(0) {}

void CS16311BOE::beginDefault(uint8_t brightness){
  begin(10, 13, 11, 12, brightness);
}

void CS16311BOE::begin(uint8_t stb, uint8_t clk, uint8_t din, uint8_t dout, uint8_t brightness){
  _stb = stb; _clk = clk; _din = din; _dout = dout;
  _brightness = (brightness > 15) ? 15 : brightness;
  _icon36Shadow = 0;
  _barsShadow = 0;

  pinMode(_stb, OUTPUT);
  pinMode(_clk, OUTPUT);
  pinMode(_din, OUTPUT);
  pinMode(_dout, INPUT);

  digitalWrite(_stb, HIGH);
  digitalWrite(_clk, HIGH);

  displayModeInit();
  setBrightness(_brightness);
  clear();
  clearGlobalIcons();
  setBars(0);
}

void CS16311BOE::displayModeInit(){
  // Modo display (según pruebas)
  sendCmd(0x0C);
}

void CS16311BOE::setBrightness(uint8_t level0_15){
  _brightness = (level0_15 > 15) ? 15 : level0_15;
  // 0x88..0x8F => display ON + brillo
  sendCmd((uint8_t)(0x88 | _brightness));
}

void CS16311BOE::clear(){
  for(uint8_t i=0;i<64;i++) writeOne(i, 0x00);
}

void CS16311BOE::wrByte(uint8_t b){
  for(uint8_t i=0;i<8;i++){
    digitalWrite(_clk, LOW);
    digitalWrite(_din, (b & 0x01) ? HIGH : LOW);
    b >>= 1;
    digitalWrite(_clk, HIGH);
  }
}

void CS16311BOE::sendCmd(uint8_t c){
  digitalWrite(_stb, LOW);
  wrByte(c);
  digitalWrite(_stb, HIGH);
}

void CS16311BOE::writeOne(uint8_t addr, uint8_t val){
  sendCmd(0x40); // write, auto-inc
  digitalWrite(_stb, LOW);
  wrByte((uint8_t)(0xC0 | (addr & 0x3F)));
  wrByte(val);
  digitalWrite(_stb, HIGH);
}

uint8_t CS16311BOE::rdByteLSB(){
  uint8_t b = 0;
  for(uint8_t i=0;i<8;i++){
    digitalWrite(_clk, LOW);
    if(digitalRead(_dout)) b |= (1 << i);
    digitalWrite(_clk, HIGH);
  }
  return b;
}

void CS16311BOE::readKeys(uint8_t k[4]){
  digitalWrite(_stb, LOW);
  wrByte(0x42); // read key scan
  k[0] = rdByteLSB();
  k[1] = rdByteLSB();
  k[2] = rdByteLSB();
  k[3] = rdByteLSB();
  digitalWrite(_stb, HIGH);
}

uint8_t CS16311BOE::readK0(){
  uint8_t k[4];
  readKeys(k);
  return k[0];
}

void CS16311BOE::madisonToBOE(uint16_t pat, uint8_t &base, uint8_t &extra) const{
  base = 0; extra = 0;

  if(pat & (1<<0))  extra |= BOE_a; // A
  if(pat & (1<<1))  extra |= BOE_b; // B
  if(pat & (1<<2))  base  |= BOE_c; // C
  if(pat & (1<<3))  base  |= BOE_d; // D
  if(pat & (1<<4))  base  |= BOE_e; // E
  if(pat & (1<<5))  extra |= BOE_f; // F

  if(pat & (1<<6))  base  |= BOE_g; // G1 -> g
  if(pat & (1<<7))  base  |= BOE_m; // G2 -> m

  if(pat & (1<<8))  extra |= BOE_h; // H
  if(pat & (1<<9))  extra |= BOE_j; // J
  if(pat & (1<<10)) extra |= BOE_k; // K

  if(pat & (1<<11)) base  |= BOE_r; // L -> r
  if(pat & (1<<12)) base  |= BOE_p; // M -> p
  if(pat & (1<<13)) base  |= BOE_n; // N -> n
}

void CS16311BOE::glyphFromChar(char ch, uint8_t &base, uint8_t &extra) const{
  if(!isPrintable(ch)) ch = ' ';
  uint16_t pat = (uint16_t)pgm_read_word(&FourteenSegmentASCII[(uint8_t)ch - 32]);
  madisonToBOE(pat, base, extra);
}

void CS16311BOE::putRaw(uint8_t pos, uint8_t base, uint8_t extra, uint8_t third){
  if(pos > 10) return;
  writeOne(baseAddr(pos),  base);
  writeOne(extraAddr(pos), extra);
  writeOne(thirdAddr(pos), third);
}

void CS16311BOE::putChar(uint8_t pos, char ch){
  uint8_t base, extra;
  glyphFromChar(ch, base, extra);
  putRaw(pos, base, extra, 0);
}

void CS16311BOE::print11(const char* s){
  // humana: s[0] = izquierda (POS=10) ... s[10] = derecha (POS=0)
  for(int pos = 10; pos >= 0; --pos){
    char ch = (s && *s) ? *s++ : ' ';
    putChar((uint8_t)pos, ch);
  }
}

void CS16311BOE::setCharIcon(uint8_t pos, bool on){
  if(pos > 10) return;
  // No tenemos read-back de RAM, así que: generamos el glyph actual del char NO lo sabemos.
  // Uso recomendado: llamar después de escribir el carácter (putChar/print11).
  // Acá solo tocamos el bit del icono por carácter:
  uint8_t ex = (uint8_t)(on ? BOE_icon : 0x00);
  writeOne(extraAddr(pos), ex);
}

void CS16311BOE::setGlobalIcon(GlobalIcon icon, bool on){
  uint8_t bit = 0;
  switch(icon){
    case ICON_USB:  bit = 0x01; break;
    case ICON_PLAY: bit = 0x02; break;
    case ICON_REC:  bit = 0x04; break;
    default: return;
  }
  if(on) _icon36Shadow |= bit;
  else   _icon36Shadow &= (uint8_t)~bit;
  writeOne(36, _icon36Shadow);
}

void CS16311BOE::setGlobalIcons(bool usb, bool play, bool rec){
  _icon36Shadow = 0;
  if(usb)  _icon36Shadow |= 0x01;
  if(play) _icon36Shadow |= 0x02;
  if(rec)  _icon36Shadow |= 0x04;
  writeOne(36, _icon36Shadow);
}

void CS16311BOE::clearGlobalIcons(){
  _icon36Shadow = 0;
  writeOne(36, 0);
}

void CS16311BOE::setBars(uint16_t mask10){
  mask10 &= 0x03FF; // 10 bits
  _barsShadow = mask10;

  // R1..R6 en ADDR0 bits2..7
  uint8_t a0 = 0;
  for(uint8_t i=0;i<6;i++){
    if(mask10 & (1<<i)) a0 |= (1<<(i+2));
  }
  // R7..R10 en ADDR1 bits0..3
  uint8_t a1 = 0;
  for(uint8_t i=0;i<4;i++){
    if(mask10 & (1<<(i+6))) a1 |= (1<<i);
  }
  writeOne(0, a0);
  writeOne(1, a1);
}

void CS16311BOE::setBar(uint8_t index1_10, bool on){
  if(index1_10 < 1 || index1_10 > 10) return;
  uint16_t bit = (uint16_t)(1u << (index1_10 - 1));
  uint16_t m = _barsShadow;
  if(on) m |= bit;
  else   m &= (uint16_t)~bit;
  setBars(m);
}

uint16_t CS16311BOE::getBars() const{
  return _barsShadow;
}
