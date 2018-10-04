#include <iostream>
#include <getopt.h>

#include <cnbicore/CcBasic.hpp>
#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiIc.hpp>
#include <cnbiloop/ClTobiId.hpp>
#include <cnbismr/CSmrBars.hpp>
#include <cnbiconfig/CCfgConfig.hpp>
#include <cnbiprotocol/CpProbability.hpp>
#include <cnbiprotocol/CpTriggerFactory.hpp>
#include <cnbiprotocol/cp_utilities.hpp>

#include "mi_utilities.hpp"
#include "artifact_utilities.hpp"

void usage(void) { 
	printf("Usage: mi_control [OPTION]\n\n");
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

	CcCore::OpenLogger("mi_control");
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
	artcfg_t artcfg;		// New config structure for artifacts
	GDFEvent artevent;
	bool artifact = false;
	CpProbability* probs = NULL;
	HWTrigger* t_hwt = NULL;
	GDFEvent* t_gdf = NULL;
	float* thresholds = new float[4];

	// Tools for device
	devmi p_dev;

	// Temporary variables
	float tmp_step = 0.01f;

	// Tools for TOBI iD on the /bus bus
	ClTobiId id(ClTobiId::SetGet);
	IDMessage idm;
	IDSerializerRapid ids(&idm);
	idm.SetDescription("mi_control");
	idm.SetFamilyType(IDMessage::FamilyBiosig);
	idm.SetEvent(0);
	int fidx = TCBlock::BlockIdxUnset;

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
	 * - Get the appropriate taskset
	 * - Extract the protocol p_dt
	 * - Extract the protocol t_hwt and t_gdf
	 */
	if(mi_get_taskset_online(&config, &taskset, xml_block, xml_taskset) == false) {
		CcLogFatal("Control taskset configuration failed");
		goto shutdown;
	}
	if(mi_get_dt(&config, &p_dt) == false) {
		CcLogFatal("Control timings configuration failed");
		goto shutdown;
	}
	if(mi_get_triggers(taskset, &t_hwt, &t_gdf) == false) {
		CcLogFatal("Control triggers configuration failed");
		goto shutdown;
	}
	if(mi_get_markers(&config, &p_hwt, &p_gdf) == false) {
		CcLogFatal("Control markers configuration failed");
		goto shutdown;
	}
	if(mi_get_devmarkers(&config, &p_dev) == false) {
		CcLogFatal("Control markers configuration failed");
		goto shutdown;
	}
	if(mi_get_thresholds(taskset, thresholds) == false) {
		CcLogFatal("Control thresholds configuration failed");
		goto shutdown;
	}
	if(artifact_configure_events(&config, &artcfg) == false) {
		CcLogWarning("Artifacts structure not configured");
	}
	CcLogInfo("Protocol configured");
	// Attach iC
	if(ic.Attach(taskset->ndf.ic) == false) {
		CcLogFatal("Cannot attach iC");
		goto shutdown;
	}
	
	
	/* Finish configuration:
	 * - initialize the control probabilities
	 * - set the number of classes for the bars
	 */
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
	
	unsigned int idx_hit;
	while(true) {

		/* Reset bars */
		bars->Control(probs->Priors());
		bars->FixationCue(CSmrBars::DrawFixation);
		
		if (artifact == true)
			bars->ShowBlock("Artifact");

		/* Continuous feedback */
		bars->ScreenSync(p_hwt.cfeedback);
		idm.SetEvent(p_gdf.cfeedback);
		id.SetMessage(&ids, TCBlock::BlockIdxUnset, &fidx);
			
		/* Continuous feedback
		 * - Cosume all the messages 
		 * - Enter main loop 
		 * - Dispatch events on /bus 
		 */
		while(ic.WaitMessage(&ics) == ClTobiIc::HasMessage);
		while(true) {

			if(id.GetMessage(&ids) == true) {
				artevent = idm.GetEvent();
				if(artevent == artcfg.on.gdfevent) {
					bars->ShowBlock("Artifact");
					CcLogInfoS("Artifact detection: on (event|"<<artcfg.on.gdfevent<<")"); 
					artifact = true;
				} else if(artevent == artcfg.off.gdfevent) {
					bars->FixationCue(CSmrBars::DrawFixation);
					CcLogInfoS("Artifact detection: off (event|"<<artcfg.off.gdfevent<<")"); 
					artifact = false;
				}
			}
			
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

				probs->Hard(idx_hit);
				// mi_control send the corresponding task event
				// + the device mask
				idm.SetEvent(t_gdf[idx_hit] + p_dev.device);
				
				bars->Control(probs, idx_hit, true);
				bars->FixationCue(CSmrBars::DrawFixation);
				
				if (artifact == true)
					bars->ShowBlock("Artifact");

				id.SetMessage(&ids);
				CcTime::Sleep(p_dt.boom);
				
				break;
			} else {
				bars->Control(probs, idx_hit, false);
				bars->FixationCue(CSmrBars::DrawFixation);
				
				if (artifact == true)
					bars->ShowBlock("Artifact");
			}
		
			// Update dynamically thresholds if changed
			for(auto it = taskset->Begin(); it != taskset->End(); it++) {
				auto id = it->second->id; 
				auto task = it->second->name;
				if(cp_parameter_update("mi", task, "threshold", &thresholds[id])) {
					bars->UpdateThreshold(thresholds);
					CcLogConfigS("Threshold changed to "<< thresholds[id]<<" for mi|" << task);
				}
			}
					
		}
		
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

	id.Detach();
	
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
