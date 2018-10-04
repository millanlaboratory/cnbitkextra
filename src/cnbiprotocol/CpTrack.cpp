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

#ifndef CPTRACK_CPP 
#define CPTRACK_CPP 

#include "CpTrack.hpp" 

CpTrack::CpTrack(void) {
}

CpTrack::CpTrack(const std::string& track, char separator) {
	this->Split(track, separator);
}

CpTrack::~CpTrack(void) {
	this->_track.clear();
}
		
void CpTrack::Set(const std::string& track, char separator) {
	this->Split(track, separator);
}

std::string CpTrack::Get(unsigned int i) {
	if(i > this->_track.size())
		return std::string("");
	return this->_track[i];
}

unsigned int CpTrack::Size(void) {
	return this->_track.size();
}

void CpTrack::Split(const std::string& strack, char separator) {
    int startpos = 0;
    int endpos = 0;
	this->_track.clear();

    endpos = strack.find_first_of(separator, startpos);
	while(endpos != -1) {       
        this->_track.push_back(strack.substr(startpos, endpos - startpos));
        startpos = endpos + 1;
        endpos = strack.find_first_of(separator, startpos);
		if(endpos == -1)
            this->_track.push_back(strack.substr(startpos));
    }
}

#endif
