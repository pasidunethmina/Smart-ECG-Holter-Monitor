/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

 #include <stdio.h>
 #include <inttypes.h>
 #include "sdkconfig.h"
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "esp_chip_info.h"
 #include "esp_flash.h"
 #include "esp_system.h"
 
 #include "driver/spi_master.h"
 #include "driver/gpio.h"
 
 // Pin definitions
 #define PIN_MISO 37
 #define PIN_MOSI 35
 #define PIN_SCLK 36
 #define PIN_CS   39
 // Control Pins
 #define PIN_DRDYB 14
 #define PIN_ALARMB 15
 
 
 #define SPI_HOST    SPI2_HOST
 #define DMA_CHAN    SPI_DMA_CH_AUTO
 
 spi_device_handle_t ads1293_handle;
 
 // Initialize SPI and GPIOs for ADS1293
 void ads1293_init(void) {
     spi_bus_config_t buscfg = {
         .miso_io_num = PIN_MISO,
         .mosi_io_num = PIN_MOSI,
         .sclk_io_num = PIN_SCLK,
         .quadwp_io_num = -1,
         .quadhd_io_num = -1,
         .max_transfer_sz = 32
     };
     spi_device_interface_config_t devcfg = {
         .clock_speed_hz = 1000000, // 1 MHz
         .mode = 0,
         .spics_io_num = PIN_CS,
         .queue_size = 1,
     };
     spi_bus_initialize(SPI_HOST, &buscfg, DMA_CHAN);
     spi_bus_add_device(SPI_HOST, &devcfg, &ads1293_handle);
 
     // Configure DRDYB and ALARMB as input
     gpio_config_t io_conf = {
         .pin_bit_mask = (1ULL << PIN_DRDYB) | (1ULL << PIN_ALARMB),
         .mode = GPIO_MODE_INPUT,
         .pull_up_en = GPIO_PULLUP_ENABLE,
     };
     gpio_config(&io_conf);
 }
 
 // Write to ADS1293 register
 void ads1293_write_reg(uint8_t reg, uint8_t value) {
     uint8_t tx[2] = { (reg & 0x7F), value };
     spi_transaction_t t = {
         .length = 16,
         .tx_buffer = tx,
     };
     spi_device_transmit(ads1293_handle, &t);
 }
 
 // Read from ADS1293 register
 uint8_t ads1293_read_reg(uint8_t reg) {
     uint8_t tx[2] = { (reg & 0x7F) | 0x80, 0x00 };
     uint8_t rx[2] = {0};
     spi_transaction_t t = {
         .length = 16,
         .tx_buffer = tx,
         .rx_buffer = rx,
     };
     spi_device_transmit(ads1293_handle, &t);
     return rx[1];
 }
 
 // Read ECG data (9 bytes for 3 channels)
 void ads1293_read_ecg(uint8_t *data) {
     uint8_t tx[10] = { 0x80 | 0x37, 0,0,0,0,0,0,0,0,0 }; // Read from register 0x37, 9 bytes
     spi_transaction_t t = {
         .length = 80, // 10 bytes * 8 bits
         .tx_buffer = tx,
         .rx_buffer = data,
     };
     spi_device_transmit(ads1293_handle, &t);
 }
 
 void app_main(void)
 {
     printf("Hello world!\n");
 
     /* Print chip information */
     esp_chip_info_t chip_info;
     uint32_t flash_size;
     esp_chip_info(&chip_info);
     printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
            (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
            (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");
 
     unsigned major_rev = chip_info.revision / 100;
     unsigned minor_rev = chip_info.revision % 100;
     printf("silicon revision v%d.%d, ", major_rev, minor_rev);
     if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
         printf("Get flash size failed");
         return;
     }
 
     printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
 
     printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
 
     // --- ADS1293 Example Code ---
     ads1293_init();
 
     uint8_t reg00 = ads1293_read_reg(0x00);
     printf("Reg 0x00 = 0x%02X\n", reg00);
 
     // Example configuration sequence for ADS1293
     // Reset the device
     ads1293_write_reg(0x00, 0x01); // Soft reset
     vTaskDelay(10 / portTICK_PERIOD_MS);
 
 
 
     // Configure ADS1293 for 5-lead ECG, 3 channels, 853 Hz, per datasheet example
     struct {
         uint8_t reg;
         uint8_t val;
     } ads1293_config[] = {
         {0x00, 0x02}, // 0. Put in stand by mode
         {0x01, 0x11}, // 1. Channel 1: INP to IN2, INN to IN1
         {0x02, 0x19}, // 2. Channel 2: INP to IN3, INN to IN1
         {0x03, 0x2E}, // 3. Channel 3: INP to IN5, INN to IN6
         {0x0A, 0x07}, // 4. Enable common-mode detector on IN1, IN2, IN3
         {0x0C, 0x04}, // 5. RLD amplifier output to IN4
         {0x0D, 0x01}, // 6. Wilson ref buffer 1 to IN1
         {0x0E, 0x02}, //    Wilson ref buffer 2 to IN2
         {0x0F, 0x03}, //    Wilson ref buffer 3 to IN3
         {0x10, 0x01}, // 7. Wilson ref output to IN6
         {0x12, 0x04}, // 8. Use external crystal, output internal osc to digital
         {0x14, 0x00},
         {0x21, 0x02}, // 9. R2 decimation rate = 5 (all channels)
         {0x22, 0x02}, // 10. R3 decimation rate = 6 (ch1)
         {0x23, 0x02}, // 11. R3 decimation rate = 6 (ch2)
         {0x24, 0x02}, // 12. R3 decimation rate = 6 (ch3)
         {0x27, 0x08}, // 13. DRDYB source = ECG channel 1
         {0x2F, 0x70}, // 14. Enable ECG ch1, ch2, ch3 for loop read-back
     };
 
     for (size_t i = 0; i < sizeof(ads1293_config)/sizeof(ads1293_config[0]); ++i) {
         int attempt;
         uint8_t read_val = 0;
         for (attempt = 0; attempt < 5; ++attempt) {
             ads1293_write_reg(ads1293_config[i].reg, ads1293_config[i].val);
             read_val = ads1293_read_reg(ads1293_config[i].reg);
             printf("Wrote 0x%02X to reg 0x%02X, read back 0x%02X %s (try %d)\n",
                 ads1293_config[i].val, ads1293_config[i].reg, read_val,
                 (read_val == ads1293_config[i].val) ? "OK" : "FAIL", attempt + 1);
             if (read_val == ads1293_config[i].val) {
                 break;
             }
             vTaskDelay(2 / portTICK_PERIOD_MS);
         }
         if (read_val != ads1293_config[i].val) {
             printf("Failed to write reg 0x%02X after 5 attempts\n", ads1293_config[i].reg);
         }
     }
 
     ads1293_write_reg(0x00, 0x01); // 15. Start data conversion
     uint8_t read_val = ads1293_read_reg(0x00);
     printf("Wrote 0x01 to reg 0x00, read back 0x%02X %s\n",
         read_val, (read_val == 0x01) ? "OK" : "FAIL");
 
     while (1) {
         // Wait for DRDYB to go low (data ready)
         while (gpio_get_level(PIN_DRDYB) != 0) {
             vTaskDelay(1);
         }
 
         uint8_t ecg_data[10] = {0};
         ads1293_read_ecg(ecg_data);
 
         // Print ECG data (first 9 bytes: 3 channels, 3 bytes each)
         printf("ECG: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
             ecg_data[1], ecg_data[2], ecg_data[3],
             ecg_data[4], ecg_data[5], ecg_data[6],
             ecg_data[7], ecg_data[8], ecg_data[9]);
 
         vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay to avoid flooding output
     }
 }
 