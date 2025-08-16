
# 🫀 ECG Holter Monitor with IoT Integration 

A compact, portable ECG Holter monitor designed to acquire ECG signals and transmit them wirelessly using WiFi for real-time monitoring and analysis. Built using ESP32-S3 microcontroller and integrated with cloud support for long-term data logging and remote access.  

    
- The [ads_1293](https://github.com/pasidunethmina/Smart-ECG-Holter-Monitor/tree/main/ads_1293) folder includes the codes for both Arduino framework and ESP-idf framework.   
- [firebase](https://github.com/pasidunethmina/Smart-ECG-Holter-Monitor/tree/main/firebase) folder only contains the files to upload data to firebase via wifi.

This repository only contains micro-controller programming part and files for the enclosure design. There is another repository for other parts of this project (PCB design, Mobile app design, etc.). [![Other Repo](https://img.shields.io/badge/GitHub-Other%20Repo-blue?logo=github)](https://github.com/SahanWeerasiri/Smart_Holter_Monitor.git)

---

## Machine Learning Model
- Developed a Convolutional Neural Network (CNN) model to detect abnormalities in ECG signals.
- Trained and tested using a Kaggle dataset.

---

## 📦 Features

- ✅ Continuous ECG monitoring
- 📡 WiFi-enabled data transmission
- 💾 SD card storage for offline data logging
- 🔋 Battery-powered for portability
- 📱 Cloud dashboard integration (optional)
- ⚙️ Built-in signal filtering and ADC sampling
- 🔒 Patient privacy and data security considerations

---

## 🧰 Hardware Components

| Component         | Description                                     |
|-------------------|-------------------------------------------------|
| ESP32-S3 MCU      | built-in USB, ultra low power & built-in wifi   |
| ECG Front-end     | ADS1293                                         |
| SD Card Module    | For backup data logging                         |
| Battery + Charger | Li-ion                                          |
| Enclosure         | 3D-printed                                      |

---
