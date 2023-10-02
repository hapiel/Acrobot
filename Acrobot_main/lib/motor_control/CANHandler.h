#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include <ACAN_ESP32.h>

class CANHandler {
public:
    CANHandler();
    void setupCAN();
    void sendCANMessage(const CANMessage &message);
    void update();
    CANMessage latestFrame[4];
    CANMessage getLatestFrame(uint8_t id);

private:
    static const uint32_t DESIRED_BIT_RATE = 1000UL * 1000UL; // Example desired bit rate

};

#endif // CAN_HANDLER_H
