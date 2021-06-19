#include <iostream>
#include "Fdfs_api.h"

int main(int argc,char *argv[])
{
    Fdfs_api api;
    api.fdfs_upload_file(argv[1]);
    cout<<api.getFile_id()<<endl;
    api.fdfs_download_file(api.getFile_id());
 api.fdfs_delete_file(api.getFile_id());
    
    return 0;
}

