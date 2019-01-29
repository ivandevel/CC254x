@REM This batch file has been generated by the IAR Embedded Workbench
@REM C-SPY Debugger, as an aid to preparing a command line for running
@REM the cspybat command line utility using the appropriate settings.
@REM
@REM Note that this file is generated every time a new debug session
@REM is initialized, so you may want to move or rename the file before
@REM making changes.
@REM
@REM You can launch cspybat by typing the name of this batch file followed
@REM by the name of the debug file (usually an ELF/DWARF or UBROF file).
@REM
@REM Read about available command line parameters in the C-SPY Debugging
@REM Guide. Hints about additional command line parameters that may be
@REM useful in specific cases:
@REM   --download_only   Downloads a code image without starting a debug
@REM                     session afterwards.
@REM   --silent          Omits the sign-on message.
@REM   --timeout         Limits the maximum allowed execution time.
@REM 


@echo off 

if not "%~1" == "" goto debugFile 

@echo on 

"D:\Program Files\Programming Solution\IAR 8051\common\bin\cspybat" -f "D:\Documents\Sync\Dropbox\蓝牙\代码_蓝牙主从一体_1.4.2\BLE\BLE-CC254x-1.4.2-Serial\Projects\ble\SimpleBLECP\CC2540DB\settings\SimpleBLEPeripheral.CC2540.general.xcl" --backend -f "D:\Documents\Sync\Dropbox\蓝牙\代码_蓝牙主从一体_1.4.2\BLE\BLE-CC254x-1.4.2-Serial\Projects\ble\SimpleBLECP\CC2540DB\settings\SimpleBLEPeripheral.CC2540.driver.xcl" 

@echo off 
goto end 

:debugFile 

@echo on 

"D:\Program Files\Programming Solution\IAR 8051\common\bin\cspybat" -f "D:\Documents\Sync\Dropbox\蓝牙\代码_蓝牙主从一体_1.4.2\BLE\BLE-CC254x-1.4.2-Serial\Projects\ble\SimpleBLECP\CC2540DB\settings\SimpleBLEPeripheral.CC2540.general.xcl" "--debug_file=%~1" --backend -f "D:\Documents\Sync\Dropbox\蓝牙\代码_蓝牙主从一体_1.4.2\BLE\BLE-CC254x-1.4.2-Serial\Projects\ble\SimpleBLECP\CC2540DB\settings\SimpleBLEPeripheral.CC2540.driver.xcl" 

@echo off 
:end