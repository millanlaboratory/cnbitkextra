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

#ifndef CSMRBARS_CPP
#define CSMRBARS_CPP

#include "CSmrBars.hpp"
#include <string.h>


CSmrBars::CSmrBars(int winw, int winh, CpTrigger* trigger) :
	CSmrEngineExt("libcnbismr::CSmrBars", winw, winh, trigger) {
	this->Init();
}
		
CSmrBars::CSmrBars(CCfgConfig* configuration, CpTrigger* trigger) :
	CSmrEngineExt(configuration, trigger) {
	this->Init();
}

CSmrBars::~CSmrBars(void) {
	//this->Teardown();
}

void CSmrBars::Init(void) {
	this->SetParameters();
	this->SetAspect();
	this->SetPosition();

	this->_cordctrl[0] = 0.0;
	this->_cordctrl[1] = 0.0;
	this->_cordctrl[2] = 0.0;
	this->_cordctrl[3] = 0.0;

	this->_text_font  = dtk_load_font("arial:style=bold");
}
		
bool CSmrBars::SetParameters(unsigned int classes, float threshold) {
	if(classes < 2 || classes > 4) {
		CcLogError("Classes must be [2, 4]");
		return false;
	}

	this->_classes = classes;
	this->_threshold[0] = threshold;
	this->_threshold[1] = threshold;
	this->_threshold[2] = threshold;
	this->_threshold[3] = threshold;
	return true;
}

bool CSmrBars::SetParameters2(unsigned int classes, float* threshold) {
	if(classes < 2 || classes > 4) {
		CcLogError("Classes must be [2, 4]");
		return false;
	}

	this->_classes = classes;

	switch(this->_classes) {
		case 4:
			this->_threshold[3] = threshold[3];
		case 3:
			this->_threshold[2] = threshold[2];
		case 2:
			this->_threshold[1] = threshold[1];
			this->_threshold[0] = threshold[0];
	}
	return true;
}
		
void CSmrBars::SetAspect(float fw, float fl, float sr) {
	this->_feedbackwidth = fw;
	this->_feedbacklenght = fl;
	this->_boomradius = sr;
}

void CSmrBars::SetPosition(float x, float y) {
	this->_x = x;
	this->_y = y;
}

void CSmrBars::UpdateThreshold(float* threshold) {

	this->_threshold[0] = threshold[0];
	this->_threshold[1] = threshold[1];
	this->_threshold[2] = threshold[2];
	this->_threshold[3] = threshold[3];

	CSmrEngine::DataWait();
	this->RenderBars();
	this->RenderArrows();
	this->RenderThreshold(this->_threshold);
	CSmrEngine::DataPost();
}

void CSmrBars::Setup(void) {
	this->_color_bars			= dtk_aluminium2_dark;
	this->_color_control			= dtk_aluminium_dark;
	this->_color_controlflash		= dtk_chameleon_dark;
	this->_color_threshold			= dtk_aluminium_light;
	this->_color_cue			= dtk_scarletred_dark;
	this->_color_rest 			= dtk_butter_light;
	this->_color_block 			= dtk_scarletred_dark;
	this->_color_fixation			= dtk_aluminium_light;
	this->_color_calibration 		= dtk_aluminium_light; 
	this->_colors_arrow[this->ArrowRight] 	= dtk_scarletred_med;
	this->_colors_arrow[this->ArrowLeft]  	= dtk_skyblue_med;
	this->_colors_arrow[this->ArrowUp]	= dtk_chameleon_dark;
	this->_colors_arrow[this->ArrowDown]  	= dtk_orange_dark;
	this->_colors_arrow[this->ArrowNone]  	= dtk_chocolate_light;

	/* 2009-10-19  Michele Tavella <michele.tavella@epfl.ch> 
	 * Copy Cue and Calibrations locally to alter alpha
	 */
	memcpy(this->_rgb_calibration, this->_color_calibration, 4 * sizeof(float));
	memcpy(this->_rgb_fixation, this->_color_fixation, 4 * sizeof(float));
	memcpy(this->_rgb_rest, this->_color_rest, 4 * sizeof(float));
	memcpy(this->_rgb_block, this->_color_block, 4 * sizeof(float));

	this->_cuerotations[0] = +180.00f;
	this->_cuerotations[1] =    0.00f;
	this->_cuerotations[2] =  -90.00f;
	this->_cuerotations[3] =  +90.00f;
	
	CSmrEngine::AddScene("begin");
	CSmrEngine::AddScene("wait");
	CSmrEngine::AddScene("feedback");
	CSmrEngine::AddScene("end");
	CSmrEngine::AddScene("start");
	CSmrEngine::AddScene("stop");

	CSmrEngine::AddShape("feedback", "0600_cue"); 
	CSmrEngine::AddShape("feedback", "0700_rest"); 
	CSmrEngine::AddShape("feedback", "0800_block"); 
	CSmrEngine::AddShape("feedback", "0900_calibration"); 

	CSmrEngine::AddShape("feedback", "0500_fixationH");
	CSmrEngine::AddShape("feedback", "0500_fixationV");

	CSmrEngine::AddShape("feedback", "0100_bar0");
	CSmrEngine::AddShape("feedback", "0100_bar1");
	if(this->_classes >= 3)
		CSmrEngine::AddShape("feedback", "0100_bar2");
	if(this->_classes == 4) 
		CSmrEngine::AddShape("feedback", "0100_bar3");
	CSmrEngine::AddShape("feedback", "0100_barC");
	
	CSmrEngine::AddShape("feedback", "0300_arrow0");
	CSmrEngine::AddShape("feedback", "0300_arrow1");
	if(this->_classes >= 3)
		CSmrEngine::AddShape("feedback", "0300_arrow2");
	if(this->_classes == 4) 
		CSmrEngine::AddShape("feedback", "0300_arrow3");
	
	CSmrEngine::AddShape("feedback", "0300_tail0");
	CSmrEngine::AddShape("feedback", "0300_tail1");
	if(this->_classes >= 3)
		CSmrEngine::AddShape("feedback", "0300_tail2");
	if(this->_classes == 4) 
		CSmrEngine::AddShape("feedback", "0300_tail3");
	
	CSmrEngine::AddShape("feedback", "0200_liquid0");
	CSmrEngine::AddShape("feedback", "0200_liquid1");
	if(this->_classes >= 3)
		CSmrEngine::AddShape("feedback", "0200_liquid2");
	if(this->_classes == 4)
		CSmrEngine::AddShape("feedback", "0200_liquid3");
	
	CSmrEngine::AddShape("feedback", "0110_threshold0");
	CSmrEngine::AddShape("feedback", "0110_threshold1");
	if(this->_classes >= 3)
		CSmrEngine::AddShape("feedback", "0110_threshold2");
	if(this->_classes == 4)
		CSmrEngine::AddShape("feedback", "0110_threshold3");
	
	CSmrEngine::AddShape("feedback", "0400_boom");

	CSmrEngine::AddShape("wait", "2000_step01"); 
	CSmrEngine::AddShape("wait", "2000_step02"); 
	CSmrEngine::AddShape("wait", "2000_step03"); 
	CSmrEngine::AddShape("wait", "2000_step04"); 
	CSmrEngine::AddShape("wait", "2000_step05"); 
	CSmrEngine::AddShape("wait", "2000_step06"); 
	CSmrEngine::AddShape("wait", "2000_step07"); 
	CSmrEngine::AddShape("wait", "2000_step08"); 
	CSmrEngine::AddShape("wait", "2000_step09"); 
	CSmrEngine::AddShape("wait", "2000_step10"); 
	CSmrEngine::AddShape("wait", "2000_step11"); 
	CSmrEngine::AddShape("wait", "2000_step12"); 
	
	CSmrEngine::AddShape("start", "0000_start_fg"); 
	CSmrEngine::AddShape("start", "0000_start_bg"); 
	CSmrEngine::AddShape("stop", "0000_stop_fg"); 
	CSmrEngine::AddShape("stop", "0000_stop_bg"); 

	CSmrEngine::DataWait();
	this->RenderBegin();
	this->RenderEnd();
	this->RenderWait(0);
	this->RenderRest(0.00f);
	this->RenderBlock(0.00f);
	this->RenderFixation(0.00f);
	this->RenderCue(0.00f, this->_color_bars, 0.00f);
	this->RenderCalibration(0.00f);
	this->RenderBars();
	this->RenderArrows();
	CpProbability probability(this->_classes);
	probability.Priors();
	this->RenderControl(&probability, this->_color_control);
	this->RenderThreshold(this->_threshold);
	this->RenderBoom(this->_color_bars, 0.00f);
	CSmrEngine::DataPost();
}

void CSmrBars::Teardown(void) {
	if(CSmrEngine::HasScene("begin"))
		CSmrEngine::DestroyScene("begin");
	if(CSmrEngine::HasScene("feedback"))
		CSmrEngine::DestroyScene("feedback");
	if(CSmrEngine::HasScene("end"))
		CSmrEngine::DestroyScene("end");
	if(CSmrEngine::HasScene("start"))
		CSmrEngine::DestroyScene("start");
	if(CSmrEngine::HasScene("stop"))
		CSmrEngine::DestroyScene("stop");
	if(CSmrEngine::HasScene("wait"))
		CSmrEngine::DestroyScene("wait");
	dtk_destroy_font(this->_text_font);
}
		
void CSmrBars::Control(CpProbability* probs, CSmrBarsClass sclass, bool boom) {
	CSmrEngine::DataWait();
	const float* color = this->_color_control;

	if(this->_classes != probs->Size()) {
		CcLogErrorS("Control size " << probs->Size() << ", but configured for "
				<< this->_classes);
		return;
	}

	if(boom) {
		switch(sclass){
			case CSmrBars::ArrowRight:
			case CSmrBars::ArrowLeft:
			case CSmrBars::ArrowUp:
			case CSmrBars::ArrowDown:
			case CSmrBars::ArrowNone:
				color = this->_colors_arrow[sclass];
				break;
			default:
				color = this->_color_controlflash;
		}
	}
	this->RenderControl(probs, color);
	this->RenderBoom(color, boom ? 1.00f : 0.00f);

	CSmrEngine::DataPost();
	CSmrEngine::RequestUpdate();
}

void CSmrBars::FixationCue(CSmrBarsCue type, CSmrBarsClass sclass, 
		bool calibration) {
	CSmrEngine::DataWait();
	
	switch(type) {
		case CSmrBars::DrawNone:
			this->RenderCalibration(0.00f);
			this->RenderCue(0.00f, this->_colors_arrow[0], 0.00f);
			this->RenderFixation(0.00);
			this->RenderRest(0.00f);
			this->RenderBlock(0.00f);
			break;
		case CSmrBars::DrawCue:
			if(sclass == CSmrBars::ArrowNone) 
				break;
			this->RenderRest(0.00f);
			this->RenderBlock(0.00f);
			this->RenderFixation(0.00);
			this->RenderCue(this->_cuerotations[sclass],
					this->_colors_arrow[sclass],
					1.00f);
			if(calibration)
				this->RenderCalibration(1.00f);
			break;
		case CSmrBars::DrawFixation:
			this->RenderCalibration(0.00f);
			this->RenderCue(0.00f, this->_colors_arrow[0], 0.00f);
			this->RenderFixation(1.00f);
			this->RenderRest(0.00f);
			this->RenderBlock(0.00f);
			break;
		case CSmrBars::DrawRest:
			this->RenderCalibration(0.00f);
			this->RenderCue(0.00f, this->_colors_arrow[0], 0.00f);
			this->RenderFixation(0.00f);
			this->RenderRest(1.00f);
			this->RenderBlock(0.00f);
			break;
		case CSmrBars::DrawBlock:
			this->RenderCalibration(0.00f);
			this->RenderCue(0.00f, this->_colors_arrow[0], 0.00f);
			this->RenderFixation(0.00f);
			this->RenderRest(0.00f);
			this->RenderBlock(0.00f);
			break;
	}

	CSmrEngine::DataPost();
	CSmrEngine::RequestUpdate();
}

void CSmrBars::ShowBlock(std::string text) {
	CSmrEngine::DataWait();

	this->RenderBlock(1.00f, text);

	CSmrEngine::DataPost();
	CSmrEngine::RequestUpdate();
}

void CSmrBars::RenderCue(float rotation, const float* color, float alpha) {
	float rgb[4];
	memcpy(rgb, color, 4 * sizeof(float));
	rgb[3] = alpha;

	dtk_hshape arr = CSmrEngine::GetShape("feedback", "0600_cue");
	dtk_hshape arrn = dtk_create_arrow(arr,
			0.00f,
			0.00f,
			0.90f*(2*this->_feedbackwidth), 
			0.90f*(2*this->_feedbackwidth),
			1, rgb);
			
	dtk_rotate_shape(arrn, rotation);
	dtk_move_shape(arrn, this->_x, this->_y);
	
	if(arrn != arr)
		CSmrEngine::SetShape("feedback", "0600_cue", arrn);
}

void CSmrBars::RenderFixation(float alpha) {
	this->_rgb_fixation[3] = alpha;

	dtk_hshape fixo = CSmrEngine::GetShape("feedback", "0500_fixationH");
	dtk_hshape fixon = dtk_create_rectangle_2p(fixo,
			this->_x - 0.20f*this->_feedbackwidth, 
			this->_y - 0.80f*this->_feedbackwidth, 
			this->_x + 0.20f*this->_feedbackwidth, 
			this->_y + 0.80f*this->_feedbackwidth, 
			1, this->_rgb_fixation);
	dtk_hshape fixv = CSmrEngine::GetShape("feedback", "0500_fixationV");
	dtk_hshape fixvn = dtk_create_rectangle_2p(fixv,
			this->_x - 0.80f*this->_feedbackwidth, 
			this->_y - 0.20f*this->_feedbackwidth, 
			this->_x + 0.80f*this->_feedbackwidth, 
			this->_y + 0.20f*this->_feedbackwidth, 
			1, this->_rgb_fixation);
	
	if(fixo != fixon)
		CSmrEngine::SetShape("feedback", "0500_fixationH", fixon);
	if(fixv != fixvn)
		CSmrEngine::SetShape("feedback", "0500_fixationV", fixvn);
}

void CSmrBars::RenderRest(float alpha) {
	this->_rgb_rest[3] = alpha;

	dtk_hshape rest = CSmrEngine::GetShape("feedback", "0700_rest");
	dtk_hshape restn = dtk_create_circle(rest,
			this->_x,
			this->_y,
			0.60f*this->_feedbackwidth,
			1, this->_rgb_rest, 200);
	
	if(rest != restn)
		CSmrEngine::SetShape("feedback", "0700_rest", restn);
}

void CSmrBars::RenderBlock(float alpha, std::string text) {
	float rgb_bblock[4];
	
	this->_rgb_block[3] = alpha;
	memcpy(rgb_bblock, dtk_aluminium_light, 4 * sizeof(float));
	rgb_bblock[3] = alpha;

	this->_block_text = text;
	
	dtk_hshape block  = CSmrEngine::GetShape("feedback", "0800_block");
	dtk_hshape blocklist[] = { dtk_create_circle(NULL, this->_x, this->_y,
						     2.6*this->_feedbackwidth, 1, 
					      	     rgb_bblock, 8),
		   		   dtk_create_circle(NULL, this->_x, this->_y,
					      	     2.5*this->_feedbackwidth, 1, 
					      	     this->_rgb_block, 8),
				   dtk_create_string(NULL, this->_block_text.c_str(), 1.2*this->_feedbackwidth,
						    this->_x, this->_y, DTK_HMID|DTK_VMID, rgb_bblock, 
						    this->_text_font)
	};
	
	dtk_rotate_shape(blocklist[0], 360.0f/8.0f/2.0f);
	dtk_rotate_shape(blocklist[1], 360.0f/8.0f/2.0f);

	dtk_hshape blockn = dtk_create_composite_shape(block, sizeof(blocklist)/sizeof(blocklist[0]), 
						       blocklist, 1);

	if(block != blockn)
		CSmrEngine::SetShape("feedback", "0800_block", blockn);
}

void CSmrBars::RenderCalibration(float alpha) {
	this->_rgb_calibration[3] = alpha;
	
	dtk_hshape cal = CSmrEngine::GetShape("feedback", "0900_calibration");
	dtk_hshape caln = dtk_create_rectangle_2p(cal,
			this->_x + 0.80f, 
			this->_y - 0.80f, 
			this->_x + 1.00f, 
			this->_y - 1.00f, 
			1, this->_rgb_calibration);
	
	if(caln != cal)
		CSmrEngine::SetShape("feedback", "0900_calibration", caln);
}
		
void CSmrBars::RenderBars(void) {
	dtk_hshape bar0 = CSmrEngine::GetShape("feedback", "0100_bar0");
	dtk_hshape bar0n = dtk_create_rectangle_2p(bar0, 
			this->_x + this->_feedbackwidth,
			this->_y - this->_feedbackwidth,
			this->_x + this->_feedbacklenght + this->_feedbackwidth,
			this->_y + this->_feedbackwidth,
			1, this->_color_bars);
	
	dtk_hshape bar1 = CSmrEngine::GetShape("feedback", "0100_bar1");
	dtk_hshape bar1n = dtk_create_rectangle_2p(bar1, 
			this->_x - this->_feedbackwidth,
			this->_y - this->_feedbackwidth,
			this->_x - this->_feedbacklenght - this->_feedbackwidth,
			this->_y + this->_feedbackwidth,
			1, this->_color_bars);

	if(bar0n != bar0)
		CSmrEngine::SetShape("feedback", "0100_bar0", bar0n);
	if(bar1n != bar1)
		CSmrEngine::SetShape("feedback", "0100_bar1", bar1n);
	
	if(this->_classes >= 3) {
		dtk_hshape bar2 = CSmrEngine::GetShape("feedback", "0100_bar2");
		dtk_hshape bar2n = dtk_create_rectangle_2p(bar2, 
				this->_x + this->_feedbackwidth,
				this->_y + this->_feedbackwidth,
				this->_x - this->_feedbackwidth,
				this->_y + this->_feedbacklenght + this->_feedbackwidth,
				1, this->_color_bars);
		
		if(bar2n != bar2)
			CSmrEngine::SetShape("feedback", "0100_bar2", bar2n);
	}
	if(this->_classes == 4) {
		dtk_hshape bar3 = CSmrEngine::GetShape("feedback", "0100_bar3");
		dtk_hshape bar3n = dtk_create_rectangle_2p(bar3, 
				this->_x - this->_feedbackwidth, 
				this->_y - this->_feedbackwidth,
				this->_x + this->_feedbackwidth,
				this->_y - this->_feedbacklenght - this->_feedbackwidth,
				1, this->_color_bars);
		
		if(bar3n != bar3)
			CSmrEngine::SetShape("feedback", "0100_bar3", bar3n);
	}
}

void CSmrBars::RenderControl(CpProbability* probs, const float* rgb) {
	dtk_hshape ctrl0 = CSmrEngine::GetShape("feedback", "0200_liquid0");
	dtk_hshape ctrl0n = dtk_create_rectangle_2p(ctrl0,
			this->_x + this->_feedbackwidth,
			this->_y - this->_feedbackwidth,
			this->_x + probs->data[0]*this->_feedbacklenght 
				+ this->_feedbackwidth,
			this->_y + this->_feedbackwidth,
			1, rgb);
	if(ctrl0n != ctrl0)
		CSmrEngine::SetShape("feedback", "0200_liquid0", ctrl0n);

	dtk_hshape ctrl1 = CSmrEngine::GetShape("feedback", "0200_liquid1");
	dtk_hshape ctrl1n = dtk_create_rectangle_2p(ctrl1,
			this->_x - this->_feedbackwidth,
			this->_y - this->_feedbackwidth,
			this->_x - probs->data[1]*this->_feedbacklenght 
				- this->_feedbackwidth,
			this->_y + this->_feedbackwidth,
			1, rgb);
	if(ctrl1n != ctrl1)
		CSmrEngine::SetShape("feedback", "0200_liquid1", ctrl1n);

	if(this->_classes >= 3) {
		dtk_hshape ctrl2 = CSmrEngine::GetShape("feedback", "0200_liquid2");
		dtk_hshape ctrl2n = dtk_create_rectangle_2p(ctrl2,
				this->_x + this->_feedbackwidth,
				this->_y + this->_feedbackwidth,
				this->_x - this->_feedbackwidth,
				this->_y + probs->data[2]*this->_feedbacklenght 
					+ this->_feedbackwidth,
				1, rgb);
		if(ctrl2n != ctrl2)
			CSmrEngine::SetShape("feedback", "0200_liquid2", ctrl2n);
	}

	if(this->_classes == 4) {
		dtk_hshape ctrl3 = CSmrEngine::GetShape("feedback", "0200_liquid3");
		dtk_hshape ctrl3n = dtk_create_rectangle_2p(ctrl3,
				this->_x - this->_feedbackwidth,
				this->_y - this->_feedbackwidth,
				this->_x + this->_feedbackwidth,
				this->_y - probs->data[3]*this->_feedbacklenght 
					- this->_feedbackwidth,
				1, rgb);
		if(ctrl3n != ctrl3)
			CSmrEngine::SetShape("feedback", "0200_liquid3", ctrl3n);
	}
	
	dtk_hshape barC = CSmrEngine::GetShape("feedback", "0100_barC");
	dtk_hshape barCn  = dtk_create_rectangle_2p(barC, 
			this->_x - this->_feedbackwidth,
			this->_y - this->_feedbackwidth,
			this->_x + this->_feedbackwidth,
			this->_y + this->_feedbackwidth,
			1, rgb);
	
	if(barCn != barC)
		CSmrEngine::SetShape("feedback", "0100_barC", barCn);
}

void CSmrBars::RenderThreshold(float* threshold) {
	float l0 = this->_threshold[0];
	float l1 = this->_threshold[1];
	float l2 = this->_threshold[2];
	float l3 = this->_threshold[3];

	dtk_hshape ctrl0 = CSmrEngine::GetShape("feedback", "0110_threshold0");
	dtk_hshape ctrl0n = dtk_create_rectangle_2p(ctrl0,
			this->_x + this->_feedbacklenght*l0  + this->_feedbackwidth ,
			this->_y - this->_feedbackwidth,
			this->_x + this->_feedbacklenght*l0 + this->_feedbackwidth - 0.01f,
			this->_y + this->_feedbackwidth,
			1, this->_color_threshold);
	if(ctrl0n != ctrl0)
		CSmrEngine::SetShape("feedback", "0110_threshold0", ctrl0n);

	dtk_hshape ctrl1 = CSmrEngine::GetShape("feedback", "0110_threshold1");
	dtk_hshape ctrl1n = dtk_create_rectangle_2p(ctrl1,
			this->_x -this->_feedbacklenght*l1 - this->_feedbackwidth,
			this->_y -this->_feedbackwidth,
			this->_x -this->_feedbacklenght*l1 - this->_feedbackwidth + 0.01f,
			this->_y +this->_feedbackwidth,
			1, this->_color_threshold);
	if(ctrl1n != ctrl1)
		CSmrEngine::SetShape("feedback", "0110_threshold1", ctrl1n);

	if(this->_classes >= 3) {
		dtk_hshape ctrl2 = CSmrEngine::GetShape("feedback", "0110_threshold2");
		dtk_hshape ctrl2n = dtk_create_rectangle_2p(ctrl2,
				this->_x + this->_feedbackwidth,
				this->_y + this->_feedbacklenght*l2 + this->_feedbackwidth,
				this->_x - this->_feedbackwidth,
				this->_y + this->_feedbacklenght*l2 + 
				this->_feedbackwidth - 0.01f,
				1, this->_color_threshold);
		
		if(ctrl2n != ctrl2)
			CSmrEngine::SetShape("feedback", "0110_threshold2", ctrl2n);
	}

	if(this->_classes == 4) {
		dtk_hshape ctrl3 = CSmrEngine::GetShape("feedback", "0110_threshold3");
		dtk_hshape ctrl3n = dtk_create_rectangle_2p(ctrl3,
				this->_x - this->_feedbackwidth,
				this->_y - this->_feedbacklenght*l3 - this->_feedbackwidth,
				this->_x + this->_feedbackwidth,
				this->_y - this->_feedbacklenght*l3-this->_feedbackwidth+0.01f,
				1, this->_color_threshold);
		if(ctrl3n != ctrl3)
			CSmrEngine::SetShape("feedback", "0110_threshold3", ctrl3n);
	}
}
		
void CSmrBars::RenderArrows(float kwidth, float klenght) {
	dtk_hshape arrow0 = CSmrEngine::GetShape("feedback", "0300_arrow0");
	dtk_hshape arrow0n = dtk_create_triangle(arrow0,
			this->_x + this->_feedbacklenght + this->_feedbackwidth,
			this->_y + kwidth*this->_feedbackwidth,
			this->_x + this->_feedbacklenght + this->_feedbackwidth,
			this->_y - kwidth*this->_feedbackwidth,
			this->_x + this->_feedbacklenght + klenght*this->_feedbackwidth,
			this->_y,
			1, this->_colors_arrow[0]);
	if(arrow0n != arrow0)
		CSmrEngine::SetShape("feedback", "0300_arrow0", arrow0n);
	
	dtk_hshape arrow1 = CSmrEngine::GetShape("feedback", "0300_arrow1");
	dtk_hshape arrow1n = dtk_create_triangle(arrow1,
			this->_x - this->_feedbacklenght - this->_feedbackwidth,
			this->_y + kwidth*this->_feedbackwidth,
			this->_x - this->_feedbacklenght - this->_feedbackwidth,
			this->_y - kwidth*this->_feedbackwidth,
			this->_x - this->_feedbacklenght - klenght*this->_feedbackwidth,
			this->_y,
			1, this->_colors_arrow[1]);
	if(arrow1n != arrow1)
		CSmrEngine::SetShape("feedback", "0300_arrow1", arrow1n);
	
	if(this->_classes >= 3) {
		dtk_hshape arrow2 = CSmrEngine::GetShape("feedback", "0300_arrow2");
		dtk_hshape arrow2n = dtk_create_triangle(arrow2,
				this->_x + kwidth*this->_feedbackwidth,
				this->_y + this->_feedbacklenght + this->_feedbackwidth,
				this->_x - kwidth*this->_feedbackwidth,
				this->_y + this->_feedbacklenght + this->_feedbackwidth,
				this->_x,
				this->_y + this->_feedbacklenght + klenght*this->_feedbackwidth,
				1, this->_colors_arrow[2]);
		if(arrow2n != arrow2)
			CSmrEngine::SetShape("feedback", "0300_arrow2", arrow2n);
	}
	
	if(this->_classes >= 4) {
		dtk_hshape arrow3 = CSmrEngine::GetShape("feedback", "0300_arrow3");
		dtk_hshape arrow3n = dtk_create_triangle(arrow3,
				this->_x + kwidth*this->_feedbackwidth,
				this->_y - this->_feedbacklenght - this->_feedbackwidth,
				this->_x - kwidth*this->_feedbackwidth,
				this->_y - this->_feedbacklenght - this->_feedbackwidth,
				this->_x,
				this->_y - this->_feedbacklenght - klenght*this->_feedbackwidth,
				1, this->_colors_arrow[3]);
		if(arrow3n != arrow3)
			CSmrEngine::SetShape("feedback", "0300_arrow3", arrow3n);
	}
	// Tails
	
	dtk_hshape tail0 = CSmrEngine::GetShape("feedback", "0300_tail0");
	dtk_hshape tail0n = dtk_create_rectangle_2p(tail0,
			this->_x +this->_feedbacklenght * this->_threshold[0] + 
				this->_feedbackwidth,
			this->_y -this->_feedbackwidth,
			this->_x +this->_feedbacklenght + this->_feedbackwidth,
			this->_y +this->_feedbackwidth,
			1, this->_colors_arrow[0]);
	if(tail0n != tail0)
		CSmrEngine::SetShape("feedback", "0300_tail0", tail0n);
	
	dtk_hshape tail1 = CSmrEngine::GetShape("feedback", "0300_tail1");
	dtk_hshape tail1n = dtk_create_rectangle_2p(tail1,
			this->_x -this->_feedbacklenght * this->_threshold[1] - 
				this->_feedbackwidth,
			this->_y -this->_feedbackwidth,
			this->_x -this->_feedbacklenght - this->_feedbackwidth,
			this->_y +this->_feedbackwidth,
			1, this->_colors_arrow[1]);
	if(tail1n != tail1)
		CSmrEngine::SetShape("feedback", "0300_tail1", tail1n);
	
	if(this->_classes >= 3) {
		dtk_hshape tail2 = CSmrEngine::GetShape("feedback", "0300_tail2");
		dtk_hshape tail2n = dtk_create_rectangle_2p(tail2,
			this->_x -this->_feedbackwidth,
			this->_y +this->_feedbacklenght * this->_threshold[2] + 
				this->_feedbackwidth,
			this->_x +this->_feedbackwidth,
			this->_y +this->_feedbacklenght + this->_feedbackwidth,
				1, this->_colors_arrow[2]);
		if(tail2n != tail2)
			CSmrEngine::SetShape("feedback", "0300_tail2", tail2n);
	}
	
	if(this->_classes == 4) {
		dtk_hshape tail3 = CSmrEngine::GetShape("feedback", "0300_tail3");
		dtk_hshape tail3n = dtk_create_rectangle_2p(tail3,
			this->_x -this->_feedbackwidth,
			this->_y -this->_feedbacklenght * this->_threshold[3] - 
				this->_feedbackwidth,
			this->_x +this->_feedbackwidth,
			this->_y -this->_feedbacklenght - this->_feedbackwidth,
				1, this->_colors_arrow[3]);
		if(tail3n != tail3)
			CSmrEngine::SetShape("feedback", "0300_tail3", tail3n);
	}
}

void CSmrBars::RenderBoom(const float* color, float alpha) {
	float rgb[4];
	memcpy(rgb, color, 4 * sizeof(float));
	rgb[3] = alpha;

	dtk_hshape boomC = CSmrEngine::GetShape("feedback", "0400_boom");
	dtk_hshape boomCn  = dtk_create_circle(boomC, 
			this->_x,
			this->_y,
			this->_boomradius,
			1, rgb, 200);

	if(boomCn != boomC)
		CSmrEngine::SetShape("feedback", "0400_boom", boomCn);
}

void CSmrBars::RenderWait(unsigned int step) {
	float size = 0.20f;
	float rad = 1.50f * (CSmrBars::_boomradius - 
			CSmrBars::_boomradius * size); 
	
	float cos30 = 0.8660f;
	float sin30 = 0.5000f;
	float cos60 = 0.5000f;
	float sin60 = 0.8660f;

	float rgbU[4], rgbS[4];
	memcpy(rgbU, CSmrBars::_color_threshold, 4*sizeof(float));
	memcpy(rgbS, CSmrBars::_color_threshold, 4*sizeof(float));
	
	float alphaU = 0.25f;
	float alphaS = 0.80f;

	rgbU[3] = (step == 0 ? alphaU : alphaU);
	rgbS[3] = (step == 0 ? CSmrBars::ShapeHidden : alphaS);

	dtk_hshape step01 = CSmrEngine::GetShape("feedback", "2000_step01");
	dtk_hshape step01N = dtk_create_circle(step01, 
			CSmrBars::_x,
			CSmrBars::_y + rad,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 1 ? rgbS : rgbU,
			400);
	
	dtk_hshape step02 = CSmrEngine::GetShape("feedback", "2000_step02");
	dtk_hshape step02N = dtk_create_circle(step02, 
			CSmrBars::_x + rad * cos60,
			CSmrBars::_y + rad * sin60,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 2 ? rgbS : rgbU,
			400);
	
	dtk_hshape step03 = CSmrEngine::GetShape("feedback", "2000_step03");
	dtk_hshape step03N = dtk_create_circle(step03, 
			CSmrBars::_x + rad * cos30,
			CSmrBars::_y + rad * sin30,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 3 ? rgbS : rgbU,
			400);

	dtk_hshape step04 = CSmrEngine::GetShape("feedback", "2000_step04");
	dtk_hshape step04N = dtk_create_circle(step04, 
			CSmrBars::_x + rad,
			CSmrBars::_y,
			CSmrBars::_boomradius * size, 
			1,
			step >= 4 ? rgbS : rgbU,
			400);
	
	dtk_hshape step05 = CSmrEngine::GetShape("feedback", "2000_step05");
	dtk_hshape step05N = dtk_create_circle(step05, 
			CSmrBars::_x + rad * cos30,
			CSmrBars::_y - rad * sin30,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 5 ? rgbS : rgbU,
			400);
	
	dtk_hshape step06 = CSmrEngine::GetShape("feedback", "2000_step06");
	dtk_hshape step06N = dtk_create_circle(step06, 
			CSmrBars::_x + rad * cos60,
			CSmrBars::_y - rad * sin60,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 6 ? rgbS : rgbU,
			400);

	dtk_hshape step07 = CSmrEngine::GetShape("feedback", "2000_step07");
	dtk_hshape step07N = dtk_create_circle(step07, 
			CSmrBars::_x,
			CSmrBars::_y - rad,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 7 ? rgbS : rgbU,
			400);
	
	dtk_hshape step08 = CSmrEngine::GetShape("feedback", "2000_step08");
	dtk_hshape step08N = dtk_create_circle(step08, 
			CSmrBars::_x - rad * cos60,
			CSmrBars::_y - rad * sin60,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 8 ? rgbS : rgbU,
			400);
	
	dtk_hshape step09 = CSmrEngine::GetShape("feedback", "2000_step09");
	dtk_hshape step09N = dtk_create_circle(step09, 
			CSmrBars::_x - rad * cos30,
			CSmrBars::_y - rad * sin30,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 9 ? rgbS : rgbU,
			400);

	dtk_hshape step10 = CSmrEngine::GetShape("feedback", "2000_step10");
	dtk_hshape step10N = dtk_create_circle(step10, 
			CSmrBars::_x - rad,
			CSmrBars::_y,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 10 ? rgbS : rgbU,
			400);
	
	dtk_hshape step11 = CSmrEngine::GetShape("feedback", "2000_step11");
	dtk_hshape step11N = dtk_create_circle(step11, 
			CSmrBars::_x - rad * cos30,
			CSmrBars::_y + rad * sin30,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 11 ? rgbS : rgbU,
			400);
	
	dtk_hshape step12 = CSmrEngine::GetShape("feedback", "2000_step12");
	dtk_hshape step12N = dtk_create_circle(step12, 
			CSmrBars::_x - rad * cos60,
			CSmrBars::_y + rad * sin60,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 12 ? rgbS : rgbU,
			400);

	if(step01N != step01)
		CSmrEngine::SetShape("wait", "2000_step01", step01N);
	if(step02N != step02)
		CSmrEngine::SetShape("wait", "2000_step02", step02N);
	if(step03N != step03)
		CSmrEngine::SetShape("wait", "2000_step03", step03N);
	if(step04N != step04)
		CSmrEngine::SetShape("wait", "2000_step04", step04N);
	if(step05N != step05)
		CSmrEngine::SetShape("wait", "2000_step05", step05N);
	if(step06N != step06)
		CSmrEngine::SetShape("wait", "2000_step06", step06N);
	if(step07N != step07)
		CSmrEngine::SetShape("wait", "2000_step07", step07N);
	if(step08N != step08)
		CSmrEngine::SetShape("wait", "2000_step08", step08N);
	if(step09N != step09)
		CSmrEngine::SetShape("wait", "2000_step09", step09N);
	if(step10N != step10)
		CSmrEngine::SetShape("wait", "2000_step10", step10N);
	if(step11N != step11)
		CSmrEngine::SetShape("wait", "2000_step11", step11N);
	if(step12N != step12)
		CSmrEngine::SetShape("wait", "2000_step12", step12N);
}
		
void CSmrBars::ShowWait(float step) {
	CSmrEngine::DataWait();
	if(step == 0.00f)
		this->RenderWait(0);
	else
		this->RenderWait((unsigned int)(11 * step + 1));
	CSmrEngine::DataPost();
	
	CSmrEngine::RequestUpdate();
}

void CSmrBars::RenderBegin(void) {
	dtk_hshape fg = CSmrEngine::GetShape("start", "0000_start_fg");
	dtk_hshape fgN = dtk_create_circle(fg, 0.00f, 0.00f, 
			CSmrBars::_boomradius * 1.00f, 
			1, dtk_chameleon_dark, 400);
	if(fgN != fg)
		CSmrEngine::SetShape("start", "0000_start_fg", fgN);
	
	dtk_hshape bg = CSmrEngine::GetShape("start", "0000_start_bg");
	dtk_hshape bgN = dtk_create_circle(bg, 0.00f, 0.00f, 
			CSmrBars::_boomradius * 1.05f,
			1, dtk_aluminium_light, 400);
	if(bgN != bg)
		CSmrEngine::SetShape("start", "0000_start_bg", bgN);
}

void CSmrBars::RenderEnd(void) {
	dtk_hshape fg = CSmrEngine::GetShape("stop", "0000_stop_fg");
	dtk_hshape fgN = dtk_create_circle(fg, 0.00f, 0.00f, 
			CSmrBars::_boomradius * 1.00f, 
			1, dtk_scarletred_dark, 400);
	if(fgN != fg)
		CSmrEngine::SetShape("stop", "0000_stop_fg", fgN);
	
	dtk_hshape bg = CSmrEngine::GetShape("stop", "0000_stop_bg");
	dtk_hshape bgN = dtk_create_circle(bg, 0.00f, 0.00f, 
			CSmrBars::_boomradius * 1.05f,
			1, dtk_aluminium_light, 400);
	if(bgN != bg)
		CSmrEngine::SetShape("stop", "0000_stop_bg", bgN);
}

void CSmrBars::Open(void) {
	this->Setup();
	CSmrEngineExt::Open();
}

void CSmrBars::Close(void) {
	CSmrEngineExt::Close();
	this->Teardown();
}
		
#endif
