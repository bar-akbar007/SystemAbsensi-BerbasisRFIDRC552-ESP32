// Absensi RFID - Google Apps Script (mengikuti contoh GitHub chethanavishmika)
// Menerima ?name=...&nis=...&kelas=... dari ESP32 dan menambah satu baris:
// Nama | NIS | Kelas | Tanggal | Jam
//
// Pasang di Google Sheets: Extensions > Apps Script (script terikat ke spreadsheet-mu).
// Deploy: Deploy > New deployment > Web app
//   Execute as: Me | Who has access: Anyone

// Spreadsheet tempat script ini terpasang (tidak perlu ID spreadsheet)
var ss = SpreadsheetApp.getActiveSpreadsheet();

// Nama sheet (tab) tempat absensi disimpan
var sheet = ss.getSheetByName('Sheet1');

// Zona waktu
var timezone = "Asia/Jakarta";

// Format tanggal dan jam
var dateFormat = "dd/MM/yyyy";   // contoh 15/11/2026
var timeFormat = "HH:mm:ss";     // 24 jam, contoh 17:15:30

function doGet(e) {
  var p = (e && e.parameter) ? e.parameter : {};
  var name = p.name ? p.name : "";
  var nis = p.nis ? p.nis : "";
  var kelas = p.kelas ? p.kelas : "";

  if (name === "") {
    return ContentService.createTextOutput("Error: nama tidak diterima");
  }

  var now = new Date();
  var date = Utilities.formatDate(now, timezone, dateFormat);
  var time = Utilities.formatDate(now, timezone, timeFormat);

  // Kolom B (NIS) disimpan sebagai teks supaya angka 0 di depan tidak hilang
  sheet.getRange('B:B').setNumberFormat('@');
  sheet.appendRow([name, nis, kelas, date, time]);

  return ContentService.createTextOutput("OK");
}
