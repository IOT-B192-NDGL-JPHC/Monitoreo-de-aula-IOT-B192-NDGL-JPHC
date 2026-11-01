# Monitor de Aula Conectado - Semana 12

Sistema IoT que monitorea temperatura, humedad y luminosidad de un aula y permite el control remoto de actuadores (LED y buzzer) desde un dashboard en la nube usando **Adafruit IO**.

## Arquitectura

## Componentes utilizados

- ESP32
- Sensor DHT11 (temperatura y humedad)
- Sensor LDR (luminosidad)
- LED Rojo + LED Verde
- Módulo Buzzer (activo en bajo nivel)
- Resistencias 10kΩ y 220Ω

## Configuración de Adafruit IO

1. Crea una cuenta en [https://io.adafruit.com](https://io.adafruit.com)
2. Obtén tu **Username** y tu **AIO Key**
3. Crea los siguientes feeds:
   - `temperatura`
   - `humedad`
   - `luminosidad`
   - `led-rojo`
   - `buzzer`
   - `estado`
4. Crea un Dashboard e incluye:
   - Line Chart → temperatura
   - Gauge → humedad
   - Gauge → luminosidad
   - Toggle Switch → led-rojo
   - Toggle Switch → buzzer

## Cómo usar el código

1. Abre el archivo `.ino` en el Arduino IDE.
2. Cambia las siguientes líneas por tus datos reales:

```cpp
const char* ssid     = "TU_RED_WIFI";
const char* password = "TU_CONTRASEÑA";
#define AIO_USERNAME    "tu_usuario"
#define AIO_KEY         "aio_xxxxxxxxxxxx"
