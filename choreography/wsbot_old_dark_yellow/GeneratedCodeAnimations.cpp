// GENERATED CODE. DO NOT CHANGE
// Trigger animations automatically using the configuration in the app, or callbacks in the BottangoArduinoCallbacks.cpp file
// Look at the method "onLateLoop" in the callbacks file for examples

// Animation Index Key:
// 0: staan
// 1: EJC_acro
// 2: EJC_balance pose
// 3: EJC_wave
// 4: EJC_bow


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

    // animation 1, "EJC_acro"
    const char ANIM_1_DATA0[] PROGMEM = "sSY,sC,27,0,5000,6281,1250,0,7258,-1250,0;26,0,9000,4096,2250,0,4096,-441,0;25,0,9000,4096,2250,0,4096,-441,0;12,0,5000,6281,1250,0,7258,-1250,0;\nsSY,sC,12,5000,2000,7258,500,0,7258,-500,0;27,5000,2000,7258,500,0,7258,-500,0;\nsSY,sC,12,7000,500,7258,125,0,7613,-125,0;27,7000,500,7258,125,0,7613,-125,0;\nsSY,sC,12,7500,500,7613,125,0,7613,-125,0;27,7500,500,7613,125,0,7613,-125,0;\nsSY,sC,27,8000,500,7613,125,0,7258,-125,0;12,8000,500,7613,125,0,7258,-125,0;\nsSY,sC,12,8500,500,7258,125,0,7258,-125,0;27,8500,500,7258,125,0,7258,-125,0;\nsSY,sC,25,9000,1767,4096,441,0,233,-308,0;26,9000,1767,4096,441,0,233,-308,0;12,9000,3000,7258,750,0,325,-750,0;27,9000,3000,7258,750,0,325,-750,0;\nsSY,sC,26,10767,1233,233,308,0,1022,-344,-561;25,10767,1233,233,308,0,1022,-344,-561;\nsSY,sC,12,12000,1467,325,366,0,1253,-366,0;25,12000,1467,1022,332,543,4138,-366,0;26,12000,1467,1022,332,543,4138,-366,0;27,12000,1467,325,366,0,1253,-366,0;\nsSY,sC,27,13467,2966,1253,741,0,1253,-741,0;12,13467,2966,1253,741,0,1253,-741,0;25,13467,366,4138,91,0,4138,-91,0;26,13467,366,4138,91,0,4138,-91,0;\nsSY,sC,25,13833,1967,4138,491,0,1744,-491,0;26,13833,1967,4138,491,0,6510,-491,0;\nsSY,sC,26,15800,4300,6510,1075,0,6330,-1075,0;25,15800,4300,1744,1075,0,1884,-1075,0;\nsSY,sC,27,16433,3667,1253,916,0,1298,-916,0;12,16433,2300,1253,575,0,6380,-575,0;\nsC,12,18733,1367,6380,341,0,6142,-341,0\nsSY,sC,25,20100,533,1884,133,0,1699,-133,0;12,20100,1933,6142,483,0,5675,-483,0;27,20100,533,1298,133,0,1040,-349,0;26,20100,533,6330,133,0,4829,-133,283;\nsSY,sC,26,20633,1400,4829,350,-744,4032,-350,0;27,20633,1400,1040,349,0,5675,-350,0;25,20633,1400,1699,350,0,4032,-350,0;\nsSY,sC,12,22033,334,5675,83,0,5675,-83,0;26,22033,4934,4032,1233,0,4032,-1233,0;25,22033,4934,4032,1233,0,4032,-1233,0;27,22033,334,5675,83,0,5675,-83,0;\nsSY,sC,12,22367,533,5675,133,0,1235,-133,0;27,22367,533,5675,133,0,1235,-133,0;\nsSY,sC,27,22900,667,1235,166,0,1235,-166,0;12,22900,667,1235,166,0,1235,-166,0;\nsSY,sC,12,23567,1633,1235,408,0,3745,-408,0;27,23567,1633,1235,408,0,3745,-408,0;\nsSY,sC,12,25200,500,3745,125,0,3745,-125,0;27,25200,500,3745,125,0,3745,-125,0;\nsSY,sC,12,25700,533,3745,133,0,2943,-133,0;27,25700,533,3745,133,0,2943,-133,0;\nsSY,sC,12,26233,534,2943,133,0,3745,-133,0;27,26233,534,2943,133,0,3745,-133,0;\nsSY,sC,27,26767,866,3745,216,0,0,-216,0;12,26767,866,3745,216,0,0,-216,0;\nsSY,sC,25,26967,966,4032,241,0,2192,-241,0;26,26967,966,4032,241,0,2192,-241,0;\nsSY,sC,27,27633,3334,0,833,0,0,-833,0;12,27633,3334,0,833,0,0,-833,0;\nsSY,sC,25,27933,3034,2192,758,0,2192,-758,0;26,27933,3034,2192,758,0,2192,-758,0;\nsSY,sC,27,30967,3133,0,783,0,6281,-783,0;12,30967,3133,0,783,0,6281,-783,0;25,30967,3133,2192,783,0,4096,-783,0;26,30967,3133,2192,783,0,4096,-783,0;\n\n";
    const char *const ANIM_1_DATAARRAY[] PROGMEM = {ANIM_1_DATA0};
    const char ANIM_1_LOOP[] PROGMEM = "";
    const uint16_t ANIM_1_CONFIG[] PROGMEM = {0, 0, 0, 22, 0, 0, 0, 0};

    // animation 2, "EJC_balance pose"
    const char ANIM_2_DATA0[] PROGMEM = "sSY,sC,12,0,3967,6281,991,0,3332,-991,0;27,0,3967,6281,991,0,135,-991,0;25,0,3967,4096,991,0,4610,-991,0;26,0,3967,4096,991,0,19,-991,0;\n\n";
    const char *const ANIM_2_DATAARRAY[] PROGMEM = {ANIM_2_DATA0};
    const char ANIM_2_LOOP[] PROGMEM = "sSY,sC,12,20000,444,3332,222,0,6281,-222,0;27,20000,926,135,463,0,6281,-463,0;25,20000,78,4610,39,0,4096,-39,0;26,20000,614,19,307,0,4096,-307,0;\n";
    const uint16_t ANIM_2_CONFIG[] PROGMEM = {0, 0, 0, 36, 0, 0, 0, 0};

    // animation 3, "EJC_wave"
    const char ANIM_3_DATA0[] PROGMEM = "sSY,sC,25,0,3000,4096,750,0,4096,-750,0;26,0,3000,4096,750,0,4096,-750,0;12,0,1500,6281,374,0,1040,-374,0;27,0,1500,6281,375,0,1040,-375,0;\nsSY,sC,27,1500,500,1040,125,0,1629,-125,0;12,1500,500,1040,124,0,1629,-124,0;\nsSY,sC,12,2000,500,1629,125,0,1040,-125,0;27,2000,500,1629,125,0,1040,-125,0;\nsSY,sC,12,2500,500,1040,125,0,1629,-125,0;27,2500,500,1040,125,0,1629,-125,0;\nsSY,sC,25,3000,4000,4096,1000,0,4096,-1000,0;26,3000,4000,4096,1000,0,4096,-1000,0;12,3000,4000,1629,1000,0,1629,-1000,0;27,3000,4000,1629,1000,0,1629,-1000,0;\nsSY,sC,26,7000,3000,4096,750,0,4096,-750,0;25,7000,3000,4096,750,0,4096,-750,0;27,7000,3000,1629,750,0,6281,-750,0;12,7000,3000,1629,750,0,6281,-750,0;\n\n";
    const char *const ANIM_3_DATAARRAY[] PROGMEM = {ANIM_3_DATA0};
    const char ANIM_3_LOOP[] PROGMEM = "";
    const uint16_t ANIM_3_CONFIG[] PROGMEM = {0, 0, 0, 39, 0, 0, 0, 0};

    // animation 4, "EJC_bow"
    const char ANIM_4_DATA0[] PROGMEM = "sSY,sC,26,0,1800,19,450,0,4096,-450,0;27,0,2033,135,508,0,5426,-508,0;25,0,1800,4610,450,0,4096,-449,0;12,0,2033,3332,508,0,5426,-507,0;\nsSY,sC,26,1800,1267,4096,316,0,1050,-316,0;25,1800,1267,4096,316,0,1050,-316,0;\nsSY,sC,27,2033,1034,5426,258,0,3276,-258,0;12,2033,1034,5426,258,0,3276,-258,0;\nsSY,sC,26,3067,2733,1050,683,0,1050,-683,0;25,3067,2733,1050,683,0,1050,-683,0;27,3067,2733,3276,683,0,3276,-683,0;12,3067,2733,3276,683,0,3276,-683,0;\nsSY,sC,26,5800,1533,1050,383,0,4096,-383,0;12,5800,1533,3276,383,0,6281,-383,0;27,5800,1533,3276,383,0,6281,-383,0;25,5800,1533,1050,383,0,4096,-383,0;\n\n";
    const char *const ANIM_4_DATAARRAY[] PROGMEM = {ANIM_4_DATA0};
    const char ANIM_4_LOOP[] PROGMEM = "sSY,sC,12,20000,444,6281,222,0,3332,-222,0;27,20000,926,6281,463,0,135,-463,0;25,20000,78,4096,39,0,4610,-39,0;26,20000,614,4096,307,0,19,-307,0;\n";
    const uint16_t ANIM_4_CONFIG[] PROGMEM = {0, 0, 0, 34, 0, 0, 0, 0};


    // configs
    const uint16_t *const CONFIGS_ARRAY[] PROGMEM = {ANIM_0_CONFIG, ANIM_1_CONFIG, ANIM_2_CONFIG, ANIM_3_CONFIG, ANIM_4_CONFIG};

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
        case 2:
            return new CommandStream(new CodeCommandStreamDataSource(ANIM_2_DATAARRAY, 1, ANIM_2_LOOP));
        case 3:
            return new CommandStream(new CodeCommandStreamDataSource(ANIM_3_DATAARRAY, 1, ANIM_3_LOOP));
        case 4:
            return new CommandStream(new CodeCommandStreamDataSource(ANIM_4_DATAARRAY, 1, ANIM_4_LOOP));

        }
        return nullptr;
    }

    byte getAnimationCount()
    {
        return 5;
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
