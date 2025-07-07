@echo off
setlocal enabledelayedexpansion

rem === 현재 폴더 이름 = 출력 텍스트 파일 이름 ===
for %%F in (.) do set "OUTPUT=%%~nxF.txt"

rem === 기존 내용을 임시로 저장 ===
if exist "%OUTPUT%" (
    copy "%OUTPUT%" "%OUTPUT%.bak" >nul
)

rem === 폴더 수 초기화 및 카운트 ===
set /a folderCount=0
for /d %%D in (*) do (
    set /a folderCount+=1
)

rem === 새 출력 파일 시작 ===
echo %folderCount% > "%OUTPUT%"

rem === 하위 폴더 처리 ===
for /d %%D in (*) do (
    pushd "%%D"

    rem === 파일명 숫자 리네이밍 ===
    for %%F in (*_*.png) do (
        set "fname=%%~nF"
        call :ExtractNumber "%%~nF" num
        ren "%%F" "!num!%%~xF" >nul 2>&1
    )

    rem === PNG 파일 개수 세기 ===
    dir /b *.png | find /c /v "" > filecount.tmp
    set /p count=<filecount.tmp
    del filecount.tmp

    rem === 경로: Resource부터 시작 ===
    for %%P in ("%cd%") do set "full=%%~fP"
    set "relativePath=!full:*Resource=Resource!"

    rem === 기본 loop/fps 값 설정 ===
    set "loop=0"
    set "fps=15"

    rem === 기존 txt에서 해당 항목의 loop/fps 찾아서 유지 ===
    if exist "%~dp0%OUTPUT%.bak" (
        for /f "tokens=1-5 delims=," %%A in ('findstr /b "%%D," "%~dp0%OUTPUT%.bak"') do (
            set "loop=%%D"
            set "fps=%%E"
        )
    )

    rem === 정보 줄 기록 ===
    echo %%D,!relativePath!\%%D,!count!,!loop!,!fps! >> "%~dp0%OUTPUT%"

    popd
)

rem === 임시 백업 삭제 ===
if exist "%OUTPUT%.bak" del "%OUTPUT%.bak"

echo 작업 완료: %OUTPUT%
pause
goto :eof

rem === 확장자 앞 숫자 추출 ===
:ExtractNumber
setlocal
set "name=%~1"
for %%A in (%name:_= %) do set "last=%%A"
(
endlocal
set "%~2=%last%"
)
goto :eof