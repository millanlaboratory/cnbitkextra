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

#include <cnbiconfig/CCfgConfig.hpp>
#include <cnbicore/CcBasic.hpp>
#include <cnbiprotocol/CpDynamicP2.hpp>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include <stdlib.h>

typedef struct dterrpm_struct {
	float begin;
	float end;
	float cue;
	float fixation;
	float waitmin;
	float waitmax;
	float dfeedback;
	float step;
	float success; // mkg
	float failure; // mkg	
	float waitundo;
	float timeout; // mkg
	float buttonpress; //mkg
	float buttonfeedback; //mkg
} dterrpm;

typedef struct gdferrpm_struct {
	GDFEvent wait;
	GDFEvent fixation;
	GDFEvent dfeedback;
	GDFEvent correct;
	GDFEvent error;
	GDFEvent trial;
	GDFEvent off;
	GDFEvent left;
	GDFEvent right;
	GDFEvent targethit;// mkg
	GDFEvent targetmiss;// mkg
	GDFEvent beep; // mkg
	GDFEvent feedbackc;// mkg
	GDFEvent feedbacke;// mkg		
} gdferrpm;

typedef struct hwterrpm_struct {
	HWTrigger wait;
	HWTrigger fixation;
	HWTrigger dfeedback;
	HWTrigger correct;
	HWTrigger error;
	HWTrigger trial;
	HWTrigger off;
	HWTrigger left;
	HWTrigger right;
	HWTrigger targethit;
	HWTrigger targetmiss;
	HWTrigger beep; //mkg
	HWTrigger feedbackc;//mkg
	HWTrigger feedbacke;//mkg
} hwterrpm;

typedef struct sqperrpm_struct {
	float sqsize;
} sqperrpm;

typedef struct errpm_options {
	bool buttonpress;
} opterrpm;


float errpm_randfloat(void){
	srand(time(NULL));
	return ((float)rand())/((float)RAND_MAX);
}

int errpm_randdirection(void){
	srand(time(NULL));
	return (((float)rand())/((float)RAND_MAX) > 0.5 ?-1:1);
}
	
bool errpm_configure_dynamic(CCfgConfig* config, std::string& block, 
		std::string& taskset, std::string& modality, unsigned int* trials, 
		unsigned int* squares, float* probcorrect) {
	unsigned int nsquares = 0, ntrials = 0;
	float pcorrect = 0; 	
	try {
		config->RootEx()->GoEx(modality)->SetBranch();
		nsquares = config->BranchEx()->GoEx(block)->QuickIntEx("squares");
		ntrials = config->BranchEx()->GoEx(block)->QuickIntEx("trials");
		pcorrect = config->BranchEx()->GoEx(block)->QuickFloatEx("probcorrect");
	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
	*trials = ntrials;
	*squares = nsquares;
	*probcorrect = pcorrect;
	return true;
}

bool errpm_get_dt(CCfgConfig* config, dterrpm* timings) {
	try {
		config->RootEx()->QuickEx("protocol/errpm/")->SetBranch();
		timings->begin = config->BranchEx()->QuickFloatEx("scenes/begin");
		timings->end= config->BranchEx()->QuickFloatEx("scenes/end");
		timings->fixation= config->BranchEx()->QuickFloatEx("trial/fixation");
		timings->cue = config->BranchEx()->QuickFloatEx("trial/cue");
		timings->waitmin = config->BranchEx()->QuickFloatEx("trial/waitmin");
		timings->waitmax = config->BranchEx()->QuickFloatEx("trial/waitmax");
		timings->dfeedback = config->BranchEx()->QuickFloatEx("trial/dfeedback");
		timings->step = config->BranchEx()->QuickFloatEx("trial/step");
		timings->success = config->BranchEx()->QuickFloatEx("trial/success"); //mkg
		timings->failure = config->BranchEx()->QuickFloatEx("trial/failure"); //mkg		
		timings->waitundo = config->BranchEx()->QuickFloatEx("trial/waitundo");
		timings->timeout = config->BranchEx()->QuickFloatEx("trial/timeout"); //mkg
		timings->buttonpress = config->BranchEx()->QuickFloatEx("trial/buttonpress");//mkg
		timings->buttonfeedback = config->BranchEx()->QuickFloatEx("trial/buttonfeedback");//mkg
	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
	return true;
}

bool errpm_get_square_param(CCfgConfig* config, sqperrpm* sqp) { //mkg
	try {
		config->RootEx()->QuickEx("options/errpm")->SetBranch();
		sqp->sqsize = config->BranchEx()->QuickFloatEx("sqsize");
	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
	return true;
}

bool errpm_get_options(CCfgConfig* config, opterrpm* opt) { //mkg
	try {
		config->RootEx()->QuickEx("options/errpm")->SetBranch();
		opt->buttonpress = config->BranchEx()->QuickBoolEx("buttonpress");
	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
	return true;
}

bool errpm_get_markers(CCfgConfig* config, hwterrpm* hwt, gdferrpm* gdf) {
	try {
		config->RootEx()->QuickEx("events")->SetBranch();
		gdf->wait = config->BranchEx()->QuickGDFIntEx("gdfevents/wait");
		gdf->fixation = config->BranchEx()->QuickGDFIntEx("gdfevents/fixation");
		gdf->dfeedback = config->BranchEx()->QuickGDFIntEx("gdfevents/dfeedback");
		gdf->correct = config->BranchEx()->QuickGDFIntEx("gdfevents/cnbi/correct");
		gdf->error = config->BranchEx()->QuickGDFIntEx("gdfevents/cnbi/error");
		gdf->trial = config->BranchEx()->QuickGDFIntEx("gdfevents/trial");
		gdf->off = config->BranchEx()->QuickGDFIntEx("gdfevents/off");
		gdf->right = config->BranchEx()->QuickGDFIntEx("gdfevents/cnbi/right");
		gdf->left = config->BranchEx()->QuickGDFIntEx("gdfevents/cnbi/left");
		gdf->targethit = config->BranchEx()->QuickGDFIntEx("gdfevents/targethit"); //mkg
		gdf->targetmiss = config->BranchEx()->QuickGDFIntEx("gdfevents/targetmiss");//mkg
		gdf->beep = config->BranchEx()->QuickGDFIntEx("gdfevents/beep");//mkg	
		gdf->feedbackc = config->BranchEx()->QuickGDFIntEx("gdfevents/cnbi/top");//mkg	
		gdf->feedbacke = config->BranchEx()->QuickGDFIntEx("gdfevents/cnbi/bottom");//mkg
	
		hwt->wait = config->BranchEx()->QuickIntEx("hwtriggers/wait");
		hwt->fixation = config->BranchEx()->QuickIntEx("hwtriggers/fixation");
		hwt->dfeedback = config->BranchEx()->QuickIntEx("hwtriggers/dfeedback");
		hwt->correct = config->BranchEx()->QuickIntEx("hwtriggers/cnbi/correct");
		hwt->error = config->BranchEx()->QuickIntEx("hwtriggers/cnbi/error");
		hwt->trial = config->BranchEx()->QuickIntEx("hwtriggers/trial");
		hwt->off = config->BranchEx()->QuickIntEx("hwtriggers/off");
		hwt->right = config->BranchEx()->QuickIntEx("hwtriggers/cnbi/right");
		hwt->left = config->BranchEx()->QuickIntEx("hwtriggers/cnbi/left");
		hwt->targethit = config->BranchEx()->QuickIntEx("hwtriggers/targethit");//mkg
		hwt->targetmiss = config->BranchEx()->QuickIntEx("hwtriggers/targetmiss");//mkg
		hwt->beep = config->BranchEx()->QuickIntEx("hwtriggers/beep");//mkg
		hwt->feedbackc = config->BranchEx()->QuickIntEx("hwtriggers/cnbi/top");//mkg
		hwt->feedbacke = config->BranchEx()->QuickIntEx("hwtriggers/cnbi/bottom");//mkg
	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
	return true;
}


