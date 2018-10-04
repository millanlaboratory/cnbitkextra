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

#include <ndf/ndf_codec.h>
#include <cnbicore/CcBasic.hpp>
#include <cnbicore/CcPipe.hpp>
#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiId.hpp>
#include <cnbiprotocol/CpTriggerFactory.hpp>
#include <cnbismr/CSmrEngineExt.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

using namespace std;

void usage(void) { 
	printf("Usage: lc_eventcheck -p PIPENAME\n");
	exit(1);
}

int main(int argc, char* argv[]) {
	int opt;
	std::string optname, pipename;
	
	while((opt = getopt(argc, argv, "p:h")) != -1) {
		if(opt == 'p')
			optname.assign(optarg);
		else {
			usage();
			return(opt == 'h') ? EXIT_SUCCESS : EXIT_FAILURE;
		}
	}

	if(optname.empty())
		usage();
	
	CcCore::OpenLogger("lc_eventcheck");
	CcCore::CatchSIGINT();
	CcCore::CatchSIGTERM();
	ClLoop::Configure();
	
	// Connect to loop
	CcLogInfo("Connecting to loop...");
	while(ClLoop::Connect() == false) {
		if(CcCore::receivedSIGAny.Get())
			exit(1);
		CcTime::Sleep(2000);
	}
	
	if(ClLoop::nms.Query(optname, &pipename) != ClNmsLang::Successful) {
		CcLogFatal("Cannot query pipe");
		exit(2);
	}

	ClTobiId idSet(ClTobiId::SetOnly);
	ClTobiId idGet(ClTobiId::GetOnly);
	IDMessage idm;
	IDSerializerRapid ids(&idm);
	idm.SetDescription("lc_eventcheck");
	idm.SetFamilyType(IDMessage::FamilyBiosig);
	
	
	// Attach iD
	if(idSet.Attach("/bus") == false) {
		CcLogFatal("Cannot attach iD sender");
		exit(3);
	}
	if(idGet.Attach("/bus") == false) {
		CcLogFatal("Cannot attach iD receiver");
		exit(4);
	}
	
	uint32_t ndfevent0 = 0;
	uint32_t ndfevent1 = 0;
	uint32_t ideventSet = 0;
	uint32_t ideventGet = 0;
	bool settling = true;

	size_t rsize = 0;
	ndf_frame frame;
	ndf_ack ack;
	CcPipe reader;
	if(reader.Open(pipename, CcPipe::Reader) == false) {
		CcLogFatal("Cannot open pipe");
		goto shutdown;
	}
	
	ack.bsize = reader.Read(&ack.buffer, NDF_ACK_SIZET);
	if(ack.bsize <= 0) {
		CcLogFatal("Source is down");
		exit(1);
	}
	if(ack.bsize != NDF_ACK_SIZET) {
		CcLogFatal("ACK not received correctly");
		exit(2);
	}

	CcLogInfo("ACK received, initializing NDF frame");
	ndf_initack(&frame, &ack);
	
	CcLogInfo("Receiving...");
	while(true) { 
		fflush(stdout);
		idm.SetEvent(++ideventSet);
		idSet.SetMessage(&ids);

		rsize = reader.Read(frame.data.buffer, frame.data.bsize);
		if(frame.data.bsize != rsize) {
			CcLogWarning("Broken pipe");
			goto shutdown;
		}
		
		
		ndfevent0 = 0;
		ndfevent1 = 0;
		ndf_get_label(&frame, (void*)&ndfevent0, 0);
		ndf_get_label(&frame, (void*)&ndfevent1, 1);

		if(ndfevent0 != 0 && ndfevent1 == 0) {
			while(idGet.GetMessage(&ids) == true);
			ideventGet = idm.GetEvent();
			printf("iDout=%-6u iDin=%-6u NDF=%-6u", 
					ideventSet, ideventGet, ndfevent0);
			printf("[iDout-iDin]=%u [iDout-NDF]=%u [iDin-NDF]=%u\r", 
					ideventSet - ideventGet,
					ideventSet - ndfevent0,
					ideventGet - ndfevent0);
			settling = false;
		} else if(ndfevent0 == 0 && ndfevent1 == 0) {
			if(settling == true) {
				CcLogWarning("NDF carries no labels: loop settling");
			} else {
				CcLogFatal("NDF carries no labels: loop broken");
				goto shutdown;
			}
		} else if(ndfevent0 != 0 && ndfevent1 != 0) {
			CcLogFatal("NDF carries multiple labels: loop broken");
			goto shutdown;
		}

		if(CcCore::receivedSIGAny.Get())
			break;
	}

shutdown:
	fflush(stdout);
	printf("\n");
	idSet.Detach();
	idGet.Detach();
	reader.Close();
	ndf_free(&frame);
	CcCore::Exit(0);
}

