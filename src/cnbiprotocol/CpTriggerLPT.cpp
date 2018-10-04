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

#ifndef CPTRIGGERLPT_CPP
#define CPTRIGGERLPT_CPP

#include "CpTriggerLPT.hpp"
#include <cnbicore/CcBasic.hpp>

CpTriggerLPT::CpTriggerLPT(int port, unsigned int dt, unsigned char level) {
	this->_port = port;
	this->_dt = dt;
	this->_level = level;
	this->Init();
}
		
CpTriggerLPT::CpTriggerLPT(CCfgConfig* config) {
	this->Init();

	try {
		this->_port = config->RootEx()->QuickIntEx("configuration/cptriggerlpt/port");
		this->_dt = config->RootEx()->QuickIntEx("configuration/cptriggerlpt/duration");
		this->_level = config->RootEx()->QuickIntEx("configuration/cptriggerlpt/level");
	} catch(XMLException e) {
		CcLogErrorS("Configuration failed, using default values: " << e.Info());
		this->_port = -1;
		this->_dt = 15;
		this->_level = 0;
	}
}

void CpTriggerLPT::Init(void) {
	this->_open = false;
}

CpTriggerLPT::~CpTriggerLPT(void) { 
	this->Close();
}

void CpTriggerLPT::Open(void) { 
	this->_trigger = OpenLPTTrigger(this->_level, this->_dt, this->_port);
	if(this->_trigger == NULL)
		throw CcException("Cannot open LPT port", __PRETTY_FUNCTION__);
	this->_open = true;
}

void CpTriggerLPT::Close(void) { 
	CloseLPTTrigger(this->_trigger);
	this->_open = false;
}

void CpTriggerLPT::Send(CpTriggerByte data) { 
	if(this->_open == false)
		throw CcException("LPT port is not open", __PRETTY_FUNCTION__);
	SignalTrigger(this->_trigger, data);
}

#endif
