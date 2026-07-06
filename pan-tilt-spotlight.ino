#include "Spotlight.h"

Spotlight spotlight;

void setup() {
  Serial.begin(115200);
  
  spotlight.begin();
}

void loop() {
  // 1. Keep the engine running (this sweeps the pan for us)
  spotlight.update();
}