#include <SPI.h>
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

#define delayMillis 25000UL

unsigned long thisMillis = 0;
unsigned long lastMillis = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(2000);
  Serial.println(Ethernet.localIP());
  Serial.println("connecting...");
  Serial.println(F("Ready"));
}

void loop()
{
  // If using a static IP, comment out the next line
  Ethernet.maintain();


  thisMillis = millis();

  if(thisMillis - lastMillis > delayMillis) {
    lastMillis = thisMillis;

    h = dht.readHumidity();
    t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    dtostrf(h, 3, 2, h_str);
    dtostrf(t, 3, 2, t_str);

    sprintf(params,"t=%s&h=%s&add=sub", t_str, h_str);
    if(!postPage(serverName,serverPort,pageName,params)) Serial.print(F("Fail "));
    else Serial.print(F("Pass "));
    Serial.println(params);
  }

  delay(100);
}


byte postPage(char* domainBuffer,int thisPort,char* page,char* thisData)
{
  int inChar;
  char outBuf[64];

  Serial.print(F("connecting..."));

  if(client.connect(domainBuffer, thisPort) == 1) {
    Serial.println(F("connected"));
    sprintf(outBuf,"POST %s HTTP/1.1", page);
    client.println(outBuf);
    sprintf(outBuf,"Host: %s", domainBuffer);
    client.println(outBuf);
    client.println(F("Connection: keep-alive\r\nContent-Type: application/x-www-form-urlencoded"));
    sprintf(outBuf, "Content-Length: %u\r\n", strlen(thisData));
    client.println(outBuf);

    // send the body (variables)
    client.print(thisData);
  } else {
    Serial.println(F("failed"));
    return 0;
  }

  int connectLoop = 0;

  while(client.connected())
  {
    while(client.available())
    {
      inChar = client.read();
      Serial.write(inChar);
      connectLoop = 0;
    }

    delay(1);
    connectLoop++;
    if(connectLoop > 10000)
    {
      Serial.println();
      Serial.println(F("Timeout"));
      client.stop();
    }
  }

  Serial.println();
  Serial.println(F("disconnecting."));
  client.stop();
  return 1;
}
