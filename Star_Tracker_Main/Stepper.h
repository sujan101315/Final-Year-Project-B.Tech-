#ifndef STEPPER_H
#define STEPPER_H

#include <TMCStepper.h>
#include <HardwareSerial.h>
#include <DFRobot_QMC5883.h>

#define numReadings 10 
#define mag_samples 50

#define EN_PIN_1           13      // Enable - AZ 
#define STEP_PIN_1         12      // Step - AZ 
#define DIR_PIN_1          14      // Direction - 

#define EN_PIN_2           27      // Enable - ALT 
#define STEP_PIN_2         33      // Step - ALT 
#define DIR_PIN_2          32      // Direction - ALT 

#define RXD1 26  // UART RX pin for TMC2209
#define TXD1 25  // UART TX pin for TMC2209

#define AZdriver_ADDRESS 0b00  // Address for first TMC2209
#define ALTdriver_ADDRESS 0b01  // Address for second TMC2209
#define R_SENSE 0.11f           // Sense resistor value

#define AZ_current 1600
#define ALT_current 1600

// Stepper Specifications
#define MOTOR_STEPS 200     // 1.8-degree stepper -> 200 steps per revolution
#define GEAR_RATIO 9       // Gear ratio of 1:9

#define Stepper_speed 10000  // In microseconds   

DFRobot_QMC5883 compass(&Wire, 0x0D);

sVector_t readings[numReadings]; // Array to store readings

long readIndex = 0; // Index for the current reading
sVector_t total = {0, 0, 0}; // Running total of readings
sVector_t average = {0, 0, 0}; // Averaged reading

bool AzMotor_Status = false;
bool AltMotor_Status = false;
bool motorControl=true;
uint8_t microsteps = 4;       // Using 1/4 microstepping
float declination = 1.2, azimuth = 0;
float getAzimuth();


HardwareSerial TMCSerial(1);  // Use Serial1 for TMC2209 (remapped)

TMC2209Stepper AZdriver(&TMCSerial, R_SENSE, AZdriver_ADDRESS);   // Azimuth TMC2209
TMC2209Stepper ALTdriver(&TMCSerial, R_SENSE, ALTdriver_ADDRESS); // Altitude TMC2209


void Stepper_init() 
{
  TMCSerial.begin(115200, SERIAL_8N1, RXD1, TXD1); // UART1 for TMC2209

  pinMode(EN_PIN_1, OUTPUT);    
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  digitalWrite(EN_PIN_1, LOW);
  AzMotor_Status = true;
  

  pinMode(EN_PIN_2, OUTPUT);
  pinMode(STEP_PIN_2, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);
  digitalWrite(EN_PIN_2, LOW);
  AltMotor_Status = true;
  motorControl=true;

  AZdriver.begin();
  delay(100);
  ALTdriver.begin();
  delay(100);
  AZdriver.toff(5);
  ALTdriver.toff(5);
  AZdriver.ihold(40);  // Set holding current to 40% of run current
  ALTdriver.ihold(60);

  AZdriver.rms_current(AZ_current);
  ALTdriver.rms_current(ALT_current);
  AZdriver.microsteps(microsteps);
  ALTdriver.microsteps(microsteps);

  AZdriver.en_spreadCycle(false);
  ALTdriver.en_spreadCycle(false);
  AZdriver.pwm_autoscale(true);
  ALTdriver.pwm_autoscale(true);
}

void Stepper_run(uint8_t DoF, float angle)       //DoF for Az: 1 , Alt: 2
{
  Stepper_init();
  
  int step_pin, dir_pin;

  if (DoF==1)
  {
    step_pin = STEP_PIN_1;
    dir_pin = DIR_PIN_1;
  }
  else
  {
    step_pin = STEP_PIN_2;
    dir_pin = DIR_PIN_2;
  }

  digitalWrite(dir_pin, ( angle > 0 ? 1 : 0 ) );           // Set direction

  float steps_per_degree = (MOTOR_STEPS * microsteps * GEAR_RATIO) / 360.0;
  uint64_t total_steps = abs(angle * steps_per_degree);

  Serial.print("\n Microsteps: ");
  Serial.print(microsteps);
  Serial.print("\t Steps per deg: ");
  Serial.println(steps_per_degree);
  
  
  delay(2000);

  Serial.print("\nMoving: ");
  Serial.print(angle);
  Serial.print(" degrees \tSteps: ");
  Serial.println(total_steps);

  for (int i = 0; i < total_steps; i++) 
  {
    digitalWrite(step_pin, HIGH);
    delayMicroseconds(Stepper_speed);
    digitalWrite(step_pin, LOW);
    delayMicroseconds(Stepper_speed);
    delay(20);
  }
  Serial.println("Movement complete");
}

float getAzimuth() 
{
  float xAxis = 0, yAxis = 0, zAxis = 0;

  // Collect multiple samples and average them
  for (int i = 0; i < mag_samples; i++) {
    sVector_t mag = compass.readRaw();  // Read raw values

    xAxis += mag.XAxis;
    yAxis += mag.YAxis;
    zAxis += mag.ZAxis;

    delay(20);  // Small delay between samples (adjust as needed)
  }

  // Calculate the average of the raw readings
  xAxis /= mag_samples;
  yAxis /= mag_samples;
  zAxis /= mag_samples;

  // Compute heading (atan2 returns -180 to 180 degrees, convert to 0-360)
  float heading = atan2(yAxis, xAxis) * (180.0 / PI);

  // Adjust for magnetic declination
  heading += declination;

  // Normalize heading to 0-360 degrees
  if (heading < 0) {
    heading += 360;
  } else if (heading >= 360) {
    heading -= 360;
  }

  // Debugging output (optional)
  Serial.print("X: "); Serial.print(xAxis);
  Serial.print(" Y: "); Serial.print(yAxis);
  Serial.print(" Z: "); Serial.print(zAxis);
  Serial.print(" Degrees: "); Serial.println(heading);

  // Return the calculated heading
  return heading;
}


#endif