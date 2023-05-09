#include "Arduino.h"

class GPS {
    public:
        uint8_t	RX_PIN;
        uint8_t TX_PIN;

        GPS(uint8_t rx, uint8_t tx);
        float latitude = 0;
        float longitude = 0;
        String time = "";
        String date = "";

        void begin(uint32_t baud);
        void UpdateData();
        bool checkSignal();
};