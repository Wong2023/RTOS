#define RGB_BUILTIN 38 
#define RGB_BRIGHTNESS 50 
 
void setup() { 
  // No separate initialisation is required for this example. 
} 
 
void loop() { 
  // Red ON 
  neopixelWrite(RGB_BUILTIN, 20, 0, 0); 
  delay(500); 
 // Green ON
  neopixelWrite(RGB_BUILTIN, 0, 255, 0); 
  delay(500);
 //Blue ON
  neopixelWrite(RGB_BUILTIN, 0, 0, 30); 
  delay(500);
 //white
  neopixelWrite(RGB_BUILTIN, 16, 15, 17); 
  delay(500);
 //Rose
  neopixelWrite(RGB_BUILTIN, 60, 20, 0); 
  delay(500);
  // LED OFF 
  neopixelWrite(RGB_BUILTIN, 0, 0, 0); 
  delay(500); 
} 
