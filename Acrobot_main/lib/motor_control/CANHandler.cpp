#include "CANHandler.h"

CANHandler::CANHandler() {
    // Constructor initialization if needed
}

void CANHandler::setupCAN() {
    ACAN_ESP32_Settings settings(DESIRED_BIT_RATE);
    settings.mRequestedCANMode = ACAN_ESP32_Settings::NormalMode;
    settings.mTxPin = GPIO_NUM_14; // TODO: move to settable in constructor, define in main
    settings.mRxPin = GPIO_NUM_27;
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

void CANHandler::update() {

    CANMessage message;
    uint8_t count = 0;
    while (ACAN_ESP32::can.receive(message) && count < 4) {
        latestFrame[message.id - 1] = message;
        count++; // run max 4 times
    }
}

CANMessage CANHandler::getLatestFrame(uint8_t id) {
    return latestFrame[id - 1];
}
