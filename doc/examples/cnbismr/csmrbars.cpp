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

#include "CSmrBars.hpp"
#include "CSmrCopilot.hpp"
#include <cnbiprotocol/CpTriggerFactory.hpp>
#include <cnbiprotocol/CpProbability.hpp>

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
	copilot.SetThreshold(0.875f);
	copilot.SetCoefficientViscosity(0.05f);

	CSmrBars* feedback = new CSmrBars(800, 600, CpTriggerFactory::Find());
	feedback->SetParameters(copilot.GetClassNumber(), copilot.GetThreshold());
	
	/*
	float th[4] = {0.3, 0.6, 0.9, 1.0};
	feedback->SetParameters2(copilot.GetClassNumber(), th);
	feedback->Open();
	*/

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
	
	unsigned int sclass;
	CpProbability control(copilot.GetClassNumber());
	control.Priors();
	
	for(unsigned int strial = 0; strial < copilot.GetTrialNumber(); strial++) {
		feedback->FixationCue(CSmrBars::DrawFixation);

		sclass = copilot.InitializeEx(strial);
		control.Priors();
		feedback->Control(&control);
		CcTime::Sleep(ex_dt_before_cue);
		feedback->ScreenSync(1);
		feedback->FixationCue(CSmrBars::DrawCue, sclass);
		CcTime::Sleep(ex_dt_after_cue);
		
		bool targethit = false;
		for(unsigned int s = 0; s < copilot.GetStepNumber(); s++) {
			targethit = copilot.GetControlValuesEx(s, &control, sclass);
			feedback->Control(&control, sclass, targethit);
			if(s == copilot.GetStepNumber() -1) 
				feedback->FixationCue(CSmrBars::DrawFixation);
			CcTime::Sleep(1000/copilot.GetControlGranularity());
			if(feedback->eventkeyboard.Get(0) == DTKK_ESCAPE) {
				std::cout << "Going down..." << std::endl;
				break;
			}
		}
		CcTime::Sleep(ex_dt_trial);
	}
	
	feedback->SwitchScene("end");
	CcTime::Sleep(ex_dt_end);
	
	feedback->Close();
	delete feedback;
	return 0;
}
