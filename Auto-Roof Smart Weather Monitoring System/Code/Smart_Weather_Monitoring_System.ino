// ---- Blynk Template Info ----
#define BLYNK_TEMPLATE_ID        "TMPL3czG1h-TZ"
#define BLYNK_TEMPLATE_NAME      "smart weather forecast"
#define BLYNK_AUTH_TOKEN         "CkRSKcXYFtqmqaDxz3jUgw8hH-KvchnO"
 
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
 
// ---- PIN DEFINITIONS ----
#define DHTPIN D5
#define DHTTYPE DHT11
 
#define RAIN_ANALOG A0
#define LDR_PIN D0       // OK
#define SERVO_PIN D6     // Servo on D6
 
#define LED_PRESS D4     // Boot-safe HIGH
#define LED_TEMP D7
 
// ---- WIFI ----
char ssid[] = "Infinix..94677";
char pass[] = "sindhu000";
 
// ---- SENSOR OBJECTS ----
Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);
Servo myservo;
 
float temperature, humidity, pressure;
int rainValue;
int ldrValue;
 
bool servoAt180 = false;   // servo state
bool servoAt0 = true;      // default starting state
 
void setup() {
 
  Serial.begin(115200);
  delay(500);
 
  // WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
 
  // Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
 
  // Sensors
  Wire.begin(D2, D1);  // SDA=D2 , SCL=D1
  if (!bmp.begin()) Serial.println("BMP085 sensor not found!");
  dht.begin();
 
  // Servo
  myservo.attach(SERVO_PIN);   // D6
 
  // LEDs
  pinMode(LED_PRESS, OUTPUT);
  pinMode(LED_TEMP, OUTPUT);
 
  Serial.println("Weather Station Started...");
}
 
void loop() {
  Blynk.run();
 
  // --- Read Sensors ---
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  pressure = bmp.readPressure() / 100.0;
  rainValue = analogRead(RAIN_ANALOG);
  ldrValue = analogRead(LDR_PIN);
 
  // --- LDR Threshold ---
  int ldrStatus = (ldrValue > 500) ? 1 : 0;
 
  // --- Servo Based on Rain Value ---
  int rainThreshold = 100;
 
  if (rainValue > rainThreshold && !servoAt180) {
    // Rotate FORWARD to 180 degree
    Serial.println("Rain > 100 → Servo turning to 180°");
    myservo.write(180);
    delay(700);   // allow motion
    myservo.write(90);  // STOP torque
    servoAt180 = true;
    servoAt0 = false;
  }
 
  if (rainValue <= rainThreshold && !servoAt0) {
    // Rotate BACKWARD to 0 degree
    Serial.println("Rain <= 100 → Servo returning to 0°");
    myservo.write(0);
    delay(700);   // allow motion
    myservo.write(90);  // STOP torque
    servoAt0 = true;
    servoAt180 = false;
  }
 
  // --- Pressure LED ---
  digitalWrite(LED_PRESS, pressure > 1000);
 
  // --- Temperature LED ---
  digitalWrite(LED_TEMP, temperature > 40);
 
  // --- Send Data to Blynk ---
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, pressure);
  Blynk.virtualWrite(V3, rainValue);
  Blynk.virtualWrite(V4, ldrStatus);
 
  // --- Serial Monitor ---
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print("  Hum: "); Serial.print(humidity);
  Serial.print("  Pressure: "); Serial.print(pressure);
  Serial.print("  Rain: "); Serial.print(rainValue);
  Serial.print("  Servo: "); Serial.print(servoAt180 ? "180°" : "0°");
  Serial.print("  LDR: "); Serial.println(ldrStatus);
 
  delay(400);
}
