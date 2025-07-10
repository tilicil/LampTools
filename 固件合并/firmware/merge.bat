@echo off

set "AppName=app"
set "AppVersion=V1.0"
set "BootName=D21_BOOT"
set "BootVersion=V01.00.08_0x00"

for %%i in ("%~dp0.") do set "SCRIPT_DIR=%%~dpi"  

set "TARGET_PATH=%SCRIPT_DIR%firmware"  
set "SRC_PATH=%~dp0..\Debug\Exe"
set "App_Hex_File=%SRC_PATH%\%AppName%.hex"

echo %TARGET_PATH%
echo %App_Hex_File%

@REM set "pyPath=%TARGET_PATH%\B1_BootAppMerge.py"
set "HexViewPath=%TARGET_PATH%\HexView\hexview.exe"

set "Boot_Bin_File=%TARGET_PATH%\%BootName%.bin"
set "Boot_Hex_File=%TARGET_PATH%\%BootName%.hex"

set "AppInfo_Hex_File=%TARGET_PATH%\appinfo\appInfo.hex"
set "AppInfo_Bin_File=%TARGET_PATH%\appinfo\appInfo.bin"

set "Boot_Backup_Hex_File=%TARGET_PATH%\%BootName%_backup.hex"

set "UDS_Hex_File=%TARGET_PATH%\%AppName%_UDS.hex"

set "Merge_temp_File=%TARGET_PATH%\%AppName%_mergeBoot_Temp.hex"
set "APP_Hex_Temp_File=%TARGET_PATH%\%AppName%_Temp.hex"

set "Merge_Hex_File=%TARGET_PATH%\%AppName%_%AppVersion%+%BootName%_%BootVersion%.hex"

@REM set "UDS_S19_File=%TARGET_PATH%\%AppName%_UDS.srec"
@REM set "Merge_Hex_File=%TARGET_PATH%\%AppName%_%AppVersion%+%BootName%_%BootVersion%.srec"
@REM set "Driver_S19_File=%TARGET_PATH%\D21_FlashDrive_V01.00.00_0x20000010.srec"
@REM set "UDS_S19_Driver_File=%TARGET_PATH%\%AppName%_mergeDriver.srec"
@REM set "UDS_Hex_Driver_File=%TARGET_PATH%\%AppName%_mergeDriver.hex"

echo post_build_Step_0: del old file.
@REM del %UDS_S19_File%
del %UDS_Hex_File%

@REM del %Merge_S19_File%
del %Merge_Hex_File%

@REM del %UDS_S19_Driver_File%
del %UDS_Hex_Driver_File%

@REM  boot填入app后生成UDS文件
echo post_build_Step_1: Generate app UDS file.

@REM %HexViewPath% /S %Boot_Bin_File% /XI:16 -o %Boot_Hex_File%
%HexViewPath% /S %Boot_Hex_File% /XN -o %Boot_Bin_File%

@REM boot放入0x70000后面
%HexViewPath% /s /MO:%Boot_Bin_File%;0x70000 /XI:16 -o %Boot_Backup_Hex_File%

@REM app和偏移后的boot合并
@REM %HexViewPath% /s /MT:%App_Hex_File%+%Boot_Backup_Hex_File% /XS:16 -o %UDS_S19_File%
%HexViewPath% /s /MT:%App_Hex_File%+%Boot_Backup_Hex_File% /XI:16 -o %UDS_Hex_File%
del %Boot_Backup_Hex_File%

@REM 生成appinfo文件
echo post_build_Step_2: Generate appinfo file.

@REM 清空目标区域
%HexViewPath% %AppInfo_Hex_File% /s /CR:0xE000-0xFFFF /XI:16 -o %AppInfo_Hex_File%  

@REM 写入指纹秘钥
%HexViewPath% %AppInfo_Hex_File% /s /FR:0xE000-0xE00F /FP:0x0100000000000000000000005245474E /XI:16 -o %AppInfo_Hex_File%
%HexViewPath% %AppInfo_Hex_File% /s /FR:0xE010-0xE015 /FP:0x494634383146 /XI:16 -o %AppInfo_Hex_File%

@REM 写入编程信息，填空
%HexViewPath% %AppInfo_Hex_File% /s /FR:0xE016-0xE01F /FP:0x00 /XI:16 -o %AppInfo_Hex_File%

@REM 计算app校验，校验值填入0xE020
%HexViewPath% %App_Hex_File% /s /CSR9:@0xE020;[0x10000-0x6ffff] /XI:16 -o %APP_Hex_Temp_File%

@REM 删除app部分，保留校验值
%HexViewPath% %APP_Hex_Temp_File% /s /CR:0x10000-0x6ffff /XI:16 -o %APP_Hex_Temp_File%

@REM 合并之前appinfo与生成的校验值
%HexViewPath% /s /MT:%APP_Hex_Temp_File%+%AppInfo_Hex_File% /XI:16 -o %AppInfo_Hex_File%

@REM 继续写入编程信息，填空
%HexViewPath% %AppInfo_Hex_File% /s /FR:0xE024-0xE025 /FP:0x0100 /XI:16 -o %AppInfo_Hex_File%
%HexViewPath% %AppInfo_Hex_File% /s /FR:0xE025-0xE037 /FP:0x00 /XI:16 -o %AppInfo_Hex_File%

@REM 计算前面全部校验值，校验值填入0xE038
%HexViewPath% %AppInfo_Hex_File% /s /CSR9:@0xE038;[0xE000-0xE037] /XI:16 -o %AppInfo_Hex_File%

@REM 未使用到部分填充为FF
%HexViewPath% %AppInfo_Hex_File% /s /FR:0xE000-0xFFFF /FP:0xFF /XI:16 -o %AppInfo_Hex_File%
del %APP_Hex_Temp_File%

@REM 生成合并文件
echo post_build_Step_3: Generate merge file.

@REM 合并生成的uds升级文件，boot，appinfo三个文件
%HexViewPath% /s /MT:%UDS_Hex_File%+%Boot_Hex_File%+%AppInfo_Hex_File% /XI:16 -o %Merge_temp_File%

@REM 未使用到部分填充为FF
%HexViewPath% %Merge_temp_File% /s /FR:0x00-0x7FFFF /FP:0xFF /XI:16 -o %Merge_Hex_File%
del %Merge_temp_File%

@REM echo post_build_Step_3: Generate mergeDriver file.
@REM %HexViewPath% /s /MT:%Driver_S19_File%+%UDS_S19_File% /XS:16 -o %UDS_S19_Driver_File%

pause
