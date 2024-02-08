#include "Sequencer.h"

Sequencer::Sequencer(MovePlayer &movePlayer, RemoteDebug &Debug) : movePlayer(movePlayer), Debug(Debug), file(nullptr) {}

void Sequencer::update()
{
if (sequenceStartTime + moveStartTime < millis()){
  if (moveParsedWaiting)
  {
    moveParsedWaiting = false;
    if (moveCommand == SEQ_MOVEMENT)
    {
      movePlayer.startMove(moveFile, false, moveRepeat, moveKp, moveKi, moveStartSpeed);
    }
    else if (moveCommand == SEQ_START_ONLY)
    {
      movePlayer.startMove(moveFile, true, moveRepeat, moveKp, moveKi, moveStartSpeed);
    }
    parseNextMove();
  }
}
}

void Sequencer::startSequence(const char *csvDir, bool repeat)
{
  if (!openFile(csvDir))
  {
    debugE("Sequence: Failed to open file %s", csvDir);
    return;
  }
  sequenceStartTime = millis();

  // to bypass header
  file.readStringUntil('\n');
  parseNextMove();
}

bool Sequencer::openFile(const char *filename)
{
  file.close(); // Close any previously opened file
  moveParsedWaiting = false;

  if (!SD.exists(filename))
  {
    debugE("Seq: File does not exist: %s", filename);
    return false;
  }

  file = SD.open(filename, FILE_READ);

  if (!file)
  {
    // File opening failed
    debugE("Seq: Failed to open file: %s", filename);
    return false;
  }

  return true;
}


void Sequencer::parseNextMove()
{
  // writing my own csv parser because the lib could only deal with 1 file at a time

  if (!file)
  {
    return;
  }

  if (file.available())
  {

    String line = file.readStringUntil('\n');

    int pos = 0;
    String values[10]; // Assuming there are 10 values in each line
    // TODO: Don't parse the 'notes' column.
    while (line.length() > 0)
    {
      int index = line.indexOf(',');
      if (index >= 0)
      {
        values[pos++] = line.substring(0, index);
        line = line.substring(index + 1);
      }
      else
      {
        values[pos++] = line;
        break;
      }
    }

    if (values[0] == ""){
      debugD("Seq: No duration found. Ending file.");
      file.close();
      return;
    }

    // start time
    moveStartTime = values[0].toInt();

    // command type
    if (values[2] == "movement")
    {
      moveCommand = SEQ_MOVEMENT;
    }
    else if (values[2] == "start_only")
    {
      moveCommand = SEQ_START_ONLY;
    }
    // file name, add "/" to file name
    if (values[3].length() + 1 < sizeof(moveFile))
    {
      String concatenated = "/" + values[3];
      strcpy(moveFile, concatenated.c_str());
    }
    else
    {
      debugE("Seq: Move file name too long");
    }
    // repeat
    if (values[5] == "TRUE")
    {
      moveRepeat = true;
    }
    else
    {
      moveRepeat = false;
    }
    // start speed, kp, ki
    moveStartSpeed = values[6].toFloat();
    moveKp = values[7].toFloat();
    moveKi = values[8].toFloat();

    debugD("Seq: Move parsed: %d, %d, %s, %d, %d, %f, %f, %f", moveStartTime, moveCommand, moveFile, moveRepeat, moveStartSpeed, moveKp, moveKi);

    moveParsedWaiting = true;
  } else {
    debugD("Seq: End of file reached");
    file.close();
  }


}
