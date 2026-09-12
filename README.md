# IOT-C-M
IoT-based patient monitoring system — ATmega640 main board + two Arduino satellite boards tracking temperature/humidity, respiration rate, and SpO₂/HR, with EEPROM/SD logging, OLED display, buzzer/LED alerts, and Wi-Fi cloud upload.

# IoT COVID Monitor (IOT-C-M)

A layered-architecture embedded system that monitors vital signs and environmental conditions (body temperature & humidity, respiratory rate, heart rate / SpO₂) from a patient, aggregates the data on a central board, displays it locally, raises audible/visual alerts on abnormal readings, and pushes it to the cloud over Wi-Fi for remote monitoring.

The project was designed and built from scratch: custom PCB schematics, bare-metal AVR firmware (register-level drivers, no Arduino core) for the main controller, and Arduino-based firmware for the satellite sensor boards.

> ⚠️ **Disclaimer:** This is a student/prototype engineering project. The thresholds, calibration constants, and SpO₂ approximation used in the code are **not medical-grade** and must not be used for real diagnosis or patient care without proper calibration and certification.

---

## System overview

The device is split into a **main board** and two **remote sensor boards**, all talking over UART.

```
                    ┌───────────────────────────────────────────┐
                    │              MAIN BOARD (ATmega640)         │
   ┌─────────────┐  │  UART2 ◄── AM2305B board (temp/humidity)    │   ┌────────┐
   │ AM2305B      │──┼─►                                          │──►│ OLED   │
   │ Temp/Humidity│  │  UART3 ◄── RS board (respiration)          │   └────────┘
   │ + SD logging │  │                                             │   ┌────────┐
   └─────────────┘  │  SPI  ◄──► AFE4490 (SpO₂ / HR analog front  │──►│ Buzzer │
                     │             end, on-board)                  │   │ +LEDs  │
   ┌─────────────┐   │                                             │   └────────┘
   │ RS board     │──┼─► EEPROM (non-volatile log of readings)     │   ┌────────┐
   │ HX711+MMR920 │   │                                             │──►│ Wi-Fi  │──► Cloud
   │ + SD logging │  │  I2C  ◄──► OLED display                     │   │ Module │
   └─────────────┘   └───────────────────────────────────────────┘   └────────┘
```

<img width="1280" height="977" alt="signal plan" src="https://github.com/user-attachments/assets/11f9840b-ff97-4dad-96f0-ce288ca9b289" />
<img width="1280" height="960" alt="hardware plan" src="https://github.com/user-attachments/assets/5691c7a4-9aa9-455f-9233-7b676633d061" />


- **Main Board** – `ATmega640V` running custom bare-metal firmware. Reads the AFE4490 (SpO₂/HR) over SPI, receives temperature/humidity and respiration data over UART from the two satellite boards, evaluates old vs. new readings, stores records in EEPROM, drives the OLED status display and buzzer/LED alerts, and forwards data to an ESP-style Wi-Fi module for cloud upload.
- **AM2305B Interface Board (S3BFNTH)** – `ATmega328P` (Arduino) reading a DHT/AM2305B temperature & humidity sensor, logging every reading to an SD card, and streaming `<AM,...>` data packets to the main board over UART.
- **RS Interface Board (S3BRS)** – `ATmega328P` (Arduino) reading a load-cell style respiration sensor (MMR920) through an HX711 ADC, applying an IIR low-pass filter and peak-detection to derive breaths-per-minute, logging waveform samples to an SD card, and streaming `<RS,...>` packets to the main board over UART.

Each satellite board keeps the previous reading (from its SD card or RAM), so every transmitted packet includes **old value, new value, difference, and a computed trend/status**, letting the main board make simple LOW/NORMAL/HIGH and IMPROVED/WORSE/NO_CHANGE decisions without re-deriving history.

---

## Hardware architecture

| Board | MCU | Role | Key parts |
|---|---|---|---|
| Main Board | ATmega640V-8AU | Central hub, display, alerts, Wi-Fi/cloud gateway | AFE4490 (SpO₂/HR AFE), OLED (I2C), buzzer, R/G status LEDs, CH340C USB-UART, ESP Wi-Fi module, 7805 regulator |
| AM2305B Interface Board | ATmega328P | Temperature & humidity acquisition | AM2305B/DHT sensor, SD card (SPI), UART link to main board |
| RS Interface Board | ATmega328P | Respiration acquisition | HX711 24-bit ADC + MMR920/load sensor front end, SD card (SPI), UART link to main board |
| Power Supply | — | Bench 5 V supply | Transformer + bridge rectifier + 7805, status LED |

Full schematics (KiCad-style sheets) for all four boards are included in `hardware/`:
- `Main Board` — ATmega640 core, AFE4490 connector, OLED/buzzer/LED control, Wi-Fi interface, USB-UART, reset & clock circuits
- `AM2305B Interface Board` — ATmega328P + AM2305B sensor + SD card
- `RS Interface Board` — ATmega328P + HX711 + SD card
- `PSU-5V` — mains-to-5V linear power supply

Design/planning whiteboard photos (block diagrams for hardware, signal-processing pipeline, and software architecture) are included in `docs/planning/` for reference.

---

## SW-FLOWCHART

<img width="653" height="336" alt="CMMM2021-8591036 003" src="https://github.com/user-attachments/assets/9e722248-4cc1-4c21-9f74-e18e1ec83ae5" />


<img width="550" height="247" alt="9845162-fig-1-source-small" src="https://github.com/user-attachments/assets/0efd661c-b3e7-4e55-b143-44325162c30f" />


<img width="550" height="335" alt="9845162-fig-2-source-small" src="https://github.com/user-attachments/assets/0e4531e5-9665-4efa-8841-ec8e9401d8f7" />


<img width="661" height="391" alt="CMMM2021-8591036 002" src="https://github.com/user-attachments/assets/319ebf5b-7aff-4fd7-b7ac-f5b6f4428018" />


## Firmware architecture

### Main board (`/firmware/main-board`) — bare-metal AVR, no Arduino core

```
IOT_C_M_PROJ/
├── main.c                        # Application logic: packet parsing, thresholds, EEPROM logging, alerts
├── DataTypes.h                   # Fixed-width type aliases (DT_uint8_t, DT_int16_t, ...)
├── ATmega640_REGS.h              # Register map for the ATmega640
├── DRIVERS/                      # Low-level, register-level MCU peripheral drivers
│   ├── GPIO/                     # Pin direction / read / write
│   ├── UART/                     # Multi-UART driver (UART0 Wi-Fi, UART1/UART2 sensor boards)
│   ├── SPI/                      # SPI master driver (AFE4490)
│   ├── I2C/                      # I2C driver (OLED)
│   ├── ADC/                      # Analog-to-digital driver
│   ├── TIMER/  (+ PWM/)          # Timer & PWM driver
│   ├── EEPROM/                   # Internal EEPROM read/write for persistent logging
│   ├── EXT_INT/                  # External interrupt driver
│   └── WDT/                      # Watchdog timer driver
└── HAL/                          # Hardware abstraction layer built on top of DRIVERS
    ├── AFE4490-INTERFACE/        # SpO₂ / heart-rate analog front-end driver
    ├── OLED/                     # SSD1306-style OLED driver + font engine
    └── CONTROL/                  # Buzzer / status LED control logic
```

Responsibilities of `main.c`:
- Parses `<AM,...>` (temperature/humidity) and `<RS,...>` (respiration) UART packets from the satellite boards using a lightweight custom packet parser (no `sscanf`/dynamic memory).
- Reads SpO₂/HR from the AFE4490 over SPI.
- Classifies each vital as LOW / NORMAL / HIGH (or improving/worsening for respiration) against configurable thresholds.
- Persists the latest readings and a running record sequence number to EEPROM (with a magic-byte header for validity checking) so history survives a reset/power loss.
- Drives the OLED display and buzzer/red/green LED indicators via the `CONTROL` HAL.
- Relays consolidated data to the Wi-Fi module (UART0) for cloud upload.
- Includes stale-data timeouts so the display/alerts don't show frozen readings if a satellite board stops responding.

### Satellite boards (`/firmware/satellite-boards`) — Arduino (C++)

| File | Board | Summary |
|---|---|---|
| `S3BFNTH.cpp` | AM2305B Interface Board | Reads temperature & humidity from the AM2305B sensor, keeps the last reading on an SD card (`AM2305B.TXT`), computes old/new/diff and a LOW/NORMAL/HIGH status per value, and transmits a `<AM,...>` packet over UART every cycle. Logs SD/read/write errors as `<AM,ERROR,...>` packets. |
| `S3BRS.cpp` | RS Interface Board | Reads a load-cell signal through an HX711 ADC, applies an IIR low-pass filter, detects respiratory peaks (rise/fall + threshold + minimum inter-peak interval) to compute breaths-per-minute, keeps history on an SD card (`respiration.csv`), computes old/new/diff/trend, and transmits an `<RS,...>` packet over UART. |

**Packet formats**

```
<AM,OldDataValid,OldTemp,NewTemp,DiffTemp,TempStatus,OldHum,NewHum,DiffHum,HumStatus>
  e.g. <AM,1,36.50,37.00,0.50,NORMAL,55.00,58.00,3.00,NORMAL>

<RS,OldRR,NewRR,Diff,Trend>
  e.g. <RS,18.0,20.0,2.0,WORSE>
```

---

## Repository layout

```
.
├── firmware/
│   ├── main-board/            # ATmega640 bare-metal project (AVR-GCC / Atmel Studio)
│   └── satellite-boards/
│       ├── S3BFNTH.cpp        # AM2305B (temp/humidity) board sketch
│       └── S3BRS.cpp          # RS (respiration) board sketch
├── hardware/
│   └── Schematic_IOT_C_M_SMART-DEVICE.pdf   # Main board, AM2305B board, RS board, PSU schematics
├── docs/
│   └── planning/               # Whiteboard hardware / signal-flow / software architecture diagrams
└── README.md
```

<img width="1169" height="827" alt="0e11af9bed6df36d9a4801d40603093ccd6a987c" src="https://github.com/user-attachments/assets/5a1c39a3-1c18-4077-b369-e499dc415570" />
<img width="779" height="754" alt="Screenshot 2026-09-08 010623" src="https://github.com/user-attachments/assets/6bec6bcb-46d9-41ee-b9fe-34586295c09a" />


---

## Getting started

### Main board firmware
1. Open `firmware/main-board/IOT_C_M_PROJ.cproj` in **Atmel Studio / Microchip Studio** (or build with `avr-gcc` using the included `Makefile` under `Debug/`).
2. Set the target device to `ATmega640` and clock to 16 MHz (external crystal, per schematic).
3. Build and flash via the on-board USB-to-UART (CH340C) or an ISP programmer.

### Satellite boards
1. Open `S3BFNTH.cpp` or `S3BRS.cpp` in the **Arduino IDE**.
2. Select **Arduino Uno / ATmega328P** as the board.
3. Install the required libraries: `DHT sensor library` (for `S3BFNTH.cpp`) and `HX711` (for `S3BRS.cpp`), plus the built-in `SD` and `SPI` libraries.
4. Wire according to the corresponding schematic sheet and upload.


<img width="1536" height="1024" alt="ChatGPT Image Aug 29, 2026, 06_54_51 PM" src="https://github.com/user-attachments/assets/ed6d368d-464d-4865-b012-fd6a2fa0fe39" />



### Wiring summary
- Main board UART2 ↔ AM2305B board UART (temperature/humidity packets)
- Main board UART3 ↔ RS board UART (respiration packets)
- Main board SPI ↔ AFE4490 (SpO₂/HR)
- Main board I2C ↔ OLED display
- Main board GPIO ↔ buzzer, red LED, green LED
- Main board UART0 ↔ Wi-Fi module ↔ Cloud

---

## Status / roadmap

- [x] Main board bare-metal driver stack (GPIO, UART, SPI, I2C, ADC, Timer/PWM, EEPROM, external interrupt, watchdog)
- [x] AFE4490 SpO₂/HR HAL
- [x] OLED display HAL with custom font engine
- [x] EEPROM-backed data persistence with old/new/diff tracking
- [x] Buzzer/LED alerting logic
- [x] Satellite board firmware (temperature/humidity, respiration) with SD logging
- [x] Full PCB schematics for all four boards
- [ ] Cloud dashboard / mobile app integration
- [ ] Sensor calibration against a reference clinical device
- [ ] Enclosure and mechanical design

---

## Authors

Designed and built by **Dark_Griffein** (schematics: shehabeldinkhalid / SHEHAB_ELDIN).




https://github.com/user-attachments/assets/2f4ea03b-985b-4d97-9e3d-c5e1ce307569




<img width="780" height="711" alt="Screenshot 2026-08-31 171457" src="https://github.com/user-attachments/assets/2513e44f-d276-45d6-b910-0e956d79bf02" />

<img width="1013" height="709" alt="Screenshot 2026-08-31 152123" src="https://github.com/user-attachments/assets/4f461ec2-482f-4830-b86d-6e38128197b3" />


## Components
<img width="300" height="257" alt="ESP8266-Pinout-Datasheet-Features-Applications" src="https://github.com/user-attachments/assets/3d0710b7-2082-493a-8505-2d45a28ac2e0" />
<img width="640" height="640" alt="ebc49d2e-4101-44ae-a59b-a85bcd83dce8 jpg_640xaf" src="https://github.com/user-attachments/assets/a0f5ba7c-afac-4605-884e-38bb346c7037" />
<img width="1080" height="1080" alt="micro-sd-card-reader-module-1080x1080" src="https://github.com/user-attachments/assets/e90571c0-cabe-4c44-86fa-a3d636c119eb" />
<img width="1280" height="1807" alt="wBmJ_PfYHnVpn3hWxv_Vp95CRw0hmS5oX5IOHAfVogSH7FYaPFmR9_2qYXQdrmbMLL78nrRo5BT5iXaap_3y7MLNDcMkxpoPk1aMXQIEGPzkhNC15CxS3st8pKcBenO73hsQDi6bbr1-TmQ_RH_B9-SjPX3-XvVWXoML5ny4ZGpz3wQkDSPshbWzgMhxRKcC" src="https://github.com/user-attachments/assets/0ea48ef4-c47e-4f3d-9079-306086f0a97d" />


