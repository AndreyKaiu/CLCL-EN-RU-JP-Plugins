@Echo Off
Set PthBAT=%~dp0
CD /D %PthBAT% 
CD /D ..\
Set Pth1=%CD%
CD /D %PthBAT%



REM ===================== plugin tool_utl EN ========================== 
REM get file version
Set "PFileVer=%Pth1%\CLCLPlugin\tool_utl\EN_Release\tool_utl.dll"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_EN

REM List of files to archive
Set PF1=%Pth1%\CLCLPlugin\tool_utl\EN_Release\tool_utl.dll
Set PF2=%Pth1%\CLCLPlugin\tool_utl\tool_utl_en.txt
Set PF3=%Pth1%\CLCLPlugin\tool_utl\tool_utl_sound1.wav
Set PF4=%Pth1%\CLCLPlugin\tool_utl\tool_utl_sound2.wav
Set PF5=%Pth1%\CLCLPlugin\tool_utl\tool_utl_sound3.wav
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5%

Set zipF=%PthBAT%\EN\tool_utl_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM ===================== plugin tool_utl JP ========================== 
REM get file version
Set "PFileVer=%Pth1%\CLCLPlugin\tool_utl\JP_Release\tool_utl.dll"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_JP

REM List of files to archive
Set PF1=%Pth1%\CLCLPlugin\tool_utl\JP_Release\tool_utl.dll
Set PF2=%Pth1%\CLCLPlugin\tool_utl\tool_utl_jp.txt
Set PF3=%Pth1%\CLCLPlugin\tool_utl\tool_utl_sound1.wav
Set PF4=%Pth1%\CLCLPlugin\tool_utl\tool_utl_sound2.wav
Set PF5=%Pth1%\CLCLPlugin\tool_utl\tool_utl_sound3.wav
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5%

Set zipF=%PthBAT%\JP\tool_utl_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM ===================== plugin tool_utl RU ========================== 
REM get file version
Set "PFileVer=%Pth1%\CLCLPlugin\tool_utl\RU_Release\tool_utl.dll"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_RU

REM List of files to archive
Set PF1=%Pth1%\CLCLPlugin\tool_utl\RU_Release\tool_utl.dll
Set PF2=%Pth1%\CLCLPlugin\tool_utl\tool_utl_ru.txt
Set PF3=%Pth1%\CLCLPlugin\tool_utl\tool_utl_sound1.wav
Set PF4=%Pth1%\CLCLPlugin\tool_utl\tool_utl_sound2.wav
Set PF5=%Pth1%\CLCLPlugin\tool_utl\tool_utl_sound3.wav
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5%

Set zipF=%PthBAT%\RU\tool_utl_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM return to directory with BAT
CD /D %PthBAT%




