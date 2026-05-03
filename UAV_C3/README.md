# 🛸 ESP32-C3 Flight Control System (UAV-C3)

## 📌 Project Overview
This project features a custom-built flight controller for a mini-drone using the **ESP32-C3** MCU. The firmware focuses on real-time attitude estimation and stable flight control using a self-implemented PID algorithm, designed specifically for high-performance mini-aerial vehicles.

## 🛠 Technical Specifications
- **Microcontroller**: ESP32-C3 (RISC-V architecture).
- **Sensors**: MPU6050 (6-axis IMU) for attitude tracking.
- **Communication**: I2C protocol for sensor data (SDA: GPIO 3, SCL: GPIO 4).
- **Control Interface**: Bluetooth connectivity via **Dabble** for real-time gamepad input and telemetry.
- **Actuation**: 4x 8520 brushed motors managed through high-frequency PWM signals (Pins: 1, 7, 5, 10).

## 🚀 Key Engineering Features
- **Custom PID Implementation**: Dual-rate PID control loop for Roll, Pitch, and Yaw stabilization.
  - **Roll/Pitch**: Kp=1.3, Ki=0.008, Kd=0.5.
  - **Yaw**: Kp=1.5, Ki=0.0005, Kd=0.4 (with 1.2° deadzone to eliminate gyro drift).
- **Safety Protocols**:
  - Secure **Arm/Disarm** mechanism via dedicated Gamepad button toggle.
  - Automatic signal cutoff to motors when the system is in a disarmed state.
- **Pre-flight Calibration**: Integrated sensor offset reset functionality (Circle button) to ensure accurate leveling before takeoff.
- **Dynamic Mixing**: Optimized X-configuration motor mixer converting control outputs into microsecond PWM pulses (1500µs - 2000µs).

## 📂 Repository Structure
- `testmotor2.ino`: Core firmware source code containing PID logic and motor mixing.
- `.gitignore`: Configured to exclude IDE artifacts and sensitive configuration files.

## ✅ Status & Performance
- **Flight Status**: Successfully completed. Achieved stable indoor hovering and responsive maneuvering.
- **Loop Frequency**: Optimized for low-latency feedback to maintain stability during rapid attitude changes.

## 🛠 Setup & Installation
1. Install the ESP32 board support in Arduino IDE or VS Code.
2. Required Libraries: `DabbleESP32`, `MPU6050_tockn`, `ESP32Servo`.
3. Open `testmotor2.ino`, compile, and flash to your ESP32-C3 Dev Module.