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

#ifndef CPDYNAMICP2_HPP 
#define CPDYNAMICP2_HPP 
#include <stdlib.h>

class CpDynamicP2 {
	public:
		CpDynamicP2(void);
		virtual ~CpDynamicP2(void);
		void Dump(void);
		void Scramble(unsigned int size, unsigned int E, int direction);
		bool IsOver(void);
		unsigned int TotalSteps(void);

		int GetStep(bool* iserror = NULL);
		int GetStepInvert(bool* iserror = NULL);
		int UndoStep(void);
		void Invert(void);
	private:
		void Initialize(unsigned int hsize);
		void Prepare(unsigned int E, int direction);
		void NewMatrix(void);
		void DelMatrix(void);
		void Zero(void);
		void Diag(void);
		unsigned int Rand(unsigned int min, unsigned int max);
		int Get(bool* iserror, bool docorrect);
	protected:

	public:
		const static int Right = +1;
		const static int Left = -1;
		const static int Over = 666;
	private:
		unsigned int** _matrix;
		unsigned int _size;
		unsigned int _origin;
		unsigned int _E;
		unsigned int _getOrigin;
		int _getDirection;
		int _getPosition;
	protected:
};

#endif
