/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2014 by Vector Informatik GmbH.                                              All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/**        \file  expdatproc.cpp
 *        \brief  Implementation of the dataprocessing functions.
 *
 *      \details  This file contains general purpose interface functions for the DLL.
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
 *  01.07.05  2010-05-20  vishp                 HMAC Korrektur bei Berechnung mit MD5.
 *  01.08.00  2012-08-25  vishp                 SHA256 hinzugefügt.
 *
 *  01.08.07  2014-11-21  vishp   ESCAN00079758:Support ARLE compression/decompression
 *  02.00.00  2014-12-18  vishp                 Complete rework of file structure.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <windows.h>

#include "expdat.h"


/**********************************************************************************************************************
 *  LOCAL DATA
 **********************************************************************************************************************/

/*! \brief Error text corresponding to EExportStatus (see expdat.h) reported through the error function GetExportStateInfo() */
static const char *sInfoText[] = {
   "Interface state is idle or not initialized"
   ,"Incompatible version of EXPDATPROC.DLL"
   ,"General checksum calculation error"
   ,"No constant data found in INI-file"
   ,"Data compression calculation error"
   ,"Data encryption error"
   ,"Base address mis-alignment"
   ,"Data length mis-alignment"
   ,"Internal error: Unknown action item or action state"
   ,"Parameter missing or wrong for data processing"
   ,"General failure on reading key"
   ,"Key parameter duplicate defined"
   ,"Key parameter is missing"
   ,"Key parameter contains HMAC information"
   ,"Key parameter contains RSA information"
   ,"Block size must be between 128 bytes and 70000000 bytes for compression"
   ,"AES algorithm requires 16-byte block length. Align data or use PKCS7, RFC1321 or ANSI X.923 padding method"
   ,"Unknown or invalid AES padding method"
   ,"General RSA calculation error. Keys or data length incorrect"
   ,"Invalid data length for RSA operation"
   ,"Signature verification failed"
   ,"Public key invalid."
   ,"Signature has invalid size"
   ,"Dynamic length data processing operation not possible with this method. You may increase the 'buffer to file size threshold' or 'temporary buffer size'."
};

static const char *generalFailure = "General unknown error";


BOOL WINAPI _CRT_INIT(_In_ HANDLE _HDllHandle, _In_ DWORD _Reason, _In_opt_ LPVOID _Reserved);

/**********************************************************************************************************************
 **********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * DllEntryPoint()
 **********************************************************************************************************************/
BOOL WINAPI DllEntryPoint(HINSTANCE hInstance, DWORD dwReason, LPVOID plvReserved)
{
  switch(dwReason) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
      if (!_CRT_INIT(hInstance, dwReason, plvReserved))
            return(FALSE);
       break;
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_DETACH:
      if (!_CRT_INIT(hInstance, dwReason, plvReserved))
            return(FALSE);
      break;
  }

  return true;
}


/**********************************************************************************************************************
 * GetExportStateInfo()
 **********************************************************************************************************************/
/*! \brief        Reporting a text information on an error.
 *  \params[out]  infoText location to place the error text pointer for the application.
 *  \param[in]    The error status where the error shall be reported for.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 **********************************************************************************************************************/
void __declspec(dllexport) __cdecl GetExportStateInfo(char **infoText, enum EExportStatus actionState ) 
{
  if (actionState < (sizeof(sInfoText)/sizeof(char *)))
  {
     *infoText = (char *)sInfoText[actionState];
  }
  else
  {
     *infoText = (char *)generalFailure;
  }
}


