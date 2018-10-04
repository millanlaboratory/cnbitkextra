/*
    Copyright (C) 2009-2011  EPFL (Ecole Polytechnique Fédérale de Lausanne)
    Michele Tavella <michele.tavella@epfl.ch>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CPTRIGGERSERIAL_CPP
#define CPTRIGGERSERIAL_CPP

#include "CpTriggerSerial.hpp"
#include <cnbicore/CcBasic.hpp>
#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

CpTriggerSerial::CpTriggerSerial(std::string port) {
	this->_port = port;
}
		
CpTriggerSerial::CpTriggerSerial(CCfgConfig* config) {

	try {
		this->_port = config->RootEx()->QuickStringEx("configuration/cptriggerserial/port");
	} catch(XMLException e) {
		CcLogErrorS("Configuration failed, using default values: " << e.Info());
		this->_port = "/dev/ttyACM0";
	}
}

CpTriggerSerial::~CpTriggerSerial(void) { 
	this->Close();
}

void CpTriggerSerial::Open(void) { 
	/* Open File Descriptor */
	this->_fid = open( (this->_port).c_str(), O_WRONLY | O_NONBLOCK | O_NOCTTY);
	if(this->_fid < 0)
		throw CcException("Cannot open serial port", __PRETTY_FUNCTION__);

	// Configure serial port
	memset (&this->_tty, 0, sizeof(this->_tty));

	/* Error Handling */
	if ( tcgetattr ( this->_fid, &this->_tty ) != 0 ) {
	   std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
	}

	/* Set Baud Rate */
	cfsetospeed (&this->_tty, (speed_t)B115200);

	/* Setting other Port Stuff */
	this->_tty.c_cflag     &=  ~PARENB;            // Make 8n1
	this->_tty.c_cflag     &=  ~CSTOPB;
	this->_tty.c_cflag     &=  ~CSIZE;
	this->_tty.c_cflag     |=  CS8;

	this->_tty.c_cflag     &=  ~CRTSCTS;           // no flow control
	
	/* Make raw */
	cfmakeraw(&this->_tty);

	/* Flush Port, then applies attributes */
	tcflush( this->_fid, TCIFLUSH );
	if ( tcsetattr ( this->_fid, TCSANOW, &this->_tty ) != 0) {
	   std::cout << "Error " << errno << " from tcsetattr" << std::endl;
	}
	this->_open = true;
}

void CpTriggerSerial::Close(void) { 
	close(this->_fid);
	this->_open = false;
}

void CpTriggerSerial::Send(CpTriggerByte data) { 
	if(this->_open == false)
		throw CcException("Serial port is not open", __PRETTY_FUNCTION__);
	int n_written = write( this->_fid, &data, 1);
	if(n_written != 1)
		throw CcException("Data could not be sent", __PRETTY_FUNCTION__);
}

#endif
