#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial gpsSerial(4, 3);   
SoftwareSerial gsm(7, 8);       

TinyGPSPlus gps;
MPU6050 mpu;

#define VIBRATION_PIN 2

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  gsm.begin(9600);

  pinMode(VIBRATION_PIN, INPUT);

  Wire.begin();
  mpu.initialize();

  Serial.println("System Ready");
}

void loop() {

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  Serial.print("AccelX: "); Serial.print(ax);
  Serial.print(" Y: "); Serial.print(ay);
  Serial.print(" Z: "); Serial.println(az);

  if (digitalRead(VIBRATION_PIN) == HIGH) {
    Serial.println("Vibration Detected");

    if (gps.location.isValid()) {
      float latitude = gps.location.lat();
      float longitude = gps.location.lng();

      sendSMS(latitude, longitude);
    } else {
      Serial.println("GPS not valid");
    }

    delay(5000); 
  }

  delay(1000);
}

void sendSMS(float lat, float lon) {

  gsm.println("AT");
  delay(1000);

  gsm.println("AT+CMGF=1"); 
  delay(1000);

  gsm.println("AT+CMGS=\"+91xxxxxxxxxx\"");  
  delay(1000);

  gsm.println("Alert! Vibration detected.");
  gsm.print("Location: ");
  gsm.print("Lat: "); gsm.print(lat, 6);
  gsm.print(" Lon: "); gsm.println(lon, 6);

  gsm.print("https://maps.google.com/?q=");
  gsm.print(lat, 6);
  gsm.print(",");
  gsm.println(lon, 6);

  gsm.write(26);  
  delay(5000);

  Serial.println("SMS Sent");
}
