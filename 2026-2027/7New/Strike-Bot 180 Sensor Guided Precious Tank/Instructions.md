# Tank-Full [ Military Tank Model ]

## Step 1

First To Make The Project You Need The Following Components/Devices :

- Arduino UNO (Or Nano)
- HC-SR04 Ultrasonic Sensor
- SG90 Servo (Scanning Head) [ x1 ]
- SG90 Servo (Dart Pusher) [ x1 ]
- 2 x High-Speed DC Motors With Rubber Flywheels
- Logic-Level N-MOSFET (IRLZ44N Or Similar)
- Diode (1N5819 Or 1N4007)
- Resistors [ 100 Ω, 10 kΩ ]
- Switch (Arm Switch)
- Switch (Flywheel Battery Kill Switch)
- Nerf Foam Darts (Stock Power Only)
- Flywheel Battery Pack (2S LiPo Or The Donor Blaster's Pack)
- 5 V Supply For The Servos (4 x AA Pack Or 5 V Regulator)
- USB Power Bank Or USB Cable (For The Arduino)
- Breadboard, Jumper Wires And Mounting Hardware

## Step 2

**!Warning!**

This Project Fires Foam Darts. Keep It At Stock Nerf Power, Keep The Wheels Fully Enclosed, And Never Aim It At A Person's Face. Keep The Arm Switch **OPEN** While Loading, Clearing A Jam Or Carrying The Tank. Disconnect The Flywheel Battery For Any Jam.

## Step 3

Mount The Parts On The Tank :

- Mount The Scanning Servo On The Turret And Fix The HC-SR04 On The Servo Head Facing Forward.
- Mount The 2 Flywheel Motors Facing Each Other, With The Gap Slightly Smaller Than The Dart's Diameter.
- Fix The Dart Pusher Servo Behind The Gap So It Pushes One Dart Straight Between The Wheels.
- Put A Guard Over The Wheels.

## Step 4

Wire The Components To The Arduino Using The Pin Map, Connect All Grounds Together :

**Sensor**

- HC-SR04 VCC -> 5V
- HC-SR04 Trig -> D7
- HC-SR04 Echo -> D8
- HC-SR04 GND -> GND

**Servos** [ Power Them From The 5 V Servo Supply, Not The Arduino ]

- Scanning Servo Signal -> D6
- Dart Pusher Servo Signal -> D5
- Both Servo Power Wires -> 5 V Servo Supply (+)
- Both Servo Ground Wires -> 5 V Servo Supply (-) And Arduino GND

**Arm Switch**

- Arm Switch -> One Side To D4, Other Side To GND

**Flywheel Motors**

- Arduino D3 -> 100 Ω -> MOSFET Gate
- MOSFET Gate -> 10 kΩ -> GND
- MOSFET Source -> GND
- MOSFET Drain -> (-) Wire Of Both Motors Joined Together
- (+) Wire Of Both Motors -> Kill Switch -> Flywheel Battery (+)
- Diode -> Stripe End To Battery (+), Other End To MOSFET Drain
- Flywheel Battery (-) -> GND

Both Motors Are Wired In Parallel. Connect Arduino GND, Servo Supply (-), Flywheel Battery (-) And MOSFET Source Together At One Point.

## Step 5

Check The Wheel Direction. Looking Down From Above With The Muzzle Pointing Away From You, The Left Wheel Must Spin Counterclockwise And The Right Wheel Clockwise, So The Sides Touching The Dart Move Forward. If A Strip Of Paper Held Near The Gap Is Pulled Backward, Swap The Two Wires On **One** Motor Only.

## Step 6

Download Tank-Full.ino And Flash The Code To The Arduino Using The Arduino IDE Or Arduino Cloud.

## Step 7

Tune The Values At The Top Of The Code Before Loading A Dart :

- flywheelSpeed -> Starts At 120, Raise In Steps Of 20 Only Until The Dart Flies Reliably (Max 255)
- triggerRest / triggerFire -> The Pusher Servo Angles, Set With No Dart Loaded
- spinUpMs -> Time The Wheels Need To Reach Full Speed
- fireDelayMs -> The Warning Countdown After A Target Is Detected
- cooldownMs -> Minimum Time Between Shots

## Step 8

Power On With The Arm Switch **OPEN**. Open The Serial Monitor At 9600 Baud, It Should Say "System Ready. Scanning..." And "Launcher safe (switch open)". The Head Sweeps From 0° To 180° And Back And Pauses When Something Comes Within 25 cm.

## Step 9

Do A Dry Test With No Dart Loaded :

1. Close The Arm Switch.
2. Put Your Hand In Front Of The Sensor.
3. The Head Stops And The Wheels Spin Up During A 3 Second Countdown.
4. The Pusher Servo Moves Once, And Then The Wheels Stop.

Open The Arm Switch Again When Done.

## Step 10

Load A Dart Only At The Demo Table, Pointing The Tank At A Soft Target Board. Close The Arm Switch Last.

The Tank Fires **One Dart** For Each Detection. After A Shot It Waits Until The Scan Resumes And A New Target Is Detected. If The Target Stays There For 25 Seconds The Scan Resumes And Ignores That Target Until It Leaves.

### Note [ Please Read ]

If The Launcher Does Not Work Or Is Not Tested, Leave The Arm Switch Open Or Disconnect The Flywheel Battery. The Scanner Demo Still Works Without It.

Keep The Demo Zone Marked, Keep Eye Protection Available For Anyone Close, And Turn Off The Kill Switch When Moving The Tank.

If The Head Stops A Few Degrees After The Object, That Is Normal, Because A Target Needs 3 Readings In A Row To Be Confirmed. Set minHits To 1 In The Code To Stop Instantly.
