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

#include "CpAudio.hpp"
#include "CpSound.hpp"
#include "CpPlayer.hpp"

int main(void) {
	CpAudio* audio = CpAudio::Open();
	CpSound sound0, sound1;
	CpPlayer player0;
	CpPlayer player1;

	sound0.Load("/usr/share/sounds/alsa/Front_Right.wav");
	sound1.Load("/usr/share/sounds/alsa/Front_Left.wav");

	player0.Play(&sound0);
	player0.Wait();
	
	player0.Play(&sound0);
	player1.Play(&sound1);
	player0.Wait();
	player1.Wait();

	audio->Close();

	return 0;
}
