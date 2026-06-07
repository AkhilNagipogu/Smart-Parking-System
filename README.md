# Smart Parking System — ESP32 + Firebase

A hobby/college project I built to solve the classic "is there a free spot?" problem. The system uses ultrasonic sensors to detect whether each slot is occupied and pushes that data to Firebase in real time, so it can be read by a web dashboard or mobile app. There's also a servo-controlled barrier gate at the entrance that opens automatically when a car pulls up.

---

## How it works

Four HC-SR04 ultrasonic sensors are mounted above each parking slot. If the distance reading drops below 10 cm, the slot is marked OCCUPIED — otherwise it's FREE. That status gets pushed to Firebase only when it changes, so we're not hammering the database every 500ms for no reason.

At the gate, two IR sensors handle entry and exit. When a car triggers the entry IR, the servo lifts the barrier. It waits until the car clears the second IR sensor, then closes. Same logic in reverse for exit. A 16x2 LCD on the board shows how many slots are free at a glance, and shows "Parking Full!" if someone pulls up when everything's taken.

---

## Hardware used

- ESP32 Dev Board
- HC-SR04 Ultrasonic Sensor ×4
- IR Sensor Module ×2
- SG90 Servo Motor ×1
- 16×2 I2C LCD (address `0x27`) ×1

### Wiring

| Component | Pin |
|---|---|
| Slot 1 — TRIG / ECHO | GPIO 19 / 18 |
| Slot 2 — TRIG / ECHO | GPIO 5 / 17 |
| Slot 3 — TRIG / ECHO | GPIO 16 / 4 |
| Slot 4 — TRIG / ECHO | GPIO 2 / 15 |
| IR Entry (outside) | GPIO 35 |
| IR Exit (inside) | GPIO 32 |
| Servo | GPIO 13 |
| LCD | I2C (SDA/SCL) |

---

## Firebase structure

```
/location/
  Slot/
    slot1: "FREE" | "OCCUPIED"
    slot2: "FREE" | "OCCUPIED"
    slot3: "FREE" | "OCCUPIED"
    slot4: "FREE" | "OCCUPIED"
```

All slots reset to FREE every time the ESP32 boots up.

---

## Getting started

### 1. Clone the repo

```bash
git clone https://github.com/your-username/SmartParkingSystem.git
cd SmartParkingSystem
```

### 2. Set up credentials

```bash
cp config.example.h config.h
```

Open `config.h` and fill in your WiFi and Firebase details. This file is gitignored so your credentials won't accidentally get pushed.

### 3. Install libraries

In Arduino IDE, install these via the Library Manager:

- `Firebase ESP32 Client` by Mobizt
- `ESP32Servo`
- `LiquidCrystal I2C`

### 4. Flash

Select your ESP32 board and port, then upload. Open Serial Monitor at `115200` baud to see live logs.

---

## Project structure

```
SmartParkingSystem/
├── SmartParkingSystem.ino   # Main code
├── config.h                 # Your credentials (gitignored)
├── config.example.h         # Template — safe to commit
├── .gitignore
└── README.md
```

---

## Notes

- The distance threshold is set to 10 cm — tweak `DISTANCE_THRESHOLD_CM` depending on how your sensors are mounted
- The gate logic assumes IR sensors go LOW when a vehicle is detected (active low)
- LCD I2C address is `0x27` — run an I2C scanner sketch if your display isn't showing anything

---

