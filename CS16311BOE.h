#pragma once
#include <Arduino.h>

class CS16311BOE {
public:
  // Iconos globales (ADDR 36)
  enum GlobalIcon : uint8_t {
    ICON_USB  = 0,
    ICON_PLAY = 1,
    ICON_REC  = 2
  };

  CS16311BOE();

  // Parametrizable
  void begin(uint8_t stb, uint8_t clk, uint8_t din, uint8_t dout, uint8_t brightness = 15);

  // Conveniencia para tu banco de pruebas (STB=10 CLK=13 DIN=11 DOUT=12)
  void beginDefault(uint8_t brightness = 15);

  void setBrightness(uint8_t level0_15);
  void clear();

  // POS: 0 = 2G (derecha), 10 = 12G (izquierda)
  void putChar(uint8_t pos, char ch);
  void putRaw(uint8_t pos, uint8_t base, uint8_t extra, uint8_t third = 0);

  // Humana: s[0] va al caracter más a la izquierda (POS=10) y s[10] al más a la derecha (POS=0)
  void print11(const char* s);

  // Icono por carácter (EXTRA bit 0x40) - útil como indicador IR, etc.
  void setCharIcon(uint8_t pos, bool on);

  // Iconos globales (USB/PLAY/REC) en ADDR 36
  void setGlobalIcon(GlobalIcon icon, bool on);
  void setGlobalIcons(bool usb, bool play, bool rec);
  void clearGlobalIcons();

  // Barras R1..R10 (bit0=R1 ... bit9=R10)
  void setBars(uint16_t mask10);
  void setBar(uint8_t index1_10, bool on);   // index 1..10
  uint16_t getBars() const;

  // Teclado
  void readKeys(uint8_t k[4]);
  uint8_t readK0();

  // Utilidad: devuelve base/extra traducidos desde ASCII (sin escribir)
  void glyphFromChar(char ch, uint8_t &base, uint8_t &extra) const;

private:
  uint8_t _stb, _clk, _din, _dout;
  uint8_t _brightness;
  uint8_t _icon36Shadow;   // sombra de ADDR36
  uint16_t _barsShadow;    // sombra barras R1..R10

  // Low-level
  void wrByte(uint8_t b);
  void sendCmd(uint8_t c);
  void writeOne(uint8_t addr, uint8_t val);

  uint8_t rdByteLSB();
  void displayModeInit();

  // Addressing
  static uint8_t baseAddr(uint8_t pos)  { return 3 + pos*3; }
  static uint8_t extraAddr(uint8_t pos) { return 4 + pos*3; }
  static uint8_t thirdAddr(uint8_t pos) { return 5 + pos*3; }

  // Madison -> BOE
  void madisonToBOE(uint16_t pat, uint8_t &base, uint8_t &extra) const;

  // Helpers
  static bool isPrintable(char c) { return (c >= 32 && c <= 126); }
};
