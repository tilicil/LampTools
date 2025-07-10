/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2014-2018 by Vector Informatik GmbH.                                              All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/**        \file  expdat_datproc.cpp
 *        \brief  Implementation of the dataprocessing functions.
 *
 *      \details  This file contains the interface functions for the dataprocessing routines.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  AUTHOR IDENTITY
 *  -------------------------------------------------------------------------------------------------------------------
 *  Name                          Initials      Company
 *  -------------------------------------------------------------------------------------------------------------------
 *  Armin Happel                  vishp         Vector Informatik GmbH
 *  -------------------------------------------------------------------------------------------------------------------
 *  REVISION HISTORY
 *  -------------------------------------------------------------------------------------------------------------------
 *  Version   Date        Author  Change Id     Description
 *  -------------------------------------------------------------------------------------------------------------------
 *  01.00.00  2014-12-18  vishp                 Creation
 *  01.00.01  2015-07-25  vishp                 Correcting RSA decryption with private key.
 *  01.00.02  2015-08-26  vishp                 Remove check for private key from GetRsaKeys(), moved to the calls on RsaEncrypt/decrypt.
 *  01.01.00  2016-10-13  vishp                 Support for ED25519 signature
 *                                              Add support for CMAC.
 *  01.02.00  2018-01-05  vishp                 Adding streaming support for Hash and AES data processing operations.
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "expdat.h"
#include "expdat_datproc.h"



#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *  LOCAL CONSTANT MACROS
 **********************************************************************************************************************/
#ifndef _WIN32
#define _strnicmp    strncasecmp
#endif

#ifndef _MAX_PATH
   #define  _MAX_PATH      256
#endif

#ifndef min
#define min(a, b)             ((a) < (b) ? (a) : (b)) 
#endif

#ifdef __cplusplus
#define EXP_MEM_ALLOC(type, size)       new type[(size)]
#define EXP_MEM_FREE(ptr)               delete [] (ptr)
#else
#define EXP_MEM_ALLOC(type, size)       malloc(sizeof(type)*size)
#define EXP_MEM_FREE(ptr)               free(ptr)
#endif

/**********************************************************************************************************************
 *  LOCAL FUNCTION MACROS
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/

/*! \brief XOR data structure */
typedef struct tXorParam
{
  int xorLen;
  int xorCurrent;
  BYTE *xorData;
} tXorParam;



/**********************************************************************************************************************
 *  LOCAL DATA
 **********************************************************************************************************************/


/* Name of the data processing functions (appears in the dialog field) */
static const char *dpFunctionName[] = {
    "No action"
   ,"XOR data with byte parameter"
};


/**********************************************************************************************************************
 *  GLOBAL DATA
 **********************************************************************************************************************/



/**********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************************/

static int StringToBytes(const char *textBuffer, unsigned char *data, const int maxDataLen);


/**********************************************************************************************************************
 **********************************************************************************************************************
 *  LOCAL FUNCTIONS
 **********************************************************************************************************************
 **********************************************************************************************************************/


  /********************************************************/


/**********************************************************************************************************************
 * InitXorOperation()
 **********************************************************************************************************************/
/*! \brief        Initializes the AES workspace data structure.
 *  \param[in]    info: Pointer to the complete dataprocessing workspace.
 *  \param[out]   xorParam: Pointer to the XOR workspace.
 *  \return       TRUE:  Initialisation successfully completed.
 *                FALSE: Initialisation failed. Detailed error code in info->exState.
 **********************************************************************************************************************/
static bool InitXorOperation(TExportDataInfo *info, tXorParam *xorParam)
{
   char     keyBuffer[1024];
   int      maxLen=0;


   if ((info->generalParam!=NULL) && (maxLen=strlen(info->generalParam)) > 0)
   {
     strncpy(keyBuffer, info->generalParam, sizeof(keyBuffer)-1 );
     keyBuffer[sizeof(keyBuffer)-1] = '\0';

     maxLen = (int)strlen(keyBuffer);
   }

   if (maxLen == 0)  // No data in buffer or NULL-string
   {
      maxLen = 1;
      xorParam->xorData = (BYTE *)ExpDat_AllocWorkspace(sizeof(unsigned char));
      xorParam->xorLen = 1;
      xorParam->xorData[0] = 0xFF;
   }
   else
   {
      xorParam->xorData = (BYTE *)ExpDat_AllocWorkspace( (sizeof(unsigned char) * maxLen) );
      xorParam->xorLen = StringToBytes((const char *)info->generalParam,xorParam->xorData,maxLen);
   }
   xorParam->xorCurrent=0;     /* Begin with first byte */

   return true;
}

/**********************************************************************************************************************
 * StringToBytes()
 **********************************************************************************************************************/
/*! \brief        Converts the hex string into a byte array.
 *  \param[in]    textBuffer: NULL terminated string that contains HEX-ormatted string, e.g. 0xE4 or EA36DE7.
 *  \params[out]  data: Points to a buffer where the results of the data are placed to.
 *  \param[in]    Max. length of the output buffer.
 **********************************************************************************************************************/
static int StringToBytes(const char *textBuffer, unsigned char *data, const int maxDataLen)
{
   char tempStr[5];
   int rc=0;

   while (*textBuffer)
   {
      // Skip any blanks or commas.
      if (*textBuffer == ' ' || *textBuffer == ',')
      {
         textBuffer++;
         continue;
      }
      if (rc == maxDataLen) return rc;

      // If hex-byte starts with '0x'
      if ((textBuffer[0]=='0') && (textBuffer[1]=='x'))
      {
         textBuffer++;
         textBuffer++;
      }

      // make the byte a hex string.
      tempStr[0] = '0';
      tempStr[1] = 'x';
      tempStr[2] = *textBuffer++;
      if (*textBuffer)  tempStr[3] = *textBuffer++;
      else              tempStr[3] = '\0';
      tempStr[4] = '\0';
      data[rc] = (unsigned char)strtoul(tempStr, NULL, 0);
      rc++;
   }
   return rc;
}


/**********************************************************************************************************************
 **********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * GetDataProcessingFunctionCount()
 **********************************************************************************************************************/
/*! \brief        Provides the total number of available dataprocessing functions.
 *  \return       The number of available checksum functions.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 **********************************************************************************************************************/
DLL_FUNC(int) GetDataProcessingFunctionCount(void)
{
   return kDatProcItems; 
}


/**********************************************************************************************************************
 * GetDataProcessingFunctionName()
 **********************************************************************************************************************/
/*! \brief        Provides a verbal name of the dataprocessing function used in dialog boxes.
 *  \param[in]    index: The index number of the operation.
 *  \param[out]   Pointer to the string buffer where the text will be placed to.
 *  \param[in]    Size: Number of bytes available in the space where name points to.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 *                IMPORTANT: Enough space must be available for name in the calling routine or string is cut off!!
 **********************************************************************************************************************/
DLL_FUNC(bool) GetDataProcessingFunctionName(int index, char * name, int size)
{
   /* Not enough string elements available as requested */
   if (index >= (sizeof(dpFunctionName)/sizeof(const unsigned char *)))
   {
      return false;
   }

   sprintf(name, "%2d:", index);
   size -= (int)strlen(name);
   size--;  // char. termination.
   strncat(name, dpFunctionName[index], size);

   return true;
}


/**********************************************************************************************************************
 * InitDataProcessing()
 **********************************************************************************************************************/
/*! \brief        This function is called from the main EXE to initialize the data processing operation.
 *  \details      Work to do: The function allocates a workspace for the upcoming operation.
 *                In many cases, the function takes a parameter string for input.
 *  \param[in]    info: The workspace for the operation.
 *  \return       True if operation was successful
 *                False otherwise.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 *                If the return value is false, then the exState contains a more detailed error indication.
 *     Important: Thus, after a succesful Init, a deinitialisation is required to free the workspace!
 **********************************************************************************************************************/
DLL_FUNC(bool) InitDataProcessing( TExportDataInfo *info )
{
   bool rval=false;


   /* Check for interface version */
   if (info->DllInterfaceVersion != DllInterfaceVersion)
   {
      info->exState = ExportStateDllInterfaceVersionError;
      return false;
   }

   info->voidPtr = NULL;
   info->expDatResultSize = 0;
   info->exState = ExportStateDataEncryptionError;    // General failure.

   switch (info->index) 
   {
      case kDatProcNoAction:
            rval = true;
            break;

      case kDatProcXoring:
            info->voidPtr = ExpDat_AllocWorkspace(sizeof(tXorParam));
            rval = InitXorOperation(info, (tXorParam *)info->voidPtr);

            break;

   } 

   return rval;
}


/**********************************************************************************************************************
 * DoDataProcessing()
 **********************************************************************************************************************/
/*! \brief        Does the dataprocessing operation.
 *  \details      This function runs the dataprocessing calculation. It can be called several times.
 *                At least one time per block (see actionState).
 *  \param[in]    info: The workspace for the operation.
 *  \return       True if operation was successful
 *                False otherwise.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 *                If the return value is false, then the exState contains a more detailed error indication.
 **********************************************************************************************************************/
DLL_FUNC(bool) DoDataProcessing( TExportDataInfo *info )
{
    bool rval=false;
    info->exState = ExportStateDataEncryptionError;    // General failure.


   /* Check for interface version */
   if (info->DllInterfaceVersion != DllInterfaceVersion)
   {
      info->exState = ExportStateDllInterfaceVersionError;
      return false;
   }

    switch (info->index) 
    {

      // 2-Complement without carry -> unsigned short
      case kDatProcNoAction: 
                rval = true;
       break;

      case kDatProcXoring: 
         {
            char *pcBuf=info->segInData;
            DWORD i;


             // By default, in- and out-data are identically.
            // If nothing's changed here, in could be used for out.
            // Otherwise, adapt segOutXxx values in info-struct.
            for (i=0l ; i < info->segInLength ; i++)
            {
               *pcBuf = *pcBuf ^ ((tXorParam *)(info->voidPtr))->xorData[((tXorParam *)(info->voidPtr))->xorCurrent++];
               pcBuf++;
               if (((tXorParam *)(info->voidPtr))->xorCurrent >= ((tXorParam *)(info->voidPtr))->xorLen) ((tXorParam *)(info->voidPtr))->xorCurrent=0;
            }
         }
         rval = true;
       break;

   } 

   return rval;
}

/**********************************************************************************************************************
 * DeinitDataProcessing()
 **********************************************************************************************************************/
/*! \brief        Concludes a checksum operation.
 *  \param[in]    info: The workspace for the operation.
 *  \return       True if operation was successful
 *                False otherwise.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 *                If the return value is false, then the exState contains a more detailed error indication.
 **********************************************************************************************************************/
DLL_FUNC(bool) DeinitDataProcessing(TExportDataInfo *info )
{
    bool rval=false;
    info->exState = ExportStateDataEncryptionError;    // General failure.


    /* Check for interface version */
    if (info->DllInterfaceVersion != DllInterfaceVersion)
    {
      info->exState = ExportStateDllInterfaceVersionError;
      return false;
    }

    info->expDatResultSize = 0;  /* Data Processing has only exceptionally results in result buffer (i.e. signature). */

    switch (info->index) 
    {

      // No action
      case kDatProcNoAction:   
               rval = true;
              break;

      // Increment by 1
      case kDatProcXoring:   
            if (NULL!=info->voidPtr) {
               ExpDat_FreeWorkspace((void **)&(((tXorParam *)(info->voidPtr))->xorData));
               ExpDat_FreeWorkspace((void **)&(info->voidPtr)); 
            }  
            rval = true;
            break;

   } 

   return rval;
}



#ifdef __cplusplus
}
#endif

/**********************************************************************************************************************
 *  END OF FILE: expdat_datproc.cpp
 *********************************************************************************************************************/



