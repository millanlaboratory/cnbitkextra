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

#include "fesbytestream.h"
#include "fesdecode.h"

int fl_decode_status(fl_reply* reply, fl_tbyte* locked, fl_tlarge* angle, 
		fl_tbyte* active) {
	
	if(reply->cid != FESREQUEST_STATUS)
		return 1;

	unsigned int temp;
	char cache[32];
	
	memset(cache, 0, 32);
	cache[0] = '0';
	cache[1] = 'x';
	strncpy(cache + 2, (const char*)reply->bytefield + 0, 2);
	sscanf(cache, "%x", &temp);
	*locked = temp;

	memset(cache, 0, 32);
	cache[0] = '0';
	cache[1] = 'x';
	strncpy(cache + 2, (const char*)reply->bytefield + 2, 4);
	sscanf(cache, "%x", angle);

	memset(cache, 0, 32);
	cache[0] = '0';
	cache[1] = 'x';
	strncpy(cache + 2, (const char*)reply->bytefield + 6, 2);
	sscanf(cache, "%x", &temp);
	*active = temp;
	return 0;
}

int fl_decode(fl_reply* reply) {
	char cache[FESREPLY_BSIZE], trash[FESREPLY_BSIZE];
	reply->sof = 0;
	reply->dl = 0;
	reply->cid = 0;
	reply->cs = 0;
	reply->data = 0;

	int result = sscanf((const char*)reply->buffer, 
			"%[^'0']%x:dl:%x:cid:%x:data:%x%[^'>']",
			trash,
			(fl_tlarge*)&(reply->sof),
			(fl_tlarge*)&(reply->dl),
			(fl_tlarge*)&(reply->cid),
			(fl_tlarge*)&(reply->data),
			trash);
	if(result != 6)
		return 1;
	
	if(reply->cid > 100)
		return 0;
	
	sscanf((const char*)reply->buffer, 
			"%[^'0']%x:dl:%x:cid:%x:data:%[^':']%[^'>']",
			trash,
			(fl_tlarge*)&(reply->sof),
			(fl_tlarge*)&(reply->dl),
			(fl_tlarge*)&(reply->cid),
			cache,
			trash);
	
	memset(reply->bytefield, 0, FESREPLY_BSIZE);
	strncpy((char*)reply->bytefield, cache + 2, strlen(cache)-2);

	return 0;
}

void fl_inspect(fl_reply* reply) {
	printf("[fl_inspect] Inspecting:\n");
	printf("  SOF:    0x%x\n", reply->sof);
	printf("  DL:     0x%x, %d\n", reply->dl, reply->dl);
	printf("  CID:    0x%x, %d\n", reply->cid, reply->cid);
	printf("  DATA:   0x%x, %d\n", reply->data, reply->data);
	printf("  CS:     0x%x, %d\n", reply->cs, reply->cs);
	printf("  Buffer: %s\n", reply->buffer);
}
