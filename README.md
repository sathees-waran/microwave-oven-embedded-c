# Microwave Oven Controller — Embedded C (PIC16F877A)

A fully functional **Microwave Oven simulation** built using **Embedded C** on a **PIC16F877A microcontroller**, simulated on **PICSIMLAB 4**. The system replicates real microwave oven behavior with multiple cooking modes, timer control, and a character LCD interface.

---

## Project Structure

```
MICROWAVE/
├── main.c              # Entry point, system initialization
├── micro_oven.c        # Core microwave oven logic & state machine
├── micro_oven.h
├── clcd.c              # Character LCD (HD44780) driver
├── clcd.h
├── matrix_keypad.c     # 4x3 Matrix keypad driver
├── matrix_keypad.h
├── timers.c            # Timer configuration and ISR helpers
├── timers.h
├── isr.c               # Interrupt Service Routines
├── Makefile            # Build configuration
└── docs/
    ├── board4_Schematic_v0.8.8.pdf     # PICSIMLAB 4 board schematic
    ├── pic16f877x_datasheet.pdf        # PIC16F877A datasheet
    └── hd44780.pdf                     # HD44780 CLCD datasheet
```

---

## Hardware Components

| Component | Details |
|---|---|
| Microcontroller | PIC16F877A |
| Display | HD44780 16x4 Character LCD (CLCD) |
| Input | 4x3 Matrix Keypad (SW1–SW12) |
| Output | Fan (via relay), Buzzer | 
| IDE | MPLAB X IDE |
| Compiler | XC8 Compiler |
| Simulator | PICSIMLAB 4 |
| Clock | 20 MHz |

---

## System Block Diagram

```
                        ┌─────────────────────┐
                        │                     │
      Matrix Keypad ───►│                     │──► CLCD Display
      (SW1 - SW12)      │   PIC16F877A MCU    │    (Modes, Time, Temp)
                        │                     │
                        │                     │──► Fan
                        │                     │
                        │                     │──► Buzzer
                        └─────────────────────┘
```

---

## Cooking Modes

### 1.  Micro (Macro) Mode — `KEY1`
- Displays power level (`Power = XYZ W`)
- After 3 seconds, prompts user to set cooking time (MM:SS)
- Keys `0–9` to enter time, `*` to clear, `#` to confirm and start
- Displays remaining time with Start/Resume (4), Pause (5), Stop (6) controls

### 2.  Grill Mode — `KEY2`
- Directly prompts user to set cooking time (MM:SS)
- Keys `0–9` to enter time, `*` to clear, `#` to confirm and start
- Displays remaining time with Start/Resume (4), Pause (5), Stop (6) controls

### 3.  Convection Mode — `KEY3`
- Prompts user to set temperature (°C)
- After confirmation (`#`), starts **Pre-Heating** phase with countdown
- Once pre-heating completes, prompts for cooking time
- Full Start/Resume/Pause/Stop controls

### 4.  Quick Start — `KEY4`
- Starts cooking immediately with **default 30 seconds**
- Each additional `KEY4` press adds 30 seconds to the timer
- Full Start/Resume/Pause/Stop controls

---

##  LCD Screen Flow

```
Power ON
   │
   ▼
┌─────────────────┐
│ ████████████████│
│   Powering ON   │
│  Microwave Oven │
│ ████████████████│
└─────────────────┘
   │
   ▼
┌─────────────────┐
│ 1. Micro        │
│ 2. Grill        │
│ 3. Convection   │
│ 4. Start        │
└─────────────────┘
   │
   ├──KEY1──► Power Display ──► Set Time ──► Cooking (Timer countdown)
   ├──KEY2──► Set Time ──────────────────► Cooking (Timer countdown)
   ├──KEY3──► Set Temp ──► Pre-Heat ──► Set Time ──► Cooking
   └──KEY4──► Cooking (30s default, +30s per KEY4 press)
```

---

##  Keypad Mapping

| Key | Function |
|---|---|
| KEY1 | Select Micro mode |
| KEY2 | Select Grill mode |
| KEY3 | Select Convection mode |
| KEY4 | Quick Start / Start / Resume |
| KEY5 | Pause cooking |
| KEY6 | Stop cooking → back to mode screen |
| KEY0–9 | Enter time / temperature digits |
| KEY* | Clear entered value |
| KEY# | Confirm / Enter |

---

##  How to Run

1. Clone the repository:
```bash
   git clone https://github.com/sathees-waran/microwave-oven-embedded-c.git
```

2. Open **MPLAB X IDE** and import the source files (`*.c`, `*.h`)

3. Select **XC8 Compiler** and set the target device to **PIC16F877A**

4. Build the project to generate the `.hex` file

5. Open **PICSIMLAB 4**, load the board configuration and select the generated `.hex` file

6. Run the simulation — refer to the board schematic:

---

##  References & Documentation

| Document | Location |
|---|---|
| Board Schematic | [`docs/board4_Schematic_v0.8.8.pdf`](docs/board4_Schematic_v0.8.8.pdf) |
| PIC16F877A Datasheet | [`docs/pic16f877x_datasheet.pdf`](docs/pic16f877x_datasheet.pdf) |
| HD44780 CLCD Datasheet | [`docs/hd44780.pdf`](docs/hd44780.pdf) |
| Requirements & Design Doc | [`Microwave_Oven_RDD.pdf`](docs/) |

---

##  Technologies Used

![Embedded C](https://img.shields.io/badge/Embedded_C-00599C?style=for-the-badge&logo=c&logoColor=white)
![PIC](https://img.shields.io/badge/PIC16F877A-Microchip-red?style=for-the-badge)
![MPLAB](https://img.shields.io/badge/MPLAB_X_IDE-Microchip-red?style=for-the-badge)
![XC8](https://img.shields.io/badge/XC8_Compiler-Microchip-orange?style=for-the-badge)
![PICSIMLAB](https://img.shields.io/badge/PICSIMLAB_4-Simulator-blue?style=for-the-badge)

---

##  Author

**Satheeswaran**
- GitHub: [@sathees-waran](https://github.com/sathees-waran)

---
