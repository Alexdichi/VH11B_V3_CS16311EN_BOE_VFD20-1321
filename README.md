# VH11B_V3_CS16311EN_BOE_VFD20-1321

Arduino library and technical documentation for the **VH11B-V3** board using the **CS16311EN** VFD controller and the **BOE VFD20-1321** display.

This project is based on **real hardware reverse engineering and extensive empirical testing**.  
No complete official schematics were available. All functionality was validated directly on the hardware.
![1766806656031](https://github.com/user-attachments/assets/c050e405-5678-483b-ac7b-0029c3c4b8c2)

---

## ✨ Features

- Full control of a **14-segment alphanumeric VFD display**
- Support for **11 characters**
- Control of **icons and bar indicators**
- **Keyboard scanning** via CS16311 (DOUT-based)
- **External IR receiver support** (independent from CS16311)
- Clear abstraction for:
  - text
  - icons
  - bars
  - raw segments
- Designed for **DIY electronics, maker projects, instrumentation and retro displays**

---

## 🧠 Hardware Overview

- **Board:** VH11B-V3 (front panel from satellite receivers)
- **VFD Controller:** CS16311EN (CS16311 compatible)
- **Display:** BOE VFD20-1321
- **MCU:** Arduino Nano (ATmega328P)
- **Inputs:**
  - Integrated keyboard matrix (via CS16311)
  - External IR receiver (direct to MCU)

> ⚠️ **Important**  
> **DIN and DOUT are NOT the same line.**  
> Keyboard reading works **only via DOUT**.  
> Attempting to read keys via DIN does **not** work.

---

## 🔌 Board Connector (7-pin header)

The VH11B-V3 board exposes a **7-pin connector**, not a direct CS16311 header.

One pin is dedicated to the **IR receiver output** and does **not** pass through the CS16311.  
The remaining pins provide serial control and power for the CS16311.

### 7-pin Connector Pinout

| Pin | Signal | Description |
|----:|--------|-------------|
| 1 | IR_OUT | IR receiver output (direct to MCU, not via CS16311) |
| 2 | STB | CS16311 STB (latch / strobe) |
| 3 | CLK | CS16311 CLK (serial clock) |
| 4 | DIN | Data MCU → CS16311 |
| 5 | DOUT | Data CS16311 → MCU (keyboard) |
| 6 | +5V | Logic supply (routed through **two series diodes**) |
| 7 | GND | Ground |

### Power note

- The **+5V line does NOT go directly** to the CS16311.
- It passes through **two series diodes** before reaching **CS16311 VDD (pin 14)**.
- Voltage drop: approximately **1.2–1.4 V** (confirmed by board tracing).

---

## 🔌 Example Arduino Nano Wiring

| Board Pin | Signal | Arduino Nano |
|----------:|--------|--------------|
| 1 | IR_OUT | D2 |
| 2 | STB | D10 |
| 3 | CLK | D13 |
| 4 | DIN | D11 |
| 5 | DOUT | D12 |
| 6 | +5V | 5V |
| 7 | GND | GND |

---

## 🔤 Display Capabilities

- **14-segment alphanumeric characters**
- **11 character positions** (2G … 12G)
- **3 RAM bytes per character**
- Custom segment mapping for BOE display
- ASCII font adapted from **FourteenSegmentASCII**
---

## 🎛 Icons and Bars

- Dedicated RAM addresses for icons (USB, PLAY, REC, etc.)
- **10-step bar indicators** (R1–R10)

Example API:

```
display.setIcon(ICON_USB, true);
display.setBarLevel(5);
```

⌨ Keyboard

Scanned internally by CS16311

Read using command 0x42

Data output via DOUT

4-byte key data (K0–K3)

❌ Do NOT use INPUT_PULLUP on DOUT

📁 Repository Structure (planned)
/docs        → Technical documentation
/src         → Driver and display abstraction
/examples    → Usage examples
/fonts       → 14-segment font tables

🚀 Project Status

✔ Display fully working

✔ ASCII text rendering validated

✔ Icons and bars mapped

✔ Keyboard input working

✔ IR input verified

✔ Solid base for a reusable library

📜 Philosophy

This project documents what actually works, not just what datasheets claim.

It is intended to help makers reuse VFD hardware that would otherwise be discarded and make it usable in real-world projects.

If it works on this board, it works in real life.

🙏 Credits

The 14-segment ASCII font mapping used in this project was inspired by open-source work from the maker community and adapted specifically for the BOE VFD20-1321 display and CS16311 RAM layout.

Original reference:
https://github.com/dmadison/LED-Segment-ASCII

📄 License

To be defined.

🤝 Contributions

Contributions, testing on similar hardware, and documentation improvements are welcome.
If you reuse this display in your own project, feel free to share your results.
