#include "common.h"
#include "patcher.h"
#include "main.h"
#include "General.h"
#include "RwHelper.h"
#include "Clouds.h"
#include "Draw.h"
#include "Sprite2d.h"
#include "Renderer.h"
#include "Coronas.h"
#include "WaterLevel.h"
#include "Weather.h"
#include "Glass.h"
#include "WaterCannon.h"
#include "SpecialFX.h"
#include "Shadows.h"
#include "Skidmarks.h"
#include "Antennas.h"
#include "Rubbish.h"
#include "Particle.h"
#include "Pickups.h"
#include "WeaponEffects.h"
#include "PointLights.h"
#include "Fluff.h"
#include "Replay.h"
#include "Camera.h"
#include "World.h"
#include "Ped.h"
#include "Font.h"
#include "Pad.h"
#include "Hud.h"
#include "User.h"
#include "Messages.h"
#include "Darkel.h"
#include "Garages.h"
#include "MusicManager.h"
#include "VisibilityPlugins.h"
#include "NodeName.h"
#include "DMAudio.h"
#include "CutsceneMgr.h"
#include "Lights.h"
#include "Credits.h"
#include "ZoneCull.h"
#include "Timecycle.h"
#include "TxdStore.h"
#include "FileMgr.h"
#include "Text.h"
#include "RpAnimBlend.h"
#include "Frontend.h"

#define DEFAULT_VIEWWINDOW (Tan(DEGTORAD(CDraw::GetFOV() * 0.5f)))


GlobalScene &Scene = *(GlobalScene*)0x726768;

uint8 work_buff[55000];
//char gString[256];
//char gString2[512];
//wchar gUString[256];
//wchar gUString2[256];
char *gString = (char*)0x711B40;
char *gString2 = (char*)0x878A40;
wchar *gUString = (wchar*)0x74B018;
wchar *gUString2 = (wchar*)0x6EDD70;

bool &b_FoundRecentSavedGameWantToLoad = *(bool*)0x95CDA8;


bool DoRWStuffStartOfFrame_Horizon(int16 TopRed, int16 TopGreen, int16 TopBlue, int16 BottomRed, int16 BottomGreen, int16 BottomBlue, int16 Alpha);
void DoRWStuffEndOfFrame(void);

void RenderScene(void);
void RenderDebugShit(void);
void RenderEffects(void);
void Render2dStuff(void);
void RenderMenus(void);
void DoFade(void);
void Render2dStuffAfterFade(void);

CSprite2d *LoadSplash(const char *name);
void DestroySplashScreen(void);


extern void (*DebugMenuProcess)(void);
extern void (*DebugMenuRender)(void);
void DebugMenuInit(void);
void DebugMenuPopulate(void);

void PrintGameVersion();

RwRGBA gColourTop;

void
InitialiseGame(void)
{
	LoadingScreen(nil, nil, "loadsc0");
	CGame::Initialise("DATA\\GTA3.DAT");
}

void
Idle(void *arg)
{
#ifdef ASPECT_RATIO_SCALE
	CDraw::SetAspectRatio(CDraw::FindAspectRatio());
#endif

	CTimer::Update();
	CSprite2d::InitPerFrame();
	CFont::InitPerFrame();
	CPointLights::InitPerFrame();
	CGame::Process();
	DMAudio.Service();

	if(CGame::bDemoMode && CTimer::GetTimeInMilliseconds() > (3*60 + 30)*1000 && !CCutsceneMgr::IsCutsceneProcessing()){
		FrontEndMenuManager.m_bStartGameLoading = true;
		FrontEndMenuManager.m_bLoadingSavedGame = false;
		return;
	}

	if(FrontEndMenuManager.m_bStartGameLoading || b_FoundRecentSavedGameWantToLoad)
		return;

	SetLightsWithTimeOfDayColour(Scene.world);

	if(arg == nil)
		return;

	if((!FrontEndMenuManager.m_bMenuActive || FrontEndMenuManager.field_452 == 1) &&
	   TheCamera.GetScreenFadeStatus() != FADE_2){
#ifdef GTA_PC
		// This is from SA, but it's nice for windowed mode
		RwV2d pos;
		pos.x = SCREEN_WIDTH/2.0f;
		pos.y = SCREEN_HEIGHT/2.0f;
		RsMouseSetPos(&pos);
#endif
		CRenderer::ConstructRenderList();
		CRenderer::PreRender();

		if(CWeather::LightningFlash && !CCullZones::CamNoRain()){
			if(!DoRWStuffStartOfFrame_Horizon(255, 255, 255, 255, 255, 255, 255))
				return;
		}else{
			if(!DoRWStuffStartOfFrame_Horizon(CTimeCycle::GetSkyTopRed(), CTimeCycle::GetSkyTopGreen(), CTimeCycle::GetSkyTopBlue(),
						CTimeCycle::GetSkyBottomRed(), CTimeCycle::GetSkyBottomGreen(), CTimeCycle::GetSkyBottomBlue(),
						255))
				return;
		}

		DefinedState();

		// BUG. This has to be done BEFORE RwCameraBeginUpdate
		RwCameraSetFarClipPlane(Scene.camera, CTimeCycle::GetFarClip());
		RwCameraSetFogDistance(Scene.camera, CTimeCycle::GetFogStart());

		RenderScene();
		RenderDebugShit();
		RenderEffects();

		if((TheCamera.m_BlurType == MBLUR_NONE || TheCamera.m_BlurType == MBLUR_NORMAL) &&
		   TheCamera.m_ScreenReductionPercentage > 0.0)
		        TheCamera.SetMotionBlurAlpha(150);
		TheCamera.RenderMotionBlur();

		Render2dStuff();
	}else{
		float viewWindow = DEFAULT_VIEWWINDOW;
		CameraSize(Scene.camera, nil, viewWindow, DEFAULT_ASPECT_RATIO);
		CVisibilityPlugins::SetRenderWareCamera(Scene.camera);
		RwCameraClear(Scene.camera, &gColourTop, rwCAMERACLEARZ);
		if(!RsCameraBeginUpdate(Scene.camera))
			return;
	}

	RenderMenus();
#ifndef FINAL
	PrintGameVersion();
#endif
	DoFade();
	Render2dStuffAfterFade();
	CCredits::Render();
	DoRWStuffEndOfFrame();

//	if(g_SlowMode) 
//		ProcessSlowMode();
}

void
FrontendIdle(void)
{
#ifdef ASPECT_RATIO_SCALE
	CDraw::SetAspectRatio(CDraw::FindAspectRatio());
#endif

	CTimer::Update();
	CSprite2d::SetRecipNearClip();
	CSprite2d::InitPerFrame();
	CFont::InitPerFrame();
	CPad::UpdatePads();
	FrontEndMenuManager.Process();

	if(RsGlobal.quit)
		return;

	float viewWindow = DEFAULT_VIEWWINDOW;
	CameraSize(Scene.camera, nil, viewWindow, DEFAULT_ASPECT_RATIO);
	CVisibilityPlugins::SetRenderWareCamera(Scene.camera);
	RwCameraClear(Scene.camera, &gColourTop, rwCAMERACLEARZ);
	if(!RsCameraBeginUpdate(Scene.camera))
		return;

	DefinedState();
	RenderMenus();
#ifndef FINAL
	PrintGameVersion();
#endif
	DoFade();
	Render2dStuffAfterFade();
	CFont::DrawFonts();
	DoRWStuffEndOfFrame();
}

bool
DoRWStuffStartOfFrame(int16 TopRed, int16 TopGreen, int16 TopBlue, int16 BottomRed, int16 BottomGreen, int16 BottomBlue, int16 Alpha)
{
	CRGBA TopColor(TopRed, TopGreen, TopBlue, Alpha);
	CRGBA BottomColor(BottomRed, BottomGreen, BottomBlue, Alpha);

	CameraSize(Scene.camera, nil, DEFAULT_VIEWWINDOW, SCREEN_ASPECT_RATIO);
	CVisibilityPlugins::SetRenderWareCamera(Scene.camera);
	RwCameraClear(Scene.camera, &gColourTop, rwCAMERACLEARZ);

	if(!RsCameraBeginUpdate(Scene.camera))
		return false;

	CSprite2d::InitPerFrame();

	if(Alpha != 0)
		CSprite2d::DrawRect(CRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT), BottomColor, BottomColor, TopColor, TopColor);

	return true;
}

bool
DoRWStuffStartOfFrame_Horizon(int16 TopRed, int16 TopGreen, int16 TopBlue, int16 BottomRed, int16 BottomGreen, int16 BottomBlue, int16 Alpha)
{
	CameraSize(Scene.camera, nil, DEFAULT_VIEWWINDOW, SCREEN_ASPECT_RATIO);
	CVisibilityPlugins::SetRenderWareCamera(Scene.camera);
	RwCameraClear(Scene.camera, &gColourTop, rwCAMERACLEARZ);

	if(!RsCameraBeginUpdate(Scene.camera))
		return false;

	TheCamera.m_viewMatrix.Update();
	CClouds::RenderBackground(TopRed, TopGreen, TopBlue, BottomRed, BottomGreen, BottomBlue, Alpha);

	return true;
}

void
DoRWStuffEndOfFrame(void)
{
	// CDebug::DebugDisplayTextBuffer();
	// FlushObrsPrintfs();
	RwCameraEndUpdate(Scene.camera);
	RsCameraShowRaster(Scene.camera);
}


// This is certainly a very useful function
void
DoRWRenderHorizon(void)
{
	CClouds::RenderHorizon();
}

void
RenderScene(void)
{
	CClouds::Render();
	DoRWRenderHorizon();
	CRenderer::RenderRoads();
	CCoronas::RenderReflections();
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
	CRenderer::RenderEverythingBarRoads();
	CRenderer::RenderBoats();
	DefinedState();
	CWaterLevel::RenderWater();
	CRenderer::RenderFadingInEntities();
	CRenderer::RenderVehiclesButNotBoats();
	CWeather::RenderRainStreaks();
}

void
RenderDebugShit(void)
{
	// CTheScripts::RenderTheScriptDebugLines()
	if(gbShowCollisionLines)
		CRenderer::RenderCollisionLines();
}

void
RenderEffects(void)
{
	CGlass::Render();
	CWaterCannons::Render();
	CSpecialFX::Render();
	CShadows::RenderStaticShadows();
	CShadows::RenderStoredShadows();
	CSkidmarks::Render();
	CAntennas::Render();
	CRubbish::Render();
	CCoronas::Render();
	CParticle::Render();
	CPacManPickups::Render();
	CWeaponEffects::Render();
	CPointLights::RenderFogEffect();
	CMovingThings::Render();
	CRenderer::RenderFirstPersonVehicle();
}

void
Render2dStuff(void)
{
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);

	CReplay::Display();
	CPickups::RenderPickUpText();

	if(TheCamera.m_WideScreenOn)
		TheCamera.DrawBordersForWideScreen();

	CPed *player = FindPlayerPed();
	int weaponType = 0;
	if(player)
		weaponType = player->GetWeapon()->m_eWeaponType;

	bool firstPersonWeapon = false;
	int cammode = TheCamera.Cams[TheCamera.ActiveCam].Mode;
	if(cammode == CCam::MODE_SNIPER ||
	   cammode == CCam::MODE_SNIPER_RUN_AROUND ||
	   cammode == CCam::MODE_ROCKET ||
	   cammode == CCam::MODE_ROCKET_RUN_AROUND)
		firstPersonWeapon = true;

	// Draw black border for sniper and rocket launcher
	if((weaponType == WEAPONTYPE_SNIPERRIFLE || weaponType == WEAPONTYPE_ROCKETLAUNCHER) && firstPersonWeapon){
		CRGBA black(0, 0, 0, 255);

		// top and bottom strips
		if (weaponType == WEAPONTYPE_ROCKETLAUNCHER) {
			CSprite2d::DrawRect(CRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT / 2 - SCREEN_SCALE_Y(180)), black);
			CSprite2d::DrawRect(CRect(0.0f, SCREEN_HEIGHT / 2 + SCREEN_SCALE_Y(170), SCREEN_WIDTH, SCREEN_HEIGHT), black);
		}
		else {
			CSprite2d::DrawRect(CRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT / 2 - SCREEN_SCALE_Y(210)), black);
			CSprite2d::DrawRect(CRect(0.0f, SCREEN_HEIGHT / 2 + SCREEN_SCALE_Y(210), SCREEN_WIDTH, SCREEN_HEIGHT), black);
		}
		CSprite2d::DrawRect(CRect(0.0f, 0.0f, SCREEN_WIDTH / 2 - SCREEN_SCALE_X(210), SCREEN_HEIGHT), black);
		CSprite2d::DrawRect(CRect(SCREEN_WIDTH / 2 + SCREEN_SCALE_X(210), 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT), black);
	}

	MusicManager.DisplayRadioStationName();
//	TheConsole.Display();
/*
	if(CSceneEdit::m_bEditOn)
		CSceneEdit::Draw();
	else
*/
		CHud::Draw();
	CUserDisplay::OnscnTimer.ProcessForDisplay();
	CMessages::Display();
	CDarkel::DrawMessages();
	CGarages::PrintMessages();
	CPad::PrintErrorMessage();
	CFont::DrawFonts();

	DebugMenuRender();
}

void
RenderMenus(void)
{
	if(FrontEndMenuManager.m_bMenuActive)
		FrontEndMenuManager.DrawFrontEnd();
}

bool &JustLoadedDontFadeInYet = *(bool*)0x95CDB4;
bool &StillToFadeOut = *(bool*)0x95CD99;
uint32 &TimeStartedCountingForFade = *(uint32*)0x9430EC;
uint32 &TimeToStayFadedBeforeFadeOut = *(uint32*)0x611564;

void
DoFade(void)
{
	if(CTimer::GetIsPaused())
		return;

	if(JustLoadedDontFadeInYet){
		JustLoadedDontFadeInYet = false;
		TimeStartedCountingForFade = CTimer::GetTimeInMilliseconds();
	}

	if(StillToFadeOut){
		if(CTimer::GetTimeInMilliseconds() - TimeStartedCountingForFade > TimeToStayFadedBeforeFadeOut){
			StillToFadeOut = false;
			TheCamera.Fade(3.0f, FADE_IN);
			TheCamera.ProcessFade();
			TheCamera.ProcessMusicFade();
		}else{
			TheCamera.SetFadeColour(0, 0, 0);
			TheCamera.Fade(0.0f, FADE_OUT);
			TheCamera.ProcessFade();
		}
	}

	if(CDraw::FadeValue != 0 || CMenuManager::m_PrefsBrightness < 256){
		CSprite2d *splash = LoadSplash(nil);

		CRGBA fadeColor;
		CRect rect;
		int fadeValue = CDraw::FadeValue;
		float brightness = min(CMenuManager::m_PrefsBrightness, 256);
		if(brightness <= 50)
			brightness = 50;
		if(FrontEndMenuManager.m_bMenuActive)
			brightness = 256;

		if(TheCamera.m_FadeTargetIsSplashScreen)
			fadeValue = 0;

		float fade = fadeValue + 256 - brightness;
		if(fade == 0){
			fadeColor.r = 0;
			fadeColor.g = 0;
			fadeColor.b = 0;
			fadeColor.a = 0;
		}else{
			fadeColor.r = fadeValue * CDraw::FadeRed / fade;
			fadeColor.g = fadeValue * CDraw::FadeGreen / fade;
			fadeColor.b = fadeValue * CDraw::FadeBlue / fade;
			int alpha = 255 - brightness*(256 - fadeValue)/256;
			if(alpha < 0)
				alpha = 0;
			fadeColor.a = alpha;
		}

		if(TheCamera.m_WideScreenOn){
			// what's this?
			float y = SCREEN_HEIGHT/2 * TheCamera.m_ScreenReductionPercentage/100.0f;
			rect.left = 0.0f;
			rect.right = SCREEN_WIDTH;
			rect.top = y - 8.0f;
			rect.bottom = SCREEN_HEIGHT - y - 8.0f;
		}else{
			rect.left = 0.0f;
			rect.right = SCREEN_WIDTH;
			rect.top = 0.0f;
			rect.bottom = SCREEN_HEIGHT;
		}
		CSprite2d::DrawRect(rect, fadeColor);

		if(CDraw::FadeValue != 0 && TheCamera.m_FadeTargetIsSplashScreen){
			fadeColor.r = 255;
			fadeColor.g = 255;
			fadeColor.b = 255;
			fadeColor.a = CDraw::FadeValue;
			splash->Draw(CRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT), fadeColor, fadeColor, fadeColor, fadeColor);
		}
	}
}

void
Render2dStuffAfterFade(void)
{
	CHud::DrawAfterFade();
	CFont::DrawFonts();
}

CSprite2d splash;
int splashTxdId = -1;

CSprite2d*
LoadSplash(const char *name)
{
	RwTexDictionary *txd;
	char filename[140];
	RwTexture *tex = nil;

	if(name == nil)
		return &splash;
	if(splashTxdId == -1)
		splashTxdId = CTxdStore::AddTxdSlot("splash");

	txd = CTxdStore::GetSlot(splashTxdId)->texDict;
	if(txd)
		tex = RwTexDictionaryFindNamedTexture(txd, name);
	// if texture is found, splash was already set up below

	if(tex == nil){
		CFileMgr::SetDir("TXD\\");
		sprintf(filename, "%s.txd", name);
		if(splash.m_pTexture)
			splash.Delete();
		if(txd)
			CTxdStore::RemoveTxd(splashTxdId);
		CTxdStore::LoadTxd(splashTxdId, filename);
		CTxdStore::AddRef(splashTxdId);
		CTxdStore::PushCurrentTxd();
		CTxdStore::SetCurrentTxd(splashTxdId);
		splash.SetTexture(name);
		CTxdStore::PopCurrentTxd();
		CFileMgr::SetDir("");
	}

	return &splash;
}

void
DestroySplashScreen(void)
{
	splash.Delete();
	if(splashTxdId != -1)
		CTxdStore::RemoveTxdSlot(splashTxdId);
	splashTxdId = -1;
}

float NumberOfChunksLoaded;
#define TOTALNUMCHUNKS 73.0f

// TODO: compare with PS2
void
LoadingScreen(const char *str1, const char *str2, const char *splashscreen)
{
	CSprite2d *splash;

#ifndef RANDOMSPLASH
	if(CGame::frenchGame || CGame::germanGame || !CGame::nastyGame)
		splashscreen = "mainsc2";
	else
		splashscreen = "mainsc1";
#endif

	splash = LoadSplash(splashscreen);

	if(RsGlobal.quit)
		return;

	if(DoRWStuffStartOfFrame(0, 0, 0, 0, 0, 0, 255)){
		CSprite2d::SetRecipNearClip();
		CSprite2d::InitPerFrame();
		CFont::InitPerFrame();
		DefinedState();
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);
		splash->Draw(CRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT), CRGBA(255, 255, 255, 255));

		if(str1){
			NumberOfChunksLoaded += 1;

			float hpos = SCREEN_SCALE_X(40);
			float length = SCREEN_WIDTH - SCREEN_SCALE_X(100);
			float vpos = SCREEN_HEIGHT - SCREEN_SCALE_Y(13);
			float height = SCREEN_SCALE_Y(7);
			CSprite2d::DrawRect(CRect(hpos, vpos, hpos + length, vpos + height), CRGBA(40, 53, 68, 255));

			length *= NumberOfChunksLoaded/TOTALNUMCHUNKS;
			CSprite2d::DrawRect(CRect(hpos, vpos, hpos + length, vpos + height), CRGBA(81, 106, 137, 255));

			// this is done by the game but is unused
			CFont::SetScale(SCREEN_SCALE_X(2), SCREEN_SCALE_Y(2));
			CFont::SetPropOn();
			CFont::SetRightJustifyOn();
			CFont::SetFontStyle(FONT_HEADING);

#ifdef CHATTYSPLASH
			// my attempt
			static wchar tmpstr[80];
			float scale = SCREEN_SCALE_Y(0.8f);
			vpos -= 50*scale;
			CFont::SetScale(scale, scale);
			CFont::SetPropOn();
			CFont::SetRightJustifyOff();
			CFont::SetFontStyle(FONT_BANK);
			CFont::SetColor(CRGBA(255, 255, 255, 255));
			AsciiToUnicode(str1, tmpstr);
			CFont::PrintString(hpos, vpos, tmpstr);
			vpos += 25*scale;
			AsciiToUnicode(str2, tmpstr);
			CFont::PrintString(hpos, vpos, tmpstr);
#endif
		}

		CFont::DrawFonts();
 		DoRWStuffEndOfFrame();
	}
}

void
ResetLoadingScreenBar(void)
{
	NumberOfChunksLoaded = 0.0f;
}

void
LoadingIslandScreen(const char *levelName)
{
	CSprite2d *splash;
	wchar *name;
	char str[100];
	wchar wstr[80];
	CRGBA col;

	splash = LoadSplash(nil);
	name = TheText.Get(levelName);
	if(!DoRWStuffStartOfFrame(0, 0, 0, 0, 0, 0, 255))
		return;

	CSprite2d::SetRecipNearClip();
	CSprite2d::InitPerFrame();
	CFont::InitPerFrame();
	DefinedState();
	col = CRGBA(255, 255, 255, 255);
	splash->Draw(CRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT), col, col, col, col);
	CFont::SetBackgroundOff();
	CFont::SetScale(1.5f, 1.5f);
	CFont::SetPropOn();
	CFont::SetRightJustifyOn();
	CFont::SetRightJustifyWrap(150.0f);
	CFont::SetFontStyle(FONT_HEADING);
	sprintf(str, "WELCOME TO");
	AsciiToUnicode(str, wstr);
	CFont::SetDropColor(CRGBA(0, 0, 0, 255));
	CFont::SetDropShadowPosition(3);
	CFont::SetColor(CRGBA(243, 237, 71, 255));
	CFont::SetScale(SCREEN_STRETCH_X(1.2f), SCREEN_STRETCH_Y(1.2f));
	CFont::PrintString(SCREEN_WIDTH - 20, SCREEN_STRETCH_FROM_BOTTOM(110.0f), TheText.Get("WELCOME"));
	TextCopy(wstr, name);
	TheText.UpperCase(wstr);
	CFont::SetColor(CRGBA(243, 237, 71, 255));
	CFont::SetScale(SCREEN_STRETCH_X(1.2f), SCREEN_STRETCH_Y(1.2f));
	CFont::PrintString(SCREEN_WIDTH-20, SCREEN_STRETCH_FROM_BOTTOM(80.0f), wstr);
	CFont::DrawFonts();
	DoRWStuffEndOfFrame();
}

char*
GetLevelSplashScreen(int level)
{
	static char *splashScreens[4] = {
		nil,
		"splash1",
		"splash2",
		"splash3",
	};

	return splashScreens[level];
}

char*
GetRandomSplashScreen(void)
{
	int index;
	static int index2 = 0;
	static char splashName[128];
	static int splashIndex[24] = {
		25, 22, 4, 13,
		1, 21, 14, 16,
		10, 12, 5, 9,
		11, 18, 3, 2,
		19, 23, 7, 17,
		15, 6, 8, 20
	};

	index = splashIndex[4*index2 + CGeneral::GetRandomNumberInRange(0, 3)];
	index2++;
	if(index2 == 6)
		index2 = 0;
	sprintf(splashName, "loadsc%d", index);
	return splashName;
}

#include "rwcore.h"
#include "rpworld.h"
#include "rpmatfx.h"
#include "rpskin.h"
#include "rphanim.h"
#include "rtbmp.h"

_TODO("temp, move this includes out of here")

static RwBool 
PluginAttach(void)
{
	if( !RpWorldPluginAttach() )
	{
		printf("Couldn't attach world plugin\n");
		
		return FALSE;
	}
	
	if( !RpSkinPluginAttach() )
	{
		printf("Couldn't attach RpSkin plugin\n");
		
		return FALSE;
	}
	
	if( !RpHAnimPluginAttach() )
	{
		printf("Couldn't attach RpHAnim plugin\n");
		
		return FALSE;
	}
	
	if( !NodeNamePluginAttach() )
	{
		printf("Couldn't attach node name plugin\n");
		
		return FALSE;
	}
	
	if( !CVisibilityPlugins::PluginAttach() )
	{
		printf("Couldn't attach visibility plugins\n");
		
		return FALSE;
	}
	
	if( !RpAnimBlendPluginAttach() )
	{
		printf("Couldn't attach RpAnimBlend plugin\n");
		
		return FALSE;
	}
	
	if( !RpMatFXPluginAttach() )
	{
		printf("Couldn't attach RpMatFX plugin\n");
		
		return FALSE;
	}

	return TRUE;
}

static RwBool 
Initialise3D(void *param)
{
	if (RsRwInitialise(param))
	{
		//
		DebugMenuInit();
		DebugMenuPopulate();
		//

		return CGame::InitialiseRenderWare();
	}

	return (FALSE);
}


static void 
Terminate3D(void)
{
	CGame::ShutdownRenderWare();
	
	RsRwTerminate();

	return;
}

RsEventStatus
AppEventHandler(RsEvent event, void *param)
{
	switch( event )
	{
		case rsINITIALISE:
		{
			CGame::InitialiseOnceBeforeRW();
			return RsInitialise() ? rsEVENTPROCESSED : rsEVENTERROR;
		}

		case rsCAMERASIZE:
		{
											
			CameraSize(Scene.camera, (RwRect *)param,
				DEFAULT_VIEWWINDOW, DEFAULT_ASPECT_RATIO);
			
			return rsEVENTPROCESSED;
		}

		case rsRWINITIALISE:
		{
			return Initialise3D(param) ? rsEVENTPROCESSED : rsEVENTERROR;
		}

		case rsRWTERMINATE:
		{
			Terminate3D();

			return rsEVENTPROCESSED;
		}

		case rsTERMINATE:
		{
			CGame::FinalShutdown();

			return rsEVENTPROCESSED;
		}

		case rsPLUGINATTACH:
		{
			return PluginAttach() ? rsEVENTPROCESSED : rsEVENTERROR;
		}

		case rsINPUTDEVICEATTACH:
		{
			AttachInputDevices();

			return rsEVENTPROCESSED;
		}

		case rsIDLE:
		{
			Idle(param);

			return rsEVENTPROCESSED;
		}

		case rsFRONTENDIDLE:
		{
			FrontendIdle();

			return rsEVENTPROCESSED;
		}

		case rsACTIVATE:
		{
			param ? DMAudio.ReacquireDigitalHandle() : DMAudio.ReleaseDigitalHandle();

			return rsEVENTPROCESSED;
		}

		default:
		{
			return rsEVENTNOTPROCESSED;
		}
	}
}

void PrintGameVersion()
{
	CFont::SetPropOn();
	CFont::SetBackgroundOff();
	CFont::SetScale(SCREEN_SCALE_X(0.7f), SCREEN_SCALE_Y(0.5f));
	CFont::SetCentreOff();
	CFont::SetRightJustifyOff();
	CFont::SetBackGroundOnlyTextOff();
	CFont::SetFontStyle(FONT_BANK);
	CFont::SetWrapx(SCREEN_WIDTH);
	CFont::SetDropShadowPosition(0);
	CFont::SetDropColor(CRGBA(0, 0, 0, 255));
	CFont::SetColor(CRGBA(235, 170, 50, 255));

	strcpy(gString, "RE3");
	AsciiToUnicode(gString, gUString);
	CFont::PrintString(SCREEN_SCALE_X(10.5f), SCREEN_SCALE_Y(8.0f), gUString);
}

STARTPATCHES
	InjectHook(0x48E480, Idle, PATCH_JUMP);
	InjectHook(0x48E700, FrontendIdle, PATCH_JUMP);

	InjectHook(0x48CF10, DoRWStuffStartOfFrame, PATCH_JUMP);
	InjectHook(0x48D040, DoRWStuffStartOfFrame_Horizon, PATCH_JUMP);
	InjectHook(0x48E030, RenderScene, PATCH_JUMP);
	InjectHook(0x48E080, RenderDebugShit, PATCH_JUMP);
	InjectHook(0x48E090, RenderEffects, PATCH_JUMP);
	InjectHook(0x48E0E0, Render2dStuff, PATCH_JUMP);
	InjectHook(0x48E450, RenderMenus, PATCH_JUMP);
	InjectHook(0x48D120, DoFade, PATCH_JUMP);
	InjectHook(0x48E470, Render2dStuffAfterFade, PATCH_JUMP);

	InjectHook(0x48D550, LoadSplash, PATCH_JUMP);
	InjectHook(0x48D670, DestroySplashScreen, PATCH_JUMP);
	InjectHook(0x48D770, LoadingScreen, PATCH_JUMP);
	InjectHook(0x48D760, ResetLoadingScreenBar, PATCH_JUMP);
	
	InjectHook(0x48D470, PluginAttach, PATCH_JUMP);
	InjectHook(0x48D520, Initialise3D, PATCH_JUMP);
	InjectHook(0x48D540, Terminate3D, PATCH_JUMP);
	InjectHook(0x48E800, AppEventHandler, PATCH_JUMP);
ENDPATCHES
