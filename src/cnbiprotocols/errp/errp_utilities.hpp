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

typedef struct dterrp_struct {
	float begin;
	float end;
	float cue;
	float fixation;
	float waitmin;
	float waitmax;
	float dfeedback;
	float step;
	float waitundo;
} dterrp;

typedef struct gdferrp_struct {
	GDFEvent wait;
	GDFEvent fixation;
	GDFEvent dfeedback;
	GDFEvent correct;
	GDFEvent error;
	GDFEvent trial;
	GDFEvent off;
	GDFEvent left;
	GDFEvent right;

} gdferrp;

typedef struct hwterrp_struct {
	HWTrigger wait;
	HWTrigger fixation;
	HWTrigger dfeedback;
	HWTrigger correct;
	HWTrigger error;
	HWTrigger trial;
	HWTrigger off;
	HWTrigger left;
	HWTrigger right;
} hwterrp;

unsigned int errp_get_dynaerrors(int value) {
	return (unsigned int)(abs(value) - 10);
}

int errp_get_dynadirection(int value) {
	if(value > 0)
		return CpDynamicP2::Right;
	else
		return CpDynamicP2::Left;
}

std::vector<int> errp_get_dyna6_020_240(void) {
	std::vector<int> trials;
	trials.push_back(+10); 
	trials.push_back(+10); 
	trials.push_back(-10);
	trials.push_back(+11); 
	trials.push_back(+11); 
	trials.push_back(-11); 
	trials.push_back(-11);
	trials.push_back(+12); 
	trials.push_back(+12); 
	trials.push_back(+12); 
	trials.push_back(+12); 
	trials.push_back(+12); 
	trials.push_back(-12); 
	trials.push_back(-12); 
	trials.push_back(-12); 
	trials.push_back(-12); 
	trials.push_back(-12);
	trials.push_back(+13); 
	trials.push_back(+13); 
	trials.push_back(-13);
	trials.push_back(-13);
	trials.push_back(-14); 
	trials.push_back(-14); 
	trials.push_back(+14);

	struct timeval seed;
	gettimeofday(&seed, NULL);
	srand(seed.tv_usec);
 
	random_shuffle(trials.begin(), trials.end());

	return trials;
}

bool errp_configure_dynamic(CCfgConfig* config, std::string& block, 
		std::string& taskset, std::string& modality, std::vector<int>* trials, 
		unsigned int* squares) {
	unsigned int nsquares = 0, ntrials = 0;
	try {
		config->RootEx()->GoEx(modality)->SetBranch();
		nsquares = config->BranchEx()->GoEx(block)->QuickIntEx("squares");
		ntrials = config->BranchEx()->GoEx(block)->QuickIntEx("trials");
	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
	
	if(nsquares == 6 && ntrials == 240) {
		*trials = errp_get_dyna6_020_240();
		*squares = nsquares;
	} else {
		CcLogFatalS("Cannot find preset for " << nsquares << "/" << ntrials);
		return false;
	}
	return true;
}

bool errp_get_dt(CCfgConfig* config, dterrp* timings) {
	try {
		config->RootEx()->QuickEx("protocol/errp/")->SetBranch();
		timings->begin = config->BranchEx()->QuickFloatEx("scenes/begin");
		timings->end= config->BranchEx()->QuickFloatEx("scenes/end");
		timings->fixation= config->BranchEx()->QuickFloatEx("trial/fixation");
		timings->cue = config->BranchEx()->QuickFloatEx("trial/cue");
		timings->waitmin = config->BranchEx()->QuickFloatEx("trial/waitmin");
		timings->waitmax = config->BranchEx()->QuickFloatEx("trial/waitmax");
		timings->dfeedback = config->BranchEx()->QuickFloatEx("trial/dfeedback");
		timings->step = config->BranchEx()->QuickFloatEx("trial/step");
		timings->waitundo = config->BranchEx()->QuickFloatEx("trial/waitundo");
	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
	return true;
}

bool errp_get_markers(CCfgConfig* config, hwterrp* hwt, gdferrp* gdf) {
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
	
		hwt->wait = config->BranchEx()->QuickIntEx("hwtriggers/wait");
		hwt->fixation = config->BranchEx()->QuickIntEx("hwtriggers/fixation");
		hwt->dfeedback = config->BranchEx()->QuickIntEx("hwtriggers/dfeedback");
		hwt->correct = config->BranchEx()->QuickIntEx("hwtriggers/cnbi/correct");
		hwt->error = config->BranchEx()->QuickIntEx("hwtriggers/cnbi/error");
		hwt->trial = config->BranchEx()->QuickIntEx("hwtriggers/trial");
		hwt->off = config->BranchEx()->QuickIntEx("hwtriggers/off");
		hwt->right = config->BranchEx()->QuickIntEx("hwtriggers/cnbi/right");
		hwt->left = config->BranchEx()->QuickIntEx("hwtriggers/cnbi/left");
	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
	return true;
}

bool errp_get_taskset(CCfgConfig* config, CCfgTaskset** taskset,
		std::string bname, std::string tname, bool online) {
	try { 
		if(online == true) 
			*taskset = config->OnlineEx(bname, tname);
		else
			*taskset = config->OfflineEx(bname, tname);
	} catch(XMLException e) {
		CcLogException(e.Info());
		CcLogException(e.Caller());
		return false;
	}
	return true;
}

bool errp_get_triggers(CCfgTaskset* taskset, HWTrigger** triggers, GDFEvent** events) {
	CCfgTasksetConstIt it;
	*triggers = new HWTrigger(taskset->Count());
	*events = new GDFEvent(taskset->Count());
	for(it = taskset->Begin(); it != taskset->End(); it++) {
		CCfgTask* task = it->second;
		(*triggers)[task->id] = task->hwt;
		(*events)[task->id] = task->gdf;
	}
	return true;
}
