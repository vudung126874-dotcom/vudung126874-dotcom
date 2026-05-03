# 🌊 Remote Operated Vehicle (ROV) System

## 📌 Project Overview
This project features a complete control system for a mini-underwater ROV designed for deep-water exploration (up to 50m). The system consists of a surface **Remote Controller** and a submerged **Robot Unit**, communicating via a high-speed serial link (RS485 protocol) for reliable data transmission over long cables.

## 🛠 Technical Specifications
### 1. Submerged Unit (Slave)
- **MCU**: Arduino-based controller.
- **Actuators**: 3x High-torque Servos/ESCs managing Left, Right, and Vertical thrusters.
- **Sensor**: Analog Battery Voltage sensor with real-time feedback telemetry.
- **Fail-safe**: Integrated disconnection logic that resets thrusters to neutral (90°) if the signal is lost for more than 10,000 cycles.

### 2. Surface Remote (Master)
- **Display**: SSD1306 OLED (128x32) providing real-time telemetry (Mode, Speed, Battery Voltages).
- **Control Interface**: 
  - Dual 2-axis Joysticks for 3D maneuvering.
  - Dedicated switches for **ARM/DISARM** and **High/Low Speed** modes.
- **Communication**: RS485 Master-Slave architecture using half-duplex hardware flow control.

## 🚀 Key Engineering Features
- **Dynamic Speed Scaling**: Software-based speed limiting (50% power mode) for precise maneuvering in confined spaces.
- **Bidirectional Telemetry**: 
  - **Master to Slave**: Sends encoded thruster pulses (L, R, A) and arming commands.
  - **Slave to Master**: Returns submerged battery voltage ($V_d$) during disarmed states for pre-dive checks.
- **Custom Protocol**: A lightweight string-based protocol for motor speed decoding ($L_{val}R_{val}A_{val}$).
- **Power Monitoring**: Real-time voltage mapping and calculation for both surface and submerged units to prevent deep discharge.

## 📂 Repository Structure
- `ROV_Slave.ino`: Firmware for the submerged unit, handling thruster mixing and telemetry.
- `ROV_Remote.ino`: Firmware for the surface controller, managing OLED UI and joystick mapping.

## ✅ Operational Logic
- **Armed Mode**: Continuous high-speed transmission of joystick-mapped thruster values (40µs - 140µs range).
- **Disarmed Mode**: System locks thrusters at neutral and switches the RS485 bus to "Listen Mode" to receive battery telemetry from the ROV.
- **Mixer Logic**: Implements differential steering for horizontal thrusters (L/R) and dedicated vertical control (A).

## 🛠 Setup & Installation
1. Connect RS485 modules to the hardware Serial pins (TX/RX).
2. Use `MasterEnable` (Pin 3) and `Slave_EN` (Pin 5) for RS485 flow control.
3. Libraries required: `Servo`, `SPI`, `Wire`, `Adafruit_GFX`, `Adafruit_SSD1306`.
