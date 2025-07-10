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
/**        \file  expdat_csum.h
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

#if !defined (__EXPDAT_CSUM_H__)
#define __EXPDAT_CSUM_H__


/**********************************************************************************************************************
 * INCLUDES
 *********************************************************************************************************************/


/**********************************************************************************************************************
   *  GLOBAL CONSTANT MACROS
   *********************************************************************************************************************/

typedef enum 
{
       kCsumBytesum___Into16Bit_BEout          //0    // VAG-style (A)
      ,kCsumBytesum___Into16Bit_LEout          //1    // VAG-style (B)
      ,kCsumWordsumBE_Into16Bit_BEout          //2
      ,kCsumWordsumLE_Into16Bit_LEout          //3
      ,kCsumBytesum___Into2Compl16Bit_BEout    //4    // GM old style
      ,kCsumWordsumBE_Into2Compl16Bit_BEout    //5
      ,kCsumWordsumLE_Into2Compl16Bit_LEout    //6
      ,kCsumCRC16Standard_LEout               // 7
      ,kCsumCRC16CCITT_LEout                  // 8    // unused and errorneous
      ,kCsumCRC32Standard_LEout               // 9
      ,kCsumHashSHA1                          // 10
      ,kCsumHashRIPEMD160                     // 11
      ,kCsumWordsumLE_Into2Compl16Bit_BEout   // 12
      ,kCsumCRC16CCITTTable_LEout              //13
      ,kCsumCRC16CCITTTable_BEout             // 14
      ,kCsumHashMD5                           // 15
      ,kCsumConstant                           //16
      ,kCsumCRC16CCITT_X25_LEout               //17      /* Hella specific */
      ,kCsumCRC16CCITT_X25_BEout               //18      /* Hella specific */
	  , kCsumHashSHA512                        //19
	  , kCsumHashSHA256                        //20
      ,kCsumCRC16CCITT_X25_LEout_CAFE         // 21 PREH specific X.25 calculation 
      ,kCsumCRC16CCITT_X25_BEout_CAFE         // 22: Same as 21, but with different result endianess
      ,kCsumItems                             //    /* Total number of items in Csum. Must always be the last entry. */
} ECsumMethodNames;



/**********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/**********************************************************************************************************************
 *  GLOBAL DATA PROTOTYPES
 *********************************************************************************************************************/



/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

/* Global function prototypes can be found in expdat.h */

#ifdef __cplusplus
}
#endif

#endif  /* __EXPDAT_CSUM_H__ */

/**********************************************************************************************************************
 *  END OF FILE: expdat_csum.h
 *********************************************************************************************************************/
