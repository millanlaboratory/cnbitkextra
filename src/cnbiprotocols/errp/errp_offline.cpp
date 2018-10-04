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

#include "errp_utilities.hpp"
#include "ErrpSquares.hpp"
#include <cnbicore/CcBasic.hpp>
#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiId.hpp>
#include <cnbiconfig/CCfgConfig.hpp>
#include <cnbiprotocol/CpTriggerFactory.hpp>
#include <getopt.h>

void usage(void) { 
	printf("Usage: errp_offline [OPTION]\n\n");
	printf("  -n       Run online\n");
	printf("  -h       display this help and exit\n");
	CcCore::Exit(1);
}

int main(int argc, char *argv[]) {
	int opt;
	std::string binname = "errp", modality = "offline";
	bool asonline = false;
		
	while((opt = getopt(argc, argv, "nh")) != -1) {
		if(opt == 'n') {
			asonline = true;
			binname = "errp";
			modality = "offline";
		} else {
			usage();
			CcCore::Exit(opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
		}
	}

	CcCore::OpenLogger(binname);
	CcCore::CatchSIGINT();
	CcCore::CatchSIGTERM();
	ClLoop::Configure();

	CcLogConfigS("Running " << (asonline == true ? "online" : "offline"));

	// Tools for configuration
	CCfgConfig config;
	CCfgTaskset* taskset = NULL;
	
	// Parameters for experiment
	std::string xml_modality, xml_block, xml_taskset, xml_file;

	// Tools for feedback
	CpTrigger* trigger = NULL; 
	ErrpSquares* squares = NULL;
	CpDynamicP2 dynamic;
	std::vector<int> trials;
	unsigned int nsquares;
	
	// Tools for protocol
	dterrp p_dt;
	hwterrp p_hwt;
	gdferrp p_gdf;
	HWTrigger* t_hwt = NULL;
	GDFEvent* t_gdf = NULL;
	
	/* Tools for TOBI iD
	 * - we will receive 779 from the classifier
	 */
	ClTobiId id(ClTobiId::SetGet);
	IDMessage idm;
	IDSerializerRapid ids(&idm);
	idm.SetDescription(binname);
	idm.SetFamilyType(IDMessage::FamilyBiosig);
	idm.SetEvent(0);

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

	// Get configuration from nameserver
	xml_modality = ClLoop::nms.RetrieveConfig(binname, "modality");
	xml_block    = ClLoop::nms.RetrieveConfig(binname, "block");
	xml_taskset  = ClLoop::nms.RetrieveConfig(binname, "taskset");
	xml_file     = ClLoop::nms.RetrieveConfig(binname, "xml");
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
	squares = new ErrpSquares(&config, trigger);
	
	/* Configuration block 
	 * - Get a trial list for CpDynamicP2 
	 * - Extract the protocol p_dt
	 * - Extract the protocol t_hwt and t_gdf
	 */
	if(errp_configure_dynamic(&config, xml_block, xml_taskset, modality,
				&trials, &nsquares) == false) {
		CcLogFatal("Dynamic configuration failed");
		goto shutdown;
	}
	if(errp_get_dt(&config, &p_dt) == false) {
		CcLogFatal("Timing configuration failed");
		goto shutdown;
	}
	if(errp_get_markers(&config, &p_hwt, &p_gdf) == false) {
		CcLogFatal("Markers configuration failed");
		goto shutdown;
	}
	if(errp_get_taskset(&config, &taskset, xml_block, xml_taskset, asonline) == false) {
		CcLogFatal("Taskset configuration failed");
		goto shutdown;
	}
	if(errp_get_triggers(taskset, &t_hwt, &t_gdf) == false) {
		CcLogFatal("Triggers configuration failed");
		goto shutdown;
	}
	
	CcLogInfo("Protocol configured");

	/* Finish configuration:
	 * - set the number of squares for the dancing robot
	 */
	squares->SetSquares(nsquares);
	squares->Open();

	squares->SwitchScene("begin");
	CcTime::Sleep(p_dt.begin);
	squares->SwitchScene("feedback");

	int direction;
	unsigned int errors;

	for(unsigned int t = 0; t < trials.size(); t++) {
		errors = errp_get_dynaerrors(trials[t]);
		direction = errp_get_dynadirection(trials[t]);
		dynamic.Scramble(nsquares, errors, direction);

		CcLogInfoS("Trial=" << t + 1 << "/" << trials.size() << 
				", Direction=" << direction << ", Errors=" << errors);
		
		/* Reset squares & trial on */
		squares->ScreenSync(p_hwt.trial);
		idm.SetEvent(p_gdf.trial);
		id.SetMessage(&ids);
		squares->PresentSquares(ErrpSquares::NoDraw, ErrpSquares::NoDraw);
		
		/* Wait */
		squares->FixationOff();
		CcTime::Sleep(p_dt.waitmin, p_dt.waitmax); 

		/* Fixation */
		squares->ScreenSync(p_hwt.fixation);
		idm.SetEvent(p_gdf.fixation);
		id.SetMessage(&ids);
		squares->FixationOn();
		CcTime::Sleep(p_dt.fixation);

		/* Cue */
		squares->ScreenSync(direction > 0 ? p_hwt.right : p_hwt.left);
		idm.SetEvent(direction > 0 ? p_gdf.right : p_gdf.left);
		id.SetMessage(&ids);
		squares->PresentSquares(direction*nsquares, ErrpSquares::NoDraw);
		CcTime::Sleep(p_dt.cue);

		/* Discrete feedback */
		squares->ScreenSync(p_hwt.dfeedback);
		idm.SetEvent(p_gdf.dfeedback);
		id.SetMessage(&ids);
		squares->PresentSquares(direction*nsquares, 0);
		CcTime::Sleep(p_dt.dfeedback);

		/* Trial loop */
		unsigned int totalsteps = 0;
		while(true) {
			bool error = false;
			int step = dynamic.GetStep(&error);
			if(step == CpDynamicP2::Over)
				break;
			
			if(totalsteps >= trials.size() * 2) {
				CcLogWarning("Maximum number of steps reached");
				break;
			}

			/* Implement a step */
			squares->ScreenSync(error == true ? p_hwt.error : p_hwt.correct);
			idm.SetEvent(error == true ? p_gdf.error: p_gdf.correct);
			id.SetMessage(&ids);
			squares->PresentSquares(direction*nsquares, step);
			totalsteps++;
			
			if(asonline == false) {
				/* Offline
				 * - simply wait
				 */
				CcTime::Sleep(p_dt.step);
			} else {
				/* Online
				 * - wait for the correction event to arrive on iD
				 * - true positive: immediately go to the next step
				 * - false positive: undo the last correct step
				 */
				bool correction = false;
				CcTimeValue tic;
				CcTime::Tic(&tic);
				id.Clear();
				while(CcTime::Toc(&tic) <= p_dt.waitundo) {
					/* Receive from iD */
					if(id.GetMessage(&ids) == false)
						continue;
					if(idm.GetEvent() != (int)t_gdf[0]) {
						CcTime::Sleep(25.00f);
						continue;
					}
					correction = true;
					break;
				}

				/* Correction */
				if(correction == true) {
					if(error == true) {
						/* True positive 
						 * - go to the next step with GetStep
						 */
						CcLogWarning("Undo: true positive");
						step = dynamic.GetStep(&error);
						squares->ScreenSync(1);
					} else { 
						/* False positive 
						 * - go to the previous step with UndoStep 
						 */
						CcLogWarning("Undo: false positive");
						step = dynamic.UndoStep();
						squares->ScreenSync(1);
					}
					squares->PresentSquares(direction*nsquares, step);
					totalsteps++;
					CcTime::Sleep(p_dt.step);
				} else {
					CcTime::Sleep(p_dt.step - p_dt.waitundo);
				}
			}
		}

		/* Trial off */
		trigger->Send(p_hwt.off);
		idm.SetEvent(p_gdf.trial + p_gdf.off);
		id.SetMessage(&ids);

		/* Handbrake */
		if(ClLoop::IsConnected() == false) {
			CcLogFatal("Cannot connect to loop");
			goto shutdown;
		}
		if(CcCore::receivedSIGAny.Get()) {
			CcLogWarning("User asked to go down");
			goto shutdown;
		}
		if(squares->eventkeyboard.Get(0) == DTKK_ESCAPE) {
			CcLogWarning("User asked to quit");
			goto shutdown;
		}
	}

shutdown:
	if(squares != NULL) {
		if(squares->IsOpen()) {
			squares->SwitchScene("end");
			CcTime::Sleep(p_dt.end);
			squares->Close();
		}
		delete squares;
	}
	ClLoop::Disconnect();
	if(taskset != NULL)
		delete(taskset);
	if(trigger != NULL)
		delete(trigger);
	CcCore::Exit(0);
	
	return 0;
}
