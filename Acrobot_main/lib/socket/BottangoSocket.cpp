#include "BottangoSocket.h"

BottangoSocket::BottangoSocket(RemoteDebug &Debug, Menu &menu, Arm &armL, Arm &armR, Leg &legL, Leg &legR) : Debug(Debug), menu(menu)
{
  limbs[0] = &armL;
  limbs[1] = &armR;
  limbs[2] = &legL;
  limbs[3] = &legR;
}

void BottangoSocket::update()
{
  if (!connected)
  {
    return;
  }
  if (client.available() > 5)
  {
    String line = client.readStringUntil('\n');
    String splitValues[10];

    int splitCount = splitString(line, ',', splitValues, 10);

    // handshake
    if (splitCount >= 1 && splitValues[0] == "hRQ")
    {
      String handshake = "btngoHSK,CUSTOM7," + String(splitValues[1]) + ",1\n";
      client.print(handshake);
    }

    // set motor min & max
    if (splitCount >= 3 && splitValues[0] == "rMTR")
    {
      for (const auto &mapping : nameToIndex)
      {
        if (splitValues[1] == mapping.keyword)
        {
          minDegrees[mapping.index] = splitValues[2].toInt();
          maxDegrees[mapping.index] = splitValues[3].toInt();
          break; // Exit loop once the pattern is matched
        }
      }
    }

    // set motor position
    if (splitCount >= 2 && splitValues[0] == "sCI")
    {
      for (const auto &mapping : nameToIndex)
      {
        if (splitValues[1] == mapping.keyword)
        {
          int target = splitValues[2].toInt();

          float targetDegrees = fMap(target, 0, BOTTANGO_MAX_VALUE, minDegrees[mapping.index], maxDegrees[mapping.index]);
          
          limbs[mapping.index]->setTarget(targetDegrees, menu.getP(), menu.getD());
          break; // Exit loop once the pattern is matched
        }
      }
    }


    client.print("OK\n");
  }
}

void BottangoSocket::start()
{
  if (!client.connect(host, port))
  {
    debugE("Bottango socket connection failed!");
    return;
  }
  connected = true;
}

void BottangoSocket::stop()
{
  connected = false;
  client.stop();
}
