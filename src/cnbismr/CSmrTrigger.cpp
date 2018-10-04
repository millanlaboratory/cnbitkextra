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

#ifndef CSMRTRIGGER_CPP
#define CSMRTRIGGER_CPP

#include "CSmrTrigger.hpp"
#include <stdio.h>

CSmrTrigger::CSmrTrigger(CSmrEngine* engine, CpTrigger* trigger, 
		bool verbose) {
	this->_engine = engine;
	this->_trigger = trigger;
	this->_verbose = verbose;
	this->Disable();

	CB_CSmrEngine(this->_engine->i_OnDraw, this, HandleDraw);
}

void CSmrTrigger::HandleDraw(CSmrEngine* caller) {
	this->_semsend.Wait();
	if(this->_enabled) {
		this->_trigger->Send(this->_value);
	}
	
	if(this->_verbose) {
		if(this->_enabled) 
			printf("[CSmrTrigger::HandleDraw] Sending...\n");
		else
			printf("[CSmrTrigger::HandleDraw] Not sending\n");
	}
	this->_enabled = false;
	this->_semsend.Post();
}

void CSmrTrigger::Enable(CpTriggerByte value) {
	this->_semsend.Wait();
	this->_value = value;
	this->_enabled = true;
	this->_semsend.Post();
	
	if(this->_verbose)
		printf("[CSmrTrigger::Enable] CpTrigger enabled\n");
}

void CSmrTrigger::Disable(void) {
	this->_semsend.Wait();
	this->_enabled = false;
	this->_semsend.Post();
	
	if(this->_verbose)
		printf("[CSmrTrigger::Disable] CpTrigger disabled\n");
}

#endif
