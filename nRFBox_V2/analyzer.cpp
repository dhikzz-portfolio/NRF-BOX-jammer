/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/nrfbox
   ________________________________________ */
   
#include <Arduino.h> 
#include "analyzer.h"
#include "setting.h"

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
extern Adafruit_NeoPixel pixels;

#define NRF24_CONFIG      0x00
#define NRF24_EN_AA       0x01
#define NRF24_RF_CH       0x05
#define NRF24_RF_SETUP    0x06
#define NRF24_RPD         0x09

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define N 128
uint8_t values[N];

#define CE  5
#define CSN 17


void writeRegister(uint8_t reg, uint8_t value) {
    digitalWrite(CSN, LOW);
    SPI.transfer(reg | 0x20);
    SPI.transfer(value);
    digitalWrite(CSN, HIGH);
}

uint8_t readRegister(uint8_t reg) {
    digitalWrite(CSN, LOW);
    SPI.transfer(reg & 0x1F);
    uint8_t result = SPI.transfer(0x00);
    digitalWrite(CSN, HIGH);
    return result;
}

void setChannel(uint8_t CHannel) {
    writeRegister(NRF24_RF_CH, CHannel);
}

void powerUP() {
    uint8_t config = readRegister(NRF24_CONFIG);
    writeRegister(NRF24_CONFIG, config | 0x02);
    delay(5);
}

void powerDOWN() {
    uint8_t config = readRegister(NRF24_CONFIG);
    writeRegister(NRF24_CONFIG, config & ~0x02);
}

void startListening() {
    digitalWrite(CE, HIGH);
}

void stopListening() {
    digitalWrite(CE, LOW);
}

bool carrierDetected() {
    return readRegister(NRF24_RPD) & 0x01;
}


void analyzerSetup(){
  
    Serial.begin(115200);
    
    esp_bt_controller_deinit();
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();
  
    pinMode(CE, OUTPUT);
    pinMode(CSN, OUTPUT);

    SPI.begin(18, 19, 23, 17);
    SPI.setDataMode(SPI_MODE0);
    SPI.setFrequency(10000000);
    SPI.setBitOrder(MSBFIRST);

    digitalWrite(CSN, HIGH);
    digitalWrite(CE, LOW);

    powerUP();
    writeRegister(NRF24_EN_AA, 0x00);
    writeRegister(NRF24_RF_SETUP, 0x0F);  

}

void analyzerLoop(){

    memset(values, 0, sizeof(values));

    setNeoPixelColour("purple");

    int n = 50;
    while (n--) {
        for (int i = 0; i < N; ++i) {
            // nRF24L01 hanya mendukung kanal 0..125.
            setChannel((uint8_t)((125L * i) / (N - 1)));
            startListening();
            delayMicroseconds(128);
            stopListening();
            if (carrierDetected()) {
                ++values[i];
            }
        }
    }

    setNeoPixelColour("0");

    u8g2.clearBuffer();

    for (int i = 0; i < N; ++i) {
        // values[i] maksimum 50; skala ke tinggi bar 0..62.
        int bar = map(values[i], 0, 50, 0, 62);
        if (bar < 0) bar = 0;
        if (bar > 62) bar = 62;

        // U8g2 drawVLine(x, y, height), digambar dari dasar layar ke atas.
        u8g2.drawVLine(i, 63 - bar, bar + 1);
    }

    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(0, 62);
    u8g2.print("1...5...10...25..50...80...128");
    u8g2.sendBuffer();
}
