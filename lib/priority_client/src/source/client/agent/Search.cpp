#include "Search.h"

Search::Search(void)
{
	pripacket->initMETAdata(&ret_fileinfo);
	for(int i=0;i<DATAID_MAX;i++){
		filepath[i] = FILEINFO_INITSTRING;
	}
}

Search::~Search(void)
{
	delete pripacket;
}


string Search::path2file(const string &path) {
	string ret = path.substr(std::max<signed>(path.find_last_of('/'), path.find_last_of('\\')) + 1);
	return ret;
}

bool Search::isExist(unsigned int id ,string path,int mode){
	switch(mode){
    	case NEWEST:
    		return isExistNewest(id,path);
    	case OLDEST:
    		return isExistOldest(id,path);
    	case SMALLEST:
    		return isExistSmallest(id,path);
    }
}

bool Search::isExistNewest(unsigned int id ,string path){
	if(id <= 0 || DATAID_MAX < id){
		return 0;
	}

	FILE *fp;
	char buf[255];
	bool check;
	//file_path update
	string cmd="sh ./script/serch_newestfilepath_noempty.sh 2>/dev/null ";
	cmd += path;
	if ( (fp=popen(cmd.c_str(),"r")) == NULL) {
		filepath[id]=FILEINFO_INITSTRING;
		return 0;
	}
	memset(buf,0,sizeof(buf));
	check = 0;
	while(fgets(buf, 255, fp) != NULL) {
		check = 1;
		filepath[id] = buf;
		if(filepath[id].size() <= 1){
			filepath[id]=FILEINFO_INITSTRING;
			return 0;
		}
		filepath[id].erase(filepath[id].size()-1);
	}
	(void) pclose(fp);
	if(!check){
		filepath[id]=FILEINFO_INITSTRING;
		return 0;
	}

	return 1;
}

bool Search::isExistOldest(unsigned int id ,string path){
	if(id <= 0 || DATAID_MAX < id){
		return 0;
	}

	FILE *fp;
	char buf[255];
	bool check;
	//file_path update
	string cmd="sh ./script/serch_oldestfilepath_noempty.sh 2>/dev/null ";
	cmd += path;
	if ( (fp=popen(cmd.c_str(),"r")) == NULL) {
		filepath[id]=FILEINFO_INITSTRING;
		return 0;
	}
	memset(buf,0,sizeof(buf));
	check = 0;
	while(fgets(buf, 255, fp) != NULL) {
		check = 1;
		filepath[id] = buf;
		if(filepath[id].size() <= 1){
			filepath[id]=FILEINFO_INITSTRING;
			return 0;
		}
		filepath[id].erase(filepath[id].size()-1);
	}
	(void) pclose(fp);
	if(!check){
		filepath[id]=FILEINFO_INITSTRING;
		return 0;
	}

	return 1;
}

bool Search::isExistSmallest(unsigned int id ,string path){
	if(id <= 0 || DATAID_MAX < id){
		return 0;
	}

	FILE *fp;
	char buf[255];
	bool check;
	//file_path update
	string cmd="sh ./script/serch_smallestfilepath_noempty.sh 2>/dev/null ";
	cmd += path;
	if ( (fp=popen(cmd.c_str(),"r")) == NULL) {
		filepath[id]=FILEINFO_INITSTRING;
		return 0;
	}
	memset(buf,0,sizeof(buf));
	check = 0;
	while(fgets(buf, 255, fp) != NULL) {
		check = 1;
		filepath[id] = buf;
		if(filepath[id].size() <= 1){
			filepath[id]=FILEINFO_INITSTRING;
			return 0;
		}
		filepath[id].erase(filepath[id].size()-1);
	}
	(void) pclose(fp);
	if(!check){
		filepath[id]=FILEINFO_INITSTRING;
		return 0;
	}

	return 1;
}

FileInfo Search::getMETAdata(unsigned int id,string * path){
	pripacket->initMETAdata(&ret_fileinfo);

	*path = filepath[id];

	FILE *fp;
	char buf[255];
	bool check;

	//access_time update
	string cmd = "stat -c %X ";
	cmd += filepath[id];
	cmd += " 2>/dev/null"; 
	if ( (fp=popen(cmd.c_str(),"r")) ==NULL) {
		return ret_fileinfo;
	}
	memset(buf,0,sizeof(buf));
	check = 0;
	while(fgets(buf, 255, fp) != NULL) {
		check = 1;
		ret_fileinfo.timestamp = buf;
		if(ret_fileinfo.timestamp.size() <= 1){
			return ret_fileinfo;
		}
		ret_fileinfo.timestamp.erase(ret_fileinfo.timestamp.size()-1);
	}
	(void) pclose(fp);
	if(!check){
		return ret_fileinfo;
	}

	//filesize update
	unsigned int ret;
	string tmp;
	cmd="ls -l ";
	cmd += filepath[id];
	cmd += " 2>/dev/null | awk '{print $5}'"; 
	if ( (fp=popen(cmd.c_str(),"r")) ==NULL) {
		return ret_fileinfo;
	}
	memset(buf,0,sizeof(buf));
	check = 0;
	while(fgets(buf, 255, fp) != NULL) {
		check = 1;
		tmp = buf;
		if(tmp.size() != 0){
			ret = atoi(buf);
		}else{
			pripacket->initMETAdata(&ret_fileinfo);
			return ret_fileinfo;
		}
	}
	(void) pclose(fp);
	if(!check){
		pripacket->initMETAdata(&ret_fileinfo);
		return ret_fileinfo;
	}
	ret_fileinfo.filesize=ret;

	ret_fileinfo.id = id;
	ret_fileinfo.filename = path2file(filepath[id]);

	return ret_fileinfo;
}

unsigned int Search::getTimestamp(unsigned int id){
	string timestamp;

	FILE *fp;
	char buf[255];
	bool check;

	//access_time update
	string cmd = "stat -c %X ";
	cmd += filepath[id];
	cmd += " 2>/dev/null"; 
	if ( (fp=popen(cmd.c_str(),"r")) ==NULL) {
		return 0;
	}
	memset(buf,0,sizeof(buf));
	check = 0;
	while(fgets(buf, 255, fp) != NULL) {
		check = 1;
		timestamp = buf;
		if(timestamp.size() <= 1){
			return 0;
		}
		timestamp.erase(timestamp.size()-1);
	}
	(void) pclose(fp);
	if(!check){
		return 0;
	}

	return (unsigned int)strtoul(timestamp.c_str(),NULL,10);
}

unsigned int Search::getSize(unsigned int id){
	FILE *fp;
	char buf[255];
	bool check;

	//filesize update
	unsigned int ret;
	string tmp;
	string cmd="ls -l ";
	cmd += filepath[id];
	cmd += " 2>/dev/null | awk '{print $5}'"; 
	if ( (fp=popen(cmd.c_str(),"r")) ==NULL) {
		return 0;
	}
	memset(buf,0,sizeof(buf));
	check = 0;
	while(fgets(buf, 255, fp) != NULL) {
		check = 1;
		tmp = buf;
		if(tmp.size() != 0){
			ret = atoi(buf);
		}else{
			return 0;
		}
	}
	(void) pclose(fp);
	if(!check){
		return 0;
	}
	//filesize=ret;

	return ret;
}
