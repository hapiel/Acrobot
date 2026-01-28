// GENERATED CODE. DO NOT CHANGE
// Trigger animations automatically using the configuration in the app, or callbacks in the BottangoArduinoCallbacks.cpp file
// Look at the method "onLateLoop" in the callbacks file for examples

// Animation Index Key:
// 0: staan
// 1: salto_clapping


// !!! Do not change the below, unless you know what you are doing.
// !!!
// !!!


#include "GeneratedCodeAnimations.h"
#include "src/CodeCommandStreamDataSource.h"

namespace GeneratedCodeAnimations
{
    // setup
    const char SETUP_DATA_0[] PROGMEM = "rSVPin,12,2400,549,3000,981\nrSVPin,27,550,2400,3000,1968\nrSVPin,25,2400,550,3000,1475\nrSVPin,26,550,2400,3000,1475\n";
    const char *const SETUP_DATAARRAY[] PROGMEM = {SETUP_DATA_0};

    // animation 0, "staan"
    const char ANIM_0_DATA0[] PROGMEM = "\n";
    const char *const ANIM_0_DATAARRAY[] PROGMEM = {ANIM_0_DATA0};
    const char ANIM_0_LOOP[] PROGMEM = "";
    const uint16_t ANIM_0_CONFIG[] PROGMEM = {1, 0, 0, 0, 0, 0, 0, 0};

    // animation 1, "salto_clapping"
    const char ANIM_1_DATA0[] PROGMEM = "sC,26,0,2833,4096,708,0,4096,-708,0\nsC,25,0,2833,4096,708,0,4096,-708,0\nsC,27,0,167,6133,41,0,6281,-41,0\nsC,12,0,167,6731,41,0,6281,-41,0\nsC,27,167,2066,6281,514,0,1237,-514,0\nsC,12,167,2066,6281,516,0,1237,-516,0\nsC,27,2233,600,1237,150,0,1237,-150,0\nsC,12,2233,600,1237,150,0,1237,-150,0\nsC,12,2833,1134,1237,283,0,2254,-283,0\nsC,27,2833,1134,1237,283,0,2254,-283,0\nsC,26,2833,1134,4096,283,0,2936,-283,0\nsC,25,2833,1134,4096,283,0,2936,-283,0\nsC,25,3967,466,2936,116,0,2936,-116,0\nsC,27,3967,466,2254,116,0,2254,-116,0\nsC,26,3967,466,2936,116,0,2936,-116,0\nsC,12,3967,466,2254,116,0,2254,-116,0\nsC,25,4433,567,2936,141,0,4096,-141,0\nsC,26,4433,567,2936,141,0,4096,-141,0\nsC,12,4433,567,2254,141,0,1237,-141,0\nsC,27,4433,567,2254,141,0,1237,-141,0\nsC,26,5000,1133,4096,283,0,2936,-283,0\nsC,12,5000,1133,1237,283,0,2254,-283,0\nsC,25,5000,1133,4096,283,0,2936,-283,0\nsC,27,5000,1133,1237,283,0,2254,-283,0\nsC,12,6133,467,2254,116,0,2254,-116,0\nsC,25,6133,467,2936,116,0,2936,-116,0\nsC,27,6133,467,2254,116,0,2254,-116,0\nsC,26,6133,467,2936,116,0,2936,-116,0\nsC,12,6600,567,2254,141,0,1237,-141,0\nsC,26,6600,567,2936,141,0,4096,-141,0\nsC,25,6600,567,2936,141,0,4096,-141,0\nsC,27,6600,567,2254,141,0,1237,-141,0\nsC,25,7167,1133,4096,283,0,2936,-283,0\nsC,12,7167,1133,1237,283,0,2254,-283,0\nsC,26,7167,1133,4096,283,0,2936,-283,0\nsC,27,7167,1133,1237,283,0,2254,-283,0\nsC,26,8300,467,2936,116,0,2936,-116,0\nsC,27,8300,467,2254,116,0,2254,-116,0\nsC,12,8300,467,2254,116,0,2254,-116,0\nsC,25,8300,467,2936,116,0,2936,-116,0\nsC,12,8767,566,2254,141,0,1237,-141,0\nsC,25,8767,566,2936,141,0,4096,-141,0\nsC,27,8767,566,2254,141,0,1237,-141,0\nsC,26,8767,566,2936,141,0,4096,-141,0\n\n";
    const char *const ANIM_1_DATAARRAY[] PROGMEM = {ANIM_1_DATA0};
    const char ANIM_1_LOOP[] PROGMEM = "sC,12,20000,828,1237,414,0,6731,-414,0\nsC,27,20000,738,1237,369,0,6133,-369,0\n";
    const uint16_t ANIM_1_CONFIG[] PROGMEM = {0, 0, 0, 36, 0, 0, 0, 0};


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
