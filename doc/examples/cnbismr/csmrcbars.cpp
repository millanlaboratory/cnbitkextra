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

#include "CSmrCBars.hpp"
#include "CSmrCopilot.hpp"
#include <cnbiprotocol/CpTriggerFactory.hpp>

int main(void) {
	unsigned int ex_dt_before_cue = 2000;	// Milliseconds
	unsigned int ex_dt_after_cue = 2000;	// Milliseconds
	unsigned int ex_dt_begin = 1000;		// Milliseconds
	unsigned int ex_dt_end = 1000;			// Milliseconds
	unsigned int ex_dt_trial = 5000;		// Milliseconds

	unsigned int trials[4] = {1, 1, 1, 1};
	CSmrCopilot copilot;
	copilot.SetClassNumber(4);
	copilot.SetTrialNumber(trials);
	copilot.SetTrialDuration(ex_dt_trial);
	copilot.SetControlGranularity(16);
	copilot.SetThreshold(0.70f);
	copilot.SetCoefficientViscosity(0.05f);

	CSmrCBars* feedback = new CSmrCBars(800, 600, CpTriggerFactory::Find());
	feedback->SetParameters(copilot.GetClassNumber(), 
			copilot.GetThreshold());
	feedback->SetThrottle(2.20f);
	feedback->SetPosition(0.00f, 0.00f);
	feedback->Setup();
	//feedback->Start();
	feedback->Open();
	
	feedback->SwitchScene("wait");
	for(int i = 0; i < 1; i++) {
		for(int i = 1; i < 13; i++) {
			feedback->ShowWait((float)i/12);
			CcTime::Sleep(250.00f);
		}
	}
	feedback->ShowWait(1.00f);
	CcTime::Sleep(2000.00f);

	feedback->SwitchScene("begin");
	CcTime::Sleep(ex_dt_begin);
	feedback->SwitchScene("feedback");
	
	unsigned int sclass = 0;
	CpProbability control(copilot.GetClassNumber());
	feedback->Control(&control);

	for(unsigned int strial = 0; strial < copilot.GetTrialNumber(); strial++) {
		feedback->DisableContinuous();
		feedback->FixationCue(CSmrBars::DrawFixation);
		sclass = copilot.InitializeEx(strial);
		control.Priors();
		feedback->Control(&control);
		CcTime::Sleep(ex_dt_before_cue);
		feedback->ScreenSync(1);
		feedback->FixationCue(CSmrBars::DrawCue, sclass);
		CcTime::Sleep(ex_dt_after_cue);
		
		bool boom = false;
		for(unsigned int s = 0; s < copilot.GetStepNumber(); s++) {
			boom = copilot.GetControlValuesEx(s, &control, sclass);
			feedback->Control(&control, sclass, boom);
			if(s == copilot.GetStepNumber() -1) {
				feedback->FixationCue(CSmrBars::DrawNone);
			}
			CcTime::Sleep(1000/copilot.GetControlGranularity());
		}

		for(int s = 1; s <= 36; s++) {
			feedback->ShowTeaser(s/36.00f);
			CcTime::Sleep(1000.00f/16);
		}
		feedback->ShowTeaser(0.00f);
		for(int sc = 0; sc < 80; sc++) {
			control.Set(sclass, sc * 1.00f/80);
			feedback->ControlContinuous(&control, sclass, sc * 1.00f/80);
			CcTime::Sleep(1000/copilot.GetControlGranularity());
		}
		
		CcTime::Sleep(ex_dt_trial);
	}
	
	feedback->SwitchScene("end");
	CcTime::Sleep(ex_dt_end);

	feedback->Close();
	delete feedback;
	return 0;
}
