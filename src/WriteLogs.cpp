#include "Arduino.h"
#include "WriteLogs.hpp"

#include "Adafruit_BMP280.h"
#include "GPS.hpp"


GPS GPSdata(18, 19);
Adafruit_BMP280 bmp0;


WriteLogs::WriteLogs(uint8_t _delay_) {
    _delay = _delay_;
}


void WriteLogs::begin() {
    GPSdata.begin(9600);
    Wire.begin();
    bmp0.begin(0x76, 0x60);
}


void WriteLogs::updateData() {
    while (true) {
        GPSdata.UpdateData();
        if (GPSdata.checkSignal()) {

            temperature = bmp0.readTemperature();
            String BatLvl = "100";

            latitude = String(GPSdata.latitude, 16);
            longitude = String(GPSdata.longitude, 16);
            hora_ = GPSdata.time;
            data_ = GPSdata.date;

            String DataLogs = latitude + "," + longitude + "," + String(temperature) + "," + hora_ + "," + data_ + "," + BatLvl;
            logArray[logIndex] = DataLogs;
            
            Serial.println(String(logIndex)+") " + DataLogs);

            digitalWrite(2, 1);
            delay(100);
            digitalWrite(2, 0);
            
            logIndex++;
        }
        else Serial.println("No GPS signal...");
        vTaskDelay(_delay * 1000);
    }
}


void WriteLogs::service() {

    while (true) updateData();

}