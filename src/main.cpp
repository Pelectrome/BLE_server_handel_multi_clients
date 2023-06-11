#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>


#define SERVICE_UUID        "0000180F-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID "00002A19-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID2 "00002A19-0000-1000-8000-00805F9B34FF"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLECharacteristic* pCharacteristic2 = NULL;

void bleInit();


class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) {
        uint8_t rxValue = *pCharacteristic->getData();
        uint8_t length = pCharacteristic->getLength();
        std::string data = pCharacteristic->getValue();


        BLEAddress clientAddress = param->write.bda;
        Serial.print("Received Value: ");
        Serial.print(rxValue);
        Serial.print(",");
        Serial.print(length);
        Serial.print(",");
        Serial.print(data.c_str());
        Serial.print(", Address: ");
        Serial.print(clientAddress.toString().c_str());
        Serial.println();
      
    }
};

class ServerCallback: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
      Serial.print(" - ServerCallback - onConnect : ");
       /* Get the MAC address of the connected client */
      int ClientID = param->connect.conn_id;
      BLEAddress ClientAddress = param->connect.remote_bda;
      Serial.print("ADRESS: "); Serial.println(ClientAddress.toString().c_str());
       BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println(" - ServerCallback - onDisconnect");
    }
};

void setup() {
  Serial.begin(9600);
  bleInit();
}
uint32_t value = 0;

void loop() {
  // Handle any connected devices
        int connectedCount = pServer->getConnectedCount();
       Serial.printf("connectedCount: %d\n", connectedCount);
  
      if (connectedCount >0) { 
         pCharacteristic->setValue((uint8_t*)&value, 4);
          pCharacteristic->notify(); 
    
          
          pCharacteristic2->setValue((uint8_t*)&value, 20);
          pCharacteristic2->notify();   
      }
      value++;
      delay(1000);
}

void bleInit(){
  
  BLEDevice::init("BLE-Secure-Server");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallback());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY 
                    );
  BLEDescriptor Descriptor(BLEUUID((uint16_t)0x2901));                 
  Descriptor.setValue("Percentage 0 - 100");
  pCharacteristic->addDescriptor(&Descriptor);
 
  
  pCharacteristic2 = pService->createCharacteristic(
                      CHARACTERISTIC_UUID2,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY 
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();

}

