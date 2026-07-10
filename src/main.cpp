#include <Arduino.h>
#include "washingMachine.h"

// 1. Definimos los pines (pueden ser constantes aquí para mayor orden)
const int PIN_WATER = 13;
const int PIN_DETERGENT = 14;
const int PIN_DRAIN = 16;
const int PIN_MOTOR_RIGHT = 17;
const int PIN_MOTOR_LEFT = 18;
const int PIN_STANDING = 27;

const int PIN_COMPLETE_CYCLE = 19;
const int PIN_RINSE = 21;
const int PIN_DRY = 22;
const int PIN_START = 23;
const int PIN_STOP = 25;

// 2. CREAMOS EL OBJETO LAVADORA (Llamamos al constructor)
// Le pasamos los pines en el mismo orden que pide el WashingMachine.h
WashingMachine miLavadora(
  PIN_WATER, PIN_DETERGENT, PIN_DRAIN, PIN_MOTOR_RIGHT, PIN_MOTOR_LEFT, PIN_STANDING, 
  PIN_COMPLETE_CYCLE, PIN_RINSE, PIN_DRY, PIN_START, PIN_STOP
);

void setup() {
  Serial.begin(115200);
  miLavadora.begin();
}

void loop() {
  miLavadora.update();
  delay(10);
}
