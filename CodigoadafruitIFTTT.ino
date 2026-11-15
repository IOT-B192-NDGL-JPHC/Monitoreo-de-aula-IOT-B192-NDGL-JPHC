#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ====== CREDENCIALES ======
const char* ssid     = "Galaxial";          // Cambia si es necesario
const char* password = "shadow92710";

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME  "Juanpablo44Sha"
#define AIO_KEY       "aio_fOzu358NUeQmacl1Ha7ciDE8VcRF"

// =====================================================
//                     PINES
// =====================================================

#define DHTPIN 4
#define DHTTYPE DHT11

#define LDR_PIN 34

#define LED_ROJO 5
#define LED_VERDE 18

#define BUZZER 19

// =====================================================
//                  OBJETOS
// =====================================================

DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

// =====================================================
//              CONFIGURACIÓN DE TIEMPO
// =====================================================

// Semana 13: publicar cada 10 segundos
unsigned long ultimoEnvio = 0;
const unsigned long INTERVALO = 10000;

// Tiempo para revisar WiFi
unsigned long ultimoIntentoWiFi = 0;
const unsigned long INTERVALO_WIFI = 10000;

// =====================================================
//                 CALLBACK MQTT
// =====================================================

void callback(char* topic, byte* payload, unsigned int length) {

  String mensaje = "";

  for (int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  Serial.println();
  Serial.println("===== MENSAJE MQTT RECIBIDO =====");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Mensaje: ");
  Serial.println(mensaje);

  // ---------------------------------------------------
  // CONTROL LED ROJO / LED VERDE
  // ---------------------------------------------------

  if (String(topic) == String(AIO_USERNAME) + "/feeds/led-rojo") {

    if (mensaje == "ON" || mensaje == "1") {

      digitalWrite(LED_ROJO, HIGH);
      digitalWrite(LED_VERDE, LOW);

      Serial.println("LED ROJO: ENCENDIDO");
      Serial.println("LED VERDE: APAGADO");

    } else {

      digitalWrite(LED_ROJO, LOW);
      digitalWrite(LED_VERDE, HIGH);

      Serial.println("LED ROJO: APAGADO");
      Serial.println("LED VERDE: ENCENDIDO");
    }
  }

  // ---------------------------------------------------
  // CONTROL BUZZER
  // ---------------------------------------------------

  if (String(topic) == String(AIO_USERNAME) + "/feeds/buzzer") {

    if (mensaje == "ON" || mensaje == "1") {

      digitalWrite(BUZZER, HIGH);

      Serial.println("BUZZER: ENCENDIDO");

    } else {

      digitalWrite(BUZZER, LOW);

      Serial.println("BUZZER: APAGADO");
    }
  }
}

// =====================================================
//                  CONECTAR WIFI
// =====================================================

void conectarWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.println();
  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  unsigned long inicio = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - inicio < 15000) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("[OK] WiFi conectado");
    Serial.print("IP del ESP32: ");
    Serial.println(WiFi.localIP());

  } else {

    Serial.println("[ERROR] No se pudo conectar al WiFi");
  }
}

// =====================================================
//                RECONEXIÓN MQTT
// =====================================================

void reconnect() {

  while (!client.connected()) {

    Serial.print("Conectando a Adafruit IO... ");

    String clientID = "ESP32_AulaUTS_";
    clientID += String((uint32_t)ESP.getEfuseMac(), HEX);

    if (client.connect(
          clientID.c_str(),
          AIO_USERNAME,
          AIO_KEY)) {

      Serial.println("OK");

      // ------------------------------------------------
      // SUSCRIPCIÓN AL FEED DEL LED
      // ------------------------------------------------

      String topicLed =
        String(AIO_USERNAME) + "/feeds/led-rojo";

      client.subscribe(topicLed.c_str());

      // ------------------------------------------------
      // SUSCRIPCIÓN AL FEED DEL BUZZER
      // ------------------------------------------------

      String topicBuzzer =
        String(AIO_USERNAME) + "/feeds/buzzer";

      client.subscribe(topicBuzzer.c_str());

      // ------------------------------------------------
      // PUBLICAR ESTADO ONLINE
      // ------------------------------------------------

      String topicEstado =
        String(AIO_USERNAME) + "/feeds/estado";

      client.publish(
        topicEstado.c_str(),
        "online"
      );

      Serial.println("Suscrito a:");
      Serial.println(topicLed);
      Serial.println(topicBuzzer);

      Serial.println("ESP32: ONLINE");

    } else {

      Serial.print("Fallo MQTT, rc=");
      Serial.println(client.state());

      Serial.println("Reintentando en 2 segundos...");

      delay(2000);
    }
  }
}

// =====================================================
//                 PUBLICAR DATOS
// =====================================================

void publicarDatos() {

  // ---------------------------------------------------
  // LEER DHT11
  // ---------------------------------------------------

  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();

  // ---------------------------------------------------
  // LEER LDR
  // ---------------------------------------------------

  int luminosidad = analogRead(LDR_PIN);

  // ---------------------------------------------------
  // COMPROBAR DHT11
  // ---------------------------------------------------

  if (isnan(temperatura) || isnan(humedad)) {

    Serial.println();
    Serial.println("[ERROR] No se pudo leer el DHT11");

    return;
  }

  // ===================================================
  // CREAR TOPICS
  // ===================================================

  String topicTemperatura =
    String(AIO_USERNAME) + "/feeds/temperatura";

  String topicHumedad =
    String(AIO_USERNAME) + "/feeds/humedad";

  String topicLuminosidad =
    String(AIO_USERNAME) + "/feeds/luminosidad";

  // ===================================================
  // CONVERTIR DATOS A TEXTO
  // ===================================================

  String valorTemperatura =
    String(temperatura, 1);

  String valorHumedad =
    String(humedad, 1);

  String valorLuminosidad =
    String(luminosidad);

  // ===================================================
  // PUBLICAR TEMPERATURA
  // ===================================================

  bool enviadoTemperatura = client.publish(
    topicTemperatura.c_str(),
    valorTemperatura.c_str()
  );

  // ===================================================
  // PUBLICAR HUMEDAD
  // ===================================================

  bool enviadoHumedad = client.publish(
    topicHumedad.c_str(),
    valorHumedad.c_str()
  );

  // ===================================================
  // PUBLICAR LUMINOSIDAD
  // ===================================================

  bool enviadoLuminosidad = client.publish(
    topicLuminosidad.c_str(),
    valorLuminosidad.c_str()
  );

  // ===================================================
  // MOSTRAR RESULTADOS EN MONITOR SERIAL
  // ===================================================

  Serial.println();
  Serial.println("=================================");
  Serial.println("       DATOS ENVIADOS A MQTT");
  Serial.println("=================================");

  Serial.print("Temperatura: ");
  Serial.print(temperatura, 1);
  Serial.println(" °C");

  Serial.print("Humedad:     ");
  Serial.print(humedad, 1);
  Serial.println(" %");

  Serial.print("Luz ADC:     ");
  Serial.println(luminosidad);

  Serial.println("---------------------------------");

  Serial.print("Temperatura publicada: ");
  Serial.println(enviadoTemperatura ? "SI" : "NO");

  Serial.print("Humedad publicada:     ");
  Serial.println(enviadoHumedad ? "SI" : "NO");

  Serial.print("Luminosidad publicada: ");
  Serial.println(enviadoLuminosidad ? "SI" : "NO");

  Serial.println("=================================");
}

// =====================================================
//                       SETUP
// =====================================================

void setup() {

  // ---------------------------------------------------
  // MONITOR SERIAL
  // ---------------------------------------------------

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("=================================");
  Serial.println("     PROYECTO IoT - SEMANA 13");
  Serial.println("=================================");

  // ---------------------------------------------------
  // CONFIGURAR PINES
  // ---------------------------------------------------

  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // ---------------------------------------------------
  // ESTADO INICIAL
  // ---------------------------------------------------

  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(BUZZER, LOW);

  // ---------------------------------------------------
  // INICIAR DHT11
  // ---------------------------------------------------

  dht.begin();

  Serial.println("DHT11 iniciado");
  Serial.println("LDR iniciado");
  Serial.println("LEDs configurados");
  Serial.println("Buzzer configurado");

  // ---------------------------------------------------
  // CONECTAR WIFI
  // ---------------------------------------------------

  conectarWiFi();

  // ---------------------------------------------------
  // CONFIGURAR MQTT
  // ---------------------------------------------------

  client.setServer(
    AIO_SERVER,
    AIO_SERVERPORT
  );

  client.setCallback(callback);

  Serial.println();
  Serial.println("Servidor MQTT configurado");
  Serial.println("Adafruit IO: io.adafruit.com");

  Serial.println();
  Serial.println("Sistema listo.");
}

// =====================================================
//                        LOOP
// =====================================================

void loop() {

  // ===================================================
  // COMPROBAR WIFI
  // ===================================================

  if (WiFi.status() != WL_CONNECTED) {

    unsigned long ahora = millis();

    if (ahora - ultimoIntentoWiFi >= INTERVALO_WIFI) {

      ultimoIntentoWiFi = ahora;

      Serial.println();
      Serial.println("[AVISO] WiFi desconectado");

      conectarWiFi();
    }

    return;
  }

  // ===================================================
  // COMPROBAR MQTT
  // ===================================================

  if (!client.connected()) {

    reconnect();
  }

  // Mantener comunicación MQTT
  client.loop();

  // ===================================================
  // PUBLICAR CADA 10 SEGUNDOS
  // ===================================================

  unsigned long ahora = millis();

  if (ahora - ultimoEnvio >= INTERVALO) {

    ultimoEnvio = ahora;

    publicarDatos();
  }
}
