
var variable = "%VSINSTALLDIR% %USERNAME% %USERDOMAIN% %COMPUTERNAME% %HOMEPATH% %APPDATA%";
 
var shell = new ActiveXObject("WScript.Shell");
 
variable = shell.ExpandEnvironmentStrings(variable);
 
WScript.Echo(variable);


var WshNetwork = WScript.CreateObject("WScript.Network");
WScript.Echo(WshNetwork.UserDomain);
WScript.Echo(WshNetwork.ComputerName);
WScript.Echo(WshNetwork.UserName);


var objArgs = WScript.Arguments;
for (i = 0; i < objArgs.length; i++)
{
   WScript.Echo(objArgs(i));
}


WScript.Echo("2008-05-16 12:49:02, DEBUG   : Unknown name...");
WScript.Sleep(3000);
WScript.Echo("2008-05-16 12:49:02, STATUS  : Copying...");
WScript.Sleep(3000);

WScript.Echo("2008-05-16 12:49:02, STATUS  : Hello world!");
WScript.Sleep(3000);
WScript.Echo("2008-05-16 12:49:02, DEBUG   : Unknown name...");
WScript.Sleep(3000);
WScript.Echo("2008-05-16 12:49:02, STATUS  : Starting software synchronization Profile inconsitency: Package with ID 'java' is listed in profile but does not exist within  the package database or the local settings file. Please contact your system administrator!");
WScript.Sleep(3000);
WScript.Echo("2008-05-16 12:49:02, STATUS  : Number of packages to be removed: 0");
WScript.Sleep(3000);
WScript.Echo("2008-05-16 12:49:02, STATUS  : Install: Verifying package 'KB888111' (1/14)");
WScript.Sleep(3000);
WScript.Echo("2008-05-16 12:49:02, STATUS  : Install: Verifying package 'Mozilla Firefox 2.0' (2/14)");
WScript.Sleep(3000);
WScript.Echo("2008-05-16 12:49:02, STATUS  : Install: Verifying package 'nx6325 audio driver' (3/14)");
WScript.Sleep(3000);
WScript.Echo("2008-05-16 12:49:02, STATUS  : Install: Verifying package 'Broadcom 802.11 Wireless LAN Adapter' (4/14)");
WScript.Sleep(3000);
WScript.Echo("2008-05-16 12:49:02, STATUS  : Install: Verifying package 'HDAUDIO Soft Data Fax Modem with SmartCP' (5/14)");
WScript.Sleep(3000);


WScript.Echo("2008-05-16 12:49:02, STATUS  : Copying large file...");
WScript.Sleep(120000);



WScript.Echo("2008-05-16 12:49:02, STATUS  : Successfuly done.");
WScript.Sleep(4000);

WScript.Quit(0);