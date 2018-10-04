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

#include "CpDynamicP2.hpp"
#include <iostream>

using namespace std;

int main(void) {
	int total = 0;
	CpDynamicP2 dynamic;
	
	/* 2011-04-30  Michele Tavella <michele.tavella@epfl.ch>
	 * Goodie. The first argument is the number of "squares" for side. 
	 * So, if you write 3, it means 3 squares per side plus a fixation one in
	 * the middle. The squares are encoded then as:
	 *            
	 * -3 -2 -1 0 +1 +2 +3
	 *          ^
	 *          |
	 *         "0" is the central one for fixation
	 * The Get* methods return the next.
	 */

	dynamic.Scramble(3, 1, CpDynamicP2::Right);
	dynamic.Dump();
	total = 0;
	while(dynamic.GetStep() != CpDynamicP2::Over) { total++; };
	cout << "Total=" << total << ", Expected=" << dynamic.TotalSteps() << endl;
	
	dynamic.Scramble(3, 1, CpDynamicP2::Left);
	dynamic.Dump();
	total = 0;
	while(dynamic.GetStep() != CpDynamicP2::Over) { total++; };
	cout << "Total=" << total << ", Expected=" << dynamic.TotalSteps() << endl;

	dynamic.Scramble(6, 2, CpDynamicP2::Right);
	dynamic.Dump();
	total = 0;
	while(dynamic.GetStep() != CpDynamicP2::Over) { total++; };
	cout << "Total=" << total << ", Expected=" << dynamic.TotalSteps() << endl;

	dynamic.Scramble(6, 2, CpDynamicP2::Left);
	dynamic.Dump();
	total = 0;
	while(dynamic.GetStep() != CpDynamicP2::Over) { total++; };
	cout << "Total=" << total << ", Expected=" << dynamic.TotalSteps() << endl;
	
	/* 2011-04-30  Michele Tavella <michele.tavella@epfl.ch>
	 * This piece of code simulates a user that online never corrects the
	 * feedback. 
	 * This means that the dynamic.Invert() function is never called and that
	 * the subject will bounce back and forth depending on where the errors are.
	 */
	dynamic.Scramble(6, 2, CpDynamicP2::Right);
	dynamic.Dump();
	total = 0;
	while(true) {
		bool error;
		int step = dynamic.GetStepInvert(&error);
		if(step == CpDynamicP2::Over)
			break;
		total++; 

		if(error == false)
			cout << "Going to: " << step << endl;
		else {
			cout << "Error! Going to: " << step << endl;
			//dynamic.Invert();
		}
	}
	/* Ideally, the number of total steps will be higher then the one expected,
	 * that is returned by dynamic.TotalSteps()
	 */
	cout << "Total=" << total << ", Expected=" << dynamic.TotalSteps() << endl;


	/* 2011-04-30  Michele Tavella <michele.tavella@epfl.ch>
	 * This piece of code simulates a PERFECT subject that online corrects all
	 * the errors.
	 * This means that the dynamic.Invert() function is always called when an
	 * error is detected and that the subject will finish the trial with the
	 * same number of steps as the expected ones
	 */
	dynamic.Scramble(6, 2, CpDynamicP2::Right);
	dynamic.Dump();
	total = 0;
	while(true) {
		bool error;
		int step = dynamic.GetStepInvert(&error);
		if(step == CpDynamicP2::Over)
			break;
		total++; 

		if(error == false)
			cout << "Going to: " << step << endl;
		else {
			dynamic.Invert();
			step = dynamic.GetStepInvert(&error);
			total++;
			cout << "Error! Going (back) to: " << step << endl;
		}
	}
	/* Ideally, the number of total steps will be higher then the one expected,
	 * that is returned by dynamic.TotalSteps()
	 */
	cout << "Total=" << total << ", Expected=" << dynamic.TotalSteps() << endl;
	
	/* 2011-04-30  Michele Tavella <michele.tavella@epfl.ch>
	 * This piece of code simulates a PERFECT subject that online corrects all
	 * the errors, following the no-inversion CNBI protocol.
	 * This means that the dynamic.Invert() function is always called when an
	 * error is detected and that the subject will finish the trial with the
	 * same number of steps as the expected ones
	 */
	dynamic.Scramble(6, 2, CpDynamicP2::Right);
	dynamic.Dump();
	total = 0;
	while(true) {
		bool error;
		int step = dynamic.GetStep(&error);
		if(step == CpDynamicP2::Over)
			break;
		total++; 

		if(error == false)
			cout << "Going to: " << step << endl;
		else {
			step = dynamic.GetStep(&error);
			total++;
			cout << "Error! Going to next: " << step << endl;
		}
	}
	/* Ideally, the number of total steps will be higher then the one expected,
	 * that is returned by dynamic.TotalSteps()
	 */
	cout << "Total=" << total << ", Expected=" << dynamic.TotalSteps() << endl;

	/* 2011-04-30  Michele Tavella <michele.tavella@epfl.ch>
	 * This piece of code simulates a 1/2 PERFECT subject that online corrects all
	 * the errors (when the step >2), but also corrects all the correct,
	 * following the no-inversion CNBI protocol.
	 */
	dynamic.Scramble(6, 2, CpDynamicP2::Right);
	dynamic.Dump();
	total = 0;
	while(total < 20) {
		bool error;
		int step = dynamic.GetStep(&error);
		if(step == CpDynamicP2::Over)
			break;
		total++; 

		if(error == false) {
			if(step > 2)
				step = dynamic.UndoStep();
			cout << "Going to: " << step << endl;
		} else {
			step = dynamic.GetStep(&error);
			total++;
			cout << "Error! Going to next: " << step << endl;
		}
	}
	/* Ideally, the number of total steps will be higher then the one expected,
	 * that is returned by dynamic.TotalSteps()
	 */
	cout << "Total=" << total << ", Expected=" << dynamic.TotalSteps() << endl;

	return 0;
}
