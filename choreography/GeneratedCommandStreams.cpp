#include "GeneratedCommandStreams.h"

namespace GeneratedCommandStreams
{

    //// !!! Insert/edit your animation triggering and playback logic here !!! ////
    /* Some helpful methods:

        BottangoCore::commandStreamProvider.streamIsInProgress()                                        //returns if any animation is currently playing
        BottangoCore::commandStreamProvider.startCommandStream(byte animationID, bool looping)          //starts playing an animation (and stops the current if any)
        BottangoCore::commandStreamProvider.stop()                                                      //stops the current playing animation, if any
    */

    /* Animations key:
        0 - acro_podcheska
    */

    bool playBegan = false;
    bool playOnStart = true;
    bool loop = false;
    byte startingAnimation = 0;

    // called once per update loop, your logic probably goes here.
    void updatePlayStatus()
    {
        // if haven't started any animations, or if should loop and nothing is playing (IE the last loop ended)
        if (playOnStart && (!playBegan || (loop && !BottangoCore::commandStreamProvider.streamIsInProgress())))
        {
            BottangoCore::commandStreamProvider.startCommandStream(startingAnimation, loop);
            playBegan = true;
        }
    }

    //// ----------------------------------------- /////

    //// GENERATED CODE. You probably don't need to change any of the below ////
    const char SETUP_CHAR_STREAM[] PROGMEM = "rMTR,m_arm_r,-180,540,3000,180\nrMTR,m_arm_l,-180,540,3000,180\nrMTR,m_leg_l,30,330,3000,185\nrMTR,m_leg_r,30,330,3000,185\n";
    const char STREAM_ACRO_PODCHESKA_CHAR_STREAM[] PROGMEM = "sC,m_arm_r,0,1500,4096,375,0,4424,-375,0\nsC,m_arm_l,0,1500,4096,375,0,4424,-375,0\nsC,m_leg_l,0,5467,4233,1367,0,4233,-1367,0\nsC,m_leg_r,0,5467,4233,1367,0,4233,-1367,0\nsC,m_arm_r,1500,1500,4424,375,0,4424,-375,0\nsC,m_arm_l,1500,1500,4424,375,0,4424,-375,0\nsC,m_arm_l,3000,533,4424,133,0,4669,-133,0\nsC,m_arm_r,3000,533,4424,133,0,4669,-133,0\nsC,m_arm_r,3533,334,4669,83,0,4669,-83,0\nsC,m_arm_l,3533,334,4669,83,0,4669,-83,0\nsC,m_arm_l,3867,533,4669,133,0,4424,-133,0\nsC,m_arm_r,3867,533,4669,133,0,4424,-133,0\nsC,m_arm_l,4400,333,4424,83,0,4424,-83,0\nsC,m_arm_r,4400,333,4424,83,0,4424,-83,0\nsC,m_arm_l,4733,534,4424,133,0,4121,-133,0\nsC,m_arm_r,4733,534,4424,133,0,4121,-133,0\nsC,m_arm_l,5267,400,4121,100,0,4121,-100,-1\nsC,m_arm_r,5267,400,4121,100,0,4121,-100,-1\nsC,m_leg_l,5467,1200,4233,495,-4,0,-538,24\nsC,m_leg_r,5467,1200,4233,495,-4,0,-538,24\nsC,m_arm_l,5667,533,4121,213,2,3403,-148,401\nsC,m_arm_r,5667,533,4121,133,1,3403,-148,401\nsC,m_arm_l,6200,600,3403,240,-1008,1846,-177,0\nsC,m_arm_r,6200,600,3403,240,-1008,1846,-177,0\nsC,m_leg_l,6667,833,0,208,-9,4096,-208,0\nsC,m_leg_r,6667,833,0,208,-9,4096,-208,0\nsC,m_arm_l,6800,700,1846,285,63,2048,-175,0\nsC,m_arm_r,6800,700,1846,285,63,2048,-175,0\n";
    const char STREAM_ACRO_PODCHESKA_CHAR_STREAM_LOOP[] PROGMEM = "sC,m_arm_r,20000,240,2048,120,0,4096,-120,0\nsC,m_arm_l,20000,240,2048,120,0,4096,-120,0\nsC,m_leg_l,20000,7,4096,3,0,4233,-3,0\nsC,m_leg_r,20000,7,4096,3,0,4233,-3,0\n";

    CommandStream setupStream = CommandStream(SETUP_CHAR_STREAM, 0);
    CommandStream stream_acro_podcheska = CommandStream(STREAM_ACRO_PODCHESKA_CHAR_STREAM, 20000, STREAM_ACRO_PODCHESKA_CHAR_STREAM_LOOP, 239);    

    CommandStream *getCommandStream(byte streamID)
    {
        switch (streamID)
        {
        case 0:
            return &stream_acro_podcheska;
        default:
            return NULL;
        }
    }

    CommandStream *getSetupCommandStream()
    {
        return &setupStream;
    }
} // namespace GeneratedCommandStreams
