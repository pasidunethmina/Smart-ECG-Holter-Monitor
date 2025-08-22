
# 🫀 ECG Holter Monitor with IoT Integration 

A compact, portable ECG Holter monitor designed to acquire ECG signals and transmit them wirelessly using WiFi for real-time monitoring and analysis. Built using ESP32-S3 microcontroller and integrated with cloud support for long-term data logging and remote access.  

    
- The [ads_1293](https://github.com/pasidunethmina/Smart-ECG-Holter-Monitor/tree/main/ads_1293) folder includes the codes for both Arduino framework and ESP-idf framework.   
- [firebase](https://github.com/pasidunethmina/Smart-ECG-Holter-Monitor/tree/main/firebase) folder only contains the files to upload data to firebase via wifi.

This repository only contains micro-controller programming part and files for the enclosure design. There is another repository for other parts of this project (PCB design, Mobile app design, etc.). [![Other Repo](https://img.shields.io/badge/GitHub-Other%20Repo-blue?logo=github)](https://github.com/SahanWeerasiri/Smart_Holter_Monitor.git)

---

## Machine Learning Model
- Developed a [Convolutional Neural Network (CNN) model](https://github.com/pasidunethmina/Smart-ECG-Holter-Monitor/tree/main/ML_model) to detect abnormalities in ECG signals.
- Trained and tested using a [Kaggle dataset](https://github.com/pasidunethmina/Smart-ECG-Holter-Monitor/blob/main/ML_model/data_sets/ecg_train1.csv). [Link for Kaggle dataset](https://www.kaggle.com/datasets/shayanfazeli/heartbeat)
- **Data preprocessing**:
  - Normalized signals using **StandardScaler** to improve training stability.
  - Reshaped signals for 1D CNN input `(samples, time steps, 1 channel)`.
- **Model training**:
  - Split dataset into training and testing sets.
  - Trained CNN to classify normal and abnormal ECG patterns.
- **Model deployment**:
  - Saved the trained model using `keras` and the fitted scaler with `joblib`.
  - Ready for integration with Python-based ECG GUI for real-time predictions.
- **Repository structure**:
  - `ML_model/` → contains model code, training scripts, dataset and [results](https://github.com/pasidunethmina/Smart-ECG-Holter-Monitor/tree/main/ML_model/results).
  - `scaler.pkl` → saved scaler for normalizing incoming ECG data.
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

# The Champions of SLIoT Challenge 2025 - Open Category 🏆

We emerged as the Champions of the SLIoT Challenge 2025 – Open Category with our Smart ECG Holter Monitor.

![Alt text](assets/img_sliot.jpg)
