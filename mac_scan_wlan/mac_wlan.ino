#include <WiFi.h>
#include <Wire.h>
#include "esp_wifi.h"

String maclist[256];
int macListSize = 256;
int listcount = 0;
int count = 0;
int cMac80 = 0;

const wifi_promiscuous_filter_t filt={ //Idk what this does
    .filter_mask=WIFI_PROMIS_FILTER_MASK_MGMT|WIFI_PROMIS_FILTER_MASK_DATA
};

typedef struct { // or this
  uint8_t mac[6];
} __attribute__((packed)) MacAddr;

typedef struct { // still dont know much about this
  int16_t fctl;
  int16_t duration;
  MacAddr da;
  MacAddr sa;
  MacAddr bssid;
  int16_t seqctl;
  unsigned char payload[];
} __attribute__((packed)) WifiMgmtHdr;


#define maxCh 13 //max Channel -> US = 11, EU = 13, Japan = 14
#define maxRSSI 80

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

    //Serial.println("--------------------------- All " + String(listcount) + " Mac addresses in Channel " + String(curChannel) + ": ---------------------------");

    // Get the RSSI value and print the MAC address and RSSI value
    int8_t rssi = p->rx_ctrl.rssi;
    //Serial.print("MAC: ");
    Serial.print(mac);
    Serial.print("/");             //Seperates the MAC from the RSSI value
    //Serial.print(", RSSI: ");
    Serial.print(rssi);            //Used as a limiter
    Serial.println(";");
    //Serial.println(" dBm;");
    //Serial.println(mac + "/" + rssi + ";");

    if(listcount >= macListSize) {
      Serial.println("Too many addresses");
      listcount = 0;
    }
  }
}




//===== SETUP =====//
void setup() {

  /* start Serial */
  Serial.begin(115200);

  /* setup wifi */
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);

  //Serial.println("starting!");
}

void showMacAddresses(bool online){
  
  //Serial.println("--------------------------- All " + String(listcount) + " Mac addresses in Channel " + String(curChannel) + ": ---------------------------");
  
  for(int i = 0; i < listcount; i++) {
    // Get the MAC address and signal strength
    String mac = maclist[i];
    int8_t rssi = WiFi.RSSI(i);
  /*  
    // Print the MAC address and signal strength
    Serial.print("MAC: ");
    Serial.print(mac);
    Serial.print(", Signal Strength: ");
    Serial.print(rssi);
    Serial.println(" dBm");
  */
  count++;
  if(WiFi.RSSI(i) <= maxRSSI)
    cMac80++;
  }
  
}

//===== LOOP =====//
void loop() {
    //Serial.println("Neuer Channel-Scan");
    //Serial.println("------------------");
    for(int ci=0;ci<13;ci++){
          //Serial.println("Changed channel:" + String(curChannel));
    if(curChannel > maxCh){
      curChannel = 1;
    }
    esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
    delay(250);
    //Serial.println("-------");
    showMacAddresses(false);
    char maclist = '\0';
    listcount = 0;
    curChannel++;
    }
    //Serial.println(String(count) + " Adressen gefunden!");
    Serial.println("-----");
    //delay(5000);
    Serial.println("=====");
    count = 0;
    cMac80 = 0;
}
