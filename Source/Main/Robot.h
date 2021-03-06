// Ensure this library description is only included once
#ifndef Robot_h
#define Robot_h

#include "Arduino.h"
#include "Controller/MotorController.h"
#include "Controller/BluetoothController.h"
#include "Controller/WifiController.h"
#include "Controller/SensorController.h"
#include "Util/Point.h"

class Robot
{
	public:
		Robot();
		void process();

		// Motor controller
		Point getPosition();
    	Point getRawPosition();
		double getHeading();
		void moveForward(double distance);
		void moveBackward(double distance);
		void turnLeft(int degrees);
		void turnRight(int degrees);
		void startVacuum();
		void stopVacuum();
		void toggleVacuum();
		void brake();

		// Wifi controller
		char** wifiScan(int &size);
		bool connectToWifi(char* wifiName, char* password);
		bool connectToServer(char* serverIp, int serverPort);
  		bool writeToServer(char* messageToWrite, int messageToWriteSize);

  		// Sensor controller
  		void startSensorsServo();
  		void stopSensorsServo();
  		void toggleSensorsServo();
  		float getLeftEyeRawData();
  		Point getLeftEyeData();
  		float getFrontEyeRawData();
  		Point getFrontEyeData();
  		float getRightEyeRawData();
  		Point getRightEyeData();

  		int getNumberOfCommands();
  		int getVacuumStatus();
  		int getSensorsServoStatus();

	private:
		bool _vacuumFlag;
		bool _sensorsServoFlag;

		void resetFlags();
		MotorController _motorController;
		SensorController _sensorController;
		BluetoothController _bluetoothController;
		WifiController _wifiController;

		// Callbacks called by bluetooth module
		static void enableRemoteControlCallback();
		static void moveForwardCallback();
		static void moveBackwardCallback();
		static void turnLeftCallback();
		static void turnRightCallback();
		static void enablePumpCallback();
		static void enableServoFreeMoveCallback();

		void updateMovement();
};
#endif
