#define BLYNK_TEMPLATE_ID "TMPL2OjNm8JvU"
#define BLYNK_TEMPLATE_NAME "Esp32 Wheather Report"
#define BLYNK_AUTH_TOKEN "ArMAkavJssWtOCkZDQeFGS8L9pzT8_nB"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

BlynkTimer timer;

char network[] = "Estudiantes_ADK";
char password[] = "18610*1825*";


#include <DHT.h>
#define DHTPIN 32
#define DHTTYPE DHT11
const int bombaPin = 12;     // GPIO12 control bomba
const int ventiladorPin = 13; // GPIO13 control ventilador
const int humedadSueloPin = 34; // Sensor de humedad del suelo (corregido nombre)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Blynk.begin(BLYNK_AUTH_TOKEN,network,password);
  Serial.begin(115200);
  dht.begin();
  pinMode(bombaPin, OUTPUT);
  pinMode(ventiladorPin, OUTPUT);
  digitalWrite(bombaPin, LOW);
  digitalWrite(ventiladorPin, LOW);
  Serial.println("Iniciando sistema de monitoreo...");
  Serial.println("Variables: Humedad Suelo | Humedad Ambiente | Temperatura");
}

void loop() {

  Blynk.run();
  // Leer sensores
  float humedadAmbiente = dht.readHumidity();
  float temperatura = dht.readTemperature();
  
  int rawValue = analogRead(humedadSueloPin);  // Lectura ADC (0-4095)
  int humedadSueloPercent = map(rawValue, 4095, 1500, 0, 100);  // Conversión a %
  humedadSueloPercent = constrain(humedadSueloPercent, 0, 100);  // Limita entre 0% y 100%

  Blynk.virtualWrite(V0,humedadAmbiente);
  Blynk.virtualWrite(V1,temperatura);
  Blynk.virtualWrite(V2,humedadSueloPercent);
  
  // Mostrar valores por serial
  Serial.print("Humedad Suelo: ");
  Serial.print(humedadSueloPercent);
  Serial.print("% | Humedad Ambiente: ");
  Serial.print(humedadAmbiente);
  Serial.print("% | Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C");
  
  // Lógica de control
  if(humedadSueloPercent < 30) { // Valor a calibrar
    Serial.println("Activando bomba por baja humedad del suelo");
    activarBomba(2000); // Riego por 2 segundos
  }
  
  if(temperatura > 28 || humedadAmbiente > 70) { // Valores de ejemplo
    Serial.println("Activando ventilador por alta temperatura/humedad");
    controlarVentilador(true);
  } else {
    controlarVentilador(false);
  }
  
  delay(5000); // Muestreo cada 5 segundos
}

void activarBomba(int tiempoMs) {
  digitalWrite(bombaPin, HIGH);
  delay(tiempoMs);
  digitalWrite(bombaPin, LOW);
}

void controlarVentilador(bool estado) {
  digitalWrite(ventiladorPin, estado ? HIGH : LOW);
}