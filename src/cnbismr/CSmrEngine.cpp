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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "CSmrEngine.hpp"

using namespace std;
using std::pair;
using std::make_pair;

CSmrEngine::CSmrEngine(const std::string& caption, int width, int height, int bpp){
	this->Init();
	this->_windowX = 0;
	this->_windowY = 0;
	this->_windowW = width;
	this->_windowH = height;
	this->_windowCaption = caption;
	this->_windowBpp = bpp;
}
		
CSmrEngine::CSmrEngine(CCfgConfig* configuration) {
	this->Init();
	try {
		this->_windowCaption =
			configuration->RootEx()->QuickStringEx("configuration/csmrengine/caption");
		this->_windowX =
			configuration->RootEx()->QuickIntEx("configuration/csmrengine/x");
		this->_windowY =
			configuration->RootEx()->QuickIntEx("configuration/csmrengine/y");
		this->_windowW =
			configuration->RootEx()->QuickIntEx("configuration/csmrengine/width");
		this->_windowH =
			configuration->RootEx()->QuickIntEx("configuration/csmrengine/height");
		this->_windowBpp =
			configuration->RootEx()->QuickIntEx("configuration/csmrengine/bpp");
	} catch(XMLException e) {
		CcLogErrorS("Configuration failed, using default values: " << e.Info());
		this->_windowCaption = "libcnbismr::CSmrEngine";
		this->_windowX = 0;
		this->_windowY = 0;
		this->_windowW = 800;
		this->_windowH = 600;
		this->_windowBpp = 24;
	}
}

CSmrEngine::~CSmrEngine() {
}

void CSmrEngine::Init(void) {
	this->_semscene.Wait();
	this->_semupdate.Wait();
	this->_semwindow.Wait();
	this->_windowPtr = NULL;
}

bool CSmrEngine::AddScene(const std::string& scene) {
	if(CcThread::IsRunning()) {
		CcLogError("Thread is running");
		return false;
	}

	this->_semdata.Wait();
	bool status = this->HasScene_Unsafe(scene);
	if(!status)
		this->_data.insert(make_pair(scene, CSmrShapes()));
	this->_semdata.Post();
	
	return !status;
}

bool CSmrEngine::HasScene(const std::string& scene) {
	this->_semdata.Wait();
	bool status = this->HasScene_Unsafe(scene);
	this->_semdata.Post();
	
	return status;
}

void CSmrEngine::DestroyScene(const std::string& scene) {
	if(CcThread::IsRunning()) {
		CcLogError("Thread is running");
		return;
	}

	this->_semdata.Wait();
	if(this->HasScene_Unsafe(scene)) {
		CSmrShapes::iterator itshape;
		itshape = this->_data[scene].begin();
		while(itshape != this->_data[scene].end()) {
			dtk_destroy_shape(itshape->second);
			++itshape;
		}
		this->_data.erase(scene);
	}
	this->_semdata.Post();
}
		
bool CSmrEngine::AddShape(const std::string& scene, const std::string& shape, dtk_hshape hshape) {
	if(CcThread::IsRunning()) {
		CcLogError("Thread is running");
		return false;
	}
	
	this->_semdata.Wait();
	if(this->HasShape_Unsafe(scene, shape) == false)
		this->_data[scene].insert(make_pair(shape, hshape));
	this->_semdata.Post();

	return true;
}

bool CSmrEngine::HasShape(const std::string& SHAPE, const std::string& SCENE) {
	CSmrShapes::iterator itshape;

	this->_semdata.Wait();
	itshape = this->_data[SCENE].find(SHAPE);
	bool status = (itshape != this->_data[SCENE].end());
	this->_semdata.Post();

	return status;
}

void CSmrEngine::DestroyShape(const std::string& scene, const std::string& shape) {
	if(CcThread::IsRunning()) {
		CcLogError("Thread is running");
		return;
	}
	
	
	this->_semdata.Wait();
	CSmrShapes::iterator itshape;
	if(this->HasShape_Unsafe(scene, shape)) {
		itshape = this->_data[scene].find(shape);
		
		if(itshape->second != NULL) {
			dtk_destroy_shape(itshape->second);
			this->_data[scene].erase(shape);
		}
		else 
			CcLogWarningS(scene << "::" << itshape->first << " is NULL");
	}
	this->_semdata.Post();
}

dtk_hshape CSmrEngine::GetShape(const std::string& scene, const std::string& shape) {
	if(this->HasShape_Unsafe(scene, shape) == false)
		return NULL;
	return (this->_data[scene].find(shape))->second;
}

dtk_hshape CSmrEngine::SetShape(const std::string& scene, const std::string& shape, 
		dtk_hshape hshape) {
	if(this->HasShape_Unsafe(scene, shape) == false)
		return NULL;
	return (this->_data[scene].find(shape))->second = hshape;
}

void CSmrEngine::BindShapes(void) {
	CSmrScenes::iterator itscene;
	CSmrShapes::iterator itshape;

	this->_semdata.Wait();
	if(this->_windowPtr == NULL) {
		CcLogError("Window pointer is NULL");
		return;
	}	

	if(!this->_data.empty()) {
		itscene = this->_data.begin();
		while(itscene != this->_data.end()) {
			for(itshape = this->_data[itscene->first].begin(); 
					itshape != this->_data[itscene->first].end();
					++itshape) {
				//if(itshape->second != NULL) 
				//	dtk_bind_shape_to_window(itshape->second, this->_windowPtr);
				//else
				//CcLogWarningS(itscene->first << "::" << itshape->first << 
				//		" is NULL");
			}
			++itscene;
		} 
	} 
	this->_semdata.Post();
}

void CSmrEngine::RenderScene(void) {
	this->_semdata.Wait();
	this->_semscene.Wait();
	std::string scene = this->_scene;
	this->_semscene.Post();

	CSmrShapes::iterator itshape;
	if(!this->_data[scene].empty())
		for(itshape = this->_data[scene].begin(); 
				itshape != this->_data[scene].end(); ++itshape) {
			if(itshape->second != NULL)
				dtk_draw_shape(itshape->second);
			else
				CcLogWarningS(scene << "::" << itshape->first << " is NULL");
		}
	this->_semdata.Post();
}

void CSmrEngine::MoveShape(const std::string& scene, const std::string& shape, float dx, float dy) {
	CSmrShapes::iterator itshape;

	this->_semdata.Wait();
	if(this->HasShape_Unsafe(scene, shape)) {
		itshape = this->_data[scene].find(shape);
		dtk_move_shape(itshape->second, dx, dy);
	}
	this->_semdata.Post();
	this->RequestUpdate();
}

void CSmrEngine::MoveShapeRel(const std::string& scene, const std::string& shape, float dx, float dy) {
	CSmrShapes::iterator itshape;

	this->_semdata.Wait();
	if(this->HasShape_Unsafe(scene, shape)) {
		itshape = this->_data[scene].find(shape);
		dtk_relmove_shape(itshape->second, dx, dy);
	}
	this->_semdata.Post();
	this->RequestUpdate();
}

void CSmrEngine::Main(void) {
	if(!CcThread::IsRunning())
		return;
		
	CcLogConfig("Creating OpenGL window");
	this->_windowPtr = dtk_create_window(this->_windowW, this->_windowH,
			this->_windowX, this->_windowY, 
			this->_windowBpp, this->_windowCaption.c_str());
	dtk_make_current_window(this->_windowPtr);
	this->_semwindow.Post();

	if(this->SetupForImages() == false) 
		this->BindShapes();

	CcLogConfig("Starting");
	this->_semscene.Post();
	while(CcThread::IsRunning()) {
		this->WaitUpdate();

		this->_semwindow.Wait();
		dtk_clear_screen(this->_windowPtr);
		this->RenderScene();
		dtk_update_screen(this->_windowPtr);
		this->i_OnDraw.Execute(this);
		this->_semwindow.Post();
	}
	
	CcLogConfig("Stopped");
	this->_semwindow.Wait();
	dtk_close(this->_windowPtr);
	this->_semwindow.Post();
}

void CSmrEngine::SwitchScene(const std::string& scene) {
	this->_semscene.Wait();
	if(this->HasScene_Unsafe(scene))
		this->_scene = scene;
	this->_semscene.Post();

	this->RequestUpdate();
}

void CSmrEngine::RequestUpdate() {
	this->_semupdate.Post();
}

void CSmrEngine::WaitUpdate() {
	this->_semupdate.Wait();
}

bool CSmrEngine::HasScene_Unsafe(const std::string& scene) {
	CSmrScenes::iterator itscene;

	itscene = this->_data.find(scene);
	return(itscene != this->_data.end());
}

bool CSmrEngine::HasShape_Unsafe(const std::string& scene, const std::string& shape) {
	CSmrShapes::iterator itshape;

	if(!this->HasScene_Unsafe(scene))
		return false;

	itshape = this->_data[scene].find(shape);
	return(itshape != this->_data[scene].end());
}

void CSmrEngine::DataWait(void) {
	this->_semdata.Wait();
}

void CSmrEngine::DataPost(void) {
	this->_semdata.Post();
}

void CSmrEngine::WindowWait(void) {
	this->_semwindow.Wait();
}

void CSmrEngine::WindowPost(void) {
	this->_semwindow.Post();
}

void CSmrEngine::Dump(void) {
	CSmrScenes::iterator itscene;
	CSmrShapes::iterator itshape;

	this->_semdata.Wait();
	printf("[CSmrEngine::Dump] Dumping internal structure:\n");
	if(!this->_data.empty()) {
		itscene = this->_data.begin();
		while(itscene != this->_data.end()) {
			printf(" [+] %s\n", itscene->first.c_str());

			for(itshape = this->_data[itscene->first].begin(); 
					itshape != this->_data[itscene->first].end();
					++itshape) {
				printf("  `-- %s\n", itshape->first.c_str());
			}
			++itscene;
		} 
	} else
		printf("  `-- Empty!\n");
	this->_semdata.Post();
}


dtk_hwnd CSmrEngine::GetWindow(void) {
	return this->_windowPtr;
}
		
void CSmrEngine::Open(void) {
	CcThread::Start();
}

void CSmrEngine::Close(void) {
	if(CcThread::IsRunning() == false)
		return;

	CcThread::Stop();
	this->RequestUpdate();
	CcThread::Join();
}

bool CSmrEngine::IsOpen(void) {
	return CcThread::IsRunning();
}
