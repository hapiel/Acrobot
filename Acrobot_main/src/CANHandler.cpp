#include "CANHandler.h"

CANHandler::CANHandler() {
    // Constructor initialization if needed
}

void CANHandler::setupCAN() {
    ACAN_ESP32_Settings settings(DESIRED_BIT_RATE);
    settings.mRequestedCANMode = ACAN_ESP32_Settings::NormalMode;
    const uint32_t errorCode = ACAN_ESP32::can.begin(settings);
    if (errorCode == 0) {
        // Successful setup code
    } else {
        // Error handling
    }
}

void CANHandler::sendCANMessage(const CANMessage &message) {
    if (ACAN_ESP32::can.tryToSend(message)) {
        // Successfully sent the message
    } else {
        // Failed to send the message
    }
}
