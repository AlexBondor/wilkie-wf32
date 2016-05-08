#ifndef WifiController_h
#define WifiController_h

#include "Arduino.h"

#define MAX_NETWORK_NAME_LENGTH 20
// Define the connection timeout
// Number of counts it takes to connect ~600k
#define CONNECTION_TIMEOUT 700000

typedef enum
{
  NONE = 0,
  CONNECT,
  CONNECTED,
  TCPCONNECT,
  TCPCONNECTED,
  WRITE,
  ACK,
  WRITTEN,
  READ,
  READED,
  CLOSE,
  DONE,
} STATE;

class WifiController
{
	public:
    WifiController();
    void doSomething();
		char** scan(int &size);
		void process();
		bool connectToWifi(char* wifiName, char* password);
		bool connectToServer(char* serverIp, int serverPort);
		bool writeToServer(char* messageToWrite, int messageToWriteSize);

	private:
};		

#endif