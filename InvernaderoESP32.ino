#define BLYNK_TEMPLATE_ID "TMPL2OjNm8JvU"
#define BLYNK_TEMPLATE_NAME "Esp32 Wheather Report"
#define BLYNK_AUTH_TOKEN "ArMAkavJssWtOCkZDQeFGS8L9pzT8_nB"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

BlynkTimer timer;

char network[] = "Estudiantes_ADK";
char password[] = "18610*1825*";

#define DHTPIN 32
#define DHTTYPE DHT11
const int bombaPin = 12;     // GPIO12 control bomba
const int ventiladorPin = 13; // GPIO13 control ventilador
const int humedadSueloPin = 34; // Sensor de humedad del suelo

DHT dht(DHTPIN, DHTTYPE);

// Variables de estado
bool controlManualBomba = false; // Indica si la bomba está controlada manualmente
unsigned long tiempoFinRiego = 0; // Para controlar el tiempo de riego automático

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, network, password);
  dht.begin();
  pinMode(bombaPin, OUTPUT);
  pinMode(ventiladorPin, OUTPUT);
  digitalWrite(bombaPin, LOW);
  digitalWrite(ventiladorPin, LOW);
  Serial.println("Iniciando sistema de monitoreo...");
  Serial.println("Variables: Humedad Suelo | Humedad Ambiente | Temperatura");
}

BLYNK_WRITE(V3) { // Control manual de la bomba desde Blynk
  int valor = param.asInt(); // 0 = apagado, 1 = encendido
  
  controlManualBomba = (valor == 1);
  
  if (controlManualBomba) {
    Serial.println("Bomba activada manualmente desde Blynk");
    digitalWrite(bombaPin, HIGH);
  } else {
    Serial.println("Bomba desactivada manualmente desde Blynk");
    digitalWrite(bombaPin, LOW);
    tiempoFinRiego = 0; // Cancelar cualquier riego automático en curso
  }
}

void controlarVentilador(bool estado) {
  digitalWrite(ventiladorPin, estado ? HIGH : LOW);
}

void activarBomba(int tiempoMs) {
  digitalWrite(bombaPin, HIGH);
  delay(tiempoMs);
  digitalWrite(bombaPin, LOW);
}

void loop() {
  Blynk.run();
  
  // Leer sensores
  float humedadAmbiente = dht.readHumidity();
  float temperatura = dht.readTemperature();
  
  int rawValue = analogRead(humedadSueloPin);
  int humedadSueloPercent = map(rawValue, 4095, 1500, 0, 100);
  humedadSueloPercent = constrain(humedadSueloPercent, 0, 100);

  // Enviar datos a Blynk
  Blynk.virtualWrite(V0, humedadAmbiente);
  Blynk.virtualWrite(V1, temperatura);
  Blynk.virtualWrite(V2, humedadSueloPercent);
  
  // Mostrar valores por serial
  Serial.print("Humedad Suelo: ");
  Serial.print(humedadSueloPercent);
  Serial.print("% | Humedad Ambiente: ");
  Serial.print(humedadAmbiente);
  Serial.print("% | Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C");
  
  // Lógica de control de bomba (solo si no está en control manual)
  if (!controlManualBomba) {
    if (humedadSueloPercent < 30) {
      activarBomba(2000); // Riego por 2 segundos
    }
  }
  
  // Control de ventilador
  if (temperatura > 28 || humedadAmbiente > 70) {
    Serial.println("Activando ventilador por alta temperatura/humedad");
    controlarVentilador(true);
  } else {
    controlarVentilador(false);
  }
  
  delay(1000); // Reducido a 1 segundo para mejor respuesta
}