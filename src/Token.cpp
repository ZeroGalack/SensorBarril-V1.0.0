#include "Arduino.h"
#include "MD5.h"
//#include "GPS.hpp"
#include "Barril.hpp"


String generateToken(String numSerie) {

    numSerie = numSerie.substring(0, 6);
    //Serial.println("numSerieC: " + numSerie);

    String hora_ = GPSdata.time;
    String data_ = GPSdata.date;


    String Time_data = data_ + hora_;
    Time_data = Time_data.substring(0, 9);
    //Serial.println("data_ + hora_: " + Time_data);

    float SomanumSerie = 0;
    for (int h = 0; h <= numSerie.length(); h++){
    SomanumSerie += numSerie.substring(h, h+1).toInt();
    }
    //Serial.println("SomanumSerie: " + String(SomanumSerie));

    float SomaTime_data = 0;
    for (int h = 0; h <= Time_data.length(); h++){
    SomaTime_data += Time_data.substring(h, h+1).toInt();
    }
    //Serial.println("SomaTime_data: " + String(SomaTime_data));

    float ratio = SomanumSerie / SomaTime_data;
    //Serial.println("pt3:" + String(ratio));
    char result[30];
    dtostrf(ratio, String(ratio).length(), 2, result);

    unsigned char* hash=MD5::make_hash(result);
    char *md5str = MD5::make_digest(hash, 16); 
    //Serial.println(md5str);

    return String(md5str);  
}