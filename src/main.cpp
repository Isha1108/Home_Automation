#include <Arduino.h>
#include <HttpClient.h>
#include <WiFi.h>
#include <inttypes.h>
#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "Wire.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_AHTX0.h>
#include <Keypad.h>

#define MotorPin 21
#define delay1 5000
#define RED_PIN 32
#define YELLOW_PIN 22
#define GREEN_PIN 17

// Define the rows and columns of the keypad
const byte ROWS = 4; // 4 rows
const byte COLS = 4; // 4 columns

// Define the key map for the keypad
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Define the row and column pins connected to the keypad
byte rowPins[ROWS] = {12, 13, 15, 2}; // Row pins
byte colPins[COLS] = {27, 26, 25, 33}; // Column pins

bool RED_STATE = true;
bool YELLOW_STATE = true;
bool GREEN_STATE = true;
bool MOTOR_STATE = true;

// Create a Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


Adafruit_AHTX0 aht;
 // This example downloads the URL "http://arduino.cc/"
char ssid[50]; // your network SSID (name)
char pass[50]; // your network password (use for WPA, or use
// as key for WEP)
// Name of the server we want to connect to
const char kHostname[] = "worldtimeapi.org";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
const char kPath[] = "/api/timezone/Europe/London.txt";
// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30 * 1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;
void nvs_access() {
  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
    err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  // Open
  Serial.printf("\n");
  Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, & my_handle);
  if (err != ESP_OK) {
    Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  } else {
    Serial.printf("Done\n");
    Serial.printf("Retrieving SSID/PASSWD\n");
    size_t ssid_len;
    size_t pass_len;
    err = nvs_get_str(my_handle, "ssid", ssid, & ssid_len);
    err |= nvs_get_str(my_handle, "pass", pass, & pass_len);
    switch (err) {
    case ESP_OK:
      Serial.printf("Done\n");
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      Serial.printf("The value is not initialized yet!\n");
      break;
    default:
      Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
  }
  // Close
  nvs_close(my_handle);
}
void setup() {
  Serial.begin(9600);
  delay(1000);
  // Retrieve SSID/PASSWD from flash before anything else
  nvs_access();
  // We start by connecting to a WiFi network
  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
    Serial.println("4x4 Keypad Test");
    pinMode(MotorPin, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
  // if (! aht.begin()) {
  //   Serial.println("Could not find AHT? Check wiring");
  //   while (1) delay(10);
  // }
  // Serial.println("AHT10 or AHT20 found");
}
void loop() {
  int err = 0;
  WiFiClient c;
  HttpClient http(c);
  // sensors_event_t humidity, temp;
  // aht.getEvent(&humidity, &temp);

  // String url = String("/?temperature=") + temp.temperature + "&humidity=" + humidity.relative_humidity;
  // err = http.get("18.144.72.39", 5000, url.c_str(), NULL);

  // Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  // Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

    char key = keypad.getKey();
    if (key == '0') { 
        digitalWrite(MotorPin, MOTOR_STATE ? HIGH : LOW);
        MOTOR_STATE = !MOTOR_STATE;
    } else if(key == '2'){
      digitalWrite(RED_PIN, RED_STATE ? HIGH : LOW);
      RED_STATE = !RED_STATE;
    } else if(key == '3'){
      digitalWrite(YELLOW_PIN, YELLOW_STATE ? HIGH : LOW);
      YELLOW_STATE = !YELLOW_STATE;
    }else if(key == '4'){
      digitalWrite(GREEN_PIN, GREEN_STATE ? HIGH : LOW);
      GREEN_STATE = !GREEN_STATE;
    }

    String url = String("/api/data?MOTOR_STATE=") + MOTOR_STATE + "&RED_STATE=" + RED_STATE + "&YELLOW_STATE=" + YELLOW_STATE + "&GREEN_STATE=" + GREEN_STATE ;
    err = http.get("18.144.72.39", 5000, url.c_str(), NULL);
    delay(3000);


  if (err == 0) {
    Serial.println("startedRequest ok");
    err = http.responseStatusCode();
    if (err >= 0) {
      Serial.print("Got status code: ");
      Serial.println(err);
      err = http.skipResponseHeaders();
      if (err >= 0) {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ((http.connected() || http.available()) &&
          ((millis() - timeoutStart) < kNetworkTimeout)) {
          if (http.available()) {
            c = http.read();
            // Print out this character
            Serial.print(c);
            bodyLen--;
            timeoutStart = millis();
          } else {
            delay(kNetworkDelay);
          }
        }
      } else {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    } else {
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  } else {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
}

