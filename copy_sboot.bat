
@echo off
SET LIB_FILE=.\objects\sboot.bin
SET DEST_FOLDER=..\rivet_gun\control\control\

xcopy /e /q /i /y %LIB_FILE%    %DEST_FOLDER%

::@echo Done!

::pause