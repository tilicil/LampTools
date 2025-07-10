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
/**        \file  expdat_csum.cpp
 *        \brief  Implementation of the checksum calculation functions.
 *
 *      \details  This file contains the interface functions for the checksum calculations.
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
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <string.h>
#include <stdio.h>

#include "expdat.h"
#include "expdat_csum.h"
#include "expdat_csumTables.h"


/**********************************************************************************************************************
 *  LOCAL CONSTANT MACROS
 **********************************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *  LOCAL FUNCTION MACROS
 **********************************************************************************************************************/



/**********************************************************************************************************************
 *  LOCAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/



/**********************************************************************************************************************
 *  LOCAL DATA
 **********************************************************************************************************************/

static const char *csumFunctionName[] = {
     "ByteSum into 16-Bit, BE-out"
   , "ByteSum into 16-Bit, LE-out"
   , "Wordsum BE into 16-Bit, BE-Out"
   , "Wordsum LE into 16-Bit, LE-Out"  
   , "ByteSum w/ 2s complement into 16-Bit BE (GM old-style)"  
   , "Wordsum BE into 16-Bit, 2's Compl BE-Out (GM new style)"
   , "Wordsum LE into 16-Bit, 2's Compl LE-Out (GM new style)"
   , "CRC16 CCITT X.25 LE-Out (CAFE)"
   , "CRC16 CCITT X.25 BE-Out (CAFE)"
};


/**********************************************************************************************************************
 *  GLOBAL DATA
 **********************************************************************************************************************/

/** Declaration of global and uninitialized vars ***********************************************************************/



/**********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************************/

static bool BeginChecksumCalculation(TExportDataInfo *info);
static bool DoChecksumCalculation(TExportDataInfo *info);
static bool EndChecksumCalculation(TExportDataInfo *info, BYTE *pResult, WORD *pResultLen);

static int  GetChecksumLength(int index);

/**********************************************************************************************************************
 **********************************************************************************************************************
 *  LOCAL FUNCTIONS
 **********************************************************************************************************************
 **********************************************************************************************************************/


/**********************************************************************************************************************
 * GetChecksumLength()
 **********************************************************************************************************************/
/*! \brief        Provides the length of the results in bytes.
 *  \param[in]    index: The index number of the operation.
 *  \return       The length of the operation results in bytes.
 **********************************************************************************************************************/
static int  GetChecksumLength(int index)
{
   int result=0;


   switch (index) 
   {
      case kCsumBytesum___Into16Bit_BEout:  
      case kCsumBytesum___Into16Bit_LEout:  
      case kCsumWordsumBE_Into16Bit_BEout:  
      case kCsumWordsumLE_Into16Bit_LEout:  
      case kCsumBytesum___Into2Compl16Bit_BEout:  
      case kCsumWordsumBE_Into2Compl16Bit_BEout:  
      case kCsumWordsumLE_Into2Compl16Bit_LEout:  
      case kCsumCRC16CCITT_X25_LEout_CAFE:
      case kCsumCRC16CCITT_X25_BEout_CAFE:
               result = 2;
               break;

      default: result = 0;
   }

   return result;
}


/**********************************************************************************************************************
 * BeginChecksumCalculation()
 **********************************************************************************************************************/
/*! \brief        Called when checksum operaton shall begin [DoCalculateChecksum(info, csumActionBegin)].
 *  \details      Work to do: initialize your workspace parameters
 *  \param[in]    info: Pointer to the context buffer with its workspace.
 *  \return       TRUE if the operation has succeeded,
 *                False otherwise.
 **********************************************************************************************************************/
static bool BeginChecksumCalculation(TExportDataInfo *info)
{
   bool result=false;
   unsigned short *pwCS;


   // -------------------------------------
   // Action item: Initialize system layer.
   // -------------------------------------
   switch (info->index) {

   // 2-Complement without carry -> unsigned short
   case kCsumBytesum___Into16Bit_BEout:
   case kCsumBytesum___Into16Bit_LEout: 
   case kCsumWordsumBE_Into16Bit_BEout:
   case kCsumWordsumLE_Into16Bit_LEout:
   case kCsumBytesum___Into2Compl16Bit_BEout:
   case kCsumWordsumBE_Into2Compl16Bit_BEout:
   case kCsumWordsumLE_Into2Compl16Bit_LEout:
         /* Set the checksum start value */
         pwCS = (unsigned short *)info->voidPtr;
         *pwCS = 0;

         result = true;
      break;

   case kCsumCRC16CCITT_X25_LEout_CAFE:
   case kCsumCRC16CCITT_X25_BEout_CAFE:
         pwCS = (unsigned short *)info->voidPtr;
         *pwCS = 0xCAFE;//0xCAFE;

         result = true;
      break;
   } 

   return result;
}

/**********************************************************************************************************************
 * DoChecksumCalculation()
 **********************************************************************************************************************/
/*! \brief        Called when checksum operaton shall be executed on data [DoCalculateChecksum(info, csumActionDoData)].
 *  \details      Work to do: Calculate the checksum
 *  \param[in]    info: Pointer to the context buffer with its workspace.
 *  \return       TRUE if the operation has succeeded,
 *                False otherwise.
 *  \note         This function will be called several times, each time per block (continuous address space).
 **********************************************************************************************************************/
static bool DoChecksumCalculation(TExportDataInfo *info)
{
   bool result=false;
   unsigned short *pwCS;
   DWORD           i;

   switch (info->index) 
   {
   case kCsumBytesum___Into16Bit_BEout:
   case kCsumBytesum___Into16Bit_LEout:
         pwCS = (unsigned short *)info->voidPtr;
         for (i=0l; i < info->segInLength; i++) 
         {
            *pwCS += (BYTE) (((unsigned char)info->segInData[i])&0xff);
         }

         result = true;
      break;

   case kCsumWordsumBE_Into16Bit_BEout: 
   case kCsumWordsumBE_Into2Compl16Bit_BEout:
         pwCS = (unsigned short *)info->voidPtr;
         // if address/length correction necessary 
         // do this here in segOutLength and segOutAddress.
         if (info->segInAddress&(DWORD)1)
         {
            info->exState = ExportStateChecksumAddressMisalignedError;
            return false;
         }
         if (info->segInLength&(DWORD)1)
         {
            info->exState = ExportStateChecksumLengthMisalignedError;
            return false;
         }
         for (i=0l; i < info->segInLength; i+=2) 
         {
            WORD tmp;

            /* This is big endian summary of 16-bit values */
            tmp  = (WORD)info->segInData[i+1] & 0x00ff;
            tmp |= (((WORD)info->segInData[i])*256) & 0xff00;

            *pwCS += tmp;
         }
         result = true;
      break;

   case kCsumBytesum___Into2Compl16Bit_BEout:
         pwCS = (unsigned short *)info->voidPtr;
         for (i=0l; i < info->segInLength; i++) 
         {
            *pwCS += (BYTE) (((~(unsigned char)info->segInData[i])&0xff) + 1);
         }
         result = true;
      break;

   case kCsumWordsumLE_Into16Bit_LEout:
   case kCsumWordsumLE_Into2Compl16Bit_LEout: 
   case kCsumCRC16CCITT_X25_LEout_CAFE:
   case kCsumCRC16CCITT_X25_BEout_CAFE:
      {
         BYTE table_index;
         pwCS = (unsigned short *) info->voidPtr;
         for( i=0; i < info->segInLength; i++)
         {
            table_index = (((BYTE)((*pwCS) >> 8)) ^ ((BYTE)info->segInData[i]));
            (*pwCS)  = (((WORD)((*pwCS) << 8)) ^ kausCrcTable16_cafe[table_index]);
         }
         result = true;
      }
      break;
   default:
      break;
   } 

   return result;
}

/**********************************************************************************************************************
 * EndtChecksumCalculation()
 **********************************************************************************************************************/
/*! \brief        Called when checksum operaton is finished on all data [DoCalculateChecksum(info, csumActionEnd)].
 *  \details      Work to do: Finalize the calculation and present the result
 *  \param[in]    info: Pointer to the context buffer with its workspace.
 *  \return       TRUE if the operation has succeeded,
 *                False otherwise.
 *  \note         This function typically provides the results of the operation in info->segOutData/info->segOutLength.
 **********************************************************************************************************************/
static bool EndChecksumCalculation(TExportDataInfo *info, BYTE *pResult, WORD *pResultLen)
{
   bool result=false;
   WORD resultLen;
   unsigned short *pwCS;


   resultLen = GetChecksumLength(info->index);
   if (resultLen > *pResultLen)
   {
      resultLen = *pResultLen;
   }
   else
   {
      *pResultLen = resultLen;
   }


   switch (info->index) 
   {
      case kCsumBytesum___Into16Bit_LEout:
      case kCsumWordsumLE_Into16Bit_LEout:
            memcpy(pResult, info->voidPtr, resultLen);
            result = true;
         break;

      case kCsumWordsumBE_Into16Bit_BEout: 
      case kCsumBytesum___Into16Bit_BEout:
            pwCS = (unsigned short *)info->voidPtr;
            {
               int tmp;

               // swap lo- and hi-byte-
               tmp  = (*pwCS>>8)&0xff;
               *pwCS <<= 8;
               *pwCS &= 0xff00;
               *pwCS |= tmp;
            }
            memcpy(pResult, info->voidPtr, resultLen);
            result = true;
         break;

      case kCsumBytesum___Into2Compl16Bit_BEout:
            pwCS = (unsigned short *)info->voidPtr;
            {
               int tmp;

               // swap lo- and hi-byte-
               tmp  = (*pwCS>>8)&0xff;
               *pwCS <<= 8;
               *pwCS &= 0xff00;
               *pwCS |= tmp;

            }
            memcpy(pResult, info->voidPtr, resultLen);
            result = true;
         break;
      case kCsumWordsumBE_Into2Compl16Bit_BEout:
            pwCS = (unsigned short *)info->voidPtr;
            {
               int tmp;

               // Make 2's complement.
               *pwCS = ~*pwCS + 1;
               // swap lo- and hi-byte-
               tmp  = (*pwCS>>8)&0xff;
               *pwCS <<= 8;
               *pwCS &= 0xff00;
               *pwCS |= tmp;
            }
            memcpy(pResult, info->voidPtr, resultLen);
            result = true;
         break;
      case kCsumWordsumLE_Into2Compl16Bit_LEout:
            pwCS = (unsigned short *)info->voidPtr;
            // Make 2's complement.
            *pwCS = ~*pwCS + 1;

            memcpy(pResult, info->voidPtr, resultLen);
            result = true;
         break;

      default:
         break;
   }

   return result;
}


/**********************************************************************************************************************
 **********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * GetChecksumFunctionCount()
 **********************************************************************************************************************/
/*! \brief        Provides the total number of available checksum functions.
 *  \return       The number of available checksum functions.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 **********************************************************************************************************************/
DLL_FUNC(int) GetChecksumFunctionCount() 
{ 
   return kCsumItems; 
}

/**********************************************************************************************************************
 * GetChecksumFunctionName()
 **********************************************************************************************************************/
/*! \brief        Provides a verbal name of the checksum function used in dialog boxes.
 *  \param[in]    index: The index number of the operation.
 *  \param[out]   Pointer to the string buffer where the text will be placed to.
 *  \param[in]    Size: Number of bytes available in the space where name points to.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 *                IMPORTANT: Enough space must be available for name in the calling routine or string is cut off!!
 **********************************************************************************************************************/
DLL_FUNC(bool) GetChecksumFunctionName(int index, char * name, int size)
{
   /* Not enough string elements available as requested */
   if (index >= (sizeof(csumFunctionName)/sizeof(const unsigned char *)))
   {
      return false;
   }

   sprintf(name, "%2d:", index);
   size -= (int)strlen(name);
   size--;  // char. termination.
   strncat(name, csumFunctionName[index], size);

   return true;
}

/**********************************************************************************************************************
 * GetChecksumSizeOfResult()
 **********************************************************************************************************************/
/*! \brief        Provides the length of the results in bytes.
 *  \param[in]    index: The index number of the operation.
 *  \return       The length of the operation results in bytes.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 **********************************************************************************************************************/
DLL_FUNC(int) GetChecksumSizeOfResult(int index)
{
   return GetChecksumLength(index);
}


/**********************************************************************************************************************
 * InitChecksum()
 **********************************************************************************************************************/
/*! \brief        This function is called from the main EXE to initialize the checksum calculation.
 *  \details      Work to do: The function allocates a workspace for the upcoming operation.
 *  \param[in]    info: The workspace for the operation.
 *  \return       True if operation was successful
 *                False otherwise.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 *                If the return value is false, then the exState contains a more detailed error indication.
 *     Important: Thus, after a succesful Init, a deinitialisation is required to free the workspace!
 **********************************************************************************************************************/
DLL_FUNC(bool) InitChecksum( TExportDataInfo *info )  
{
   bool rval=false;

      
   /* Check for interface version */
   if (info->DllInterfaceVersion != DllInterfaceVersion)
   {
      info->exState = ExportStateDllInterfaceVersionError;
      return false;
   }

   // Default error info.
   info->exState = ExportStateUnknownActionItemOrActionType;
   info->expDatResultSize = 0;
   info->voidPtr = NULL;

   // -----------------------------------------
   // Create the context pointer to data memory.
   // -----------------------------------------
   switch (info->index) 
   {
      case kCsumBytesum___Into16Bit_BEout:
      case kCsumBytesum___Into16Bit_LEout:  
      case kCsumWordsumBE_Into16Bit_BEout:
      case kCsumWordsumLE_Into16Bit_LEout:
      case kCsumBytesum___Into2Compl16Bit_BEout:
      case kCsumWordsumBE_Into2Compl16Bit_BEout:
      case kCsumWordsumLE_Into2Compl16Bit_LEout:
      case kCsumCRC16CCITT_X25_LEout_CAFE: 
      case kCsumCRC16CCITT_X25_BEout_CAFE:
            info->voidPtr =  ExpDat_AllocWorkspace(sizeof(unsigned short));
            rval = true;
            break;

      default:
            break;
   } 

   return rval;
}

/**********************************************************************************************************************
 * DoCalculateChecksum()
 **********************************************************************************************************************/
/*! \brief        Does the checksum operation.
 *  \details      This function runs the checksum calculation. It can be called several times.
 *                At least three times per block (see actionState).
 *  \param[in]    info: The workspace for the operation.
 *  \param[in]    actionState: One of the three action states in sequence: 
 *                  csumActionBegin:  Initialises the checksum operation.
 *                  csumActionDoData: Performs the operation on the given data of a block.
 *                                    Can be called multiple times, one time per data block (continuous address space).
 *                  csumActionEnd:    Concludes the checksum operation and provides the result.
 *  \return       True if operation was successful
 *                False otherwise.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 *                If the return value is false, then the exState contains a more detailed error indication.
 **********************************************************************************************************************/
DLL_FUNC(bool) DoCalculateChecksum(TExportDataInfo *info,
                                                      EChecksumAction actionState)
{
   bool result=false;


    /* Check for interface version */
    if (info->DllInterfaceVersion != DllInterfaceVersion)
    {
      info->exState = ExportStateDllInterfaceVersionError;
      return false;
    }

   // Default error info.
   info->exState = ExportStateUnknownActionItemOrActionType;

   if (actionState==CSumActionBegin)
   {
      /* Initialise the workspace per operation */
      result = BeginChecksumCalculation(info);
   }
   else if (actionState==CSumActionDoData)
   {
      /* Run the checksum calculation */
      result = DoChecksumCalculation(info);
   }
   else if (actionState==CSumActionEnd)
   {
      /* Conclude the checksum calculation and provide the results  */
      info->expDatResultSize = EXPDAT_RESULTS_MAXSIZE;
      result = EndChecksumCalculation(info, info->expDatResults, &(info->expDatResultSize));

      /* A copy is placed in a special buffer for the commandline */
      if (result==true)
      {
         info->segOutData = (char *)info->expDatResults;
         info->segOutLength = info->expDatResultSize;  // 2 byte data in output.
      }
      else
      {
         info->expDatResultSize = 0;
      }
   }

   return result;
}

/**********************************************************************************************************************
 * DeinitChecksum()
 **********************************************************************************************************************/
/*! \brief        Concludes a checksum operation.
 *  \param[in]    info: The workspace for the operation.
 *  \return       True if operation was successful
 *                False otherwise.
 *  \note         This is an exported interface function of the DLL intended to be called from the EXE.
 *                If the return value is false, then the exState contains a more detailed error indication.
 **********************************************************************************************************************/
DLL_FUNC(bool) DeinitChecksum( TExportDataInfo *info )  
{
   bool rval=false;


   /* Check for interface version */
   if (info->DllInterfaceVersion != DllInterfaceVersion)
   {
      info->exState = ExportStateDllInterfaceVersionError;
      return false;
   }

   // Default error info.
   info->exState = ExportStateUnknownActionItemOrActionType;

   // -----------------------------------------
   // Delete the context data memory
   // -----------------------------------------
   switch (info->index) 
   {
      case kCsumBytesum___Into16Bit_BEout:
      case kCsumBytesum___Into16Bit_LEout:  
      case kCsumWordsumBE_Into16Bit_BEout:
      case kCsumWordsumLE_Into16Bit_LEout:
      case kCsumBytesum___Into2Compl16Bit_BEout:
      case kCsumWordsumBE_Into2Compl16Bit_BEout:
      case kCsumWordsumLE_Into2Compl16Bit_LEout:
      case kCsumCRC16CCITT_X25_LEout_CAFE:
      case kCsumCRC16CCITT_X25_BEout_CAFE:
            ExpDat_FreeWorkspace((void **)&(info->voidPtr)); 
            rval = true;
            break;
      default:
            break;
   } 

   return rval;
}


#ifdef __cplusplus
}
#endif

/**********************************************************************************************************************
 *  END OF FILE: expdat_csum.cpp
 *********************************************************************************************************************/



