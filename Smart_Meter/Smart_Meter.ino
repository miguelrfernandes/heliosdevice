#define BLYNK_TEMPLATE_ID "TMPLdrYh0k9O"
#define BLYNK_DEVICE_NAME "Helios"
#define BLYNK_AUTH_TOKEN "d8lfwPoqZQay1E0GysZpcYPCgeAxjVmo"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);
#include "EmonLib.h"

EnergyMonitor emon;

#define vCalibration 83.3
#define currCalibration 0.50
 
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Tecstorm2022";
char pass[] = "2022Tecstorm";

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}



float kWh = 0;
unsigned long lastmillis = millis();
 
void myTimerEvent()
{
  emon.calcVI(20, 2000);
  kWh = kWh + emon.apparentPower * (millis() - lastmillis) / 3600000000.0;
  yield();
  Serial.print("Vrms: ");
  Serial.print(emon.Vrms, 2);
  Serial.print("V");
 
  Serial.print("\tIrms: ");
  Serial.print(emon.Irms, 4);
  Serial.print("A");
 
  Serial.print("\tPower: ");
  Serial.print(emon.apparentPower, 4);
  Serial.print("W");
 
  Serial.print("\tkWh: ");
  Serial.print(kWh, 5);
  Serial.println("kWh");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vrms:");
  lcd.print(emon.Vrms, 2);
  lcd.print("V");
  lcd.setCursor(0, 1);
  lcd.print("Irms:");
  lcd.print(emon.Irms, 4);
  lcd.print("A");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Power:");
  lcd.print(emon.apparentPower, 4);
  lcd.print("W");
  lcd.setCursor(0, 1);
  lcd.print("kWh:");
  lcd.print(kWh, 4);
  lcd.print("W");
  delay(2000);
 
  lastmillis = millis();
 
  Blynk.virtualWrite(V0, emon.Vrms);
  Blynk.virtualWrite(V1, emon.Irms);
  Blynk.virtualWrite(V2, emon.apparentPower);
  Blynk.virtualWrite(V3, kWh);
}


 
void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  lcd.begin(16, 2);
 
  emon.voltage(35, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift
  emon.current(34, currCalibration); // Current: input pin, calibration.
 
  timer.setInterval(5000L, myTimerEvent);
  //timer.setInterval(1000L, myTimerEvent);
  lcd.setCursor(3, 0);
  lcd.print("IoT Energy");
  lcd.setCursor(5, 1);
  lcd.print("Meter");
  delay(3000);
  lcd.clear();
}


void loop()
{
  Blynk.run();
  timer.run();
}
