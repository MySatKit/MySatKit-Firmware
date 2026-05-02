#pragma once
#include <LittleFS.h>
#include "sensors_data.h"

const char* EVENT_LOG_FILE  = "/event_log.txt";
const int   MAX_EVENT_LINES = 100;

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

  src.readStringUntil('\n');

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

void writeEventLog(const String& event) {
  if (!LittleFS.exists(EVENT_LOG_FILE)) {
    File f = LittleFS.open(EVENT_LOG_FILE, "w");
    if (f) f.close();
  }

  if (countEventLogLines() >= MAX_EVENT_LINES) {
    dropOldestEventLine();
  }

  File f = LittleFS.open(EVENT_LOG_FILE, "a");
  if (f) {
    f.println(eventTimestamp() + ": " + event);
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
  writeEventLog("BOOT");
  LOG_INFO("[EVENT_LOG] BOOT event written.");
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
