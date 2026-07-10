#pragma once // Esto evita que el archivo se incluya más de una vez por accidente

#include <Arduino.h>

// Nos traemos los ENUMs aquí porque son parte de la definición de la lavadora
enum STATE_VALUES {
  WAITING,
  WATERING,
  POURING_DETERGENT,
  WASHING_RIGHT,
  WASHING_LEFT,
  DRAIN
};

enum CYCLE_VALUES {
  COMPLETE,
  RINSE,
  DRY,
  NONE
};

class WashingMachine {
  private:
    // --- PINES DE SALIDA ---
    int _pinWater;
    int _pinDetergent;
    int _pinDrain;
    int _pinMotorRight;
    int _pinMotorLeft;
    int _pinStanding;

    // --- PINES DE ENTRADA ---
    int _pinCompleteCycle;
    int _pinRinse;
    int _pinDry;
    int _pinStart;
    int _pinStop;

    // --- VARIABLES DE CONTROL ---
    unsigned long _stepTime = 0; 
    int _cycleCount = 0;
    STATE_VALUES _state = WAITING;
    CYCLE_VALUES _cycle = COMPLETE;

    // Métodos privados internos (los que hacían el trabajo sucio en main)
    void changeState(STATE_VALUES newState);
    bool timePassed(unsigned long waitMilliseconds);
    void stop(String message = "¡CANCELADO DE EMERGENCIA!");
    void ledBlinking(int pin, int frequency = 2);
    
    // Acciones de la lavadora
    void runWatering();
    void runPouringDetergent();
    void runWashingRight();
    void runWashingLeft();
    void runDrain();
    void runFinishing();
    void runWaiting();
    
    // Ciclos completos
    void runCompleteCycle();
    void runRinse();
    void runDry();

  public:
    // Constructor: Aquí pedimos todos los pines que necesita la lavadora para existir
    WashingMachine(int water, int detergent, int drain, int motorRight, int motorLeft, 
                   int standing, int completeCycle, int rinse, int dry, int start, int stop);

    // Inicializa los pines (equivale al setup() del main original)
    void begin();

    // Actualiza el estado (equivale al loop() del main original)
    void update();
    
}; 

