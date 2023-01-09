@Echo Off
Set PthBAT=%~dp0
CD /D %PthBAT% 
CD /D ..\
Set Pth1=%CD%
CD /D %PthBAT%



REM ===================== plugin tool_text EN ========================== 
REM get file version
Set "PFileVer=%Pth1%\CLCLPlugin\tool_text\EN_Release\tool_text.dll"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_EN

REM List of files to archive
Set PF1=%Pth1%\CLCLPlugin\tool_text\EN_Release\tool_text.dll
Set PF2=%Pth1%\CLCLPlugin\tool_text\tool_text_en.txt
Set pFiles=%PF1% %PF2%

Set zipF=%PthBAT%\EN\tool_text_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM ===================== plugin tool_text JP ========================== 
REM get file version
Set "PFileVer=%Pth1%\CLCLPlugin\tool_text\JP_Release\tool_text.dll"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_JP

REM List of files to archive
Set PF1=%Pth1%\CLCLPlugin\tool_text\JP_Release\tool_text.dll
Set PF2=%Pth1%\CLCLPlugin\tool_text\tool_text_jp.txt
Set pFiles=%PF1% %PF2%

Set zipF=%PthBAT%\JP\tool_text_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM ===================== plugin tool_text RU ========================== 
REM get file version
Set "PFileVer=%Pth1%\CLCLPlugin\tool_text\RU_Release\tool_text.dll"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_RU

REM List of files to archive
Set PF1=%Pth1%\CLCLPlugin\tool_text\RU_Release\tool_text.dll
Set PF2=%Pth1%\CLCLPlugin\tool_text\tool_text_ru.txt
Set pFiles=%PF1% %PF2%

Set zipF=%PthBAT%\RU\tool_text_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM return to directory with BAT
CD /D %PthBAT%




