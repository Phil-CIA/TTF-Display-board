#include <Arduino.h>

// TTF Display Board — Phase 1 skeleton
// Target: ESP32-S3-WROOM-1-N16R8
// Goal: confirm boot, serial output, PSRAM detection

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("TTF Display Board — boot OK");

    // PSRAM check
    if (psramFound()) {
        Serial.printf("PSRAM: %u KB available\n", ESP.getFreePsram() / 1024);
    } else {
        Serial.println("WARNING: PSRAM not found");
    }
}

void loop() {
    // Phase 1: nothing here yet
    delay(5000);
    Serial.println("alive");
}
