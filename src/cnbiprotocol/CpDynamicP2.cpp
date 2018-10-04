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

#ifndef CPDYNAMICP2_CPP 
#define CPDYNAMICP2_CPP 

#include "CpDynamicP2.hpp" 
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
using namespace std;

CpDynamicP2::CpDynamicP2(void) {
	this->_origin = 0;
	this->_size = 0;
	this->_matrix = NULL;
}

CpDynamicP2::~CpDynamicP2(void) {
}
		
void CpDynamicP2::Initialize(unsigned int hsize) {
	unsigned int new_size = hsize*2 + 1;
	if(this->_size != new_size) {
		this->DelMatrix();
		this->_origin = hsize;
		this->_size = new_size;
		this->NewMatrix();
	}
	this->Zero();
	this->Diag();
}
		
void CpDynamicP2::NewMatrix(void) {
	if(this->_matrix != NULL)
		return;

	this->_matrix = new unsigned int*[this->_size];
	for(unsigned int i = 0; i < this->_size; i++) 
		this->_matrix[i] = new unsigned int[this->_size];
}

void CpDynamicP2::DelMatrix(void) {
	if(this->_matrix == NULL)
		return;

	for(unsigned int i = 0; i < this->_size; i++) 
		delete [] this->_matrix[i];
	delete [] this->_matrix;
	this->_matrix = NULL;
}

void CpDynamicP2::Zero(void) {
	for(unsigned int i = 0; i < this->_size; i++) 
		for(unsigned int j = 0; j < this->_size; j++) 
			this->_matrix[i][j] = 0;
}


void CpDynamicP2::Diag(void) {
	for(unsigned int i = 0; i < this->_size; i++) 
		this->_matrix[i][i] = 1;
}

void CpDynamicP2::Dump(void) {
	printf("      ");
	for(unsigned int j = 0; j < this->_size; j++)
		printf("%2d ", j - this->_origin);
	printf("\n");
	
	printf("    +");
	for(unsigned int j = 0; j < this->_size; j++)
		printf("---");
	printf("\n");
	

	for(unsigned int i = 0; i < this->_size; i++) {
		printf("%3d | ", i - this->_origin);
		for(unsigned int j = 0; j < this->_size; j++) {
			if(i == j) {
				if(i == this->_origin)
					printf(" X ");
				else
					printf(" * ");
			} else { 
				if(this->_matrix[i][j] == 0)
					printf("   ");
				else
					printf("%2d ", this->_matrix[i][j]);
			}
			//printf("%2d ", this->_matrix[i][j]);
		}
		printf("\n");
	}
}
	
void CpDynamicP2::Prepare(unsigned int E, int direction) {
	unsigned int step_error, step_min, step_max, k, e = 0;
	while(e < E) {
		if(direction > 0) {
			step_min = 2 + this->_origin;
			step_max = this->_size - 1;
			k = -1;
		} else {
			step_min = 0;
			step_max = -1 + this->_origin;
			k = +1;
		}
		step_error = this->Rand(step_min, step_max);

		if(step_error > 0 && step_error < this->_size) {
			e++;
			this->_matrix[step_error][step_error + k] += 1;
		}
	}
	this->_getDirection = direction;
	this->_getPosition = 0;
	this->_getOrigin = this->_origin;
	this->_E = E;
}
		
unsigned int CpDynamicP2::Rand(unsigned int min, unsigned int max) {
	struct timeval seed;
	gettimeofday(&seed, NULL);

	srand(seed.tv_usec);
	return min + rand() % (max - min);
}
		
void CpDynamicP2::Scramble(unsigned int size, unsigned int E, int direction) {
	this->Initialize(size);
	this->Prepare(E, direction);
}

bool CpDynamicP2::IsOver(void) {
	return(this->_getOrigin <= 0 || this->_getOrigin >= this->_size - 1);
}
		
unsigned int CpDynamicP2::TotalSteps(void) {
	return(this->_origin + this->_E*2);
}

int CpDynamicP2::GetStep(bool* iserror) {
	return this->Get(iserror, true);
}

int CpDynamicP2::GetStepInvert(bool* iserror) {
	return this->Get(iserror, false);
}

int CpDynamicP2::Get(bool* iserror, bool docorrect) {
	unsigned int error = 0;

	if(this->IsOver())
		return CpDynamicP2::Over;

	if(this->_getDirection > 0) {
		error = this->_matrix[this->_getOrigin][this->_getOrigin - 1];
		if(error == 0) {
			this->_getOrigin++;
			this->_getPosition++;
		} else { 
			this->_matrix[this->_getOrigin][this->_getOrigin - 1]--;
			this->_getOrigin--;
			this->_getPosition--;
			if(docorrect == false)
				this->Invert();
		}
	} else {
		error = this->_matrix[this->_getOrigin][this->_getOrigin + 1];
		if(error == 0) {
			this->_getOrigin--;
			this->_getPosition--;
		} else { 
			this->_matrix[this->_getOrigin][this->_getOrigin + 1]--;
			this->_getOrigin++;
			this->_getPosition++;
			if(docorrect == false)
				this->Invert();
		}
	}

	if(iserror != NULL) 
		*iserror = (error > 0);
	return this->_getPosition;
}
		
void CpDynamicP2::Invert(void) {
	this->_getDirection *= -1;
}
		
int CpDynamicP2::UndoStep(void) {
	/* 2011-05-02  Michele Tavella <michele.tavella@epfl.ch>
	 * TODO Do not allow undo when error==true
	 */
	if(this->_getDirection > 0) {
		this->_getOrigin--;
		this->_getPosition--;
	} else {
		this->_getOrigin++;
		this->_getPosition++;
	}
	return this->_getPosition;
}

#endif
