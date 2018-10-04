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

#ifndef HBFEEDBACKEVENTS_HPP
#define HBFEEDBACKEVENTS_HPP

#include "CSmrEngine.hpp"
#include <drawtk.h>
#include <dtk_event.h>

typedef struct dtk_keyevent CSmrKeyboard;
typedef struct dtk_mouseevent CSmrMouse;
typedef int CSmrEventType;

// Forward declaration
class CSmrEventsProxy;

class CSmrEvents : public CcThread {
	public:
		CSmrEvents(CSmrEngine* engine);
		virtual void Main(void);

	public:
		//static const float WaitEvent = 50.00f;
		static constexpr float WaitEvent = 50.00f;	// C++11 compliant
	protected:
		dtk_hwnd _window;
		CSmrEngine* _engine;
		
	public:
		CcCallback2<CSmrEventsProxy, CSmrEvents*, CSmrMouse> i_OnMouse;
		CcCallback2<CSmrEventsProxy, CSmrEvents*, CSmrMouse> i_OnMouseMotion;
		CcCallback2<CSmrEventsProxy, CSmrEvents*, CSmrKeyboard> i_OnKeyboard;
		CcCallback1<CSmrEventsProxy, CSmrEvents*> i_OnQuit;
		CcCallback1<CSmrEventsProxy, CSmrEvents*> i_OnRedraw;
};

class CSmrEventsProxy {
	public:
		virtual void HandleMouse(CSmrEvents* caller, CSmrMouse event)  {}
		virtual void HandleMouseMotion(CSmrEvents* caller, CSmrMouse event)  {}
		virtual void HandleKeyboard(CSmrEvents* caller, CSmrKeyboard event) {}
		virtual void HandleQuit(CSmrEvents* caller) {}
		virtual void HandleRedraw(CSmrEvents* caller) {}
};

#define CB_CSmrEvents(sender_event, receiver_ptr, method) 	\
	sender_event.Register((CSmrEventsProxy*)receiver_ptr, 	\
			&CSmrEventsProxy::method);

#endif
