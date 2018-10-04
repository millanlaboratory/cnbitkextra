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

#include "CSmrEvents.hpp"
#include <stdio.h>

CSmrEventType eventType;
CSmrKeyboard eventKeyboard;
CSmrMouse eventMouse;

int event_handler(dtk_hwnd wnd, int type, const union dtk_event* evt) {
	eventType = type;
	switch(eventType) {
		case DTK_EVT_QUIT:
			break;
		case DTK_EVT_REDRAW:
			break;
		case DTK_EVT_KEYBOARD:
			eventKeyboard.sym = evt->key.sym;
			eventKeyboard.state = evt->key.state;
			eventKeyboard.mod = evt->key.mod;
			break;
		case DTK_EVT_MOUSEBUTTON:
		case DTK_EVT_MOUSEMOTION:
			eventMouse.x = evt->mouse.x;
			eventMouse.y = evt->mouse.y;
			eventMouse.button = evt->mouse.button;
			eventMouse.state = evt->mouse.state;
			break;
	}
	return 1;
}

CSmrEvents::CSmrEvents(CSmrEngine* engine) {
	this->_engine = engine;
}

void CSmrEvents::Main(void) {
	bool acquired;
	static bool csmr_nonce = true;

	while(CcThread::IsRunning()) { 
		this->_engine->WindowWait();
		this->_window = this->_engine->GetWindow();
		acquired = (this->_window != NULL);
		this->_engine->WindowPost();

		if(acquired){
			CcLogConfig("Acquired CSmrEngine...");
			break;
		}
		if (csmr_nonce){
			CcLogConfig("Waiting for CSmrEngine...");
			csmr_nonce = false;
		}
	}

	CcLogConfig("Events enabled");
		
	eventType = -1;
	dtk_set_event_handler(this->_window, event_handler);

	while(CcThread::IsRunning()) { 
		this->_engine->WindowWait();
		dtk_process_events(this->_window);
		this->_engine->WindowPost();

		switch(eventType) {
			case DTK_EVT_KEYBOARD:
				if(this->i_OnKeyboard.IsRegistered())
					this->i_OnKeyboard.Execute(this, eventKeyboard);
				break;
			case DTK_EVT_MOUSEBUTTON:
				if(this->i_OnMouse.IsRegistered())
					this->i_OnMouse.Execute(this, eventMouse);
				break;
			case DTK_EVT_MOUSEMOTION:
				if(this->i_OnMouseMotion.IsRegistered())
					this->i_OnMouseMotion.Execute(this, eventMouse);
				break;
			case DTK_EVT_QUIT:
				if(this->i_OnQuit.IsRegistered())
					this->i_OnQuit.Execute(this);
				break;
			case DTK_EVT_REDRAW:
				if(this->i_OnRedraw.IsRegistered())
					this->i_OnRedraw.Execute(this);
				break;
			default:
				break;
		}
		eventType = 0;
		CcTime::Sleep(CSmrEvents::WaitEvent);
	}
	CcLogConfig("Events disabled");
}
