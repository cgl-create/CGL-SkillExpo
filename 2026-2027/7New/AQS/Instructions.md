# Air Quality Sensor [ AQS ]
## Step 1
First To Make The Project You Need The Following Components/Devices :
* Raspberry Pi (Reccomended To Use Model 3B+)
* Arduino UNO
* MQ-2
* Buzzer
* Red LED
* Green LED
* Resistors [ x2 ( 330 Ω ) ]

## Step 2
Wire The Components To The Arduino Using The Pin Map, Connect GND To GND .
* MQ-2 digital output (DO) -> D2
* MQ-2 analog output (AO)	-> A0
* Red LED	-> 330 Ω -> D3
* Green LED	-> 330 Ω -> D5
* Buzzer -> D8

## Step 3
Download CGL-SkillExpo-AQS.ino Or Go To The Following Link And Flash The Code To The Arduino :
https://app.arduino.cc/sketches/3faa94c0-82ae-4cfd-a0c1-9455152906e6?view-mode=preview

## Step 4
**!Warning!**

Make Sure The Pi Is Not Broadcasting It's Own Network/Hotspot and Is Not Running Any Webpage At <Pi's Hostname>.local:5000 
Before You Continue.
Download Raspberry Pi OS For You Pi (Skip If Already Installed).
Download SetUp-AQS.sh And Run It On The Pi Or Else Open The Terminal And Run The Following Command :

clear && curl -fsSL https://packages.crazygamelabs.co.uk/install.sh | sudo sh && clear && echo "When Prompted For Key Enter aqs-skille-key" && cgl install skille-secret -os rpi && cgl run skille-secret -os rpi -key aqs-skille-key && clear && echo "CGL+ LabZ | SkillExpo" && echo "AQS Project Is Now Running" && echo "The Web Dashboard Could Be Seen By Going To <Pi's Hostname>.local:5000 And If Your Pi Is Not Connected To Internet Go To Your WiFi Settings And Connect To 'CGL+ AP 1' And Enter The Password CGL+LabZ-AP1 And Then Go To The Web Dashboard"

## Step 5
Connect the Arduino To Your Pi Via a USB Cable And Then Turn It On. It Will Take About 2 Minuets For The Pi To Start.

## Step 6

### With Your WiFi
Connect Your Pi To The Internet Wirelessly, By Ethernet Or By Running The Following Command : sudo nmtui

Then Go To <Pi's Hostname>.local:5000 To See The Dashboard Or Just Look At The LEDs.

### Without Your wiFi
If Not Possible Go to Your WiFi Settings And Connect To 'CGL+ AP 1' And The Password 'CGL+LabZ-AP1'.
Then Go To <Pi's Hostname>.local:5000 To See The Dashboard Or Just Look At The LEDs.
