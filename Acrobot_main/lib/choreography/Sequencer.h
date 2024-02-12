#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "Arduino.h"
#include "SD.h"
#include "MovePlayer.h"
#include "RemoteDebug.h"

enum MoveCommandType
{
  SEQ_MOVEMENT,
  SEQ_START_ONLY
};

class Sequencer
{

public:
  Sequencer(MovePlayer &movePlayer, RemoteDebug &Debug);
  void update();
  void startSequence(const char *csvDir, bool repeat = false);
  void stop();

private:
  MovePlayer &movePlayer;
  RemoteDebug &Debug;
  File file;

  bool stopped = true;

  uint32_t sequenceStartTime = 0;
  bool moveParsedWaiting = false;

  MoveCommandType moveCommand;
  uint32_t moveStartTime = 0;
  char moveFile[256];
  bool moveRepeat = false;
  float moveStartSpeed = 0;
  float moveKp = 0;
  float moveKi = 0;

  bool openFile(const char *filename);
  void closeFile();
  void parseNextMove();

};


#endif