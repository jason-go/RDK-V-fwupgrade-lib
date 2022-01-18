/**--------------------------------------------------------------------
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
*/

/**
 * @file   mfrApi.h
 * @brief  MFRLibs API interface.
 *
 * This API defines the HAL for Manufacturer Libs interface.
 *
 * @par Document
 * Document reference.
 *
 * @par Open Issues (in no particular order)
 * -# None
 *
 * @par Assumptions
 * -# None
 *
 * @par Abbreviations
 * - BE:      Big-Endian.
 * - cb:      Callback function (suffix).
 * - DS:      Device Settings.
 * - FPD:     Front-Panel Display.
 * - HAL:     Hardware Abstraction Layer.
 * - LE:      Little-Endian.
 * - LS:      Least Significant.
 * - MBZ:     Must be zero.
 * - MS:      Most Significant.
 * - RDK:     Reference Design Kit.
 * - _t:      Type (suffix).
 *
 * @par Implementation Notes
 * -# None
 *
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @defgroup MFRLIBS MFRLibs
 * The Manufacturer Libraries (MFRLibs) is an abstraction layer used by various system components to gain access to:
 *  - hardware capabilities resident on the STB (aka. serialization data),
 *    including manufacturer, model name, product class, hardware/software versions,
 *  - secure NVRAM, including DTCP certificates,
 *  - non-secure NVRAM, including boot image info,
 *  - flash memory for STB image upgrade.
 * Below are currently used APIs for the RDK enabled devices that need to be normalized into a set of
 * operator specific MFR APIs specification. These APIs should include any HALs as well as other dependent components.
 * - Initializes the manufacturer specific modules like, drivers, NVRAM and other devices
 * - Gets serialized read-only data from the device & it is upto the application to de-serialize and understand the data returned.
 * - Reads data from the secure NVRAM partition.
 * - Writes data to the secure NVRAM partition.
 * - Reads data from the un-secure NVRAM partition.
 * - Writes data to the un-secure NVRAM partition.
 * - Writes firmware image to the flash memory.
 * - API to take care of validating headers, manufacturer, model number and perform a signature
 * check while writing firmware image into the flash memory.
 * - Gets current firmware upgrade status.
 *
 * @defgroup MFRLIBS_HAL_TYPES MFRLibs HAL Data types
 * @ingroup MFRLIBS
 *
 * @defgroup MFRLIBS_HAL_API MFRLibs HAL API
 * @ingroup MFRLIBS
 * @{
 */

/**
 * @brief MFRLibs return codes.
 *
 * This enumeration defines types of errors that can be returned from the MFRLibs sub-system.
 */
typedef enum _mfrError_t {
    mfrERR_NONE = 0,                    /**< Operation successful.      */
    mfrERR_GENERAL = 0x1000,            /**< General unspecified error. */
    mfrERR_INVALID_PARAM,               /**< Invalid input parameters.  */
    mfrERR_INVALID_STATE,               /**< Invalid state.             */
    mfrERR_OPERATION_NOT_SUPPORTED,     /**< Operation not supported.   */
    mfrERR_MALLOC_FAILED,               /**< Memory allocation failed.  */
    mfrERR_NOT_INITIALIZED,             /**< MFRLibs not initialized.   */
    mfrERR_ENCRYPTION_FAILED,           /**< Encryption failed.         */
    mfrERR_DECRYPTION_FAILED,           /**< Decryption failed.         */
    mfrERR_UNKNOWN,                     /**< Unknown error.             */
} mfrError_t;

/**
 * @brief Serialized data types.
 *
 * This enumeration defines types of read-only data that can be read from the hardware.
 */
typedef enum _mfrSerializedType_t {
    mfrSERIALIZED_TYPE_MANUFACTURER = 0,                /**< Manufacturer name.             */
    mfrSERIALIZED_TYPE_MANUFACTUREROUI,                 /**< Manufacturer OUI.              */
    mfrSERIALIZED_TYPE_MODELNAME,                       /**< CPE model name.                */
    mfrSERIALIZED_TYPE_DESCRIPTION,                     /**< CPE description.               */
    mfrSERIALIZED_TYPE_PRODUCTCLASS,                    /**< Product class.                 */
    mfrSERIALIZED_TYPE_SERIALNUMBER,                    /**< CPE serial number.             */
    mfrSERIALIZED_TYPE_HARDWAREVERSION,                 /**< Hardware version.              */
    mfrSERIALIZED_TYPE_SOFTWAREVERSION,                 /**< Software version.              */
    mfrSERIALIZED_TYPE_PROVISIONINGCODE,                /**< ACS provisioning code.         */
    mfrSERIALIZED_TYPE_FIRSTUSEDATE,                    /**< First use date and time (UTC). */
    mfrSERIALIZED_TYPE_VENDOR_CONFIGFILE_ENTRIES,       /**< Vendor ConfigFile entries.     */
    mfrSERIALIZED_TYPE_VENDOR_LOGFILE_ENTRIES,          /**< Vendor LogFile entries.        */
    mfrSERIALIZED_TYPE_SUPPORTED_DATAMODEL_ENTRIES,     /**< Supported DataModel entries.   */
    mfrSERIALIZED_TYPE_PROCESSOR_ENTRIES,               /**< Processor entries.             */
    mfrSERIALIZED_TYPE_DEVICEMAC,
    mfrSERIALIZED_TYPE_MOCAMAC,
    mfrSERIALIZED_TYPE_HDMIHDCP,
    mfrSERIALIZED_TYPE_HDMIHDCP_INPUT,
    mfrSERIALIZED_TYPE_HDMIHDCP_2_2,
    mfrSERIALIZED_TYPE_HDMIHDCP_2_2_LEN,
    mfrSERIALIZED_TYPE_PDRIVERSION,
    mfrSERIALIZED_TYPE_CMMAC,
    mfrSERIALIZED_TYPE_ESTBMAC,
    mfrSERIALIZED_TYPE_DRM_WIDEVINE,
    mfrSERIALIZED_TYPE_DRM_PLAYREADY,
    mfrSERIALIZED_TYPE_DRM_NETFLIX,
    mfrSERIALIZED_TYPE_DRM_SSL,
    mfrSERIALIZED_TYPE_SSL_CERT,
    mfrSERIALIZED_TYPE_WIFIMAC,
    mfrSERIALIZED_TYPE_BLUETOOTHMAC,
    mfrSERIALIZED_TYPE_ETHERNETMAC,
    mfrSERIALIZED_TYPE_MAX,
} mfrSerializedType_t;

/**
 * @brief Upgrade image types.
 *
 * This enumeration defines the types of upgrade images.
 */
typedef enum _mfrImageType_t {
    mfrIMAGE_TYPE_CDL,  /**< DOCSIS-based download. */
    mfrIMAGE_TYPE_RCDL, /**< HTTP-based download.   */
} mfrImageType_t;
/**
 * @brief Upgrade status types.
 *
 * This enumeration defines the statuses of an initiated firmware upgrade.
 */
typedef enum _mfrUpgradeProgress_t {
  mfrUPGRADE_PROGRESS_NOT_STARTED = 0,  /**< Upgrade not started.            */
  mfrUPGRADE_PROGRESS_STARTED,          /**< Upgrade in progress.            */
  mfrUPGRADE_PROGRESS_ABORTED,          /**< Upgrade aborted.                */
  mfrUPGRADE_PROGRESS_COMPLETED,        /**< Upgrade successfully completed. */
} mfrUpgradeProgress_t;
/**
 * @brief Serialized data structure.
 *
 * This structure defines the serialized read-only data.
 */
typedef struct _mfrSerializedData_t {
    size_t bufLen;                      /**< Length of the data buffer.                                  */
    char *pBuffer;                      /**< Buffer containing the serialized data.                      */
    void (* freeBuffer) (char *buf);    /**< Function used to free the buffer. If NULL, nothing to free. */
} mfrSerializedData_t;
/**
 * @brief Upgrade status structure.
 *
 * This structure defines the upgrade status information.
 */
typedef struct _mfrUpgradeStatus_t {
  mfrUpgradeProgress_t progress;        /**< Status of progress (e.g. started, aborted).  */
  mfrError_t error;                     /**< The error code to denote the upgrade status. */
  int percentage;                       /**< Percentage of the upgrade progress.          */
} mfrUpgradeStatus_t;

/**
 * @brief Upgrade status notification structure.
 *
 * This structure defines the upgrade notification callback and its frequency of occurence.
 */
typedef struct _mfrUpgradeStatusNotify_t {
    void * cbData;
    void (*cb) (mfrUpgradeStatus_t status, void *cbData); /**< Pointer to the callback function used to notify of the upgrade status. */
    int interval;  /**< Interval in seconds between two successive callbacks.
                    * The value 0 means that the notification is expected only once to report final upgrade result.
                    * @note Manufacturer must at least support notifying the upgrade status at the end. */
} mfrUpgradeStatusNotify_t;

/**
 * @brief Initializes the MFRLibs sub-system.
 *
 * This function initializes the manufacturer specific modules like, drivers, NVRAM and other devices.
 *
 * @param  None.
 *
 * @return Error code.
 * @retval ::mfrERR_NONE Initialization succeeced.
 * @retval ::mfrError_t  Other specific code on error. The initialization has failed and no further calls can work as expected.
 */
mfrError_t mfrInit(void);
/**
 * @brief Terminates the MFRLibs sub-system.
 *
 * This function terminates the manufacturer specific modules like drivers, NVRAM etc.
 * It cancels the threads that are running within. No calls to the MFRLibs can be made after this call.
 *
 * @param  None.
 *
 * @return Error code.
 * @retval ::mfrERR_NONE Termination succeeded.
 * @retval ::mfrError_t Other specific code on error if the MFRLibs have not been properly terminated.
 */
mfrError_t mfrTerm(void);
/**
 * @brief Gets serialized read-only data from the device.
 *
 * This function retrieves serialized read-only data from device. The serialized data is returned as a byte stream.
 * It is up to the application to de-serialize and understande the data returned. The caller must allocate
 * memory for ::mfrSerializedData_t and pass it.
 *
 * @param [in] type  Type of serialized data to read.
 * @param [in] data  Memory location of a buffer to hold the ::mfrSerializedData_t data structure on return.
 *
 * @return Error code.
 * @retval ::mfrERR_NONE The data has been read successfully.
 * @retval ::mfrError_t  Other specific code on error. The reading has failed and values in @a data should not be used.
 *
 * @note The data buffer @a data->pBuffer will be provided by this function. The caller is only responsible
 * of releasing the data buffer if @a data->freeBuffer function is provided. If @a freeBuffer is not NULL then
 * the caller must use this function to free the data buffer once it has finished using it. Otherwise the caller
 * must not try to free the data buffer.
 * Even if the serialized data returned as "string", the buffer is not required to contain the null-terminator.
 *
 */
mfrError_t mfrGetSerializedData( mfrSerializedType_t type,  mfrSerializedData_t *data );
/**
 * @brief Writes firmware image to the flash memory.
 *
 * This function validates and writes a firmware image into the flash memory. The process should follow these major steps:
 *   - Validate headers, manufacturer, model.
 *   - Perform signature check.
 *   - Flash the image.
 *   - Perform CRC of flashed NVRAM.
 *   - Update boot params and switch banks to prepare for a reboot event.
 *
 * State transitions:
 *  - Before the this function is invoked, the upgrade process should be in ::mfrUPGRADE_PROGRESS_NOT_STARTED state.
 *  - After this function returns with success, the upgrade process moves to ::mfrUPGRADE_PROGRESS_STARTED state.
 *  - After this function returns with error, the upgrade process stays in ::mfrUPGRADE_PROGRESS_NOT_STARTED state.
 *      Notify function will not be invoked.
 *  - The notify function is called at regular interval with @a progress set to ::mfrUPGRADE_PROGRESS_STARTED.
 *  - The last invocation of notify function should have either @a progress set to ::mfrUPGRADE_PROGRESS_COMPLETED or
 *    ::mfrUPGRADE_PROGRESS_ABORTED with an appropriate error code set.
 *
 * @param [in] name    The filename of the image file.
 * @param [in] path    The path of the image file in the STB file system.
 * @param [in] type    The type (e.g. format, signature type) of the image.
 *                     This can dictate the handling of the image within the MFR library.
 * @param [in] notify  Memory location of the notification structure holding the callback to provide status
 *                     of the image flashing process.
 *
 * @return Error code.
 * @retval ::mfrERR_NONE The image flashing process has been started successfully.
 * @retval ::mfrError_t  Other specific code on error if the image flashing process has not started.
 *
 * @note All upgrades should be done in the alternate bank. The current bank should not be disturbed at any cost.
 * (I.e. a second upgrade before a reboot will overwrite the non-current bank only.)
 */
mfrError_t mfrWriteImage(const char *name,  const char *path, mfrImageType_t type,  mfrUpgradeStatusNotify_t notify);
/**
* @brief Verify image signature.  The process should follow these major steps:
*    1) Validate headers, manufacturer, model.
*    2) Perform Signature check.
*
* @param [in] name:  the path of the image file in the STB file system.
* @param [in] path:  the directory name containing the image file.
* @return Error Code:
*/
mfrError_t mfrVerifyImage(const char *name, const char *path );
/**
* @brief Unpack image.  The process should follow these major steps:
*    1) Validate headers, manufacturer, model.
*    2) Perform Signature check.
*    3) Unpack the inner data - decrypting if necessary (as indicated by the TLVs)
* Unpacked data is written to file <out_name> in directory <out_path>
*
* @param [in] name:  the path of the image file in the STB file system.
* @param [in] path:  the filename of the image file.
* @param [in] out_name:  the path of the unpacked file in the STB file system.
* @param [in] out_path:  the filename of the image file.
* @return Error Code:  If error code is returned, the image flashing is not initiated..
*/
mfrError_t mfrUnpackImage(const char *name,     const char *path,
                          const char *out_name, const char *out_path);
/**
* @brief Delete the P-DRI image if it is present
*
* @return Error Code:  Return mfrERR_NONE if P-DRI is succesfully deleted or not present, mfrERR_GENERAL if deletion fails
*/
mfrError_t mfrDeletePDRI(void);
/**
* @brief Delete the platform images
*
* @return Error Code:  Return mfrERR_NONE if the images are scrubbed, mfrERR_GENERAL if scrubbing fails
*/
mfrError_t mfrScrubAllBanks(void);
/**
 * @brief Gets current firmware upgrade status.
 *
 * This function returns the current status of the upgrade process. The caller must allocate memory for
 * ::mfrUpgradeProgress_t and pass it.
 *
 * @param [out] pStatus  Memory location of a buffer to hold ::mfrUpgradeProgress_t data structure on return.
 *
 * @return Error code.
 * @retval ::mfrERR_NONE The upgrade status has been retrieved successfully.
 * @retval ::mfrError_t  Other specific code on error if the status couldn't be retrieved.
 */
mfrError_t mfrGetUpgradeStatus(mfrUpgradeProgress_t *pStatus);

mfrError_t mfrFWUpgradeInit(void);

mfrError_t mfrFWUpgradeTerm(void);
/* End of MFRLIBS_HAL_API doxygen group */
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
