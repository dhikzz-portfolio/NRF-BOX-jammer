# nRF-BOX V2.5 — Dokumentasi Proyek

Dokumentasi lengkap untuk perangkat **nRF-BOX V2.5**, sebuah alat uji keamanan nirkabel portabel berbasis ESP32 dengan tiga modul radio nRF24L01, layar OLED 128×64, dan LED NeoPixel.

> **Peringatan penting**
> Proyek ini adalah alat *radio frequency testing*. Fungsi **jammer / blackout** memancarkan sinyal pengganggu pada pita 2.4 GHz. Di hampir semua negara (termasuk Indonesia), **mengganggu komunikasi radio publik adalah tindakan ilegal** dan dapat dikenai sanksi pidana. Gunakan hanya pada perangkat milik sendiri, di lingkungan laboratorium berlisensi, atau dengan izin tertulis dari regulator (di Indonesia: Kominfo / SDPPI). Penulis dokumentasi dan pengembang tidak bertanggung jawab atas penyalahgunaan.

---

## 1. Ringkasan

| Item | Keterangan |
|------|------------|
| Nama perangkat | nRF-BOX (nRF-BOX V2.5) |
| Versi firmware | **v2.5.0** (ditampilkan pada layar boot) |
| Mikrokontroler | ESP32-WROOM-32U (dual-core, 2.4 GHz, BLE + WiFi) |
| Radio | 3 × modul nRF24L01+ (pita 2.4 GHz, 126 kanal RF) |
| Tampilan | OLED 0.96" SSD1306 128×64 (I2C) |
| Indikator | 1 × LED WS2812B (NeoPixel) di GPIO14 |
| Input | 5 tombol push-button (active-low, `INPUT_PULLUP`) |
| Catu daya | Baterai Li-ion + TP4056 charger, atau USB Type-C |
| Antena | 3 × konektor SMA (satu per modul nRF24) |
| Bahasa | C++ / Arduino (framework ESP32) |
| Lisensi kode | MIT (berdasarkan proyek cifertech/nrfbox) |

Perangkat bekerja sepenuhnya **offline dan standalone**: menu, eksekusi fungsi, dan penyimpanan pengaturan semuanya berjalan di perangkat tanpa perlu komputer pendamping.

---

## 2. Fitur

Menu utama berisi 11 item (ditampilkan sebagai daftar berikon):

| # | Menu | Fungsi | Butuh hardware RF24? |
|---|------|--------|----------------------|
| 0 | **Scanner** | Grafik spektrum 2.4 GHz 64 kanal (level aktivitas per kanal) | Ya (radio A) |
| 1 | **Analyzer** | Analisis 128 kanal, grafik batang deteksi *carrier* | Ya (radio A) |
| 2 | **WLAN Jammer** | Pengganggu pita WiFi 2.4 GHz dengan 3 radio | Ya (A+B+C) |
| 3 | **Proto Kill** / Blackout | *Constant carrier* yang menyasar 8 jenis protokol | Ya (A+B+C) |
| 4 | **BLE Jammer** | Pengganggu kanal advertising BLE / Bluetooth klasik | Ya (A+B+C) |
| 5 | **BLE Spoofer** | Memalsukan iklan BLE perangkat Apple (17 model) + log | Tidak (pakai BLE ESP32) |
| 6 | **Sour Apple** | *Flooding* paket advertising Apple dengan MAC acak | Tidak (pakai BLE ESP32) |
| 7 | **BLE Scan** | Pemindaian perangkat BLE aktif + detail perangkat | Tidak (pakai BLE ESP32) |
| 8 | **WiFi Scan** | Pemindaian jaringan WiFi sekitar + detail (SSID/BSSID/RSSI/kanal) | Tidak (pakai WiFi ESP32) |
| 9 | **About** | Layar kredit pembuat | Tidak |
| 10 | **Setting** | Aktif/nonaktif NeoPixel dan atur kecerahan OLED | Tidak |

---

## 3. Spesifikasi Hardware

Rincian diambil dari `Schematic.jpg` (nRFBOX V2 schematic, digambar 24 September 2024).

### 3.1 Komponen utama

| Ref | Komponen | Keterangan |
|-----|----------|------------|
| U1 | ESP32-WROOM-32U | MCU utama, dengan konektor antena U.FL |
| U2 | CP2102 | USB-to-UART bridge (driver Silicon Labs) |
| U5, U6, U7 | nRF24L01 | 3 × transceiver 2.4 GHz |
| U4 | Modul OLED 0.96" | SSD1306, 128×64, I2C |
| U3 | LF33 (regulator 3.3 V) | Catu daya rail 3.3 V |
| IC2 | TP4056 | Charger baterai Li-ion |
| Q1, Q2 | S8050 | Rangkaian auto-reset DTR/RTS untuk flashing |
| D1 | WS2812B | LED RGB alamat-tunggal |
| B3–B7 | Push button | 5 tombol navigasi |
| J1 | USB Type-C | Catu daya + data |
| SW1 | Sakelar | On/Off baterai |
| ANT1–ANT3 | SMA | Antena untuk tiap modul nRF24 |

### 3.2 Pemetaan pin (sesuai firmware)

**Bus SPI (dipakai bersama 3 modul nRF24):**

| Sinyal | GPIO |
|--------|------|
| SCK | 18 |
| MISO | 19 |
| MOSI | 23 |

**Modul nRF24:**

| Radio | CE | CSN |
|-------|----|-----|
| Radio A (U5) | 5 | 17 |
| Radio B (U6) | 16 | 4 |
| Radio C (U7) | 15 | 2 |

**OLED & LED:**

| Perangkat | Pin |
|-----------|-----|
| OLED SCL | 22 |
| OLED SDA | 21 |
| OLED alamat I2C | 0x3C (default U8g2) |
| NeoPixel DIN | 14 |

**Tombol (semua `INPUT_PULLUP`, aktif LOW):**

| Tombol fisik | GPIO | Nama di firmware |
|--------------|------|------------------|
| B3 | 25 | "kiri"/back, PA level (jammer), prev (spoofer) |
| B4 | 27 | "select" sekunder, data rate (jammer), next (spoofer) |
| B5 | 26 | "up"/next mode, start/stop jammer |
| B6 | 33 | "down", ganti adv type (spoofer) |
| B7 | 32 | SELECT menu utama |

> Catatan: beberapa modul memakai pemetaan tombol yang berbeda-beda. Lihat tabel navigasi pada bagian 8.

### 3.3 Catu daya

- USB Type-C 5 V → CP2102 + TP4056.
- TP4056 mengisi baterai Li-ion 1 sel; keluaran baterai melewati SW1 lalu LF33 untuk rail 3.3 V.
- Tidak ada pembacaan tegangan baterai di firmware (tidak ada indikator baterai di layar).

---

## 4. Struktur Repositori

```
NRF-BOX-jammer/
├── nRFBox_V2/                  # Source code firmware (Arduino sketch)
│   ├── nRFBox_V2.ino           # Entry point: setup(), loop(), menu utama
│   ├── icon.h                  # Semua bitmap/ikon OLED (PROGMEM)
│   ├── neopixel.h / .cpp       # Driver LED WS2812 + palet warna
│   ├── setting.h / .cpp        # Menu Setting (NeoPixel & brightness)
│   ├── scanner.h / .cpp        # Scanner spektrum 2.4 GHz
│   ├── analyzer.h / .cpp       # Analyzer 128 kanal
│   ├── jammer.h / .cpp         # WLAN Jammer (3 radio)
│   ├── blackout.h / .cpp       # Proto Kill / blackout multi-protokol
│   ├── blejammer.h / .cpp      # BLE / Bluetooth jammer
│   ├── spoofer.h / .cpp        # BLE Spoofer perangkat Apple
│   ├── sourapple.h / .cpp      # Sour Apple (advertising flood)
│   ├── blescan.h / .cpp        # Pemindai BLE
│   └── wifiscan.h / .cpp       # Pemindai WiFi
├── Library/                    # Library Arduino yang dibutuhkan (vendored)
│   ├── RF24/                   # v1.4.11
│   ├── U8g2/                   # v2.35.30
│   └── Adafruit_NeoPixel/      # v1.12.4
├── flash_download_tool_3.9.7/  # Alat flashing ESP32 (Espressif)
├── 3_nRFBox_v2.5.bin           # Firmware siap-flash (1.553.536 byte)
├── Schematic.jpg               # Skema rangkaian
├── CP210x_*.txt / silabser.*   # Driver USB-UART Silicon Labs (Windows)
├── x86/ x64/ arm/ arm64/       # Driver CP210x per arsitektur
├── SLAB_License_Agreement_VCP_Windows.txt
├── UpdateParam.bat / .reg      # Tweaker parameter port (Windows)
└── README.md                   # Hanya berisi judul proyek
```

Semua modul mengikuti pola seragam:

```cpp
void <modul>Setup();   // inisialisasi pin, bus, radio/peripheral
void <modul>Loop();    // satu iterasi logika + render OLED
```

---

## 5. Arsitektur Perangkat Lunak

### 5.1 Entry point

`nRFBox_V2.ino` mendeklarasikan objek global yang dipakai bersama seluruh modul lewat `extern`:

```cpp
RF24 RadioA(5, 17);      // objek awal; modul lain mendeklarasikan ulang
RF24 RadioB(16, 4);
RF24 RadioC(15, 2);

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
Adafruit_NeoPixel pixels(1, 14, NEO_GRB + NEO_KHZ800);
```

Saat boot, firmware:

1. Menyalakan NeoPixel (jika diaktifkan di EEPROM).
2. Mengonfigurasi tiga radio: auto-ack off, CRC off, `PA_MAX`, 2 Mbps.
3. Membaca kecerahan OLED dari EEPROM alamat `1`, lalu menampilkan splash **"nRF-BOX / Yellow Purple / v2.5.0"** selama ±3 detik.
4. Menampilkan logo bitmap `logo_cifer` selama 250 ms.
5. Mengaktifkan tiga tombol menu (`INPUT_PULLUP`).
6. Masuk ke `loop()` yang merender menu dan menunggu input.

### 5.2 Alur menu

```
                 +-----------------------+
                 |  Menu utama (11 item) |
                 +-----------+-----------+
                             |
        +--------------------+----------------------+
        |                    |                      |
   [Scanner]           [WLAN Jammer]          [Setting]
   scannerSetup()      jammerSetup()          settingSetup()
   scannerLoop()       jammerLoop()           settingLoop()
        |                    |                      |
        +--------------------+----------------------+
                             |
                      kembali ke menu
```

Menu digambar manual di buffer U8g2: ikon 16×16 di kiri, teks di tengah, kotak seleksi (`bitmap_item_sel_outline`) di item aktif, dan scrollbar di sisi kanan (`drawBox(125, 64/11 * item_selected, 3, 64/11)`).

Pola pemanggilan modul di `loop()` berbentuk:

```cpp
if (item_selected == N) {
  <modul>Setup();
  while (item_selected == N) {
    if (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
      if (callAbout) { <modul>Loop(); callAbout = false; }
      else break;
      /* tunggu tombol dilepas */
    }
  }
}
```

Detail perilaku ini dijelaskan pada bagian 9 (Catatan implementasi).

### 5.3 Lapisan perangkat keras bersama

- **SPI** dipakai bergantian oleh 3 radio dengan CSN berbeda.
- **I2C** (Wire) khusus OLED.
- **BLE** (Bluedroid `BLEDevice`) dipakai oleh Spoofer, Sour Apple, dan BLE Scan. Sebelum modul nRF24 aktif, firmware memanggil `esp_bt_controller_deinit()` agar tidak bentrok.
- **WiFi** dipakai oleh WiFi Scan; modul RF juga mematikan WiFi (`esp_wifi_stop()` / `esp_wifi_deinit()`) untuk mengurangi interferensi dan konsumsi daya.
- **EEPROM** (emulasi NVS 512 byte) menyimpan preferensi pengguna.

---

## 6. Penjelasan Teknis per Modul

### 6.1 Scanner — `scanner.cpp`

- Memindai **64 kanal** pada pita 2.4 GHz dengan mengakses register nRF24 langsung via SPI (tanpa library RF24): `CONFIG (0x00)`, `EN_AA (0x01)`, `RF_CH (0x05)`, `RF_SETUP (0x06)`, `RPD (0x09)`.
- Untuk tiap kanal, dilakukan **50 sampel**: radio di-`RX`, tunggu 100 µs, matikan, baca bit **RPD** (Received Power Detector). Nilai diakumulasi lalu dikonversi ke persen: `(total * 100) / samplesPerChannel`.
- Nilai maksimum seluruh kanal dinormalisasi ke tinggi grafik 0–64 px.
- Grafik bergerak dari kanan ke kiri dengan buffer `sensorArray[129]`; nilai terbaru masuk di indeks 0.
- Grafik **disimpan ke EEPROM** setiap 5 detik (alamat 2–129) sehingga riwayat tetap ada setelah reboot.
- NeoPixel berwarna ungu selama pemindaian, mati saat selesai.
- Konfigurasi SPI: mode 0, 16 MHz, MSB-first. Register `RF_SETUP = 0x0F` (gain maksimum, 2 Mbps).

### 6.2 Analyzer — `analyzer.cpp`

- Memindai **128 kanal** (`values[128]`), 50 siklus per frame.
- Tiap kanal: set `RF_CH`, `CE` HIGH 128 µs, `CE` LOW, lalu baca RPD. Setiap deteksi menambah counter kanal.
- Digambar sebagai garis vertikal per kanal (`drawVLine`), tinggi = `63 - value*3`, dengan label sumbu `"1...5...10...25..50...80...128"`.
- Berbeda dari Scanner, analyzer tidak menyimpan data dan tidak memakai library RF24.

### 6.3 WLAN Jammer — `jammer.cpp`

- Tiga radio diinisialisasi lewat library RF24: `openWritingPipe(0xFFFFFFFFFF)`, auto-ack off, retry 0, `PA_MAX`, 2 Mbps, CRC off.
- Metode yang **aktif di kode** adalah `methode = 1` (nilai hardcoded): mengirim pola byte `0xAA, 0x55, ...` (16 byte) pada rentang kanal `(channels*5)+1` sampai `(channels*5)+22`, sehingga satu "kanal WiFi" mencakup ±22 kanal RF.
- Metode 2 dan 3 ada di kode tetapi tidak dapat dipilih dari UI.
- Kontrol:
  - **BT1 (GPIO33)** — ganti kanal 1–14 (dengan debounce 200 ms).
  - **BT2 (GPIO26)** — mulai/berhenti jamming.
  - **BT3 (GPIO27)** — siklus data rate: 250 kbps → 1 Mbps → 2 Mbps.
  - **BT4 (GPIO25)** — siklus daya: MIN → LOW → HIGH → MAX.
- Semua tombol memakai interupsi `FALLING` + debounce di `millis()`.
- OLED menampilkan kanal, PA level, data rate, dan status jamming; NeoPixel merah saat aktif.

> Catatan: komentar di bagian atas file menyebut BT2 = data rate dan BT3 = jamming, tetapi implementasi sebenarnya adalah BT2 = jamming dan BT3 = data rate. Dokumentasi ini mengikuti kode.

### 6.4 Proto Kill / Blackout — `blackout.cpp`

- Menghasilkan **constant carrier** (`startConstCarrier(RF24_PA_MAX, channel)`) pada tiga radio sekaligus, dengan pembagian grup kanal:
  - Radio 1: `{2, 5, 8, 11}`
  - Radio 2: `{26, 29, 32, 35}`
  - Radio 3: `{80, 83, 86, 89}`
- Delapan mode protokol yang dapat dipilih:

| Mode | Kanal yang disasar |
|------|--------------------|
| WiFi | 1–12 |
| Video TX (CCTV) | 70, 75, 80 |
| RC | 1, 3, 5, 7 |
| BLE | 2, 26, 80 |
| Bluetooth klasik | 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 32, 34, 46, 48, 50, 52, 74, 76, 78, 80 |
| USB Wireless | 40, 50, 60 |
| Zigbee | 11, 15, 20, 25 |
| nRF24 | 76, 78, 79 |

- Tombol: **GPIO25** = mode sebelumnya, **GPIO27** = mode berikutnya, **GPIO26** = toggle ACTIVE/DEACTIVE (power down radio).
- OLED menampilkan ikon besar per mode (WiFi, CCTV, RC, BLE, Bluetooth, USB, Zigbee, nRF24) plus status `-ACTIVE-` / `-DEACTIVE-`.

### 6.5 BLE Jammer — `blejammer.cpp`

- Tiga radio dengan tiga grup kanal tetap (`{2,5,8,11}`, `{26,29,32,35}`, `{80,83,86,89}`) dikonfigurasi sebagai constant carrier.
- Mode: `DEACTIVE`, `BLE` (kanal 2/26/80), `Bluetooth` (21 kanal advertising klasik).
- Tombol **GPIO26** mengganti mode (debounce 500 ms).
- OLED menampilkan mode aktif serta status tiap radio (`isChipConnected()`).

### 6.6 BLE Spoofer — `spoofer.cpp`

- Meniru iklan BLE perangkat audio Apple: **17 payload** (AirPods, AirPods Pro/Max/Gen 2/Gen 3/Pro Gen 2, PowerBeats, PowerBeats Pro, Beats Solo Pro, Beats Studio Buds, Beats Flex, Beats X, Beats Solo 3, Beats Studio 3, Beats Studio Pro, Beats Fit Pro, Beats Studio Buds Plus).
- Payload berupa manufacturer-specific data (Company ID `0x4C00` = Apple) sepanjang 31 byte yang disimpan pada array `DEVICES[17][31]`.
- Setiap siklus advertising: alamat MAC diacak (`random(256)`, byte pertama di-OR `0xF0` agar bertipe random), service UUID `00003082-0000-1000-9000-00805f9b34fb`, interval min/max `0x20`, lalu `start()` → tunggu 1 detik → `stop()`.
- Tipe advertising diacak di antara `ADV_TYPE_IND`, `ADV_TYPE_SCAN_IND`, `ADV_TYPE_NONCONN_IND`.
- Kontrol: **GPIO27** = perangkat berikutnya, **GPIO25** = perangkat sebelumnya, **GPIO33** = tipe advertising, **GPIO26** = advertising ON/OFF.
- OLED menampilkan nama perangkat, adv type, dan status advertising.
- Daya pancar BLE diset maksimum (`ESP_PWR_LVL_P9`).

### 6.7 Sour Apple — `sourapple.cpp`

- Memancarkan paket *Apple proximity* 17 byte secara terus-menerus (flood) dengan interval 40 ms.
- Struktur paket: length, `0xFF` (manufacturer specific), `0x4C 0x00` (Apple), tipe `0x0F`, length `0x05`, action flags `0xC1`, action type acak dari 11 nilai (`0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0`), authentication tag acak 3 byte, dan padding.
- MAC diacak tiap iterasi; OLED menampilkan log 8 baris terakhir berisi tipe dan byte paket yang dikirim.

### 6.8 BLE Scan — `blescan.cpp`

- `BLEDevice::getScan()` dengan **active scan**, durasi 5 detik.
- Selama scan, layar menampilkan animasi "Scanning BLE . . ." dan NeoPixel berkedip putih.
- Setelah selesai: daftar hingga 5 perangkat (nama dipotong 7 karakter + RSSI), navigasi dengan tombol up/down, tombol select membuka detail (nama, alamat MAC, RSSI), tombol back (`GPIO25`) kembali ke daftar.

### 6.9 WiFi Scan — `wifiscan.cpp`

- `WiFi.mode(WIFI_STA)` + `WiFi.disconnect()`, lalu `WiFi.scanNetworks()`.
- Daftar hingga 5 jaringan (SSID 7 karakter + RSSI), detail berisi SSID, BSSID, RSSI, dan kanal.
- Timeout scan 5 detik; navigasi sama dengan BLE Scan.

### 6.10 Setting — `setting.cpp` + `neopixel.cpp`

- Dua opsi: aktif/nonaktif **NeoPixel** dan **kecerahan OLED** (kelipatan 10%, wrap 0–100%).
- Nilai disimpan ke EEPROM (alamat 0 dan 1) dan langsung diterapkan (`u8g2.setContrast`).
- `setNeoPixelColour()` menerima nama warna (`"red"`, `"green"`, `"blue"`, `"yellow"`, `"purple"`, `"cyan"`, `"white"`, `"null"`) dengan nilai RGB maksimum 5 (sengaja diredupkan agar tidak menyilaukan dan hemat daya). `flash()` tersedia untuk pola kedip, tetapi tidak dipakai di alur utama.

### 6.11 About

Layar statis berisi kredit: `Youtube/YellowPurple`, `Yellow Purple`, `Rubelymail@gmail.com`.

---

## 7. Peta EEPROM

Ukuran emulasi EEPROM: **512 byte** (`EEPROM.begin(512)`).

| Alamat | Ukuran | Isi |
|--------|--------|-----|
| `0x00` | 1 byte | Status NeoPixel (0 = nonaktif, 1 = aktif) |
| `0x01` | 1 byte | Kecerahan OLED (0–255) |
| `0x02`–`0x81` | 128 byte | Riwayat grafik Scanner (buffer `sensorArray`) |

Penulisan harus diakhiri `EEPROM.commit()`. Setting dan Scanner sama-sama menulis ke EEPROM; Scanner menulis setiap 5 detik saat aktif.

---

## 8. Navigasi Tombol

| Layar | GPIO26 | GPIO27 | GPIO33 | GPIO25 | GPIO32 |
|-------|--------|--------|--------|--------|--------|
| Menu utama | naik | — | turun | — | pilih / masuk / keluar |
| Scanner/Analyzer | — | — | — | — | keluar |
| WLAN Jammer | start/stop | data rate | kanal | PA level | — |
| Proto Kill | ACTIVE/DEACTIVE | mode + | — | mode − | — |
| BLE Jammer | ganti mode | — | — | — | — |
| BLE Spoofer | advertising ON/OFF | device + | adv type | device − | — |
| Sour Apple | — | — | — | — | keluar |
| BLE/WiFi Scan | naik | detail | turun | kembali | — |
| Setting | naik | pilih/ubah | turun | — | — |

Cara keluar dari modul: tekan **SELECT (GPIO32)** sekali lagi (lihat bagian 9 mengenai perilaku loop).

---

## 9. Catatan Implementasi & Keterbatasan

Hal-hal berikut ditemukan saat membaca kode dan penting diketahui sebelum memodifikasi firmware:

1. **`if (callAbout = true)` adalah assignment, bukan perbandingan.** Ekspresi ini selalu bernilai `true`, sehingga loop "tunggu tombol dilepas" langsung `break`.
2. **Modul hanya menjalankan satu iterasi `Loop()` per penekanan SELECT.** Alur `while (item_selected == N)` keluar setelah `callAbout` berubah menjadi `false`; `item_selected` sendiri tidak pernah diubah di dalam loop. Akibatnya mode seperti WLAN Jammer dan Spoofer hanya berjalan satu siklus lalu kembali ke menu. Untuk penggunaan berkelanjutan, pola ini perlu diubah (mis. `while (digitalRead(SELECT) == HIGH)` atau state machine tanpa `while`).
3. **`initializeRadios()` di `blejammer.cpp` memakai `currentMode == !DEACTIVE_MODE`.** `!0` bernilai `1` (= `BLE_MODULE`), sehingga inisialisasi multi-mode hanya berjalan pada mode BLE, bukan pada mode Bluetooth.
4. **Spoofer membungkus `deviceType` pada 26, padahal payload hanya 17.** Nilai 18–26 akan jatuh ke `default` dan menampilkan AirPods standar.
5. **Jammer memakai `methode = 1` hardcoded.** Metode 2 dan 3 tidak dapat diakses dari UI.
6. **Scanner memanggil `EEPROM.begin(128)`** sedangkan entry point memakai 512 byte; menulis di alamat >127 pada konteks lain bisa tidak konsisten.
7. **Tidak ada indikator baterai / pembacaan ADC** meskipun hardware memiliki TP4056 dan baterai.
8. **Interupsi tombol** dipasang di dalam `Loop()` (jammer, blackout, blejammer) sehingga `attachInterrupt` dipanggil berulang setiap iterasi. Tidak fatal, tetapi tidak efisien.
9. **Konsumsi flash besar** (1,48 MB) karena library BLE + WiFi + U8g2 full framebuffer; partition default 1,2 MB tidak cukup (lihat bagian 10).
10. **NeoPixel dibatasi pada nilai RGB 5** (dari 255) sehingga tampak redup; ini disengaja di kode.
11. **Tidak ada mekanisme verifikasi RF24 saat boot**; bila modul tidak terpasang, fungsi terkait tetap berjalan tanpa peringatan (kecuali BLE Jammer yang menampilkan `isChipConnected()`).

---

## 10. Build & Flash

### Opsi A — Memakai firmware siap pakai (`3_nRFBox_v2.5.bin`)

1. Pasang driver **CP210x** (folder `x64/` atau `x86/`, jalankan `silabser.inf`) bila Windows belum mengenali port COM.
2. Buka `flash_download_tool_3.9.7/flash_download_tool_3.9.7.exe`, pilih **ChipType: ESP32**, **WorkMode: Develop**, **LoadMode: UART**.
3. Tambahkan file `3_nRFBox_v2.5.bin` pada alamat **`0x10000`** (offset aplikasi standar ESP32; ini alamat yang tercatat pada log flashing bawaan repo).
4. Pilih port COM perangkat, baud 115200, lalu **START**.
5. Setelah `Download result: PASS`, tekan tombol reset.

> Firmware prebuilt hanya berisi aplikasi. Bootloader dan tabel partisi harus sudah ada di perangkat (kondisi normal untuk perangkat jadi). Jika perangkat benar-benar kosong, flash juga bootloader (`0x1000`), tabel partisi (`0x8000`), dan `boot_app0.bin` (`0xE000`).

### Opsi B — Compile dari source (Arduino IDE)

1. Install **Arduino IDE 2.x** dan **ESP32 board support** (Boards Manager → `esp32` by Espressif).
2. Salin isi `Library/` ke folder libraries Arduino (`Documents/Arduino/libraries/`), atau install via Library Manager:
   - **U8g2** ≥ 2.35.30
   - **RF24** ≥ 1.4.11
   - **Adafruit NeoPixel** ≥ 1.12.4
3. Buka `nRFBox_V2/nRFBox_V2.ino`.
4. Board: **ESP32 Dev Module** (atau DOIT ESP32 DEVKIT V1), Flash Size **4 MB**.
5. Partition Scheme: **Huge APP (3MB No OTA/1MB SPIFFS)** — wajib, karena ukuran aplikasi ±1,48 MB melebihi skema default.
6. Upload Speed 921600, pilih port, lalu **Upload**.
7. Jika kompilasi gagal pada API BLE (`BLEAdvertisementData`, `esp_ble_tx_power_set`), gunakan ESP32 core **2.0.x**. API Bluedroid tersebut berubah pada core 3.x sehingga perlu penyesuaian kode.

### Dependensi internal ESP32

`BLEDevice`, `BLEUtils`, `BLEServer`, `WiFi`, `SPI`, `Wire`, `EEPROM`, `esp_bt.h`, `esp_wifi.h` sudah termasuk dalam paket board ESP32, tidak perlu install terpisah.

---

## 11. Troubleshooting

| Gejala | Kemungkinan penyebab | Solusi |
|--------|----------------------|--------|
| Layar kosong | OLED tidak terdeteksi / kecerahan 0 | Cek I2C di GPIO21/22; masuk Setting dan naikkan brightness; coba `EEPROM` reset |
| Port COM tidak muncul | Driver CP210x belum terpasang | Install `silabser.inf` dari folder `x64/`/`x86/` |
| Upload gagal "not enough space" | Skema partisi default | Pilih **Huge APP (3MB No OTA)** |
| Upload gagal "MD5/checksum" | Kabel/baud terlalu tinggi | Turunkan upload speed ke 115200, ganti kabel data |
| Scanner/Analyzer datar | Modul nRF24 tidak terpasang / kabel SPI | Cek SCK18, MISO19, MOSI23, CE/CSN tiap radio |
| BLE Scan tidak menemukan apa pun | BLE controller gagal init setelah deinit WiFi/BT | Reboot perangkat sebelum masuk BLE Scan |
| Semua fungsi RF lemah | Daya dari USB tanpa baterai, antena tidak terpasang | Pasang antena SMA, gunakan baterai terisi |
| Setting tidak tersimpan | `EEPROM.commit()` gagal / memori aus | Flash ulang, hindari menulis EEPROM terus-menerus |

---

## 12. Lisensi & Kredit

- Kode sumber asli: **cifertech/nrfbox** — lisensi **MIT** (header "This software is licensed under the MIT License" ada di setiap file).
- Repositori ini: fork `dhikzz-portfolio/NRF-BOX-jammer`, dengan commit `feat: multijammer sub-frequency`.
- Kredit tampilan About: **Yellow Purple** (`Rubelymail@gmail.com`, YouTube: `YellowPurple`).
- Library pihak ketiga memakai lisensi masing-masing:
  - RF24 — GPL-2.0
  - U8g2 — BSD-2-Clause
  - Adafruit NeoPixel — LGPL-3.0 (sesuai repo Adafruit)
- Driver CP210x — Silicon Labs (lihat `SLAB_License_Agreement_VCP_Windows.txt`).

---

## 13. Ide Pengembangan Lanjutan

1. Ubah pola `while (item_selected == N)` menjadi state machine agar mode berjalan kontinu tanpa menekan tombol berulang.
2. Perbaiki perbandingan `currentMode == !DEACTIVE_MODE` dan sinkronkan jumlah payload spoofer (17 vs 26).
3. Tambahkan indikator tegangan baterai (ADC pada pembagi resistor) dan peringatan baterai lemah.
4. Tambahkan mode "sub-frequency sweep" yang dapat dipilih dari UI, bukan hardcoded.
5. Kurangi penulisan EEPROM Scanner (mis. hanya saat keluar mode) untuk memperpanjang umur flash.
6. Simpan setting ke Preferensi NVS (`Preferences.h`) alih-alih EEPROM emulasi manual.
