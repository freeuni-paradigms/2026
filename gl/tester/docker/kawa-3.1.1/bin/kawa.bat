@echo off

rem Ideas and some code re-used from Ant's ant.bat.

@setlocal

if "%KAWA_HOME%"=="" goto setDefaultKawaHome
:stripKawaHome
if not _%KAWA_HOME:~-1%==_\ goto endKawaHome
set KAWA_HOME=%KAWA_HOME:~0,-1%
goto stripKawaHome
:setDefaultKawaHome
set KAWA_HOME=%~dp0..
rem set THIS_SCRIPT2=%~dp$PATH:0
:endKawaHome

:checkJava
set _JAVACMD=%JAVACMD%
if "%JAVA_HOME%" == "" goto noJavaHome
if not exist "%JAVA_HOME%\bin\java.exe" goto noJavaHome
if "%_JAVACMD%" == "" set _JAVACMD=%JAVA_HOME%\bin\java.exe
goto endcheckJava
:noJavaHome
if "%_JAVACMD%" == "" set _JAVACMD=java.exe
:endcheckJava

set KAWA_PATH=%KAWA_HOME%\lib\kawa.jar
rem ??? set EXTRA_PATH=@DEFAULT_EXTRA_PATH@
set KAWA_EXTRA_PATH="%KAWA_HOME%\lib\jline.jar;%KAWA_HOME%\lib\domterm.jar;%KAWA_HOME%\lib\servlet.jar"

"%_JAVACMD%" -Dkawa.home="%KAWA_HOME%" -classpath "%KAWA_PATH%;%KAWA_EXTRA_PATH%" kawa.repl %*%
