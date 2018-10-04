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

#ifndef CPTRIALS_HPP
#define CPTRIALS_HPP

#include <map>
#include <vector>

typedef unsigned int CpClassLabel;

class CpTrials {
	public:
		CpTrials(void);
		virtual ~CpTrials(void);

		bool AddClassLabel(CpClassLabel klabel, unsigned int occurrences);
		bool IncreaseClassLabel(CpClassLabel klabel, unsigned int occurrences);
		bool Create(void);
		bool Shuffle(void);
		void Dump(void);
		CpClassLabel At(unsigned int pos);
		unsigned int Total(void);

	public:
		static const unsigned int None = 666;
	private:
		std::vector<CpClassLabel> _trials;
		std::map<CpClassLabel, unsigned int> _koccurr;
};
#endif
