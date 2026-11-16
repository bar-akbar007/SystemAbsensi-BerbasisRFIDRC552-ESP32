# 📚 Sistem Absensi RFID Berbasis ESP32 dan RC522 dengan Google Sheets

Sistem absensi otomatis berbasis **ESP32 dan RFID RC522** yang digunakan untuk mencatat kehadiran siswa menggunakan kartu RFID. Data absensi dikirim secara otomatis melalui jaringan WiFi dan disimpan ke dalam **Google Sheets** menggunakan Google Apps Script.

Proyek ini dirancang untuk mempermudah proses absensi di sekolah sehingga tidak perlu melakukan pencatatan kehadiran secara manual.

## ✨ Fitur Utama

- **Absensi menggunakan RFID:** Siswa cukup menempelkan kartu RFID ke modul RC522.
- **Identifikasi siswa otomatis:** Sistem mengenali siswa berdasarkan UID kartu RFID yang telah didaftarkan.
- **Integrasi Google Sheets:** Data absensi dikirim secara otomatis ke Google Sheets.
- **Informasi siswa:** Menyimpan nama, NIS, dan kelas siswa.
- **Buzzer sebagai indikator:**
  - 1 kali bunyi: Kartu berhasil dikenali.
  - 2 kali bunyi: Kartu tidak terdaftar.
  - 3 kali bunyi cepat: Gagal mengirim data ke Google Sheets.
- **Cooldown RFID:** Mencegah pembacaan kartu yang sama secara berulang dalam waktu 10 detik.
- **Koneksi WiFi otomatis:** ESP32 menghubungkan perangkat ke jaringan WiFi.
- **Pencatatan waktu otomatis:** Tanggal dan jam absensi ditambahkan oleh Google Apps Script.

## 🛠️ Komponen yang Dibutuhkan

| No. | Komponen | Jumlah |
|---|---|---|
| 1 | ESP32 Dev Board | 1 |
| 2 | RFID RC522 | 1 |
| 3 | Kartu RFID / Keychain RFID | Sesuai kebutuhan |
| 4 | Buzzer aktif | 1 |
| 5 | Kabel jumper | Secukupnya |
| 6 | Kabel USB | 1 |
| 7 | Komputer / Laptop | 1 |
| 8 | Jaringan WiFi | 1 |

## 🔌 Wiring / Konfigurasi Pin

### RFID RC522 ke ESP32

| Pin RC522 | Pin ESP32 |
|---|---|
| SDA (SS) | GPIO 21 |
| SCK | GPIO 18 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |
| RST | GPIO 22 |
| GND | GND |
| 3.3V | 3V3 |
| IRQ | Tidak digunakan |

### Buzzer ke ESP32

| Pin Buzzer | Pin ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| I/O | GPIO 15 |

**⚠️ Perhatian:** Modul RFID RC522 menggunakan tegangan 3.3V. Jangan menghubungkan pin VCC RC522 ke 5V karena dapat menyebabkan kerusakan modul.

## 📦 Library yang Dibutuhkan

Sebelum mengunggah program ke ESP32, instal library berikut melalui Arduino IDE Library Manager:

1. `MFRC522` – Untuk komunikasi dengan modul RFID RC522.

Library berikut sudah tersedia secara bawaan pada ESP32 Arduino Core:

- SPI
- WiFi
- WiFiClientSecure
- HTTPClient

## ⚙️ Persiapan dan Instalasi

### 1. Persiapan Arduino IDE

- Instal Arduino IDE.
- Tambahkan ESP32 Board Manager.
- Pilih board ESP32 yang digunakan.
- Hubungkan ESP32 ke komputer menggunakan kabel USB.
- Instal library MFRC522.

### 2. Konfigurasi WiFi

Buka kode program dan ubah bagian konfigurasi berikut sesuai jaringan WiFi yang digunakan:

```cpp
const char* WIFI_SSID = "NAMA_WIFI";
const char* WIFI_PASS = "PASSWORD_WIFI";
```

### 3. Konfigurasi Google Apps Script

Sistem ini menggunakan Google Apps Script sebagai penghubung antara ESP32 dan Google Sheets.

Buat Google Spreadsheet untuk menyimpan data absensi, kemudian buat project Google Apps Script yang berfungsi menerima data dari ESP32.

Deploy Apps Script sebagai Web App dengan konfigurasi:

- Execute as: Me
- Who has access: Anyone

Setelah berhasil melakukan deployment, salin URL Web App yang berakhiran `/exec`.

Masukkan URL tersebut ke dalam kode ESP32:

```cpp
const char* SCRIPT_URL = "URL_WEB_APP_ANDA";
```

**Catatan:** Pastikan Google Apps Script sudah dibuat dan di-deploy dengan benar agar ESP32 dapat mengirim data absensi.

### 4. Mendaftarkan Kartu RFID

Setiap kartu RFID memiliki UID yang berbeda. UID tersebut digunakan sebagai identitas untuk mengenali siswa.

Tambahkan data siswa pada bagian berikut:

```cpp
struct Student {
  const char* uid;
  const char* name;
  const char* nis;
  const char* kelas;
};

Student students[] = {
  {"5081FB5C", "Muhammad Akbar", "88200012", "XI TKJ A"},
  {"89B88429", "Marvelio Putra A", "35345345", "XI TKJ A"},
};
```

Format data:

```cpp
{"UID_KARTU", "NAMA_SISWA", "NIS", "KELAS"}
```

Untuk menambahkan siswa baru, cukup tambahkan data kartu ke dalam array `students`.

Pastikan UID kartu sesuai dengan UID yang terbaca oleh modul RC522.

### 5. Upload Program

Setelah semua konfigurasi selesai:

1. Hubungkan ESP32 ke komputer.
2. Buka program absensi di Arduino IDE.
3. Pilih board ESP32.
4. Pilih port COM yang sesuai.
5. Klik tombol Upload.
6. Buka Serial Monitor dengan baud rate `115200`.
7. Tunggu hingga ESP32 terhubung ke WiFi.

## 🚀 Cara Kerja Sistem

Berikut alur kerja sistem absensi:

1. ESP32 dinyalakan.
2. ESP32 menginisialisasi modul RFID RC522 dan buzzer.
3. ESP32 menghubungkan diri ke jaringan WiFi.
4. Sistem menunggu kartu RFID ditempelkan.
5. RC522 membaca UID kartu RFID.
6. ESP32 mencocokkan UID dengan daftar siswa.
7. Jika UID terdaftar, buzzer berbunyi satu kali.
8. ESP32 mengirim nama, NIS, dan kelas siswa ke Google Apps Script.
9. Google Apps Script menambahkan tanggal dan jam secara otomatis.
10. Data absensi disimpan ke Google Sheets.

Jika kartu tidak terdaftar, sistem memberikan dua kali bunyi buzzer dan data tidak dikirim.

Jika pengiriman data gagal, sistem memberikan tiga kali bunyi buzzer dengan cepat.

## 🔔 Indikator Buzzer

| Kondisi | Bunyi Buzzer | Keterangan |
|---|---|---|
| ESP32 mulai berjalan | 1 kali | Sistem aktif |
| Kartu terdaftar | 1 kali | Kartu dikenali |
| Kartu tidak terdaftar | 2 kali | UID tidak ditemukan |
| Gagal mengirim data | 3 kali cepat | WiFi atau server bermasalah |

## 📊 Format Data Google Sheets

Data yang dikirim oleh ESP32 terdiri dari:

| No. | Data | Keterangan |
|---|---|---|
| 1 | Nama | Nama siswa |
| 2 | NIS | Nomor Induk Siswa |
| 3 | Kelas | Kelas siswa |
| 4 | Tanggal | Tanggal absensi |
| 5 | Jam | Waktu absensi |

Tanggal dan jam dibuat oleh Google Apps Script, bukan oleh ESP32.

Contoh tampilan data:

| Nama | NIS | Kelas | Tanggal | Jam |
|---|---|---|---|---|
| Muhammad Akbar | 88200012 | XI TKJ A | 16/11/2026 | 07:00:00 |
| Marvelio Putra A | 35345345 | XI TKJ A | 16/11/2026 | 07:05:00 |

*Data di atas hanya contoh tampilan.*

## 🧩 Struktur Sistem

```text
              +----------------+
              |    Kartu RFID  |
              +-------+--------+
                      |
                      v
              +----------------+
              |   RFID RC522   |
              +-------+--------+
                      |
                      v
              +----------------+
              |      ESP32     |
              +-------+--------+
                      |
             +--------+--------+
             |                 |
             v                 v
       +-----------+     +-------------+
       |  Buzzer   |     |    WiFi     |
       +-----------+     +------+------+
                                |
                                v
                       +----------------+
                       | Google Apps    |
                       | Script         |
                       +-------+--------+
                               |
                               v
                       +----------------+
                       | Google Sheets  |
                       +----------------+
```

## 🔧 Troubleshooting

| Masalah | Kemungkinan Penyebab | Solusi |
|---|---|---|
| RC522 tidak terbaca | Kabel salah atau tegangan tidak sesuai | Periksa wiring dan gunakan 3.3V |
| Kartu tidak dikenali | UID belum terdaftar | Tambahkan UID ke array students |
| WiFi gagal tersambung | SSID atau password salah | Periksa konfigurasi WiFi |
| Data tidak masuk Google Sheets | URL Apps Script salah | Periksa URL Web App |
| Buzzer tidak berbunyi | Wiring atau polaritas salah | Periksa GPIO 15 dan sambungan buzzer |
| Absensi gagal dikirim | Server atau jaringan bermasalah | Periksa koneksi internet dan deployment Apps Script |

## 🔒 Catatan Keamanan

- Jangan membagikan password WiFi ke publik.
- Jangan mengunggah URL Web App pribadi jika tidak ingin endpoint tersebut digunakan orang lain.
- Untuk proyek yang dipublikasikan di GitHub, gunakan placeholder pada informasi sensitif.
- Pada implementasi produksi, pertimbangkan penggunaan autentikasi dan validasi data pada Google Apps Script.

## 👨‍💻 Teknologi yang Digunakan

- **ESP32:** Mikrokontroler utama.
- **RFID RC522:** Membaca UID kartu RFID.
- **Arduino IDE:** Pemrograman ESP32.
- **C++:** Bahasa pemrograman.
- **WiFi:** Komunikasi jaringan.
- **HTTP GET:** Pengiriman data ke server.
- **Google Apps Script:** Pengolahan data absensi.
- **Google Sheets:** Database penyimpanan absensi.

## 📄 Lisensi

Proyek ini dibuat untuk tujuan pembelajaran, pengembangan, dan implementasi sistem absensi digital berbasis Internet of Things (IoT).

Silakan dikembangkan dan dimodifikasi sesuai kebutuhan.

---

**Made with ❤️ using ESP32, RFID RC522, and Google Sheets.**