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

#include <stdio.h>
#include <signal.h>

#include <libhybrid/CcByte.hpp>
#include <libhybrid/HbNetClient.hpp>
#include <libhybrid/HbNetServerSingle.hpp>
#include <libhybrid/HbTime.hpp>
#include <libhybrid/HbThread.hpp>

#define MESSAGE_SIZE			128
#define PROBABILITY_INCREMENT	0.005f

void cleantty(int signal) {
	if(system("stty cooked")) {}
}

void init_probs(float* probs, int classes) {
	memset(probs, 0, classes * sizeof(float));
	for(int i = 0; i < classes; i++)
		probs[i] = 1.00f/classes;
}

void update_hardprobs(float* probs, int classes, int k) {
	memset(probs, 0, sizeof(float) * classes);
	probs[k] = 1;
}

void update_softprobs(float* probs, int classes, int k) {
	for(int i = 0; i < classes; i++) {
		if(i == k)
			probs[i] += PROBABILITY_INCREMENT;
		else 
			probs[i] -= PROBABILITY_INCREMENT / (classes - 1);
	}

	if(probs[k] >= 1) {
		for(int i = 0; i < classes; i++) {
			if(i == k)
				probs[i] = 1.00f;
			else 
				probs[i] = 0.00f;
		}
	}
}

bool create_message(CcByte* message, float* probs, int classes) {
	memset(message, 0, MESSAGE_SIZE);
	
	switch(classes) {
		case 2:
			sprintf(message, "<postprobs>2:%f:%f</postprobs>\n",
					probs[0], probs[1]);
			break;
		case 3:
			sprintf(message, "<postprobs>3:%f:%f:%f</postprobs>\n",
					probs[0], probs[1], probs[2]);
			break;
		case 4:
			sprintf(message, "<postprobs>4:%f:%f:%f:%f</postprobs>\n",
					probs[0], probs[1], probs[2], probs[3]);
			break;
		default:
			return false;
	}
	return true;
}

class HandlerNet : public HbNetObjectProxy {
	public:
		void HandleRecv(HbNetObject* caller, CcByte* message) { 
			string buffer;
			HbStreamer* stream = ((HbNetClient*)caller)->stream;
		}
		
		void HandleDisconnect(HbNetObject* caller) { 
			cleantty(0);
			printf("\n[HandlerNet] Disconnected\n");
			exit(1);
		}

		void HandleAccept(HbNetObject* caller) { 
			//printf("[HandlerNet] Accepted\n");
		}

		void HandleDrop(HbNetObject* caller) { 
			cleantty(0);
			printf("\n[HandlerNet] Dropped\n");
			exit(0);
		}
};

class SluttyThread : public HbThread { 
	public:
		SluttyThread(unsigned int classes, 
				HbNetClient* client = NULL, 
				HbNetServerSingle* server = NULL) {

			this->_client = client;
			this->_server = server;
			this->_classes = classes;
			this->_probs = new float[this->_classes];
		}

		void Main(void) {
			unsigned int k = 1;
			float kd = 1;
			init_probs(this->_probs, this->_classes);
			
			while(HbThread::IsRunning()) {
				update_softprobs(this->_probs, this->_classes, k);
				create_message(this->_message, this->_probs, this->_classes);
				
				for(unsigned int i = 0; i < this->_classes; i++)
					if(this->_probs[i] >= 1.00f) {
						init_probs(this->_probs, this->_classes);
						 srand(time(NULL));
						 kd = (float)(rand())/((float)RAND_MAX);
						 kd > 0.50f ? k = 1 : k = 0;
					}

				if(this->_client != NULL)
					this->_client->Send(this->_message);
				if(this->_server != NULL)
					this->_server->Send(this->_message);
				
				HbTime::Sleep(1000.00f/16);
			}
		}

	private:
		HbNetClient* _client;
		HbNetServerSingle* _server;
		unsigned int _classes;
		float* _probs;
		CcByte _message[MESSAGE_SIZE];
};

void usage(void) {
	printf("Usage: lcosimclassifier --host [IP] --port [PORT] --crisp [CLASSES]\n");
	printf("       lcosimclassifier --host [IP] --port [PORT] --soft [CLASSES]\n");
	printf("       lcosimclassifier --host [IP] --port [PORT] --auto [CLASSES]\n\n");
	printf("Where: --host           IP address of the neurofeedback\n");
	printf("       --port           port\n");
	printf("       --soft           number of classes to simulate\n");
	printf("       --crisp          \n");
	printf("       --auto           \n");

	exit(0);
}

int main(int argc, char * const argv[]) {
	(void)signal(SIGINT, cleantty);

	bool servermode;
	string binHost;
	string binPort;
	string binClasses;
	string binProbs;
	string binThreshold;
	
	switch(argc) {
		case 5:
			servermode = true;
			binHost = "0.0.0.0";
			binPort = argv[2];
			binProbs = argv[3];
			binClasses = argv[4];
			break;
		case 7:
			servermode = false;
			binHost = argv[2];
			binPort = argv[4];
			binProbs = argv[5];
			binClasses = argv[6];
			break;
		default:
			usage();
	}

	bool sendauto = (binProbs.compare("--auto") == 0 ? true : false);
	bool sendhard = (binProbs.compare("--crisp") == 0 ? true : false);
	int classes = atoi(binClasses.c_str());
	
	printf("[lcosimclassifier] Starting experiment:\n");
	printf("            Host:       %s\n", binHost.c_str());
	printf("            Port:       %s\n", binPort.c_str());
	printf("            Classes:    %s\n", binClasses.c_str());
	
	HbNetServerSingle* server = NULL;
	HbNetClient* client = NULL;
	HandlerNet* handler= new HandlerNet();

	if(servermode) {
		/* Setup HbNetServer */
		server = new HbNetServerSingle();
		CB_HbNetObject(server->i_OnRecv, 		 handler, HandleRecv);
		CB_HbNetObject(server->i_OnAccept,	 handler, HandleAccept);
		CB_HbNetObject(server->i_OnDrop,		 handler, HandleDrop);
		if(server->Bind(atoi(binPort.c_str())))
			printf("[lcosimclassifier] Socket bound\n");
		server->Listen();
		server->Start();
	} else {
		/* Setup HbNetClient */
		client = new HbNetClient();
		CB_HbNetObject(client->i_OnRecv, 		 handler, HandleRecv);
		CB_HbNetObject(client->i_OnDisconnect, handler, HandleDisconnect);
		if(client->Connect(binHost, atoi(binPort.c_str())))
			printf("[lcosimclassifier] Connection to FeedBack estabilished\n");
		client->Start();
	}
	
	printf("[lcosimclassifier] Starting main loop. Use 'q' for quitting.\n");
	if(system("stty raw")) {}
	
	int k = 0;
	char input = 0;
	CcByte message[MESSAGE_SIZE];
	float* probs = new float[classes];
	init_probs(probs, classes);

	bool isrunning = true, 
		 zeroed = true;

	if(sendauto == false) {
		while(isrunning) {
			printf("\r[lcosimclassifier] Send crispy probabilities: ");

			if(sendauto == false) {
				input = getchar(); 
				switch(input) {
					case 'q':
						printf("\n\r[lcosimclassifier] Going down...\n\r");
						isrunning = false;
						break;
					case 'z':
						init_probs(probs, classes);
						zeroed = true;
						break;
					default:
						k = atoi(&input) - 1;
						zeroed = false;
				}

				if(k >= 0 && k <= classes - 1)
					if(sendhard && !zeroed)
						update_hardprobs(probs, classes, k);
					else
						update_softprobs(probs, classes, k);
				else
					continue;

				if(create_message(message, probs, classes))
					if(servermode) 
						server->Send(message);
					else
						client->Send(message);
				else
					printf("\a");
				fflush(stdout);

				if(servermode) {
					if(server->IsRunning() == false)
						break;
				} else {
					if(client->IsRunning() == false)
						break;
				}
			}
		}
	} else {
		SluttyThread slutty(classes, client, server);
		slutty.Start();

		while(isrunning) {
			printf("\r[lcosimclassifier] Send randomized probabilities: ");
			input = getchar();
			switch(input) {
				case 'q':
					printf("\n\r[lcosimclassifier] Going down...\n\r");
					isrunning = false;
					break;
				case 'z':
					init_probs(probs, classes);
					zeroed = true;
					break;
				default:
					k = atoi(&input) - 1;
					zeroed = false;
			}
		}
		slutty.Stop();
		slutty.Join();
	}

	if(system("stty cooked")) {}
	if(servermode) {
		server->Stop();
		server->Join();
		delete server;
	} else {
		client->Disconnect();
		client->Join();
		delete client;
	}
	return 0;
}
