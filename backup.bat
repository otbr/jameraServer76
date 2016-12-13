@echo off
 
set MYSQLDUMP_PATH="mysqldump.exe"
set BACKUP_OUTPUT_DIR="backup"
 
set MYSQL_USER="root"
set MYSQL_PWD=""
set MYSQL_DB=""

FOR /F "tokens=1,2,3 delims=/ " %%a in ("%DATE%") do (
set DIA=%%a
set MES=%%b
set ANO=%%c
)

FOR /F "tokens=1,2,3 delims=:, " %%a in ("%TIME%") do (
set H=%%a
set M=%%b
set S=%%c
)

set H=0%H%
set H=%H:~-2%
goto CRIAR

:CRIAR
%MYSQLDUMP_PATH% -u %MYSQL_USER% -p %MYSQL_PWD% %MYSQL_DB% > %BACKUP_OUTPUT_DIR%\%MYSQL_DB%-%DIA%-%MES%-%ANO%___%H%-%M%-%S%.sql

echo [Server Save] Backup Completed!