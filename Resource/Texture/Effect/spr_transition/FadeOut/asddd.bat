@echo off
setlocal enabledelayedexpansion

rem 1단계: 임시 이름으로 변경 (충돌 방지)
for /L %%i in (0,1,27) do (
    ren "%%i.png" "temp_%%i.png"
)

rem 2단계: 역순으로 이름 변경
for /L %%i in (0,1,27) do (
    set /a newname=27 - %%i
    ren "temp_%%i.png" "!newname!.png"
)

echo 역순 리네이밍 완료.
pause