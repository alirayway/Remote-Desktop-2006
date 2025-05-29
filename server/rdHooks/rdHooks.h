//
// Winrd uses this DLL to hook into the system message pipeline, allowing it
// to intercept messages which may be relevant to screen update strategy
//

#if !defined(_rdHOOKS_DLL_)
#define _rdHOOKS_DLL_

#include <windows.h>

/////////////////////////////////////////////////////////////////////////////
// Define the import/export tags

#define DllImport __declspec(dllimport)
#define DllExport __declspec(dllexport)

/////////////////////////////////////////////////////////////////////////////
//
// Functions used by Winrd

extern "C"
{
	// DLL functions:
	DllExport BOOL SetHook(
		HWND hWnd,
		UINT UpdateMsg,
		UINT CopyMsg,
		UINT MouseMsg
		);											// Set the hook
	DllExport BOOL UnSetHook(HWND hWnd);			// Remove it

	DllExport BOOL SetKeyboardFilterHook(BOOL activate);
													// Control keyboard filtering
	DllExport BOOL SetMouseFilterHook(BOOL activate);
													// Control mouse filtering
}

#endif // !defined(_rdHOOKS_DLL_)
