======== version 2.1.3.5 \ バージョン 2.1.3.5 \ версия 2.1.3.5 ==========
====================================================================
============= WHAT IS THIS \ これは何ですか \ ЧТО ЭТО ====================
====================================================================
=============================== EN =================================
CLCL is a program that keeps clipboard history.
For more details, see the file "readme_en.txt".
The release for your language can be downloaded here:
https://github.com/AndreyKaiu/CLCL-EN-RU-JP-Plugins/tree/main/To-the-website/EN

=============================== JP =================================
CLCL(クルクル)は、クリップボードの履歴を取るソフトです。
詳細については、ファイル「readme_jp.txt」を参照してください。
お使いの言語のリリースは、次の場所からダウンロードできます。
https://github.com/AndreyKaiu/CLCL-EN-RU-JP-Plugins/tree/main/To-the-website/JP

=============================== RU =================================
CLCL — это программа, которая ведет историю буфера обмена.
Подробнее смотрите в файле «readme_ru.txt».
Релиз для вашего языка можно скачать тут:
https://github.com/AndreyKaiu/CLCL-EN-RU-JP-Plugins/tree/main/To-the-website/RU
====================================================================



====================================================================
==== INFORMATION FOR DEVELOPERS \ 開発者向け情報 \ ДЛЯ РАЗРАБОТЧИКОВ ====
====================================================================
=============================== EN =================================
Project created in "Microsoft Visual Studio Community 2022"
Run the file with "vcxproj" extension. Use batch assembly to build the program for different languages.
Change the version of the file in the resources (where the version is "*.rc" files), description files "*.txt", in the file "General.h"
Catalog "CLCLSet" - CLCL setup program project
Directory "CLCLHook" - DLL project for CLCL
"CLCLPlugin" directory - contains an example of the "tool_test" plugin, as well as other folders with plugins
"To-the-website" directory - contains copies of releases so they can be posted on the site and distributed

"Microsoft Visual Studio Community 2022" must have Windows XP, MSVC, MFC C++ Support Pack for version 141 installed

=============================== JP =================================
「Microsoft Visual Studio Community 2022」で作成されたプロジェクト
拡張子が「vcxproj」のファイルを実行します。 バッチ アセンブリを使用して、さまざまな言語用のプログラムをビルドします。
リソース (バージョンは「*.rc」ファイル)、記述ファイル「*.txt」、ファイル「General.h」内のファイルのバージョンを変更します。
カタログ「CLCLSet」 - CLCLセットアッププログラムプロジェクト
ディレクトリ "CLCLHook" - CLCL の DLL プロジェクト
「CLCLPlugin」ディレクトリ - 「tool_test」プラグインの例と、プラグインを含む他のフォルダが含まれています
「To-the-website」ディレクトリ - リリースのコピーが含まれているため、サイトに投稿して配布することができます

「Microsoft Visual Studio Community 2022」には、バージョン 141 の Windows XP、MSVC、MFC C++ サポート パックがインストールされている必要があります。

=============================== RU =================================
Проект создан в "Microsoft Visual Studio Community 2022"
Запустите файл с расширением "vcxproj". Используйте пакетную сборку , чтобы собрать программу под разные языки.
Измените версию файла в ресурсах (где версия, файлы "*.rc"), файлах описаниях "*.txt",  в файле "General.h"
Каталог "CLCLSet" - проект программы настройки CLCL
Каталог "CLCLHook" - проект DLL для CLCL
Каталог "CLCLPlugin" - содержит пример плагина "tool_test", так и другие папки с плагинами
Каталог "To-the-website" - содержит копии релизов, чтобы их можно было размещать на сайте и распростанять

В "Microsoft Visual Studio Community 2022" должны быть установлены пакет поддержки Windows XP, MSVC, MFC C++ для версии 141 



====================================================================
======== IMPORTANT CHANGES \ 重要な変更 \ ВАЖНЫЕ ИЗМЕНЕНИЯ ============
====================================================================
=============================== EN =================================
- A menu with actions can be shown immediately with the active item specified as a number counted from above (if <0, then from below).
- The position of the selection in fields like ComboBoxes that allow you to edit data is remembered, but they reset the focus of the selection using Alt+C.
- Ability to remember the selection position in other fields (Options window, “Menu” tab).
- After dragging a word into some other program, you can set the right key press and the space key press, which makes it easier to enter words using this method (the other program becomes active forcibly, since for some reason Notepad++ does not do this). This function can be enabled in the options window, “Viewer” tab.
----------------------------------------------------------------------
- The options window is now always displayed on top of all windows, since CLCL can be on top of all windows.
- Fixed bug with CLCL status indication when full data was reloaded.
- Ctrl+Shift+Click allows you not to execute the history, but to open the viewer immediately at this point for editing (copying as by Shift is left).
- Fixed keyboard control in the menu by "Ctrl + Enter" (showing the tool menu or inserting and reopening the menu).
- Fixed bug with string saving in plugins "tool_text.dll" and "tool_find.dll" when the string contained spaces at the end.
----------------------------------------------------------------------
- The file from version 2.1.3.2 comes with the setting immediately for portability (clcl_app.ini contains [GENERAL] portable=1)
- File version is now written with 4 characters, example: 2.1.3.2
- In the menu of the program "Help" the item "View help" (F1) has been added, which will open the file "readme_en.txt" (or readme_XX.txt where XX is the designation of the program language).
- Hotkeys for some actions have been added to the program menu.
- Added toggle button "Watch clipboard" (F9) to the toolbar.
- Added toggle button "Above all windows" (F12) with custom icon on the toolbar. Now there is no need to use a plugin for this feature.
- In the "View" menu, it is possible to set the display of control characters (space, tab, line feed) in order to more accurately see the difference in the copied lines.
- Changed the icon of the program. With a yellow background if "Watch clipboard", otherwise a gray background with a red circle (the red circle is now set to the application icon). The icon for "CLCLSet.exe" is made in color, with the inscription "CLCL" (so far, the icons are everywhere as they were in 16 colors, so in theory there should be no problems on the most ancient monitors).
- Now the program reacts more correctly to program scaling in Windows versions less than 8.1. Icons have been changed so that a better visual effect can be realized.
- "Tools" has been added to the context menu on the clipboard item, since it is logical to find the function in it to clear the clipboard.
- Changed for the Russian version calling templates from the "Ш" key to a space (it's more convenient even than R or T). While one hand presses Alt + C, the other is already ready to press the spacebar and then you do not need to reach for R, but immediately to the desired number or character that will launch the menu item.
- If clipboard tracking is disabled, and tools require copy and paste functionality, then the context menu will say "off clcl!" in the names of these tools. An additional reminder to let you know that the tool won't work properly and requires clipboard tracking to be enabled.
- Clicking the right mouse button on a context menu item now performs either the function of showing a tool or pasting and re-showing the menu (without copying to history, for quick various pastings in a row). It is possible to select one of these two functions and by "Ctrl + right-click".
- If in the history parameters it is worth "delete when copying to the clipboard" and clipboard tracking is turned off, then only deletion from the history occurred and this did not allow duplicate entry of the same value (but you cannot copy it to the history). I had to change the algorithm so that if clipboard tracking is turned off, it does not delete when copying to the clipboard.
- The selected inactive item in the history tree is now shown in blue font color to make it easier to find with a glance, otherwise the faint shade of blue background on some computers was not always clearly visible.

- The options window is now always displayed in the center of the desktop. The last page on which “OK” was clicked is remembered, so that later, when called, it will be shown immediately.
- In the options window, you can move items by Alt+Up(Down), as it works for the main CLCL window.
- In the "History" parameters, the flag "When activated, do not add the current clipboard to the history" has been added. If you turned off "Monitor the clipboard" and copied some personal correspondence to the clipboard, then the subsequent inclusion of CLCL should not add your personal data to the History without your permission.
- In the options, if you have selected a path to a file that is in the CLCL folder, then the path will be shown relative, not absolute. Absolute paths make it difficult to transfer a portable program to another computer. In this regard, it is preferable to keep all your DLLs, icons or some other useful scripts in one place, that is, in the CLCL folder.
- Changes in the size of columns in tables are remembered, as it is more convenient for the user (the width is at least 10 pixels, but you can change #define MIN_COL_SIZE_LISTVIEW 10 in the program code).
- Possibility to set in the parameters to force switching of the menu language to ENG, so that all accelerators & format %A were active in any other input language.
- Changed default format for menu numbering from "&%1d. %t" to "&%1C. %t" (at the beginning of the number from 1 to 9 and then the letters of the English language).
- If the plugin has no parameters (the developer commented in the .def file), then the plugin properties button will not be active.
- There can be several identical plug-ins in the program, but the properties of the file are the same (common for each identical plug-in function), and for differences it is necessary to change the command line. The format of the command line is not always clear, and to simplify it, it is now possible to fill in the command line by the button of the usual properties (the plugin should be able to), and if it is empty, then this is considered as common properties. In order to immediately see which properties will work (from the command line or general ones), the “•” sign is added to the beginning of the name of the button with the property (the “?•” is added if there is a ? sign at the beginning of the command line, which can indicate that the user will be shown a dialog box to select options).
- Default settings: for actions, the left mouse button goes first, then the right one (since the reading order is familiar from left to right). Added to these actions the menu item "Watch clipboard". By default, the “show tools on the right button” flag (show_tool_menu) is disabled in the menu settings, since the tools may not be installed at all, and the new “insert and re-show menu” function is more convenient when without the Ctrl + combination. The default menu width is 250, as the previous 200 isn't quite enough.

- Fixed "alloc_copy_n" function error from "Memory.c" when not copying the last character with "lstrcpyn".
- Added relative path for sound file in tool_utl plugin. The selected file is played immediately. (added 3 identical files with different volume)
- The "ztop" and "unztop" functions have been removed from the tool_utl plugin, since CLCL already knows how to do this from version 2.1.3.2.
- In the tool_utl plugin, the code has been changed so that a call from the context menu returns the focus again to the window that was being worked on.
- In the tool_find plugin, the ability to set the flag for reopening the window has been added. The window can remember its position on the screen while working with the program (until the program is reopened).
- In the tool_find plugin, the ability to select previously entered strings that have already been searched has been added.
- In the tool_text plugin, in the text editing function, you can set the window font using the settings in the tool_text.ini file. The window can remember its height, width, and position on the screen while working with the program (until the program is reopened).
- In the tool_text plugin, in the quoting functions (quote, unquote), it is possible to select from the drop-down list the previously entered character (string) of the quotation. The window can remember its position on the screen while working with the program (until the program is reopened).
- In the tool_text plugin, in the function to remove line breaks (delete_crlf), you can set the command 1 - remove leading spaces, 0 - do not delete, ? - ask when calling. The window can remember its position on the screen while working with the program (until the program is reopened).
- In the tool_text plugin, in the text insertion function (put_text), it is possible to select previously entered lines from the drop-down list. The window can remember its position on the screen while working with the program (until the program is reopened).
- In the tool_text plugin, in the function of combining texts (join_text), you can set the command ? - ask when calling. You can choose to merge text in reverse order. The window can remember its position on the screen while working with the program (until the program is reopened).
- Added 3 plugins: "tool_bitmap", "fmt_metafile", "fmt_rtf".
- It was decided not to litter the main location of the program with files related to plugins and create a special folder for them "tool_dll" (for tools) and "format_dll" (for formats).
- Individual actions can now set the paste keys as the specified window (from the "Keys" tab). This is necessary so that some templates can be intercepted by third-party programs. See http://forum.ru-board.com/topic.cgi?forum=5&topic=50494&start=0 "CLCL_AHK - clipboard manager plus advanced text processing with Autohotkey and AutoIt"

? The problem with a slight slowdown of Alt + C in the NOTEPAD ++ program has not been resolved if the main menu is quickly accessed by pressing the C key (different interface languages have their own input accelerators). Disabling the Alt+C hotkey in the NOTEPAD++ program itself does not solve the problem. You can solve the problem by editing the translation of the program menu.

=============================== JP =================================
- アクションを含むメニューは、上から数えた番号 (<0 の場合は下から数えた数) として指定されたアクティブな項目とともにすぐに表示できます。
- データを編集できるコンボボックスなどのフィールド内の選択範囲の位置は記憶されますが、Alt+C を使用すると選択範囲のフォーカスがリセットされます。
- 他のフィールド（オプションウィンドウ、「メニュー」タブ）での選択位置を記憶する機能。
- 単語を他のプログラムにドラッグした後、右キーの押下とスペース キーの押下を設定できます。これにより、この方法を使用して単語を入力することが容易になります (何らかの理由で Notepad++ ではこれが行われないため、他のプログラムが強制的にアクティブになります) ）。 この機能は、オプションウィンドウの「ビューアー」タブで有効にできます。
----------------------------------------------------------------------
- CLCL はすべてのウィンドウの上に表示できるため、オプション ウィンドウは常にすべてのウィンドウの上に表示されるようになりました。
- 完全なデータがリロードされたときの CLCL ステータス表示のバグを修正しました。
- Ctrl+Shift+Click を使用すると、履歴を実行するのではなく、編集のためにこの時点でビューアをすぐに開くことができます (Shift によるコピーはそのままです)。
- 「Ctrl + Enter」によるメニューのキーボード制御を修正しました (ツール メニューを表示するか、メニューを挿入して再度開く)。
- 文字列の末尾にスペースが含まれている場合、プラグイン「tool_text.dll」および「tool_find.dll」で文字列を保存する際のバグを修正しました。
----------------------------------------------------------------------
- バージョン 2.1.3.2 のファイルには、移植性のための即時設定が付属しています (clcl_app.ini には [GENERAL] Portable=1 が含まれています)。
- ファイル バージョンが 4 文字で表記されるようになりました。例: 2.1.3.2
- 「ヘルプ」プログラムのメニューに、「ヘルプを表示」(F1) という項目が追加されました。これにより、「readme_en.txt」ファイル (または、XX はプログラム言語の指定である readme_XX.txt) が開きます。
- 一部のアクションのホットキーがプログラム メニューに追加されました。
・トグルボタン「クリップボードを見る」(F9)をツールバーに追加。
- ツールバーのカスタム アイコンにトグル ボタン「すべてのウィンドウの上」(F12) を追加しました。 現在、この機能にプラグインを使用する必要はありません。
・「表示」メニューでは、コピーした行の違いがより正確にわかるように、制御文字（スペース、タブ、改行）の表示設定が可能です。
- プログラムのアイコンを変更しました。 「Watch clipboard」の場合は黄色の背景、それ以外の場合は灰色の背景に赤い円 (赤い円はアプリケーション アイコンに設定されています)。 「CLCLSet.exe」のアイコンはカラーで作成され、「CLCL」と刻印されています（これまでのところ、アイコンは 16 色だったのでどこにでもあるので、理論上は最も古いモニターでも問題はないはずです）。
- Windows バージョン 8.1 未満でのプログラムのスケーリングに対して、プログラムがより正確に反応するようになりました。 より良い視覚効果を実現できるように、アイコンが変更されました。
- クリップボード アイテムのコンテキスト メニューに [ツール] が追加されました。これは、クリップボードをクリアする機能を見つけるのが論理的であるためです。
- ロシア語版の呼び出しテンプレートを「Ш」キーからスペースに変更しました (R や T よりも便利です)。 一方の手で Alt + C を押している間、もう一方の手はすでにスペースバーを押す準備ができているため、R に手を伸ばす必要はありませんが、メニュー項目を起動する目的の数字または文字にすぐに到達します。
- クリップボード トラッキングが無効で、ツールにコピー アンド ペースト機能が必要な場合、コンテキスト メニューのツール名に「オフ clcl!」と表示されます。 ツールが正しく動作せず、クリップボードの追跡を有効にする必要があることを知らせる追加のリマインダー。
- コンテキスト メニュー項目でマウスの右ボタンをクリックすると、ツールを表示する機能、またはメニューを貼り付けて再表示する機能のいずれかが実行されるようになりました (履歴にコピーせずに、連続してすばやくさまざまな貼り付けを行うことができます)。 この 2 つの機能のいずれかを選択して、「Ctrl + 右クリック」することができます。
- 履歴パラメーターで「クリップボードにコピーするときに削除する」価値があり、クリップボードの追跡がオフになっている場合、履歴からの削除のみが発生し、これにより同じ値の重複入力が許可されませんでした (ただし、それを歴史）。 クリップボードの追跡がオフになっている場合、クリップボードにコピーするときに削除されないように、アルゴリズムを変更する必要がありました。
- ヒストリー ツリーで選択された非アクティブなアイテムは、一目で見つけやすくするために青色のフォント カラーで表示されるようになりました。

- オプション ウィンドウが常にデスクトップの中央に表示されるようになりました。 「OK」がクリックされた最後のページが記憶されているため、後で呼び出されたときにすぐに表示されます。
- オプション ウィンドウでは、メインの CLCL ウィンドウで機能するように、Alt+Up(Down) でアイテムを移動できます。
- 「ヒストリー」パラメーターに、「アクティブ化された場合、現在のクリップボードをヒストリーに追加しない」というフラグが追加されました。 「クリップボードを監視する」をオフにして個人的なやり取りをクリップボードにコピーした場合、その後 CLCL を含めても、許可なく履歴に個人データが追加されることはありません。
- オプションで、CLCL フォルダにあるファイルへのパスを選択した場合、パスは絶対ではなく相対で表示されます。 絶対パスを使用すると、移植可能なプログラムを別のコンピューターに転送することが難しくなります。 この点で、すべての DLL、アイコン、またはその他の便利なスクリプトを 1 つの場所、つまり CLCL フォルダーに保存することをお勧めします。
- ユーザーにとってより便利なように、テーブル内の列のサイズの変更が記憶されます (幅は少なくとも 10 ピクセルですが、プログラム コードで #define MIN_COL_SIZE_LISTVIEW 10 を変更できます)。
- すべてのアクセラレータとフォーマット %A が他の入力言語でアクティブになるように、メニュー言語を強制的に ENG に切り替えるパラメータを設定する可能性。
- メニューの番号付けのデフォルト形式を "&%1d​​. %t" から "&%1C. %t" (1 から 9 までの数字と英語の文字の先頭)。
- プラグインにパラメーターがない場合 (開発者が .def ファイルでコメントした場合)、プラグイン プロパティ ボタンはアクティブになりません。
- プログラム内に同一のプラグインが複数存在する可能性がありますが、ファイルのプロパティは同じ (同一のプラグイン関数ごとに共通) であり、相違点についてはコマンド ラインを変更する必要があります。 コマンド ラインの形式は常に明確であるとは限りません。簡略化するために、通常のプロパティのボタンでコマンド ラインに入力できるようになりました (プラグインができるはずです)。は共通のプロパティと見なされます。 (コマンド ラインまたは一般的なものから) どのプロパティが機能するかをすぐに確認するために、プロパティを持つボタンの名前の先頭に「•」記号が追加されます (「?•」がある場合は追加されます)。 ? 記号をコマンド ラインの先頭に付けると、オプションを選択するためのダイアログ ボックスがユーザーに表示されることを示すことができます)。
- デフォルト設定: アクションでは、マウスの左ボタンが最初に移動し、次に右ボタンが移動します (読み上げ順序は左から右に慣れているため)。 これらのアクションに、メニュー項目「クリップボードを見る」が追加されました。 デフォルトでは、ツールがまったくインストールされない可能性があるため、メニュー設定で「右ボタンにツールを表示」フラグ（show_tool_menu）が無効になっています。 Ctrl + 組み合わせ。 以前の 200 では十分ではないため、デフォルトのメニュー幅は 250 です。

- 「lstrcpyn」で最後の文字をコピーしない場合の「Memory.c」からの「alloc_copy_n」関数エラーを修正しました。
- tool_utl プラグインにサウンド ファイルの相対パスを追加しました。 選択したファイルがすぐに再生されます。 （ボリュームの異なる3つの同一ファイルを追加）
- "ztop" および "unztop" 関数は tool_utl プラグインから削除されました。これは、CLCL がバージョン 2.1.3.2 からこれを行う方法を既に知っているためです。
- tool_utl プラグインで、コンテキスト メニューからの呼び出しで、作業中のウィンドウに再びフォーカスが戻るようにコードが変更されました。
- tool_find プラグインで、ウィンドウを再度開くためのフラグを設定する機能が追加されました。 ウィンドウは、プログラムを操作している間 (プログラムが再び開かれるまで)、画面上の位置を記憶できます。
- tool_find プラグインで、以前に入力した文字列を検索済みとして選択する機能が追加されました。
- tool_text プラグインでは、テキスト編集機能で、tool_text.ini ファイルの設定を使用してウィンドウのフォントを設定できます。 ウィンドウは、プログラムを操作している間 (プログラムが再び開かれるまで)、その高さ、幅、および画面上の位置を記憶できます。
- tool_text プラグインでは、引用機能(quote、unquote)で、先に入力した引用の文字(文字列)をドロップダウンリストから選択できるようになりました。 ウィンドウは、プログラムを操作している間 (プログラムが再び開かれるまで)、画面上の位置を記憶できます。
- tool_text プラグインでは、改行を削除する機能 (delete_crlf) で、コマンド 1 - 先頭のスペースを削除、0 - 削除しない、? を設定できます。 - 電話をかけるときに尋ねます。 ウィンドウは、プログラムを操作している間 (プログラムが再び開かれるまで)、画面上の位置を記憶できます。
- tool_text プラグインでは、テキスト挿入関数 (put_text) で、以前に入力した行をドロップダウン リストから選択することができます。 ウィンドウは、プログラムを操作している間 (プログラムが再び開かれるまで)、画面上の位置を記憶できます。
- tool_text プラグインでは、テキストを結合する機能 (join_text) で、コマンド ? を設定できます。 - 電話をかけるときに尋ねます。 テキストを逆順に結合することを選択できます。 ウィンドウは、プログラムを操作している間 (プログラムが再び開かれるまで)、画面上の位置を記憶できます。
- 3 つのプラグインを追加: "tool_bitmap"、"fmt_metafile"、"fmt_rtf"。
- プログラムのメインの場所にプラグインに関連するファイルを散らかさず、「tool_dll」(ツール用) と「format_dll」(フォーマット用) 用の特別なフォルダを作成しないことが決定されました。
- 個々のアクションで、指定したウィンドウとして貼り付けキーを設定できるようになりました ([キー] タブから)。 これは、一部のテンプレートがサードパーティ プログラムによって傍受されるために必要です。 http://forum.ru-board.com/topic.cgi?forum=5&topic=50494&start=0 「CLCL_AHK - クリップボード マネージャーと Autohotkey および AutoIt による高度なテキスト処理」を参照してください。

? Cキーを押してメインメニューにすばやくアクセスした場合、NOTEPAD ++プログラムでAlt + Cのわずかな速度低下の問題は解決されていません（さまざまなインターフェイス言語には独自の入力アクセラレータがあります）。 NOTEPAD++ プログラム自体で Alt+C ホットキーを無効にしても、問題は解決しません。 プログラムメニューの翻訳を編集することで問題を解決できます。

=============================== RU =================================
- Меню с действиями может быть показано сразу с активным пунктом задаваемым как номер отсчитываемый сверху (если <0, то снизу).
- Запоминается позиция выделения в полях типа ComboBox которые позволяют редактировать данные, но фокус выделения они сбрасывают по Alt+C.
- Возможность запоминать позицию выделения и в других полях (Окно параметров, вкладка «Меню»).
- После перетаскивания слова в какую-то другую программу можно задать нажатие клавиши вправо и нажатие клавиши пробел, что облегчает ввод слов таким методом (другая программа становится активной принудительно, так как почему-то Notepad++ это не делает). Данную функцию можно включить в окне параметров, вкладка «Просмотр в CLCL».
----------------------------------------------------------------------
- Окно параметров теперь всегда выводится поверх всех окон, так как CLCL может быть поверх всех окон.
- Исправлена ошибка индикации статуса CLCL когда полностью данные перезагружались.
- Ctrl+Shift+Клик позволяет не выполнить историю, а открыть вьювер сразу на этом пункте для редактирования (копирование как по Shift оставлено).
- Исправлено управление с клавиатуры в меню по «Ctrl+Enter» (показ меню инструментов или вставка и переоткрытие меню).
- Исправлена ошибка сохранения строки в плагинах «tool_text.dll» и «tool_find.dll» когда строка содержала пробелы в конце.
----------------------------------------------------------------------
- Файл с версии 2.1.3.2 поставляется с настройкой сразу на портативность (clcl_app.ini содержит [GENERAL] portable=1)
- Версия файла теперь пишется из 4 символов, пример: 2.1.3.2
- В меню программы «Справка» добавлен пункт «Просмотр справка» (F1) который откроет на просмотр файл «readme_en.txt» (или readme_XX.txt где XX обозначение языка программы).
- В меню программы добавлены горячие клавиши на некоторые действия. 
- На панели инстументов добавлена переключаемая кнопка «Следить за буфером обмена» (F9).
- На панели инстументов добавлена переключаемая кнопка «Поверх всех окон» (F12) с созданной иконкой. Теперь нет необходимости использовать плагин для этой функции.
- В меню «Вид» есть возможность задать показ управляющих символов (пробел, табуляция, перевод строки), чтобы точнее видеть отличие в скопированных строках.
- Изменился значок у программы. С желтым фоном если «Следить за буфером обмена», а иначе серый фон с красным кругом (красный круг устанавливается теперь и на иконку приложения). Иконка для «CLCLSet.exe» сделана цветной, с надписью «CLCL» (пока везде иконки как и были в 16 цветов, так что по идее на самых древних мониторах проблем не должно быть).
- Теперь программа более корректно реагирует на масштабирование программы в версиях Windows менее 8.1. Изменялись иконки, чтобы можно было реализовать лучший видимый эффект.
- В контекстное меню на пункте буфера обмена добавлен «Инструменты», так как логично для очистки буфера обмена найти функцию именно в нём.
- Изменен для русской версии вызов шаблонов с клавиши «Ш» на пробел (это удобней даже чем R или T). Пока одна рука нажимает Alt+C другая уже готова нажать пробел и не требуется потом тянуться к R, а сразу к нужному номеру или символу который запустит пункт меню.
- Если отключено слежение за буфером обмена, а инструментам требуется функция копирования и вставки, то в контекстном меню в названиях этих инструментов будет написано «off clcl!». Дополнительное напоминание, чтобы знали, что инструмент правильно не сработает и для него требуется включить слежение за буфером обмена.
- Нажатие правой кнопки мыши на пункт контекстного меню теперь выполняет или функцию показа инструмента или же вставку и повторный показ меню (без копирования в историю, для быстрых разнообразных вставок подряд). Возможен выбор одной из этих двух функций и по «Ctrl + клик правой кнопкой мыши».
- Если в параметрах истории стоит «удалять при копировании в буфер обмена» и выключено слежение за буфером обмена, то происходило только удаление из истории и это не позволяло продублировать ввод еще такого-же значения (а скопировать в историю нельзя). Пришлось изменить алгоритм так, чтобы если выключено слежение за буфером обмена, то не удаляло при копировани в буфер обмена.
- Выбранный неактивный пункт в дереве с историей теперь показывается синим цветом шрифта, чтобы его проще находить взглядом, а то слабый оттенок синего фона на некоторых компьютерах не всегда был хорошо виден.

- Окно параметров теперь всегда выводится по центру рабочего стола. Запоминается последняя страница на которой нажали «ОК», чтобы потом при вызове показывать сразу её.
- В окне параметров можно перемещать пункты по Alt+Up(Down), как это работает для главного окна CLCL.
- В параметрах «История» добавлен флаг «При активизации не добавлять текущий буфер обмена в историю». Если вы выключили «Следить за буфером обмена» и какую-то личную переписку скопировали в буфер, то последующее включение CLCL не должно доблять ваши личные данные в Историю без вашего разрешения.
- В параметрах если вы выбрали путь к файлу который лежит в папке CLCL, то путь будет показан относительный, а не абсолютный. Абсолютные пути затрудняют перенос портативной программы на другой компьютер. В связи с этим предпочтительно все ваши DLL, иконки или какие-то другие полезные скрипты держать в одном месте, то есть в папке CLCL.
- Запоминается изменения размеров колонок в таблицах, так как это удобнее пользователю (ширина не менее 10 пикселей, но в коде программы можно поменять #define MIN_COL_SIZE_LISTVIEW 10).
- Возможность задать в параметрах принудительное переключени языка меню в ENG, чтобы все акселераторы & формата %A были активны при любом другом языке ввода.
- Изменен по умолчанию формат для нумерации в меню с «&%1d. %t» на «&%1C. %t» (в начале цифры от 1 до 9 и потом буквы английского языка).
- Если у плагина нет параметров (разработчик закомментировал в файле .def), то кнопка свойств плагина не будет активна.
- В программе может быть несколько одинаковых плагинов, но свойства у файла одни (общие для каждой одинаковой функции плагина), а для отличия необходимо изменять командную строку. Формат командной строки не всегда ясен и для упрощения теперь возможно по кнопке обычных свойств заполнить командную строку (должен уметь плагин), а если она пустая, то это считается как общие свойства. Чтобы сразу можно было видеть какие свойства будут работать (из командной строки или общие), то в начале названия у кнопки со свойством добавляется знак «•» (добавляется «?•» если в командной строке есть в начале знак ?, что может говорить о том, что пользователю будет показано диалоговое окно для выбора параметров).
- Настройки по умолчанию: для действий в начале идёт левая кнопка мыши, потом правая (так как порядок чтения привычный слева направо). Добавлен в эти действия пункт меню «Следить за буфером обмена». По умолчанию в настройках меню выключен флаг «показывать инструменты по правой кнопке» (show_tool_menu), так как инструменты могут вообще не установить, а новая функция «вставка и повторный показ меню» удобнее когда без комбинации Ctrl+. Ширина меню по умолчанию 250, так как прежних 200 не совсем достаточно.
- Подправлена ошибка функции «alloc_copy_n» из «Memory.c» когда не копировало последний символ с помощью «lstrcpyn».
- В плагине tool_utl добавлен относительный путь для файла звука. Выбраный файл сразу проигрывается. (добавлено 3 одинаковых файла с разной громкостью)
- Из плагина tool_utl удалены фунции «ztop» и «unztop», так как это умеет уже делать CLCL с версии 2.1.3.2.
- В плагине tool_utl изменен код так, чтобы вызов из контекстного меню возвращал фокус снова окну с которым работали.
- В плагине tool_find добавлена возможность установить флаг переоткрытия окна. Окно может запоминать свою позицию на экране на время работы с программой (пока программу не переоткрыли).
- В плагине tool_find добавлена возможность выбрать ранее введенные строки которые уже искали. 
- В плагине tool_text, в функции редактирование текста можно задать шрифт окна с помощью настройки в файле tool_text.ini. Окно может запоминать свою высоту, ширину и позицию на экране на время работы с программой (пока программу не переоткрыли). 
- В плагине tool_text, в функциях цитирования (quote, unquote) возможно выбрать из выпадающего списка ранее введенный знак (строку) цитаты. Окно может запоминать свою позицию на экране на время работы с программой (пока программу не переоткрыли).
- В плагине tool_text, в функции удаления разрывов строк (delete_crlf) можно задать команду 1 — удалить начальные пробелы, 0 - не удалять, ? - спросить при вызове. Окно может запоминать свою позицию на экране на время работы с программой (пока программу не переоткрыли).
- В плагине tool_text, в функции вставки текста (put_text) возможно выбрать из выпадающего списка ранее введенные строки. Окно может запоминать свою позицию на экране на время работы с программой (пока программу не переоткрыли).
- В плагине tool_text, в функции объединения текстов (join_text) можно задать команду ? - спросить при вызове. Можно выбрать объединение текста в обратном порядке. Окно может запоминать свою позицию на экране на время работы с программой (пока программу не переоткрыли). 
- Добавлены 3 плагина: «tool_bitmap», «fmt_metafile», «fmt_rtf».
- Решено не захламлять основное расположение программы файлами относящимися к плагинам и создать для них специальную папку «tool_dll» (для инструментов) и «format_dll» (для форматов).
- Отдельные действия теперь могут установить клавиши вставки как у заданного окна (из вкладки "Клавиши"). Это необходимо чтобы какие-то шаблоны отдельные перехватывать сторонними программами. Смотри http://forum.ru-board.com/topic.cgi?forum=5&topic=50494&start=0 "CLCL_AHK - менеджер буфера обмена плюс расширенная обработка текста средствами Autohotkey и AutoIt"

? Не решена проблема с небольшой заторможенностью работы Alt+C в программе NOTEPAD++ если главное меню будет иметь быстрый доступ по клавише C (в разных языках интерфейса свои ускорители ввода). Отключение горячей клавиши Alt+C в самой программе NOTEPAD++ не решает проблему. Решить проблему можно отредактировав перевод меню программы. 
