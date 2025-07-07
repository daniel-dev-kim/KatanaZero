@echo off
setlocal enabledelayedexpansion

echo 숫자만 남기고 리네이밍 중...

for %%F in (*_*.png) do (
    set "orig=%%~nF"
    call :ExtractNumber "%%~nF" num
    ren "%%F" "!num!%%~xF" >nul 2>&1
)

echo 완료!
pause
goto :eof

:ExtractNumber
setlocal
set "name=%~1"
rem 마지막 _ 뒤 숫자만 추출
for %%A in (%name:_= %) do set "last=%%A"
(
endlocal
set "%~2=%last%"
)
goto :eof