#pragma once
#include <iostream>
#include "fdfs_client.h"
#include "logger.h"
// 日志
#include <log4cplus/log4cplus.h>

using namespace std;
using namespace log4cplus;

const string m_conf_file = "/etc/fdfs/client.conf";

class Fdfs_api
{
public:
    Fdfs_api();
    ~Fdfs_api();
    int fdfs_upload_file(const char* local_file);
    int fdfs_download_file(string file_id);
    int fdfs_delete_file(string file_id);
    string getFile_id();
private:
    void initLogFile();
private:
    Logger logger;
    string m_file_id;
	ConnectionInfo *pTrackerServer;
    char* conf_filename = NULL;
};

