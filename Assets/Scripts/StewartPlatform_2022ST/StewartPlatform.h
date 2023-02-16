#pragma once
#include <Arduino.h>
#include <Servo.h>

//----------------------------------------------------------
// Constants
//----------------------------------------------------------
#define FRAME_START '!'
#define FRAME_END '#'
#define SERVO_COUNT 6
#define FLOAT_BYTE_SIZE 4
#define ANGLE_MIN 30
#define ANGLE_MAX 150
#define D2R PI/180
#define R2D 180/PI

#define RIGHT_FRONT 300
#define RIGHT_MID 120
#define RIGHT_BACK 180
#define LEFT_BACK 0
#define LEFT_MID 60
#define LEFT_FRONT 240 

//----------------------------------------------------------
// Custom Data Types
//----------------------------------------------------------

typedef union
{
	float floatingPoint;
	byte binary[FLOAT_BYTE_SIZE];
} BinaryFloat;

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3 operator+(const Vector3& a) const
	{
		return Vector3(x + a.x, y + a.y, z + a.z);
	}
	Vector3 operator-(const Vector3& a) const
	{
		return Vector3(x - a.x, y - a.y, z - a.z);
	}
	Vector3 operator*(const float& a) const
	{
		return Vector3(a * x, a * y, a * z);
	}
	Vector3 operator+=(const Vector3& a)
	{
		x = x + a.x;
		y = y + a.y;
		z = z + a.z;
		return Vector3(x, y, z);
	}
};

class StewartPlatform
{
public:
	//----------------------------------------------------
	// *** Enter the values for your platform/hardware here
	//----------------------------------------------------
	int servoPins[SERVO_COUNT] = { 5,3,11,10,9,6 }; // servo pins
	int servoTrims[SERVO_COUNT] = { 9,-9,7,-9,-7,6 }; // fine tuning trim adjustments
	int backLeftServoIndex = 0; // The index of your back left servo (starting at zero)
	int servoOrderDirection = -1; // -1 for counter-clockwise, 1 for clockwise
	float hornLength = 12.4f; // Servo Horn Length
	float rodLength = 93.35f; // Connecting Rod Length
	float servoAxisAngles[SERVO_COUNT] = { LEFT_BACK, RIGHT_BACK, RIGHT_MID, RIGHT_FRONT ,LEFT_FRONT,LEFT_MID };


	enum OperationStates { Input_8Bit = 0, Input_Float32 };
	OperationStates operationState = Input_8Bit;
	bool SHOW_DEBUG = true; // gets set false after one successful cycle
	
	enum ReadStates { WaitingToStart, Reading_Platform_Data, Reading_LED_Byte, WaitingToEnd };
	ReadStates currentReadState = WaitingToStart;

	//----------------------------------------------------------
	// Variables used for input
	//----------------------------------------------------------
	byte inputBytes[SERVO_COUNT];
	BinaryFloat inputFloats[SERVO_COUNT];
	int byteIndex = 0;
	int inputIndex = 0;

	//----------------------------------------------------------
	// Servo
	//----------------------------------------------------------
	Servo myServos[SERVO_COUNT];

	// DOFs - This array holds the raw values for the desired position and orientation of the platform
	// [0 = Sway (mm), 1 = Surge (mm), 2 = Heave (mm), 3 = Pitch (rads), 4 = Roll (rads), 5 = Yaw( rads)]
	float DOFs[SERVO_COUNT] = { 0, 0, 0, 0, 0, 0 };

	//----------------------------------------------------
	// Calculated Values
	//----------------------------------------------------
	float servoAnglesInRadians[SERVO_COUNT] = { 0, 0, 0, 0, 0, 0 };
	float platformDefaultHeight;
	Vector3 platformDefaultHeightVector;

	//----------------------------------------------------
	// Dynamic (Allocate memory only when needed)
	//----------------------------------------------------
	Vector3 basePoints[SERVO_COUNT];
	Vector3 platformPoints[SERVO_COUNT];

	void InitializePlatform(StewartPlatform::OperationStates newState);
	void AttachServos();
	void SetOperationState(StewartPlatform::OperationStates newState);
	void ChangeReadState(ReadStates _state);
	void DefineBasePoints();
	void DefinePlatformPoints();
	void CalculateDefaultHeight();

	bool ReadSerialInput();
	bool CalculateServoValues();
	void UpdateServos();

	float Sway();
	float Surge();
	float Heave();
	float Pitch();
	float Roll();
	float Yaw();

	float MapRange(float val, float min, float max, float newMin, float newMax);
	void LogVector(String name, Vector3 vector, bool useLineSeparator);
	void LogValue(String name, String value, bool useLineSeparator);
	float GetMagnitude(Vector3 v);
	int Mod(int x, int m);
	Vector3 CrossProduct(Vector3 v1, Vector3 v2);
	Vector3 RotatePoint(Vector3 point, float degX, float degY, float degZ);
	float StewartPlatform::GetServoAngle(int i, Vector3 difference);
};
