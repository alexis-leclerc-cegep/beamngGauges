#include <WiFi.h> 
#include <WiFiUdp.h>
#include <Arduino.h>
#include <Wire.h>
#include <credentials.h>
#include <Adafruit_SSD1306.h>

const int UDP_PORT = 4444;

const float BAR_2_PSI = 14.504;


#define I2C2_SDA 32
#define I2C2_SCL 33


char incomingPacket[96];

 typedef struct outgauge_t  {
      unsigned       time;            // I  time in milliseconds (to check order)
      char           car[4];          // 4s Car name
      unsigned short flags;           // H  Info (see OG_x below)
      char           gear;            // c  Reverse:0, Neutral:1, First:2...
      char           plid;            // c  Unique ID of viewed player (0 = none)
      float          speed;           // f  M/S
      float          rpm;             // f  RPM
      float          turbo;           // f  BAR
      float          engTemp;         // f  C
      float          fuel;            // f  0 to 1
      float          oilPressure;     // f  BAR
      float          oilTemp;         // f  C
      unsigned       dashLights;      // I  Dash lights available (see DL_x below)
      unsigned       showLights;      // I  Dash lights currently switched on
      float          throttle;        // f  0 to 1
      float          brake;           // f  0 to 1
      float          clutch;          // f  0 to 1
      char           display1[16];    // c  Usually Fuel
      char           display2[16];    // c  Usually Settings
      int            id;              // i  optional - only if OutGauge ID is specified
  } outgauge_t;

WiFiUDP udp;
outgauge_t outgauge;


Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_SSD1306 display2(128, 64, &Wire1, -1);

void setup(){
    Serial.begin(115200);

    Wire.begin();
    Wire1.begin(I2C2_SDA, I2C2_SCL, 100000);


    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display2.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display2.clearDisplay();

    display.setRotation(2);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,20);
    display.println("Waiting...");
    display.display();

    display2.setRotation(0);
    display2.setTextSize(2);
    display2.setTextColor(WHITE);
    display2.setCursor(0,20);
    display2.println("chepo...");
    display2.display();

    Serial.println("Connecting");
    WiFi.begin(WIFI_SSID, WIFI_PW);
    delay(100);


    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());


    udp.begin(UDP_PORT);
}

int counter = 0;
bool invert = false;

void loop(){
  display.clearDisplay();
  display2.clearDisplay();
  int size = udp.parsePacket();
  if (size) {
    udp.read(incomingPacket, 96);
    outgauge_t* data = (outgauge_t*)incomingPacket;
    float boost_psi = data->turbo * BAR_2_PSI;

    //display.invertDisplay(data->showLights >> 0 & 1);       //first bit of data->showLights

    display2.setTextSize(4);
    display2.setCursor(0,0);
    display2.printf("%.0f", data->rpm);

    display2.setTextSize(2);
    display2.setCursor(0,40);
    display2.println("RPM");

    display2.display();

    display.setTextSize(4);

    display.setCursor(0,0);
    display.printf("%.0f", data->speed * 3.6);         // m/s to km/h

    display.setCursor(0,40);
    display.setTextSize(2);
    display.println("KM/H");         // m/s to km/h

    display.display();

    udp.flush();
  }

}