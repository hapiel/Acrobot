#include <WiFi.h>
#include <wifiConfig.h>

const uint16_t port = 59225;
const char *host = "192.168.20.126";

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

void loop()
{

  // client.print("Hello from ESP32!");

  if (client.available() > 2)
  {
    uint32_t start = millis();
    String line = client.readStringUntil('\n');
    Serial.println(line);
    Serial.println(millis() - start);
  }

  // delay(1000);
}