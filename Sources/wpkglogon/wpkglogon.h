

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WPKGLOGON_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WPKGLOGON_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//#ifdef WPKGLOGON_EXPORTS
//#define WPKGLOGON_API __declspec(dllexport)
//#else
//#define WPKGLOGON_API __declspec(dllimport)
//#endif

//// This class is exported from the wpkglogon.dll
//class WPKGLOGON_API Cwpkglogon {
//public:
//	Cwpkglogon(void);
//	// TODO: add your methods here.
//};
//
//extern WPKGLOGON_API int nwpkglogon;
//
//WPKGLOGON_API int fnwpkglogon(void);
//WPKGLOGON_API VOID WLEventLogon (PWLX_NOTIFICATION_INFO pInfo);
