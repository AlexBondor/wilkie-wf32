#include "WifiController.h"

#include <MRF24G.h> 
#include <DEIPcK.h>
#include <DEWFcK.h>
#include "../Util/ByteList.h"

TCPSocket _tcpSocket;
STATE state = NONE;

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