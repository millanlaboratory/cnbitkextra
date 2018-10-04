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

#ifndef CSMRENGINEEXT_CPP 
#define CSMRENGINEEXT_CPP 

#include "CSmrEngineExt.hpp" 

CSmrEngineExt::CSmrEngineExt(const std::string& caption, int winw, int winh, 
		CpTrigger* trigger) : CSmrEngine(caption, winw, winh) {
	this->Init(trigger);
}
		
CSmrEngineExt::CSmrEngineExt(CCfgConfig* configuration, CpTrigger* trigger) 
	: CSmrEngine(configuration) {
	this->Init(trigger);
}

CSmrEngineExt::~CSmrEngineExt(void) {
	if(this->_events != NULL) {
		this->_events->Stop();
		this->_events->Join();
		delete(this->_events);
		this->_events = NULL;
	}
	if(this->_screensync != NULL) {
		delete(this->_screensync);
		this->_screensync = NULL;
	}
}
void CSmrEngineExt::Init(CpTrigger* trigger) {
	this->_events = NULL;
	this->_events = new CSmrEvents(this);
	CB_CSmrEvents(this->_events->i_OnKeyboard, this, HandleKeyboard);
	CB_CSmrEvents(this->_events->i_OnQuit, this, HandleQuit);
	CB_CSmrEvents(this->_events->i_OnRedraw, this, HandleRedraw);

	this->_trigger = trigger;
	this->_screensync = NULL;
	if(this->_trigger != NULL)
		this->_screensync = new CSmrTrigger(this, this->_trigger, false);
}

void CSmrEngineExt::Open(void) {
	CcThread::Start();
	this->_events->Start();
}

void CSmrEngineExt::Close(void) {
	if(this->_events->IsRunning() == true) {
		this->_events->Stop();
		this->_events->Join();
	}
	if(this->_screensync)
		this->_screensync->Disable();
	CSmrEngine::Close();
}

bool CSmrEngineExt::ScreenSync(CpTriggerByte value) {
	if(this->_screensync) {
		this->_screensync->Enable(value);
		return true;
	}
	return false;
}

void CSmrEngineExt::HandleKeyboard(CSmrEvents* caller, CSmrKeyboard event) {
	CcLogDebugS("Grabbed keyboard event:" << event.sym);
	this->eventkeyboard.TrySet(event.sym);
}

void CSmrEngineExt::HandleQuit(CSmrEvents* caller) {
	CcLogDebug("Grabbed quit event");
	this->eventquit.TrySet(true);
}

void CSmrEngineExt::HandleRedraw(CSmrEvents* caller) {
	//CcLogDebug("Grabbed redraw event");
	this->eventredraw.TrySet(true);
}

#endif
