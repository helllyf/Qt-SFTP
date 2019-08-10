;A simple nsis script file: sample.nsi
!define QTDIR "D:\software\qt\4.8.4"
!define LIBSSH2 "D:\workSpace\qtwork\sftp001"
outfile "sftp-installer20180720.exe"
installDir "$PROGRAMFILES\NJJTSFTP" 
RequestExecutionLevel admin

!include "MUI2.nsh"

;Page directory
;Page instfiles

!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_LANGUAGE "SimpChinese"


!define INSTALLERNAME "SftpSetup"
!define PRODUCT_VERSION "1.0.2.0"
!define PRODUCT_NAME "Sftp"
!define COMPANY_NAME "NJJT"
 
VIProductVersion "${PRODUCT_VERSION}" 	;must be X.X.X.X
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} FileDescription "${PRODUCT_NAME}"  
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} FileVersion "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} ProductName "${PRODUCT_NAME}" 
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} ProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} LegalCopyright "Copyright 2018"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} CompanyName "${COMPANY_NAME}"


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
file ${QTDIR}\plugins\imageformats\qico4.dll

setOutPath $INSTDIR\iconengines
file ${QTDIR}\plugins\iconengines\qsvgicon4.dll

setOutPath $INSTDIR

CreateShortCut "$DESKTOP\鲛腾SFTP上传工具.lnk" "$INSTDIR\sftp001.exe"

sectionEnd
