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

REM Create directories
mkdir %PthBAT%\EN\format_dll
mkdir %PthBAT%\EN\tool_dll

REM Copy
copy /y %Pth1%\CLCLPlugin\tool_find\EN_Release\tool_find.dll %PthBAT%\EN\tool_dll\tool_find.dll
copy /y %Pth1%\CLCLPlugin\tool_find\tool_find_en.txt %PthBAT%\EN\tool_dll\tool_find_en.txt

copy /y %Pth1%\CLCLPlugin\tool_history\EN_Release\tool_history.dll %PthBAT%\EN\tool_dll\tool_history.dll
copy /y %Pth1%\CLCLPlugin\tool_history\tool_history_en.txt %PthBAT%\EN\tool_dll\tool_history_en.txt

copy /y %Pth1%\CLCLPlugin\tool_text\EN_Release\tool_text.dll %PthBAT%\EN\tool_dll\tool_text.dll
copy /y %Pth1%\CLCLPlugin\tool_text\tool_text_en.txt %PthBAT%\EN\tool_dll\tool_text_en.txt

copy /y %Pth1%\CLCLPlugin\tool_utl\EN_Release\tool_utl.dll %PthBAT%\EN\tool_dll\tool_utl.dll
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_en.txt %PthBAT%\EN\tool_dll\tool_utl_en.txt
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_sound1.wav %PthBAT%\EN\tool_dll\tool_utl_sound1.wav
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_sound2.wav %PthBAT%\EN\tool_dll\tool_utl_sound2.wav
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_sound3.wav %PthBAT%\EN\tool_dll\tool_utl_sound3.wav

copy /y %Pth1%\CLCLPlugin\tool_bitmap\EN_Release\tool_bitmap.dll %PthBAT%\EN\tool_dll\tool_bitmap.dll
copy /y %Pth1%\CLCLPlugin\tool_bitmap\tool_bitmap_en.txt %PthBAT%\EN\tool_dll\tool_bitmap_en.txt

copy /y %Pth1%\CLCLPlugin\fmt_rtf\EN_Release\fmt_rtf.dll %PthBAT%\EN\format_dll\fmt_rtf.dll
copy /y %Pth1%\CLCLPlugin\fmt_rtf\fmt_rtf_en.txt %PthBAT%\EN\format_dll\fmt_rtf_en.txt

copy /y %Pth1%\CLCLPlugin\fmt_metafile\EN_Release\fmt_metafile.dll %PthBAT%\EN\format_dll\fmt_metafile.dll
copy /y %Pth1%\CLCLPlugin\fmt_metafile\fmt_metafile_en.txt %PthBAT%\EN\format_dll\fmt_metafile_en.txt

REM List of files to archive
Set PF1=%Pth1%\EN_Release\CLCL.exe
Set PF2=%Pth1%\Release\CLCLHook.dll
Set PF3=%Pth1%\CLCLSet\EN_Release\CLCLSet.exe
Set PF4=%Pth1%\clcl_app.ini
Set PF5=%Pth1%\readme_en.txt
Set PF6=%PthBAT%\EN\format_dll
Set PF7=%PthBAT%\EN\tool_dll
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5% %PF6% %PF7%

Set zipF=%PthBAT%\EN\clcl_all_plugins_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
call cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%

REM Removing previously created directories
RD /S /Q %PthBAT%\EN\format_dll
RD /S /Q %PthBAT%\EN\tool_dll


REM ===================== CLCL all plugins JP ====================================== 
REM get file version
Set "PFileVer=%Pth1%\JP_Release\CLCL.exe"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_JP

REM Create directories
mkdir %PthBAT%\JP\format_dll
mkdir %PthBAT%\JP\tool_dll

REM Copy
copy /y %Pth1%\CLCLPlugin\tool_find\JP_Release\tool_find.dll %PthBAT%\JP\tool_dll\tool_find.dll
copy /y %Pth1%\CLCLPlugin\tool_find\tool_find_jp.txt %PthBAT%\JP\tool_dll\tool_find_jp.txt

copy /y %Pth1%\CLCLPlugin\tool_history\JP_Release\tool_history.dll %PthBAT%\JP\tool_dll\tool_history.dll
copy /y %Pth1%\CLCLPlugin\tool_history\tool_history_jp.txt %PthBAT%\JP\tool_dll\tool_history_jp.txt

copy /y %Pth1%\CLCLPlugin\tool_text\JP_Release\tool_text.dll %PthBAT%\JP\tool_dll\tool_text.dll
copy /y %Pth1%\CLCLPlugin\tool_text\tool_text_jp.txt %PthBAT%\JP\tool_dll\tool_text_jp.txt

copy /y %Pth1%\CLCLPlugin\tool_utl\JP_Release\tool_utl.dll %PthBAT%\JP\tool_dll\tool_utl.dll
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_jp.txt %PthBAT%\JP\tool_dll\tool_utl_jp.txt
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_sound1.wav %PthBAT%\JP\tool_dll\tool_utl_sound1.wav
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_sound2.wav %PthBAT%\JP\tool_dll\tool_utl_sound2.wav
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_sound3.wav %PthBAT%\JP\tool_dll\tool_utl_sound3.wav

copy /y %Pth1%\CLCLPlugin\tool_bitmap\JP_Release\tool_bitmap.dll %PthBAT%\JP\tool_dll\tool_bitmap.dll
copy /y %Pth1%\CLCLPlugin\tool_bitmap\tool_bitmap_jp.txt %PthBAT%\JP\tool_dll\tool_bitmap_jp.txt

copy /y %Pth1%\CLCLPlugin\fmt_rtf\JP_Release\fmt_rtf.dll %PthBAT%\JP\format_dll\fmt_rtf.dll
copy /y %Pth1%\CLCLPlugin\fmt_rtf\fmt_rtf_jp.txt %PthBAT%\JP\format_dll\fmt_rtf_jp.txt

copy /y %Pth1%\CLCLPlugin\fmt_metafile\JP_Release\fmt_metafile.dll %PthBAT%\JP\format_dll\fmt_metafile.dll
copy /y %Pth1%\CLCLPlugin\fmt_metafile\fmt_metafile_jp.txt %PthBAT%\JP\format_dll\fmt_metafile_jp.txt

REM List of files to archive
Set PF1=%Pth1%\JP_Release\CLCL.exe
Set PF2=%Pth1%\Release\CLCLHook.dll
Set PF3=%Pth1%\CLCLSet\JP_Release\CLCLSet.exe
Set PF4=%Pth1%\clcl_app.ini
Set PF5=%Pth1%\readme_jp.txt
Set PF6=%PthBAT%\JP\format_dll
Set PF7=%PthBAT%\JP\tool_dll
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5% %PF6% %PF7%

Set zipF=%PthBAT%\JP\clcl_all_plugins_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
call cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%

REM Removing previously created directories
RD /S /Q %PthBAT%\JP\format_dll
RD /S /Q %PthBAT%\JP\tool_dll



REM ===================== CLCL all plugins RU ====================================== 
REM get file version
Set "PFileVer=%Pth1%\RU_Release\CLCL.exe"
FOR /F "tokens=1,2 delims== " %%i IN ('"WMIC DATAFILE WHERE name="%PFileVer:\=\\%" get Version /format:Textvaluelist"') DO If /I "%%i"=="Version" Set "Version=%%j"
Set Vers=Ver_%Version:.=_%_RU

REM Create directories
mkdir %PthBAT%\RU\format_dll 
mkdir %PthBAT%\RU\tool_dll

REM Copy
copy /y %Pth1%\CLCLPlugin\tool_find\RU_Release\tool_find.dll %PthBAT%\RU\tool_dll\tool_find.dll
copy /y %Pth1%\CLCLPlugin\tool_find\tool_find_ru.txt %PthBAT%\RU\tool_dll\tool_find_ru.txt

copy /y %Pth1%\CLCLPlugin\tool_history\RU_Release\tool_history.dll %PthBAT%\RU\tool_dll\tool_history.dll
copy /y %Pth1%\CLCLPlugin\tool_history\tool_history_ru.txt %PthBAT%\RU\tool_dll\tool_history_ru.txt

copy /y %Pth1%\CLCLPlugin\tool_text\RU_Release\tool_text.dll %PthBAT%\RU\tool_dll\tool_text.dll
copy /y %Pth1%\CLCLPlugin\tool_text\tool_text_ru.txt %PthBAT%\RU\tool_dll\tool_text_ru.txt

copy /y %Pth1%\CLCLPlugin\tool_utl\RU_Release\tool_utl.dll %PthBAT%\RU\tool_dll\tool_utl.dll
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_ru.txt %PthBAT%\RU\tool_dll\tool_utl_ru.txt
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_sound1.wav %PthBAT%\RU\tool_dll\tool_utl_sound1.wav
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_sound2.wav %PthBAT%\RU\tool_dll\tool_utl_sound2.wav
copy /y %Pth1%\CLCLPlugin\tool_utl\tool_utl_sound3.wav %PthBAT%\RU\tool_dll\tool_utl_sound3.wav

copy /y %Pth1%\CLCLPlugin\tool_bitmap\RU_Release\tool_bitmap.dll %PthBAT%\RU\tool_dll\tool_bitmap.dll
copy /y %Pth1%\CLCLPlugin\tool_bitmap\tool_bitmap_ru.txt %PthBAT%\RU\tool_dll\tool_bitmap_ru.txt

copy /y %Pth1%\CLCLPlugin\fmt_rtf\RU_Release\fmt_rtf.dll %PthBAT%\RU\format_dll\fmt_rtf.dll
copy /y %Pth1%\CLCLPlugin\fmt_rtf\fmt_rtf_ru.txt %PthBAT%\RU\format_dll\fmt_rtf_ru.txt

copy /y %Pth1%\CLCLPlugin\fmt_metafile\RU_Release\fmt_metafile.dll %PthBAT%\RU\format_dll\fmt_metafile.dll
copy /y %Pth1%\CLCLPlugin\fmt_metafile\fmt_metafile_ru.txt %PthBAT%\RU\format_dll\fmt_metafile_ru.txt

REM List of files to archive
Set PF1=%Pth1%\RU_Release\CLCL.exe
Set PF2=%Pth1%\Release\CLCLHook.dll
Set PF3=%Pth1%\CLCLSet\RU_Release\CLCLSet.exe
Set PF4=%Pth1%\clcl_app.ini
Set PF5=%Pth1%\readme_ru.txt
Set PF6=%PthBAT%\RU\format_dll
Set PF7=%PthBAT%\RU\tool_dll
Set pFiles=%PF1% %PF2% %PF3% %PF4% %PF5% %PF6% %PF7%

Set zipF=%PthBAT%\RU\clcl_all_plugins_%Vers%.zip

REM Delete such archive if it already exists
if exist %zipF% DEL /F /Q %zipF%

REM Create an archive with the necessary files
call cmd /c ""%PthBAT%\7-Zip\App\32\7z.exe"" -y -tzip -ssw -mx9 a %zipF% %pFiles%

REM Removing previously created directories
RD /S /Q %PthBAT%\RU\format_dll
RD /S /Q %PthBAT%\RU\tool_dll

REM return to directory with BAT
CD /D %PthBAT%




