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

#include <cnbicore/CcException.hpp>
#include "CpProbability.hpp"
#include <iostream>

using namespace std;

int main(void) {
	CpProbability p(3);
	p.Dump();

	p.Set(0, 0.5);
	p.Set(1, 0.4);
	p.Set(2, 0.1);
	p.Dump();

	cout << p.Get(0) << endl;
	cout << p.Get(1) << endl;
	cout << p.Get(2) << endl;

	unsigned int atmax, atmin;
	float max, min;
	max = p.Max(&atmax);
	min = p.Min(&atmin);
	cout << "Max at " << atmax << ": " << max << endl;
	cout << "Min at " << atmin << ": " << min << endl;

	ICClassifier classifierMI("cnbi_mi", "CNBI MI Classifier", 
			ICClassifier::ValueProb, 
			ICClassifier::LabelBiosig);
	ICClass class_rh(0x300, 0.10f);
	ICClass class_lh(0x301, 0.90f);
	classifierMI.classes.Add(&class_rh);
	classifierMI.classes.Add(&class_lh);
	
	ICMessage message;
	message.classifiers.Add(&classifierMI);

	CpProbability pc(&classifierMI);
	pc.Dump();
	pc.MapICClass(0x300, 0);
	pc.MapICClass(0x301, 1);
	pc.Dump();
	pc.Update(&classifierMI);
	pc.Dump();
	
	CpProbability pm(&message, "cnbi_mi");
	pm.MapICClass(0x300, 1);
	pm.MapICClass(0x301, 0);
	pm.Dump();
	pm.Update(&message);
	pm.Dump();

	return 0;
}
