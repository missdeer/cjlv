; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{889D6462-F830-4294-B564-3824442FD6EC}
AppName=Cisco Jabber Log Viewer 1.0 - x64
AppVerName=Cisco Jabber Log Viewer 1.0 - x64
AppPublisher=fyang3@cisco.com
AppPublisherURL=http://www.cisco.com/
AppSupportURL=http://www.cisco.com/
AppUpdatesURL=http://www.cisco.com/
DefaultDirName={pf}\Cisco Jabber Log Viewer
AllowNoIcons=true
OutputDir=.\
OutputBaseFilename=CiscoJabberLogViewerSetup_x64
SolidCompression=true
Compression=lzma/Ultra
InternalCompressLevel=Ultra
VersionInfoVersion=1.0.0.1
VersionInfoCompany=Cisco
VersionInfoDescription=Cisco Jabber Log Viewer 1.0 - x64
VersionInfoTextVersion=Cisco Jabber Log Viewer 1.0 - x64
VersionInfoCopyright=Copyright (c) 2016 Cisco.com. All rights reserved.
AppCopyright=Copyright (C) 2016 Cisco.com. All rights reserved.
PrivilegesRequired=none
AllowRootDirectory=true
AllowUNCPath=false
ShowLanguageDialog=auto
WizardImageFile=compiler:WizModernImage-IS.bmp
WizardSmallImageFile=compiler:WizModernSmallImage-IS.bmp
AppVersion=1.0.0.1
UninstallDisplayName=Cisco Jabber Log Viewer 1.0 - x64
ChangesAssociations=true
ChangesEnvironment=true
DefaultGroupName=Cisco Jabber Log Viewer
AppContact=fyang3@cisco.com
SetupIconFile=..\..\cjlv\cjlv.ico
ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: enu; MessagesFile: compiler:Default.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}

[Files]
Source: "Cisco Jabber Log Viewer.exe"; DestDir: "{app}"
Source: "ScintillaEdit3.dll"; DestDir: "{app}"
Source: "iconengines\*"; DestDir: "{app}\iconengines"; Flags: recursesubdirs       
Source: "imageformats\*"; DestDir: "{app}\imageformats"; Flags: recursesubdirs
Source: "language\*"; DestDir: "{app}\language"; Flags: recursesubdirs
Source: "platforms\*"; DestDir: "{app}\platforms"; Flags: recursesubdirs
Source: "sqldrivers\qsqlite.dll"; DestDir: "{app}\sqldrivers"; Flags: recursesubdirs
Source: "themes\*"; DestDir: "{app}\themes"; Flags: recursesubdirs
Source: "extensions\*"; DestDir: "{app}\extensions"; Flags: recursesubdirs
Source: "langmap.xml"; DestDir: "{app}"
Source: "d3dcompiler_47.dll"; DestDir: "{app}"
Source: "Everything.dll"; DestDir: "{app}"
Source: "Everything.exe"; DestDir: "{app}"
Source: "libEGL.dll"; DestDir: "{app}"
Source: "libGLESV2.dll"; DestDir: "{app}"
Source: "opengl32sw.dll"; DestDir: "{app}"
Source: "Qt5Core.dll"; DestDir: "{app}"
Source: "Qt5Gui.dll"; DestDir: "{app}"
Source: "Qt5Sql.dll"; DestDir: "{app}"
Source: "Qt5Svg.dll"; DestDir: "{app}"
Source: "Qt5Widgets.dll"; DestDir: "{app}"
Source: "Qt5Xml.dll"; DestDir: "{app}"

[Icons]
Name: {commondesktop}\Cisco Jabber Log Viewer; Filename: {app}\Cisco Jabber Log Viewer.exe; WorkingDir: {app}; Tasks: desktopicon; Comment: Launch Cisco Jabber Log Viewer
Name: {group}\Cisco Jabber; Filename: {app}\Cisco Jabber Log Viewer.exe; WorkingDir: {app}; Comment: Launch Cisco Jabber Log Viewer
Name: {group}\{cm:UninstallProgram, Cisco Jabber Log Viewer}; Filename: {uninstallexe}; IconFilename: {app}\Cisco Jabber Log Viewer.exe; IconIndex: 0; 

[Messages]
BeveledLabel=Powered by fyang3@cisco.com

[UninstallDelete]

[Registry]
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\Cisco Jabber Log Viewer.exe; ValueType: string; ValueData: {app}\Cisco Jabber Log Viewer.exe; Flags: uninsdeletekey

[Run]
Filename: {app}\Cisco Jabber Log Viewer.exe; Description: "Launch Cisco Jabber Log Viewer"; Flags: postinstall shellexec 
