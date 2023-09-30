#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>

class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
  void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
    Serial.print(advertisedDevice->getAddress().toString().c_str());
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
}

void loop() {
  Serial.println("=====");
  NimBLEScanResults scanResults = NimBLEDevice::getScan()->start(10, false);
  NimBLEDevice::getScan()->clearResults();
  Serial.println("-----");
  delay(10000); // Pause zwischen den Scans
}
