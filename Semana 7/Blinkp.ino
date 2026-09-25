#include <DHT.h>

#define DHT_PIN 2
#define DHT_TYPE DHT11
#define LDR_PIN A0

#define TEMP_MAX 28.0
#define TEMP_MIN 15.0
#define HUM_MIN 40.0
#define HUM_MAX 80.0
#define LUZ_MIN 300          // Ajustado para Arduino (0-1023)

DHT dht(DHT_PIN, DHT_TYPE);

unsigned long ultimoMuestreo = 0;
const unsigned long INTERVALO_MUESTREO = 2000;

void setup() {
  Serial.begin(9600);

  Serial.println("==============================");
  Serial.println("  SISTEMA DE MONITOREO DE AULA");
  Serial.println("  Semana 7 - Comunicación Serial");
  Serial.println("==============================");
  Serial.println();

  dht.begin();
  Serial.println("[OK] Sensor DHT11 inicializado");
  Serial.println("[OK] Sensor LDR configurado");
  Serial.println();
  Serial.println("Iniciando lecturas...");
  Serial.println("------------------------------");
}

void loop() {
  if (millis() - ultimoMuestreo >= INTERVALO_MUESTREO) {
    ultimoMuestreo = millis();

    float temperatura = dht.readTemperature();
    float humedad = dht.readHumidity();
    int luminosidad = analogRead(LDR_PIN);

    if (isnan(temperatura) || isnan(humedad)) {
      Serial.println("[ERROR] Fallo al leer el sensor DHT11");
      Serial.println("        Verifique las conexiones");
      Serial.println("------------------------------");
      return;
    }

    Serial.println("=== LECTURA DE SENSORES ===");
    Serial.print("Temperatura: ");
    Serial.print(temperatura, 1);
    Serial.println(" °C");

    Serial.print("Humedad: ");
    Serial.print(humedad, 1);
    Serial.println(" %");

    Serial.print("Luminosidad: ");
    Serial.println(luminosidad);

    bool alerta = false;

    if (temperatura > TEMP_MAX) {
      Serial.println("[ALERTA] Temperatura ALTA");
      alerta = true;
    } else if (temperatura < TEMP_MIN) {
      Serial.println("[ALERTA] Temperatura BAJA");
      alerta = true;
    }

    if (humedad < HUM_MIN) {
      Serial.println("[ALERTA] Humedad BAJA");
      alerta = true;
    } else if (humedad > HUM_MAX) {
      Serial.println("[ALERTA] Humedad ALTA");
      alerta = true;
    }

    if (luminosidad < LUZ_MIN) {
      Serial.println("[ALERTA] Iluminación INSUFICIENTE");
      alerta = true;
    }

    if (!alerta) {
      Serial.println("[OK] Todos los valores en rango normal");
    }

    Serial.println("------------------------------");
  }
}