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
#include "CSmrEngineExt.hpp"
#include <cnbicore/CcTime.hpp>
#include <cnbiprotocol/CpTriggerFactory.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;

class FeedbackTest : public CSmrEngineExt {
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

	private:
		dtk_hshape _rect;
		float _rgb0[4];
		float _rgb1[4];
};

int main(void) {
	CcCore::OpenLogger("csmr_triggercheck");
	FeedbackTest feedback;
	CpTrigger *trigger = CpTriggerFactory::Find();
	CSmrTrigger fbtrigger(&feedback, trigger);

	feedback.Setup();
	feedback.Open();
	
	feedback.SwitchScene("test"); 

	cout << "[csmr_triggercheck] 2x silent for 2000ms, 10x up for 100ms every 250ms\n";
	for(int i = 0; i < 2; i++) {
		CcTime::Sleep(2000);
		for(int j = 0; j < 10; j++) {
			feedback.ScreenSync(1);
			feedback.Show();
			CcTime::Sleep(100);

			feedback.ScreenSync(2);
			feedback.Hide();
			CcTime::Sleep(250);
		}
	}

	cout << "[csmr_triggercheck] Done\n";
	feedback.Close();
	feedback.Teardown();

	CcCore::Exit(0);
}
