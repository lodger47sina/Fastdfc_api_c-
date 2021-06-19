#include "Fdfs_api.h"
#include <string.h>
#include <cstdio>


Fdfs_api::Fdfs_api()
{
    conf_filename = const_cast<char*>(m_conf_file.c_str());
    int result;
    if ((result=fdfs_client_init(conf_filename)) != 0)
    {
        LOG4CPLUS_ERROR(logger,"Fdfs_api()");
    }
    pTrackerServer = tracker_get_connection();
    if (pTrackerServer == NULL)
    {
        fdfs_client_destroy();
        LOG4CPLUS_ERROR(logger,"Fdfs_api()");
    }
}
Fdfs_api::~Fdfs_api()
{
    tracker_disconnect_server_ex(pTrackerServer, true);
    fdfs_client_destroy();
}
int Fdfs_api::fdfs_upload_file(const char* local_file)
{
    char *local_filename = const_cast<char*>(local_file);
    char group_name[FDFS_GROUP_NAME_MAX_LEN + 1];
    int result;
    int store_path_index;
    ConnectionInfo storageServer;
    char file_id[128];

    *group_name = '\0';
    if ((result=tracker_query_storage_store(pTrackerServer, \
                                            &storageServer, group_name, &store_path_index)) != 0)
    {
        fdfs_client_destroy();
        LOG4CPLUS_ERROR(logger,"tracker_query_storage_store()");
        return result;
    }

    result = storage_upload_by_filename1(pTrackerServer, \
                                         &storageServer, store_path_index, \
                                         local_filename, NULL, \
                                         NULL, 0, group_name, file_id);
    if (result == 0)
    {
        m_file_id = string(file_id);
    }
    else
    {
        LOG4CPLUS_ERROR(logger,"storage_upload_by_filename1()");
    }

    return result;
}

string Fdfs_api::getFile_id()
{
    return m_file_id;
}

int Fdfs_api::fdfs_download_file(string local_file)
{
    char *local_filename;
    int result;
    char file_id[128];
    int64_t file_size;
    int64_t file_offset;
    int64_t download_bytes;

    snprintf(file_id, sizeof(file_id), "%s", const_cast<char*>(local_file.data()));
    file_offset = 0;
    download_bytes = 0;
    local_filename = strrchr(file_id, '/');
    if (local_filename != NULL)
    {
        local_filename++;  //skip /
    }
    else
    {
        local_filename = file_id;
    }


    result = storage_do_download_file1_ex(pTrackerServer, \
                                          NULL, FDFS_DOWNLOAD_TO_FILE, file_id, \
                                          file_offset, download_bytes, \
                                          &local_filename, NULL, &file_size);
    if (result != 0)
    {
        LOG4CPLUS_ERROR(logger,"storage_do_download_file1_ex()");
    }
    return 0;
}

int Fdfs_api::fdfs_delete_file(string local_file)
{
    int result;
    char file_id[128];

    snprintf(file_id, sizeof(file_id), "%s", const_cast<char*>(local_file.c_str()));
    if ((result=storage_delete_file1(pTrackerServer, NULL, file_id)) != 0)
    {
        LOG4CPLUS_ERROR(logger,"storage_delete_file1()");
    }
    return result;
}

void Fdfs_api::initLogFile()
{
    Initializer Initializer;

    SharedAppenderPtr fileAppender(new FileAppender(LOG4CPLUS_TEXT("log.txt"),std::ios_base::app));

    fileAppender->setName("file");
    string pattern = "%d{%m/%d/%y %H:%M:%S} - %m [%1]%n";

    fileAppender->setLayout(unique_ptr<Layout>(new PatternLayout(pattern)));
    logger = Logger::getInstance("test");
    logger.setLogLevel(INFO_LOG_LEVEL);

    logger.addAppender(fileAppender);
}

