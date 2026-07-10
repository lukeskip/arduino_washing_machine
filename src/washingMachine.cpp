#include "washingMachine.h"

WashingMachine::WashingMachine(int water, int detergent, int drain, int motorRight, int motorLeft, 
                               int standing, int completeCycle, int rinse, int dry, int start, int stop) {
  _pinWater = water;
  _pinDetergent = detergent;
  _pinDrain = drain;
  _pinMotorRight = motorRight;
  _pinMotorLeft = motorLeft;
  _pinStanding = standing;
  
  _pinCompleteCycle = completeCycle;
  _pinRinse = rinse;
  _pinDry = dry;
  _pinStart = start;
  _pinStop = stop;
}

void WashingMachine::begin() {
  pinMode(_pinWater, OUTPUT);
  pinMode(_pinDetergent, OUTPUT);
  pinMode(_pinDrain, OUTPUT);
  pinMode(_pinMotorRight, OUTPUT);
  pinMode(_pinMotorLeft, OUTPUT);
  pinMode(_pinStanding, OUTPUT);
  pinMode(_pinCompleteCycle, INPUT);
  pinMode(_pinRinse, INPUT);
  pinMode(_pinDry, INPUT);
  pinMode(_pinStart, INPUT);
  pinMode(_pinStop, INPUT);
}

void WashingMachine::changeState(STATE_VALUES newState) {
  _state = newState;
  _stepTime = millis();
}

bool WashingMachine::timePassed(unsigned long waitMilliseconds) {
  return (millis() - _stepTime >= waitMilliseconds);
}

void WashingMachine::stop(String message) {
  if(_state != WAITING){
    Serial.println(message);
    // Usamos _pinWater, _pinDetergent, etc.
    digitalWrite(_pinWater, LOW);
    digitalWrite(_pinDetergent, LOW);
    digitalWrite(_pinDrain, LOW);
    digitalWrite(_pinMotorRight, LOW);
    digitalWrite(_pinMotorLeft, LOW);
    _state = WAITING;
  }
}

void WashingMachine::ledBlinking(int pin, int frequency) {
  static unsigned long last_toggle = 0;
  if(millis() - last_toggle >= 1000 / frequency) {
    digitalWrite(pin, !digitalRead(pin));
    last_toggle = millis();
  }
}


void WashingMachine::runWatering() {
   Serial.println("Iniciando llenado...");
   digitalWrite(_pinWater, HIGH);
   changeState(WATERING);
}

void WashingMachine::runPouringDetergent() {
  Serial.println("Agua completada, echando jabón...");
  digitalWrite(_pinWater, LOW);
  digitalWrite(_pinDetergent, HIGH);
  changeState(POURING_DETERGENT);
}

void WashingMachine::runWashingRight() {
  Serial.println("iniciando lavado a la derecha...");
  digitalWrite(_pinDetergent, LOW);
  digitalWrite(_pinWater, LOW);
  digitalWrite(_pinMotorLeft, LOW);
  digitalWrite(_pinMotorRight, HIGH);
  changeState(WASHING_RIGHT);
}

void WashingMachine::runWashingLeft() {
  Serial.println("Lavando a la izquierda...");
  digitalWrite(_pinMotorRight, LOW);
  digitalWrite(_pinMotorLeft, HIGH);
  changeState(WASHING_LEFT);
}

void WashingMachine::runDrain() {
  Serial.println("DRENANDO...");
  digitalWrite(_pinMotorRight, LOW);
  digitalWrite(_pinMotorLeft, LOW);
  digitalWrite(_pinDrain, HIGH);
  changeState(DRAIN);
}

void WashingMachine::runFinishing() {
  stop("TERMINANDO...");
}

void WashingMachine::runWaiting() {
  ledBlinking(_pinStanding);
}

void WashingMachine::runCompleteCycle(){
  switch(_state){
    case WAITING:

      // Esperar a que se presione el botón de inicio
      if (digitalRead(_pinStart)) {
        Serial.println("COMPLETE CYCLE...");
       WashingMachine::runWatering();
      }
    break;
    case WATERING:
      if(timePassed(3000)){
        runPouringDetergent();
      }
    break;
    case POURING_DETERGENT:
      if(timePassed(2000)){
        _cycleCount = 0;
        runWashingRight();
      }
    break;
    case WASHING_RIGHT:
      if(timePassed(5000)){
        runWashingLeft();
      }
    break;
    case WASHING_LEFT:
      if(timePassed(5000)){
        Serial.println("Ciclos contados: " + String(_cycleCount));
        if (_cycleCount < 5) {
          _cycleCount++;
          runWashingRight();
        } else {
          _cycleCount = 0;
          runDrain();
        }
      }
    break;
    case DRAIN:
      if(timePassed(5000)){ // 5 segundos de drenado
        runFinishing();
      }
    break;
  }
}

void WashingMachine::runRinse (){
  switch (_state) {
    case WAITING:
      // Esperar a que se presione el botón de inicio
      if (digitalRead(_pinStart)) {
        runWatering();
        Serial.println("RINSE CYCLE...");
      }
    break;
    case WATERING:
      if(timePassed(3000)){
        _cycleCount = 0;
        runWashingRight();
      }
    break;
    case WASHING_RIGHT:
      if(timePassed(5000)){
        runWashingLeft();
      }
    break;
    case WASHING_LEFT:
      if(timePassed(5000)){
        if (_cycleCount < 5) {
          _cycleCount++;
          runWashingRight();
        } else {
          _cycleCount = 0;
          runDrain();
          changeState(DRAIN);
        }
      }
    break;
    case DRAIN:
      if(timePassed(5000)){ // 5 segundos de drenado
        runFinishing();
      }
    break;
  }
}

void WashingMachine::runDry (){
  switch (_state) {
    case WAITING:
      if (digitalRead(_pinStart)) {
        _cycleCount = 0;
        Serial.println("DRY CYCLE...");
        runWashingRight();
      }
    break;
    case WASHING_RIGHT:
      if(timePassed(5000)){
        runWashingLeft();
      }
    break;
    case WASHING_LEFT:
      if(timePassed(5000)){
        Serial.println("Ciclos contados: " + String(_cycleCount));
        if(_cycleCount < 5){
          _cycleCount++;
          runWashingRight();
        } else {
          _cycleCount = 0;
          runDrain();
        }
      }
    break;
    case DRAIN:
      if(timePassed(5000)){ // 5 segundos de drenado
        runFinishing();
      }
    break;
  }
}

void WashingMachine::update() {
  bool stopValue = digitalRead(_pinStop);
  if(!stopValue){
    if(_state == WAITING){
      runWaiting();
      
      bool rinseValue = digitalRead(_pinRinse);
      bool dryValue = digitalRead(_pinDry);
      bool completeCycleValue = digitalRead(_pinCompleteCycle);
      // Leemos el dipswitch para seleccionar el ciclo
      if(completeCycleValue) { 
        _cycle = COMPLETE; 
      } else if(rinseValue) { 
        _cycle = RINSE; 
      } else if(dryValue) { 
        _cycle = DRY; 
      } else {
        _cycle = NONE;
      }
    }
  
    // Ejecutamos el ciclo seleccionado
    switch (_cycle) {
      case RINSE:
        runRinse();
        break;
      case DRY:
        runDry();
        break;
      case COMPLETE:
        runCompleteCycle();
        break;
      case NONE:
        break;
    }
  } else {
    stop();
  }
}