/*
   Interface to relay hardware.
   
   This is a RelayInterface base class that is not intended
   for use but provides the interface functions.  To create
   a usable relay interface, inherit from RelayInterface and
   define kRelayPin.  The functions can then be used via
   calls to static methods.
*/

#include "RelayInterface.hpp"

#include <Arduino.h>



void RelayInterface::initialize() {
  pinMode(kRelayPin, OUTPUT);
};
void RelayInterface::turnOn() {
  digitalWrite(kRelayPin, HIGH);
}
void RelayInterface::turnOff() {
  digitalWrite(kRelayPin, LOW);
}
