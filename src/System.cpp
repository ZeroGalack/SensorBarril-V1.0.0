#include "Arduino.h"


void SleepEsp(float Segs = 1)
{
  int sleepTime = Segs * 60000000;
  esp_sleep_enable_timer_wakeup(sleepTime);

  // Disable all power domains except RTC
  esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF); 
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON); 
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON); 
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_ON);

  // Go to deep sleep
  esp_deep_sleep_start();   
}