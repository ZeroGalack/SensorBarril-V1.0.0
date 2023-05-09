#include <Arduino.h>
#include "SIM800L.hpp"
#include "WriteLogs.hpp"
#include <Embeddo.hpp>
#include "System.hpp"
#include "Token.hpp"


Sim800L GSM;
WriteLogs Logs(10);


const String apn = "claro.com.br ";                            // APN 
const String apn_u = "claro";                                  // APN-Username 
const String apn_p = "claro";                                  // APN-Password 
const String url = "http://barril.gensokyo.shop/dbpost.php";   // Server URL


void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando...");
  Logs.start();
  GSM.begin();
  GSM.reset();
  GSM.setFunctionalityMode(1);
  
}


void loop() {

  if (Logs.logIndex > 0) {
    // Aguarda registro na rede
    while (true) {
      String status = GSM.RegistrationStatus();
      Serial.println(status);
      if (status == "0,1") {
        break;
      }
    }

    // Configuração GPRS
    GSM.CONFIG_GPRS(apn ,apn_u, apn_p);
    delay(1000);
    GSM.Activate_GPRS();
    delay(1000);
    GSM.Start_HTTP(url);
    delay(1000);

    // Envia logs para o servidor
    for (int i = 0; i < Logs.logIndex;) {
      Serial.println(" -- Nº Logs["+ String(Logs.logIndex) +"] --- Log["+ String(i) +"] --- ");
  
      String token = generateToken(GSM.getNumSerie());

      GSM.Send_CONFIG_HTTP(192, 5000, "token=" + token + "&disp_id=1&event_data=" + Logs.logArray[i]);  
      if (GSM.Send_HTTP(1).indexOf("200")) i++;

    }

    Logs.logIndex = 0;

    // Encerra conexão GPRS
    GSM.Close_Connection();
    delay(1000);
    Serial.println("END ;)");

    SleepEsp(0.1);   
  }
}