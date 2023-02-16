#include <Servo.h>
#include "StewartPlatform.h"

StewartPlatform stewartPlatform; // main stewart platform class
unsigned long nextSend = 0; // timer in milliseconds to delay outgoing messages (optional)
unsigned int delayBetweenSend = 20; // length of the delay between serial sends(ms)

//----------------------------------------------------
// Setup
//----------------------------------------------------
void setup()
{
	Serial.begin(115200);
	Serial.setTimeout(20);

	stewartPlatform.DefineBasePoints(); // The x and y points for each base attachment (center point of the servo horn attachment)
	stewartPlatform.DefinePlatformPoints(); // The x and y points for each platform attachment (center of ball joint on upper rod attachment)
	stewartPlatform.CalculateDefaultHeight(); // Mathematically determine the default height (all servos horns horizontal at 90 degrees)
	stewartPlatform.InitializePlatform(StewartPlatform::Input_Float32); // operation mode
}

//----------------------------------------------------
// Main Loop
//----------------------------------------------------
void loop()
{
	// Continuously look for serial data, if we receive a valid message (return value of true)
	// then we calculate the values and if the calculations are valid, we apply them to the servos
	if (stewartPlatform.ReadSerialInput())
	{
		if (stewartPlatform.CalculateServoValues())
		{
			stewartPlatform.UpdateServos();
		}
	}
	//SendData(); // optional - if you need to send sensor data from Arduino to Unity
}

//----------------------------------------------------
// Send out some serial data (optional)
//----------------------------------------------------
void SendData()
{
	// put it on a timestamp limitation because we only need data at 50 - 60 fps
	if (millis() > nextSend)
	{
		// Here you can do any sensor readings and send data in whatever format
		String s = String(random(0, 1023)) + "," + String(25.4) + ",What's up," + random(0, 2);
		Serial.println(s);
		nextSend = millis() + delayBetweenSend;
	}
}