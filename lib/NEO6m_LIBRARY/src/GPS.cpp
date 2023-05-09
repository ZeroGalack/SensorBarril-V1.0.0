#include "Arduino.h"
#include "GPS.hpp"

HardwareSerial SerialGPS(1);

GPS::GPS(uint8_t rx, uint8_t tx) {
    RX_PIN 		= rx;
    TX_PIN 		= tx;
}

void GPS::begin(uint32_t baud) {
    SerialGPS.begin(baud, SERIAL_8N1, RX_PIN, TX_PIN);
}

void GPS::UpdateData() {
    String lista[20];
    int StringCount = 0;
    String Line = "";
    // while (true)
    //     if (SerialGPS.available())
    //         Serial.print((char) SerialGPS.read());

    while (SerialGPS.available()) SerialGPS.read();

    while (!(Line = SerialGPS.readStringUntil('\n')).startsWith("$GPRMC")){}

    //Serial.println(Line);
    while (Line.length() > 0){ 
    int index = Line.indexOf(',');

    if (index == -1){
        lista[StringCount++] = Line;
        break;
    }
    else{
        lista[StringCount++] = Line.substring(0, index);
        Line = Line.substring(index+1);
    }
    }

    String latlon = lista[3];
    float latlon_n1 = latlon.substring(0, 2).toFloat();
    float latlon_n2 = latlon.substring(2, latlon.length()).toFloat();
    latitude = latlon_n1 + (latlon_n2 / 60);

    String latitude_S = lista[4];
    if (latitude_S == "S"){
    latitude = (latlon_n1 + (latlon_n2 / 60)) * -1;
    }

    latlon = lista[5];
    latlon_n1 = latlon.substring(0, 3).toFloat();
    latlon_n2 = latlon.substring(3, latlon.length()).toFloat();
    longitude = latlon_n1 + (latlon_n2 / 60);

    String longitude_W = lista[6];
    if (longitude_W == "W"){
    longitude = (latlon_n1 + (latlon_n2 / 60)) * -1;
    }

    time = lista[1];
    date = lista[9];

}

bool GPS::checkSignal() {
    return (latitude != 0.0 && longitude != 0.0 && time != "" && date != "");
}
