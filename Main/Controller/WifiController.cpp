#include "WifiController.h"

#include <MRF24G.h> 
#include <DEIPcK.h>
#include <DEWFcK.h>
#include "../Util/ByteList.h"

TCPSocket _tcpSocket;
STATE state = NONE;
IPSTATUS status = ipsSuccess;

unsigned _tStart = 0;
unsigned _tWait = 5000;

// Network connection details
char* _wifiName = "";
char* _password = "";

// Server details
char* _serverIp = "";
int _serverPort = 80;

// Write to server variables
ByteList _messageQueue;
int _messageQueueSize = 0;

byte* _messageToWrite;
byte comma[] = {','};
byte newline[] = {'\n'};
int _messageToWriteSize = 0;

byte _messageToRead[1024];
int _messageToReadSize = 0;

/*
 *  Makes a scan of the available WiFi networks
 *  Returns a list of the names of the networks and binds
 *  the function argument to the number of available networks
 */
char** WifiController::scan(int &size)
{
  char** result;
  while(!deIPcK.wfScan(&size, &status))
  {
    DEIPcK::periodicTasks();
  }

  #ifdef WIFI_DEBUG
  Serial.println(size);
  #endif

  result = new char*[size];
  if(IsIPStatusAnError(status))
  {
    #ifdef WIFI_DEBUG
    Serial.println("Scan Failed");
    #endif
  }
  SCANINFO scanInfo;
  for (int i = 0; i < size; i++)
  {
    if(deIPcK.getScanInfo(i, &scanInfo))
    {
      #ifdef WIFI_DEBUG
      Serial.print("Scan info for index: ");
      Serial.println(i, DEC);
      Serial.print("SSID: ");
      #endif

      result[i] = new char[scanInfo.ssidLen];
      for (int j = 0; j < scanInfo.ssidLen; j++)
      {
        result[i][j] = (char) scanInfo.ssid[j];
      }
    }
    else
    {
      result[i] = "Unavailable";
    }
  }
  return result;
}

/*
 *  This method must be called each loop cycle in order
 *  for the WiFi module to be responsive.
 *  Switch case taken from the TCPEchoClient example application
 *  provided by Digilent
 */
void WifiController::process()
{
	IPSTATUS status;
  int cbRead = 0;

  switch (state)
  {

	  case CONNECT:
	  {
      if (deIPcK.wfConnect(_wifiName, _password, &status))
      {
      	#ifdef WIFI_DEBUG
        Serial.println("Connected to WiFi.");
        #endif
        deIPcK.begin();
        state = CONNECTED;
      }
      else if (IsIPStatusAnError(status))
      {
      	#ifdef WIFI_DEBUG
        Serial.print("Unable to connect, status: ");
        Serial.println(status, DEC);
        #endif
        state = CLOSE;
      }
      break;	  	
	  }

	  case TCPCONNECT:
	  {
      if (deIPcK.tcpConnect(_serverIp, _serverPort, _tcpSocket))
      {
      	#ifdef WIFI_DEBUG
        Serial.println("Connected to server.");
        #endif
        state = TCPCONNECTED;
      }
  		break;	  	
	  }

	  // write out the strings
	  case WRITE:
	  {
	  	if(_messageQueueSize > 1 && _tcpSocket.isEstablished())
      {     
      	int temp = _messageQueueSize;
      	if (temp % 2 != 0)
      	{
      		temp --;
      	}
      	for (int i = 0; i < temp; i++)
      	{
	      	_tcpSocket.writeStream(_messageQueue.getFirstMessage(), _messageQueue.getFirstLength());
	      	if (i == temp - 1)
	      	{
	      		_tcpSocket.writeStream(newline, 1);
	      	}
	      	else
	      	{
	      		_tcpSocket.writeStream(comma, 1);
	      	}
	      	_messageQueue.removeFirst();
	      	_messageQueueSize--;
      	}

      	#ifdef WIFI_DEBUG
	      Serial.println("Written to server.");
	      #endif
	      state = ACK;
      }
  		break;
	  }
	      
    // look for the echo back
    // case READ:
    // {
    // 	// see if we got anything to read
    //   if((cbRead = _tcpSocket.available()) > 0)
    //   {
    //     cbRead = cbRead < sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
    //     cbRead = _tcpSocket.readStream(rgbRead, cbRead);

    //     for(int i=0; i < cbRead; i++)
    //     {
    //       Serial.print((char) rgbRead[i]);
    //     }
    //   }

    //   // give us some time to get everything echo'ed back
    //   else if( (((unsigned) millis()) - tStart) > tWait )
    //   {
    //     Serial.println("");
    //     state = READED;
    //   }
    //   break;
    // }   

    case ACK:
    {
    	if ((_messageToReadSize = _tcpSocket.available()) > 0)
    	{
    		_messageToReadSize = _messageToWriteSize < sizeof(_messageToRead) ? _messageToReadSize : sizeof(_messageToRead);
    		_messageToReadSize = _tcpSocket.readStream(_messageToRead, _messageToReadSize);

    		// _messageQueue.removeFirst();

    		// _messageQueueSize--;

      	#ifdef WIFI_DEBUG
    		Serial.println("Ack from server");
    		#endif

      	state = WRITE;
    	}
      break;
    }     

	  // done, so close up the tcpSocket
	  case CLOSE:
	  {
	  	_tcpSocket.close();
    	#ifdef WIFI_DEBUG
      Serial.println("Closing TcpClient, Done with sketch.");
      #endif
      state = DONE;
      break;
	  }
	      
	  case DONE:
	  {
	  	break;
	  }

	  default:
	  {
      break;	  	
	  }
  }

  // keep the stack alive each pass through the loop()
  DEIPcK::periodicTasks();
}

/*
 *  Tries to establish a connection to the wifi whose details
 *  are provided as parameters
 */
bool WifiController::connectToWifi(char* wifiName, char* password)
{
	state = CONNECT;
	_wifiName = wifiName;
	_password = password;
	while (state == CONNECT)
	{
		process();
	}
	return state == CONNECTED;
}

/*
 *  Tries to establish a connection to a server whose details are
 *  provided as parameters.
 *  Note that a successful WiFi connection must be established in the
 *  first place.
 */
bool WifiController::connectToServer(char* serverIp, int serverPort)
{
	state = TCPCONNECT;
	_serverIp = serverIp;
	_serverPort = serverPort;
	while (state == TCPCONNECT)
	{
		process();
	}
	state = WRITE;
	return state == TCPCONNECTED;
}

/*
 *  Adds a message to be sent to the server to the messages queue.
 *  The messages are not sent directly because we don't want to make
 *  the robot unresponsive. We buffer the messages and once we find
 *  a small window we send all the buffered messages to the server.
 */
bool WifiController::writeToServer(char* messageToWrite, int messageToWriteSize)
{
	_messageToWriteSize = messageToWriteSize;
	_messageToWrite = new byte(_messageToWriteSize);
	for (int i = 0; i < _messageToWriteSize; i++)
	{
		_messageToWrite[i] = (byte)messageToWrite[i]; 
	}
	_messageQueue.add(_messageToWrite, _messageToWriteSize);
	_messageQueueSize++;
	return true;
}