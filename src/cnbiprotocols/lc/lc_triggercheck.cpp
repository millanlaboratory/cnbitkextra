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

#include <cnbicore/CcBasic.hpp>
#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiId.hpp>
#include <cnbiprotocol/CpTriggerFactory.hpp>
#include <cnbismr/CSmrEngineExt.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void usage(void) { 
	printf("Usage: lc_triggercheck -a /acquisition\n");
	exit(1);
}

class TriggerCheck : public CSmrEngineExt {
	public:
		~TriggerCheck(void) { 
			this->Teardown();
		}

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
	CcCore::OpenLogger("lc_triggercheck");
	CcCore::CatchSIGINT();
	CcCore::CatchSIGTERM();
	ClLoop::Configure();

	TriggerCheck check;
	check.Setup();
	check.Open();
	check.SwitchScene("test"); 
	
	// Connect to loop
	CcLogInfo("Connecting to loop...");
	while(ClLoop::Connect() == false) {
		if(CcCore::receivedSIGAny.Get())
			exit(1);
		CcTime::Sleep(2000);
	}
	
	ClTobiId id(ClTobiId::SetOnly);
	IDMessage idm;
	IDSerializerRapid ids(&idm);
	idm.SetDescription("lctriggercheck");
	idm.SetFamilyType(IDMessage::FamilyBiosig);
	idm.SetEvent(0);

	// Attach iD
	if(id.Attach("/acquisition") == false) {
		CcLogFatal("Cannot attach iD");
		goto shutdown;
	}

	CcLogInfo("2x silent for 2000ms, 10x up for 100ms every 250ms");
	for(int i = 0; i < 2; i++) {
		CcTime::Sleep(2000);
		for(int j = 0; j < 10; j++) {
			check.ScreenSync(1);
			idm.SetEvent(1); 
			id.SetMessage(&ids);
			check.Show();
			CcTime::Sleep(100);

			check.ScreenSync(2);
			idm.SetEvent(2); 
			id.SetMessage(&ids);
			check.Hide();
			CcTime::Sleep(250);
		
			if(CcCore::receivedSIGAny.Get())
				goto shutdown;
			if(check.eventkeyboard.Get(0) == DTKK_ESCAPE) 
				goto shutdown;
		}
	}
	
	CcLogInfo("20x silent for 250ms, triggers 30ms every 60-180ms");
	for(int j = 0; j < 20; j++) {
		CcTime::Sleep(250);
		check.ScreenSync(3);
		idm.SetEvent(3); 
		id.SetMessage(&ids);
		check.Show();
		CcTime::Sleep(30);

		check.ScreenSync(4);
		idm.SetEvent(4);
		id.SetMessage(&ids);
		check.Hide();
		CcTime::Sleep(60, 180);
	}
	
shutdown:
	check.Close();
	id.Detach();
	CcCore::Exit(0);
}

