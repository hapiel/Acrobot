// GENERATED CODE. DO NOT CHANGE
// Trigger animations automatically using the configuration in the app, or callbacks in the BottangoArduinoCallbacks.cpp file
// Look at the method "onLateLoop" in the callbacks file for examples

// Animation Index Key:
// 0: staan
// 1: Europapark


// !!! Do not change the below, unless you know what you are doing.
// !!!
// !!!


#include "GeneratedCodeAnimations.h"
#include "src/CodeCommandStreamDataSource.h"

namespace GeneratedCodeAnimations
{
    // setup
    const char SETUP_DATA_0[] PROGMEM = "rSVPin,25,2400,549,3000,981\nrSVPin,26,550,2400,3000,1968\nrSVPin,12,2400,550,3000,1475\nrSVPin,27,550,2400,3000,1475\n";
    const char *const SETUP_DATAARRAY[] PROGMEM = {SETUP_DATA_0};

    // animation 0, "staan"
    const char ANIM_0_DATA0[] PROGMEM = "\n";
    const char *const ANIM_0_DATAARRAY[] PROGMEM = {ANIM_0_DATA0};
    const char ANIM_0_LOOP[] PROGMEM = "";
    const uint16_t ANIM_0_CONFIG[] PROGMEM = {0, 0, 0, 0, 0, 0, 0, 0};

    // animation 1, "Europapark"
    const char ANIM_1_DATA0[] PROGMEM = "sC,25,0,1033,6281,258,0,6281,-258,0\nsC,27,0,1033,4096,258,0,4096,-258,0\nsC,12,0,1033,4096,258,0,4096,-258,0\nsC,26,0,1033,6281,258,0,6281,-258,0\nsC,26,1033,1900,6281,475,0,3854,-475,0\nsC,12,1033,1900,4096,475,0,1638,-475,0\nsC,25,1033,1900,6281,475,0,3854,-474,0\nsC,27,1033,1900,4096,475,0,1638,-475,0\nsC,27,2933,2134,1638,533,0,1638,-533,0\nsC,12,2933,2134,1638,533,0,1638,-533,0\nsC,25,2933,2134,3854,533,0,3854,-533,0\nsC,26,2933,2134,3854,533,0,3854,-533,0\nsC,27,5067,1733,1638,433,0,4096,-433,0\nsC,26,5067,2133,3854,533,0,3854,-533,0\nsC,12,5067,1733,1638,433,0,4096,-433,0\nsC,25,5067,2133,3854,533,0,3854,-532,0\nsC,26,7200,1400,3854,350,0,6179,-350,0\nsC,25,7200,1400,3854,349,0,6179,-349,0\n\n";
    const char *const ANIM_1_DATAARRAY[] PROGMEM = {ANIM_1_DATA0};
    const char ANIM_1_LOOP[] PROGMEM = "sC,26,30000,16,6179,8,0,6281,-8,0\nsC,25,30000,16,6179,8,0,6281,-8,0\n";
    const uint16_t ANIM_1_CONFIG[] PROGMEM = {0, 0, 0, 22, 0, 0, 0, 0};


    // configs
    const uint16_t *const CONFIGS_ARRAY[] PROGMEM = {ANIM_0_CONFIG, ANIM_1_CONFIG};

    CommandStream *GenerateSetupCommandStream()
    {
        return new CommandStream(new CodeCommandStreamDataSource(SETUP_DATAARRAY, 1));
    }

    CommandStream *GenerateCommandStreamByIndex(byte animationIndex)
    {
        switch (animationIndex)
        {
        case 0:
            return new CommandStream(new CodeCommandStreamDataSource(ANIM_0_DATAARRAY, 1, ANIM_0_LOOP));
        case 1:
            return new CommandStream(new CodeCommandStreamDataSource(ANIM_1_DATAARRAY, 1, ANIM_1_LOOP));

        }
        return nullptr;
    }

    byte getAnimationCount()
    {
        return 2;
    }

    const uint16_t *getConfigValues(uint8_t animationIndex)
    {
        // AVR Needs to move it to sram    
#if defined(ARDUINO_ARCH_AVR)
#define CONFIG_SIZE 8

        static uint16_t copy[CONFIG_SIZE];
        const uint16_t *flashPtr =
            (const uint16_t *)pgm_read_word_near(&CONFIGS_ARRAY[animationIndex]);

        for (uint8_t i = 0; i < CONFIG_SIZE; i++)
        {
            copy[i] = pgm_read_word_near(&flashPtr[i]);
        }
        return copy;
#else
        // Everything else can use normal accessing
        return CONFIGS_ARRAY[animationIndex];
#endif
    }
}
