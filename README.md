# arduino-race-chrono-oil-temp-sensor
Arduino-based project for oil temperature sensor. Installed into Suzuki Swift Sport ZC32S

## Overview
This repository contains code, pictures, schematics and instructions on how to  
put together a Bluetooth oil tempeartue sensor that sends data to RaceChrono app.
Physically the sensor fits average oil filter sandwich.
The code, schematics and instructions are provided with no warranty and no liability, follow at you own risk!

## Oil temperature sensor
### Calibrating the sensor
The average analog oil temperature sensor off of ebay is used. The resistance of the sensor changes depending on  
the temperature. The documetation for the sensor provides resistance at 40C and 120C. It is easy to the the third point - 100C  
by putting the sensor into boiling water and measuring resistance. Use the https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm  
and three data points to find out which model and coefficients reflect the behaviour of the sensor the best.  
Use the model and coefficients to calculate the resistance of the sensor at a given temperature. In this project the 5C step is picked, with temperatures in between  
approximated linearly.

### Arduino ADC input / Picking the dividor resistor
The Arduino ADC is used to convert analog input into digital data. Voltage divider is used to conver sensor resistance output into Arduino ADC input signal.  
Sensor is connected to 3.3V output and ADC input, divider resistor is conntected to ADC input and the ground. The impedance of divider resistor is picked to provide  
greater resolution in the area of operating temperatues of the engine. Oil temperature sensors use thermistors, so the resistance drops with higher temperature,  
reaching thousands or hundreds in the operating range (100..130C). I recommend to use ~1kOhm resistor.
Combine the exact impedance of the resistor with data from "Calibrating the sensor" to calculate voltage outputs of the divider.
In this code the ADC is configured to provide 12 bits resolution, or values in 0..4095 range. 
Fill out the table 
```
const uint16_t rawDataTable[24] = {
 72, 84, 97, 113, 132, 153, 178, 208, //20 .. 55 C
 242, 283, 331, 387, 455, 535, 630, 743,// 60 .. 95 C
 880, 1038, 1241, 1481, 1772, 2129, 2581, 3304 // 100 .. 135 C
};
```
with your calculated values.

Arduino Nano 33 BLE has issues with ADC inputs: https://forum.arduino.cc/t/analogread-very-noisy-adc-values-on-nano-33-ble/631315
To mitigate those the digital `RunningMedian` filter is added to this project.

## Connecting to RaceChrono
This project implements RaceChrono Bluetooth DIY compatible device: https://racechrono.com/forum/discussion/1922/racechrono-diy-bluetooth-ble-protocol.
The Arduino output is split into two 8-bit frames and is put back toghether in the RaceChrono app settings.
To set up the device: 
 - turn on Arduino; uncomment the `Serial.print(...)` lines if you want to debug Bluetooth connections
 - open the app, click on gear icon
 - under "Other devices" click on "+ Add other device" -> "RaceChrono DIY" -> "Bluetooth LE". The device must be on and advertising BLE connection for this step to work
 - under "Vehicle profiles" click on "Car" (yes it is a clickable UI element the brings you to another setting section, this confused me A LOT)
 - under "CAN-Bus channels" click "+ Add channel", pick a name like "Engine oil temperature 1", enter following settings:
   
   <img src="https://github.com/iaroshukandrii/arduino-race-chrono-oil-temp-sensor/blob/main/IMG_5103.jpg?raw=true" alt="race_chrono_settings" width="200"/>

   (note the "Equation" putting high and low bits back together with a formula)

## Pictures
Arduino back and front:  

<img src="https://github.com/iaroshukandrii/arduino-race-chrono-oil-temp-sensor/blob/main/IMG_4443.JPG?raw=true" alt="race_chrono_settings" width="300"/>
<img src="https://github.com/iaroshukandrii/arduino-race-chrono-oil-temp-sensor/blob/main/IMG_4444.JPG?raw=true" alt="race_chrono_settings" width="300"/>

## Suzuki Swift Sport installation tips
The most important one - average ebay oil sandwich does not fit the depth of oil filter winding.
Even fully screwed on the fitting does not hold the sandwich in place, there is a gap of ~1mm. The sim is required to offset the gap and ensure tight seal between  
sandwich plate and oil filter flange on the engine:  
<img src="https://github.com/iaroshukandrii/arduino-race-chrono-oil-temp-sensor/blob/main/IMG_4663.JPG?raw=true" alt="race_chrono_settings" width="300"/>
<img src="https://github.com/iaroshukandrii/arduino-race-chrono-oil-temp-sensor/blob/main/IMG_4666.JPG?raw=true" alt="race_chrono_settings" width="300"/>

(note the shim installed between the sandwich and the fitting)

Oil feed diameter:
Suzuki Swift Sport oil feed inner diameter is 14mm (TODO add photo). The average ebay oil sandwich fitting is 12mm. The metal is soft enough and there is enough "meat"  
to grind it off with a file, or machine it - if you have access to one.  
Top - as sold on ebay, bottom - opened up with a file:  
<img src="https://github.com/iaroshukandrii/arduino-race-chrono-oil-temp-sensor/blob/main/IMG_4662.JPG?raw=true" alt="race_chrono_settings" width="300"/>

Fitting in the engine bay:
The sandwitch fits as-is, but for better fitment angle of the sensor and better exhaust downpipe clearance I recommend to grind off the side of the sandwich.  
<img src="https://github.com/iaroshukandrii/arduino-race-chrono-oil-temp-sensor/blob/main/IMG_4594.JPG?raw=truee" alt="race_chrono_settings" width="300"/>

(note the bottom side of the sandwich on the pic was ground off to clear exhaust)

## Closing words
To complete the project and install the sensor in the car you need at least basic understanding of car electical systems.  
I recommend connecting Arduino to ACC inputs so it is turned on when you turn the ignition key.  
Use fire-proof engine harness insulation for the wiring, think about the best route for the new wires and secure them in place in the engine bay to prevent wear and tear caused by moving parts and vibration.  
Any contributions to this repo are welcome - please submit PRs and/or open issues. 
