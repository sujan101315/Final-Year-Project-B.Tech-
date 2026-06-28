#ifndef HTML_CONTENT_H
#define HTML_CONTENT_H

const char html_content[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Star Tracker</title>
  <style>
    body {
      font-family: 'Montserrat', sans-serif;
      font-size: 14px;
      text-align: center;
      display: flex;
      justify-content: center;
      align-items: center;
      height: auto;
      margin: 5;
      background: linear-gradient(to right, #141E30, #243B55);
      color: white;
    }
    .container {
      background: rgba(255, 255, 255, 0.1);
      padding: 5px;
      border-radius: 15px;
      box-shadow: 0px 0px 15px rgba(255, 255, 255, 0.2);
      width: 90%;
      max-width: 700px;
      backdrop-filter: blur(10px);
      margin-top: 10px;
      max-height: auto; Ensures it fits within the viewport */
      overflow-y: auto; /* Enables scrolling when content exceeds height */
    }
    table {
      width: 100%;
      border-collapse: collapse;
      margin-top: 10px;
      margin-bottom: 15px;
      table-layout: fixed;
    }
    td {
      padding: 5px;
      border: 1px solid rgba(255, 255, 255, 0.3);
      word-wrap: break-word;
      width: 50%;
    }
    input {
      padding: 5px;
      border-radius: 5px;
      border: none;
      outline: none;
      width: 95%;
    }
    select {
      padding: 5px;
      border-radius: 5px;
      border: none;
      outline: none;
      width: 98%;
    }
    button {
      background: #00C9A7;
      color: white;
      font-size: 13px;
      padding: 12px 12px;
      border: none;
      border-radius: 6px;
      cursor: pointer;
      margin: 3px;
      width: 80%;
      transition: 0.3s;
      font-weight: bold;
    }
    button:hover {
      background: #008F7A;
    }
    .status {
      margin-top: 8px;
      font-weight: bold;
    }

    .tracking-container 
    {
      display: flex;
      align-items: center;  /* Align items vertically */
      justify-content: center; /* Center horizontally */
      text-align: center;  /* Center text within each element */
      width: 90%;
      margin: 0 auto; /* Center container itself */
      gap: 60px; /* Space between elements */
    }

    h1 
    {
      margin-bottom:10px;
    }
    h2 
    {
      margin-bottom:5px;
      margin-top:0px;
    }

  </style>
  <link href="https://fonts.googleapis.com/css2?family=Montserrat:wght@400;700&display=swap" rel="stylesheet">
</head>
<body>
  <div class="container">    
    <h1>Star Tracker</h1>
    <div class="tracking-container">  
      <span class="tracking-status">Tracking Status: <span id="trackingState">OFF</span></span>   
      <h3>Tracking Object: <span id="object">Unknown</span></h3>
      <span id="time"> - -  -:- </span>
    </div>
    <table> 
      <tr><td>Object Alt:</td> <td><span id="objectAlt">0</span>°</td> <td>Object Az:</td> <td><span id="objectAz">0</span>°</td></tr>    
      <tr><td>Telecscope Altitude:</td> <td><span id="altitude">0</span>°</td> <td>Telescope Azimuth:</td> <td><span id="azimuth">0</span>°</td></tr>
      <tr> <td>Roll (Y-axis):</td> <td><span id="roll">0</span>°</td> <td>Latitude:</td> <td><span id="latitude">0</span>°</td></tr>
      <tr><td>Magnetic Declination:</td> <td><span id="magneticDeclination">0</span>°</td> <td>Longitude:</td> <td><span id="longitude">0</span>°</td></tr>
      <tr><td>RA:</td> <td><span id="ra">0h0m0s</span></td> <td>DEC:</td> <td><span id="dec">0°0'0&quot;</span></td></tr>
      <tr><td>RA in Deg:</td> <td><span id="raDeg">0</span>°</td> <td>DEC in Deg:</td> <td><span id="decDeg">0</span>°</td></tr>
      <tr><td>Temperature:</td> <td><span id="temperature">0</span>°</td> <td>Elevation:</td> <td><span id="elevation">0</span>m</td></tr>
    </table>
    
    <div class="controls">
      <table>
        <tr>
          <td>
            <select id="objectSelect" onchange="selectObject()">
              <option value="Sun" selected>Select Object to Track</option> 
              <option value="Sun">Sun</option>
              <option value="Mercury">Mercury</option>
              <option value="Venus">Venus</option>
              <option value="Mars">Mars</option>
              <option value="Jupiter">Jupiter</option>
              <option value="Saturn">Saturn</option>
              <option value="Uranus">Uranus</option>
              <option value="Neptune">Neptune</option>
              <option value="Pluto">Pluto (Sub)</option>
              <option value="Moon">Moon</option>
              <option value="Phobos">Phobos</option>
              <option value="Deimos">Deimos</option>
              <option value="Io">Io</option>
              <option value="Europa">Europa</option>
              <option value="Ganymede">Ganymede</option>
              <option value="Callisto">Callisto</option>
              <option value="Titan">Titan</option>
              <option value="Enceladus">Enceladus</option>
              <option value="Triton">Triton</option>
              <option value="Ceres">Ceres</option>
              <option value="Vesta">Vesta</option>
              <option value="Eros">Eros</option>
              <option value="Halley">Halley</option>
              <option value="Bennu">Bennu</option>
              <option value="Voyager 1">Voyager 1</option>
              <option value="Voyager 2">Voyager 2</option>
              <option value="New Horizons">New Horizons</option>
              <option value="JWST">James Webb Space Telescope (JWST)</option>
            </select>
          </td>
          <td>
              <select id="selectObserveTime" onchange="selectedTime()">
                <option value="1" selected>Select Observation Time</option> 
                <option value="1">1 min</option>
                <option value="5">5 min</option>
                <option value="10">10 min</option>
                <option value="15">15 min</option>
                <option value="30">30 min</option>
                <option value="60">1 hr</option>
                <option value="120">2 hr</option>
                <option value="180">3 hr</option>
                <option value="240">4 hr</option>
              </select>
            </td>
            <td>
              <select id="selectMicrosteps" onchange="selectedMicrosteps()">
                <option value="2" selected >Select Steps per Revolution</option> 
                <option value="1">200(Full)</option>
                <option value="2">400</option>
                <option value="4">800</option>
                <option value="8">1600</option>
                <option value="16">3200</option>
              </select>
            </td>

        </tr>
        <tr>
            <td><input type="number" id="latitudeInput" placeholder="Enter Latitude" step="0.01"></td>
            <td><input type="number" id="longitudeInput" placeholder="Enter Longitude" step="0.01"></td>
            <td><button onclick="setManualLocation()">Set Location</button></td>
        </tr>

        <tr>
          <td><input type="number" id="declinationInput" placeholder="Enter Declination" step="0.01"></td>
          <td><button onclick="setDeclination()">Set Declination</button></td>
          <td><button onclick="refreshData()">Refresh Data</button></td>
        </tr>

        <tr><td>
              <button onclick="startHoming()">Homing</button>
              <button onclick="offsetMPU()">Offset MPU</button>
              <button onclick="controlMotors(1, this)">Disable Az Motor</button>
            </td>
            <td>
              <button onclick="autoTime()">Update Time</button>
              <button onclick="manualAzPos()">Manual set Az 180</button>
              <button onclick="controlMotors(2, this)">Disable Alt Motor</button>
            </td>
            <td>
              <button onclick="startTracking()">Start Tracking</button>
              <button onclick="stopTracking()">Stop Tracking</button>  
              <button onclick="controlMotors(3, this)">Disable All Motor</button> 
            </td> 
        </tr> 

      </table>      
    </div>

    <div class="status" id="status">Status: Idle</div>
  </div>

  <script>
    async function manualAzPos() 
    {
      try {
        // Fetch the response from the server
        const response = await fetch('/set_azpos');

        // Check if the response is OK (status code 200-299)
        if (!response.ok) {
          throw new Error(`HTTP error! Status: ${response.status}`);
        }

        // Parse the response as text
        const message = await response.text();

        // Update the status element with the response message
        document.getElementById('status').textContent = `Status: ${message}`;
      } catch (error) {
        // Log the error to the console
        console.error("Error in setting manual Az position:", error);

        // Update the status element with an error message
        document.getElementById('status').textContent = "Error in setting manual Az position!";
      }
    }

    async function selectedTime()
    {
      try 
      {
        const response = await fetch('/set_observationTime', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
          },
          body: `observationTime=${document.getElementById("selectObserveTime").value}`,
        });
    
        if (!response.ok) throw new Error("Error in setting Observation Time!");

        const message = await response.text();
        document.getElementById('status').textContent = `Status: ${message}`;
      } 
      catch (error) 
      {
        console.error("Error in setting Observation Time:", error);
        document.getElementById('status').textContent = "Error in setting Observation Time!";
      }
    }

    async function selectedMicrosteps()
    {
      try 
      {
        const response = await fetch('/set_microsteps', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
          },
          body: `microsteps=${document.getElementById("selectMicrosteps").value}`,
        });
    
        if (!response.ok) throw new Error("Failed to set microsteps!");

        const message = await response.text();
        document.getElementById('status').textContent = `Status: ${message}`;
      } 
      catch (error) 
      {
        console.error("Failed to set microsteps:", error);
        document.getElementById('status').textContent = "Failed to set microsteps!";
      }
    }

  
    async function controlMotors(cmd, button) 
    {
      let command = cmd;
      console.log("Button clicked with cmd:", cmd);

      // Toggle button text and update command
      if (button.textContent.startsWith("Enable")) {
        button.textContent = button.textContent.replace("Enable", "Disable");
        command = command + 3; // Modify command if needed
      } else {
        button.textContent = button.textContent.replace("Disable", "Enable");
        // Optionally, reset or modify the command here if needed
      }

      try {
        // Send the motor control command to the server
        const response = await fetch('/motor_control', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
          },
          body: `motor=${command}`,
        });

        // Check if the response is OK (status code 200-299)
        if (!response.ok) {
          throw new Error("Failed to send motor control!");
        }

        // Parse the response as text
        const message = await response.text();

        // Update the status element with the response message
        document.getElementById('status').textContent = `Status: ${message}`;
      } catch (error) {
        // Log the error to the console
        console.error("Failed to send motor control!:", error);

        // Update the status element with an error message
        document.getElementById('status').textContent = "Failed to send motor control!";
      }
    }

    async function autoTime()
    {
      try {
        const response = await fetch('/update_time');
        const message = await response.text();
        document.getElementById('status').textContent = `Status: ${message}`;
      } catch (error) {
        console.error("Time is not updated:", error);
        document.getElementById('status').textContent = "Error in Updating Time!";
      }
    }


    async function startHoming()
    {
      try {
        const response = await fetch('/homing');
        const message = await response.text();
        document.getElementById('status').textContent = `Status: ${message}`;
      } catch (error) {
        console.error("Homing not performed!:", error);
        document.getElementById('status').textContent = "Error in Homing!";
      }
    }
    
    async function offsetMPU()
    {
      try {
        const response = await fetch('/offsetMPU');
        const message = await response.text();
        document.getElementById('status').textContent = `Status: ${message}`;
      } catch (error) {
        console.error("Error in Offsetting MPU!:", error);
        document.getElementById('status').textContent = "Error in Offsetting MPU!";
      }
    }

    async function fetchData() 
    {
      try 
      {
        const response = await fetch('/data');
        if (!response.ok) throw new Error("Failed to fetch data");

        const data = await response.json();
        let errorFields = [];

        function updateElement(id, value, decimalPlaces = 2)
        {
          const element = document.getElementById(id);
          if (element) 
          {
            element.textContent = (typeof value === "number" ? value.toFixed(decimalPlaces) : "N/A");
          } 
          else 
          {
            errorFields.push(id);
          }
        }

        document.getElementById('object').textContent = data.object || "Unknown";
        document.getElementById('time').textContent = data.time;
        document.getElementById('trackingState').textContent = data.tracking;
        updateElement('objectAlt', data.objectAlt);
        updateElement('objectAz', data.objectAz);
        updateElement('altitude', data.altitude);
        updateElement('roll', data.roll);
        updateElement('azimuth', data.azimuth, 0);
        updateElement('latitude', data.latitude, 6);
        updateElement('longitude', data.longitude, 6);
        updateElement('magneticDeclination', data.declination);
        document.getElementById('ra').textContent = data.ra;
        document.getElementById('dec').textContent = data.dec;
        document.getElementById('raDeg').textContent = data.raDeg;
        document.getElementById('decDeg').textContent = data.decDeg;
        updateElement('elevation', data.elevation, 0);
        updateElement('temperature', data.temperature);
        document.getElementById('status').textContent = data.error ;

        if(data.error!="NA")
          setTimeout(() => 
          {
            document.getElementById('status').textContent = data.error;
          }, 2000);
       

        // Fixed Altitude Condition
        if (data.objectAlt > 0 && data.objectAlt < 90) 
        {
          document.getElementById('status').textContent = "Object is observable!";
        } 
        else
        {
          document.getElementById('status').textContent = "Object is not observable!";
        }

        // Fixed Missing Elements Handling
        if (errorFields.length > 0) 
        {
          console.warn("Missing elements:", errorFields);
          document.getElementById('status').textContent = `Warning: Missing elements - ${errorFields.join(', ')}`;
        }
      } 
      catch (error) 
      {
        console.error("Error fetching data:", error);
        document.getElementById('status').textContent = "Error fetching data!";
      }
    }


    async function selectObject() 
    {
      try 
      {
        const response = await fetch('/select_object', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
          },
          body: `object=${document.getElementById("objectSelect").value}`,
        });
    
        if (!response.ok) throw new Error("Failed to set object");

        const message = await response.text();
        document.getElementById('status').textContent = `Status: ${message}`;
      } 
      catch (error) 
      {
        console.error("Error fetching data:", error);
        document.getElementById('status').textContent = "Error fetching data!";
      }
    }

    async function setManualLocation() 
    {
      const latitudeInput = document.getElementById('latitudeInput').value;
      const longitudeInput = document.getElementById('longitudeInput').value;

      // Validate inputs
      if (isNaN(latitudeInput) || isNaN(longitudeInput)) 
      {
        alert("Please enter valid numeric values for latitude and longitude!");
        return;
      }

      // Send data to the backend
      try 
      {
        const response = await fetch('/set_location', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
          },
          body: `latitude=${latitudeInput}&longitude=${longitudeInput}`,
        });
    
        if (!response.ok) throw new Error("Failed to set location");

        const message = await response.text();
        document.getElementById('status').textContent = `Status: ${message}`;
      } 
      catch (error) 
      {
        console.error("Error setting location:", error);
        document.getElementById('status').textContent = "Error setting location!";
      }
    } 

    async function setDeclination() 
    {
      const declinationInput = parseFloat(document.getElementById('declinationInput').value);
      
      if (isNaN(declinationInput)) {
        alert("Please enter a valid numeric value for declination!");
        return;
      }

      try {
        const response = await fetch('/set_declination', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
          },
          body: `declination=${declinationInput}`,
        });

        const message = await response.text();
        document.getElementById('status').textContent = `Status: ${message}`;
      } 
      catch (error) {
        console.error("Error setting declination:", error);
        document.getElementById('status').textContent = "Error setting declination!";
      }
    }
  

    async function refreshData() 
    {
      try {
        const response = await fetch('/refresh');
        const message = await response.text();
        document.getElementById('status').textContent = `Status: ${message}`;
      } catch (error) {
        console.error("Error refreshing data:", error);
        document.getElementById('status').textContent = "Error refreshing data!";
      }
    }
    
    async function startTracking()
    {
      try 
      {
        const response = await fetch('/start');
        const message = await response.text();
        document.getElementById('status').textContent = `Status: ${message}`;

        if (response.ok)
        {
          const button = document.getElementById("motorToggle");
          button.textContent = "Disable Motors";
        }

      } 
      catch (error) {
        console.error("Error starting tracking:", error);
        document.getElementById('status').textContent = "Error starting tracking!";
      }
    }

    async function stopTracking() 
    {
      try {
        const response = await fetch('/stop');
        const message = await response.text();
        document.getElementById('status').textContent = `Status: ${message}`;
      } catch (error) {
        console.error("Error stopping tracking:", error);
        document.getElementById('status').textContent = "Error stopping tracking!";
      }
    }

    // Fetch data every second
    setInterval(fetchData, 2000);
</script>
</body>
</html>

)=====";

#endif
