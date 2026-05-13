#pragma once
#include <LittleFS.h>
#include <Preferences.h>
#include "sensors_data.h"

const char* EVENT_LOG_FILE  = "/event_log.txt";
const int   MAX_EVENT_LINES = 100;

extern Preferences prefs;
extern bool isSystemStable;
extern unsigned long lastHeartbeatUpdate;

// Pack time data for NVS storage
static uint32_t packTime(rtc_struct* t) {
  uint32_t p = 0;
  p |= ((t->year_ - 2000) & 0x3F) << 26;
  p |= (t->month_ & 0x0F) << 22;
  p |= (t->day_ & 0x1F) << 17;
  p |= (t->hour_ & 0x1F) << 12;
  p |= (t->minute_ & 0x3F) << 6;
  p |= (t->second_ & 0x3F);
  return p;
}

static String unpackTime(uint32_t p) {
  int y = ((p >> 26) & 0x3F) + 2000;
  int m = (p >> 22) & 0x0F;
  int d = (p >> 17) & 0x1F;
  int h = (p >> 12) & 0x1F;
  int min = (p >> 6) & 0x3F;
  int s = p & 0x3F;
  char buf[25];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", y, m, d, h, min, s);
  return String(buf);
}

static void saveHeartbeatToNVS() {
  if (init_status.rtc_) {
    rtc_struct* t = get_rtc();
    uint32_t hb = packTime(t);
    prefs.begin("sys", false);
    prefs.putUInt("hb", hb);
    prefs.end();
  }
}

static int countEventLogLines() {
  File f = LittleFS.open(EVENT_LOG_FILE, "r");
  if (!f) return 0;
  int n = 0;
  while (f.available()) {
    f.readStringUntil('\n');
    n++;
  }
  f.close();
  return n;
}

static void dropOldestEventLine() {
  File src = LittleFS.open(EVENT_LOG_FILE, "r");
  if (!src) return;

  src.readStringUntil('\n'); // Skip first line (oldest event)

  const char* TMP = "/event_log.tmp";
  File tmp = LittleFS.open(TMP, "w");
  if (!tmp) { src.close(); return; }

  while (src.available()) {
    String line = src.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      tmp.println(line);
    }
  }
  src.close();
  tmp.close();

  LittleFS.remove(EVENT_LOG_FILE);
  LittleFS.rename(TMP, EVENT_LOG_FILE);
}

static String eventTimestamp() {
  if (!init_status.rtc_) return "0000-00-00 00:00:00";
  rtc_struct* t = get_rtc();
  char buf[25];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
           t->year_, t->month_, t->day_,
           t->hour_, t->minute_, t->second_);
  return String(buf);
}

void writeEventLog(const String& event, String customTimestamp = "") {
  if (!LittleFS.exists(EVENT_LOG_FILE)) {
    File f = LittleFS.open(EVENT_LOG_FILE, "w");
    if (f) f.close();
  }

  if (countEventLogLines() >= MAX_EVENT_LINES) {
    dropOldestEventLine();
  }

  File f = LittleFS.open(EVENT_LOG_FILE, "a");
  if (f) {
    String ts = (customTimestamp == "") ? eventTimestamp() : customTimestamp;
    f.println(ts + ": " + event);
    f.close();
  }
}

void initEventLog() {
  if (!LittleFS.exists(EVENT_LOG_FILE)) {
    File f = LittleFS.open(EVENT_LOG_FILE, "w");
    if (f) f.close();
    logDebug("[EVENT_LOG] Created new event_log.txt");
  } else {
    logDebug("[EVENT_LOG] Found existing event_log.txt");
  }
}

void sendEventLogToSerial() {
  Serial.println("\n=== EVENT LOG ===");
  if (!LittleFS.exists(EVENT_LOG_FILE)) {
    Serial.println("(empty)");
    Serial.println("=================\n");
    return;
  }
  File f = LittleFS.open(EVENT_LOG_FILE, "r");
  if (!f) {
    Serial.println("(cannot open)");
    Serial.println("=================\n");
    return;
  }
  while (f.available()) {
    Serial.println(f.readStringUntil('\n'));
  }
  f.close();
  Serial.println("=================\n");
}

void finalizeSystemStartup() {
  if (!isSystemStable) {
    isSystemStable = true;
    
    // Restore SHUTDOWN
    prefs.begin("sys", true);
    uint32_t lastHb = prefs.getUInt("hb", 0);
    prefs.end();

    if (lastHb > 0) {
      writeEventLog("SHUTDOWN", unpackTime(lastHb));
      prefs.begin("sys", false);
      prefs.remove("hb");
      prefs.end();
    }
    
    writeEventLog("BOOT");

    lastHeartbeatUpdate = millis(); 
    saveHeartbeatToNVS();
  }
}

void updateSystemHeartbeat() {
  unsigned long now = millis();
  
  if (isSystemStable && init_status.rtc_ && (now - lastHeartbeatUpdate >= 60000)) {
     lastHeartbeatUpdate = now;
     saveHeartbeatToNVS(); 
  }
}