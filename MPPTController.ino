#include <math.h>
#include "MPPTController.h"

/*
 * Technology Demonstration of a Maximum Power Point Transfer Controller using the "Peturb and Observe" Algorithm
 * 
 * Theory of Operation
 * 
 * We are measuring voltage and current at the battery, so we can also calculate power
 * 
 * if Power_now > Power_prev {
 *  if Volts_now > Volts_prev {
 *    increase PWM output
 *  }
 *  else {
 *    decrease PWM output
 *  }
 * else {
 *  if Volts_now > Volts_prev {
 *    decrease PWM output
 *  }
 *  else {
 *    increase PWM output
 *  }
 * }
 * 
 * PINS
 * D0  - TxD
 * D1  - RxD
 * D2  - Rotary Encoder
 * D3  - Rotart Encoder
 * D4  - Encoder Pushbutton
 * D5  - PWM Out to MOSFET
 * D6  -
 * D7  - Panel Cutoff Relay
 * D8  -
 * D9  -
 * D10 - (SS)
 * D11 - (MOSI)
 * D12 - (MISO)
 * D13 - (SCK)
 * A0  - Vin
 * A1  - Iin
 * A2  - Vout
 * A3  -
 * A4  - SDA
 * A5  - SCL
 * A6  -
 * A7  -
 */

float readVolts(void){
  return (float)analogRead(VOUT) * (float)VOLTAGE_SCALE;
}

float readAmps(void) {
  return (float)abs(analogRead(IOUT)-512) * (float)CURRENT_SCALE / 0.066;
}

float calculatePower(float vIn, float iIn) {
  float power = 0.0;
  power = (vIn * iIn);
  return power;
}

void pwmUp(void) {
  if(powerOut < 256)
    powerOut++;
}

void pwmDown(void) {
  if(powerOut > 0)
    powerOut--;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("MPPT Controller 0.0.1");
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_BUTTON, INPUT_PULLUP);
  pinMode(RELAY, OUTPUT);
  pinMode(IOUT, INPUT);
  pinMode(VOUT, INPUT);

  // Turn on the solar panel
  digitalWrite(RELAY, HIGH);
  analogWrite(DRIVE, 0);
}

void loop() {
  lastPower = currentPower;
    lastVoltage = volts;

  volts = readVolts();
  amps = readAmps();
  currentPower = calculatePower(volts, amps);
  Serial.print("Vin, Iin, Power, PWM: ");
  Serial.print(String(volts));
  Serial.print(", ");
  Serial.print(String(amps));
  Serial.print(", ");
  Serial.print(String(currentPower));
  Serial.print(", ");
  Serial.print(String(int((float)powerOut/(float)256 * 100)));
  Serial.println();
  analogWrite(DRIVE, powerOut);
  if(lastPower < currentPower) {
    if(volts > lastVoltage)
      pwmUp();
    else
      pwmDown();
  }
  if(lastPower > currentPower) {
    if(volts < lastVoltage)
      pwmUp();
    else
      pwmDown();
  }
  delay(1000);
}
