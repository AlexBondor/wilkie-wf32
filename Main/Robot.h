// Ensure this library description is only included once
#ifndef Robot_h
#define Robot_h

#include "Arduino.h"
#include "Controller/MotorController.h"
#include "Controller/BluetoothController.h"
#include "Controller/WifiController.h"

class Robot
{
	public:
		Robot();
		void process();
		Point getPosition();
		void moveForward(double distance);
		void moveBackward(double distance);
		void turnLeft(int degrees);
		void turnRight(int degrees);
		void startVacuum();
		void stopVacuum();
		void brake();

		// Wifi controller
		char** wifiScan(int &size);
		bool connectToWifi(char* wifiName, char* password);
		bool connectToServer(char* serverIp, int serverPort);
    	bool writeToServer(char* messageToWrite, int messageToWriteSize);

	private:
		void resetFlags();
		MotorController _motorController;
		BluetoothController _bluetoothController;
		WifiController _wifiController;

		// Callbacks called by bluetooth module
		static void enableRemoteControlCallback();
		static void enablePumpCallback();
		static void moveForwardCallback();
		static void moveBackwardCallback();
		static void turnLeftCallback();
		static void turnRightCallback();

		void updateMovement();
};
#endif
