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

#include "CpTrials.hpp"
#include <iostream>

int main(void) {
	
	CpTrials trials;
	trials.AddClassLabel(0, 5);
	trials.AddClassLabel(2, 5);
	trials.AddClassLabel(1, 5);
	trials.AddClassLabel(3, 5);
	trials.AddClassLabel(4, 5);
	trials.AddClassLabel(5, 5);
	trials.AddClassLabel(6, 5);
	trials.AddClassLabel(7, 5);
	trials.AddClassLabel(8, 10);
	trials.Create();
	trials.Dump();
	trials.Shuffle();
	trials.Dump();
	trials.IncreaseClassLabel(0, 5);
	trials.Create();
	trials.Shuffle();
	trials.Dump();
	
	CpTrials trialsE;
	trialsE.Create();
	trialsE.Dump();
	
	CpTrials trialsMI;
	trialsMI.AddClassLabel(0, 15);
	trialsMI.AddClassLabel(1, 15);
	
	trialsMI.Create();
	trialsMI.Dump();
	
	trialsMI.Shuffle();
	trialsMI.Dump();
	

	return 0;
}
