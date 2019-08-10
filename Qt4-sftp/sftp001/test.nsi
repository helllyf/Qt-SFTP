;A simple nsis script file: sample.nsi
!define QTDIR "D:\software\qt\4.8.4"
!define LIBSSH2 "D:\workSpace\qtwork\sftp001"
outfile "sftp-installer.exe"
installDir "$PROGRAMFILES\Asample" 
RequestExecutionLevel admin


Page directory
Page instfiles

section


setOutPath $INSTDIR
file sftp001.exe
file ${QTDIR}\bin\mingwm10.dll
file ${QTDIR}\bin\QtCore4.dll
file ${QTDIR}\bin\libgcc_s_dw2-1.dll
file ${QTDIR}\bin\QtGui4.dll
file ${QTDIR}\bin\QtNetwork4.dll
file ${LIBSSH2}\lib\libssh2.dll
file ${LIBSSH2}\lib\libeay32.dll
file ${LIBSSH2}\favicon.ico


setOutPath $INSTDIR\imageformats
file ${QTDIR}\plugins\imageformats\qjpeg4.dll

createShortCut "$DESKTOP\sftp.lnk" "$INSTDIR\sftp-installer.exe" "$INSTDIR\Í¼±ê.ico"

sectionEnd

