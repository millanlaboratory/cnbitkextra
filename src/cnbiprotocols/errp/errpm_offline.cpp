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
#include "errpm_utilities.hpp"
#include "errpm_squares.hpp"
#include <cnbicore/CcBasic.hpp>
#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiId.hpp>
#include <cnbiconfig/CCfgConfig.hpp>
#include <cnbiprotocol/CpTriggerFactory.hpp>
#include <getopt.h>

void usage(void) { 
	printf("Usage: errpm_offline [OPTION]\n\n");
	printf("  -n       Run online\n");
	printf("  -h       display this help and exit\n");
	CcCore::Exit(1);
}

int main(int argc, char *argv[]) {	
	int opt;
	std::string binname = "errpm", modality = "offline";
	bool asonline = false;
		
	while((opt = getopt(argc, argv, "nh")) != -1) {
		if(opt == 'n') {
			asonline = true;
			binname = "errpm";
			modality = "online";
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
	unsigned int ntrials;	
	float probcorrect;
	
	// Tools for protocol
	dterrpm p_dt;
	hwterrpm p_hwt;
	gdferrpm p_gdf;
	sqperrpm s_p;
	opterrpm o_e;
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
	 * - Extract the protocol p_dt
	 * - Extract the protocol t_hwt and t_gdf
	 */

	CcLogFatalS("Modality:     " << modality);
	CcLogFatalS("XML Modality: " << xml_modality);
	if(errpm_configure_dynamic(&config, xml_block, xml_taskset, modality,
				&ntrials, &nsquares, &probcorrect) == false) {
		CcLogFatal("Dynamic configuration failed");
		goto shutdown;
	}
	if(errpm_get_dt(&config, &p_dt) == false) {
		CcLogFatal("Timing configuration failed");
		goto shutdown;
	}
	if(errpm_get_markers(&config, &p_hwt, &p_gdf) == false) {
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
	
	if(errpm_get_square_param(&config, &s_p) == false) {
		CcLogFatal("Square configuration failed");
		goto shutdown;
	}
	
	if(errpm_get_options(&config, &o_e) == false) {
		CcLogFatal("Options configuration failed");
		goto shutdown;
	}
	
	CcLogInfo("Protocol configured");

	/* Finish configuration:
	 * - set the number of squares for the dancing robot
	 */

	squares->SetSquares(nsquares);
	//squares->Open();
	squares->Open(s_p.sqsize);

	/* Indication for starting a Run*/
	squares->ScreenSync(p_hwt.trial);
	idm.SetEvent(p_gdf.trial);
	//id.SetMessage(&ids);
	if (id.SetMessage(&ids)==false) {
		CcLogInfoS("Classifier crashed ... Starting of the run");
		goto shutdown;
	}


	squares->SwitchScene("begin");
	CcTime::Sleep(p_dt.begin);
	squares->SwitchScene("feedback");

	int direction;
	unsigned int totaltrials;
	totaltrials = 0;	

	/*
	The main loop starts. Each loop initiates a RUN. Loop finishes when the 
	number of single trials exceed the number of trials required in a
	session
	*/

	
	
	while (true) {
		
		if (totaltrials > ntrials){
			CcLogInfoS("Session over ...");
			break;
		}

		/* Obtaining direction */
		direction = errpm_randdirection();
		
		/* Reset squares & trial on */
		squares->PresentSquares(ErrpSquares::NoDraw, ErrpSquares::NoDraw);
		
		/* Wait */
		//squares->FixationOff();
		//CcTime::Sleep(p_dt.waitmin, p_dt.waitmax); 

		/* Fixation */
		squares->ScreenSync(p_hwt.fixation);
		idm.SetEvent(p_gdf.fixation);
		//id.SetMessage(&ids);
		if (id.SetMessage(&ids)==false) {
			CcLogInfoS("Classifier crashed ... Fixation");
			goto shutdown;
		}
		squares->FixationOn();
		CcTime::Sleep(p_dt.fixation);

		/* Cue position, a) Left or Right End, b) Random */
		int cuePos = direction*nsquares; // a) Left or right end

		/* Cue */
		squares->ScreenSync(direction > 0 ? p_hwt.right : p_hwt.left);
		idm.SetEvent(direction > 0 ? p_gdf.right : p_gdf.left);
		if (id.SetMessage(&ids)==false) {
			CcLogInfoS("Classifier crashed ... cue orange");
			goto shutdown;
		}
		squares->PresentSquares(cuePos, ErrpSquares::NoDraw);
		CcTime::Sleep(p_dt.cue);

		/* Feedback position a) center, b) random */
		int feedbackPos = 0; // a) center
		// Code later for the random initialization		

		/* Discrete feedback */
		squares->ScreenSync(p_hwt.dfeedback);
		idm.SetEvent(p_gdf.dfeedback);
		if (id.SetMessage(&ids)==false) {
			CcLogInfoS("Classifier crashed ... feedback blue");
			goto shutdown;
		}
		squares->PresentSquares(direction*nsquares, feedbackPos);
		CcTime::Sleep(p_dt.dfeedback);
		
		/* Initialising timeout for a Run*/	
		CcTimeValue tic_timeout;
		CcTime::Tic(&tic_timeout);

		/* Trial loop */
		while(true) {
			bool error = false;
		
			/* RUN EXIT TIMEOUT*/				
			if(CcTime::Toc(&tic_timeout) > p_dt.timeout) {
				squares->ScreenSync(p_hwt.targetmiss);
				idm.SetEvent(p_gdf.targetmiss);
				if (id.SetMessage(&ids)==false) {
					CcLogInfoS("Classifier crashed ... timeout");
					goto shutdown;
				}
				squares->PresentSquaresFailure(cuePos, feedbackPos);				
				CcTime::Sleep(p_dt.failure);
				CcLogInfoS("Timeout for a run reached ...");
				break;
			}

			float randfloat = errpm_randfloat(); 
			if (randfloat>probcorrect){
				CcLogInfoS("Move : Error");
				error = true; 
			} else {
				CcLogInfoS("Move : Correct ");
			}
				
			int feedback_pos_update = (cuePos>feedbackPos?1:-1) * ((randfloat>probcorrect)?-1:1);
			feedbackPos +=  feedback_pos_update;// CHECK FOR feedbackPos in the edges
			if (feedbackPos < -1*(int)nsquares) // Falling off left edge
				feedbackPos = -1*nsquares + 1;
			if (feedbackPos > (int)nsquares)  // Falling off right edge
				feedbackPos = nsquares - 1;

			/* Implement a step */
			squares->ScreenSync(error == true ? p_hwt.error : p_hwt.correct);
			idm.SetEvent(error == true ? p_gdf.error: p_gdf.correct);
			if (id.SetMessage(&ids)==false) {
				CcLogInfoS("Classifier crashed ... implementing a step");
				goto shutdown;
			}

			
			squares->PresentSquares(cuePos, feedbackPos);
			totaltrials++;

			if(squares->eventkeyboard.Get(0) == DTKK_ESCAPE) {
				CcLogWarning("User asked to quit");
				goto shutdown;
			}
			
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
					if(id.GetMessage(&ids) == false){
						CcTime::Sleep(10.00f);
						continue;
					}
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
						//step = dynamic.GetStep(&error);
						feedbackPos -=  feedback_pos_update;//
						squares->ScreenSync(p_hwt.beep);
						idm.SetEvent(p_gdf.beep);
						if (id.SetMessage(&ids)==false) {
							CcLogInfoS("Classifier crashed ... True positive");
							goto shutdown;
						}
					} else { 
						/* False positive 
						 * - go to the previous step with UndoStep 
						 */
						CcLogWarning("Undo: false positive");
						//step = dynamic.UndoStep();
						feedbackPos -=  feedback_pos_update;
						squares->ScreenSync(p_hwt.beep);
						idm.SetEvent(p_gdf.beep);
						if (id.SetMessage(&ids)==false) {
							CcLogInfoS("Classifier crashed ... False positive");
							goto shutdown;
						}
					}
					squares->PresentSquares(direction*nsquares, feedbackPos);
					CcTime::Sleep(p_dt.step);
				} else {
					CcTime::Sleep(p_dt.step - p_dt.waitundo);
				}
			}
			if (feedbackPos == cuePos){
				squares->ScreenSync(p_hwt.targethit);
				idm.SetEvent(p_gdf.targethit);
				if (id.SetMessage(&ids)==false) {
					CcLogInfoS("Classifier crashed ... Target hit");
					goto shutdown;
				}
				squares->PresentSquaresSuccess(cuePos, feedbackPos);
				CcLogInfoS("Run Finished ...");
				CcTime::Sleep(p_dt.success);				
				break;				
			}
		} // Run loop ends

		squares->FixationOff(); // special code
		
		// Press a button for telling which direction was the target //
		
		/* Handbrake */
		if(ClLoop::IsConnected() == false) {
			CcLogFatal("Cannot connect to loop");
			goto shutdown;
		}
		if(CcCore::receivedSIGAny.Get()) {
			CcLogWarning("User asked to go down");
			goto shutdown; // TEMPORARY
		}
		//if(squares->eventkeyboard.Get(0) == DTKK_ESCAPE) {
		if(squares->eventkeyboard.Get(0) == DTKK_ESCAPE) {
			CcLogWarning("User asked to quit");
			goto shutdown;
		}		
		
		if(o_e.buttonpress){
			int kbevent = 0;
			squares->eventkeyboard.Set(0);
			CcLogInfoS("Event in the keyboard : " << squares->eventkeyboard.Get(1));
			squares->PresentSquares(ErrpSquares::NoDraw, ErrpSquares::NoDraw);			
			CcTimeValue tic_bp;
			CcTime::Tic(&tic_bp);
			while(p_dt.buttonpress == 0 ? 1 : CcTime::Toc(&tic_bp)<= p_dt.buttonpress){
				kbevent = squares->eventkeyboard.Get(0);
				if (kbevent == DTKK_ESCAPE){
					CcLogWarning("User asked to quit*******");
					break;
				}
				if (kbevent !=  DTKK_LEFT && kbevent != DTKK_RIGHT){
					CcTime::Sleep(25.00f);					
					continue; // CHECK FOR CPU OVERLOADING
				}
				if (kbevent == DTKK_LEFT){
					squares->ScreenSync(direction < 0 ? p_hwt.feedbackc : p_hwt.feedbacke); // Trigger values to be inserted in utilities
					idm.SetEvent(direction < 0 ? p_gdf.feedbackc : p_gdf.feedbacke);        // TRIGGERS IN XML
					if (id.SetMessage(&ids)==false) {
						CcLogInfoS("Classifier crashed ... left button press");
						goto shutdown;
					}
					if (direction < 0) 
						squares->PresentSquaresSuccess(direction*nsquares,  ErrpSquares::NoDraw);
					else
						squares->PresentSquaresFailure(-1*nsquares,  -1*nsquares); 
				}				
				if (kbevent == DTKK_RIGHT){
					squares->ScreenSync(direction > 0 ?  p_hwt.feedbackc : p_hwt.feedbacke);   // TRIGGERS IN XML
					idm.SetEvent(direction > 0 ? p_gdf.feedbackc : p_gdf.feedbacke);		// TRIGGERS IN XML
					if (id.SetMessage(&ids)==false) {
						CcLogInfoS("Classifier crashed ... right button press");
						goto shutdown;
					}
					if (direction > 0) 
						squares->PresentSquaresSuccess(direction*nsquares,  ErrpSquares::NoDraw);
					else
						squares->PresentSquaresFailure(1*nsquares,  1*nsquares);
				}

				CcTime::Sleep(p_dt.buttonfeedback); 
				squares->PresentSquares(ErrpSquares::NoDraw, ErrpSquares::NoDraw);
				CcTime::Sleep(1000.0);
				break;
			}
		}		
		
	} // session loop ends

	/* Run off */
		CcTime::Sleep(2000.0);
		trigger->Send(p_hwt.off);
		idm.SetEvent(p_gdf.trial + p_gdf.off);
		if (id.SetMessage(&ids)==false) {
			CcLogInfoS("Classifier crashed ... Loop end");
			goto shutdown;
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
