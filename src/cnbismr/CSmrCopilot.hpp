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

#ifndef CSMRCOPILOT_HPP
#define CSMRCOPILOT_HPP

#include <cnbiprotocol/CpProbability.hpp>

class CSmrCopilot {
	public:
		CSmrCopilot(void);	
		~CSmrCopilot(void);	

		void SetClassNumber(unsigned int classes);
		void SetTrialNumber(unsigned int* occurrence);
		void SetTrialDuration(unsigned int duration_ms);
		void SetControlGranularity(unsigned int frequency_hz);
	
		void SetThreshold(float threshold);
		void SetCoefficientViscosity(float viscosity);
		//void SetOscillatory(OscillatorySettings* settings);
	
		unsigned int InitializeEx(unsigned int strial);
		bool GetControlValuesEx(unsigned int step, CpProbability* ctrl, 
				int sclass = -1);
		
		unsigned int GetClassNumber(void);
		unsigned int GetTrialNumber(void);
		unsigned int GetTrialDuration(void);
		unsigned int GetControlGranularity(void);
		unsigned int GetStepNumber(void);
		float GetThreshold(void);

	private:
		void InitializeSequence(void);

	private:
		unsigned int _classes;
		unsigned int _trials;
		unsigned int _duration;
		unsigned int _frequency;
		unsigned int _steps;
		unsigned int* _occurrence;
		float _bias;

		float _threshold;
		float _viscosity;
		float* _increment;

		float** _probabilities;
		bool _initialized;
};

#endif
