/**
 * @class CCPCBooster
 * Class used to communicate with CPCBooster
 * @author Thierry JOUIN
 * @version 1.1
 * @date 06/02/2006
 */

#include "CCPCBooster.h"
#include <stdio.h>

#include <iostream>

/**
 * @todo Gerer comNumber comme un entier sous windows et une chaine sous linux
 *
 * Pourquoi ne pas avoir utilise les read/write buffer depuis les read/write byte/word ?
 */
#if _WINDOWS
CCPCBooster::CCPCBooster(int comNumber) :
#else
CCPCBooster::CCPCBooster(std::string comNumber) :
_COMPortHandle(comNumber),
#endif
_COMPortNumber(comNumber),
_currentState(PortFailed),
_currentError(ErrOK)
{
	OpenPort();
}

CCPCBooster::~CCPCBooster()
{
	ClosePort();
}

bool CCPCBooster::IsOpen() const
{
	return (_currentState == PortOpened);
}

CCPCBoosterState CCPCBooster::GetState() const
{
	return _currentState;
}

void CCPCBooster::OpenPort()
{
	if (_currentState == PortOpened)
	{
		ClosePort();
	}

	_currentState = PortFailed;

	char portName[256];

/**
 * code specifique aux windows
 */
#if _WINDOWS
	sprintf(portName,"COM%d",_COMPortNumber);

    _COMPortHandle = CreateFile(	portName,
						GENERIC_READ | GENERIC_WRITE,
						0,								// must be opened with exclusive-access
						NULL,							// no security attributes
						OPEN_EXISTING,					// must use OPEN_EXISTING
						0,								// not overlapped I/O
						NULL);							// hTemplate must be NULL for comm devices
	
	if (_COMPortHandle == INVALID_HANDLE_VALUE) 
	{
		_currentError = ErrInvalidHandle;
	}
	else
	{	
		// Build on the current configuration, and skip setting the size
		// of the input and output buffers with SetupComm.
		DCB dcb;
		BOOL fSuccess = GetCommState(_COMPortHandle, &dcb);
		
		if (fSuccess != TRUE) 
		{
			_currentError = ErrGetStateFailed;
		}
		else
		{
		// configure COM properties
			// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wcecoreos5/html/wce50conprogrammingserialconnections.asp
			dcb.BaudRate = CBR_115200;				// set the baud rate
			dcb.ByteSize = 8;						// data size, xmit, and rcv
			dcb.Parity = NOPARITY;					// no parity bit
			dcb.StopBits = ONESTOPBIT;				// one stop bit
			// additionnal properties to fix some USB crappy serial interface
			dcb.fBinary = TRUE;						// Binary mode; no EOF check
			dcb.fOutxCtsFlow = FALSE;				// No CTS output flow control
			dcb.fOutxDsrFlow = FALSE;				// No DSR output flow control 
			dcb.fDtrControl = DTR_CONTROL_DISABLE;
			dcb.fDsrSensitivity = FALSE;			// DSR sensitivity
			dcb.fTXContinueOnXoff = FALSE;			// XOFF continues Tx
			dcb.fOutX = FALSE;						// No XON/XOFF out flow control
			dcb.fInX = FALSE;						// No XON/XOFF in flow control 
			dcb.fNull = FALSE;						// Disable null stripping 
			dcb.fRtsControl = RTS_CONTROL_DISABLE;
			dcb.fAbortOnError = FALSE;				// Do not abort reads/writes on error

			fSuccess = SetCommState(_COMPortHandle, &dcb);
			
			if (fSuccess != TRUE) 
			{
				_currentError = ErrSetStateFailed;
			}
			else
			{
				_currentState = PortOpened;
			}
			
			COMMTIMEOUTS timeOut;
			GetCommTimeouts(_COMPortHandle, &timeOut);

			timeOut.ReadTotalTimeoutMultiplier = 10;

			SetCommTimeouts(_COMPortHandle, &timeOut);


			}
		}

/**
 * Code specifique aux unices
 */
#else
	//portName = _COMPortNumber ;
/*
	_COMPortHandle.Open(
	    SerialPort::BAUD_115200,
	    SerialPort::CHAR_SIZE_8,
	    SerialPort::PARITY_NONE,
	    SerialPort::STOP_BITS_1,
	    SerialPort::FLOW_CONTROL_HARD);	
	    */

	_COMPortHandle.SetBaudRate(SerialStreamBuf::BAUD_115200);
	if ( !_COMPortHandle.good())
	{
	    _currentError = ErrInvalidHandle ;
	    return ;
	}

	_COMPortHandle.SetCharSize(LibSerial::SerialStreamBuf::CHAR_SIZE_8);
	if ( !_COMPortHandle.good())
	{
	    _currentError = ErrInvalidHandle ;
	    return ;
	}

	_COMPortHandle.SetParity(SerialStreamBuf::PARITY_NONE) ;
	if ( !_COMPortHandle.good())
	{
	    _currentError = ErrInvalidHandle ;
	    return ;
	}

	_COMPortHandle.SetNumOfStopBits(1);
	if ( !_COMPortHandle.good())
	{
	    _currentError = ErrInvalidHandle ;
	    return ;
	}
	
	_COMPortHandle.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE ) ;

	_COMPortHandle.unsetf( std::ios_base::skipws ) ;


	if (_COMPortHandle.IsOpen())
	{
	    _currentState = PortOpened ;
	}

#endif
	
}
void CCPCBooster::ClosePort()
{
	if (_currentState == PortOpened)
	{
#if _WINDOWS
		CloseHandle(_COMPortHandle);
#else
		_COMPortHandle.Close();
#endif
	}
	_currentState = PortClosed;
}

bool CCPCBooster::ReadWaitByte(unsigned char &val)
{
#if _WINDOWS
	unsigned long nbBytesReceived = 0;
	BOOL fSuccess = TRUE;
	while (	nbBytesReceived != 1 && fSuccess == TRUE) 
	{
		fSuccess = ReadFile (_COMPortHandle, &val, 1, &nbBytesReceived, NULL);
	}

	return ((nbBytesReceived == 1) && fSuccess);
#else
	unsigned long nbBytesReceived = 1 ;
	/*
	char byte = _COMPortHandle.ReadByte();

	val = byte ;
	*/

	_COMPortHandle >> val ;
	return true ; 
#endif
}
bool CCPCBooster::ReadByte(unsigned char &val)
{
#if _WINDOWS
	unsigned long nbBytesReceived = 0;
	
	BOOL fSuccess = ReadFile (_COMPortHandle, &val, 1, &nbBytesReceived, NULL);

	return nbBytesReceived == 1 ;
#else
	 unsigned long nbBytesReceived = 1 ;

	 /*
	 try{
		 val = _COMPortHandle.ReadByte(); 
	}catch(SerialPort::ReadTimeout e){
	    nbBytesReceived = 0 ;
	}*/

	 _COMPortHandle >> val ;
	return true;
#endif

}
bool CCPCBooster::WriteByte(const unsigned char val)
{
#if _WINDOWS
	unsigned long nbBytesSend = 0;
    
	BOOL fSuccess = WriteFile(_COMPortHandle, &val, 1, &nbBytesSend, NULL);
	
	return ((nbBytesSend == 1) && fSuccess);
#else
	unsigned long nbBytesSend = 1 ;
//	_COMPortHandle.WriteByte(val);
//

	_COMPortHandle.write( (const char *)&val,1) ;
	_COMPortHandle.flush();
	printf("\tWrite : $%x\n", val);
	return nbBytesSend == 1 ;
#endif
}

bool CCPCBooster::ReadWaitWord(unsigned short &val)
{
#if _WINDOWS
	unsigned long nbBytesReceived = 0;
	BOOL fSuccess = TRUE;
	while (	nbBytesReceived != 2 && fSuccess == TRUE) 
	{
		fSuccess = ReadFile (_COMPortHandle, &val, 2, &nbBytesReceived, NULL);
	}

	return ((nbBytesReceived == 2) && fSuccess);
#else
	unsigned char byte1, byte2 ;

	if (
		ReadWaitByte(byte1) &&
		ReadWaitByte(byte2) 
	    )
	{
	    val = byte1 * 256 + byte2 ;
	    return 1 == 1 ;
	}
	else{
	    return 1 == 0 ;
	}

#endif
}

bool CCPCBooster::ReadWord(unsigned short &val)
{
#if _WINDOWS
	unsigned long nbBytesReceived = 0;
	
	BOOL fSuccess = ReadFile (_COMPortHandle, &val, 2, &nbBytesReceived, NULL);

	return ((nbBytesReceived == 2) && fSuccess);
#else
	unsigned char byte1, byte2 ;

	if (
		ReadByte(byte1) &&
		ReadByte(byte2) 
	    )
	{
	    val = byte1 * 256 + byte2 ;
	    return 1 == 1 ;
	}
	else{
	    return 1 == 0 ;
	}


#endif
}
bool CCPCBooster::WriteWord(const unsigned short val)
{
#if _WINDOWS
	unsigned long nbBytesSend = 0;
    
	BOOL fSuccess = WriteFile(_COMPortHandle, &val, 2, &nbBytesSend, NULL);
	
	return ((nbBytesSend == 2) && fSuccess);
#else
/*
	unsigned char byte1, byte2 ;

	byte1 = val % 256 ;
	byte2 = val / 256 ;

	_COMPortHandle.WriteByte(byte1);
	_COMPortHandle.WriteByte(byte2) ;
*/

	_COMPortHandle << val ;
	return true ;
#endif
}

bool CCPCBooster::ReadWaitBuffer(unsigned char *buffer, const  long nbBytes)
{
#if _WINDOWS
	unsigned long nbBytesReceived = 0;
	BOOL fSuccess = TRUE;
	while (	nbBytesReceived != nbBytes && 
			nbBytesReceived != 0 &&
			fSuccess == TRUE) 
	{
		fSuccess = ReadFile (_COMPortHandle, buffer, nbBytes, &nbBytesReceived, NULL);
	}

	return ((nbBytesReceived == nbBytes) && fSuccess);
#else
	/*
	for (long i=0 ; i< nbBytes ; i++)
	    buffer[i] =_COMPortHandle.ReadByte( );
	    */

	_COMPortHandle.read( (char *)buffer, 
		             nbBytes) ;

	return true ;
#endif
}


bool CCPCBooster::ReadBuffer(unsigned char *buffer, const long nbBytes)
{
#if _WINDOWS
	unsigned long nbBytesReceived = 0;
	
	BOOL fSuccess = ReadFile (_COMPortHandle, buffer, nbBytes, &nbBytesReceived, NULL);

	return ((nbBytesReceived == nbBytes) && fSuccess);
#else
	/*
	for (long i=0 ; i< nbBytes ; i++)
	    buffer[i] =_COMPortHandle.ReadByte( 100 );
	    */
	_COMPortHandle.read( (char *)buffer,                                                                             
	                     nbBytes) ; 
	return true ;

#endif
}
bool CCPCBooster::WriteBuffer(unsigned char *buffer, const  long nbBytes)
{
#if _WINDOWS
	unsigned long nbBytesSend = 0;
    
	BOOL fSuccess = WriteFile(_COMPortHandle, buffer, nbBytes, &nbBytesSend, NULL);
	

	std::cout << (int)nbBytesSend << " bytes send" << std::endl;
	return ((nbBytesSend == nbBytes) && fSuccess);
#else
	/*
	 for (long i=0 ; i< nbBytes ; i++)
	    _COMPortHandle.WriteByte(buffer[i]);
    */

	_COMPortHandle.write( (char *) buffer, nbBytes);
	return true ;
#endif

}
