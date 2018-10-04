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

#include "CSmrTrigger.hpp"
#include "CSmrEngine.hpp"
#include "CSmrEvents.hpp"
#include <cnbicore/CcTime.hpp>
#include <cnbiprotocol/CpTriggerFactory.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;


class FeedbackTest : public CSmrEngine, public CSmrEventsProxy {
	public:
		void Setup(void) {
			CSmrEngine::AddScene("test");
			this->_rgb0[0] = 1.00f;
			this->_rgb0[1] = 1.00f;
			this->_rgb0[2] = 1.00f;
			this->_rgb0[3] = 1.00f;
			
			this->_rgb1[0] = 0.00f;
			this->_rgb1[1] = 0.00f;
			this->_rgb1[2] = 0.00f;
			this->_rgb1[3] = 0.00f;
				
			this->DataWait();
			this->_rect = dtk_create_rectangle_2p(NULL, 
					-1.00, -1.10, 
					+1.00, +1.10, 
					1, _rgb1);
			this->DataPost();
		
			CSmrEngine::AddShape("test",  "rectangle00", this->_rect);
		};

		void Teardown(void) {
			CSmrEngine::DestroyScene("test");
		};

		void Show(void) {
			CSmrEngine::DataWait();
			this->_rect = dtk_create_rectangle_2p(this->_rect, 
					-1.00, -1.10, 
					+1.00, +1.10, 
					1, _rgb0);
			CSmrEngine::DataPost();
			CSmrEngine::RequestUpdate();
		};

		void Hide(void) {
			CSmrEngine::DataWait();
			this->_rect = dtk_create_rectangle_2p(this->_rect, 
					-1.00, -1.10, 
					+1.00, +1.10, 
					1, _rgb1);
			CSmrEngine::DataPost();
			CSmrEngine::RequestUpdate();
		};

		/* 
		 * CSmrEngineProxy stuff
		 */
		virtual void HandleKeyboard(CSmrEvents* caller, 
				CSmrKeyboard event)  {
			cout << "Event::Keyboard" << endl;
			switch(event.sym) {
				case DTKK_ESCAPE:
					cout << "  Escape!" << endl;
					break;
				case DTKK_a:
					cout << "  a!" << endl;
					break;
				case DTKK_BACKSPACE:
					cout << "  Backspace!" << endl;
					break;
			}
		}
		
		virtual void HandleMouse(CSmrEvents* caller, CSmrMouse event) {
			cout << "Event::Mouse ";
			printf("Button %d was %s\n",
					event.button,
					event.state	== DTK_KEY_PRESSED ? "pressed" : "released");
		}
		
		virtual void HandleMouseMotion(CSmrEvents* caller, CSmrMouse event) {
			cout << "Event::MouseMotion ";
			printf("x,y = %d,%d\n", event.x, event.y);
		}
		
		virtual void HandleQuit(CSmrEvents* caller) {
			cout << "Event::Quit" << endl;
		}
		
		virtual void HandleRedraw(CSmrEvents* caller) {
			cout << "Event::Redraw" << endl;
		}

	private:
		dtk_hshape _rect;
		float _rgb0[4];
		float _rgb1[4];
};

int main(void) {
	FeedbackTest feedback;
	CpTrigger *trigger = CpTriggerFactory::Find();
	CSmrTrigger fbtrigger(&feedback, trigger);
	CSmrEvents* fbevents = new CSmrEvents(&feedback);
	
	CB_CSmrEvents(fbevents->i_OnKeyboard, 	 &feedback, HandleKeyboard);
	CB_CSmrEvents(fbevents->i_OnMouse,    	 &feedback, HandleMouse);
	CB_CSmrEvents(fbevents->i_OnMouseMotion, &feedback, HandleMouseMotion);
	CB_CSmrEvents(fbevents->i_OnQuit,     	 &feedback, HandleQuit);
	CB_CSmrEvents(fbevents->i_OnRedraw,   	 &feedback, HandleRedraw);

	feedback.Setup();
	feedback.Open();
	fbevents->Start();
	
	feedback.SwitchScene("test"); 

	cout << "[csmrevents] 100 cycles: up for 200ms every 400ms\n";
	CcTime::Sleep(5000);
	for(int j = 0; j < 10; j++) {
		fbtrigger.Enable(1);
		feedback.Show();
		CcTime::Sleep(200);

		fbtrigger.Enable(1);
		feedback.Hide();
		CcTime::Sleep(400);
	}

	cout << "[csmrevents] Done\n";
	fbevents->Stop();
	feedback.Close();
	feedback.Teardown();

	return 0;
}
