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

#ifndef CPPROBABILITY_HPP 
#define CPPROBABILITY_HPP 

#include <tobiic/ICMessage.hpp>
#include <tobiic/ICClassifier.hpp>
#include <map>

class CpProbability {
	public:
		CpProbability(const unsigned int size);
		CpProbability(ICMessage* message, const std::string& cname);
		CpProbability(ICClassifier* classifier);
		virtual ~CpProbability(void);

		CpProbability* MapICClass(int label, unsigned int index);
		CpProbability* Update(ICMessage* message);
		CpProbability* Update(ICClassifier* classifier);
		
		unsigned int Size(void);
		CpProbability* Set(unsigned int index, float value);
		CpProbability* Get(unsigned int index, float* value);
		float Get(unsigned int index);
		
		CpProbability* Zero(void);
		CpProbability* Priors(void);
		CpProbability* Hard(unsigned int index);

		float Min(unsigned int* index = 0);
		float Max(unsigned int* index = 0);
		void Dump(void);

	private:
	protected:

	public:
		float* data;
	private:
		unsigned int _size;
		bool _tobiic;
		std::string _icclassifier;
		std::map<unsigned int, int> _icmap;
	protected:
};

#endif
