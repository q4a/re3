#define _WIN32_WINDOWS 0x0500
#define WINVER 0x0500
#define DIRECTINPUT_VERSION 0x0800

#include <winerror.h>
#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>

#include <windowsx.h>
#include <basetsd.h>

#include <regstr.h>
#include <shlobj.h>

#include <dbt.h>

#pragma warning( push )
#pragma warning( disable : 4005)
#include <d3d8.h>
#include <ddraw.h>
#include <dinput.h>
#include <DShow.h>
#pragma warning( pop )

#define WM_GRAPHNOTIFY	WM_USER+13

#pragma comment( lib, "d3d8.lib" )
#pragma comment( lib, "ddraw.lib" )
#pragma comment( lib, "Winmm.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "strmiids.lib" )
#pragma comment( lib, "dinput8.lib" )

#if (defined(_MSC_VER))
#include <tchar.h>
#endif /* (defined(_MSC_VER)) */
#include <stdio.h>
#include "rwcore.h"
#include "resource.h"
#include "skeleton.h"
#include "platform.h"
#include "win.h"

#define MAX_SUBSYSTEMS		(16)


//static RwBool		  ForegroundApp = TRUE;
static RwBool		&ForegroundApp = *(RwBool*)0x060F000;

//static RwBool		  RwInitialised = FALSE;
static RwBool		&RwInitialised = *(RwBool*)0x885B88;

static RwSubSystemInfo GsubSysInfo[MAX_SUBSYSTEMS];
static RwInt32		GnumSubSystems = 0;
static RwInt32		GcurSel = 0, GcurSelVM = 0;

//static RwBool startupDeactivate;
static RwBool &startupDeactivate = *(RwBool*)0x8E2878;

//static RwBool useDefault;
static RwBool &useDefault = *(RwBool*)0x6510D4;

//static RwBool defaultFullscreenRes = TRUE;
static RwBool &defaultFullscreenRes = *(RwBool*)0x60EFFC;

/* Class name for the MS Window's window class. */

static const RwChar *AppClassName = RWSTRING("Grand theft auto 3");

//static psGlobalType PsGlobal;
static psGlobalType &PsGlobal = *(psGlobalType*)0x72CF60;


#define PSGLOBAL(var) (((psGlobalType *)(RsGlobal.ps))->var)

#undef MAKEPOINTS
#define MAKEPOINTS(l)		(*((POINTS /*FAR*/ *)&(l)))

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }
#define JIF(x) if (FAILED(hr=(x))) \
	{debug(TEXT("FAILED(hr=0x%x) in ") TEXT(#x) TEXT("\n"), hr); return;}

#include "common.h"
#include "patcher.h"
#include "main.h"
#include "FileMgr.h"
#include "Text.h"
#include "Pad.h"
#include "Timer.h"
#include "DMAudio.h"
#include "ControllerConfig.h"
#include "Frontend.h"
#include "Game.h"
#include "PCSave.h"
#include "Sprite2D.h"

VALIDATE_SIZE(psGlobalType, 0x28);

// DirectShow interfaces
IGraphBuilder *pGB = NULL;
IMediaControl *pMC = NULL;
IMediaEventEx *pME = NULL;
IVideoWindow  *pVW = NULL;
IMediaSeeking *pMS = NULL;

DWORD dwDXVersion;
DWORD _dwMemTotalPhys;
DWORD &_dwMemAvailPhys = *(DWORD*)0x70F29C;
DWORD _dwMemTotalVirtual;
DWORD _dwMemAvailVirtual;
DWORD &_dwMemTotalVideo = *(DWORD*)0x70F2A8;
DWORD _dwMemAvailVideo;
DWORD &_dwOperatingSystemVersion = *(DWORD*)0x70F290;

RwUInt32 &gGameState = *(RwUInt32*)0x8F5838;
WRAPPER bool InitialiseGame(void) { EAXJMP(0x48E7E0); }

WRAPPER const char *GetLevelSplashScreen(int32 number) { EAXJMP(0x48D750); }
//

void LoadingScreen(char const *msg1, char const *msg2, char const *screen);
CSprite2d *LoadSplash(const char *name);

enum eJoypadState
{
  JOYPAD_UNUSED,
  JOYPAD_ATTACHED,
};

struct tJoy
{
	eJoypadState m_State;
	bool m_bInitialised;
	bool m_bHasAxisZ;
	bool m_bHasAxisR;
	char _pad0;
	int32 m_nVendorID;
	int32 m_nProductID;
};

class CJoySticks
{
public:
	tJoy m_aJoys[2];
	
	CJoySticks();
	void ClearJoyInfo(int joyID);
};

static CJoySticks AllValidWinJoys;

CJoySticks::CJoySticks()
{
	for (int i = 0; i < _TODOCONST(2); i++)
	{
		ClearJoyInfo(i);
	}
}

void CJoySticks::ClearJoyInfo(int joyID)
{
	
	m_aJoys[joyID].m_State = JOYPAD_UNUSED;
	m_aJoys[joyID].m_bInitialised = false;
	m_aJoys[joyID].m_bHasAxisZ = false;
	m_aJoys[joyID].m_bHasAxisR = false;
}



/*
 *****************************************************************************
 */
void _psCreateFolder(LPCSTR path)
{
	HANDLE hfle = CreateFile(path, GENERIC_READ, 
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									FILE_FLAG_BACKUP_SEMANTICS | FILE_ATTRIBUTE_NORMAL,
									NULL);

	if ( hfle == INVALID_HANDLE_VALUE )
		CreateDirectory(path, NULL);
	else
		CloseHandle(hfle);
}

/*
 *****************************************************************************
 */
const char *_psGetUserFilesFolder()
{
#ifdef USE_MY_DOCUMENTS
	HKEY hKey = NULL;

	static CHAR szUserFiles[256];

	if ( RegOpenKeyEx(HKEY_CURRENT_USER,
						REGSTR_PATH_SPECIAL_FOLDERS,
						REG_OPTION_RESERVED,
						KEY_READ,
						&hKey) == ERROR_SUCCESS )
	{
		DWORD KeyType;
		DWORD KeycbData = sizeof(szUserFiles);
		if ( RegQueryValueEx(hKey,
							"Personal",
							NULL,
							&KeyType,
							(LPBYTE)szUserFiles,
							&KeycbData) == ERROR_SUCCESS )
		{
			RegCloseKey(hKey);
			strcat(szUserFiles, "\\GTA3 User Files");
			_psCreateFolder(szUserFiles);
			return szUserFiles;
		}	

		RegCloseKey(hKey);		
	}
	
	strcpy(szUserFiles, "data");
	return szUserFiles;
#else
	static CHAR szUserFiles[256];
	strcpy(szUserFiles, "userfiles");
	_psCreateFolder(szUserFiles);
	return szUserFiles;
#endif
}

/*
 *****************************************************************************
 */
RwBool
psCameraBeginUpdate(RwCamera *camera)
{
	if ( !RwCameraBeginUpdate(Scene.camera) )
	{
		ForegroundApp = FALSE;
		RsEventHandler(rsACTIVATE, (void *)FALSE);
		return FALSE;
	}
	
	return TRUE;
}

/*
 *****************************************************************************
 */
void
psCameraShowRaster(RwCamera *camera)
{
	if (CMenuManager::m_PrefsVsync)
		RwCameraShowRaster(camera, PSGLOBAL(window), rwRASTERFLIPWAITVSYNC);
	else
		RwCameraShowRaster(camera, PSGLOBAL(window), rwRASTERFLIPDONTWAIT);

	return;
}


/*
 *****************************************************************************
 */
RwUInt32
psTimer(void)
{
	RwUInt32 time;

	TIMECAPS TimeCaps;
	
	timeGetDevCaps(&TimeCaps, sizeof(TIMECAPS));
	
	timeBeginPeriod(TimeCaps.wPeriodMin);
	
	time = (RwUInt32) timeGetTime();

	timeEndPeriod(TimeCaps.wPeriodMin);
	
	return time;
}

/*
 *****************************************************************************
 */
void
psMouseSetPos(RwV2d *pos)
{
	POINT point;

	point.x = (RwInt32) pos->x;
	point.y = (RwInt32) pos->y;

	ClientToScreen(PSGLOBAL(window), &point);

	SetCursorPos(point.x, point.y);
	
	PSGLOBAL(lastMousePos.x) = (RwInt32)pos->x;

	PSGLOBAL(lastMousePos.y) = (RwInt32)pos->y;

	return;
}

/*
 *****************************************************************************
 */
RwMemoryFunctions*
psGetMemoryFunctions(void)
{
	return 0;
}

/*
 *****************************************************************************
 */
RwBool
psInstallFileSystem(void)
{
	return (TRUE);
}


/*
 *****************************************************************************
 */
RwBool
psNativeTextureSupport(void)
{
	return RwD3D8DeviceSupportsDXTTexture();
}

/*
 *****************************************************************************
 */
static BOOL
InitApplication(HANDLE instance)
{
	/*
	 * Perform any necessary MS Windows application initialization. Basically,
	 * this means registering the window class for this application.
	 */

	WNDCLASS windowClass;

	windowClass.style = CS_BYTEALIGNWINDOW;
	windowClass.lpfnWndProc = (WNDPROC) MainWndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = (HINSTANCE)instance;
	windowClass.hIcon = NULL;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = AppClassName;

	return RegisterClass(&windowClass);
}


/*
 *****************************************************************************
 */
static HWND
InitInstance(HANDLE instance)
{
	/*
	 * Perform any necessary initialization for this instance of the 
	 * application.
	 *
	 * Create the MS Window's window instance for this application. The
	 * initial window size is given by the defined camera size. The window 
	 * is not given a title as we set it during Init3D() with information 
	 * about the version of RenderWare being used.
	 */

	RECT rect;

	rect.left = rect.top = 0;
	rect.right = RsGlobal.maximumWidth;
	rect.bottom = RsGlobal.maximumHeight;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	return CreateWindow(AppClassName, RsGlobal.appName,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, CW_USEDEFAULT,
						rect.right - rect.left, rect.bottom - rect.top,
						(HWND)NULL, (HMENU)NULL, (HINSTANCE)instance, NULL);
}

void _GetVideoMemInfo(LPDWORD total, LPDWORD avaible)
{
	HRESULT hr;
	LPDIRECTDRAW7 pDD7;
	
	hr = DirectDrawCreateEx(NULL, (VOID**)&pDD7, IID_IDirectDraw7, NULL);
	
	if ( FAILED(hr) )
		return;
	
	DDSCAPS2 caps;
	
	ZeroMemory(&caps, sizeof(DDSCAPS2));
	caps.dwCaps = DDSCAPS_VIDEOMEMORY;
	
	pDD7->GetAvailableVidMem(&caps, total, avaible);
	
	pDD7->Release();
}

/*
 *****************************************************************************
 */
typedef HRESULT(WINAPI * DIRECTDRAWCREATEEX)( GUID*, VOID**, REFIID, IUnknown* );


//-----------------------------------------------------------------------------
// Name: GetDXVersion()
// Desc: This function returns the DirectX version number as follows:
//			0x0000 = No DirectX installed
//			0x0700 = At least DirectX 7 installed.
//			0x0800 = At least DirectX 8 installed.
// 
//		 Please note that this code is intended as a general guideline. Your
//		 app will probably be able to simply query for functionality (via
//		 QueryInterface) for one or two components.
//
//		 Please also note:
//			"if( dwDXVersion != 0x500 ) return FALSE;" is VERY BAD. 
//			"if( dwDXVersion <	0x500 ) return FALSE;" is MUCH BETTER.
//		 to ensure your app will run on future releases of DirectX.
//-----------------------------------------------------------------------------
DWORD GetDXVersion()
{
	DIRECTDRAWCREATEEX	 DirectDrawCreateEx = NULL;
	HINSTANCE			 hDDrawDLL			= NULL;
	HINSTANCE			 hD3D8DLL			= NULL;
	HINSTANCE			 hDPNHPASTDLL		= NULL;
	DWORD				 dwDXVersion		= 0;
	//HRESULT			   hr;

	// First see if DDRAW.DLL even exists.
	hDDrawDLL = LoadLibrary( "DDRAW.DLL" );
	if( hDDrawDLL == NULL )
	{
		dwDXVersion = 0;
		OutputDebugString( "Couldn't LoadLibrary DDraw\r\n" );
		return dwDXVersion;
	}


	//-------------------------------------------------------------------------
	// DirectX 7.0 Checks
	//-------------------------------------------------------------------------

	// Check for DirectX 7 by creating a DDraw7 object
	LPDIRECTDRAW7 pDD7;
	DirectDrawCreateEx = (DIRECTDRAWCREATEEX)GetProcAddress( hDDrawDLL,
													   "DirectDrawCreateEx" );
	if( NULL == DirectDrawCreateEx )
	{
		FreeLibrary( hDDrawDLL );
		OutputDebugString( "Couldn't GetProcAddress DirectDrawCreateEx\r\n" );
		return dwDXVersion;
	}

	if( FAILED( DirectDrawCreateEx( NULL, (VOID**)&pDD7, IID_IDirectDraw7,
									NULL ) ) )
	{
		FreeLibrary( hDDrawDLL );
		OutputDebugString( "Couldn't DirectDrawCreateEx\r\n" );
		return dwDXVersion;
	}

	// DDraw7 was created successfully. We must be at least DX7.0
	dwDXVersion = 0x700;
	pDD7->Release();


	//-------------------------------------------------------------------------
	// DirectX 8.0 Checks
	//-------------------------------------------------------------------------

	// Simply see if D3D8.dll exists.
	hD3D8DLL = LoadLibrary( "D3D8.DLL" );
	if( hD3D8DLL == NULL )
	{
		FreeLibrary( hDDrawDLL );
		OutputDebugString( "Couldn't LoadLibrary D3D8.DLL\r\n" );
		return dwDXVersion;
	}

	// D3D8.dll exists. We must be at least DX8.0
	dwDXVersion = 0x800;


	//-------------------------------------------------------------------------
	// DirectX 8.1 Checks
	//-------------------------------------------------------------------------

	// Simply see if dpnhpast.dll exists.
	hDPNHPASTDLL = LoadLibrary( "dpnhpast.dll" );
	if( hDPNHPASTDLL == NULL )
	{
		FreeLibrary( hDPNHPASTDLL );
		OutputDebugString( "Couldn't LoadLibrary dpnhpast.dll\r\n" );
		return dwDXVersion;
	}

	// dpnhpast.dll exists. We must be at least DX8.1
	dwDXVersion = 0x801;


	//-------------------------------------------------------------------------
	// End of checking for versions of DirectX 
	//-------------------------------------------------------------------------

	// Close open libraries and return
	FreeLibrary( hDDrawDLL );
	FreeLibrary( hD3D8DLL );
	
	return dwDXVersion;
}

/*
 *****************************************************************************
 */
static char cpuvendor[16] = "UnknownVendr";
__declspec(naked)  const char * _psGetCpuVendr()
{
	__asm
	{
		push	ebx
		xor		eax, eax
		cpuid
		mov		dword ptr [cpuvendor+0], ebx
		mov		dword ptr [cpuvendor+4], edx
		mov		dword ptr [cpuvendor+8], ecx
		mov		eax, offset cpuvendor
		pop		ebx
		retn
	}
}

/*
 *****************************************************************************
 */
__declspec(naked) RwUInt32 _psGetCpuFeatures()
{
	__asm
	{
		mov		eax, 1
		cpuid
		mov		eax, edx
		retn
	}
}

/*
 *****************************************************************************
 */
__declspec(naked) RwUInt32 _psGetCpuFeaturesEx()
{
	__asm
	{
		mov		eax, 80000000h
		cpuid

		cmp		eax, 80000000h
		jbe		short _NOEX

		mov		eax, 80000001h
		cpuid

		mov		eax, edx
		jmp		short _RETEX

_NOEX:
		xor		eax, eax
		mov		eax, eax
		
_RETEX:
		retn   
	}
}

void _psPrintCpuInfo()
{
	RwUInt32 features	= _psGetCpuFeatures();
	RwUInt32 FeaturesEx = _psGetCpuFeaturesEx();

	debug("Running on a %s", _psGetCpuVendr());

	if ( features & 0x800000 )
		debug("with MMX");
	if ( features & 0x2000000 )
		debug("with SSE");
	if ( FeaturesEx & 0x80000000 )
		debug("with 3DNow");
}

/*
 *****************************************************************************
 */
#ifdef UNDER_CE
#define CMDSTR	LPWSTR
#else
#define CMDSTR	LPSTR
#endif

/*
 *****************************************************************************
 */
RwBool
psInitialise(void)
{
	PsGlobal.lastMousePos.x = PsGlobal.lastMousePos.y = 0.0;

	RsGlobal.ps = &PsGlobal;
	
	PsGlobal.fullScreen = FALSE;
	
	PsGlobal.dinterface = NULL;
	PsGlobal.mouse	   = NULL;
	PsGlobal.joy1	= NULL;
	PsGlobal.joy2	= NULL;

	CFileMgr::Initialise();
	
	C_PcSave::SetSaveDirectory(_psGetUserFilesFolder());
	
#ifndef NASTY_GAME
		InitialiseLanguage();
#endif

	FrontEndMenuManager.LoadSettings();
	
	gGameState = GS_START_UP;
	TRACE("gGameState = GS_START_UP");
	
	_psPrintCpuInfo();
	
	OSVERSIONINFO verInfo;
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	GetVersionEx(&verInfo);
	
	_dwOperatingSystemVersion = 0;
	
	if ( verInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		if ( verInfo.dwMajorVersion == 4 )
		{
			debug("Operating System is WinNT\n");
			_dwOperatingSystemVersion = oS_WINNT;
		}
		else if ( verInfo.dwMajorVersion == 5 )
		{
			debug("Operating System is Win2000\n");
			_dwOperatingSystemVersion = OS_WIN2000;
		}
		else if ( verInfo.dwMajorVersion > 5 )
		{
			debug("Operating System is WinXP or greater\n");
			_dwOperatingSystemVersion = OS_WINXP;
		}
	}
	else if ( verInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
		if ( verInfo.dwMajorVersion > 4 || verInfo.dwMajorVersion == 4 && verInfo.dwMinorVersion == 1 )
		{
			debug("Operating System is Win98\n");
			_dwOperatingSystemVersion = OS_WIN98;
		}
		else
		{
			debug("Operating System is Win95\n");
			_dwOperatingSystemVersion = OS_WIN95;
		}
	}
	
	dwDXVersion = GetDXVersion();
	debug("DirectX version 0x%x\n", dwDXVersion);
	
	if ( _dwOperatingSystemVersion == OS_WIN95 )
	{
		MessageBoxW(NULL,
					(LPCWSTR)TheText.Get("WIN_95"),	 // Grand Theft Auto III cannot run on Windows 95
					(LPCWSTR)TheText.Get("WIN_TTL"), // Grand Theft Auto III
					MB_OK);

		return FALSE;
	}
	
	if ( dwDXVersion < 0x801 )
	{
		MessageBoxW(NULL,
					(LPCWSTR)TheText.Get("WIN_DX"),	 // Grand Theft Auto III requires at least DirectX version 8.1
					(LPCWSTR)TheText.Get("WIN_TTL"), // Grand Theft Auto III
					MB_OK);

		return FALSE;
	}
	
	MEMORYSTATUS memstats;
	GlobalMemoryStatus(&memstats);
	
	_dwMemTotalPhys	   = memstats.dwTotalPhys;
	_dwMemAvailPhys	   = memstats.dwAvailPhys;
	_dwMemTotalVirtual = memstats.dwTotalVirtual;
	_dwMemAvailVirtual = memstats.dwAvailVirtual;
	
	_GetVideoMemInfo(&_dwMemTotalVideo, &_dwMemAvailVideo);
	
	debug("Physical memory size %d\n", _dwMemTotalPhys);
	debug("Available physical memory %d\n", _dwMemAvailPhys);
	debug("Video memory size %d\n", _dwMemTotalVideo);
	debug("Available video memory %d\n", _dwMemAvailVideo);
	
	if ( _dwMemAvailVideo < (12 * 1024 * 1024) /*12 MB*/ )
	{
		MessageBoxW(NULL,
					(LPCWSTR)TheText.Get("WIN_VDM"), // Grand Theft Auto III requires at least 12MB of available video memory
					(LPCWSTR)TheText.Get("WIN_TTL"), // Grand Theft Auto III
					MB_OK);

		return FALSE;
	}
	
	TheText.Unload();

	return TRUE;
}


/*
 *****************************************************************************
 */
void
psTerminate(void)
{
	return;
}

/*
 *****************************************************************************
 */
static RwChar **_VMList;

RwInt32 _psGetNumVideModes()
{
	return RwEngineGetNumVideoModes();
}

/*
 *****************************************************************************
 */
RwBool _psFreeVideoModeList()
{
	RwInt32 numModes;
	RwInt32 i;
	
	numModes = _psGetNumVideModes();
	
	if ( _VMList == NULL )
		return TRUE;
	
	for ( i = 0; i < numModes; i++ )
	{
		RwFree(_VMList[i]);
	}
	
	RwFree(_VMList);
	
	_VMList = NULL;
	
	return TRUE;
}
							
/*
 *****************************************************************************
 */							
RwChar **_psGetVideoModeList()
{
	RwInt32 numModes;
	RwInt32 i;
	
	if ( _VMList != NULL )
	{
		return _VMList;
	}
	
	numModes = RwEngineGetNumVideoModes();
	
	_VMList = (RwChar **)RwCalloc(numModes, sizeof(RwChar*));
	
	for ( i = 0; i < numModes; i++	)
	{
		RwVideoMode			vm;
		
		RwEngineGetVideoModeInfo(&vm, i);
		
		if ( vm.flags & rwVIDEOMODEEXCLUSIVE )
		{
			if (   vm.width >= 640
				&& vm.height >= 480
				&& (vm.width == 640
				&& vm.height == 480) 
				|| !(vm.flags & rwVIDEOMODEEXCLUSIVE)
				|| (_dwMemTotalVideo - vm.depth * vm.height * vm.width / 8) > (12 * 1024 * 1024)/*12 MB*/ )
			{
				_VMList[i] = (RwChar*)RwCalloc(100, sizeof(RwChar));
				rwsprintf(_VMList[i],"%lu X %lu X %lu", vm.width, vm.height, vm.depth);
			}
			else
				_VMList[i] = NULL;
		}
		else
			_VMList[i] = NULL;
	}
	
	return _VMList;
}

/*
 *****************************************************************************
 */
void _psSelectScreenVM(RwInt32 videoMode)
{
	RwTexDictionarySetCurrent( NULL );
	
	FrontEndMenuManager.UnloadTextures();
	
	if ( !_psSetVideoMode(RwEngineGetCurrentSubSystem(), videoMode) )
	{
		RsGlobal.quit = TRUE;
		
		ShowWindow(PSGLOBAL(window), SW_HIDE);

		MessageBoxW(NULL,
			(LPCWSTR)TheText.Get("WIN_RSZ"), // Failed to select new screen resolution
			(LPCWSTR)TheText.Get("WIN_TTL"), // Grand Theft Auto III
			MB_OK);		
	}
	else
		FrontEndMenuManager.LoadAllTextures();
}

/*
 *****************************************************************************
 */
void WaitForState(FILTER_STATE State)
{
	HRESULT hr;
	
	ASSERT(pMC != NULL);
	
	// Make sure we have switched to the required state
	LONG   lfs;
	do
	{
		hr = pMC->GetState(10, &lfs);
	} while (State != lfs);
}

/*
 *****************************************************************************
 */
void HandleGraphEvent(void)
{
	LONG evCode, evParam1, evParam2;
	HRESULT hr=S_OK;
	
	ASSERT(pME != NULL);

	// Process all queued events
	while (SUCCEEDED(pME->GetEvent(&evCode, (LONG_PTR *)&evParam1,
		(LONG_PTR *)&evParam2, 0)))
	{
		// Free memory associated with callback, since we're not using it
		hr = pME->FreeEventParams(evCode, evParam1, evParam2);

		// If this is the end of the clip, reset to beginning
		if (EC_COMPLETE == evCode)
		{
			switch (gGameState)
			{
				case GS_LOGO_MPEG:
				{
					gGameState = GS_INIT_INTRO_MPEG;
					TRACE("gGameState = GS_INIT_INTRO_MPEG");
					break;
				}
				case GS_INTRO_MPEG:
				{
					gGameState = GS_INIT_ONCE;
					TRACE("gGameState = GS_INIT_ONCE");
					break;
				}
				default:
				{
					break;
				}
			}

			pME->SetNotifyWindow((OAHWND)NULL, 0, 0);
		}
	}
}

/*
 *****************************************************************************
 */
 
LRESULT CALLBACK
MainWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINTS points;
	static BOOL noMemory = FALSE;

	
	switch( message )
	{
		case WM_SETCURSOR:
		{
			ShowCursor(FALSE);
			
			SetCursor(NULL);
			
			break; // is this correct ?
		}
		
		case WM_SIZE:
		{
			RwRect r;

			r.x = 0;
			r.y = 0;
			r.w = LOWORD(lParam);
			r.h = HIWORD(lParam);

			if (RwInitialised && r.h > 0 && r.w > 0)
			{
				RsEventHandler(rsCAMERASIZE, &r);

				if (r.w != LOWORD(lParam) && r.h != HIWORD(lParam))
				{
					WINDOWPLACEMENT		wp;

					/* failed to create window of required size */
					noMemory = TRUE;

					/* stop re-sizing */
					ReleaseCapture();

					/* handle maximised window */
					GetWindowPlacement(window, &wp);
					if (wp.showCmd == SW_SHOWMAXIMIZED)
					{
						SendMessage(window, WM_WINDOWPOSCHANGED, 0, 0);
					}
				}
				else
				{
					noMemory = FALSE;
				}

			}

			return 0L;
		}

		case WM_SIZING:
		{
			/* 
			 * Handle event to ensure window contents are displayed during re-size
			 * as this can be disabled by the user, then if there is not enough 
			 * memory things don't work.
			 */
			RECT			   *newPos = (LPRECT) lParam;
			RECT				rect;

			/* redraw window */
			if (RwInitialised && gGameState == GS_PLAYING_GAME)
			{
				RsEventHandler(rsIDLE, (void *)TRUE);
			}

			/* Manually resize window */
			rect.left = rect.top = 0;
			rect.bottom = newPos->bottom - newPos->top;
			rect.right = newPos->right - newPos->left;

			SetWindowPos(window, HWND_TOP, rect.left, rect.top,
						 (rect.right - rect.left),
						 (rect.bottom - rect.top), SWP_NOMOVE);

			return 0L;
		}

		case WM_LBUTTONDOWN:
		{
			SetCapture(window);

			return 0L;
		}

		case WM_RBUTTONDOWN:
		{
			SetCapture(window);

			return 0L;
		}
		
		case WM_MBUTTONDOWN:
		{
			SetCapture(window);

			return 0L;
		}

		case WM_MOUSEWHEEL:
		{
			return 0L;
		}

		case WM_MOUSEMOVE:
		{
			points = MAKEPOINTS(lParam);

			FrontEndMenuManager.m_nMouseTempPosX = points.x;
			FrontEndMenuManager.m_nMouseTempPosY = points.y;

			return 0L;
		}

		case WM_LBUTTONUP:
		{
			ReleaseCapture();

			return 0L;
		}

		case WM_RBUTTONUP:
		{
			ReleaseCapture();

			return 0L;
		}
		
		case WM_MBUTTONUP:
		{
			ReleaseCapture();

			return 0L;
		}

		case WM_KEYDOWN:
		{
			RsKeyCodes ks;
			
			if ( _InputTranslateKey(&ks, lParam, wParam) )
				RsKeyboardEventHandler(rsKEYDOWN, &ks);

			if ( wParam == VK_SHIFT )
				_InputTranslateShiftKeyUpDown(&ks);

			return 0L;
		}

		case WM_KEYUP:
		{
			RsKeyCodes ks;

			if ( _InputTranslateKey(&ks, lParam, wParam) )
				RsKeyboardEventHandler(rsKEYUP, &ks);

			if ( wParam == VK_SHIFT )
				_InputTranslateShiftKeyUpDown(&ks);

			return 0L;
		}

		case WM_SYSKEYDOWN:
		{
			RsKeyCodes ks;
			
			if ( _InputTranslateKey(&ks, lParam, wParam) )
				RsKeyboardEventHandler(rsKEYDOWN, &ks);

			if ( wParam == VK_SHIFT )
				_InputTranslateShiftKeyUpDown(&ks);

			return 0L;
		}

		case WM_SYSKEYUP:
		{
			RsKeyCodes ks;

			if ( _InputTranslateKey(&ks, lParam, wParam) )
				RsKeyboardEventHandler(rsKEYUP, &ks);

			if ( wParam == VK_SHIFT )
				_InputTranslateShiftKeyUpDown(&ks);

			return 0L;
		}

		case WM_ACTIVATEAPP:
		{
			switch ( gGameState )
			{
				case GS_LOGO_MPEG:
				case GS_INTRO_MPEG:
				{
					ASSERT(pMC != NULL);
					
					LONG state;
					pMC->GetState(10, &state);
					
					if ( !(BOOL)wParam ) // losing activation
					{
						if ( state == State_Running && pMC != NULL )
						{
							HRESULT hr = pMC->Pause();

							if (hr == S_FALSE)
								OutputDebugString("Failed to pause the MPEG");
							else
								WaitForState(State_Paused);
						}
					}
					else
					{
						CenterVideo();
						
						if ( state != State_Running && pMC != NULL )
						{
							HRESULT hr = pMC->Run();

							if ( hr == S_FALSE )
								OutputDebugString("Failed to run the MPEG");
							else
							{
								WaitForState(State_Running);
								SetFocus(PSGLOBAL(window));
							}
						}
					}
					
					break;
				}
				
				case GS_START_UP:
				{
					if ( !(BOOL)wParam && PSGLOBAL(fullScreen) ) // losing activation
						startupDeactivate = TRUE;
					
					break;
				}
			}
			
			CPad::GetPad(0)->Clear(false);
			CPad::GetPad(1)->Clear(false);
			
			return 0L;
		}
		
		case WM_TIMER:
		{
			return 0L;
		}
		
		case WM_GRAPHNOTIFY:
		{
			if (gGameState == GS_INTRO_MPEG || gGameState == GS_LOGO_MPEG)
				HandleGraphEvent();

			break;
		}

		case WM_CLOSE:
		case WM_DESTROY:
		{
			/*
			 * Quit message handling.
			 */
			ClipCursor(NULL);
			
			_InputShutdown();

			PostQuitMessage(0);

			return 0L;
		}
		
		case WM_DEVICECHANGE:
		{
			if( wParam == DBT_DEVICEREMOVECOMPLETE )
			{
				PDEV_BROADCAST_HDR pDev = (PDEV_BROADCAST_HDR)lParam;
				
				if (pDev->dbch_devicetype != DBT_DEVTYP_VOLUME)
					break;

				if ( DMAudio.IsAudioInitialised() )
				{
					PDEV_BROADCAST_VOLUME pVol = (PDEV_BROADCAST_VOLUME)pDev;
					if ( pVol->dbcv_flags & DBTF_MEDIA )
					{
						char c = DMAudio.GetCDAudioDriveLetter();
						
						if ( c >= 'A' && pVol->dbcv_unitmask & (1 << (c - 'A')) )
						{
							OutputDebugString("About to check CD drive...");
							
							while ( true )
							{
								FrontEndMenuManager.WaitForUserCD();
								
								if ( !FrontEndMenuManager.m_bQuitGameNoCD )
								{
									if ( DMAudio.CheckForAnAudioFileOnCD() )
									{
										OutputDebugString("GTA3 Audio CD has been inserted");
										break;
									}
								}
								else
								{
									OutputDebugString("Exiting game as Audio CD was not inserted");
									break;
								}
							}
						}
					}
				}
			}
			
			break;
		}

	}

	/*
	 * Let Windows handle all other messages.
	 */
	return DefWindowProc(window, message, wParam, lParam);
}


/*
 *****************************************************************************
 */

RwBool IsForegroundApp()
{
	return !!ForegroundApp;
}

UINT GetBestRefreshRate(UINT width, UINT height, UINT depth)
{
	LPDIRECT3D8 d3d = Direct3DCreate8(D3D_SDK_VERSION);
	
	ASSERT(d3d != NULL);
	
	INT refreshRate = -1;
	D3DFORMAT format;

	if ( depth == 32 )
		format = D3DFMT_X8R8G8B8;
	else if ( depth == 24 )
		format = D3DFMT_R8G8B8;
	else
		format = D3DFMT_R5G6B5;
	
	UINT modeCount = d3d->GetAdapterModeCount(GcurSel);
	
	for ( UINT i = 0; i < modeCount; i++ )
	{
		D3DDISPLAYMODE mode;
		
		d3d->EnumAdapterModes(GcurSel, i, &mode);
		
		if ( mode.Width == width && mode.Height == height && mode.Format == format )
		{
			if ( mode.RefreshRate == 0 )
				return 0;
#pragma warning( push )
#pragma warning( disable : 4018)

			if ( mode.RefreshRate < refreshRate && mode.RefreshRate >= 60 )
				refreshRate = mode.RefreshRate;
#pragma warning( pop ) 
		}
	}
	
#ifdef FIX_BUGS
	d3d->Release();
#endif
	
	if ( refreshRate == -1 )
		return -1;

	return refreshRate;
}

/*
 *****************************************************************************
 */
RwBool
psSelectDevice()
{
	RwVideoMode			vm;
	RwInt32				subSysNum;
	RwInt32				AutoRenderer = 0;
	

	RwBool modeFound = FALSE;
	
	if ( !useDefault )
	{
		GnumSubSystems = RwEngineGetNumSubSystems();
		if ( !GnumSubSystems )
		{
			 return FALSE;
		}
		
		/* Just to be sure ... */
		GnumSubSystems = (GnumSubSystems > MAX_SUBSYSTEMS) ? MAX_SUBSYSTEMS : GnumSubSystems;
		
		/* Get the names of all the sub systems */
		for (subSysNum = 0; subSysNum < GnumSubSystems; subSysNum++)
		{
			RwEngineGetSubSystemInfo(&GsubSysInfo[subSysNum], subSysNum);
		}
		
		/* Get the default selection */
		GcurSel = RwEngineGetCurrentSubSystem();
	}
	
	/* Set the driver to use the correct sub system */
	if (!RwEngineSetSubSystem(GcurSel))
	{
		return FALSE;
	}
	
	
	if ( !useDefault )
	{
		if ( _psGetVideoModeList()[FrontEndMenuManager.m_nDisplayVideoMode] && FrontEndMenuManager.m_nDisplayVideoMode )
		{
			FrontEndMenuManager.m_nPrefsVideoMode = FrontEndMenuManager.m_nDisplayVideoMode;
			GcurSelVM = FrontEndMenuManager.m_nDisplayVideoMode;
		}
		else
		{
			while ( !modeFound && GcurSelVM < RwEngineGetNumVideoModes() )
			{
				RwEngineGetVideoModeInfo(&vm, GcurSelVM);
				if ( defaultFullscreenRes	&& vm.width	 != 640 
											|| vm.height != 480
											|| vm.depth	 != 16
											|| !(vm.flags & rwVIDEOMODEEXCLUSIVE) )
					++GcurSelVM;
				else
					modeFound = TRUE;
			}
			
			if ( !modeFound )
			{
				MessageBox(NULL, "Cannot find 640x480 video mode", "GTA3", MB_OK);
				return FALSE;
			}
		}
	}
	
	RwEngineGetVideoModeInfo(&vm, GcurSelVM);
	
	FrontEndMenuManager.m_nCurrOption = 0;
	
	/* Set up the video mode and set the apps window
	* dimensions to match */
	if (!RwEngineSetVideoMode(GcurSelVM))
	{
		return FALSE;
	}
	
	if (vm.flags & rwVIDEOMODEEXCLUSIVE)
	{
		debug("%dx%dx%d", vm.width, vm.height, vm.depth);
		
		UINT refresh = GetBestRefreshRate(vm.width, vm.height, vm.depth);
		
		if ( refresh != (UINT)-1 )
		{
			debug("refresh %d", refresh);
			RwD3D8EngineSetRefreshRate((RwUInt32)refresh);
		}
	}
	
	if (vm.flags & rwVIDEOMODEEXCLUSIVE)
	{
		RsGlobal.maximumWidth = vm.width;
		RsGlobal.maximumHeight = vm.height;
		RsGlobal.width = vm.width;
		RsGlobal.height = vm.height;
		
		PSGLOBAL(fullScreen) = TRUE;
	}
	
	return TRUE;
}

/*
 *****************************************************************************
 */
RwBool _psSetVideoMode(RwInt32 subSystem, RwInt32 videoMode)
{
	RwInitialised = FALSE;
	
	RsEventHandler(rsRWTERMINATE, NULL);
	
	GcurSel = subSystem;
	GcurSelVM = videoMode;
	
	useDefault = TRUE;
	
	if ( RsEventHandler(rsRWINITIALISE, PSGLOBAL(window)) == rsEVENTERROR )
		return FALSE;
	
	RwInitialised = TRUE;
	useDefault = FALSE;
	
	RwRect r;
	
	r.x = 0;
	r.y = 0;
	r.w = RsGlobal.maximumWidth;
	r.h = RsGlobal.maximumHeight;

	RsEventHandler(rsCAMERASIZE, &r);
	
	return TRUE;
}
 
 
/*
 *****************************************************************************
 */
static RwChar **
CommandLineToArgv(RwChar *cmdLine, RwInt32 *argCount)
{
	RwInt32 numArgs = 0;
	RwBool inArg, inString;
	RwInt32 i, len;
	RwChar *res, *str, **aptr;

	len = strlen(cmdLine);

	/* 
	 * Count the number of arguments...
	 */
	inString = FALSE;
	inArg = FALSE;

	for(i=0; i<=len; i++)
	{
		if( cmdLine[i] == '"' )
		{
			inString = !inString;
		}

		if( (cmdLine[i] <= ' ' && !inString) || i == len )
		{
			if( inArg ) 
			{
				inArg = FALSE;
				
				numArgs++;
			}
		} 
		else if( !inArg )
		{
			inArg = TRUE;
		}
	}

	/* 
	 * Allocate memory for result...
	 */
	res = (RwChar *)malloc(sizeof(RwChar *) * numArgs + len + 1);
	str = res + sizeof(RwChar *) * numArgs;
	aptr = (RwChar **)res;

	strcpy(str, cmdLine);

	/*
	 * Walk through cmdLine again this time setting pointer to each arg...
	 */
	inArg = FALSE;
	inString = FALSE;

	for(i=0; i<=len; i++)
	{
		if( cmdLine[i] == '"' )
		{
			inString = !inString;
		}

		if( (cmdLine[i] <= ' ' && !inString) || i == len )
		{
			if( inArg ) 
			{
				if( str[i-1] == '"' )
				{
					str[i-1] = '\0';
				}
				else
				{
					str[i] = '\0';
				}
				
				inArg = FALSE;
			}
		} 
		else if( !inArg && cmdLine[i] != '"' )
		{
			inArg = TRUE; 
			
			*aptr++ = &str[i];
		}
	}

	*argCount = numArgs;

	return (RwChar **)res;
}

/*
 *****************************************************************************
 */
void InitialiseLanguage()
{
#pragma warning( push )
#pragma warning( disable : 4302)
	WORD primUserLCID	= PRIMARYLANGID(GetSystemDefaultLCID());
	WORD primSystemLCID = PRIMARYLANGID(GetUserDefaultLCID());
	WORD primLayout		= PRIMARYLANGID(GetKeyboardLayout(0));
	
	WORD subUserLCID	= SUBLANGID(GetSystemDefaultLCID());
	WORD subSystemLCID	= SUBLANGID(GetUserDefaultLCID());
	WORD subLayout		= SUBLANGID(GetKeyboardLayout(0));
#pragma warning( pop ) 
	
	if (   primUserLCID	  == LANG_GERMAN
		|| primSystemLCID == LANG_GERMAN
		|| primLayout	  == LANG_GERMAN )
	{
		CGame::nastyGame = false;
		CMenuManager::m_PrefsAllowNastyGame = false;
		CGame::germanGame = true;
	}
	
	if (   primUserLCID	  == LANG_FRENCH
		|| primSystemLCID == LANG_FRENCH
		|| primLayout	  == LANG_FRENCH )
	{
		CGame::nastyGame = false;
		CMenuManager::m_PrefsAllowNastyGame = false;
		CGame::frenchGame = true;
	}
	
	if (   subUserLCID	 == SUBLANG_ENGLISH_AUS
		|| subSystemLCID == SUBLANG_ENGLISH_AUS
		|| subLayout	 == SUBLANG_ENGLISH_AUS )
		CGame::noProstitutes = true;
	
	int32 lang;
	
	switch ( primSystemLCID )
	{
		case LANG_GERMAN:
		{
			lang = LANG_GERMAN;
			break;
		}
		case LANG_FRENCH:
		{
			lang = LANG_FRENCH;
			break;
		}
		case LANG_SPANISH:
		{
			lang = LANG_SPANISH;
			break;
		}
		case LANG_ITALIAN:
		{
			lang = LANG_ITALIAN;
			break;
		}
		default:
		{
			lang = ( subSystemLCID == SUBLANG_ENGLISH_AUS ) ? -99 : LANG_ENGLISH;
			break;
		}
	}
	
	CMenuManager::OS_Language = primUserLCID;

	switch ( lang )
	{
		case LANG_GERMAN:
		{
			CMenuManager::m_PrefsLanguage = LANGUAGE_GERMAN;
			break;
		}
		case LANG_SPANISH:
		{
			CMenuManager::m_PrefsLanguage = LANGUAGE_SPANISH;
			break;
		}
		case LANG_FRENCH:
		{
			CMenuManager::m_PrefsLanguage = LANGUAGE_FRENCH;
			break;
		}
		case LANG_ITALIAN:
		{
			CMenuManager::m_PrefsLanguage = LANGUAGE_ITALIAN;
			break;
		}
		default:
		{
			CMenuManager::m_PrefsLanguage = LANGUAGE_AMERICAN;
			break;
		}
	}

	TheText.Unload();
	TheText.Load();
}

/*
 *****************************************************************************
 */
void CenterVideo(void)
{
	HRESULT hr = S_OK;
	RECT rect;

	ASSERT(pVW != NULL);
	
	GetClientRect(PSGLOBAL(window), &rect);

	JIF(pVW->SetWindowPosition(rect.left, rect.top, rect.right, rect.bottom));

	JIF(pVW->put_MessageDrain((OAHWND) PSGLOBAL(window)));

	SetFocus(PSGLOBAL(window));
}

/*
 *****************************************************************************
 */
void PlayMovieInWindow(int cmdShow, LPTSTR szFile)
{
	WCHAR wFileName[256];
	HRESULT hr;

	// Clear open dialog remnants before calling RenderFile()
	UpdateWindow(PSGLOBAL(window));

	// Convert filename to wide character string
	MultiByteToWideChar(CP_ACP, 0, szFile, -1, wFileName, sizeof(wFileName) - 1);

	// Initialize COM
	JIF(CoInitialize(NULL));

	// Get the interface for DirectShow's GraphBuilder
	JIF(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, 
						 IID_IGraphBuilder, (void **)&pGB));

	if(SUCCEEDED(hr))
	{
		// Have the graph builder construct its the appropriate graph automatically
		JIF(pGB->RenderFile(&wFileName[0], NULL));

		// QueryInterface for DirectShow interfaces
		JIF(pGB->QueryInterface(IID_IMediaControl, (void **)&pMC));
		JIF(pGB->QueryInterface(IID_IMediaEventEx, (void **)&pME));
		JIF(pGB->QueryInterface(IID_IMediaSeeking, (void **)&pMS));

		// Query for video interfaces, which may not be relevant for audio files
		JIF(pGB->QueryInterface(IID_IVideoWindow, (void **)&pVW));

		JIF(pVW->put_Owner((OAHWND) PSGLOBAL(window)));
		JIF(pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN));

		// Have the graph signal event via window callbacks for performance
		JIF(pME->SetNotifyWindow((OAHWND)PSGLOBAL(window), WM_GRAPHNOTIFY, 0));

		CenterVideo();

		// Run the graph to play the media file
		JIF(pMC->Run());

		SetFocus(PSGLOBAL(window));
	}
	
	ASSERT(pGB != NULL);
	ASSERT(pVW != NULL);
	ASSERT(pME != NULL);
	ASSERT(pMC != NULL);

	if(FAILED(hr))
		CloseClip();
}

/*
 *****************************************************************************
 */
void CloseInterfaces(void)
{
	// Release and zero DirectShow interfaces
	SAFE_RELEASE(pME);
	SAFE_RELEASE(pMS);
	SAFE_RELEASE(pMC);
	SAFE_RELEASE(pVW);
	SAFE_RELEASE(pGB);
}

/*
 *****************************************************************************
 */
void CloseClip(void)
{
	HRESULT hr;

	// Stop playback
	if(pMC)
		hr = pMC->Stop();

	// Free DirectShow interfaces
	CloseInterfaces();
}

/*
 *****************************************************************************
 */
void HandleExit()
{
	MSG message;
	while ( PeekMessage(&message, NULL, 0U, 0U, PM_REMOVE|PM_NOYIELD) )
	{
		if( message.message == WM_QUIT )
		{
			RsGlobal.quit = TRUE;
		}
		else
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
}
 
/*
 *****************************************************************************
 */
int PASCAL
_WinMain(HINSTANCE instance, 
		HINSTANCE prevInstance	__RWUNUSED__, 
		CMDSTR cmdLine, 
		int cmdShow)
{
	MSG message;
	RwV2d pos;
	RwInt32 argc, i;
	RwChar **argv;

	SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, NULL, SPIF_SENDCHANGE);

	/* 
	 * Initialize the platform independent data.
	 * This will in turn initialize the platform specific data...
	 */
	if( RsEventHandler(rsINITIALISE, NULL) == rsEVENTERROR )
	{
		return FALSE;
	}

	/*
	 * Register the window class...
	 */
	if( !InitApplication(instance) )
	{
		return FALSE;
	}

	/*
	 * Get proper command line params, cmdLine passed to us does not
	 * work properly under all circumstances...
	 */
	cmdLine = GetCommandLine();

	/*
	 * Parse command line into standard (argv, argc) parameters...
	 */
	argv = CommandLineToArgv(cmdLine, &argc);


	/* 
	 * Parse command line parameters (except program name) one at 
	 * a time BEFORE RenderWare initialization...
	 */
	for(i=1; i<argc; i++)
	{
		RsEventHandler(rsPREINITCOMMANDLINE, argv[i]);
	}

	/*
	 * Create the window...
	 */
	PSGLOBAL(window) = InitInstance(instance);
	if( PSGLOBAL(window) == NULL )
	{
		return FALSE;
	}

	PSGLOBAL(instance) = instance;
	
	ControlsManager.MakeControllerActionsBlank();
	ControlsManager.InitDefaultControlConfiguration();
	
	if ( _InputInitialise() == S_OK )
	{
		_InputInitialiseMouse();
		_InputInitialiseJoys();
	}
	
	ControlsManager.InitDefaultControlConfigMouse(MousePointerStateHelper.GetMouseSetUp());
	
	SetWindowLong(PSGLOBAL(window), GWL_STYLE, WS_POPUP);
	SetWindowPos(PSGLOBAL(window), 0, 0, 0, 0, 0,
				SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|
				SWP_FRAMECHANGED);

	/* 
	 * Initialize the 3D (RenderWare) components of the app...
	 */
	if( rsEVENTERROR == RsEventHandler(rsRWINITIALISE, PSGLOBAL(window)) )
	{
		DestroyWindow(PSGLOBAL(window));

		RsEventHandler(rsTERMINATE, NULL);

		return FALSE;
	}

	/* 
	 * Parse command line parameters (except program name) one at 
	 * a time AFTER RenderWare initialization...
	 */
	for(i=1; i<argc; i++)
	{
		RsEventHandler(rsCOMMANDLINE, argv[i]);
	}

	/* 
	 * Force a camera resize event...
	 */
	{
		RwRect r;

		r.x = 0;
		r.y = 0;
		r.w = RsGlobal.maximumWidth;
		r.h = RsGlobal.maximumHeight;

		RsEventHandler(rsCAMERASIZE, &r);
	}
	
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETPOWEROFFACTIVE, FALSE, NULL, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETLOWPOWERACTIVE, FALSE, NULL, SPIF_SENDCHANGE);
	

	STICKYKEYS SavedStickyKeys;
	SavedStickyKeys.cbSize = sizeof(STICKYKEYS);
	
	SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &SavedStickyKeys, SPIF_SENDCHANGE);
	
	STICKYKEYS NewStickyKeys;
	NewStickyKeys.cbSize = sizeof(STICKYKEYS);
	NewStickyKeys.dwFlags = SKF_TWOKEYSOFF;
	
	SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &NewStickyKeys, SPIF_SENDCHANGE);
	

	/*
	 * Show the window, and refresh it...
	 */
	ShowWindow(PSGLOBAL(window), cmdShow);
	UpdateWindow(PSGLOBAL(window));
	
	{
		CFileMgr::SetDirMyDocuments();
		
		int32 gta3set = CFileMgr::OpenFile("gta3.set", "r");
		
		if ( gta3set )
		{
			ControlsManager.LoadSettings(gta3set);
			CFileMgr::CloseFile(gta3set);
		}
		
		CFileMgr::SetDir("");
	}
	
	SetErrorMode(SEM_FAILCRITICALERRORS);


#ifdef NO_MOVIES
	gGameState = GS_INIT_FRONTEND;
	TRACE("gGameState = GS_INIT_FRONTEND");
	
	LoadingScreen(NULL, NULL, "loadsc0");
	if ( !CGame::InitialiseOnceAfterRW() )
		RsGlobal.quit = TRUE;
#endif				
						
	
	while ( TRUE )
	{
		RwInitialised = TRUE;
		
		/* 
		* Set the initial mouse position...
		*/
		pos.x = RsGlobal.maximumWidth * 0.5f;
		pos.y = RsGlobal.maximumHeight * 0.5f;

		RsMouseSetPos(&pos);
		
		WINDOWPLACEMENT		wp;
		wp.length = sizeof(WINDOWPLACEMENT);

		/*
		* Enter the message processing loop...
		*/

		while( !RsGlobal.quit && !FrontEndMenuManager.m_bStartGameLoading )
		{
			if( PeekMessage(&message, NULL, 0U, 0U, PM_REMOVE|PM_NOYIELD) )
			{
				if( message.message == WM_QUIT )
				{
					break;
				}
				else
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}
			else if( ForegroundApp )
			{
				switch ( gGameState )
				{
					case GS_START_UP:
					{
						gGameState = GS_INIT_LOGO_MPEG;
						TRACE("gGameState = GS_INIT_LOGO_MPEG");
						break;
					}
					
					case GS_INIT_LOGO_MPEG:
					{
						if ( !startupDeactivate )
							PlayMovieInWindow(cmdShow, "movies\\Logo.mpg");
						gGameState = GS_LOGO_MPEG;
						TRACE("gGameState = GS_LOGO_MPEG;");
						break;
					}
					
					case GS_LOGO_MPEG:
					{
						CPad::UpdatePads();

						if ( startupDeactivate || ControlsManager.GetJoyButtonJustDown() != 0 )
							++gGameState;
						else if ( CPad::GetPad(0)->GetLeftMouseJustDown() )
							++gGameState;
						else if ( CPad::GetPad(0)->GetPadEnterJustDown() || CPad::GetPad(0)->GetEnterJustDown() )
							++gGameState;
						else if ( CPad::GetPad(0)->GetCharJustDown(' ') )
							++gGameState;
						else if ( CPad::GetPad(0)->GetLeftAltJustDown() || CPad::GetPad(0)->GetRightAltJustDown() )
							++gGameState;
						else if ( CPad::GetPad(0)->GetTabJustDown() )
							++gGameState;

						break;
					}
					
					case GS_INIT_INTRO_MPEG:
					{
						CloseClip();
						
						CoUninitialize();
						
						if ( CMenuManager::OS_Language == LANG_FRENCH || CMenuManager::OS_Language == LANG_GERMAN )
							PlayMovieInWindow(cmdShow, "movies\\GTAtitlesGER.mpg");
						else
							PlayMovieInWindow(cmdShow, "movies\\GTAtitles.mpg");
						
						gGameState = GS_INTRO_MPEG;
						TRACE("gGameState = GS_INTRO_MPEG;");
						break;
					}
					
					case GS_INTRO_MPEG:
					{
						CPad::UpdatePads();

						if ( startupDeactivate || ControlsManager.GetJoyButtonJustDown() != 0 )
							++gGameState;
						else if ( CPad::GetPad(0)->GetLeftMouseJustDown() )
							++gGameState;
						else if ( CPad::GetPad(0)->GetPadEnterJustDown() || CPad::GetPad(0)->GetEnterJustDown() )
							++gGameState;
						else if ( CPad::GetPad(0)->GetCharJustDown(' ') )
							++gGameState;
						else if ( CPad::GetPad(0)->GetLeftAltJustDown() || CPad::GetPad(0)->GetRightAltJustDown() )
							++gGameState;
						else if ( CPad::GetPad(0)->GetTabJustDown() )
							++gGameState;

						break;
					}
					
					case GS_INIT_ONCE:
					{
						CloseClip();
						CoUninitialize();
						
						LoadingScreen(NULL, NULL, "loadsc0");
						
						if ( !CGame::InitialiseOnceAfterRW() )
							RsGlobal.quit = TRUE;
						
						gGameState = GS_INIT_FRONTEND;
						TRACE("gGameState = GS_INIT_FRONTEND;");
						break;
					}
					
					case GS_INIT_FRONTEND:
					{
						LoadingScreen(NULL, NULL, "loadsc0");
						
						FrontEndMenuManager.m_bGameNotLoaded = true;
						
						CMenuManager::m_bStartUpFrontEndRequested = true;
						
						if ( defaultFullscreenRes )
						{
							defaultFullscreenRes = FALSE;
							FrontEndMenuManager.m_nPrefsVideoMode = GcurSelVM;
							FrontEndMenuManager.m_nDisplayVideoMode = GcurSelVM;
						}
						
						gGameState = GS_FRONTEND;
						TRACE("gGameState = GS_FRONTEND;");
						break;
					}
					
					case GS_FRONTEND:
					{
						GetWindowPlacement(PSGLOBAL(window), &wp);
						
						if ( wp.showCmd != SW_SHOWMINIMIZED )
							RsEventHandler(rsFRONTENDIDLE, NULL);

						if ( !FrontEndMenuManager.m_bMenuActive || FrontEndMenuManager.m_bLoadingSavedGame )
						{
							gGameState = GS_INIT_PLAYING_GAME;
							TRACE("gGameState = GS_INIT_PLAYING_GAME;");
						}

						if ( FrontEndMenuManager.m_bLoadingSavedGame )
						{
							InitialiseGame();
							FrontEndMenuManager.m_bGameNotLoaded = false;
							gGameState = GS_PLAYING_GAME;
							TRACE("gGameState = GS_PLAYING_GAME;");
						}
						break;
					}
					
					case GS_INIT_PLAYING_GAME:
					{
						InitialiseGame();
						FrontEndMenuManager.m_bGameNotLoaded = false;
						gGameState = GS_PLAYING_GAME;
						TRACE("gGameState = GS_PLAYING_GAME;");
						break;
					}
					
					case GS_PLAYING_GAME:
					{
						float ms = (float)CTimer::GetCurrentTimeInCycles() / (float)CTimer::GetCyclesPerMillisecond();
						if ( RwInitialised )
						{
							if (!CMenuManager::m_PrefsFrameLimiter || (1000.0f / (float)RsGlobal.maxFPS) < ms)
								RsEventHandler(rsIDLE, (void *)TRUE);
						}
						break;
					}
				}
			}
			else
			{
				if ( RwCameraBeginUpdate(Scene.camera) )
				{
					RwCameraEndUpdate(Scene.camera);
					ForegroundApp = TRUE;
					RsEventHandler(rsACTIVATE, (void *)TRUE);
				}
				
				WaitMessage();
			}
		}

		
		/* 
		* About to shut down - block resize events again...
		*/
		RwInitialised = FALSE;
		
		FrontEndMenuManager.UnloadTextures();
		if ( !FrontEndMenuManager.m_bStartGameLoading )
			break;
		
		CPad::ResetCheats();
		CPad::StopPadsShaking();
		
		DMAudio.ChangeMusicMode(_TODOCONST(3));
		
		CTimer::Stop();
		
		if ( FrontEndMenuManager.m_bLoadingSavedGame )
		{
			CGame::ShutDownForRestart();
			CGame::InitialiseWhenRestarting();
			DMAudio.ChangeMusicMode(_TODOCONST(1));
			LoadSplash(GetLevelSplashScreen(CGame::currLevel));
			FrontEndMenuManager.m_bLoadingSavedGame = false;
		}
		else
		{
			if ( gGameState == GS_PLAYING_GAME )
				CGame::ShutDown();
			
			CTimer::Stop();
			
			if ( FrontEndMenuManager.m_bFirstTime == true )
			{
				gGameState = GS_INIT_FRONTEND;
				TRACE("gGameState = GS_INIT_FRONTEND;");
			}
			else
			{
				gGameState = GS_INIT_PLAYING_GAME;
				TRACE("gGameState = GS_INIT_PLAYING_GAME;");
			}
		}
		
		FrontEndMenuManager.m_bFirstTime = false;
		FrontEndMenuManager.m_bStartGameLoading = false;
	}
	

	if ( gGameState == GS_PLAYING_GAME )
		CGame::ShutDown();

	DMAudio.Terminate();
	
	_psFreeVideoModeList();


	/*
	 * Tidy up the 3D (RenderWare) components of the application...
	 */
	RsEventHandler(rsRWTERMINATE, NULL);

	/*
	 * Kill the window...
	 */
	DestroyWindow(PSGLOBAL(window));

	/*
	 * Free the platform dependent data...
	 */
	RsEventHandler(rsTERMINATE, NULL);

	/* 
	 * Free the argv strings...
	 */
	free(argv);
	
	ShowCursor(TRUE);
	
	SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &SavedStickyKeys, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETPOWEROFFACTIVE, TRUE, NULL, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETLOWPOWERACTIVE, TRUE, NULL, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, SPIF_SENDCHANGE);

	SetErrorMode(0);

	return message.wParam;
}

/*
 *****************************************************************************
 */

#define DEVICE_AXIS_MIN -2000
#define DEVICE_AXIS_MAX 2000


HRESULT _InputInitialise()
{
	HRESULT hr;

	// Create a DInput object
	if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
										IID_IDirectInput8, (VOID**)&PSGLOBAL(dinterface), NULL ) ) )
		return hr;
		
	return S_OK;
}

HRESULT _InputInitialiseMouse()
{
	HRESULT hr;

	// Obtain an interface to the system mouse device.
	if( FAILED( hr = PSGLOBAL(dinterface)->CreateDevice( GUID_SysMouse, &PSGLOBAL(mouse), NULL ) ) )
		return hr;
	
	// Set the data format to "mouse format" - a predefined data format 
	//
	// A data format specifies which controls on a device we
	// are interested in, and how they should be reported.
	//
	// This tells DirectInput that we will be passing a
	// DIMOUSESTATE2 structure to IDirectInputDevice::GetDeviceState.
	if( FAILED( hr = PSGLOBAL(mouse)->SetDataFormat( &c_dfDIMouse2 ) ) )
		return hr;
	
	if( FAILED( hr = PSGLOBAL(mouse)->SetCooperativeLevel( PSGLOBAL(window), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND ) ) )
		return hr;
	
	// Acquire the newly created device
	PSGLOBAL(mouse)->Acquire();
	
	return S_OK;
}

//#define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)
//#define FAILED(Status) ((HRESULT)(Status)<0)

RwV2d leftStickPos;
RwV2d rightStickPos;

HRESULT CapturePad(RwInt32 padID)
{
	HRESULT		hr;
	DIJOYSTATE2 js; 
	LPDIRECTINPUTDEVICE8 pPad = NULL;
	
	pPad = ( padID == 0 ) ? PSGLOBAL(joy1) : PSGLOBAL(joy2);
	
	if ( NULL == pPad )
		return S_OK;
	
	// Poll the device to read the current state
	hr = pPad->Poll();
	
	if( FAILED(hr) )
	{
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = pPad->Acquire();
		while( hr == DIERR_INPUTLOST ) 
			hr = pPad->Acquire();

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.	 This
		// may occur when the app is minimized or in the process of 
		// switching, so just try again later 
		
		if( FAILED(hr) )
			return hr; 
		
		hr = pPad->Poll();
		if( FAILED(hr) )
			return hr; 
	}
	
	// Get the input's device state
	if( FAILED( hr = pPad->GetDeviceState( sizeof(DIJOYSTATE2), &js ) ) )
		return hr; // The device should have been acquired during the Poll()
	
	if ( ControlsManager.field_0 == true )
	{
		memcpy(&ControlsManager.m_OldState, &js, sizeof(DIJOYSTATE2));
		memcpy(&ControlsManager.m_NewState, &js, sizeof(DIJOYSTATE2));
		
		ControlsManager.field_0 = false;
	}
	else
	{
		memcpy(&ControlsManager.m_OldState, &ControlsManager.m_NewState, sizeof(DIJOYSTATE2));
		memcpy(&ControlsManager.m_NewState, &js,						 sizeof(DIJOYSTATE2));
	}

	RsPadButtonStatus bs;
	bs.padID = padID;

	RsPadEventHandler(rsPADBUTTONUP, (void *)&bs);
	
	bool deviceAvailable = pPad != NULL;
	
	if ( deviceAvailable )
	{
		leftStickPos.x = (float)js.lX / (float)((DEVICE_AXIS_MAX - DEVICE_AXIS_MIN) / 2);
		leftStickPos.y = (float)js.lY / (float)((DEVICE_AXIS_MAX - DEVICE_AXIS_MIN) / 2);
		
		if (LOWORD(js.rgdwPOV[0]) != -1)
		{
			float angle = DEGTORAD((float)js.rgdwPOV[0] / 100.0f);

			leftStickPos.x = sin(angle);
			leftStickPos.y = -cos(angle);
		}
		
		if ( AllValidWinJoys.m_aJoys[bs.padID].m_bHasAxisR && AllValidWinJoys.m_aJoys[bs.padID].m_bHasAxisZ )
		{
			rightStickPos.x = (float)js.lZ	/ (float)((DEVICE_AXIS_MAX - DEVICE_AXIS_MIN) / 2);
			rightStickPos.y = (float)js.lRz / (float)((DEVICE_AXIS_MAX - DEVICE_AXIS_MIN) / 2);
		}
	}
	
	{
		if (CPad::m_bMapPadOneToPadTwo)
			bs.padID = 1;
		
		RsPadEventHandler(rsPADBUTTONUP,   (void *)&bs);
		RsPadEventHandler(rsPADBUTTONDOWN, (void *)&bs);
	}
	
	{
		if (CPad::m_bMapPadOneToPadTwo)
			bs.padID = 1;
		
		CPad *pad = CPad::GetPad(bs.padID);

		if ( fabs(leftStickPos.x)  > 0.3f )
			pad->PCTempJoyState.LeftStickX	= (int32)(leftStickPos.x  * 128.0f);
		
		if ( fabs(leftStickPos.y)  > 0.3f )
			pad->PCTempJoyState.LeftStickY	= (int32)(leftStickPos.y  * 128.0f);
		
		if ( fabs(rightStickPos.x) > 0.3f )
			pad->PCTempJoyState.RightStickX = (int32)(rightStickPos.x * 128.0f);

		if ( fabs(rightStickPos.y) > 0.3f )
			pad->PCTempJoyState.RightStickY = (int32)(rightStickPos.y * 128.0f);
	}
	
	return S_OK;
}

void _InputInitialiseJoys()
{
	DIPROPDWORD prop;
	DIDEVCAPS devCaps;

	for ( int32 i = 0; i < _TODOCONST(2); i++ )
		AllValidWinJoys.ClearJoyInfo(i);
	
	_InputAddJoys();
	
	if ( PSGLOBAL(joy1) != NULL )
	{
		devCaps.dwSize = sizeof(DIDEVCAPS);
		PSGLOBAL(joy1)->GetCapabilities(&devCaps);

		prop.diph.dwSize = sizeof(DIPROPDWORD);
		prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		prop.diph.dwObj = 0;
		prop.diph.dwHow = 0;
		
		PSGLOBAL(joy1)->GetProperty(DIPROP_VIDPID, (LPDIPROPHEADER)&prop);
		AllValidWinJoys.m_aJoys[0].m_nVendorID = LOWORD(prop.dwData);
		AllValidWinJoys.m_aJoys[0].m_nProductID = HIWORD(prop.dwData);
		AllValidWinJoys.m_aJoys[0].m_bInitialised = true;
		
		ControlsManager.InitDefaultControlConfigJoyPad(devCaps.dwButtons);
	}
		
	if ( PSGLOBAL(joy2) != NULL )
	{
		PSGLOBAL(joy2)->GetProperty(DIPROP_VIDPID, (LPDIPROPHEADER)&prop);
		AllValidWinJoys.m_aJoys[1].m_nVendorID = LOWORD(prop.dwData);
		AllValidWinJoys.m_aJoys[1].m_nProductID = HIWORD(prop.dwData);
		AllValidWinJoys.m_aJoys[1].m_bInitialised = true;
	}
}

#pragma warning( push )
#pragma warning( disable : 4700)
HRESULT _InputAddJoyStick(LPDIRECTINPUTDEVICE8 lpDevice, INT num)
{
	HRESULT hr;

	DIDEVICEOBJECTINSTANCE objInst;
	
	objInst.dwSize = sizeof( DIDEVICEOBJECTINSTANCE );

	DIPROPRANGE range;
	range.diph.dwSize = sizeof(DIPROPRANGE);
	range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	range.lMin = DEVICE_AXIS_MIN;
	range.lMax = DEVICE_AXIS_MAX;
	range.diph.dwHow = DIPH_BYOFFSET;

	// get the info about the object from the device

	range.diph.dwObj = DIJOFS_X;
	if ( lpDevice != NULL )
	{
		if ( SUCCEEDED( lpDevice->GetObjectInfo( &objInst,	DIJOFS_X, DIPH_BYOFFSET ) ) )
		{
			if( FAILED( lpDevice->SetProperty( DIPROP_RANGE, (LPCDIPROPHEADER)&range ) ) ) 
				return S_FALSE;
			else
				;
		}
	}
	
	range.diph.dwObj = DIJOFS_Y;
	if ( lpDevice != NULL )
	{
		if ( SUCCEEDED( lpDevice->GetObjectInfo( &objInst,	DIJOFS_Y, DIPH_BYOFFSET ) ) )
		{
			if( FAILED( lpDevice->SetProperty( DIPROP_RANGE, (LPCDIPROPHEADER)&range ) ) ) 
				return S_FALSE;
			else
				;
		}
	}
	
	range.diph.dwObj = DIJOFS_Z;
	if ( lpDevice != NULL )
	{
		if ( SUCCEEDED( lpDevice->GetObjectInfo( &objInst,	DIJOFS_Z, DIPH_BYOFFSET ) ) )
		{
			if( FAILED( lpDevice->SetProperty( DIPROP_RANGE, (LPCDIPROPHEADER)&range ) ) ) 
				return S_FALSE;
			else
				AllValidWinJoys.m_aJoys[num].m_bHasAxisZ = true; // z rightStickPos.x
		}
	}
	
	range.diph.dwObj = DIJOFS_RZ;
	if ( lpDevice != NULL )
	{
		if ( SUCCEEDED( lpDevice->GetObjectInfo( &objInst,	DIJOFS_RZ, DIPH_BYOFFSET ) ) )
		{
			if( FAILED( lpDevice->SetProperty( DIPROP_RANGE, (LPCDIPROPHEADER)&range ) ) ) 
				return S_FALSE;
			else
				AllValidWinJoys.m_aJoys[num].m_bHasAxisR = true; // r rightStickPos.y
		}
	}
	
	return hr;
}
#pragma warning( pop )

HRESULT _InputAddJoys()
{
	HRESULT hr;
	
	hr = PSGLOBAL(dinterface)->EnumDevices(DI8DEVCLASS_GAMECTRL,  _InputEnumDevicesCallback, NULL, DIEDFL_ATTACHEDONLY );
	 
	if( FAILED(hr) )
		return hr;
	
	if ( PSGLOBAL(joy1) == NULL )
		return S_FALSE;
	
	_InputAddJoyStick(PSGLOBAL(joy1), 0);
	
	if ( PSGLOBAL(joy2) == NULL )
		return S_OK;	// we have one device already so return OK and ignore second
	
	_InputAddJoyStick(PSGLOBAL(joy2), 1);
	
	return S_OK;
}

HRESULT _InputGetMouseState(DIMOUSESTATE2 *state)
{
	HRESULT		  hr;
	
	if ( PSGLOBAL(mouse) == NULL )
		return S_FALSE;
	
	// Get the input's device state, and put the state in dims
	ZeroMemory( state, sizeof(DIMOUSESTATE2) );
	
	hr = PSGLOBAL(mouse)->GetDeviceState( sizeof(DIMOUSESTATE2), state );

	if( FAILED(hr) ) 
	{
		 // DirectInput may be telling us that the input stream has been
		// interrupted.	 We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done.
		// We just re-acquire and try again.
		
		// If input is lost then acquire and keep trying 
		hr = PSGLOBAL(mouse)->Acquire();
		while( hr == DIERR_INPUTLOST ) 
			hr = PSGLOBAL(mouse)->Acquire();
		
		ZeroMemory( state, sizeof(DIMOUSESTATE2) );
		hr = PSGLOBAL(mouse)->GetDeviceState( sizeof(DIMOUSESTATE2), state );
		
		return hr;
	}
	
	return S_OK;
}

void _InputShutdown()
{
	SAFE_RELEASE(PSGLOBAL(dinterface));
}

BOOL CALLBACK _InputEnumDevicesCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
	HRESULT hr;
	 
	static INT Count = 0;
	
	LPDIRECTINPUTDEVICE8 pJoystick = NULL;
	
	if ( Count == 0 )
		pJoystick = PSGLOBAL(joy1);
	else if ( Count == 1 )
		pJoystick = PSGLOBAL(joy2);
	
	// Obtain an interface to the enumerated joystick. 
	hr = PSGLOBAL(dinterface)->CreateDevice( pdidInstance->guidInstance, &pJoystick, NULL );
	
	// If it failed, then we can't use this joystick. (Maybe the user unplugged
	// it while we were in the middle of enumerating it.)
	if( FAILED(hr) ) 
		return DIENUM_CONTINUE;
	
	if( FAILED( hr = pJoystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
	{
		pJoystick->Release();
		return DIENUM_CONTINUE;
	}
	
	++Count;
	
	if( FAILED( hr = pJoystick->SetCooperativeLevel( PSGLOBAL(window), DISCL_NONEXCLUSIVE) ) )
	{
		pJoystick->Release();
		return DIENUM_CONTINUE;
	}
	
	if ( Count == 2 )
		return DIENUM_STOP;
	
	// Stop enumeration. Note: we're just taking the first joystick we get. You
	// could store all the enumerated joysticks and let the user pick.
	return DIENUM_CONTINUE;
}

BOOL _InputTranslateKey(RsKeyCodes *rs, UINT flag, UINT key)
{
	*rs = rsNULL;
	
	switch ( key )
	{
		case VK_SHIFT:
		{
			if ( _dwOperatingSystemVersion == OS_WIN98 )
				*rs = rsSHIFT;
			break;
		}
		
		case VK_RETURN:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsPADENTER;
			else
				*rs = rsENTER;
			break;
		}
		
		case VK_CONTROL:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsRCTRL;
			else
				*rs = rsLCTRL;
			break;
		}
		
		case VK_MENU:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsRALT;
			else
				*rs = rsLALT;
			break;
		}
		
		case VK_APPS:
		{
			*rs = rsAPPS;
			break;
		}
		
		case VK_PAUSE:
		{
			*rs = rsPAUSE;
			break;
		}
		
		case VK_CAPITAL:
		{
			*rs = rsCAPSLK;
			break;
		}
		
		case VK_ESCAPE:
		{
			*rs = rsESC;
			break;
		}
		
		case VK_PRIOR:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsPGUP;
			else
				*rs = rsPADPGUP;
			break;
		}
		
		case VK_NEXT:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsPGDN;
			else
				*rs = rsPADPGDN;
			break;
		}
		
		case VK_END:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsEND;
			else
				*rs = rsPADEND;
			break;
		}
		
		case VK_HOME:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsHOME;
			else
				*rs = rsPADHOME;
			break;
		}
		
		case VK_LEFT:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsLEFT;
			else
				*rs = rsPADLEFT;
			break;
		}
		
		case VK_UP:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsUP;
			else
				*rs = rsPADUP;
			break;
		}
		
		case VK_RIGHT:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsRIGHT;
			else
				*rs = rsPADRIGHT;
			break;
		}
		
		case VK_DOWN:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsDOWN;
			else
				*rs = rsPADDOWN;
			break;
		}
		
		case VK_INSERT:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsINS;
			else
				*rs = rsPADINS;
			break;
		}
		
		case VK_DELETE:
		{
			if ( _InputIsExtended(flag) )
				*rs = rsDEL;
			else
				*rs = rsPADDEL;
			break;
		}
		
		case VK_LWIN:
		{
			*rs = rsLWIN;
			break;
		}
		
		case VK_RWIN:
		{
			*rs = rsRWIN;
			break;
		}
		
		case VK_NUMPAD0:
		{
			*rs = rsPADINS;
			break;
		}
		
		case VK_NUMPAD1:
		{
			*rs = rsPADEND;
			break;
		}
		
		case VK_NUMPAD2:
		{
			*rs = rsPADDOWN;
			break;
		}
		
		case VK_NUMPAD3:
		{
			*rs = rsPADPGDN;
			break;
		}
		
		case VK_NUMPAD4:
		{
			*rs = rsPADLEFT;
			break;
		}
		
		case VK_NUMPAD5:
		{
			*rs = rsPAD5;
			break;
		}
		
		case VK_NUMPAD6:
		{
			*rs = rsPADRIGHT;
			break;
		}
		
		case VK_NUMPAD7:
		{
			*rs = rsPADHOME;
			break;
		}
		
		case VK_NUMPAD8:
		{
			*rs = rsPADUP;
			break;
		}
		
		case VK_NUMPAD9:
		{
			*rs = rsPADPGUP;
			break;
		}
		
		case VK_MULTIPLY:
		{
			*rs = rsTIMES;
			break;
		}
		
		case VK_DIVIDE:
		{
			*rs = rsDIVIDE;
			break;
		}
		
		case VK_ADD:
		{
			*rs = rsPLUS;
			break;
		}
		
		case VK_SUBTRACT:
		{
			*rs = rsMINUS;
			break;
		}
		
		case VK_DECIMAL:
		{
			*rs = rsPADDEL;
			break;
		}
		
		case VK_F1:
		{
			*rs = rsF1;
			break;
		}
		
		case VK_F2:
		{
			*rs = rsF2;
			break;
		}
		
		case VK_F3:
		{
			*rs = rsF3;
			break;
		}
		
		case VK_F4:
		{
			*rs = rsF4;
			break;
		}
		
		case VK_F5:
		{
			*rs = rsF5;
			break;
		}
		
		case VK_F6:
		{
			*rs = rsF6;
			break;
		}
		
		case VK_F7:
		{
			*rs = rsF7;
			break;
		}
		
		case VK_F8:
		{
			*rs = rsF8;
			break;
		}
		
		case VK_F9:	
		{
			*rs = rsF9;
			break;
		}
		
		case VK_F10:
		{
			*rs = rsF10;
			break;
		}
		
		case VK_F11:
		{
			*rs = rsF11;
			break;
		}
		
		case VK_F12:
		{
			*rs = rsF12;
			break;
		}
		
		case VK_NUMLOCK:
		{
			*rs = rsNUMLOCK;
			break;
		}
		
		case VK_SCROLL:
		{
			*rs = rsSCROLL;
			break;
		}
		
		case VK_BACK:
		{
			*rs = rsBACKSP;
			break;
		}
		
		case VK_TAB:
		{
			*rs = rsTAB;
			break;
		}
		
		default:
		{
			UINT vkey = MapVirtualKey(key, MAPVK_VK_TO_CHAR) & 0xFFFF;
			if ( vkey < 255 )
				*rs = (RsKeyCodes)vkey;
			break;
		}
	}
	
	return *rs != rsNULL;
}

void _InputTranslateShiftKeyUpDown(RsKeyCodes *rs)
{
	if ( _dwOperatingSystemVersion != OS_WIN98 )
	{
		if ( _InputTranslateShiftKey(rs, VK_LSHIFT, TRUE) )
			RsKeyboardEventHandler(rsKEYDOWN, rs);
		if ( _InputTranslateShiftKey(rs, VK_RSHIFT, TRUE) )
			RsKeyboardEventHandler(rsKEYDOWN, rs);
		if ( _InputTranslateShiftKey(rs, VK_LSHIFT, FALSE) )
			RsKeyboardEventHandler(rsKEYUP, rs);
		if ( _InputTranslateShiftKey(rs, VK_RSHIFT, FALSE) )
			RsKeyboardEventHandler(rsKEYUP, rs);
	}
}

#pragma warning( push )
#pragma warning( disable : 4805)
BOOL _InputTranslateShiftKey(RsKeyCodes *rs, UINT key, bool bDown)
{
	*rs = rsNULL;
	switch ( key )
	{
		case VK_LSHIFT:
		{
			if ( bDown == (GetKeyState(VK_LSHIFT) & 0x8000) >> 15 )
				*rs = rsLSHIFT;
			break;
		}
		
		case VK_RSHIFT:
		{
			if ( bDown == (GetKeyState(VK_RSHIFT) & 0x8000) >> 15 )
				*rs = rsRSHIFT;
			break;
		}
		
		default:
		{
			return *rs != rsNULL;
		}
	}
	
	return TRUE;
}
#pragma warning( pop )

BOOL _InputIsExtended(INT flag)
{
	return (flag & 0x1000000) != 0;
}


STARTPATCHES	
	//InjectHook(0x580B30, &CJoySticks::CJoySticks, PATCH_JUMP);
	//InjectHook(0x580B50, &CJoySticks::ClearJoyInfo, PATCH_JUMP);
	InjectHook(0x580B70, _psCreateFolder, PATCH_JUMP);
	InjectHook(0x580BB0, _psGetUserFilesFolder, PATCH_JUMP);
	InjectHook(0x580C70, psCameraBeginUpdate, PATCH_JUMP);
	InjectHook(0x580CA0, psCameraShowRaster, PATCH_JUMP);
	InjectHook(0x580CE0, psTimer, PATCH_JUMP);
	InjectHook(0x580D20, psMouseSetPos, PATCH_JUMP);
	InjectHook(0x580E10, psGetMemoryFunctions, PATCH_JUMP);
	InjectHook(0x580E20, psInstallFileSystem, PATCH_JUMP);
	InjectHook(0x580E30, psNativeTextureSupport, PATCH_JUMP);
	InjectHook(0x580E40, InitApplication, PATCH_JUMP);
	InjectHook(0x580EB0, InitInstance, PATCH_JUMP);
	InjectHook(0x580F30, _GetVideoMemInfo, PATCH_JUMP);
	InjectHook(0x580FA0, GetDXVersion, PATCH_JUMP);
	InjectHook(0x5810C0, _psGetCpuVendr, PATCH_JUMP);
	InjectHook(0x5810E0, _psGetCpuFeatures, PATCH_JUMP);
	InjectHook(0x5810F0, _psGetCpuFeaturesEx, PATCH_JUMP);
	InjectHook(0x581120, _psPrintCpuInfo, PATCH_JUMP);
	InjectHook(0x581180, psInitialise, PATCH_JUMP);
	InjectHook(0x581460, psTerminate, PATCH_JUMP);
	InjectHook(0x581470, _psGetNumVideModes, PATCH_JUMP);
	InjectHook(0x581480, _psFreeVideoModeList, PATCH_JUMP);
	InjectHook(0x5814F0, _psGetVideoModeList, PATCH_JUMP);
	InjectHook(0x581630, _psSelectScreenVM, PATCH_JUMP);
	InjectHook(0x5816B0, WaitForState, PATCH_JUMP);
	InjectHook(0x5816E0, HandleGraphEvent, PATCH_JUMP);
	InjectHook(0x581790, MainWndProc, PATCH_JUMP);
	InjectHook(0x581C90, IsForegroundApp, PATCH_JUMP);
	InjectHook(0x581CB0, GetBestRefreshRate, PATCH_JUMP);
	InjectHook(0x581D80, psSelectDevice, PATCH_JUMP);
	InjectHook(0x581F90, _psSetVideoMode, PATCH_JUMP);
	InjectHook(0x582030, CommandLineToArgv, PATCH_JUMP);
	InjectHook(0x582160, InitialiseLanguage, PATCH_JUMP);
	InjectHook(0x5822F0, CenterVideo, PATCH_JUMP);
	InjectHook(0x582380, PlayMovieInWindow, PATCH_JUMP);
	InjectHook(0x5825E0, CloseInterfaces, PATCH_JUMP);
	InjectHook(0x582680, CloseClip, PATCH_JUMP);
	InjectHook(0x5826A0, HandleExit, PATCH_JUMP);
	InjectHook(0x582710, _WinMain, PATCH_JUMP);
	InjectHook(0x5830D0, _InputInitialise, PATCH_JUMP);
	InjectHook(0x583110, _InputInitialiseMouse, PATCH_JUMP);
	InjectHook(0x583190, CapturePad, PATCH_JUMP);
	InjectHook(0x583580, _InputInitialiseJoys, PATCH_JUMP);
	InjectHook(0x583670, _InputAddJoyStick, PATCH_JUMP);
	InjectHook(0x583810, _InputAddJoys, PATCH_JUMP);
	InjectHook(0x583870, _InputGetMouseState, PATCH_JUMP);
	InjectHook(0x583910, _InputShutdown, PATCH_JUMP);
	InjectHook(0x583940, _InputEnumDevicesCallback, PATCH_JUMP);
	InjectHook(0x583A20, _InputTranslateKey, PATCH_JUMP);
	InjectHook(0x583DC0, _InputTranslateShiftKeyUpDown, PATCH_JUMP);
	InjectHook(0x583E50, _InputTranslateShiftKey, PATCH_JUMP);
	InjectHook(0x583EE0, _InputIsExtended, PATCH_JUMP);
ENDPATCHES