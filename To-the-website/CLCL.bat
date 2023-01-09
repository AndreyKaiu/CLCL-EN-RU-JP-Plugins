@Echo Off
Set PthBAT=%~dp0
CD /D %PthBAT% 
CD /D ..\
Set Pth1=%CD%
CD /D %PthBAT%

REM ===================== CLCL EN ====================================== 
REM get file version
Set "PFileVer=%Pth1%\EN_Release\CLCL.exe"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_EN

REM List of files to archive
Set PF1=%Pth1%\EN_Release\CLCL.exe
Set PF2=%Pth1%\Release\CLCLHook.dll
Set PF3=%Pth1%\CLCLSet\EN_Release\CLCLSet.exe
Set PF4=%Pth1%\clcl_app.ini
Set PF5=%Pth1%\readme_en.txt
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5%

Set zipF=%PthBAT%\EN\clcl_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM ===================== CLCL JP ====================================== 
REM get file version
Set "PFileVer=%Pth1%\JP_Release\CLCL.exe"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_JP

REM List of files to archive
Set PF1=%Pth1%\JP_Release\CLCL.exe
Set PF2=%Pth1%\Release\CLCLHook.dll
Set PF3=%Pth1%\CLCLSet\JP_Release\CLCLSet.exe
Set PF4=%Pth1%\clcl_app.ini
Set PF5=%Pth1%\readme_jp.txt
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5%

Set zipF=%PthBAT%\JP\clcl_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM ===================== CLCL RU ====================================== 
REM get file version
Set "PFileVer=%Pth1%\RU_Release\CLCL.exe"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_RU

REM List of files to archive
Set PF1=%Pth1%\RU_Release\CLCL.exe
Set PF2=%Pth1%\Release\CLCLHook.dll
Set PF3=%Pth1%\CLCLSet\RU_Release\CLCLSet.exe
Set PF4=%Pth1%\clcl_app.ini
Set PF5=%Pth1%\readme_ru.txt
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5%

Set zipF=%PthBAT%\RU\clcl_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM return to directory with BAT
CD /D %PthBAT%




