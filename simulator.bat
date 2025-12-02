@echo off
REM === CPU SCHEDULER SIMULATOR BUILD & RUN SCRIPT (BAT)
echo.
echo Compiling the CPU Scheduler Simulator...

REM 
REM 
REM 
if exist simulator.exe (
    del simulator.exe
    if exist simulator.exe (
        echo ERROR: Previous simulator.exe is locked. Please close the program in Task Manager.
        echo Press any key to continue . . .
        pause
        exit /b 1
    )
)

REM 
REM 
g++ main.cpp sjf_non_preemptive.cpp fcfscomplete.cpp scheduler_common.cpp priority_non_preemptive.cpp srtf_preemptive.cpp round_robin.cpp comparison_tool.cpp splash_screen.cpp -o simulator.exe -std=c++11 -finput-charset=UTF-8

REM
if %errorlevel% equ 0 (
    
    REM
    REM 
    chcp 65001 > nul 
    
    echo.
    echo Compilation successful!
    
    REM
    echo Running simulator.exe...
    simulator.exe
) else (
    echo.
    echo ERROR: Compilation failed. Please check the source code for errors.
)

pause
