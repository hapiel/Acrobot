// GENERATED CODE. DO NOT CHANGE
// Trigger animations automatically using the configuration in the app, or callbacks in the BottangoArduinoCallbacks.cpp file
// Look at the method "onLateLoop" in the callbacks file for examples

// Animation Index Key:
// 0: staan
// 1: Matthias flip


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
    const char ANIM_0_LOOP[] PROGMEM = "sC,25,20000,784,1070,392,0,6281,-392,0\nsC,26,20000,784,1070,392,0,6281,-392,0\nsC,12,20000,8,4150,4,0,4096,-4,0\nsC,27,20000,8,4150,4,0,4096,-4,0\n";
    const uint16_t ANIM_0_CONFIG[] PROGMEM = {1, 0, 0, 0, 0, 0, 0, 0};

    // animation 1, "Matthias flip"
    const char ANIM_1_DATA0[] PROGMEM = "sC,12,0,4967,2589,1241,0,2589,-1241,0\nsC,27,0,4967,2589,1241,0,2589,-1241,0\nsC,25,0,4967,2355,1241,0,2355,-1241,0\nsC,26,0,4967,2355,1241,0,2355,-1241,0\nsC,25,4967,5033,2355,1257,0,6,-1257,0\nsC,12,4967,5033,2589,1257,0,5444,-1257,0\nsC,26,4967,5033,2355,1258,0,6,-1258,0\nsC,27,4967,5033,2589,1258,0,5444,-1258,0\nsC,12,10000,5000,5444,1250,0,5444,-1250,0\nsC,27,10000,5000,5444,1250,0,5444,-1250,0\nsC,25,10000,5000,6,1250,0,6,-1250,0\nsC,26,10000,5000,6,1250,0,6,-1250,0\nsC,27,15000,5000,5444,1250,0,2589,-1250,0\nsC,25,15000,5000,6,1250,0,2355,-1250,0\nsC,12,15000,5000,5444,1250,0,2589,-1250,0\nsC,26,15000,5000,6,1250,0,2355,-1250,0\n\n";
    const char *const ANIM_1_DATAARRAY[] PROGMEM = {ANIM_1_DATA0};
    const char ANIM_1_LOOP[] PROGMEM = "sC,26,20000,194,1070,97,0,2355,-97,0\nsC,25,20000,194,1070,97,0,2355,-97,0\nsC,27,20000,236,4150,118,0,2589,-118,0\nsC,12,20000,236,4150,118,0,2589,-118,0\n";
    const uint16_t ANIM_1_CONFIG[] PROGMEM = {0, 0, 0, 22, 1, 0, 0, 0};


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
