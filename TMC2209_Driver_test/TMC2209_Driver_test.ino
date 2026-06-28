#include <TMCStepper.h>
#include <HardwareSerial.h>

#define EN_PIN_1           13      // Enable - AZ (Purple)
#define STEP_PIN_1         12      // Step - AZ (Orange)
#define DIR_PIN_1          14      // Direction - AZ (White)

#define EN_PIN_2           27      // Enable - ALT (Purple)
#define STEP_PIN_2         33      // Step - ALT (Orange)
#define DIR_PIN_2          32      // Direction - ALT (White)

#define RXD1 26  // UART RX pin for TMC2209
#define TXD1 25  // UART TX pin for TMC2209

#define AZdriver_ADDRESS 0b00  // Address for first TMC2209
#define ALTdriver_ADDRESS 0b01  // Address for second TMC2209
#define R_SENSE 0.11f           // Sense resistor value

#define AZ_msteps 8
#define ALT_msteps 8 
#define AZ_current 1500
#define ALT_current 1500

// Stepper Specifications
#define MOTOR_STEPS 200     // 1.8-degree stepper -> 200 steps per revolution
#define GEAR_RATIO 1       // Gear ratio of 1:18
#define MICROSTEPS 8       // Using 1/16 microstepping

HardwareSerial TMCSerial(1);  // Use Serial1 for TMC2209 (remapped)

TMC2209Stepper AZdriver(&TMCSerial, R_SENSE, AZdriver_ADDRESS);   // Azimuth TMC2209
TMC2209Stepper ALTdriver(&TMCSerial, R_SENSE, ALTdriver_ADDRESS); // Altitude TMC2209

float prevAzimuth = 0;
float prevAltitude = 0;

void setup() 
{
  Serial.begin(115200);  // Debugging on USB Serial
  TMCSerial.begin(115200, SERIAL_8N1, RXD1, TXD1); // UART1 for TMC2209

  pinMode(EN_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  digitalWrite(EN_PIN_1, LOW);

  pinMode(EN_PIN_2, OUTPUT);
  pinMode(STEP_PIN_2, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);
  digitalWrite(EN_PIN_2, LOW);

  AZdriver.begin();
  ALTdriver.begin();
  AZdriver.toff(5);
  ALTdriver.toff(5);
  AZdriver.ihold(40);  // Set holding current to 20% of run current
  ALTdriver.ihold(40);

  AZdriver.rms_current(AZ_current);
  ALTdriver.rms_current(ALT_current);
  AZdriver.microsteps(AZ_msteps);
  ALTdriver.microsteps(ALT_msteps);

  AZdriver.en_spreadCycle(false);
  ALTdriver.en_spreadCycle(false);
  AZdriver.pwm_autoscale(true);
  ALTdriver.pwm_autoscale(true);
}



void loop() {

    float Altitude,Azimuth;

    // Prompt user for Azimuth
    Serial.println("Enter Azimuth value:");
    while (Serial.available() == 0) {}  // Wait for input
    Azimuth = Serial.parseFloat();
    Serial.print("Received AZ: ");
    Serial.println(Azimuth);

    // Prompt user for Altitude
    Serial.println("Enter Altitude value:");
    while (Serial.available() == 0) {}  // Wait for input
    Altitude = Serial.parseFloat();
    Serial.print("Received ALT: ");
    Serial.println(Altitude);

    // Move stepper motors
    moveStepper(STEP_PIN_1, DIR_PIN_1, Azimuth);  
    
    moveStepper(STEP_PIN_2, DIR_PIN_2, Altitude);
    
}


void moveStepper(int step_pin, int dir_pin, float angle) 
{
    float steps_per_degree = (MOTOR_STEPS * MICROSTEPS * GEAR_RATIO) / 360.0;
    float total_steps = (angle * steps_per_degree);

    Serial.print("Moving: ");
    Serial.print(angle);
    Serial.print(" degrees \tSteps: ");
    Serial.println(total_steps);

    digitalWrite(dir_pin, angle > 0 ? HIGH : LOW); // Set direction

    for (int i = 0; i < total_steps; i++) {
        digitalWrite(step_pin, HIGH);
        delayMicroseconds(500);
        digitalWrite(step_pin, LOW);
        delayMicroseconds(500);
    }

    Serial.println("Movement complete");
}