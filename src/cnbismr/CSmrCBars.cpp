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

#include "CSmrCBars.hpp"
#include <string.h>

CSmrCBars::CSmrCBars(int winw, int winh, CpTrigger* trigger) :
	CSmrBars(winw, winh, trigger) {
	SetThrottle();
}

CSmrCBars::CSmrCBars(CCfgConfig* configuration, CpTrigger* trigger) :
	CSmrBars(configuration, trigger) {
	SetThrottle();
}

CSmrCBars::~CSmrCBars(void) {
}

void CSmrCBars::SetThrottle(float max) {
	this->_throttlemax = max;
}

void CSmrCBars::Setup(void) {
	CSmrBars::Setup();
	
	CSmrEngine::AddShape("feedback", "0010_limitI"); 
	CSmrEngine::AddShape("feedback", "0011_limitO"); 
	CSmrEngine::AddShape("feedback", "0012_target0"); 
	CSmrEngine::AddShape("feedback", "0410_throttle"); 
	
	CSmrEngine::AddShape("feedback", "0505_tstep01"); 
	CSmrEngine::AddShape("feedback", "0505_tstep02"); 
	CSmrEngine::AddShape("feedback", "0505_tstep03"); 
	CSmrEngine::AddShape("feedback", "0505_tstep04"); 
	CSmrEngine::AddShape("feedback", "0505_tstep05"); 
	CSmrEngine::AddShape("feedback", "0505_tstep06"); 
	CSmrEngine::AddShape("feedback", "0505_tstep07"); 
	CSmrEngine::AddShape("feedback", "0505_tstep08"); 
	CSmrEngine::AddShape("feedback", "0505_tstep09"); 
	CSmrEngine::AddShape("feedback", "0505_tstep10"); 
	CSmrEngine::AddShape("feedback", "0505_tstep11"); 
	CSmrEngine::AddShape("feedback", "0505_tstep12"); 

	CSmrEngine::DataWait();
	this->RenderThrottle(0.00f, CSmrBars::ShapeHidden, 
			CSmrBars::ArrowRight);
	this->RenderLimit(CSmrBars::ShapeHidden);
	this->RenderTarget(0.00f, CSmrBars::ShapeHidden);
	this->RenderTeaser(0);
	CSmrEngine::DataPost();
}

void CSmrCBars::ControlContinuous(CpProbability* prob, CSmrBarsClass sclass, 
		float reliability, float target) {
	CSmrEngine::DataWait();
	this->RenderFixation(CSmrBars::ShapeHidden);
	this->RenderCue(this->_cuerotations[sclass], this->_color_fixation,
			CSmrBars::ShapeVisible);
	this->RenderThrottle(prob->Get(sclass), reliability, sclass);
	this->RenderLimit(CSmrBars::ShapeVisible);
	if(target == CSmrBars::ShapeHidden)
		this->RenderTarget(target, CSmrBars::ShapeHidden);
	else	
		this->RenderTarget(target, CSmrBars::ShapeVisible);
	CSmrEngine::DataPost();
	
	CSmrEngine::RequestUpdate();
}

void CSmrCBars::DisableContinuous(void) {
	CSmrEngine::DataWait();
	this->RenderCue(this->_cuerotations[CSmrBars::ArrowRight], 
			this->_color_fixation, CSmrBars::ShapeHidden);
	this->RenderThrottle(0.00f, CSmrBars::ShapeHidden, 
			CSmrBars::ArrowRight);
	this->RenderLimit(CSmrBars::ShapeHidden);
	this->RenderTarget(0.00f, CSmrBars::ShapeHidden);
	CSmrEngine::DataPost();
	
	CSmrEngine::RequestUpdate();
}

void CSmrCBars::ShowTeaser(float step) {
	CSmrEngine::DataWait();
	this->RenderLimit(step);
	this->RenderFixation(CSmrBars::ShapeHidden);
	
	if(step == 0.00f)
		this->RenderTeaser(0);
	else
		this->RenderTeaser((unsigned int)(11 * step + 1));
	
	CSmrEngine::DataPost();
	
	CSmrEngine::RequestUpdate();
}

void CSmrCBars::HideTeaser(void) {
	CSmrEngine::DataWait();
	this->RenderTeaser(0);
	this->RenderLimit(CSmrBars::ShapeHidden);
	CSmrEngine::DataPost();
	//this->RenderFixation(CSmrBars::ShapeVisible);
	CSmrEngine::RequestUpdate();
}

void CSmrCBars::RenderThrottle(float prob, float reliability,
		CSmrBarsClass sclass) {
	float rgb_throttle[4];
	
	memcpy(rgb_throttle, CSmrBars::_colors_arrow[sclass], 4 * sizeof(float));

	/*
	rgb_throttle[3] = (0.45f + reliability * 0.35f) * 
		(prob == 0.00f ? 0.00f : 1.00f);
	*/
	rgb_throttle[3] = reliability; 

	dtk_hshape throttle = CSmrEngine::GetShape("feedback", "0410_throttle");
	dtk_hshape throttleN  = dtk_create_circle(throttle, 
			CSmrBars::_x,
			CSmrBars::_y,
			CSmrBars::_boomradius * (1 + prob * (this->_throttlemax - 1)),
			1, rgb_throttle, 200);

	if(throttleN != throttle)
		CSmrEngine::SetShape("feedback", "0410_throttle", throttleN);
}

void CSmrCBars::RenderLimit(CSmrBarsAlpha alpha) {
	float rgbI[4], rgbO[4] = {0};
	memcpy(rgbI, CSmrBars::_color_bars, 4*sizeof(float));
	rgbI[3] = alpha;
	rgbO[3] = alpha;
	
	dtk_hshape inner = CSmrEngine::GetShape("feedback", "0010_limitI");
	dtk_hshape innerN = dtk_create_circle(inner, 
			CSmrBars::_x,
			CSmrBars::_y,
			CSmrBars::_boomradius * this->_throttlemax, 
			1, rgbI, 400);
	
	dtk_hshape outer = CSmrEngine::GetShape("feedback", "0010_limitO");
	dtk_hshape outerN = dtk_create_circle(outer, 
			CSmrBars::_x,
			CSmrBars::_y,
			CSmrBars::_boomradius * this->_throttlemax - 0.007, 
			1, rgbO, 400);
	
	if(innerN != inner)
		CSmrEngine::SetShape("feedback", "0010_limitI", innerN);
	if(outerN != outer)
		CSmrEngine::SetShape("feedback", "0011_limitO", outerN);
}

void CSmrCBars::RenderTarget(float prob, CSmrBarsAlpha alpha) {
	float rgb[4];
	memcpy(rgb, CSmrBars::_color_threshold, 4*sizeof(float));
	rgb[3] = alpha;
	
	dtk_hshape limit = CSmrEngine::GetShape("feedback", "0012_target0");
	dtk_hshape limitN  = dtk_create_circle(limit, 
			CSmrBars::_x,
			CSmrBars::_y,
			CSmrBars::_boomradius +
				CSmrBars::_boomradius * (this->_throttlemax - 1) * prob,
			0, rgb, 200);
	
	if(limitN != limit)
		CSmrEngine::SetShape("feedback", "0012_target0", limitN);
}


void CSmrCBars::RenderTeaser(unsigned int step) {
	float size = 0.10f;
	float rad = 0.70f * (CSmrBars::_boomradius - 
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

	rgbU[3] = (step == 0 ? CSmrBars::ShapeHidden : alphaU);
	rgbS[3] = (step == 0 ? CSmrBars::ShapeHidden : alphaS);

	dtk_hshape step01 = CSmrEngine::GetShape("feedback", "0505_tstep01");
	dtk_hshape step01N = dtk_create_circle(step01, 
			CSmrBars::_x,
			CSmrBars::_y + rad,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 1 ? rgbS : rgbU,
			400);
	
	dtk_hshape step02 = CSmrEngine::GetShape("feedback", "0505_tstep02");
	dtk_hshape step02N = dtk_create_circle(step02, 
			CSmrBars::_x + rad * cos60,
			CSmrBars::_y + rad * sin60,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 2 ? rgbS : rgbU,
			400);
	
	dtk_hshape step03 = CSmrEngine::GetShape("feedback", "0505_tstep03");
	dtk_hshape step03N = dtk_create_circle(step03, 
			CSmrBars::_x + rad * cos30,
			CSmrBars::_y + rad * sin30,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 3 ? rgbS : rgbU,
			400);

	dtk_hshape step04 = CSmrEngine::GetShape("feedback", "0505_tstep04");
	dtk_hshape step04N = dtk_create_circle(step04, 
			CSmrBars::_x + rad,
			CSmrBars::_y,
			CSmrBars::_boomradius * size, 
			1,
			step >= 4 ? rgbS : rgbU,
			400);
	
	dtk_hshape step05 = CSmrEngine::GetShape("feedback", "0505_tstep05");
	dtk_hshape step05N = dtk_create_circle(step05, 
			CSmrBars::_x + rad * cos30,
			CSmrBars::_y - rad * sin30,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 5 ? rgbS : rgbU,
			400);
	
	dtk_hshape step06 = CSmrEngine::GetShape("feedback", "0505_tstep06");
	dtk_hshape step06N = dtk_create_circle(step06, 
			CSmrBars::_x + rad * cos60,
			CSmrBars::_y - rad * sin60,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 6 ? rgbS : rgbU,
			400);

	dtk_hshape step07 = CSmrEngine::GetShape("feedback", "0505_tstep07");
	dtk_hshape step07N = dtk_create_circle(step07, 
			CSmrBars::_x,
			CSmrBars::_y - rad,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 7 ? rgbS : rgbU,
			400);
	
	dtk_hshape step08 = CSmrEngine::GetShape("feedback", "0505_tstep08");
	dtk_hshape step08N = dtk_create_circle(step08, 
			CSmrBars::_x - rad * cos60,
			CSmrBars::_y - rad * sin60,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 8 ? rgbS : rgbU,
			400);
	
	dtk_hshape step09 = CSmrEngine::GetShape("feedback", "0505_tstep09");
	dtk_hshape step09N = dtk_create_circle(step09, 
			CSmrBars::_x - rad * cos30,
			CSmrBars::_y - rad * sin30,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 9 ? rgbS : rgbU,
			400);

	dtk_hshape step10 = CSmrEngine::GetShape("feedback", "0505_tstep10");
	dtk_hshape step10N = dtk_create_circle(step10, 
			CSmrBars::_x - rad,
			CSmrBars::_y,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 10 ? rgbS : rgbU,
			400);
	
	dtk_hshape step11 = CSmrEngine::GetShape("feedback", "0505_tstep11");
	dtk_hshape step11N = dtk_create_circle(step11, 
			CSmrBars::_x - rad * cos30,
			CSmrBars::_y + rad * sin30,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 11 ? rgbS : rgbU,
			400);
	
	dtk_hshape step12 = CSmrEngine::GetShape("feedback", "0505_tstep12");
	dtk_hshape step12N = dtk_create_circle(step12, 
			CSmrBars::_x - rad * cos60,
			CSmrBars::_y + rad * sin60,
			CSmrBars::_boomradius * size, 
			1, 
			step >= 12 ? rgbS : rgbU,
			400);

	if(step01N != step01)
		CSmrEngine::SetShape("feedback", "0505_tstep01", step01N);
	if(step02N != step02)
		CSmrEngine::SetShape("feedback", "0505_tstep02", step02N);
	if(step03N != step03)
		CSmrEngine::SetShape("feedback", "0505_tstep03", step03N);
	if(step04N != step04)
		CSmrEngine::SetShape("feedback", "0505_tstep04", step04N);
	if(step05N != step05)
		CSmrEngine::SetShape("feedback", "0505_tstep05", step05N);
	if(step06N != step06)
		CSmrEngine::SetShape("feedback", "0505_tstep06", step06N);
	if(step07N != step07)
		CSmrEngine::SetShape("feedback", "0505_tstep07", step07N);
	if(step08N != step08)
		CSmrEngine::SetShape("feedback", "0505_tstep08", step08N);
	if(step09N != step09)
		CSmrEngine::SetShape("feedback", "0505_tstep09", step09N);
	if(step10N != step10)
		CSmrEngine::SetShape("feedback", "0505_tstep10", step10N);
	if(step11N != step11)
		CSmrEngine::SetShape("feedback", "0505_tstep11", step11N);
	if(step12N != step12)
		CSmrEngine::SetShape("feedback", "0505_tstep12", step12N);
}
