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

#include "mi_utilities.hpp"
#include <cnbicore/CcBasic.hpp>
#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiId.hpp>
#include <cnbismr/CSmrBars.hpp>
#include <cnbismr/CSmrCopilot.hpp>
#include <cnbiconfig/CCfgConfig.hpp>
#include <cnbiprotocol/CpProbability.hpp>
#include <cnbiprotocol/CpTriggerFactory.hpp>
#include <iostream>
#define RESET 15

int main(int argc, char *argv[]) {
	CcCore::OpenLogger("mi_offline");
	CcCore::CatchSIGINT();
	CcCore::CatchSIGTERM();
	ClLoop::Configure();

	// Tools for configuration
	CCfgConfig config;
	CCfgTaskset* taskset = NULL;

	// Parameters for experiment
	std::string xml_modality, xml_block, xml_taskset, xml_file;

	// Tools for feedback
	CpTrigger* trigger = NULL;
	CSmrCopilot copilot;
	CSmrBars* bars = NULL;
	bool waitloop = true;
	
	// Tools for protocol
	dtmi p_dt;
	hwtmi p_hwt;
	gdfmi p_gdf;
	devmi p_gdf_dev;
	CpProbability* probs = NULL;
	HWTrigger* t_hwt = NULL;
	GDFEvent* t_gdf = NULL;

	// Tools for TOBI iD
	ClTobiId id(ClTobiId::SetOnly);
	IDMessage idm;
	IDSerializerRapid ids(&idm);
	idm.SetDescription("mi_offline");
	idm.SetFamilyType(IDMessage::FamilyBiosig);
	idm.SetEvent(0);

	// Tools for TOBI iD on the /bus bus
    ClTobiId tobiIDDev(ClTobiId::SetOnly);
    IDMessage idMessageDev;
    IDSerializerRapid idSerializerDev(&idMessageDev);
    idMessageDev.SetDescription("mi_online_fes");
    idMessageDev.SetFamilyType(IDMessage::FamilyBiosig);
    idMessageDev.SetEvent(0);
	
	// Connect to loop
	CcLogInfo("Connecting to loop...");
	if(ClLoop::Connect() == false) {
		CcLogFatal("Cannot connect to loop");
		goto shutdown;
	}
	
	// Attach iD
	if(id.Attach("/bus") == false) {
		CcLogFatal("Cannot attach iD");
		goto shutdown;
	}

	// Attach iD for fesControl to /dev
    if(tobiIDDev.Attach("/dev") == false) {
        CcLogFatal("Cannot attach iD to /dev");
        goto shutdown;
    }

	// Get configuration from nameserver
	xml_modality = ClLoop::nms.RetrieveConfig("mi", "modality");
	xml_block    = ClLoop::nms.RetrieveConfig("mi", "block");
	xml_taskset  = ClLoop::nms.RetrieveConfig("mi", "taskset");
	xml_file     = ClLoop::nms.RetrieveConfig("mi", "xml");
	CcLogConfigS("Modality=" << xml_modality <<
			", Block=" << xml_block <<
			", Taskset=" << xml_taskset << 
			", Configuration=" << xml_file);

	// Import XML file
	try {
		config.ImportFileEx(xml_file);
	} catch(XMLException e) {
		CcLogException(e.Info());
		goto shutdown;
	}

	// Configure CpTrigger using XML
	trigger = CpTriggerFactory::Find(&config);
	if(trigger == NULL) {
		CcLogFatal("Trigger not available");
		goto shutdown;
	}

	// Configure CSmrBars using XML
	bars = new CSmrBars(&config, trigger);

	/* Configuration block 
	 * - Configure the copilot
	 * - Get the appropriate taskset
	 * - Extract the protocol p_dt
	 * - Extract the protocol t_hwt and t_gdf
	 */
	if(mi_configure_copilot(&config, &copilot, xml_block, xml_taskset) == false) {
		CcLogFatal("Copilot configuration failed");
		goto shutdown;
	}
	if(mi_get_taskset_offline(&config, &taskset, xml_block, xml_taskset) == false) {
		CcLogFatal("Offline taskset configuration failed");
		goto shutdown;
	}
	if(mi_get_dt(&config, &p_dt) == false) {
		CcLogFatal("Offline timings configuration failed");
		goto shutdown;
	}
	if(mi_get_triggers(taskset, &t_hwt, &t_gdf) == false) {
		CcLogFatal("Offline triggers configuration failed");
		goto shutdown;
	}
	if(mi_get_markers(&config, &p_hwt, &p_gdf) == false) {
		CcLogFatal("Offline markers configuration failed");
		goto shutdown;
	}
	if(mi_get_devmarkers(&config, &p_gdf_dev) == false) {
		CcLogFatal("Online dev markers configuration failed");
		goto shutdown;
	}
	CcLogInfo("Protocol configured");

	/* Finish configuration:
	 * - initialize the control probabilities
	 * - set the number of classes for the bars
	 */
	probs = new CpProbability(copilot.GetClassNumber());
	bars->SetParameters(copilot.GetClassNumber(), copilot.GetThreshold());
	bars->Open();
	
	// Wait for user input 
	bars->SwitchScene("wait");
	for(float progress = 0.0; progress <= 1.00f; progress += 0.1) {
		bars->ShowWait(progress);
		CcTime::Sleep(50.00f);
	}
	
	// Wait for user input 
	while(waitloop) {
		bars->ShowWait(1.00f);

		if(ClLoop::IsConnected() == false) {
			CcLogFatal("Cannot connect to loop");
			goto shutdown;
		}
		if(CcCore::receivedSIGAny.Get()) {
			CcLogWarning("User asked to go down");
			goto shutdown;
		}
		switch(bars->eventkeyboard.Get(0)) {
			case DTKK_ESCAPE:
				CcLogWarning("User asked to quit");
				goto shutdown;
				break;
			case DTKK_RETURN:
			case DTKK_SPACE:
				waitloop = false;
				break;
			default:
				CcTime::Sleep(200.00f);
				break;
		}
	}
	bars->SwitchScene("feedback");
	CcTime::Sleep(p_dt.wait);
	unsigned int idx_trial, idx_class, idx_step;
	for(idx_trial = 0; idx_trial < copilot.GetTrialNumber(); idx_trial++) {
		idx_class = copilot.InitializeEx(idx_trial);
		CcLogInfoS("Trial=" << idx_trial << "/" << copilot.GetTrialNumber() << 
				", Id=" << idx_class << 
				", GDF=" << t_gdf[idx_class]);

		/* Reset bars */
		bars->Control(probs->Priors());

		/* Wait */
		//system("speech \"Pause\"");
		system("canberra-gtk-play -f /home/cnbi/Music/Pause.wav");
		bars->ScreenSync(p_hwt.wait);
		idm.SetEvent(p_gdf.wait);
		id.SetMessage(&ids);
		bars->FixationCue(CSmrBars::DrawNone);
		CcTime::Sleep(p_dt.waitmin, p_dt.waitmax); 
		idm.SetEvent(p_gdf.wait + p_gdf.off);
		id.SetMessage(&ids);
		
		/* Fixation */
		bars->ScreenSync(p_hwt.fixation);
		idm.SetEvent(p_gdf.fixation);
		id.SetMessage(&ids);
		bars->FixationCue(CSmrBars::DrawFixation);
		//system("speech \"Prêt\"");
		system("canberra-gtk-play -f /home/cnbi/Music/Pret.wav");
		CcTime::Sleep(p_dt.fixation);
		idm.SetEvent(p_gdf.fixation + p_gdf.off);
		id.SetMessage(&ids);
		
		/* Cue */
		bars->ScreenSync(t_hwt[idx_class]);
		idm.SetEvent(t_gdf[idx_class]);
		id.SetMessage(&ids);
		bars->FixationCue(CSmrBars::DrawCue, idx_class);
		if (idx_class == 0){
			//system("speech \"Bougez\"");
			system("canberra-gtk-play -f /home/cnbi/Music/Bougez.wav");
		}
		else{
			//system("speech \"Repos\"");
			system("canberra-gtk-play -f /home/cnbi/Music/Repos.wav");
		}
		CcTime::Sleep(p_dt.cue);
		idm.SetEvent(t_gdf[idx_class] + p_gdf.off);
		id.SetMessage(&ids);

		/* Continuous feedback */
		bars->ScreenSync(p_hwt.cfeedback);
		idm.SetEvent(p_gdf.cfeedback);
		id.SetMessage(&ids);
		CcLogInfo("Starting trial");

		bool singleStimulationOn = false;
		bool doubleStimulationOn = false;

		/* Trial loop */
		for(idx_step = 0; idx_step < copilot.GetStepNumber(); idx_step++) {
			if(t_gdf[idx_class] == 782 || t_gdf[idx_class] == 784){
				if(idx_step > copilot.GetStepNumber() / 4 && !singleStimulationOn){
					idMessageDev.SetEvent(p_gdf_dev.halfcorrect);
					tobiIDDev.SetMessage(&idSerializerDev);
					singleStimulationOn = true;
				}
				else if(idx_step > copilot.GetStepNumber() * 1 / 2 && !doubleStimulationOn){
					idMessageDev.SetEvent(p_gdf_dev.threequartercorrect);
					tobiIDDev.SetMessage(&idSerializerDev);
					doubleStimulationOn = true;
				}
			}
			
			bool boom = copilot.GetControlValuesEx(idx_step, probs, idx_class);
			bars->Control(probs, idx_class, boom);
			if(boom) {
				bars->ScreenSync(p_hwt.off);
				idm.SetEvent(p_gdf.cfeedback + p_gdf.off);
				id.SetMessage(&ids);
				// Reset stimulation
				idMessageDev.SetEvent(RESET);
                tobiIDDev.SetMessage(&idSerializerDev);
				bars->FixationCue(CSmrBars::DrawFixation);
			}
			CcTime::Sleep(1000.00/copilot.GetControlGranularity());
		}
		CcLogInfo("Checking loop");
		CcTime::Sleep(p_dt.boom);
			
		/* Handbrake */
		if(ClLoop::IsConnected() == false) {
			CcLogFatal("Cannot connect to loop");
			goto shutdown;
		}
		if(CcCore::receivedSIGAny.Get()) {
			CcLogWarning("User asked to go down");
			goto shutdown;
		}
		if(bars->eventkeyboard.Get(0) == DTKK_ESCAPE) {
			CcLogWarning("User asked to quit");
			goto shutdown;
		}
		CcLogInfo("Trial done");
	}
shutdown:
	id.Detach();
	tobiIDDev.Detach();
	if(bars != NULL) {
		if(bars->IsOpen())
			bars->Close();
		delete bars;
	}

	if(taskset != NULL)
		delete(taskset);
	if(trigger != NULL)
		delete(trigger);
	if(probs != NULL)
		delete(probs);
	if(t_hwt != NULL)
		delete(t_hwt);
	if(t_gdf != NULL)
		delete(t_gdf);
	CcCore::Exit(0);
}
