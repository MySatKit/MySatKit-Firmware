//for OV2640 - satellite camera
#pragma once

#include "esp_camera.h"
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <vector>
#include <algorithm>
#include "base64.h"

extern void logDebug(String msg);

const int MAX_PHOTOS = 10;
const char* PHOTO_INDEX_FILE = "/photo_index.json";
const char* PHOTO_INDEX_TMP  = "/photo_index.tmp";

const size_t FS_BLOCK_SIZE    = 4096;
const size_t FS_RESERVE_BYTES = 4 * FS_BLOCK_SIZE;   

enum PhotoSaveResult { PHOTO_OK = 0, PHOTO_ERR_NO_SPACE, PHOTO_ERR_IO };

inline const char* photoSaveResultText(PhotoSaveResult r) {
  switch (r) {
    case PHOTO_OK:           return "saved";
    case PHOTO_ERR_NO_SPACE: return "not enough free flash space";
    default:                 return "flash write error";
  }
}

struct PhotoRecord {
  int id;
  String timestamp;
  String filename;
};

int globalPhotoCounter = 0;
std::vector<PhotoRecord> photoRecords;

bool init_camera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_XGA;
  config.jpeg_quality = 15;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("▲ Camera init failed with error 0x%x", err);
    Serial.flush();
    return false;
  }

  return true;
}

// helpers 
static size_t fsFreeBytes() {
  size_t total = LittleFS.totalBytes(), used = LittleFS.usedBytes();
  return used >= total ? 0 : total - used;
}

static size_t fileFootprint(size_t len) {
  return ((len + FS_BLOCK_SIZE - 1) / FS_BLOCK_SIZE + 1) * FS_BLOCK_SIZE;
}

static size_t sizeOfFile(const String& path) {
  File f = LittleFS.open(path.c_str(), "r");
  if (!f) return 0;
  size_t n = f.size();
  f.close();
  return n;
}

static bool writeFileVerified(const char* path, const uint8_t* data, size_t len) {
  File f = LittleFS.open(path, "w");
  if (!f) return false;
  size_t written = f.write(data, len);
  f.close();

  bool ok = (written == len);
  if (ok) {
    File check = LittleFS.open(path, "r");
    ok = check && check.size() == len;
    if (check) check.close();
  }
  if (!ok) LittleFS.remove(path);      
  return ok;
}

// Publish a finished temp file under its final name 
static bool commitFile(const char* tmpPath, const char* finalPath) {
  if (LittleFS.rename(tmpPath, finalPath)) return true;
  LittleFS.remove(finalPath);          
  if (LittleFS.rename(tmpPath, finalPath)) return true;
  LittleFS.remove(tmpPath);
  return false;
}

static bool parsePhotoId(const String& path, int& id) {
  const char* p = strrchr(path.c_str(), '/');
  p = p ? p + 1 : path.c_str();
  return path.endsWith(".jpg") && sscanf(p, "photo_%d.jpg", &id) == 1;
}

PhotoRecord* getPhotoById(int id);  

static void dropOldestPhoto() {        
  if (photoRecords.empty()) return;
  logDebug("[PHOTO] Evicting oldest: " + photoRecords[0].filename);
  LittleFS.remove(photoRecords[0].filename.c_str());
  photoRecords.erase(photoRecords.begin());
}

bool savePhotoIndex() {
  DynamicJsonDocument doc(3072);
  doc["counter"] = globalPhotoCounter;
  JsonArray photos = doc.createNestedArray("photos");
  for (const auto& record : photoRecords) {
    JsonObject photo = photos.createNestedObject();
    photo["id"] = record.id;
    photo["timestamp"] = record.timestamp;
    photo["filename"] = record.filename;
  }
  if (doc.overflowed()) {            
    LOG_ERROR("[PHOTO] Index JSON does not fit its buffer - increase the document size");
    return false;
  }

  String json;
  serializeJson(doc, json);

  // old index stays valid until the new one is completely written and verified
  if (!writeFileVerified(PHOTO_INDEX_TMP, (const uint8_t*)json.c_str(), json.length())) {
    LOG_ERROR("[PHOTO] Could not write photo index (flash full?)");
    return false;
  }
  if (!commitFile(PHOTO_INDEX_TMP, PHOTO_INDEX_FILE)) {
    LOG_ERROR("[PHOTO] Could not publish photo index");
    return false;
  }
  logDebug("[PHOTO] Index saved");
  return true;
}

static bool loadPhotoIndex() {
  if (!LittleFS.exists(PHOTO_INDEX_FILE)) return false;
  File indexFile = LittleFS.open(PHOTO_INDEX_FILE, "r");
  if (!indexFile) return false;

  DynamicJsonDocument doc(3072);        
  DeserializationError error = deserializeJson(doc, indexFile);
  indexFile.close();
  if (error) {
    LOG_ERROR("[PHOTO] Failed to parse index JSON: " + String(error.c_str()));
    return false;
  }

  globalPhotoCounter = doc["counter"] | 0;
  for (JsonObject photo : doc["photos"].as<JsonArray>()) {
    PhotoRecord record;
    record.id = photo["id"];
    record.timestamp = photo["timestamp"].as<String>();
    record.filename = photo["filename"].as<String>();
    photoRecords.push_back(record);
  }
  return true;
}

// Make the in-memory list match what is really on flash. Returns true if anything changed.
static bool reconcilePhotoFiles() {
  bool changed = false;
  std::vector<PhotoRecord> onFlash;
  std::vector<String> leftovers;

  // 1) scan first, modify afterwards 
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    String name = file.name();
    String path = name.startsWith("/") ? name : "/" + name;
    int id = 0;
    if (path.startsWith("/photo_") && path.endsWith(".tmp")) {
      leftovers.push_back(path);                    
    } else if (path.startsWith("/photo_") && parsePhotoId(path, id)) {
      PhotoRecord r;
      r.id = id;
      r.timestamp = "unknown";
      r.filename = path;
      onFlash.push_back(r);
    }
    file = root.openNextFile();
  }

  for (const auto& p : leftovers) {
    logDebug("[PHOTO] Cleanup: removing incomplete temp file " + p);
    LittleFS.remove(p.c_str());
    changed = true;
  }

  // 2) index entries whose file has vanished
  for (size_t i = 0; i < photoRecords.size();) {
    if (!LittleFS.exists(photoRecords[i].filename.c_str())) {
      logDebug("[PHOTO] Index lists missing file, dropping " + photoRecords[i].filename);
      photoRecords.erase(photoRecords.begin() + i);
      changed = true;
    } else {
      ++i;
    }
  }

  // 3) complete photos missing from the index (e.g. power cut before the index update): ADOPT them
  for (const auto& r : onFlash) {
    if (!getPhotoById(r.id)) {
      logDebug("[PHOTO] Adopting un-indexed photo " + r.filename);
      photoRecords.push_back(r);
      changed = true;
    }
  }

  // 4) IDs must never be reused
  for (const auto& r : photoRecords)
    if (r.id > globalPhotoCounter) { globalPhotoCounter = r.id; changed = true; }
  std::sort(photoRecords.begin(), photoRecords.end(),
            [](const PhotoRecord& a, const PhotoRecord& b) { return a.id < b.id; });
  while ((int)photoRecords.size() > MAX_PHOTOS) { dropOldestPhoto(); changed = true; }

  return changed;
}

void initPhotoStorage() {
  logDebug("[PHOTO] Total space: " + String(LittleFS.totalBytes()));
  logDebug("[PHOTO] Used space: " + String(LittleFS.usedBytes()));

  photoRecords.clear();
  globalPhotoCounter = 0;

  bool indexOk = loadPhotoIndex();
  bool changed = reconcilePhotoFiles();
  if (changed || (!indexOk && !photoRecords.empty())) savePhotoIndex();

  logDebug("[PHOTO] Ready: " + String(photoRecords.size()) + " photos, free "
           + String(fsFreeBytes()) + " bytes");
}

bool savePhoto(camera_fb_t* fb, const char* timestamp, PhotoSaveResult* result = nullptr) {
  PhotoSaveResult dummy;
  PhotoSaveResult& res = result ? *result : dummy;
  res = PHOTO_ERR_IO;
  if (!fb || !fb->buf || fb->len == 0) return false;

  const size_t need = fileFootprint(fb->len) + FS_RESERVE_BYTES;
  const size_t freeNow = fsFreeBytes();
  size_t reclaimed = 0, evictForSpace = 0;
  while (freeNow + reclaimed < need && evictForSpace < photoRecords.size()) {
    reclaimed += fileFootprint(sizeOfFile(photoRecords[evictForSpace].filename));
    evictForSpace++;
  }
  if (freeNow + reclaimed < need) {
    LOG_ERROR("[PHOTO] Not enough flash: need " + String(need) + " B, free " + String(freeNow)
              + " B, reclaimable " + String(reclaimed) + " B. Nothing was deleted.");
    res = PHOTO_ERR_NO_SPACE;
    return false;
  }

  for (size_t i = 0; i < evictForSpace; i++) dropOldestPhoto();

  const int nextId = globalPhotoCounter + 1;
  char tmpName[32], filename[32];
  snprintf(tmpName, sizeof(tmpName), "/photo_%d.tmp", nextId);
  snprintf(filename, sizeof(filename), "/photo_%d.jpg", nextId);
  logDebug("[PHOTO] Saving photo #" + String(nextId) + ", RAM free: " + String(ESP.getFreeHeap()));

  if (!writeFileVerified(tmpName, fb->buf, fb->len) || !commitFile(tmpName, filename)) {
    LOG_ERROR("[PHOTO] Write failed for photo #" + String(nextId));
    if (evictForSpace) savePhotoIndex();            
    res = PHOTO_ERR_IO;
    return false;
  }

  globalPhotoCounter = nextId;
  PhotoRecord record;
  record.id = nextId;
  record.timestamp = String(timestamp);
  record.filename = String(filename);
  photoRecords.push_back(record);
  while ((int)photoRecords.size() > MAX_PHOTOS) dropOldestPhoto();

  // if this fails the photo is still on flash and reconcilePhotoFiles() adopts it after a reboot
  if (!savePhotoIndex()) LOG_WARN("[PHOTO] Photo saved but index update failed");

  if (fsFreeBytes() < 2 * FS_RESERVE_BYTES)
    LOG_WARN("[PHOTO] Flash almost full: " + String(fsFreeBytes()) + " B free");

  LOG_INFO("[PHOTO] Saved photo #" + String(nextId) + " (" + String(fb->len) + " bytes)");
  res = PHOTO_OK;
  return true;
}

size_t getFreeStorageBytes() { return fsFreeBytes(); }

String getPhotoListJson() {
  DynamicJsonDocument doc(512);
  JsonArray ids = doc.createNestedArray("ids");

  for (const auto& record : photoRecords) {
    ids.add(record.id);
  }

  String result;
  serializeJson(doc, result);
  return result;
}

PhotoRecord* getPhotoById(int id) {
  for (auto& record : photoRecords) {
    if (record.id == id) {
      return &record;
    }
  }
  return nullptr;
}

void get_photo(){
  
}