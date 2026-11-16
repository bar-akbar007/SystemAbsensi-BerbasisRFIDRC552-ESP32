/*
  CODE 2 - Absensi RFID (ESP32 + RC522 + buzzer + Google Sheets)
  Dibuat mengikuti contoh GitHub chethanavishmika, disesuaikan rangkaianmu.

  - Kartu terdaftar      -> 1x bunyi, absensi masuk ke Google Sheets
  - Kartu tidak terdaftar -> 2x bunyi, tidak disimpan
  - Gagal kirim ke Sheets (WiFi / URL bermasalah) -> 3x bunyi cepat

  Tanggal dan jam ditambahkan oleh Google Apps Script (Code_contoh.gs),
  ESP32 mengirim nama, NIS, dan kelas siswa.

  Library: "MFRC522" (Arduino Library Manager). WiFi/HTTPClient sudah bawaan.

  Wiring (RC522 -> ESP32):
    SDA(SS) -> 21    SCK -> 18     MOSI -> 23
    MISO    -> 19    RST -> 22     GND  -> GND
    3.3V    -> 3V3 (JANGAN 5V!)    IRQ  -> tidak dipakai
  Buzzer: VCC -> 3V3, GND -> GND, I/O -> GPIO 15
*/

#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// ================== ISI INI ==================
const char* WIFI_SSID  = "Bagus A";
const char* WIFI_PASS  = "humaira0604";

// URL Web App dari Google Apps Script (berakhiran /exec)
const char* SCRIPT_URL = "https://script.google.com/macros/s/AKfycbynHFQHlIXOmoWntQwB5eKBIaihL94UTJpHBXzPskbZn8MMMuGE0YsdZoKABIXXNyHQ/exec";

// Kartu terdaftar: { "ID_KARTU", "Nama Siswa", "NIS", "Kelas" }
// ID kartu didapat dari CODE 1.
struct Student {
  const char* uid;
  const char* name;
  const char* nis;
  const char* kelas;
};

Student students[] = {
  {"5081FB5C", "muhammad akbar",    "88200012", "xi tkj a"},
  {"89B88429", "marvelio putra a",  "35345345", "xi tkj a"},
  // tambah kartu lain di sini...
};

const unsigned long COOLDOWN_MS = 10000;  // abaikan kartu yang sama selama 10 detik
// =============================================

#define SS_PIN     21
#define RST_PIN    22
#define SCK_PIN    18
#define MISO_PIN   19
#define MOSI_PIN   23
#define BUZZER_PIN 15

// Buzzer-mu aktif saat sinyal LOW (sudah dites)
#define BUZZER_ACTIVE_LOW true

const int STUDENT_COUNT = sizeof(students) / sizeof(students[0]);

MFRC522 rfid(SS_PIN, RST_PIN);

String lastUid = "";
unsigned long lastScanTime = 0;

// ---------- Buzzer ----------
void buzzerSet(bool on) {
  digitalWrite(BUZZER_PIN, (on ^ BUZZER_ACTIVE_LOW) ? HIGH : LOW);
}

void beep(int duration = 150) {
  buzzerSet(true);
  delay(duration);
  buzzerSet(false);
}

void beepValid()   { beep(200); }
void beepUnknown() { beep(250); delay(100); beep(250); }
void beepError()   { for (int i = 0; i < 3; i++) { beep(80); delay(80); } }

// ---------- WiFi ----------
bool connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return true;

  Serial.print("Menyambung WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi tersambung. IP: ");
    Serial.println(WiFi.localIP());
    return true;
  }
  Serial.println("WiFi gagal tersambung.");
  return false;
}

// ---------- Helper ----------
String urlEncode(const String& s) {
  String out = "";
  char buf[4];
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      out += c;
    } else {
      snprintf(buf, sizeof(buf), "%%%02X", (uint8_t)c);
      out += buf;
    }
  }
  return out;
}

int findStudent(const String& uid) {
  for (int i = 0; i < STUDENT_COUNT; i++) {
    if (uid.equalsIgnoreCase(students[i].uid)) return i;
  }
  return -1;
}

// ---------- Kirim ke Google Sheets ----------
bool sendAttendance(const Student& s) {
  if (!connectWiFi()) return false;

  WiFiClientSecure client;
  client.setInsecure();  // HTTPS sederhana tanpa cek sertifikat

  HTTPClient http;
  String url = String(SCRIPT_URL) + "?name=" + urlEncode(String(s.name)) +
               "&nis=" + urlEncode(String(s.nis)) +
               "&kelas=" + urlEncode(String(s.kelas));
  http.begin(client, url);
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);  // Google mengalihkan balasan
  http.setTimeout(15000);

  int code = http.GET();
  Serial.print("HTTP code: ");
  Serial.println(code);
  if (code > 0) {
    Serial.print("Balasan server: ");
    Serial.println(http.getString());
  }
  http.end();

  return code == 200;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(BUZZER_PIN, OUTPUT);
  buzzerSet(false);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();
  delay(50);

  Serial.println();
  Serial.println("=== Absensi RFID ===");

  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print("Versi chip RC522: 0x");
  Serial.println(version, HEX);
  if (version == 0x00 || version == 0xFF) {
    Serial.println("PERINGATAN: RC522 tidak terbaca! Cek kabel, solder, dan daya 3.3V.");
  } else {
    Serial.println("RC522 terdeteksi OK.");
  }

  connectWiFi();
  beep(100);
  Serial.println("Siap. Tempel kartu...");
}

// ---------- Loop ----------
void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Abaikan kartu yang sama kalau baru saja dibaca
  if (uid == lastUid && millis() - lastScanTime < COOLDOWN_MS) {
    return;
  }
  lastUid = uid;
  lastScanTime = millis();

  Serial.println("--------------------------");
  Serial.print("ID Kartu: ");
  Serial.println(uid);

  int idx = findStudent(uid);
  if (idx < 0) {
    Serial.println("Kartu tidak dikenal - belum terdaftar.");
    beepUnknown();
    return;
  }

  String name = students[idx].name;
  Serial.print("Siswa: ");
  Serial.println(name);
  beepValid();  // langsung bunyi, sebelum kirim ke Sheets

  if (sendAttendance(students[idx])) {
    Serial.println("Absensi tersimpan di Google Sheets.");
  } else {
    Serial.println("Gagal menyimpan absensi!");
    beepError();
  }
}
