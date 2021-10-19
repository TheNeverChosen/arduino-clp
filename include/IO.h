#pragma once

enum IOType{INVALID, INPUT_DG, INPUT_AL, OUTPUT_DG};

class Device{
public:
  uint8_t pin;
  enum IOType type;

  Device() : pin(0), type(INVALID){}
  Device(uint8_t pin, IOType type) : pin(pin), type(type){
    if(INPUT_DG) pinMode(pin, INPUT);
    else if(OUTPUT_DG) pinMode(pin, OUTPUT);
  }
};

class InputDG:public Device{
public:
  InputDG():Device(){}
  InputDG(uint8_t pin):Device(pin, INPUT_DG){}
};

class InputAL:public Device{
public:
  uint16_t divider, curValue;
  bool belowDiv;
  InputAL():Device(), divider(0), curValue(0), belowDiv(0){}
  InputAL(uint8_t pin, uint16_t divider, bool belowDiv):
    Device(pin, INPUT_AL), divider(divider), belowDiv(belowDiv), curValue(0){}
};

class OutputDG:public Device{
public:
  bool state;
  OutputDG():Device(),state(LOW){}
  OutputDG(uint8_t pin):Device(pin, type), state(LOW){
    digitalWrite(pin, LOW);
  }
  OutputDG(uint8_t pin, bool state):Device(pin, OUTPUT_DG), state(state){
    digitalWrite(pin, state);
  }
};

//0 A 3 347 1 --(variavelLadder tipo[Analogica] pinArduino divisor inicio[true/false])
class LadderVar{
public:
  uint32_t number;
  Device *device;

  LadderVar():number((uint32_t)-1), device(NULL){}
  LadderVar(uint32_t number):number(number), device(NULL){} //bobina interna
  LadderVar(uint32_t number, IOType type, uint8_t pin, uint16_t divider, bool belowDiv):  
    number(number){ //InputAL
    device = static_cast<Device*> (new InputAL(pin, divider, belowDiv));
  }
  LadderVar(uint32_t number, IOType type, uint8_t pin, uint16_t divider, bool belowDiv):
    number(number){ //Input
    device = static_cast<Device*> (new InputAL(pin, divider, belowDiv));
  }

     //construtor variavel input analogico
    
};