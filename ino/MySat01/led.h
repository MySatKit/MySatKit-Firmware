#pragma once

void blinkLEDs()
{
    // Blink leds
    digitalWrite(16, HIGH); 
    digitalWrite(0, HIGH); 
    delay(1000); 
    digitalWrite(16, LOW); 
    digitalWrite(0, LOW); 
    delay(1000); 
}