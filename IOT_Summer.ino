
#include <HTTPClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

const int trigPin = 12;
const int echoPin = 13;
const int servoPin = 5;

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo myServo;

String URL = "http://192.168.1.109/store_data.php";

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

int distance = 0;
int bottle_count = 0;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myServo.attach(servoPin);
  myServo.write(0);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Bottles: 0");
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  Load_data();

  String postData = "Distance=" + String(distance) + "&Bottle_count=" + String(bottle_count);

  HTTPClient http;
  http.begin(URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(postData);
  String payload = http.getString();

  Serial.print("URL : "); Serial.println(URL);
  Serial.print("Data: "); Serial.println(postData);
  Serial.print("httpCode: "); Serial.println(httpCode);
  Serial.print("payload : "); Serial.println(payload);
  Serial.println("--------------------------------------------------");

  if (httpCode != 200) {
    Serial.print("Error in HTTP request, code: "); Serial.println(httpCode);
  }

  http.end();

  delay(5000);
}


void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi");
  } else {
    Serial.print("Connected to: "); Serial.println(ssid);
    Serial.print("IP address: "); Serial.println(WiFi.localIP());
  }
}

void Load_data() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < 20) {
    bottle_count++;
    myServo.write(90);
    delay(1000);
    myServo.write(0);
    delay(1000);

    lcd.setCursor(0, 0);
    lcd.print("Bottles: ");
    lcd.print(bottle_count);
    lcd.print("    ");

    delay(2000);
  }

  if (isnan(distance) || isnan(bottle_count)) {
    Serial.println("Failed to read value from sensor!!!");
    distance = 0;
    bottle_count = 0;
  }

  delay(500);
}
