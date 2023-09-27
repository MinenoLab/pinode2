#include "upload_module.h"
#include <regex>
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#define MAX_BUFFER_SIZE 1024

UploadModule::UploadModule(void)
{

}


UploadModule::~UploadModule(void)
{

}

time_t convert_to_unix(string time_str) {
    struct tm tm = {};
    if (strptime(time_str.c_str(), "%Y%m%d-%H%M", &tm) == nullptr) {
        return -1;
    }
    tm.tm_isdst = -1; // サマータイムを自動調整
    tm.tm_gmtoff = 9 * 3600; // 日本時間 (UTC+9)
    time_t timestamp = mktime(&tm);
    if (timestamp == -1) {
        return -1;
    }
    return timestamp;
}

//正常に実行できたらtrue
bool check_exist(std::string cli){
    // influxDB2にデータをアップロードするコマンドを作成
    char command[MAX_BUFFER_SIZE];
    snprintf(command, MAX_BUFFER_SIZE, "%s",cli.c_str());

    // コマンドを実行し、標準出力をパイプとして取得する
    FILE* pipe = popen(command, "r");
    /*if (!pipe) {
        std::cout << "Failed to open pipe." << std::endl;
        return false;
    }*/

    // 外部コマンドの出力を1行ずつ読み込む
    char buffer[MAX_BUFFER_SIZE];
    std::string output = "";
    while (fgets(buffer, MAX_BUFFER_SIZE, pipe) != NULL) {
        output += buffer;
    }

    std::cout << output << std::endl;

    // 外部コマンドの出力にエラーメッセージが含まれるかどうかを判定する
    if (!output.empty()) {
        std::cout << "Error: Failed to upload data to influxDB2." << std::endl;
        fclose(pipe);
        return false;
    }

    // コマンドの戻り値を取得する
    int result = pclose(pipe);
    if (result == -1) {
        std::cout << "Error: Failed to get command return value." << std::endl;
        return false;
    }

    // コマンドの戻り値が0であれば成功と判定する
    if (WIFEXITED(result) && WEXITSTATUS(result) == 0) {
        std::cout << "Upload data to influxDB2 succeeded." << std::endl;
        return true;
    }
    return false;
}

void UploadModule::uploadpsql(unsigned int devid,string filepath,FileInfo info)
{
	ostringstream sqlss;
	ostringstream cmdss;
	time_t t = time(nullptr);
	const tm* lt = localtime(&t);

    std::string measurement;
    std::regex re("^.*\\.csv$");
    std::regex re1(".*image1/.*");
    std::regex re2(".*image2/.*");
	std::regex re3(".*image3/.*");
    std::regex re4(".*image4/.*");
    std::regex pattern("/(\\d+)/");
    std::smatch match;
    std::regex_search(filepath, match, pattern);
    measurement = match[1].str();
    std::string filetime = info.filename.substr(0, 13);
    time_t u_time = convert_to_unix(filetime);
	u_time += 9*3600;
	std::string timestamp = to_string(u_time);
	char resolved_path[1000];
	char* result = realpath(filepath.c_str(), resolved_path);
	std::string abs_path = std::string(resolved_path);
	if(std::regex_match(info.filename, re)){
//influx write --org MinenoLaboratory --bucket HappyQuality --token EaEdvq6Lf0AudJn1K0uurpVQfeYGlNocKqJie7_eyWcq9dPcbfzaE6NwcDuLufNhdfi92Bg8nnDh9x6n7FcN1Q== --file ../data/01/sensor/20230222-1000.csv --format csv
	cmdss << "influx write --org " << ORGANIZATION << " --bucket " BUCKET  << " --file " << filepath << " --format=csv --token=" << TOKEN << " 2>&1";
	//system(cmdss.str().c_str());
    //remove(filepath.c_str());
    bool result = check_exist(cmdss.str().c_str());
    if(result){
        remove(filepath.c_str());
        printf("remove csv file\n");
    } else {
        printf("dont remove csv file\n");
    }
    
    
	} else {
//influx write --org MinenoLaboratory --bucket HappyQuality --token EaEdvq6Lf0AudJn1K0uurpVQfeYGlNocKqJie7_eyWcq9dPcbfzaE6NwcDuLufNhdfi92Bg8nnDh9x6n7FcN1Q== 'ImageUploadTest3 back_image_path="test"'
        if(std::regex_match(filepath, re1)){
			cmdss << "influx write --org " << ORGANIZATION << " --bucket " << BUCKET << " --token " << TOKEN << " -p s '" << measurement << " image1=\"" << abs_path << "\" " << timestamp << "'";
	    	system(cmdss.str().c_str());
        } else if(std::regex_match(filepath, re2)){
			cmdss << "influx write --org " << ORGANIZATION << " --bucket " << BUCKET << " --token " << TOKEN << " -p s '" << measurement << " image2=\"" << abs_path << "\" " << timestamp << "'";
            system(cmdss.str().c_str());
        } else if(std::regex_match(filepath, re3)){
            cmdss << "influx write --org " << ORGANIZATION << " --bucket " << BUCKET << " --token " << TOKEN << " -p s '" << measurement << " image3=\"" << abs_path << "\" " << timestamp << "'";
            system(cmdss.str().c_str());
        } else if(std::regex_match(filepath, re4)){
            cmdss << "influx write --org " << ORGANIZATION << " --bucket " << BUCKET << " --token " << TOKEN << " -p s '" << measurement << " image4=\"" << abs_path << "\" " << timestamp << "'";
            system(cmdss.str().c_str());
        }
	}
}
