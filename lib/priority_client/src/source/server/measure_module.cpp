#include "measure_module.h"

MeasureModule::MeasureModule(void)
{
	outfile = 0;
	outfile_man = 0;
	man_flag = 0;
}


MeasureModule::~MeasureModule(void)
{
	if(outfile){
		log.close();
	}
}


void MeasureModule::setOutFile(void)
{
	outfile = 1;
	log.open(LOGPATH,ios_base::app);
}

void MeasureModule::setOutFile(string path)
{
	outfile = 1;
	log.open(path.c_str(),ios_base::app);
}

void MeasureModule::setOutFile_man(void)
{
	outfile_man = 1;
	log_man.open(LOGPATH_MAN,ios_base::app);
}

void MeasureModule::setOutFile_man(string path)
{
	outfile_man = 1;
	log_man.open(path.c_str(),ios_base::app);
}

void MeasureModule::measureThroughput(unsigned int devid,int state,FileInfo fiinfo)
{
	if(0 < devid && devid < DEVICEID_MAX ){
		double nowtime = get_sec();
		double ctrl_traffic;
		double time_diff;
		double throughput;
		ostringstream ss;

		switch(state){
			case SEND_ORDER:
				lastsec[devid] = nowtime;
				break;
			case RECV_METADATA:
				//order + metadata
				ctrl_traffic = (3) + (fiinfo.filename.size() + 17);

				time_diff = nowtime - lastsec[devid];
				//cout << "time_diff = " << time_diff << endl;
				throughput = ctrl_traffic * 8 / time_diff;

				cout << "====================throughput file=====================" << endl;
				ss << std::fixed << std::setprecision(3) << devid << "," << fiinfo.id << "," << lastsec[devid] << "," << nowtime << "," 
					 << std::setprecision(0) << ctrl_traffic << "," << fiinfo.filesize << "," << std::setprecision(2) << throughput << endl;
				cout << ss.str();
				cout << "====================throughput file=====================" << endl;
				if(outfile){
					lock_guard<std::mutex> lock(log_mutex_);
					log << ss.str();
					log.flush();
				}
				break;
			case FILE_RECV_START:
				//order + metadata + ACK + content
				//ctrl_traffic = (3) + (fiinfo.filename.size() + 17) + (1) + (3);
				ctrl_traffic = (3) + (fiinfo.filename.size() + 17);

				time_diff = nowtime - lastsec[devid];
				//cout << "time_diff = " << time_diff << endl;
				throughput = (ctrl_traffic + fiinfo.filesize) * 8 / time_diff;

				cout << "====================throughput file=====================" << endl;
				ss << std::fixed << std::setprecision(3) << devid << "," << fiinfo.id << "," << lastsec[devid] << "," << nowtime << "," 
					 << std::setprecision(0) << ctrl_traffic << "," << fiinfo.filesize << "," << std::setprecision(2) << throughput << endl;
				cout << ss.str();
				cout << "====================throughput file=====================" << endl;
				if(outfile){
					lock_guard<std::mutex> lock(log_mutex_);
					log << ss.str();
					log.flush();
				}
				break;
			case FILE_RECV_CONTINUE:
				//order + metadata + ACK + content
				//ctrl_traffic = (3) + (fiinfo.filename.size() + 17) + (fiinfo.filesize/1480 + 1) + ((fiinfo.filesize/1480 + 1)*3);
				ctrl_traffic = (3) + (fiinfo.filename.size() + 17);
				time_diff = nowtime - lastsec[devid];

				throughput = (ctrl_traffic + fiinfo.filesize) * 8 / time_diff;

				cout << "====================throughput file=====================" << endl;
				ss << std::fixed << std::setprecision(3) << devid << "," << fiinfo.id << "," << lastsec[devid] << "," << nowtime << "," 
					 << std::setprecision(0) << ctrl_traffic << "," << fiinfo.filesize << "," << std::setprecision(2) << throughput << endl;
				cout << ss.str();
				cout << "====================throughput file=====================" << endl;
				if(outfile){
					lock_guard<std::mutex> lock(log_mutex_);
					log << ss.str();
					log.flush();
				}
				break;
			default:
				break;
		}
	}
}

void MeasureModule::measureStart(unsigned int devid){
	lastsec_man[devid] = get_sec();
	man_flag = 1;
}

void MeasureModule::measureEnd(unsigned int devid,double datasize){
	if(man_flag){
		double nowtime = get_sec();
		double time_diff = nowtime - lastsec_man[devid];

		double throughput = datasize * 8 / time_diff;
		ostringstream ss;
		cout << "====================throughput file=====================" << endl;
		ss << std::fixed << std::setprecision(3) << devid << "," << lastsec_man[devid] << "," << nowtime << "," 
			 << std::setprecision(0) << datasize << "," << std::setprecision(2) << throughput << endl;
		cout << ss.str();
		cout << "====================throughput file=====================" << endl;
		if(outfile){
			lock_guard<std::mutex> lock(log_man_mutex_);
			log << ss.str();
			log.flush();
		}
	}
}

double MeasureModule::get_sec(void)
{
  timespec ts;
  clock_gettime(CLOCK_REALTIME_COARSE, &ts);
  return ts.tv_sec + ts.tv_nsec * 1e-9;
}
