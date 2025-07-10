/////////////////////////////////////////////////////////
//
//  Filename: EXPDAT.H
//
//  Descripition: Specifies the Export types.
//
//  Used for checksum and data processing during the data-export.
// This is the interface header-file between EXPDATPROC.DLL and the Hex-View Program.
// 
// 2015-01-03  V03.00.00   Interface version incremented because functions to allocate memory 
//                         from host have been introduced in ExportDataInfo.
//                         Change valid since expodatproc.dll V2.00.
//
// 2018-01-30  V5.00.00    Supporting streaming API for several operations.


#ifndef __EXPDAT_H__
#define __EXPDAT_H__

#ifdef _WIN32
#include <Windows.h>
#endif
/* Re-defined in v_def.h and AUTOSAR types */
# ifndef BYTE
#  define BYTE  unsigned char
# endif
# ifndef WORD
#  define WORD  unsigned short
# endif
# ifndef DWORD
#  define DWORD  unsigned long
# endif
# ifndef __cplusplus
#  define bool       boolean
#  define true       TRUE
#  define false      FALSE
# endif

// Used in csum an datproc
#define ExpDat_AllocWorkspace(size)      malloc(size)
#define ExpDat_FreeWorkspace(p)          if (*p!=NULL) { \
                                            free(*(p));  \
                                         }               \
                                         *p=NULL


/* Give FORMAT parameter in the expdat proc a meaning */
#define EXPDAT_FORMAT_GENERAL_PARAM_BINARY         0x27u

/* Define the update mode for data processing and signature verification. */
/* The mode bits must be set in the DoDataOperation flag byte */
#define DODATA_START                  0x01    // Flag is set when starting the update of a data block.
#define DODATA_UPDATE                 0x02    // Flag is set when more data to be processed (start operation flag was already given).
#define DODATA_FINISH                 0x04    // Flag is set when the operation will be concluded.

#define DODATA_START_UPDATE           (DODATA_START|DODATA_UPDATE)
#define DODATA_START_UPDATE_FINISH    (DODATA_START|DODATA_UPDATE|DODATA_FINISH)
#define DODATA_UPDATE_FINISH          (DODATA_UPDATE|DODATA_FINISH)

// Export type (unused).
typedef enum EExportType
{
   ExportTypeSRecord
  ,ExportTypeIntelHex
  ,ExportTypeFiatBinary
} EExportType;

enum EInterfaceVersion {
   DllInterfaceVersion=0005
  ,DllInterfaceVersion_0004=0004
  ,DllInterfaceVersion_0003=0003   // Used for compatibility checks.
  ,DllVersionIllegal=-1
};

// Streaming operation.
typedef enum EChecksumAction 
{
   CSumActionBegin,
   CSumActionDoData,
   CSumActionEnd
} EChecksumAction;

// Error codes.
typedef enum EExportStatus
{
   ExportStateIdle=0
  ,ExportStateDllInterfaceVersionError
  ,ExportStateChecksumCalculationError
  ,ExportStateNoConstDataResults
  ,ExportStateDataCompressionCalculationError
  ,ExportStateDataEncryptionError
  ,ExportStateChecksumAddressMisalignedError
  ,ExportStateChecksumLengthMisalignedError
  ,ExportStateUnknownActionItemOrActionType
  ,ExportStateParamterMissingForDataProcessing
  ,ExportStateKeyGeneralError
  ,ExportStateKeyDuplicateParamter
  ,ExportStateKeyParameterMissing
  ,ExportStateKeyIsHMAC
  ,ExportStateKeyIsRSA
  ,ExportStateWrongBytesPerBlockType
  ,ExportStateWrongAESBlockSize
  ,ExportStateWrongAESParameterExtension
  ,ExportStateGeneralRSAError
  ,ExportStateRSAInvalidLength
  ,ExportStateSignatureVerificationFailed
  ,ExportStatePublicKeyInvalid
  ,ExportStateSignatureInvalidSize
  ,ExportStateDynamicLengthOperationNotPossible
} EExportStatus;

/* Declare user interface functions as DLL */
#ifdef _USRDLL
#define DLL_FUNC(retval)  retval __declspec(dllexport) __cdecl
#else
/* Normal C-Interfaces */
#define DLL_FUNC(retval)   retval
#endif


// Used to identify the name of the DLL in function 'LoadLibrary'.
#define EXPDAT_DLL_NAME    "EXPDATPROC.DLL"

#define EXPDAT_RESULTS_MAXSIZE      1024

// Functions used to allocate and deallocate host memory.
// Used when segOutData array need to be extended or changed.
typedef void * (*TAllocHostMemory)(int size);
typedef void   (*TFreeHostMemory)(void *ptr);


typedef struct  TExportDataInfo
{
   enum EInterfaceVersion DllInterfaceVersion;

   // Input-section:
   // =============

   // Segment data information
   EExportType    exType;           // Provides information of export type (see above).
   int            index;            // Selected service index.

   BYTE           format;           // format byte (taken from the dialog).
   char          *generalParam;     // General purpose string (will be taken from the dialogs).

   DWORD          segInAddress;     // These information are not valid during Init and Deinit.
   DWORD          segInLength;
   char          *segInData;

   // Output section:
   // ==============

   // Segment data information
   DWORD          segOutAddress;    // On call to Checksum and process data, outXxx
   DWORD          segOutLength;     // and InXxx are identically. OutXxx will be
   char          *segOutData;       // taken for output data from ProcessData.

   EExportStatus  exState;          // Call function can provide more detailed 
                                    // status information here.
   DWORD          maxSegLen;        // max.number of bytes allowed for segInLength.

   void          *voidPtr;          // used for future general purpose pointer (e.g. context data).

   BYTE           expDatResults[EXPDAT_RESULTS_MAXSIZE];
   WORD           expDatResultSize;

   TAllocHostMemory  HostAllocMemory;  // DLL calls this function to re-allocate host memory.
   TFreeHostMemory   HostFreeMemory;   // or release it.
   
   BYTE           doDataOperation;     // This flag field indicates the mode of operation for data processing (START|UPDATE|FINISH).

   DWORD          segInTotalLength;    // Provides the total segment length of a buffer.
   LPCSTR         segInPath;           // This is the path of the to a file in case the data are located in a file.
   LPCSTR         segOutPath;          // The target file where the data will be located to.

   DWORD          reserved[8];         // Space in the struct for future and backward compatible operation.

} TExportDataInfo;

// Interface function names.
#define EXPNAME_CSUMFCTCOUNT  "GetChecksumFunctionCount"
#define EXPNAME_CSUMFCTNAME   "GetChecksumFunctionName"
#define EXPNAME_INITCSUM      "InitChecksum"
#define EXPNAME_DEINITCSUM    "DeinitChecksum"
#define EXPNAME_DOCSUM        "DoCalculateChecksum"

#define EXPNAME_CSUMRESULTSIZE "GetChecksumSizeOfResult"

#define EXPNAME_STATEINFO     "GetExportStateInfo"

#define EXPNAME_DPROCFCTCOUNT "GetDataProcessingFunctionCount"
#define EXPNAME_DPROCFCTNAME  "GetDataProcessingFunctionName"
#define EXPNAME_INITDPROC     "InitDataProcessing"
#define EXPNAME_DEINITDPROC   "DeinitDataProcessing"
#define EXPNAME_DODPROC       "DoDataProcessing"

#define EXPNAME_SIGNVERFCTCOUNT "GetSignVerifyFunctionCount"
#define EXPNAME_SIGNVERFCTNAME  "GetSignVerifyFunctionName"
#define EXPNAME_INITSIGNVER     "InitSignVerify"
#define EXPNAME_DEINITSIGNVER   "DeinitSignVerify"
#define EXPNAME_DOSIGNVER       "DoSignVerify"


#ifdef STATIC_EXPDATPROC_DLL
extern "C"  int __declspec(dllimport) __cdecl GetChecksumFunctionCount(void);
extern "C"   bool __declspec(dllimport) __cdecl GetChecksumFunctionName(int index, char * name, int size);
extern "C"   int __declspec(dllimport) __cdecl GetChecksumSizeOfResult(int index);
extern "C"   bool __declspec(dllimport) __cdecl InitChecksum( TExportDataInfo *info )  ;
extern "C"   bool __declspec(dllimport) __cdecl DeinitChecksum( TExportDataInfo *info )  ;
extern "C"   bool __declspec(dllimport) __cdecl DoCalculateChecksum(TExportDataInfo *info,
                                                       EChecksumAction actionState);

extern "C"   void __declspec(dllimport) __cdecl GetExportStateInfo(char **infoText, enum EExportStatus actionState ) ;


extern "C"   int __declspec(dllimport) __cdecl GetDataProcessingFunctionCount(void);
extern "C"   bool __declspec(dllimport) __cdecl GetDataProcessingFunctionName(int index, char * name, int size);
extern "C"   bool __declspec(dllimport) __cdecl InitDataProcessing( TExportDataInfo *info )  ;
extern "C"   bool __declspec(dllimport) __cdecl DeinitDataProcessing(TExportDataInfo *info )  ;
extern "C"   bool __declspec(dllimport) __cdecl DoDataProcessing( TExportDataInfo *info )  ;

extern "C"   int __declspec(dllimport) __cdecl GetSignVerifyFunctionCount(void);
extern "C"   bool __declspec(dllimport) __cdecl GetSignVerifyFunctionName(int index, char * name, int size);
extern "C"   bool __declspec(dllimport) __cdecl InitSignVerify( TExportDataInfo *info )  ;
extern "C"   bool __declspec(dllimport) __cdecl DeinitSignVerify(TExportDataInfo *info )  ;
extern "C"   bool __declspec(dllimport) __cdecl DoSignVerify( TExportDataInfo *info )  ;

#endif

#if defined(_USRDLL) || defined (PROGRAM_HEXVIEW)
  typedef int  (* TGetChecksumFunctionCount)(void);
  typedef bool (* TGetChecksumFunctionName)(int index, char * name, int maxNamelen);
  typedef bool (* TInitChecksum)(           TExportDataInfo *info );
  typedef bool (* TDeinitChecksum)(         TExportDataInfo *info );
  typedef bool (* TDoCalculateChecksum)(    TExportDataInfo *info, EChecksumAction actionState );

  typedef int  (* TGetChecksumSizeOfResult)( int index );

  typedef void (* TGetExportStateInfo) (    char **infoText, enum EExportStatus actionState );

  typedef int  (* TGetDataProcessingFunctionCount)(void);
  typedef bool (* TGetDataProcessingFunctionName)(int index, char * name, int maxNamelen);
  typedef bool (* TInitDataProcessing)(           TExportDataInfo *info);
  typedef bool (* TDeinitDataProcessing)(         TExportDataInfo *info);
  typedef bool (* TDoDataProcessing)(             TExportDataInfo *info);

  typedef int  (* TGetSignVerifyFunctionCount)(void);
  typedef bool (* TGetSignVerifyFunctionName)(int index, char * name, int maxNamelen);
  typedef bool (* TInitSignVerify)(           TExportDataInfo *info);
  typedef bool (* TDeinitSignVerify)(         TExportDataInfo *info);
  typedef bool (* TDoSignVerify)(             TExportDataInfo *info);
#else
#ifdef __cplusplus
extern "C" {
#endif
  int  GetChecksumFunctionCount(void);
  bool GetChecksumFunctionName(int index, char * name, int maxNamelen);
  bool InitChecksum(           TExportDataInfo *info );
  bool DeinitChecksum(         TExportDataInfo *info );
  bool DoCalculateChecksum(    TExportDataInfo *info, EChecksumAction actionState );

  int  GetChecksumSizeOfResult( int index );

  void GetExportStateInfo (    char **infoText, enum EExportStatus actionState );

  int  GetDataProcessingFunctionCount(void);
  bool GetDataProcessingFunctionName(int index, char * name, int maxNamelen);
  bool InitDataProcessing(           TExportDataInfo *info);
  bool DeinitDataProcessing(         TExportDataInfo *info);
  bool DoDataProcessing(             TExportDataInfo *info);

  int  GetSignVerifyFunctionCount(void);
  bool GetSignVerifyFunctionName(int index, char * name, int maxNamelen);
  bool InitSignVerify(           TExportDataInfo *info);
  bool DeinitSignVerify(         TExportDataInfo *info);
  bool DoDataSignVerify(             TExportDataInfo *info);

  void ExpDat_Init(TExportDataInfo *expdat, int type);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __EXPDAT_H__ */
