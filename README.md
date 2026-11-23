# NRF-BOX-jammer — nRF-BOX V2.5

Firmware ESP32 untuk **nRF-BOX V2.5**: alat uji keamanan nirkabel portabel dengan 3 modul nRF24L01, layar OLED 128×64, LED NeoPixel, dan 5 tombol.

> Dokumentasi lengkap (arsitektur, pemetaan pin, penjelasan tiap modul, peta EEPROM, troubleshooting) ada di **[DOKUMENTASI.md](DOKUMENTASI.md)**.

> ⚠️ **Peringatan:** fitur jammer/blackout memancarkan gangguan pada pita 2.4 GHz. Penggunaan terhadap jaringan atau perangkat orang lain **ilegal** di hampir semua negara, termasuk Indonesia. Gunakan hanya untuk pengujian perangkat sendiri atau di lingkungan berlisensi.

## Fitur

Menu utama berisi 11 item. Setiap mode berjalan terus-menerus sampai tombol **SELECT** ditekan lagi.

- **Scanner** — grafik spektrum 2.4 GHz 64 kanal (riwayat disimpan di EEPROM)
- **Analyzer** — analisis 128 kanal dengan grafik batang
- **WLAN Jammer** — pengganggu pita WiFi 2.4 GHz (3 radio, data rate & daya dapat diatur)
- **Proto Kill (Blackout)** — constant carrier untuk 8 protokol (WiFi, CCTV, RC, BLE, Bluetooth, USB Wireless, Zigbee, nRF24)
- **BLE Jammer** — pengganggu kanal advertising BLE / Bluetooth klasik
- **BLE Spoofer** — memalsukan iklan BLE 17 model perangkat Apple
- **Sour Apple** — flooding paket advertising Apple dengan MAC acak
- **BLE Scan** — daftar perangkat BLE + detail (nama, MAC, RSSI)
- **WiFi Scan** — daftar jaringan WiFi + detail (SSID, BSSID, RSSI, kanal)
- **About** — kredit pembuat
- **Setting** — aktif/nonaktif NeoPixel dan kecerahan OLED 10–100% (tersimpan di EEPROM)

## Hardware ringkas

| Komponen | Detail |
|----------|--------|
| MCU | ESP32-WROOM-32U (4 MB flash) |
| Radio | 3 × nRF24L01+ (CE/CSN: 5/17, 16/4, 15/2; SPI 18/19/23) |
| Display | OLED SSD1306 128×64 I2C (SCL 22, SDA 21) |
| LED | WS2812B di GPIO14 |
| Tombol | GPIO25, 26, 27, 32, 33 (`INPUT_PULLUP`) |
| USB-UART | CP2102 |
| Daya | Baterai Li-ion + TP4056, atau USB Type-C |

## Cara build

### Opsi A — Flash firmware siap pakai

1. Pasang driver CP210x dari folder `x64/` (jalankan `silabser.inf`) jika port COM belum dikenali.
2. Buka `flash_download_tool_3.9.7/flash_download_tool_3.9.7.exe`.
3. Pilih **ChipType: ESP32**, **WorkMode: Develop**, **LoadMode: UART**.
4. Tambahkan `3_nRFBox_v2.5.bin` pada alamat **`0x10000`**, pilih port COM, baud 115200, lalu **START**.
5. Setelah `Download result: PASS`, tekan reset.

> `3_nRFBox_v2.5.bin` adalah build lama tanpa perbaikan terbaru. Untuk versi dengan seluruh perbaikan, build dari source (Opsi B).

### Opsi B — Compile dari source (Arduino IDE)

1. Install **Arduino IDE 2.x** + board support **esp32** (Boards Manager).
2. Salin isi folder `Library/` ke `Documents/Arduino/libraries/` (atau install via Library Manager):
   - U8g2 ≥ 2.35.30
   - RF24 ≥ 1.4.11
   - Adafruit NeoPixel ≥ 1.12.4
3. Buka `nRFBox_V2/nRFBox_V2.ino`.
4. Board: **ESP32 Dev Module**, Flash Size: **4 MB**, Partition Scheme: **Huge APP (3MB No OTA/1MB SPIFFS)** — wajib, karena aplikasi ±1,48 MB.
5. Upload. Jika gagal pada API BLE, gunakan ESP32 core **2.0.x**.

## Lisensi & kredit

- Kode asli: [cifertech/nrfbox](https://github.com/cifertech/nrfbox) — MIT.
- Fork ini: `dhikzz-portfolio/NRF-BOX-jammer`.
- Kredit tampilan About: **Yellow Purple** (`Rubelymail@gmail.com`).
