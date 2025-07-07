@echo off
setlocal enabledelayedexpansion

:: 기존 SoundList.txt 내용을 임시 저장
if exist SoundList.txt (
    copy /Y SoundList.txt OldSoundList.txt >nul
) else (
    echo. > OldSoundList.txt
)

:: Temp 파일 초기화
echo. > TempSoundList.txt

:: 새로 파일 목록 생성
for %%f in (*.ogg *.wav) do (
    set "filename=%%~nf"
    set "ext=%%~xf"

    set "loop="
    set "volume="

    :: 기본값 설정
    if /I "%%~xf"==".ogg" (
        set "defaultLoop=1"
        set "defaultVolume=1.0"
    ) else (
        set "defaultLoop=0"
        set "defaultVolume=0.5"
    )

    :: 기존 설정에서 loop 및 volume 값 가져오기
    for /f "tokens=1-4 delims=," %%a in ('findstr /b /i "%%~nf," OldSoundList.txt') do (
        if /I "%%a"=="%%~nf" (
            set "loop=%%c"
            set "volume=%%d"
        )
    )

    if not defined loop set "loop=!defaultLoop!"
    if not defined volume set "volume=!defaultVolume!"

    echo !filename!,Resource/Sound/%%f,!loop!,!volume! >> TempSoundList.txt
)

:: 결과 저장
move /Y TempSoundList.txt SoundList.txt >nul
del OldSoundList.txt >nul

echo 완료: SoundList.txt가 기존 설정을 유지하면서 갱신되었습니다.
pause