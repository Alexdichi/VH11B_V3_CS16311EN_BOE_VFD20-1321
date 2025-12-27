#include "CS16311BOE.h"

CS16311BOE vfd;

void setup(){
  Serial.begin(115200);
  vfd.beginDefault(15);
  vfd.print11("VH11BV3 LIB");
  vfd.setGlobalIcons(true, false, false); // USB ON
  vfd.setBars(0x0001); // R1
}

void loop(){
  uint8_t k0 = vfd.readK0();
  // S6=0x10 avanza barras, S5=0x20 retrocede, S1=0x02 limpia
  if(k0 & 0x10) vfd.setBars((vfd.getBars() << 1) ? (vfd.getBars() << 1) : 1);
  if(k0 & 0x20) vfd.setBars((vfd.getBars() >> 1) ? (vfd.getBars() >> 1) : 0x0200);
  if(k0 & 0x02) { vfd.setBars(0); vfd.clearGlobalIcons(); vfd.print11("RESET      "); }
  delay(150);
}
