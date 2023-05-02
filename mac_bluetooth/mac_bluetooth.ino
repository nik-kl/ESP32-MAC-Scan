#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 10; //In seconds
BLEScan* pBLEScan;

int device_count = 0;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        //device_count++;
        //Serial.printf("Device %d: MAC Address: %s, RSSI: %d\n", device_count, advertisedDevice.getAddress().toString().c_str(), advertisedDevice.getRSSI());
        
        Serial.print(advertisedDevice.getAddress().toString().c_str());
        Serial.print("/");
        Serial.print(advertisedDevice.getRSSI());
        Serial.println(";");
    }
};

void setup() {
    Serial.begin(115200); //Enable UART on ESP32
    //Serial.println("Scanning..."); // Print Scanning

    BLEDevice::init("");ö
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); //Init Callback Function
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(10000); // set Scan interval
    pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
    device_count = 0; // Reset device count for each scan
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    
    pBLEScan->clearResults();   // delete results from BLEScan buffer to release memory
    //Serial.println("Scan Complete");
    //Serial.println();
    Serial.println("-----");
    //delay(1000); // Wait 1 second before starting the next scan
    Serial.println("=====");
    //Serial.flush(); // Clear the serial monitor
}