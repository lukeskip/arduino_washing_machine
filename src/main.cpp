#include "esp32-hal-gpio.h"
#include "esp32-hal.h"
#include <Arduino.h>

int water = 13;
int detergent = 14;
int drain = 16;
int motor_right = 17;
int motor_left = 18;
int complete_cycle_input = 19;
int rinse_input = 21;
int dry_input = 22;
int start_input = 23;
int stop_input = 25;
unsigned long step_time = 0; 
int cycle_count = 0;

enum STATE_VALUES{
  WAITING,
  WATERING,
  POURING_DETERGENT,
  WASHING_RIGHT,
  WASHING_LEFT,
  DRAIN
};

STATE_VALUES state = WAITING;

enum CYCLE_VALUES{
  COMPLETE,
  RINSE,
  DRY
};

CYCLE_VALUES cycle = COMPLETE;

void stop(String message = "¡CANCELADO DE EMERGENCIA!"){
  if(state!= WAITING){
    Serial.println(message);
    digitalWrite(water,LOW);
    digitalWrite(detergent,LOW);
    digitalWrite(drain,LOW);
    digitalWrite(motor_right,LOW);
    digitalWrite(motor_left,LOW);
    digitalWrite(water,LOW);
    digitalWrite(water,LOW);
    state = WAITING;
  }
}

void change_state (STATE_VALUES new_state){
  state = new_state;
  step_time = millis();
}

bool time_passed(unsigned long wait_milliseconds){
  return (millis() - step_time >= wait_milliseconds);
}

void run_watering(){
   Serial.println("Iniciando llenado...");
    digitalWrite(water, HIGH);
    change_state(WATERING);
}

void run_pouring_detergent(){
  Serial.println("Agua completada, echando jabón...");
  digitalWrite(water, LOW);
  digitalWrite(detergent, HIGH);
  change_state(POURING_DETERGENT);
}

void run_washing_right(){
  Serial.println("iniciando lavado a la derecha...");
  digitalWrite(detergent, LOW);
  digitalWrite(water, LOW);
  digitalWrite(motor_left, LOW);
  digitalWrite(motor_right, HIGH);
  change_state(WASHING_RIGHT);
}

void run_washing_left(){
  Serial.println("Lavando a la izquierda...");
  digitalWrite(motor_right, LOW);
  digitalWrite(motor_left, HIGH);
  change_state(WASHING_LEFT);
}

void run_drain(){
  Serial.println("DRENANDO...");
  digitalWrite(motor_right, LOW);
  digitalWrite(motor_left, LOW);
  digitalWrite(drain, HIGH);
  change_state(DRAIN);
}


void run_finishing(){
  stop("TERMINANDO...");
}

void run_complete_cycle(){
  switch(state){
    case WAITING:
      // Esperar a que se presione el botón de inicio
      if (digitalRead(start_input)) {
        Serial.println("COMPLETE CYCLE...");
       run_watering();
      }
    break;
    case WATERING:
      if(time_passed(3000)){
        run_pouring_detergent();
      }
    break;
    case POURING_DETERGENT:
      if(time_passed(2000)){
        cycle_count = 0;
        run_washing_right();
      }
    break;
    case WASHING_RIGHT:
      if(time_passed(5000)){
        run_washing_left();
      }
    break;
    case WASHING_LEFT:
      if(time_passed(5000)){
        Serial.println("Ciclos contados: " + String(cycle_count));
        if (cycle_count < 5) {
          cycle_count++;
          run_washing_right();
        } else {
          cycle_count = 0;
          run_drain();
        }
      }
    break;
    case DRAIN:
      if(time_passed(5000)){ // 5 segundos de drenado
        run_finishing();
      }
    break;
  }
}

void run_rinse (){
  switch (state) {
    case WAITING:
      // Esperar a que se presione el botón de inicio
      if (digitalRead(start_input)) {
        run_watering();
        Serial.println("RINSE CYCLE...");
      }
    break;
    case WATERING:
      if(time_passed(3000)){
        cycle_count = 0;
        run_washing_right();
      }
    break;
    case WASHING_RIGHT:
      if(time_passed(5000)){
        run_washing_left();
      }
    break;
    case WASHING_LEFT:
      if(time_passed(5000)){
        if (cycle_count < 5) {
          cycle_count++;
          run_washing_right();
        } else {
          cycle_count = 0;
          run_drain();
          change_state(DRAIN);
        }
      }
    break;
    case DRAIN:
      if(time_passed(5000)){ // 5 segundos de drenado
        run_finishing();
      }
    break;
  }
}

void run_dry (){
  switch (state) {
    case WAITING:
      if (digitalRead(start_input)) {
        cycle_count = 0;
        Serial.println("DRY CYCLE...");
        run_washing_right();
      }
    break;
    case WASHING_RIGHT:
      if(time_passed(5000)){
        run_washing_left();
      }
    break;
    case WASHING_LEFT:
      if(time_passed(5000)){
        Serial.println("Ciclos contados: " + String(cycle_count));
        if(cycle_count < 5){
          cycle_count++;
          run_washing_right();
        } else {
          cycle_count = 0;
          run_drain();
        }
      }
    break;
    case DRAIN:
      if(time_passed(5000)){ // 5 segundos de drenado
        run_finishing();
      }
    break;
  }
}

// This function runs once upon startup
void setup() {
  // Put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(water, OUTPUT);
  pinMode(detergent, OUTPUT);
  pinMode(drain, OUTPUT);
  pinMode(motor_right, OUTPUT);
  pinMode(motor_left, OUTPUT);
  pinMode(complete_cycle_input, INPUT);
  pinMode(rinse_input, INPUT);
  pinMode(dry_input, INPUT);
  pinMode(start_input, INPUT);
  pinMode(stop_input, INPUT);
}

// This function runs repeatedly
void loop() {
  bool stop_value = digitalRead(stop_input);

  if(!stop_value){
    if(state == WAITING){
      bool rinse_value = digitalRead(rinse_input);
      bool dry_value = digitalRead(dry_input);
      bool complete_cycle_value = digitalRead(complete_cycle_input);

      // Leemos el dipswitch para seleccionar el ciclo
      if(complete_cycle_value){ 
        cycle = COMPLETE; 
      } else if(rinse_value){ 
        cycle = RINSE; 
      } else if(dry_value){ 
        cycle = DRY; 
      }
    }
  
    switch (cycle) {
      case RINSE:
        
        run_rinse();
        break;
      case DRY:
        run_dry();
        break;
      case COMPLETE:
        run_complete_cycle();
        break;
    }

  }else{
    stop();
  }
  
  delay(10); // Pequeña pausa para no saturar el procesador
}
