#ifndef CP_UTILITIES_H
#define CP_UTILITIES_H

#include <string>
#include <sstream>


bool cp_expression_retrieve(const char* filename, long int* timestamp, char* expression) {
	
	std::string line;
	std::ifstream ifs;
	ifs.open(filename);
	if(ifs) {
		while (ifs >> std::ws && std::getline(ifs, line));
		std::sscanf(line.c_str(), "[%ld] %s", timestamp, expression);
		ifs.close();
	} else {
		return false;
	}
	
	return true;
}

bool cp_expression_extract(const char* expression, const char* key, std::string& value) {
	bool retcode = false;
	char* pch;
	char iexpr[100];
	strncpy(iexpr, expression, sizeof(iexpr));
	pch = strtok(iexpr, "|");
	while(pch != NULL) {
		if(strcmp(key, pch) == 0) {
			pch = strtok(NULL, "|");
			value.assign(pch);
			retcode = true;
			break;
		} else {
			pch = strtok(NULL, "|");
		}
	}
	return retcode;
}

template<class T>
bool cp_parameter_update(std::string ptype, std::string ptask, 
			       std::string pname,  T* pvalue) {
	
	char date[80];
	time_t t;
     	struct tm* now;

	char expression[100];
	long int ts;
	unsigned int id;
	std::string filename, svalue;

	t = time(0);
	now = localtime(&t);
	strftime(date,80,"%Y%m%d",now);

	bool retcode = false;

	T currentvalue = *pvalue;
	filename = CcCore::GetDirectoryTmp() + date + ".dynamic_parameters.txt";

	if(cp_expression_retrieve(filename.c_str(), &ts, expression) == true) {
		if(cp_expression_extract(expression, pname.c_str(), svalue) == true &&
		   cp_expression_extract(expression, ptask.c_str(), ptask) == true &&
		   cp_expression_extract(expression, ptype.c_str(), ptype) == true) {
				
			std::istringstream stream(svalue);
			stream >> *pvalue;
		
			if(*pvalue != currentvalue) {
				retcode = true;
			
			}
		}
	}

	return retcode;
}


#endif
