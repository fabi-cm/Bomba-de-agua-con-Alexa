#pragma once
#include <Arduino.h>

class SensorHumedad {
  public:
    SensorHumedad(int pin);
    float leerHumedad();  // Devuelve porcentaje (0-100%)
  private:
    int _pin;
    const int _airValue = 4095;  // Valor en aire seco (ajústalo)
    const int _waterValue = 1800; // Valor en agua (ajústalo)
};