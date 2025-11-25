#include "thingProperties.h"
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <DHTesp.h>

int pinDHT = 15;
int pinAnemometro = 33;
int pinLluvia = 34;

DHTesp dht;           //sensor temperatura y humedad
Adafruit_BMP280 bmp;  //sensor de presion atmosferica

TempAndHumidity dato_tyh;

void setup() {
  Serial.begin(115200);
  delay(1500); 
  // Defined in thingProperties.h
  initProperties();
  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  Wire.begin(21, 22);
  Wire.setClock(400000);

  //Declaraciones de sensores
  dht.setup(pinDHT, DHTesp::DHT11);  //inicia sensor temp-hum.

  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    Serial.println("No se detecta el sensor BMP280. Verifica conexiones!");  //verificacion de sensor de presion
    while (1);
  } 

  delay(1000);
  
}

void loop() {
  ArduinoCloud.update();
  // Your code here 
  dato_tyh = lectura_tyh();
  temperatura = leer_temperatura(dato_tyh);
  humedad = leer_humedad(dato_tyh);
  presion = leer_presion();
  leer_altitud();
  lluvia = milimetros();
  viento = velocidad_viento();
  
  delay(500); 
}

TempAndHumidity lectura_tyh() {
  TempAndHumidity lectura = dht.getTempAndHumidity();
  return lectura;
}

float leer_temperatura(TempAndHumidity lectura_inicial) {
  Serial.println("Temperatura: " + String(lectura_inicial.temperature, 2) + "°C");  //lectura de temperatura y humedad
  Serial.println("---");
  return lectura_inicial.temperature;
}

float leer_humedad(TempAndHumidity lectura_inicial) {
  Serial.println("Humedad: " + String(lectura_inicial.humidity, 2) + "%");
  Serial.println("---");
  return lectura_inicial.humidity;
}

float leer_presion() {
  float presion = bmp.readPressure() / 100.0;  // Presión en hPa
  presion = round(presion * 10) / 10.0;  // → 1 decimal

  Serial.println("Presion: " + String(presion) + "hPa");
  Serial.println("---");
  return presion;
}

float leer_altitud() {
  float altitud = bmp.readAltitude(1013.25);  // Altitud en metros

  Serial.println("Altitud: " + String(altitud) + " m");
  Serial.println("---");
  return altitud;
}

float milimetros() {

  int humedad = analogRead(pinLluvia) ;
  float milimetros=0;

  if (humedad<3500 && humedad>=2700){
    milimetros=5;
  }
  else if (humedad<2700 && humedad>=1900){
    milimetros=10;
  }
  else if (humedad<1900 && humedad>=1700){
    milimetros=20;
  }
  else if (humedad<1700 && humedad>=1600){
    milimetros=30;
  }
  else if (humedad<1600 && humedad>=1400){
    milimetros=40;
  }
  else if (humedad<1400 && humedad>=0){
    milimetros=50;
  }

  Serial.println("Cantidad de lluvia: " + String(milimetros) + " mm.");
  Serial.println("---");
  Serial.println(humedad);

  return milimetros;
}

float velocidad_viento() {
  int lectura = analogRead(pinAnemometro);   // Lectura ADC (0 - 4095)
  Serial.print("Lectura ADC: ");
  Serial.println(lectura);

  // Convertir a voltaje
  float voltaje = (lectura / 4095.0) * 3.3;

  // Conversión a km/h
  float velocidad = (voltaje * 60) / 3.3;

  Serial.print("Voltaje: "); 
  Serial.println(voltaje, 2);

  Serial.print("  Velocidad: ");
  Serial.print(velocidad, 1);
  Serial.println(" km/h");

  Serial.println("---");

  return velocidad;
}