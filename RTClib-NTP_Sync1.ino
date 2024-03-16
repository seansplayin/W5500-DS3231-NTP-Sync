#include <SPI.h>
#include <WebServer_ESP32_SC_W5500.h>
#include <RTClib.h>
#include <Wire.h>



// Ethernet settings for W5500
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 20, 90, 33); // Adjust to match your network settings

// Specify your custom SPI pins for the W5500 module
#define W5500_MOSI 11
#define W5500_MISO 13
#define W5500_SCK 12
#define W5500_SS 10
#define W5500_INT 4



// RTC DS3231
RTC_DS3231 rtc;
const int pinSDA = 36;
const int pinSCL = 37;

// Initialize NTP
    void initNTP() {
    Serial.println("Starting NTP time sync");
    // Initialize NTP with MST time zone and automatic DST adjustment to MDT
    configTime(-6 * 3600, 0, "pool.ntp.org", "time.nist.gov", "MST7MDT");
    Serial.println("Waiting for NTP time sync...");

    unsigned long startAttemptTime = millis();
    time_t nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600) {
        if (millis() - startAttemptTime > 20000) { // 20 seconds timeout
            Serial.println("NTP sync timed out.");
            return;
        }
        delay(500);
        Serial.print(".");
        nowSecs = time(nullptr);
    }

    Serial.println("\nNTP Time synchronize Successful");
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

    // Adjust RTC with NTP time
    rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, 
                        timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
  
    Serial.println("RTC adjusted to NTP time.");
}


void setup() {
  Serial.begin(115200);
    Wire.begin(pinSDA, pinSCL); // Initialize I2C

     if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  // Initialize Ethernet connection
    SPI.begin();
    ETH.begin(W5500_MISO, W5500_MOSI, W5500_SCK, W5500_SS, W5500_INT, 25, SPI3_HOST, mac);
      

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
   initNTP();
    

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  
  // Adjust RTC with NTP time
  rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
  
  Serial.println("RTC adjusted to NTP time.");

}

void loop() {
  DateTime now = rtc.now();
  
  Serial.print("Current time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  
  delay(1000); // Delay for 1 second
}
