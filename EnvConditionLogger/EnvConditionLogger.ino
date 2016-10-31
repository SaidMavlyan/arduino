/*
 * Environment condition logger/sender
 * 
 * This sketch reads DHT22 sensor and sends values to web server through POST requst.
 * 
 * Created 22 Nov 2015
 * by Azamat Bexadaev
 * modified 3 Jan 2016
 * 
 * Some parts of this sketch are taken from www.arduino.cc templates
 */

/* Libraries */

#include <EthernetServer.h>
#include <Dhcp.h>
#include <EthernetClient.h>
#include <EthernetUdp.h>
#include <Ethernet.h>
//#include <Wire.h>
#include <SPI.h>
#include <DHT.h>

/* Global variables */
#define DHTPIN 2        // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
EthernetClient client;
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const long interval = 120000;           // interval at which to send POST
float h,t;


void setup() {

  Serial.begin(115200);
  dht.begin();

  Serial.println("Sketch started");

  while(Ethernet.begin(mac) == 0){
    // statement(s)
    delay(3000);
    Serial.println(Ethernet.localIP());
  }
  
//  if (Ethernet.begin(mac) == 0) {
//    Serial.println("Failed to configure Ethernet using DHCP");
//  }
  
  Serial.println(Ethernet.localIP());
    
  // end setup
  } 



void POSTRequest(){
  
//Serial.println(sizeof(h));
//Serial.println(sizeof(t));
Serial.println(h,DEC);
Serial.println(t,DEC);

    if  (client.connect("192.168.0.120",80)){
//    if  (client.connect("http://logger.neman.kg",80)){

      Serial.println("Sending to Server: ");
      
//      client.println("POST /add.php HTTP/1.1");
      client.println("POST /logger/add.php HTTP/1.1");
        
      client.println("Host: 192.168.0.120");
//      client.println("Host: logger.neman.kg");

      client.println("User-Agent: Arduino/1.0");
//      client.println("Accept: text/html;q=0.9,*/*;q=0.8");
//      client.println("Accept-Language: en-US,en;q=0.5");
//      client.println("Accept-Encoding: gzip, deflate");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(39, DEC);
      client.println("Connection: close");
      client.println();
      //parameters
      client.print("t=");
      client.print(t,DEC);
      client.print("&h=");
      client.print(h,DEC);
      client.println("&add=sub");

//      return 1;
    }
    else {
      Serial.println("Connection failed");
//      return 0;
    }
    client.stop();
}

 
void loop() {

  currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    t = dht.readTemperature();

    previousMillis = currentMillis;
    
    int eth_maintain = Ethernet.maintain();

    Serial.println(eth_maintain);
    Serial.println(Ethernet.localIP());
    
//    if (eth_maintain != 2) {
      
//    }
    
    POSTRequest();
    
/*    if(SendStatus){
      Serial.println("Data are sent");
    }
    else {
      Serial.println("Cannot connect to Server");
    }*/
  }
  else {
    //here can be any code executed between http sending
  }

//end loop
}
