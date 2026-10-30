#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid     = "Galaxial";
const char* password = "shadow92710";

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "Juanpablo44Sha"
#define AIO_KEY         "aio_aoEq33tj2kpZydOfMVVYlkfwyt4X"

#define DHTPIN 4
#define DHTTYPE DHT11
#define LDR_PIN 34
#define LED_ROJO 5
#define LED_VERDE 18
#define BUZZER 19

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long ultimoEnvio = 0;
const unsigned long INTERVALO = 5000;

void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  if (String(topic) == String(AIO_USERNAME) + "/feeds/led-rojo") {
    if (mensaje == "ON" || mensaje == "1") {
      digitalWrite(LED_ROJO, HIGH);
      digitalWrite(LED_VERDE, LOW);
    } else {
      digitalWrite(LED_ROJO, LOW);
      digitalWrite(LED_VERDE, HIGH);
    }
  }

  if (String(topic) == String(AIO_USERNAME) + "/feeds/buzzer") {
    if (mensaje == "ON" || mensaje == "1") {
      digitalWrite(BUZZER, HIGH);
    } else {
      digitalWrite(BUZZER, LOW);
    }
  }
}

void conectarWiFi() {
  Serial.print("Conectando WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[OK] WiFi conectado");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a Adafruit IO...");
    if (client.connect("ESP32_AulaUTS", AIO_USERNAME, AIO_KEY)) {
      Serial.println("OK");
      // Suscribirse a los feeds de control
      client.subscribe((String(AIO_USERNAME) + "/feeds/led-rojo").c_str());
      client.subscribe((String(AIO_USERNAME) + "/feeds/buzzer").c_str());
      client.publish((String(AIO_USERNAME) + "/feeds/estado").c_str(), "online");
    } else {
      Serial.print("Fallo, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void publicarDatos() {
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();
  int luminosidad = analogRead(LDR_PIN);

  if (isnan(temperatura) || isnan(humedad)) {
    Serial.println("[ERROR] Fallo DHT11");
    return;
  }

  client.publish((String(AIO_USERNAME) + "/feeds/temperatura").c_str(), String(temperatura, 1).c_str());
  client.publish((String(AIO_USERNAME) + "/feeds/humedad").c_str(), String(humedad, 1).c_str());
  client.publish((String(AIO_USERNAME) + "/feeds/luminosidad").c_str(), String(luminosidad).c_str());

  Serial.println("--- Datos enviados ---");
  Serial.print("Temp: "); Serial.println(temperatura);
  Serial.print("Hum:  "); Serial.println(humedad);
  Serial.print("Luz:  "); Serial.println(luminosidad);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LED_VERDE, HIGH);

  dht.begin();
  conectarWiFi();
  client.setServer(AIO_SERVER, AIO_SERVERPORT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (millis() - ultimoEnvio >= INTERVALO) {
    ultimoEnvio = millis();
    publicarDatos();
  }
}
