#include <ArduinoBLE.h>
#include <RunningMedian.h>

/*
This program is distributed under GPLv3: https://www.gnu.org/licenses/gpl-3.0.
txt
You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

It is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
*/

BLEService raceChronoService("00001ff8-0000-1000-8000-00805f9b34fb");

BLECharacteristic canBusMainCharacteristic("1", BLERead | BLENotify, 20);
BLECharacteristic canBusFilterCharacteristic("2", BLEWrite, 0b0000100);
int analogReadPin = A1;
uint16_t pid = 255;
const uint16_t rawDataTable[24] = {
 72, 84, 97, 113, 132, 153, 178, 208, //20 .. 55 C
 242, 283, 331, 387, 455, 535, 630, 743,// 60 .. 95 C
 880, 1038, 1241, 1481, 1772, 2129, 2581, 3304 // 100 .. 135 C
};

RunningMedian medianFilter = RunningMedian(20);

void setup() {
  // Serial.begin(9600);
  // while (!Serial);

  // set up ADC
  pinMode(analogReadPin, INPUT);
  analogReadResolution(12);
  
  
  // set up Bluetooth
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (1);
  }

  BLE.setLocalName("RaceChronoService");
  BLE.setAdvertisedService(raceChronoService);

  raceChronoService.addCharacteristic(canBusMainCharacteristic);
  raceChronoService.addCharacteristic(canBusFilterCharacteristic);

  BLE.addService(raceChronoService);
  BLE.advertise();

  // Serial.println("BLE LED Peripheral"); 
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    // Serial.print("Connected to central: ");
    // Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH); // turn on the LED to indicate the connection

    while (central.connected()) {

      int sensorValue = readSensor(analogReadPin);
      uint8_t temperature = convertRawDataToTemp(sensorValue);
      medianFilter.add(temperature);
      uint8_t medianTemperature = medianFilter.getMedian();
      // Serial.print("Temparature: ");
      // Serial.println(medianTemperature);
      const uint8_t data[4] = {0b0, 0b0, (medianTemperature >> 8) & 0xFF, medianTemperature & 0xFF};
      sendData(pid, data, 4);
      
      delay(50);
    }

    // Serial.print(F("Disconnected from central: "));
    // Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW); // turn on the LED to indicate disconnect
  }
}

int readSensor(int pinId) {
  int sensorValue = analogRead(pinId);
  
   // Serial.print(F("Analog value read: "));
   // Serial.println(sensorValue);
  
  return sensorValue;
}

void sendData(uint32_t pid, const uint8_t *data, uint8_t len) {
 if (len > 8) {
    len = 8;
  }

  char buffer[20];
  buffer[0] = pid & 0xFF;
  buffer[1] = (pid >> 8) & 0xFF;
  buffer[2] = (pid >> 16) & 0xFF;
  buffer[3] = (pid >> 24) & 0xFF;
  memcpy(buffer + 4, data, len);

  canBusMainCharacteristic.writeValue(buffer, 4 + len, false);
}

uint8_t convertRawDataToTemp(int rawData) {
  if(rawData < rawDataTable[0]) return 0;
  if(rawData > rawDataTable[23]) return 255;
  
  uint8_t i = 0;
  while(rawData > rawDataTable[i]) i++;
  
  if(rawData == rawDataTable[i]) return i*5+20;
  
  return (i-1)*5 + 20 + (int) 5* (rawData - rawDataTable[i - 1]) / (rawDataTable[i] - rawDataTable[i - 1]);
}