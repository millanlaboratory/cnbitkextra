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

#include "ErrpSquares.hpp"
#include <sstream>
#include <string.h>

ErrpSquares::ErrpSquares(int winw, int winh, CpTrigger* trigger) :
	CSmrEngineExt("bincnbiprotocols::ErrpSquares", winw, winh, trigger) {
	this->Init();
}
		
ErrpSquares::ErrpSquares(CCfgConfig* configuration, CpTrigger* trigger) :
	CSmrEngineExt(configuration, trigger) {
	this->Init();
}

ErrpSquares::~ErrpSquares(void) {
}

void ErrpSquares::Init(void) {
	this->_Asquares = NULL;
	this->_Bsquares = NULL;
	this->_Asquaresn = NULL;
	this->_Bsquaresn = NULL;
	this->_Asquares_str = NULL;
	this->_Bsquares_str = NULL;

	this->SetSquares(5);

	/* squares' width: to be parametrized! */
	this->_sqs = 0;
	this->_stpsz = 0;
	this->_cwdth = 0;
	this->_chght = 0;
}

void ErrpSquares::SetSquares(unsigned int totalsquares) {
	this->_totalsquares = totalsquares;
}

void ErrpSquares::Setup(void) {
	/* squares' width: to be parametrized! */
	this->_sqs = 1 / ((1+0.5f) * this->_totalsquares + 1);
	this->_stpsz = 3.0/2 * this->_sqs;
	this->_cwdth = this->_sqs*2/3;
	this->_chght = this->_sqs*2/3;
	this->_color_fixation = dtk_aluminium_light;
	this->_color_whitesq = dtk_white;
	this->_color_target = dtk_orange_dark;
	this->_color_cursor = dtk_skyblue_med;

	this->_Asquares = new dtk_hshape[this->_totalsquares];
	this->_Bsquares = new dtk_hshape[this->_totalsquares];
	this->_Asquaresn = new dtk_hshape[this->_totalsquares];
	this->_Bsquaresn = new dtk_hshape[this->_totalsquares];
	this->_Asquares_str = new std::string[this->_totalsquares];
	this->_Bsquares_str = new std::string[this->_totalsquares];


	 /* Copy Cue and Calibrations locally to alter alpha */
	memcpy(_rgb_fixation, _color_fixation, 4 * sizeof(float));
	memcpy(_rgb_whitesq, dtk_white, 4 * sizeof(float));
	memcpy(_rgb_targetsq, _color_target, 4 * sizeof(float));
	memcpy(_rgb_cursorsq, _color_cursor, 4 * sizeof(float));

	/* Add scenes */
	CSmrEngineExt::AddScene("begin");
	CSmrEngineExt::AddScene("feedback");
	CSmrEngineExt::AddScene("viewdrawable");
	CSmrEngineExt::AddScene("end");

	/* Add shapes*/
	std::stringstream tmpstream;
	std::string tmpstr;
	
	// fixation cross
	CSmrEngineExt::AddShape("feedback", "A_fixation0");
	CSmrEngineExt::AddShape("feedback", "A_fixation1");
	
	// white square
	CSmrEngineExt::AddShape("feedback", "W_whitesq");
	
	// central frame
	CSmrEngineExt::AddShape("feedback", "K_centr_frame");
	
	// frames of first class
	for(unsigned int idx = 0; idx < this->_totalsquares; idx++){
		tmpstream << "Lsquare" << idx+1;
		this->_Asquares_str[idx] = tmpstream.str();
		CSmrEngineExt::AddShape("feedback", this->_Asquares_str[idx]);
		tmpstream.str("");
	}
	
	// frames of second class
	for(unsigned int idx = 0; idx < this->_totalsquares; idx++) {
		tmpstream << "Msquare" << idx+1;
		this->_Bsquares_str[idx] = tmpstream.str();
		CSmrEngineExt::AddShape("feedback", this->_Bsquares_str[idx]);
		tmpstream.str("");
	}
	// target and cursor squares
	CSmrEngineExt::AddShape("feedback", "B_targetsquare");
	CSmrEngineExt::AddShape("feedback", "C_cursorsquare");

	// to have a look at the drawable screen
	CSmrEngineExt::AddShape("viewdrawable", "drawable");

	/* Render shapes */
	this->DataWait();
	this->RenderFixation(1.00f);
	this->RenderWhiteSq(0.00f);
	this->RenderFrames();
	this->RenderDrawableArea();
	this->RenderTargetSquare(0.00f, 0.00f, 0.00f);
	this->RenderCursorSquare(0.00f, 0.00f, 0.00f);
	this->DataPost();
}

void ErrpSquares::Teardown(void) {
	CSmrEngineExt::DestroyScene("begin");
	CSmrEngineExt::DestroyScene("feedback");
	CSmrEngineExt::DestroyScene("end");
}


void ErrpSquares::FixationOn(void) {
	this->DataWait();
	this->RenderFixation(1.00f);
	this->DataPost();
	
	CSmrEngineExt::RequestUpdate();
}

void ErrpSquares::FixationOff(void) {
	this->DataWait();
	this->RenderFixation(0.00);
	this->DataPost();
	
	CSmrEngineExt::RequestUpdate();
}

void ErrpSquares::RenderFixation(float alpha) {
	this->_rgb_fixation[3] = alpha;

	this->_fixv = CSmrEngineExt::GetShape("feedback", "A_fixation0");
	this->_fixvn = dtk_create_rectangle_2p(this->_fixv,
			-this->_cwdth/2*0.1f, -this->_chght/2,
			+this->_cwdth/2*0.1f, +this->_chght/2,
			1, this->_rgb_fixation);
	this->_fixh = CSmrEngineExt::GetShape("feedback", "A_fixation1");
	this->_fixhn = dtk_create_rectangle_2p(this->_fixh,
			-this->_cwdth/2, -this->_chght/2*0.1f,
			+this->_cwdth/2, +this->_chght/2*0.1f,
			1, this->_rgb_fixation);

	if(this->_fixv != this->_fixvn)
		CSmrEngineExt::SetShape("feedback", "A_fixation0", this->_fixvn);
	if(this->_fixh != this->_fixhn)
		CSmrEngineExt::SetShape("feedback", "A_fixation1", this->_fixhn);
}

void ErrpSquares::MoveFixation(float stepx, float stepy){
	this->_fixv = CSmrEngineExt::GetShape("feedback", "A_fixation0");
	CSmrEngineExt::MoveShapeRel("feedback", "A_fixation0", this->_stpsz*stepx,
			this->_stpsz*stepy);
	this->_fixh = CSmrEngineExt::GetShape("feedback", "A_fixation1");
	CSmrEngineExt::MoveShapeRel("feedback", "A_fixation1", this->_stpsz*stepx,
			this->_stpsz*stepy);

}

void ErrpSquares::RenderDrawableArea(void) {
	this->_fullsquare = CSmrEngineExt::GetShape("viewdrawable", "drawable");
	this->_fullsquaren = dtk_create_rectangle_2p(NULL, -1, -1, 1, 1, 0, 
			dtk_white);
	if(this->_fullsquare != this->_fullsquaren)
		CSmrEngineExt::SetShape("viewdrawable", "drawable", _fullsquaren);
}

void ErrpSquares::RenderFrames(void) {
	// Central frame
	this->_cframe = CSmrEngineExt::GetShape("feedback", "K_centr_frame");
	this->_cframen = dtk_create_rectangle_2p(this->_cframe, 
			-this->_sqs/2*1.02, -this->_sqs/2*1.02, 
			+this->_sqs/2*1.02, +this->_sqs/2*1.02,  
			0, dtk_white);
	if(this->_cframe != this->_cframen)
		CSmrEngineExt::SetShape("feedback", "K_centr_frame", this->_cframen);

	// frames of first class
	for(int idx = 0; idx < (int)this->_totalsquares; idx++) {
		this->_Asquares[idx] = CSmrEngineExt::GetShape("feedback", 
				_Asquares_str[idx]);
		this->_Asquaresn[idx] = dtk_create_rectangle_2p(this->_Asquares[idx], 
			-(idx+1)*this->_stpsz-this->_sqs/2*1.02, -this->_sqs/2*1.02, 
			-(idx+1)*this->_stpsz+this->_sqs/2*1.02, +this->_sqs/2*1.02, 
			0, dtk_white);
		if(this->_Asquares[idx] != this->_Asquaresn[idx])
			CSmrEngineExt::SetShape("feedback", this->_Asquares_str[idx],
					this->_Asquaresn[idx]);
	}

	// frames of second class
	for(int idx = 0; idx < (int)this->_totalsquares; idx++) {
		this->_Bsquares[idx] = CSmrEngineExt::GetShape("feedback", 
				this->_Bsquares_str[idx]);
		this->_Bsquaresn[idx] = dtk_create_rectangle_2p(this->_Bsquares[idx], 
			+(idx+1)*this->_stpsz-this->_sqs/2*1.02, -this->_sqs/2*1.02, 
			+(idx+1)*this->_stpsz+this->_sqs/2*1.02, +this->_sqs/2*1.02, 
			0, dtk_white);
		if(this->_Bsquares[idx] != this->_Bsquaresn[idx])
			CSmrEngineExt::SetShape("feedback", this->_Bsquares_str[idx], 
					this->_Bsquaresn[idx]);
	}
}

void ErrpSquares::RenderTargetSquare(float xpos, float ypos, float alpha) {
	this->_rgb_targetsq[3] = alpha;

	this->_targsq = CSmrEngineExt::GetShape("feedback", "B_targetsquare");
	this->_targsqn = dtk_create_rectangle_2p (this->_targsq,
			xpos-this->_sqs/2, ypos-this->_sqs/2, 
			xpos+this->_sqs/2, ypos+this->_sqs/2, 
			1, this->_rgb_targetsq);

	if(this->_targsq != this->_targsqn)
		CSmrEngineExt::SetShape("feedback", "B_targetsquare", _targsqn);
}

void ErrpSquares::RenderCursorSquare(float xpos, float ypos, float alpha) {
	this->_rgb_cursorsq[3] = alpha;

	this->_curssq = CSmrEngineExt::GetShape("feedback", "C_cursorsquare");
	this->_curssqn = dtk_create_rectangle_2p(this->_curssq,
			xpos-this->_sqs/2, ypos-this->_sqs/2, 
			xpos+this->_sqs/2, ypos+this->_sqs/2, 
			1, this->_rgb_cursorsq);

	if(this->_curssq != this->_curssqn)
		CSmrEngineExt::SetShape("feedback", "C_cursorsquare", this->_curssqn);
}

void ErrpSquares::PresentSquares(short int targetlocation, 
		short int cursorlocation) {
	if(targetlocation == ErrpSquares::NoDraw)
		trg_alpha = 0;
	else {
		trg_xpos = targetlocation*this->_stpsz;
		trg_ypos = 0;
		trg_alpha = 1;
	}
	if(cursorlocation == ErrpSquares::NoDraw)
		cur_alpha = 0;
	else {
		cur_xpos = cursorlocation*this->_stpsz;
		cur_ypos = 0;
		cur_alpha = 1;
	}

	this->DataWait();
	this->RenderTargetSquare(trg_xpos, trg_ypos, trg_alpha);
	this->RenderCursorSquare(cur_xpos, cur_ypos, cur_alpha);
	this->DataPost();
	
	CSmrEngineExt::RequestUpdate();
}

void ErrpSquares::WhiteSqOn(void) {
	this->DataWait();
	this->RenderWhiteSq(1.00f);
	this->DataPost();
	CSmrEngineExt::RequestUpdate();
}

void ErrpSquares::WhiteSqOff(void) {
	this->DataWait();
	this->RenderWhiteSq(0.00);
	this->DataPost();
	CSmrEngineExt::RequestUpdate();
}

void ErrpSquares::RenderWhiteSq(float alpha) {
	this->_rgb_whitesq[3] = alpha;

	this->_whtsq = CSmrEngineExt::GetShape("feedback", "W_whitesq");
	this->_whtsqn = dtk_create_rectangle_2p (this->_whtsq,
			1, -1, 1.2, -0.8, 1, this->_rgb_whitesq);

	if(this->_whtsq != this->_whtsqn)
		CSmrEngineExt::SetShape("feedback", "W_whitesq", this->_whtsqn);
}

void ErrpSquares::Open(void) {
	this->Setup();
	CSmrEngineExt::Open();
}

void ErrpSquares::Close(void) {
	CSmrEngineExt::Close();
	this->Teardown();
}
