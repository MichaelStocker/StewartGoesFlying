#include "StewartPlatform.h"

//----------------------------------------------------
// Change Read State (defines params of expected data input)
//----------------------------------------------------
void StewartPlatform::ChangeReadState(ReadStates _state)
{
	currentReadState = _state;
}

void StewartPlatform::AttachServos()
{
	// attach servos
	for (int i = 0; i < SERVO_COUNT; i++)
	{
		myServos[i].attach(servoPins[i]);
	}
}

void StewartPlatform::SetOperationState(StewartPlatform::OperationStates newState)
{
	operationState = newState;

	if (operationState == Input_Float32)
	{
		Serial.println("Using 6 DOF 32 Float mode - Each Axis gets it's own float value");
	}
	else if (operationState == Input_8Bit)
	{
		Serial.println("Using 6 DOF Single Byte mode - Each Axis gets 1 byte 0-255");
	}
}

//----------------------------------------------------
// Initialize Platform
//----------------------------------------------------
void StewartPlatform::InitializePlatform(StewartPlatform::OperationStates newState)
{
	AttachServos();
	SetOperationState(newState); // determines read mode (8bit, 32bit, etc)
	ChangeReadState(WaitingToStart); // the state of reading input (start, reading, end)
	if (CalculateServoValues()) // update with starting values
	{
		UpdateServos();
	}
}

//----------------------------------------------------
// Define Base Points
//----------------------------------------------------
void StewartPlatform::DefineBasePoints()
{
	// *** 1.)
	// Student must fill this function to set the 6 basePoints
	// values for each of the 6 attachment points. Z should be 0
	// For example: basePoints[0] = { 20, 10, 0 };

	basePoints[0] = {-9.93f, -53.00f, 0.00f};
	basePoints[1] = {9.93f, -53.00f, 0.00f};
	basePoints[2] = {50.87f, 17.90f, 0.00f};
	basePoints[3] = {40.94f, 35.10f, 0.00f};
	basePoints[4] = {-40.94f, 35.10f, 0.00f};
	basePoints[5] = {-50.87f, 17.90f, 0.00f};

	if (SHOW_DEBUG)
	{
		Serial.println("\n----------------------------------");
		for (int i = 0; i < SERVO_COUNT; i++) LogVector("basePoints " + String(i), basePoints[i], false);
	}
}

//----------------------------------------------------
// Define Platform Points
//----------------------------------------------------
void StewartPlatform::DefinePlatformPoints()
{
	// *** 2.) Student must fill this function to set the 
	// platformPoints values for each of the 6 
	// attachment points (Vector3[]). Z should be 0
	// For example: platformPoints[0] = { 15, 5, 0 };

	platformPoints[0] = {-34.98f, -30.16f, 0.00f};
	platformPoints[1] = {34.98f, -30.16f, 0.00f};
	platformPoints[2] = {43.61f, -15.22f, 0.00f};
	platformPoints[3] = {8.63f, 45.38f, 0.00f};
	platformPoints[4] = {-8.63f, 45.38f, 0.00f};
	platformPoints[5] = {-43.61f, -15.22f, 0.00f};

	if (SHOW_DEBUG)
	{
		Serial.println("----------------------------------");
		for (int i = 0; i < SERVO_COUNT; i++) LogVector("platformPoints " + String(i), platformPoints[i], false);
	}
}

//----------------------------------------------------
// Calcualte Default Height
//----------------------------------------------------
void StewartPlatform::CalculateDefaultHeight()
{
	// We need to know what the "default height" of the 
	// platform is (when servos are flat 90 degrees)
	// We have enough information to use the pythagorean 
	// theorem to calculate this height

	Vector3 hornDirection = Vector3(1, 0, 0);
	Vector3 hornEndPoint = basePoints[backLeftServoIndex] + (hornDirection * hornLength);

	if (SHOW_DEBUG) LogVector("hornEndPoint", hornEndPoint, true);

	// Calculate default height (Pythagorean theorem)
	float sideC = rodLength;
	float sideB = GetMagnitude(platformPoints[backLeftServoIndex] - hornEndPoint);

	// *** 3.) Students must calculate the length of side A (c^2 = a^2 + b^2)
	float sideA = sqrt(sideC*sideC-sideB*sideB);

	platformDefaultHeight = sideA;
	platformDefaultHeightVector = Vector3(0, 0, platformDefaultHeight);

	Serial.print("Default Height:"); Serial.println(platformDefaultHeight);

	// Set the height (z) value for each of our platformPoints
	for (int i = 0; i < SERVO_COUNT; i++)
	{
		platformPoints[i].z = platformDefaultHeight;
	}
}


//----------------------------------------------------
// Calculate new servo values based on input values.
// If the calculations fail mathematically, the servos
// will not update to the new positions.
//----------------------------------------------------
bool StewartPlatform::CalculateServoValues()
{
	float servoRadians = 0; // local temp value to hold the calculated servo angle
	Vector3 platformPoint = Vector3(0, 0, 0); // local temporary value to hold results as we iterate through a for loop
	bool success = true; // gets set to false if any of our calculations result in an error value

	for (int i = 0; i < SERVO_COUNT; i++)
	{
		if (SHOW_DEBUG) { LogValue("i", "[" + String(i) + "]", true); }

		// Perform Rotate the top platform by our requested orientation values (rotation matrix)
		platformPoint = RotatePoint(platformPoints[i], Pitch(), Yaw(), Roll());

		// Translate the top platform (move each point by the requested value (displacement))
		platformPoint += Vector3(Sway(), Surge(), Heave());

		// Add our default height value back in
		platformPoint.z += platformDefaultHeight;

		// Virtual Rod Direction (Vector subtraction - get the difference between base and platform points)
		Vector3 calculatedRodVector = platformPoint - basePoints[i];
		if (SHOW_DEBUG) { LogVector("calculatedRodVector", calculatedRodVector, false); }

		// Virtual Rod Length (Magnitude)
		float calculatedRodLength = GetMagnitude(calculatedRodVector);
		if (SHOW_DEBUG) { LogValue("calculatedRodLength", String(calculatedRodLength), false); }

		servoRadians = GetServoAngle(i, calculatedRodVector);
		if (SHOW_DEBUG) { LogValue("Radians", String(servoAnglesInRadians[i]), false); }

		// Check range/erros. If our values went past what our platform can physically 
		// or mathematically achieve then we'll likely have a NAN value here. NotANumber
		// values in this case are probably due to a sqrt on a negative (i.e. out of range)
		if (isnan(servoRadians))
		{
			if (SHOW_DEBUG) Serial.println("Error: Invalid Servo Angle");
			success = false;
			break;
		}
		else
		{
			servoAnglesInRadians[i] = servoRadians;
		}
	}

	return success;
}

void StewartPlatform::UpdateServos()
{
	//----------------------------------------------------------------------------------
	// Calculations are complete, now add add trim values and write to the servos
	//----------------------------------------------------------------------------------
	float servoAngle = 0;
	for (int i = 0; i < SERVO_COUNT; i++)
	{
		// Our calculations are worked out with zero degrees as the home angle,
		// so we need to add 90 degrees to account for the servo's home "flat" position
		servoAngle = (i % 2 == 0 ? -1 : 1) * servoOrderDirection * (R2D * servoAnglesInRadians[i]) + 90;
		if (SHOW_DEBUG) { LogValue("Servo Angle Before Constrain & Trim:", String(servoAngle), i == 0 ? true : false); }

		servoAngle = constrain(servoAngle, ANGLE_MIN, ANGLE_MAX);
		myServos[i].write(servoAngle + servoTrims[i]);
	}

	if (SHOW_DEBUG) { SHOW_DEBUG = false; }
}

//----------------------------------------------------
// Getters - Encapsulate our DOF values for convenience
//----------------------------------------------------
float StewartPlatform::Sway() { return DOFs[0]; }
float StewartPlatform::Surge() { return DOFs[1]; }
float StewartPlatform::Heave() { return DOFs[2]; }
float StewartPlatform::Pitch() { return D2R * -DOFs[3]; }
float StewartPlatform::Roll() { return D2R * DOFs[4]; }
float StewartPlatform::Yaw() { return D2R * -DOFs[5]; }

float StewartPlatform::GetMagnitude(Vector3 v)
{
	// *** 4.) Student must fill in this function to return the magnitude of the Vector3 passed in
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

int StewartPlatform::Mod(int x, int m)
{
	return (x % m + m) % m;
}

float StewartPlatform::MapRange(float val, float min, float max, float newMin, float newMax)
{
	return ((val - min) / (max - min) * (newMax - newMin) + newMin);
}

void StewartPlatform::LogVector(String name, Vector3 v, bool useLineSeparator)
{
	if (useLineSeparator) Serial.println("----------------------------------");
	Serial.print(name);
	Serial.print(" : (");
	Serial.print(v.x);
	Serial.print(", ");
	Serial.print(v.y);
	Serial.print(", ");
	Serial.print(v.z);
	Serial.println(")");
}

void StewartPlatform::LogValue(String name, String value, bool useLineSeparator)
{
	if (useLineSeparator) Serial.println("----------------------------------");
	Serial.print(name + ": ");
	Serial.println(value);
}

//----------------------------------------------------
// Apply rotation matrix for each of the 6 platform points.
// This will return the rotation vector3 (euler angles) 
// as if the entire top platform were a single object.
//----------------------------------------------------
Vector3 StewartPlatform::RotatePoint(Vector3 point, float degX, float degY, float degZ)
{
	float x = point.x * cos(degZ) * cos(degY) + point.y * (sin(degX) * sin(degZ) * cos(degZ) - cos(degX) * sin(degY));
	float y = point.x * cos(degZ) * sin(degY) + point.y * (cos(degX) * cos(degY) + sin(degX) * sin(degZ) * sin(degY));
	float z = -point.x * sin(degZ) + point.y * sin(degX) * cos(degZ);
	return Vector3(x, y, z);
}

//----------------------------------------------------
// Math Stuffs
//----------------------------------------------------
Vector3 StewartPlatform::CrossProduct(Vector3 v1, Vector3 v2)
{
	double x, y, z;
	x = v1.y * v2.z - v2.y * v1.z;
	y = (v1.x * v2.z - v2.x * v1.z) * -1;
	z = v1.x * v2.y - v2.x * v1.y;
	return Vector3(x, y, z);
}

float StewartPlatform::GetServoAngle(int i, Vector3 rodVector)
{
	// Project the end point of the new virtual arm onto the servo's plane of rotation  
	float L = sq(GetMagnitude(rodVector)) - sq(rodLength) + sq(hornLength);
	if (SHOW_DEBUG) { LogValue("L", String(L), false); }

	float M = 2.0f * hornLength * rodVector.z;
	if (SHOW_DEBUG) { LogValue("M", String(M), false); }

	float N = 2.0f * hornLength * (cos(D2R * servoAxisAngles[i]) * rodVector.x + sin(D2R * servoAxisAngles[i]) * rodVector.y);
	if (SHOW_DEBUG) { LogValue("N", String(N), false); }

	// Derive the servo angle (in radians)
	return asin(L / sqrt(M * M + N * N)) - atan(N / M);
}

//----------------------------------------------------
// Read and Parse Serial Input
//----------------------------------------------------
bool StewartPlatform::ReadSerialInput()
{
	while (Serial.available() > 0)
	{
		byte inByte = Serial.read(); // read the next byte in the stream

		//----------------------------------------------------
		// Waiting for start
		//----------------------------------------------------
		if (currentReadState == WaitingToStart)
		{
			if ((char)inByte == FRAME_START)
			{
				ChangeReadState(Reading_Platform_Data);
				inputIndex = 0;
			}
		}
		//----------------------------------------------------
		// Reading_Platform_Data - message is being read
		//----------------------------------------------------
		else if (currentReadState == Reading_Platform_Data)
		{
			//----------------------------------------------------
			// Read 8 bit input (one byte per servo, 0 to 255)
			//----------------------------------------------------
			if (operationState == Input_8Bit)
			{
				inputBytes[inputIndex] = (int)inByte;
				// Remap the byte value (0-255) to our usable range (mm for position, degrees for rotation)
				DOFs[inputIndex] = MapRange(inputBytes[inputIndex], 0.0f, 255.0f, -15.0f, 15.0f);

				// Increment the index until we read values for all servos, then change to next State
				if (inputIndex < SERVO_COUNT - 1) { inputIndex++; }
				else { ChangeReadState(WaitingToEnd); }
			}
			//----------------------------------------------------
			// Read float values from binary (4 bytes per servo)
			//----------------------------------------------------
			else if (operationState == Input_Float32)
			{
				inputFloats[inputIndex].binary[byteIndex] = inByte;
				// Increment the index until we read all values, then change to next State
				if (byteIndex < FLOAT_BYTE_SIZE - 1)
				{
					byteIndex++;
				}
				else
				{
					byteIndex = 0;
					DOFs[inputIndex] = (float)inputFloats[inputIndex].floatingPoint;

					// Have we read values for all servos?
					if (inputIndex < SERVO_COUNT - 1)
					{
						inputIndex++;
					}
					else
					{
						ChangeReadState(WaitingToEnd);
					}
				}
			}
		}
		//----------------------------------------------------
		// Waiting for end
		//----------------------------------------------------
		else if (currentReadState == WaitingToEnd)
		{
			ChangeReadState(ReadStates::WaitingToStart);
			if ((char)inByte == FRAME_END)
			{
				if (SHOW_DEBUG) Serial.println("---> Message Received");
				//UpdateValues();
				return true;
			}
			else
			{
				if (SHOW_DEBUG) Serial.println("---> Message Discarded");
			}
		}
		return false;
	}
}


/*
3.sqrtf(sideC * sideC - sideB * sideB);
4.sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
*/