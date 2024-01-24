#include "movePlayer.h"

MovePlayer::MovePlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR, File &file, CSV_Parser &cp)
    : Debug(Debug), file(file), cp(cp)
{
    limbs[0] = &armL;
    limbs[1] = &armR;
    limbs[2] = &legL;
    limbs[3] = &legR;
}

void MovePlayer::update()
{
  if (state == MOVE_TO_BEGIN)
  {
    // Implement the logic to move to the beginning
  }
  else if (state == MOVE_CURVES)
  {
    // Implement the logic to move the curves
  }
}

void MovePlayer::stop()
{
  state = IDLE;
}

void MovePlayer::startMove(const char *csvDir, float kp, float ki)
{
  // Implement the logic to start the movement using the specified CSV file
}

void MovePlayer::moveToBegin(const char *csvDir, float kp, float ki)
{
  // Implement the logic to move to the beginning using the specified CSV file
}

