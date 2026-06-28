#include "WiFi_RTC.h"
#include "html_content.h" 
#include <math.h>
#include <TinyGPS++.h>
#include <ArduinoJson.h>
#include <SkyMap.h>
#include <EEPROM.h>
#include <MPU9250_WE.h>

#define GPS_RX 16  // Define correct GPS RX pin
#define GPS_TX 17  // Define correct GPS TX pin

#define WHO_AM_I_REG 0x75
#define MPU9250_ADDR 0x69



MPU9250_WE mpu = MPU9250_WE(MPU9250_ADDR);

uint16_t observationTime=1;

uint8_t readRegister(uint8_t deviceAddress, uint8_t regAddress);

TinyGPSPlus gps;  // Declaring TinyGPSPlus object

void getGPSData();
void init_GPS();
void handleHoming();
void handleRefresh();
void handleOffsetMPU();
void handleSelectObject();
void handleDataRequest();
void handleSetDeclination();
void handleSetLocation();
void handleStopTracking();
void handleStartTracking();
void handleUpdateTime();
void handleMotorControl();
void handleObservationTime();
void handleSetMicrosteps();
void handleSetAzPos();

void getMagneticDeclinationOnline();
void getMPU() ;
void calAzAlt();
void extractRA(const char* raString, int* hours, int* minutes, int* seconds);
void extRactDec(const char* decString, int& degrees, int& arcminutes, int& arcseconds);

bool manualAz =false;
bool magFound=false;
bool mpuFound= false;
bool errorStatus=false;
bool homing=false;
bool tracking=false;
bool objectStatus = false;
bool gpsStatus = false;
bool declinationStatus = false;

float pitch =0, roll=0;           // Pitch is using as tilt, roll is used to deteremine or adjust the level
float c_Alt=0.0, c_Az=0.0;
long lastMillis;
uint8_t countMillies=0;
uint16_t countObservationTime=0;

String error="NA";

void setup() 
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX); // Initialize Serial2 for GPS

  delay(1000);
  Wire.begin();
  Stepper_init();
  delay(500);

  if (!rtc.begin()) 
  {
    Serial.println("Couldn't find RTC");
    timeFound = false;
  }
    else
    {
      Serial.println("Found valid RTC");
      timeFound = true;
    }  

  // Find and initialize MPU9250
  uint8_t whoAmI = readRegister(MPU9250_ADDR, WHO_AM_I_REG);
  Serial.print("MPU9250 WHO_AM_I register: 0x");
  Serial.println(whoAmI, HEX);
  
  if (whoAmI == 0x71 || whoAmI == 0x73 || whoAmI == 0x70) 
  {
    Serial.println("MPU9250 is responding correctly.");
   
    mpu.setSampleRateDivider(5);
    mpu.setAccRange(MPU9250_ACC_RANGE_2G);
    mpu.enableAccDLPF(true);
    mpu.setAccDLPF(MPU9250_DLPF_6);
    mpuFound= true;
  } 
  else 
  {
    Serial.println("MPU9250 did not return the expected value.");
    mpuFound= false;
  }

  delay(1000);

  // Initialize QMC5883 Compass
    
  if(!compass.begin()) 
  {
      Serial.println("Could not find a valid QMC5883 sensor, check wiring!");
      magFound=false;
      delay(500);
  }
  else
  {
    Serial.println("Found a valid QMC5883 sensor.");
    compass.setRange(QMC5883_RANGE_2GA);
    compass.setMeasurementMode(QMC5883_CONTINOUS);
    compass.setDataRate(QMC5883_DATARATE_10HZ);
    compass.setSamples(QMC5883_SAMPLES_8);
    magFound= true;
  }  
  delay(1000);
  
  // Connect WiFi and update RTC
  connectWiFi();

  update_time();
    
  Serial.println("Initializing GPS...");
  delay(5000);
  getGPSData();

  getMagneticDeclinationOnline();    

  // Setup Web Server Endpoints
    
  server.on("/", HTTP_GET, []() 
  {
    Serial.println("Serving HTML content...");
    server.send_P(200, "text/html", html_content);
  });
  server.on("/refresh", handleRefresh);
  server.on("/start", handleStartTracking);
  server.on("/stop", handleStopTracking);
  server.on("/homing", handleHoming);
  server.on("/set_azpos", handleSetAzPos);
  server.on("/update_time", handleUpdateTime);
  server.on("/offsetMPU", handleOffsetMPU);
  server.on("/data", HTTP_GET, handleDataRequest);
  server.on("/set_declination", HTTP_POST, handleSetDeclination);
  server.on("/set_location", HTTP_POST, handleSetLocation);
  server.on("/select_object", HTTP_POST, handleSelectObject);
  server.on("/motor_control", HTTP_POST, handleMotorControl);
  server.on("/set_observationTime", HTTP_POST, handleObservationTime);
  server.on("/set_microsteps", HTTP_POST, handleSetMicrosteps);
   
    
  server.begin();

  lastMillis= millis();

 // Serial.println("Setup Finished!\n");
}


void loop() 
{  
 // Serial.println("Checkpoint-0\n");
  DateTime time_now = rtc.now();
  Time = formatTime(time_now);
  
  getMPU();
  azimuth=getAzimuth(); 

  server.handleClient(); // Handle web requests   
  
  if(countMillies==6)  
  {
    int lat= (int)(latitude * 100);
    int lng = (int)(longitude * 100);

    if(gpsStatus==false) getGPSData();

    if (lat != (int)(latitude * 100) || lng!=(int)(longitude * 100) || declinationStatus == false)
      getMagneticDeclinationOnline();

    countObservationTime++;
    countMillies=0;
  }

  server.handleClient(); // Handle web requests 

  if(countObservationTime==observationTime)
  {
    if(mpuFound)
    {      
      xyzFloat angles = mpu.getAngles();
      temperature = mpu.getTemperature();   
    } 
    
    if(homing&&tracking&&motorControl)
    { 
      //azimuth=getAzimuth();
      if(AzMotor_Status&&API_status&&((Az-preAz)!=0))
      {  
        Stepper_run(1 ,(Az-preAz));

        preAz += (Az-preAz);
        azimuth = getAzimuth();

        if(int(abs(azimuth-Az))>10)       
        {
          error="Azimuth motor didn't run correctly, check motor connections and power!";  
          Serial.println(error);         
        }  
        else
        {
          error="NA";
        }
      }

      server.handleClient(); // Handle web requests   

      if(mpuFound)
      {
        xyzFloat angles = mpu.getAngles();
        pitch = mpu.getPitch();
      }

      if(AltMotor_Status&&API_status&&((Alt-pitch)!=0)&&(0<Alt<90))
      {
        Stepper_run(2, (Alt-pitch));

        preAlt += (Alt-pitch);
        
        if(int(abs(pitch-Alt))>3)       
        {
          error="Altitude motor didn't run, check motor connections and power!";   
          Serial.println(error);        
        } 
        else 
        {
          error="NA";
        }  
      } 
    }
    countObservationTime=0;
  }

  server.handleClient(); // Handle web requests   

  if(millis()-lastMillis>=10000)
  {
    if (WiFi_status)
      fetchAPI(objectName);        

    countMillies++;
    Serial.print("Count every 10 second: ");
    Serial.println(countMillies);
    lastMillis= millis();
  }    

  server.handleClient(); // Handle web requests   

}

void handleSetAzPos()
{
  preAz=180;
  manualAz = true;
  Serial.println("Azimuth is set to the 180 deg");
  digitalWrite(EN_PIN_1, LOW);
  AzMotor_Status = true;
  Serial.println("Azimuth Motor is enabled.");
  delay(2000);
  server.send(200, "application/json", "Azimuth is set to the 180 deg");
}

void handleMotorControl()
{
  if (server.hasArg("motor")) 
  {
    uint8_t cmd = server.arg("motor").toInt();    
  
    if (!tracking)
    {
      switch (cmd) 
      {
        case 1: digitalWrite(EN_PIN_1, HIGH);
                Serial.println("Azimuth Motor is disabled.");
                server.send(200, "application/json", "Azimuth Motor is disabled.");
                AzMotor_Status = false;
                break;
        case 2: digitalWrite(EN_PIN_2, HIGH);
                AltMotor_Status = false;
                Serial.println("Altitude Motor is disabled.");
                server.send(200, "application/json", "Altitude Motor is disabled.");
                break;
        case 3: digitalWrite(EN_PIN_1, HIGH);
                digitalWrite(EN_PIN_2, HIGH);
                AzMotor_Status = false;
                AltMotor_Status = false;
                motorControl=false;
                Serial.println("All Motors are disabled.");
                server.send(200, "application/json", "All Motors are disabled.");
                break;
        case 4: digitalWrite(EN_PIN_1, LOW);
                AzMotor_Status = true;
                Serial.println("Azimuth Motor is enabled.");
                server.send(200, "application/json", "Azimuth Motor is enabled.");                
                break;
        case 5: digitalWrite(EN_PIN_2, LOW);
                AltMotor_Status = true;
                Serial.println("Altitude Motor is enabled.");
                server.send(200, "application/json", "Altitude Motor is enabled.");
                break;
        case 6: digitalWrite(EN_PIN_1, LOW);
                digitalWrite(EN_PIN_2, LOW);
                motorControl=true;
                AzMotor_Status = true;
                AltMotor_Status = true;
                Serial.println("All Motors are enabled.");
                server.send(200, "application/json", "All Motors are enabled.");                
                break;
        default:
                Serial.println("\nUnknown Motor control command!");
                server.send(200, "Unknown Motor control command!"); 
      }
    }
    else
      server.send(400, "text/plain", "Can't control the motors, Tracking is enabled.");
  }
}

void handleUpdateTime()
{
  timeUpdate=true;
  update_time();
  Serial.println("RTC after update: " + Time);
  server.send(200, "text/plain", "Time updated.");
}

void handleHoming()
{
  Stepper_init();

  if(!manualAz)
  {     
    Serial.println("\n Homing Azimuth motor to 180 degree");
    float homeAngle=180-getAzimuth();
    float i;
    int d=homeAngle<0?-1:1;
    for( i=0; i<=abs(homeAngle); i++)
    {
      homeAngle=180-getAzimuth();
      Stepper_run(1 , d);
      int f=homeAngle<0?-1:1;
       server.handleClient();
      if(f!=d)
        break;      
    }

    Stepper_run(1 , 180-getAzimuth());  
    preAz = 180.0;  
    
  }
  else
  {
    Stepper_run(1 , 180-preAz);  
    preAz = 180.0;
  }
  xyzFloat angles = mpu.getAngles();
  pitch = mpu.getPitch();

  Serial.println("\n Homing Altitude motor to 0 degree");
  Stepper_run(2 , (0-pitch)); 
  preAlt = 0;

  homing=true;

  AzMotor_Status = true;
  AltMotor_Status = true;
  handleOffsetMPU();

  server.send(200, "application/json", "Homing done! Now press OffsetMPU.");
}

void handleObservationTime()
{
  if (!server.hasArg("observationTime")) 
  {
    server.send(400, "text/plain", "The Observation Time not provided");
    return;
  }

  observationTime = int(server.arg("observationTime").toFloat());   

  Serial.printf("\n Observation Time is set to: %d", observationTime);
  Serial.println();

  String response = "Observation Time is set to:" + String(observationTime);
  server.send(200, "application/json", response);
}

void handleSetMicrosteps()
{
  if (!server.hasArg("microsteps")) 
  {
    server.send(400, "text/plain", "Microsteps not provided");
    return;
  }

  microsteps = int(server.arg("microsteps").toFloat());   

  Serial.printf("\n Micosteps setting is set to: %d", microsteps);
  Serial.println();

  String response = "Observation Time is set to:" + String(microsteps);
  server.send(200, "application/json", response);
}

void handleOffsetMPU()
{
  Serial.println("Position you Telescope leveled and don't move it - calibrating...");
  delay(1000);
  mpu.autoOffsets();
  Serial.println("Done!");
    
  mpu.setSampleRateDivider(5);
  mpu.setAccRange(MPU9250_ACC_RANGE_2G);
  mpu.enableAccDLPF(true);
  mpu.setAccDLPF(MPU9250_DLPF_6);
  server.send(200, "text/plain", "MPU is calibrated.");
}

void handleStartTracking()
{
  if (!homing)
    server.send(400, "application/json", "Tracking is disabled. Start Homing first.");
  else
  {
    tracking=true;
    digitalWrite(EN_PIN_1, LOW);
    digitalWrite(EN_PIN_2, LOW);
    motorControl=true;
    AzMotor_Status = true;
    AltMotor_Status = true;

    if(mpuFound)
    {
      xyzFloat angles = mpu.getAngles();
      pitch = mpu.getPitch();
      preAlt=pitch;
    }
    if(!manualAz)
      preAz=getAzimuth();
    
    server.send(200, "application/json", "Tracking Enabled Now");
  }
  
}

void handleStopTracking()
{
  tracking=false;
  server.send(200, "application/json", "Tracking Disabled Now");
}

// Function to get GPS data asynchronously
void getGPSData() 
{
  while (Serial2.available() > 0) 
    gps.encode(Serial2.read());

 // Only process new and valid GPS data
  if (gps.location.isValid() && gps.location.isUpdated() && gps.satellites.value() >= 4)
  {
    longitude = gps.location.isValid() ? gps.location.lng() : 0.0;
    latitude = gps.location.isValid() ? gps.location.lat() : 0.0;
    elevation = gps.altitude.isValid() ? gps.altitude.meters() : 0.0;
    gpsStatus = true;

    Serial.printf("Latitude: %.6f, Longitude: %.6f, Altitude: %.2f m, Satellites: %d\n", 
    latitude, longitude, elevation, gps.satellites.value());
    error="NA";
  } 
  else 
  {
    Serial.println("GPS Data: NA");
    gpsStatus = false;
    error="GPS data is not available!";
  }
}

void handleDataRequest()
{
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["object"] = objectName;
  jsonDoc["time"] = Time;
  jsonDoc["tracking"] = tracking?"ON":"OFF";
  jsonDoc["objectAlt"] = Alt;
  jsonDoc["objectAz"] = Az;
  jsonDoc["altitude"] = pitch;
  jsonDoc["roll"] = roll;
  jsonDoc["azimuth"] = preAz;    
  jsonDoc["latitude"] = latitude;
  jsonDoc["longitude"] = longitude;
  jsonDoc["declination"] = declination; 
  jsonDoc["ra"] = ra; 
  jsonDoc["dec"] = dec; 
  jsonDoc["raDeg"] = apparentRA; 
  jsonDoc["decDeg"] = apparentDEC; 
  jsonDoc["elevation"] = elevation;
  jsonDoc["temperature"] = temperature;
  jsonDoc["error"] = error;

  String response;
  serializeJson(jsonDoc, response);
  server.send(200, "application/json", response);
}


void handleSelectObject()
{
  if (server.hasArg("object")) 
  {
    objectName = server.arg("object");
    objectStatus = true;
    
    if (WiFi_status)
      fetchAPI(objectName);

    Serial.printf("\nObject Name: %s", objectName.c_str());
    Serial.println();
  }
  else
    server.send(400, "text/plain", "Object name is not provided.");

  server.send(200, "application/json", "Object name received");
}

// Handle GET request for star tracker data


void handleSetLocation()
{
  if (!server.hasArg("latitude")) 
  {
    server.send(400, "text/plain", "Latitude not provided");
    return;
  }
  if (!server.hasArg("longitude")) 
  {
    server.send(400, "text/plain", "Longitude not provided");
    return;
  }

  latitude = server.arg("latitude").toFloat();
  longitude = server.arg("longitude").toFloat();
  gpsStatus = true;
  Serial.printf("\n Manual Location set to: %.6f°", latitude);
  Serial.printf(" / %.6f°", longitude);
  Serial.println();

  server.send(200, "text/plain", "Location Updated");
}


// Handle POST request to set declination
void handleRefresh()
{  
  getGPSData();
  delay(10);
  getMagneticDeclinationOnline(); 

  Serial.printf("All data refreshed");
  server.send(200, "text/plain", "All data refreshed");
}


// Handle POST request to set declination
void handleSetDeclination()
{
  Serial.println("Changing declination");
  
    if (!server.hasArg("declination")) 
    {
        server.send(400, "text/plain", "Declination not provided");
        Serial.println("Declination not provided");
        return;
    }
    Serial.println("Declination provided");

    declination = server.arg("declination").toFloat();
    declinationStatus = true;
    Serial.printf("New Magnetic Declination Set: %.2f°\n", declination);

    server.send(200, "text/plain", "Declination Updated");
}

void getMagneticDeclinationOnline() 
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    HTTPClient http;

    String url = "https://www.ngdc.noaa.gov/geomag-web/calculators/calculateDeclination?lat1=" + String(latitude) + "&lon1=" + 
    String(longitude)+ "&key=zNEw7&resultFormat=json";

    Serial.println(url);

    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) 
    {
      String payload = http.getString();
      Serial.println("Response: " + payload);

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      declination = doc["result"][0]["declination"];
      Serial.println("Magnetic Declination fetched from Internet: " + String(declination)+"°");
      declinationStatus = true;
    } 
    else 
    {
      Serial.println("Failed to fetch Magnetic Declination, using manual input.");
      declinationStatus = false;
    }
      http.end();
  } 
  else 
  {
    Serial.println("GPS data invalid, unable to fetch declination.");
    declinationStatus = false;
  }
}

void getMPU() 
{
  if(mpuFound)
  {      
    xyzFloat angles = mpu.getAngles();

    pitch = mpu.getPitch();
    roll  = mpu.getRoll();

    temperature = mpu.getTemperature();

    Serial.print("Pitch   = "); 
    Serial.print(pitch); 
    Serial.print("  |  Roll    = "); 
    Serial.println(roll); 
    
    Serial.println();
  }
  else
  {
    Serial.println("\nCheck connections mpu9250. Divce is not responding. Try switch off and on after a while, check if this works.");
  }
  
}

uint8_t readRegister(uint8_t deviceAddress, uint8_t regAddress) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission(false); // Send restart
  Wire.requestFrom(deviceAddress, (uint8_t)1);
  
  if (Wire.available()) {
    return Wire.read();
  }
  return 0xFF; // Return 0xFF if nothing available
}

