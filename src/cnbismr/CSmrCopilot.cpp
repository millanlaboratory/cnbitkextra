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

#ifndef CSMRCOPILOT_CPP
#define CSMRCOPILOT_CPP

// TODO: dealloc!
#include "CSmrCopilot.hpp"
#include "generator.h"
#include "stimula.h"
#include <cnbicore/CcException.hpp>
#include <math.h>
		
co_stimula_sequence sequence;

CSmrCopilot::CSmrCopilot(void) {
	this->_classes = 0;
	this->_trials = 0;
	this->_duration = 0;
	this->_frequency = 0;
	this->_threshold = 0;
	this->_viscosity = 0;
	this->_steps = 0;
	this->_probabilities = NULL;
	this->_occurrence = NULL;
	this->_initialized = false;
}

CSmrCopilot::~CSmrCopilot(void) {
}

void CSmrCopilot::SetClassNumber(unsigned int classes) {
	this->_classes = classes;
	this->_bias= 1.00f / (float)this->_classes;
}

void CSmrCopilot::SetTrialNumber(unsigned int* occurrence) {
	this->_occurrence = new unsigned int[this->_classes];
	this->_trials = 0;
	for(unsigned int i = 0; i < this->_classes; i++) {
		this->_trials += occurrence[i];
		this->_occurrence[i] = occurrence[i];
	}
}

void CSmrCopilot::SetTrialDuration(unsigned int duration_ms) {
	this->_duration = duration_ms;
}

void CSmrCopilot::SetControlGranularity(unsigned int frequency_hz) {
	this->_frequency = frequency_hz;
}

void CSmrCopilot::SetThreshold(float threshold) {
	this->_threshold = threshold;
}

void CSmrCopilot::SetCoefficientViscosity(float viscosity) {
	this->_viscosity = viscosity;
}
		
//void CSmrCopilot::SetOscillatory(OscillatorySettings* settings) {
//	this->SetClassNumber(settings->classes);
//	this->SetTrialNumber(settings->trials);
//	this->SetTrialDuration(settings->dt_cfeedback);
//	this->SetControlGranularity(settings->fakefeedback_granularity);
//	this->SetThreshold(settings->fakefeedback_threshold);
//	this->SetCoefficientViscosity(settings->fakefeedback_viscosity);
//}
		
unsigned int CSmrCopilot::InitializeEx(unsigned int strial) {
	if(this->_classes <= 0) 
		throw CcException("Classes must be defined", 
				__PRETTY_FUNCTION__);
	if(this->_trials <= 0)
		throw CcException("Trials must be defined", 
				__PRETTY_FUNCTION__);
	if(this->_duration <= 0)
		throw CcException("Duration must be defined", 
				__PRETTY_FUNCTION__);
	if(this->_frequency <= 0)
		throw CcException("Frequency must be defined", 
				__PRETTY_FUNCTION__);
	if(this->_viscosity <= 0)
		throw CcException("Viscosity must be defined", 
				__PRETTY_FUNCTION__);
	if(this->_threshold <= 0)
		throw CcException("Threshold must be defined", 
				__PRETTY_FUNCTION__);

	this->_steps = this->_duration * this->_frequency / 1000;
	
	this->_probabilities = (float**)malloc(this->_steps*sizeof(float*));
	for(unsigned int k = 0; k < this->_steps; k++) 
		this->_probabilities[k] = (float*)malloc(this->_classes*sizeof(float));
	
	// Added malloc for _increment
	this->_increment = (float*)malloc(this->_classes*sizeof(float));	
	
	if(this->_initialized == false)
		this->InitializeSequence();

	unsigned int sclass = sequence.sequence[strial];
	
	/* Creation of probabilities set: [numset][numclass]
	 */
	
	for(unsigned int s = 0; s < this->_steps; s++) 
		co_generator_create(this->_probabilities[s], this->_classes, sclass);

	/* Set negative the probabilities
	 * In this way should be ready for the error as well
	 *...it's a bit a mess...
	 */
	for (unsigned int ss = 0; ss < this->_steps; ss++) {
		// Find the probability max and its index
		float max_prob = 0.0f;
		int id_max = -1;
		for (unsigned int k = 0; k < this->_classes; k++) {
			if (this->_probabilities[ss][k] > max_prob) {
				max_prob = _probabilities[ss][k];
				id_max = k;
			}
		}
		// Set negative the other probabilities
		for (int k = 0; k < (int)this ->_classes; k++) {
			if(k != id_max) 
				this->_probabilities[ss][k] = 0 - (this->_probabilities[ss][k]);
		}
	}
	/* Calculate the "integration sum" of the probabilities along all the
	 * steps for each class
	 */
	float tot_prob[this->_classes]; 
	for(unsigned int c = 0; c < this->_classes; c++) {
		tot_prob[c] = 0.00f;
		for(unsigned int s = 0; s < this->_steps; s++) 
			tot_prob[c] += this->_viscosity * fabs(this->_probabilities[s][c]);
	}
	

	float threshold	 = this->_threshold - this->_bias;

	/* Calculate the increment for the probabilities for each class to
	 * reach: 
	 *	- for the class target:	threshold 
	 *	- for the other classes: 1.0/(numClasses) 
	 */
	for (unsigned int cc = 0; cc < this->_classes; cc++) {
		if (cc == sclass) 
			this->_increment[cc] = threshold / tot_prob[cc];
		else 
			this->_increment[cc] = (1.0/this->_classes) / tot_prob[cc];
	}
	return sclass;
}

void CSmrCopilot::InitializeSequence(void) {
	sequence = co_stimula_init(this->_classes, this->_occurrence, NULL);
	co_stimula_randomize(&sequence);
	this->_initialized = true;
}

bool CSmrCopilot::GetControlValuesEx(unsigned int step, CpProbability* ctrl, 
		int sclass) {
	if(this->_classes != ctrl->Size()) 
		throw CcException("Must have same size", __PRETTY_FUNCTION__);

	for(unsigned int k = 0; k < this->_classes; k++) 
		ctrl->data[k] += this->_viscosity * this->_increment[k] *
			this->_probabilities[step][k];
	return(step == this->_steps - 1);
}
		
unsigned int CSmrCopilot::GetClassNumber(void) {
	return this->_classes;
}

unsigned int CSmrCopilot::GetTrialNumber(void) {
	return this->_trials;
}

unsigned int CSmrCopilot::GetTrialDuration(void) {
	return this->_duration;
}

unsigned int CSmrCopilot::GetControlGranularity(void) {
	return this->_frequency;
}

unsigned int CSmrCopilot::GetStepNumber(void) {
	return this->_steps;
}

float CSmrCopilot::GetThreshold(void) {
	return this->_threshold;
}

#endif
