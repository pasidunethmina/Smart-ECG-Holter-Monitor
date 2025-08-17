import serial
import matplotlib.pyplot as plt
import numpy as np
import re

# Change this to your ESP32 serial port and baudrate
SERIAL_PORT = 'COM3'  # or '/dev/ttyUSB0'
BAUDRATE = 115200

def parse_ecg_line(line):
    # Example: ECG: 5D 9C 1E 5C E3 68 5C B2 B3
    m = re.match(
        r'ECG:\s*([0-9A-Fa-f]{2})\s+([0-9A-Fa-f]{2})\s+([0-9A-Fa-f]{2})\s+'
        r'([0-9A-Fa-f]{2})\s+([0-9A-Fa-f]{2})\s+([0-9A-Fa-f]{2})\s+'
        r'([0-9A-Fa-f]{2})\s+([0-9A-Fa-f]{2})\s+([0-9A-Fa-f]{2})', line)
    if m:
        # Convert hex bytes to integers
        bytes_data = [int(m.group(i), 16) for i in range(1, 10)]
        # 24 bits per channel
        ch1 = (bytes_data[0] << 16) | (bytes_data[1] << 8) | bytes_data[2]
        ch2 = (bytes_data[3] << 16) | (bytes_data[4] << 8) | bytes_data[5]
        ch3 = (bytes_data[6] << 16) | (bytes_data[7] << 8) | bytes_data[8]
        # Handle signed 24-bit values
        def to_signed24(val):
            return val - 0x1000000 if val & 0x800000 else val
        ch1 = to_signed24(ch1)
        ch2 = to_signed24(ch2)
        ch3 = to_signed24(ch3)
        return [ch1, ch2, ch3]
    return None

def main():
    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)
    print(f"Listening on {SERIAL_PORT}...")
    ecg_ch1 = []
    ecg_ch2 = []
    ecg_ch3 = []
    plt.ion()
    fig, axs = plt.subplots(3, 1, sharex=True, figsize=(10, 8))
    lines = []
    for i, label in enumerate(['CH1', 'CH2', 'CH3']):
        line, = axs[i].plot([], [], label=label)
        lines.append(line)
        axs[i].legend()
        axs[i].set_ylabel('Raw Value')
    axs[2].set_xlabel('Sample')
    fig.suptitle('ADS1293 ECG Data')

    while True:
        try:
            raw = ser.readline().decode(errors='ignore')
            data = parse_ecg_line(raw)
            if data:
                ecg_ch1.append(data[0])
                ecg_ch2.append(data[1])
                ecg_ch3.append(data[2])
                ecg_ch1 = ecg_ch1[-500:]
                ecg_ch2 = ecg_ch2[-500:]
                ecg_ch3 = ecg_ch3[-500:]
                lines[0].set_data(np.arange(len(ecg_ch1)), ecg_ch1)
                lines[1].set_data(np.arange(len(ecg_ch2)), ecg_ch2)
                lines[2].set_data(np.arange(len(ecg_ch3)), ecg_ch3)
                for i, ecg in enumerate([ecg_ch1, ecg_ch2, ecg_ch3]):
                    axs[i].relim()
                    axs[i].autoscale_view()
                plt.pause(0.01)
        except KeyboardInterrupt:
            break

    ser.close()

if __name__ == '__main__':
    main()