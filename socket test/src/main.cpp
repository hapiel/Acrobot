#include <WiFi.h>
#include <wifiConfig.h>

const uint16_t port = 59225;
const char *host = "LAPTOP-VSACRSB1.local";

WiFiClient client;

void setup()
{

  Serial.begin(115200);

  WiFi.begin(wifiSsid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("...");
  }

  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

  if (!client.connect(host, port))
  {

    Serial.println("Connection to host failed");

    return;
  }
}

int split(const String &str, char delimiter, String *arr, int maxItems)
{
  int itemCount = 0;  // Number of items found
  int startIndex = 0; // Start index of the current substring

  // Loop through each character in the string
  for (int i = 0; i < str.length(); i++)
  {
    // Check if current character is the delimiter
    if (str.charAt(i) == delimiter)
    {
      // Extract substring from startIndex to current index
      arr[itemCount++] = str.substring(startIndex, i);
      startIndex = i + 1; // Update startIndex for the next substring
    }

    // Check if maximum items reached
    if (itemCount == maxItems - 1)
    {
      // Extract the rest of the string and exit loop
      arr[itemCount++] = str.substring(startIndex);
      break;
    }
  }

  // If there are characters left after the last delimiter, extract them
  if (startIndex < str.length())
  {
    arr[itemCount++] = str.substring(startIndex);
  }

  // Return the number of items found
  return itemCount;
}

void loop()
{

  // client.print("Hello from ESP32!");

  if (client.available() > 15)
  {
    uint32_t start = millis();
    String splitValues[10];
    String line = client.readStringUntil('\n');
    int splitCount = split(line, ',', splitValues, 10);

    if (splitCount > 0 && splitValues[0] == "hRQ")
    {
      // Do something if the condition is met
      Serial.println("Found 'hRQ', doing the handshake...");
      String handshake = "btngoHSK,CUSTOM7," + String(splitValues[1]) + ",1\n";
      client.print(handshake);
    }

    client.print("OK\n");


    Serial.println(line);
    Serial.println(millis() - start);
  }

  // delay(1000);
}