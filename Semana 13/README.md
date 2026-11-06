# Semana 13 - Alertas IoT con Adafruit IO + IFTTT

Sistema que monitorea la luminosidad del aula y envía una notificación push al celular cuando el valor es menor a 100.

## Flujo

Sensor LDR → ESP32 → Adafruit IO → IFTTT → Notificación Push

## Estructura del repositorio

- `codigo/esp32_luminosidad.ino` → Código del ESP32
- `evidencias/` → Capturas del dashboard, gráfico histórico, Applet IFTTT y notificación
- `README.md` → Este archivo

## Cómo usar

1. Configura tu AIO_USERNAME y AIO_KEY en el código.
2. Sube el código al ESP32.
3. Crea el Applet en IFTTT (Trigger: luminosidad < 100 → Action: notificación).
4. Cubre el LDR para probar la alerta.
