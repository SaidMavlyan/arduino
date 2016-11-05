#include <Ethernet.h>
#include <DHT.h>

#define DHTPIN 2        // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

char serverName[] = "logger.neman.kg";
char pageName[] = "/add.php";
char params[90];
char h_str[6];
char t_str[6];
int serverPort = 80;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
float h,t;

EthernetClient client;
IPAddress ip(192, 168, 0, 177);

#define delayMillis 60000UL

unsigned long thisMillis = 0;
unsigned long lastMillis = -delayMillis;

void setup() {
  Serial.begin(9600);
  dht.begin();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Setting up");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }

  delay(2000);
  Serial.println(Ethernet.localIP());
  Serial.println("Ready");
}

void loop()
{
  if (Ethernet.maintain()) {
    Serial.println("Maintain");
    setup();
  }

  thisMillis = millis();

  if (thisMillis - lastMillis > delayMillis) {
    lastMillis = thisMillis;

    h = dht.readHumidity();
    t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    dtostrf(h, 3, 2, h_str);
    dtostrf(t, 3, 2, t_str);

    Serial.println(freeRam());
    Serial.println(params);

    sprintf(params, "t=%s&h=%s&add=sub", t_str, h_str);
    if (postPage(serverName, serverPort, pageName, params)) {
      Serial.print("****************Fail ");
      setup();
    } else {
      Serial.print("Pass ");
    }
  }

  delay(100);
}

int freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

byte postPage(char* domainBuffer, int thisPort, char* page, char* thisData)
{
  char outBuf[64];

  Serial.println("Connecting...");

  if (client.connect(domainBuffer, thisPort) == 1) {
    Serial.println("Sending...");
    sprintf(outBuf,"POST %s HTTP/1.1", page);
    client.println(outBuf);
    sprintf(outBuf,"Host: %s", domainBuffer);
    client.println(outBuf);
    client.println("Connection: keep-alive\r\nContent-Type: application/x-www-form-urlencoded");
    sprintf(outBuf, "Content-Length: %u\r\n", strlen(thisData));
    client.println(outBuf);
    client.print(thisData);
  } else {
    Serial.println("failed");
    return 1;
  }

  delay(delayMillis/2);
  client.flush();
  client.stop();
  return 0;
}
