@Echo Off
Set PthBAT=%~dp0
CD /D %PthBAT% 
CD /D ..\
Set Pth1=%CD%
CD /D %PthBAT%



REM ===================== plugin fmt_metafile EN ========================== 
REM get file version
Set "PFileVer=%Pth1%\CLCLPlugin\fmt_metafile\EN_Release\fmt_metafile.dll"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_EN

REM List of files to archive
Set PF1=%Pth1%\CLCLPlugin\fmt_metafile\EN_Release\fmt_metafile.dll
Set PF2=%Pth1%\CLCLPlugin\fmt_metafile\fmt_metafile_en.txt
Set pFiles=%PF1% %PF2%

Set zipF=%PthBAT%\EN\fmt_metafile_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM ===================== plugin fmt_metafile JP ========================== 
REM get file version
Set "PFileVer=%Pth1%\CLCLPlugin\fmt_metafile\JP_Release\fmt_metafile.dll"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_JP

REM List of files to archive
Set PF1=%Pth1%\CLCLPlugin\fmt_metafile\JP_Release\fmt_metafile.dll
Set PF2=%Pth1%\CLCLPlugin\fmt_metafile\fmt_metafile_jp.txt
Set pFiles=%PF1% %PF2%

Set zipF=%PthBAT%\JP\fmt_metafile_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM ===================== plugin fmt_metafile RU ========================== 
REM get file version
Set "PFileVer=%Pth1%\CLCLPlugin\fmt_metafile\RU_Release\fmt_metafile.dll"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_RU

REM List of files to archive
Set PF1=%Pth1%\CLCLPlugin\fmt_metafile\RU_Release\fmt_metafile.dll
Set PF2=%Pth1%\CLCLPlugin\fmt_metafile\fmt_metafile_ru.txt
Set pFiles=%PF1% %PF2%

Set zipF=%PthBAT%\RU\fmt_metafile_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM return to directory with BAT
CD /D %PthBAT%




