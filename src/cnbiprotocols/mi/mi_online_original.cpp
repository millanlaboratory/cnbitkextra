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
#include <cnbiloop/ClTobiIc.hpp>
#include <cnbiloop/ClTobiId.hpp>
#include <cnbismr/CSmrBars.hpp>
#include <cnbiconfig/CCfgConfig.hpp>
#include <cnbiprotocol/CpProbability.hpp>
#include <cnbiprotocol/CpTriggerFactory.hpp>
#include <cnbiprotocol/CpTrials.hpp>
#include <iostream>
#include <getopt.h>

void usage(void) { 
	printf("Usage: mi_online [OPTION]\n\n");
	printf("  -s       Run synchronous\n");
	printf("  -a       Run asynchronous\n");
	printf("  -h       display this help and exit\n");
	CcCore::Exit(1);
}

int main(int argc, char *argv[]) {
	int opt;
	bool asasync = false;
	
	while((opt = getopt(argc, argv, "nh")) != -1) {
		if(opt == 's') {
			asasync = false;
		} else if(opt == 'a') {
			asasync = true;
		} else {
			usage();
			CcCore::Exit(opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
		}
	}

	CcCore::OpenLogger("mi_online");
	CcCore::CatchSIGINT();
	CcCore::CatchSIGTERM();
	ClLoop::Configure();

	CcLogInfoS("Running " << (asasync ? "synchronous" : "asynchronous"));

	// Tools for configuration
	CCfgConfig config;
	CCfgTaskset* taskset = NULL;
	CCfgTasksetConstIt tit;

	// Parameters for experiment
	std::string xml_modality, xml_block, xml_taskset, xml_file;
	
	// Tools for feedback
	CpTrigger* trigger = NULL;
	CSmrBars* bars = NULL;

	// Tools for protocol
	dtmi p_dt;
	hwtmi p_hwt;
	gdfmi p_gdf;
	CpProbability* probs = NULL;
	HWTrigger* t_hwt = NULL;
	GDFEvent* t_gdf = NULL;
	CpTrials trials;
	float* thresholds = new float[4];
	
	// Tools for TOBI iD on the /bus bus
	ClTobiId id(ClTobiId::SetOnly);
	IDMessage idm;
	IDSerializerRapid ids(&idm);
	idm.SetDescription("mi_online");
	idm.SetFamilyType(IDMessage::FamilyBiosig);
	idm.SetEvent(0);
	int fidx = TCBlock::BlockIdxUnset;

	// Tools for TOBI iD on the /dev bus
	ClTobiId id2(ClTobiId::SetOnly);
	devmi p_dev;
	
	// Tools for TOBI iC
	ClTobiIc ic(ClTobiIc::GetOnly);
	ICMessage icm;
	ICSerializerRapid ics(&icm);
	ICClassifier* icc = NULL;
	ICSetClassConstIter icit;
	bool waitflow = true, waitic = true;
	float progress = 0.00f;
	
	// Connect to loop
	CcLogInfo("Connecting to loop...");
	if(ClLoop::Connect() == false) {
		CcLogFatal("Cannot connect to loop");
		goto shutdown;
	}
	
	// Attach iD to /bus
	if(id.Attach("/bus") == false) {
		CcLogFatal("Cannot attach iD to /bus");
		goto shutdown;
	}
	
	// Attach iD2 to /dev
	if(id2.Attach("/dev") == false) {
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
	

	/* Configuration block 
	 * - Get the appropriate taskset
	 * - Extract the protocol p_dt
	 * - Extract the protocol t_hwt and t_gdf
	 */
	if(mi_get_taskset_online(&config, &taskset, xml_block, xml_taskset) == false) {
		CcLogFatal("Online taskset configuration failed");
		goto shutdown;
	}
	if(mi_get_dt(&config, &p_dt) == false) {
		CcLogFatal("Online timings configuration failed");
		goto shutdown;
	}
	if(mi_get_triggers(taskset, &t_hwt, &t_gdf) == false) {
		CcLogFatal("Online triggers configuration failed");
		goto shutdown;
	}
	if(mi_get_markers(&config, &p_hwt, &p_gdf) == false) {
		CcLogFatal("Online markers configuration failed");
		goto shutdown;
	}
	if(mi_get_devmarkers(&config, &p_dev) == false) {
		CcLogFatal("Online markers configuration failed");
		goto shutdown;
	}
	if(mi_configure_trials(taskset, &trials) == false) {
		CcLogFatal("Online trials configuration failed");
		goto shutdown;
	}
	if(mi_get_thresholds(taskset, thresholds) == false) {
		CcLogFatal("Online thresholds configuration failed");
		goto shutdown;
	}
	CcLogInfo("Protocol configured");
	
	// Attach iC
	if(ic.Attach(taskset->ndf.ic) == false) {
		CcLogFatal("Cannot attach iC");
		goto shutdown;
	}
	
	/* Configure CSmrBars using XML
	 * Finish configuration:
	 * - initialize the control probabilities
	 * - set the number of classes for the bars
	 */
	bars = new CSmrBars(&config, trigger);
	bars->SetParameters2(taskset->Count(), thresholds);
	bars->Open();

	/* Wait for iC flow 
	 */
	bars->SwitchScene("wait");
	while(waitflow == true) {
		if(waitic == true) {
			if(progress <= 1.00f)
				progress += 0.10f;
			else
				progress = 0.00f;
		} else {
			progress = 1.00f;
		}
		bars->ShowWait(progress);
		
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
				if(waitic == false)
					waitflow = false;
				break;
			default:
				break;
		}
		
		if(waitic == true) {
			switch(ic.GetMessage(&ics)) {
				case ClTobiIc::Detached:
					CcLogFatal("iC detached");
					goto shutdown;
				case ClTobiIc::HasMessage:
					CcLogInfo("iC message received");
					waitic = false;
					break;
				default:
				case ClTobiIc::NoMessage:
					break;
			}
		}
		
		if(waitflow == true)
			CcTime::Sleep(200.00f);
	}
	bars->SwitchScene("feedback");
	CcTime::Sleep(p_dt.wait);

	/* Verify incoming iC message
	 * - check if the message has the same classifier specified in the taskset
	 * - this is useful to check weather we are attached in the write place
	 * - ... or to verify is some iC client managed to attach to the wrong place
	 */
	try { 
		icc = icm.GetClassifier(taskset->classifier.id);
		CcLogConfigS("iC message verified: '" << taskset->classifier.id << 
				"' classifier found"); 
	} catch(TCException e) {
		CcLogFatalS("Wrong iC messsage: '" << taskset->classifier.id << 
				"' classifier missing");
		goto shutdown;
	}

	/* Perform the mapping between GDF labels and tasket IDs
	 * - iC does not provide a direct mapping method (aka floating labels)
	 * - we do it looping trough out the whole taskset
	 * - ... getting a task
	 * - ... checking if the classifier has that task
	 * - ... and finally performing the mapping
	 * This is the most boring part with iC mappings. Still, I have no fucking 
	 * will of fighting to have an extra field with TOBI's WP8. Plus, after
	 * fighting, I have to maintain it, so screw it. We handle it here that is
	 * more awesome. Plus, also the MEX interfaces support the very same mapping
	 * tricks.
	 */
	probs = new CpProbability(&icm, taskset->classifier.id);
	for(tit = taskset->Begin(); tit != taskset->End(); tit++) {
		CCfgTask* task = tit->second;
		if(icc->classes.Has(task->gdf) == false) {
			CcLogFatalS("Taskset class " << task->gdf << " not found in iC message");
			goto shutdown;
		}
		CcLogConfigS("Mapping taskset/iC class GDF=" << task->gdf 
				<< " '" << task->description << "'" 
				<< " as " << task->id);
		probs->MapICClass(task->gdf, task->id);	
	}
	
	// Start /dev 
	idm.SetEvent(p_dev.start);
	id2.SetMessage(&ids);

	unsigned int idx_trial, idx_class, idx_hit;
	for(idx_trial = 0; idx_trial < trials.Total(); idx_trial++) {
		idx_class = trials.At(idx_trial);
		
		CcLogInfoS("Trial=" << idx_trial << "/" << trials.Total() << 
				", Id=" << idx_class << 
				", GDF=" << t_gdf[idx_class]);


		/* Reset bars */
		bars->Control(probs->Priors());

		/* Wait */
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
		CcTime::Sleep(p_dt.fixation);
		idm.SetEvent(p_gdf.fixation + p_gdf.off);
		id.SetMessage(&ids);
		
		/* Cue */
		bars->ScreenSync(t_hwt[idx_class]);
		idm.SetEvent(t_gdf[idx_class]);
		id.SetMessage(&ids);
		bars->FixationCue(CSmrBars::DrawCue, idx_class);
		CcTime::Sleep(p_dt.cue);
		idm.SetEvent(t_gdf[idx_class] + p_gdf.off);
		id.SetMessage(&ids);
		
		/* Continuous feedback */
		bars->ScreenSync(p_hwt.cfeedback);
		idm.SetEvent(p_gdf.cfeedback);
		id.SetMessage(&ids, TCBlock::BlockIdxUnset, &fidx);
			
		// Resume /dev 
		idm.SetEvent(p_dev.resume);
		id2.SetMessage(&ids);

		/* Continuous feedback
		 * - Cosume all the messages 
		 * - Enter main loop 
		 * - Dispatch events on /bus and /dev
		 */
		while(ic.WaitMessage(&ics) == ClTobiIc::HasMessage);
		while(true) {
			while(true) { 
				switch(ic.WaitMessage(&ics)) {
					case ClTobiIc::Detached:
						CcLogFatal("iC detached");
						goto shutdown;
						break;
					case ClTobiIc::NoMessage:
						continue;
						break;
				}
				if(icm.GetBlockIdx() > fidx) {
					CcLogDebugS("Sync iD/iC : " << fidx << "/" << icm.GetBlockIdx());
					break;
				}
			}
			probs->Update(icc);

			if(probs->Max(&idx_hit) >= thresholds[idx_hit]) {
				idm.SetEvent(p_gdf.cfeedback + p_gdf.off);
				id.SetMessage(&ids);

				probs->Hard(idx_hit);
				if(idx_hit == idx_class) {
					bars->ScreenSync(p_hwt.hit);
					idm.SetEvent(p_gdf.hit);
				} else {
					bars->ScreenSync(p_hwt.miss);
					idm.SetEvent(p_gdf.miss);
				}
				bars->Control(probs, idx_hit, true);
				bars->FixationCue(CSmrBars::DrawFixation);
				id.SetMessage(&ids);

				// Right/Left commands on /dev
				idm.SetEvent(idx_hit == 0 ? p_dev.right : p_dev.left);
				id2.SetMessage(&ids);
		
				CcTime::Sleep(p_dt.boom);
				if(idx_hit == idx_class) {
					idm.SetEvent(p_gdf.hit + p_gdf.off);
				} else {
					idm.SetEvent(p_gdf.miss + p_gdf.off);
				}
				id.SetMessage(&ids);
				break;
			} else {
				bars->Control(probs, idx_class, false);
			}
		}
		
		// Suspend /dev 
		idm.SetEvent(p_dev.suspend);
		id2.SetMessage(&ids);
			
		/* Handbrake */
		if(ClLoop::IsConnected() == false) {
			CcLogFatal("Cannot connect to loop");
			goto shutdown;
		}
		if(bars->eventkeyboard.Get(0) == DTKK_ESCAPE) {
			CcLogWarning("User asked to quit");
			goto shutdown;
		}
		if(CcCore::receivedSIGAny.Get()) {
			CcLogWarning("User asked to go down");
			goto shutdown;
		}

	}

shutdown:
	idm.SetEvent(p_dev.stop);
	id2.SetMessage(&ids);

	id.Detach();
	id2.Detach();
	
	if(bars != NULL) {
		if(bars->IsOpen()) {
			bars->Close();
		}
		delete(bars);
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
