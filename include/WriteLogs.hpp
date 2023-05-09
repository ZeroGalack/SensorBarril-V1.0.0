#pragma once

#include "Arduino.h"
#include <EmService.hpp>


const int LOG_SIZE = 200;

class WriteLogs : public Embeddo::Service {
    private:
        double temperature;
        String latitude;
        String longitude;
        String hora_;
        String data_;
        uint8_t _delay;
           
    public:
    
        String logArray[LOG_SIZE];   
        int logIndex = 0;

        WriteLogs(uint8_t _delay_);
        using Embeddo::Service::Service;

        void begin() override;
        void updateData();

    protected:
        void service() override;
};

