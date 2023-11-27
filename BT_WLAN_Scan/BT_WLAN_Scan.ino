#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>
#include <WiFi.h>
#include <Wire.h>
#include "esp_wifi.h"

String maclist[256];
int macListSize = 256;
int listcount = 0;
int count = 0;
int cMac80 = 0;

const wifi_promiscuous_filter_t filt = {
  .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
};

typedef struct {
  uint8_t mac[6];
} __attribute__((packed)) MacAddr;

typedef struct {
  int16_t fctl;
  int16_t duration;
  MacAddr da;
  MacAddr sa;
  MacAddr bssid;
  int16_t seqctl;
  unsigned char payload[];
} __attribute__((packed)) WifiMgmtHdr;

#define maxCh 13
int curChannel = 1;

void sniffer(void* buf, wifi_promiscuous_pkt_type_t type) {
  wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t*)buf;
  int len = p->rx_ctrl.sig_len;
  WifiMgmtHdr *wh = (WifiMgmtHdr*)p->payload;
  len -= sizeof(WifiMgmtHdr);
  if (len < 0) {
    Serial.println("Received 0");
    return;
  }

  String packet;
  String mac;
  int fctl = ntohs(wh->fctl);

  for(int i = 8; i <= p->rx_ctrl.sig_len; i++) {
    packet += String(p->payload[i], HEX);
  }

  for(int i = 4; i <= 15; i++) {
    mac += packet[i];
  }
  mac.toUpperCase();

  bool added = false;
  for(int i = 0; i <= listcount; i++) {
    if(mac == maclist[i]) {
      added = true;
    }
  }

  if (!added) {
    maclist[listcount] = mac;
    listcount++;

    int8_t rssi = p->rx_ctrl.rssi;
    Serial.print(mac);
    Serial.print("/");
    Serial.print(rssi);
    Serial.println(";");
  }
}

class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
  void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
    std::string address = advertisedDevice->getAddress().toString();
    address.erase(std::remove(address.begin(), address.end(), ':'), address.end()); 
    String macAddress = String(address.c_str());
    macAddress.toUpperCase(); 
    Serial.print(macAddress);
    Serial.print("/");
    Serial.print(advertisedDevice->getRSSI());
    Serial.println(";");
  }
};


void setup() {
  Serial.begin(115200);
  NimBLEDevice::init("");
  NimBLEScan* pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(10000);
  pBLEScan->setWindow(99);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
}

void loop() {
  Serial.println("B=====");
  NimBLEScanResults scanResults = NimBLEDevice::getScan()->start(10, false);
  NimBLEDevice::getScan()->clearResults();
  Serial.println("-----");
  Serial.println("W=====");
  for(int ci = 0; ci < 13; ci++) {
    if(curChannel > maxCh){
      curChannel = 1;
    }
    esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
    delay(250);
    char maclist = '\0';
    listcount = 0;
    curChannel++;
  }
  Serial.println("-----");
  delay(10000); // Pause zwischen den Scans
}
