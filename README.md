

# 🧠 Eye Controlled Emergency Communication System using EOG

## 📌 Project Overview

This project presents an **assistive communication system** designed for **paralyzed or speech-impaired patients** who can move only their eyes.
Using **Electrooculography (EOG)** signals captured from eye movements, the system enables users to **control a virtual cursor** on a web interface and **select predefined emergency messages** such as *“I need water”* or *“Emergency”*.

The system is **non-invasive**, **wireless**, and **browser-based**, making it suitable for hospital, home-care, and rehabilitation environments.

---

## 🎯 Objectives

* To design an **eye-controlled communication interface** for physically challenged users
* To eliminate the need for **keyboard, mouse, or touch input**
* To provide **quick emergency communication** using simple eye movements
* To implement **real-time calibration** for reliable EOG signal detection

---

## 🧠 Working Principle

1. Eye movement generates small electrical potentials (EOG signals)
2. The **BioAmp EXG Pill sensor** amplifies these signals
3. The **ESP32** reads the signal via its ADC pin
4. A **calibration algorithm** determines baseline and noise levels
5. Eye movements are classified into:

   * Cursor movement (navigation)
   * Blink (selection)
6. Cursor movement and selection commands are sent to a web interface via **WebSocket**
7. The selected emergency message is displayed prominently on the screen

---

## 🔧 Hardware Components Used

* ESP32 Development Board
* BioAmp EXG Pill Sensor
* Ag/AgCl EOG Electrodes
* Jumper Wires
* USB Cable
* Wi-Fi Network

---

## 💻 Software & Technologies Used

* Arduino IDE
* ESP32 Arduino Core
* HTML, CSS, JavaScript
* WebServer and WebSockets libraries

---

## 🧩 System Features

* ✔ Single-channel EOG based control
* ✔ Big white virtual ring cursor
* ✔ Sequential cursor navigation
* ✔ Blink-based selection
* ✔ On-screen calibration values
* ✔ Adjustable sensitivity
* ✔ Wireless web-based interface
* ✔ Platform independent (mobile / laptop / tablet)

---

## ⚙️ Calibration Process

1. User keeps eyes steady for **3 seconds** during startup
2. System calculates:

   * Baseline signal
   * Noise level
3. Adaptive thresholds are generated for:

   * Cursor movement
   * Blink detection
4. Calibration values are displayed live on the screen to help tune sensitivity

---

## 🖥️ User Interface

* Emergency options displayed as **large icons**
* Cursor highlights options using a **white ring**
* Selected option expands to **full-screen view**
* Calibration panel displays:

  * Baseline
  * Noise
  * Threshold values
  * Live signal strength

---

## 🏥 Applications

* Communication aid for paralyzed patients
* ICU and hospital patient monitoring
* Assistive technology for ALS / stroke patients
* Rehabilitation and physiotherapy environments
* Smart healthcare systems

---

## ✅ Advantages

* Non-invasive and safe
* Low-cost and portable
* No literacy or typing required
* Works with minimal eye movement
* Easy to deploy and maintain


Just tell me 👍
