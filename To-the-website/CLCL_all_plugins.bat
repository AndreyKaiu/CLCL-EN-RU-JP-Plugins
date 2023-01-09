@Echo Off
Set PthBAT=%~dp0
CD /D %PthBAT% 
CD /D ..\
Set Pth1=%CD%
CD /D %PthBAT%

REM ===================== CLCL all plugins EN ====================================== 
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
Set PF6=%Pth1%\CLCLPlugin\tool_find\EN_Release\tool_find.dll
Set PF7=%Pth1%\CLCLPlugin\tool_find\tool_find_en.txt
Set PF8=%Pth1%\CLCLPlugin\tool_history\EN_Release\tool_history.dll
Set PF9=%Pth1%\CLCLPlugin\tool_history\tool_history_en.txt
Set PF10=%Pth1%\CLCLPlugin\tool_text\EN_Release\tool_text.dll
Set PF11=%Pth1%\CLCLPlugin\tool_text\tool_text_en.txt
Set PF12=%Pth1%\CLCLPlugin\tool_utl\EN_Release\tool_utl.dll
Set PF13=%Pth1%\CLCLPlugin\tool_utl\tool_utl_en.txt
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5% %PF6% %PF7% %PF8% %PF9% %PF10% %PF11% %PF12% %PF13%

Set zipF=%PthBAT%\EN\clcl_all_plugins_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM ===================== CLCL all plugins JP ====================================== 
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
Set PF6=%Pth1%\CLCLPlugin\tool_find\JP_Release\tool_find.dll
Set PF7=%Pth1%\CLCLPlugin\tool_find\tool_find_jp.txt
Set PF8=%Pth1%\CLCLPlugin\tool_history\JP_Release\tool_history.dll
Set PF9=%Pth1%\CLCLPlugin\tool_history\tool_history_jp.txt
Set PF10=%Pth1%\CLCLPlugin\tool_text\JP_Release\tool_text.dll
Set PF11=%Pth1%\CLCLPlugin\tool_text\tool_text_jp.txt
Set PF12=%Pth1%\CLCLPlugin\tool_utl\JP_Release\tool_utl.dll
Set PF13=%Pth1%\CLCLPlugin\tool_utl\tool_utl_jp.txt
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5% %PF6% %PF7% %PF8% %PF9% %PF10% %PF11% %PF12% %PF13%

Set zipF=%PthBAT%\JP\clcl_all_plugins_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM ===================== CLCL all plugins RU ====================================== 
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
Set PF6=%Pth1%\CLCLPlugin\tool_find\RU_Release\tool_find.dll
Set PF7=%Pth1%\CLCLPlugin\tool_find\tool_find_ru.txt
Set PF8=%Pth1%\CLCLPlugin\tool_history\RU_Release\tool_history.dll
Set PF9=%Pth1%\CLCLPlugin\tool_history\tool_history_ru.txt
Set PF10=%Pth1%\CLCLPlugin\tool_text\RU_Release\tool_text.dll
Set PF11=%Pth1%\CLCLPlugin\tool_text\tool_text_ru.txt
Set PF12=%Pth1%\CLCLPlugin\tool_utl\RU_Release\tool_utl.dll
Set PF13=%Pth1%\CLCLPlugin\tool_utl\tool_utl_ru.txt
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5% %PF6% %PF7% %PF8% %PF9% %PF10% %PF11% %PF12% %PF13%

Set zipF=%PthBAT%\RU\clcl_all_plugins_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
start cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%



REM return to directory with BAT
CD /D %PthBAT%




