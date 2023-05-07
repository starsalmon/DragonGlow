#include <Arduino.h>

//####################### Constants ########################
const int LED0_PIN = 0;  // PWM pin for LED 0
const int LED1_PIN = 1;  // PWM pin for LED 1
const int LED2_PIN = 4;  // PWM pin for LED 2
const int LDR_PIN = A1;   // Analog pin for LDR
const int LIGHT_THRESHOLD = 40;  // Threshold for light detection
#define MAX_ON_DURATION 7200000   // 2 hours in milliseconds
#define MAX_OFF_DURATION  7200000   // 2 hours in milliseconds
#define MAX_BRIGHTNESS 255 // Maximium brightness for PWM
const int num_readings = 5; // number of readings to average For ldr smoothing

//######################### Variables ######################
int readings[num_readings];  // array to store readings for ldr smoothing
int ldr_index = 0;           // index of current reading
unsigned long lastLightTime = 0;
unsigned long lastWaitTime = 0;
unsigned long lightDuration = 0;


//#################################################### Effects ############################################
//Breathing
void breathingEffect(int duration) {
  const int numBreaths = random(1, 5); // random number of breaths
  const int numSteps = 255; // number of steps in the breathing pattern
  const int stepDelay = duration / (numBreaths * numSteps); // delay between steps
  
  for (int b = 0; b < numBreaths; b++) { // loop through each breath
    for (int i = 0; i < numSteps; i++) {
      float value = sin(i * 2 * PI / numSteps); // calculate value using sin function
      analogWrite(LED0_PIN, (int)(value * MAX_BRIGHTNESS)); // set LED0 brightness
      analogWrite(LED1_PIN, (int)((value-0.25) * MAX_BRIGHTNESS)); // set LED1 brightness
      analogWrite(LED2_PIN, (int)((value-0.5) * MAX_BRIGHTNESS)); // set LED2 brightness
      delay(stepDelay); // wait for next step
    }
  }
}

//Flickering
void flickeringEffect(unsigned long duration) {
  unsigned long startTime = millis(); // get the current time

  while (millis() - startTime < ((unsigned long)duration)) { // loop for the specified duration
    analogWrite(LED0_PIN, random(0, 128));
    analogWrite(LED1_PIN, random(0, 128));
    analogWrite(LED2_PIN, random(0, 128));
    // Wait a short amount of time to smooth out the effect
    delay(random(5, 50));
  }   
}

//Strobing
void strobingEffect(unsigned long duration) {
  const int strobeDuration = 100; // the duration of each strobe in milliseconds
  const int strobeCount = random(1, 10); // the number of strobes to perform
  
  unsigned long startTime = millis(); // get the current time
  while (millis() - startTime < duration) { // loop for the specified duration
    for (int i = 0; i < strobeCount; i++) {
      digitalWrite(LED0_PIN, HIGH);
      delay(strobeDuration);
      digitalWrite(LED0_PIN, LOW);

      digitalWrite(LED1_PIN, HIGH);
      delay(strobeDuration);
      digitalWrite(LED1_PIN, LOW);
      
      digitalWrite(LED2_PIN, HIGH);
      delay(strobeDuration);
      digitalWrite(LED2_PIN, LOW);
    }
  }
}

void pulsingEffect(unsigned long duration) {
  const int pulseDuration = random(200, 1000); // the duration of each pulse in milliseconds
  const int pulseCount = random(1, 3); // the number of pulses to perform
  
  unsigned long startTime = millis(); // get the current time
  while (millis() - startTime < duration) { // loop for the specified duration
    for (int i = 0; i < pulseCount; i++) {
      // Generate random pulse rates for each LED
      int pulseRate0 = random(1, 10); // pulse rate for LED0, in pulses per second
      int pulseRate1 = random(1, 10); // pulse rate for LED1, in pulses per second
      int pulseRate2 = random(1, 10); // pulse rate for LED2, in pulses per second
      
      // Calculate the duration of each pulse for each LED
      int pulseDuration0 = pulseDuration / pulseRate0;
      int pulseDuration1 = pulseDuration / pulseRate1;
      int pulseDuration2 = pulseDuration / pulseRate2;
      
      // Pulse all three LEDs at their respective rates
      for (int j = 0; j < max(pulseRate0, max(pulseRate1, pulseRate2)) * pulseDuration / 1000; j++) {
        if (j % pulseRate0 == 0) {
          // Pulse LED0
          for (int brightness = 0; brightness <= 255; brightness++) {
            analogWrite(LED0_PIN, brightness);
            delay(pulseDuration0 / 255);
          }
          for (int brightness = 255; brightness >= 0; brightness--) {
            analogWrite(LED0_PIN, brightness);
            delay(pulseDuration0 / 255);
          }
        }
        if (j % pulseRate1 == 0) {
          // Pulse LED1
          for (int brightness = 0; brightness <= 255; brightness++) {
            analogWrite(LED1_PIN, brightness);
            delay(pulseDuration1 / 255);
          }
          for (int brightness = 255; brightness >= 0; brightness--) {
            analogWrite(LED1_PIN, brightness);
            delay(pulseDuration1 / 255);
          }
        }
        if (j % pulseRate2 == 0) {
          // Pulse LED2
          for (int brightness = 0; brightness <= 255; brightness++) {
            analogWrite(LED2_PIN, brightness);
            delay(pulseDuration2 / 255);
          }
          for (int brightness = 255; brightness >= 0; brightness--) {
            analogWrite(LED2_PIN, brightness);
            delay(pulseDuration2 / 255);
          }
        }
      }
    }
  }
}

void test(unsigned long duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    // Calculate the PWM values for this point in the effect
    float progress = (float)(millis() - startTime) / duration;
    int pwm0, pwm1, pwm2;
    
    pwm0 = (int)(127.5 * (1 + sin(2 * PI * (progress - 0.5))) - 0.5);
    pwm1 = (int)(127.5 * (1 + sin(2 * PI * progress)) + 0.5);
    pwm2 = (int)(127.5 * (1 + sin(2 * PI * (progress + 0.5))) + 0.5);

    // Set the LED PWM values
    analogWrite(LED0_PIN, pwm0);
    analogWrite(LED1_PIN, pwm1);
    analogWrite(LED2_PIN, pwm2);

    // Wait a short amount of time to smooth out the effect
    delay(random(5, 20));
  }
}


//######################### Functions #######################
// All lights off
void lights_out(){
    // Lights out
    analogWrite(LED0_PIN, 0);
    analogWrite(LED1_PIN, 0);
    analogWrite(LED2_PIN, 0);
}

// Light detection - moving average
bool isDark() { 
  // read the raw value from the ldr
  int raw_value = analogRead(LDR_PIN);
  
  // add the current reading to the readings array
  readings[ldr_index] = raw_value;
  
  // increment the index and wrap around if necessary
  ldr_index = (ldr_index + 1) % num_readings;
  
  // calculate the average of the last num_readings values
  int smoothed_value = 0;
  for (int i = 0; i < num_readings; i++) {
    smoothed_value += readings[i];
  }
  smoothed_value /= num_readings;
  
  // Determine if it's dark based on the threshold
  return smoothed_value < LIGHT_THRESHOLD;
}

// Choose a new effect and duration then show it
void show_lights() {
  // Choose a random effect
  int effect = random(0,4);
  
  // Choose a random duration
  int duration = random(5000, 30000);
  
  // Activate the effect for the duration
  switch (effect) {
    case 0:
      breathingEffect(duration);
      break;
    case 1:
      flickeringEffect(duration);
      break;
    case 2:
      strobingEffect(duration);
      break;
    case 3:
      pulsingEffect(duration);
      break;
  }
}


//############################ Setup ###########################
void setup() {
  // Initialize PWM pins for LEDs
  pinMode(LED0_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  // Initialize LDR pin
  pinMode(LDR_PIN, INPUT);

  // Needs more rand
  randomSeed(LDR_PIN);
}

//########################### Main loop #########################
void loop() {
  
  //test(5000);
  
 
  if (isDark()) {
    if (lightDuration < MAX_ON_DURATION) {
      show_lights(); // picks a new effect and duration then shows it
      lightDuration += (millis() - lastLightTime);
      lastLightTime = millis();
    }
    lights_out();
  } else {
    lights_out();
    if (lightDuration >= MAX_ON_DURATION || millis() - lastWaitTime > MAX_OFF_DURATION) {
      lastLightTime = 0;
      lightDuration = 0;
    }
    if (millis() - lastWaitTime > MAX_OFF_DURATION) {
      lights_out();

      delay(1000); // Delay for a short time before repeating
      lastWaitTime = millis();
    }
  }
}
