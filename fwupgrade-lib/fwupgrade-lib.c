/*--------------------------------------------------------------------
  * If not stated otherwise in this file or this component's Licenses.txt file the
* following copyright and licenses apply:
*
* Copyright 2020 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.


  During firmware upgrade, new image is downloaded to tmp from server
  The downloaded image is passed as argument to mfrWriteImage function
  mfrWriteImage() creates a new thread to handle image update 
----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "string.h"
#include <mfrApi.h>
typedef struct fw_Params {
    const char *name;
    const char *path;
    mfrImageType_t type;
    mfrUpgradeStatusNotify_t notify;
} fw_Params_t;
int main(void)
{
    // nothing to do
}

/********************************************************************
   Functiona Name: fwupgradeThread
   Description   : thread handled for image upgrade
   Input:    fw_Params_t
   Output:   None
   Returns:  Void

*********************************************************************/

void* fwupgradeThread(void* fw_Params)
{
    printf("fwupgradeThread handler\n");
    fw_Params_t *stParameters = (fw_Params_t *)(fw_Params);
    // filling up firmware parameters
    const char* name = stParameters->name;
    const char *path = stParameters->path;
    mfrImageType_t type = stParameters->type;
    mfrUpgradeStatusNotify_t notify = stParameters->notify;
    mfrUpgradeStatus_t status;
    
    //If path or name is NULL return error.
    if(!name || !path )
    {
       printf("name or path is not valid %s:%d \n", __FUNCTION__, __LINE__);
       status.error = mfrERR_INVALID_PARAM;
       status.progress = mfrUPGRADE_PROGRESS_ABORTED;
       free((fw_Params_t* )fw_Params);
       return NULL;
    }

    // filling up callback return values
    status.progress = mfrUPGRADE_PROGRESS_NOT_STARTED;
    status.error = mfrERR_NONE;
    status.percentage = 0;
    notify.cb(status, notify.cbData);
    
    int nSlash_space = 0;
    int retVal = 0;
    if (path[strlen(path) - 1] != '/')
    {
	    nSlash_space = 1;
    }
   
    const char* script = "sh /lib/rdk/write_kernel_rootfs.sh "; 
    //Form command for shell script, additional bytes for '\' if needed and '\0'
    char *cmd = (char*)malloc((strlen(script) + strlen(path) + strlen(name) + nSlash_space + 1) * sizeof(char));
    if (cmd)
    {
            
	    status.progress = mfrUPGRADE_PROGRESS_STARTED;
	    status.percentage = 10;
	    notify.cb(status, notify.cbData);
	    strcpy(cmd, script);
	    strcat(cmd, path);
	    if (nSlash_space)
	    {
		    strcat(cmd, "/");
	    }
	    strcat(cmd, name);
	    status.percentage = 50;
	    notify.cb(status, notify.cbData);
            printf("calling write script\n");
	    retVal = system(cmd);
	    if (0 == retVal)
	    {
		    status.progress = mfrUPGRADE_PROGRESS_COMPLETED;
		    status.error = mfrERR_NONE;
		    status.percentage = 100;
	    }
	    else
	    {
            printf("calling write script failed\n");
		    status.progress = mfrUPGRADE_PROGRESS_ABORTED;
		    status.error = mfrERR_DECRYPTION_FAILED;
		    status.percentage = 0;
	    }
	    free(cmd);
    }
    else
    {
        printf("Failed memory allocation %s:%d \n", __FUNCTION__, __LINE__);
	    status.error = mfrERR_MALLOC_FAILED;
	    status.progress = mfrUPGRADE_PROGRESS_ABORTED;
    }

    notify.cb(status, notify.cbData);
    free((fw_Params_t* )fw_Params);
    return NULL;
}

/**************************************************************************
   Functiona Name: mfrWriteImage 
   Description   : mfrWriteImage handles flashing new image to box
   Input:    fw_Params_t
   Output:   mfrUpgradeStatusNotify_t
   Returns:  mfrERR_NONE 

***************************************************************************/

mfrError_t mfrWriteImage(const char *name, const char *path, mfrImageType_t type, mfrUpgradeStatusNotify_t notify)
{
   
    if(!name || !path)
    {
       printf("name or path is NULL \n");
       return mfrERR_INVALID_PARAM;
    } 
    const char* script ="sh /lib/rdk/memory_partition.sh";
   
    int retVal = system(script);
    pthread_t tid;
    fw_Params_t *stParameters = (fw_Params_t* )malloc(sizeof(fw_Params_t));
    if (!stParameters)
    {
        return mfrERR_MALLOC_FAILED;
    }
     
    stParameters->name = name;
    stParameters->path = path;
    stParameters->type = type;
    stParameters->notify = notify;
            printf("created thread for handling\n");
    pthread_create(&tid, NULL, fwupgradeThread, stParameters);
    pthread_detach(tid);
    return mfrERR_NONE;
}
mfrError_t mfrFWUpgradeInit(void)
{
    return mfrERR_NONE;
}
mfrError_t mfrFWUpgradeTerm(void)
{
    return mfrERR_NONE;
}
