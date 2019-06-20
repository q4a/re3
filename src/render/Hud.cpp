#include "common.h"
#include "patcher.h"
#include "Camera.h"
#include "DMAudio.h"
#include "Clock.h"
#include "Darkel.h"
#include "Hud.h"
#include "Messages.h"
#include "Frontend.h"
#include "Font.h"
#include "Pad.h"
#include "Radar.h"
#include "Replay.h"
#include "Sprite.h"
#include "Sprite2d.h"
#include "Text.h"
#include "Timer.h"
#include "Script.h"
#include "TxdStore.h"
#include "User.h"
#include "World.h"

//WRAPPER void CHud::Draw(void) { EAXJMP(0x5052A0); }
//WRAPPER void CHud::DrawAfterFade(void) { EAXJMP(0x509030); }
//WRAPPER void CHud::ReInitialise(void) { EAXJMP(0x504CC0); }
WRAPPER void CHud::GetRidOfAllHudMessages(void) { EAXJMP(0x504F90); }
WRAPPER void CHud::SetHelpMessage(wchar *message, bool quick) { EAXJMP(0x5051E0); }
WRAPPER void CHud::SetMessage(wchar *message) { EAXJMP(0x50A210); }
WRAPPER void CHud::SetBigMessage(wchar *message, int16 style) { EAXJMP(0x50A250); }
WRAPPER void CHud::SetPagerMessage(wchar *message) { EAXJMP(0x50A320); }

wchar *CHud::m_HelpMessage = (wchar*)0x86B888;
wchar *CHud::m_LastHelpMessage = (wchar*)0x6E8F28;
int32 &CHud::m_HelpMessageState = *(int32*)0x880E1C;
int32 &CHud::m_HelpMessageTimer = *(int32*)0x880FA4;
int32 &CHud::m_HelpMessageFadeTimer = *(int32*)0x8F6258;
wchar *CHud::m_HelpMessageToPrint = (wchar*)0x664480;
float &CHud::m_HelpMessageDisplayTime = *(float*)0x8E2C28;
float &CHud::m_fTextBoxNumLines = *(float*)0x8E2C28;
float &CHud::m_fHelpMessageTime = *(float *)0x8E2C28;
bool &CHud::m_HelpMessageQuick = *(bool *)0x95CCF7;
int32 CHud::m_ZoneState = *(int32*)0x8F29AC;
int32 CHud::m_ZoneFadeTimer;
int32 CHud::m_ZoneNameTimer = *(int32*)0x8F1A50;
wchar *&CHud::m_ZoneName = *(wchar **)0x8E2C2C;
wchar *CHud::m_LastZoneName = (wchar*)0x8F432C;
wchar *CHud::m_ZoneToPrint;
int32 CHud::m_VehicleState = *(int32*)0x940560;
int32 CHud::m_VehicleFadeTimer;
int32 CHud::m_VehicleNameTimer = *(int32*)0x8F2A14;
wchar *&CHud::m_VehicleName = *(wchar **)0x942FB4;
wchar *CHud::m_LastVehicleName = *(wchar **)0x8E2DD8;
wchar *CHud::m_VehicleNameToPrint;
wchar *CHud::m_Message = (wchar*)0x72E318;
wchar *CHud::m_PagerMessage = (wchar*)0x878840;
bool &CHud::m_Wants_To_Draw_Hud = *(bool*)0x95CD89;
bool &CHud::m_Wants_To_Draw_3dMarkers = *(bool*)0x95CD62;
wchar(*CHud::m_BigMessage)[128] = (wchar(*)[128])0x664CE0;
float *CHud::BigMessageInUse = (float*)0x862140;
float *CHud::BigMessageAlpha = (float*)0x862108;
float *CHud::BigMessageX = (float*)0x773248;

float &CHud::OddJob2OffTimer = *(float*)0x942FA0;
int8 &CHud::CounterOnLastFrame = *(int8*)0x95CD67;
float &CHud::OddJob2XOffset = *(float*)0x8F1B5C;
int16 &CHud::CounterFlashTimer = *(int16*)0x95CC20;
int16 &CHud::OddJob2Timer = *(int16*)0x95CC52;
int8 &CHud::TimerOnLastFrame = *(int8*)0x95CDA7;
int16 &CHud::OddJob2On = *(int16*)0x95CC78;
int16 &CHud::TimerFlashTimer = *(int16*)0x95CC6C;
int16 &CHud::PagerSoundPlayed = *(int16*)0x95CC4A;
int32 &CHud::SpriteBrightness = *(int32*)0x95CC54;
float &CHud::PagerXOffset = *(float*)0x941590;
int32 CHud::m_ItemToFlash = *(int32*)0x95CC82;
int16 &CHud::PagerTimer = *(int16*)0x95CC3A;
int16 &CHud::PagerOn = *(int16*)0x95CCA0;

CSprite2d *CHud::Sprites = (CSprite2d*)0x95CB9C;
char *WeaponFilenames[] = {
	"fist",
	"fistm",
	"bat",
	"batm",
	"pistol",
	"pistolm",
	"uzi",
	"uzim",
	"shotgun",
	"shotgunm",
	"ak47",
	"ak47m",
	"m16",
	"m16m",
	"sniper",
	"sniperm",
	"rocket",
	"rocketm",
	"flame",
	"flamem",
	"molotov",
	"molotovm",
	"grenade",
	"grenadem",
	"detonator",
	"detonator_mask",
	"",
	"",
	"",
	"",
	"radardisc",
	"radardiscm",
	"pager",
	"pagerm",
	"",
	"",
	"",
	"",
	"bleeder",
	"",
	"sitesniper",
	"sitesniperm",
	"siteM16",
	"siteM16m",
	"siterocket",
	"siterocketm"
};

RwTexture *&gpSniperSightTex = *(RwTexture**)0x8F5834;
RwTexture *&gpRocketSightTex = *(RwTexture**)0x8E2C20;

void CHud::Initialise()
{
	debug("Init CHud");

	ReInitialise();

	int HudTXD = CTxdStore::AddTxdSlot("hud");
	CTxdStore::LoadTxd(HudTXD, "MODELS/HUD.TXD");
	CTxdStore::AddRef(HudTXD);
	CTxdStore::PopCurrentTxd();
	CTxdStore::SetCurrentTxd(HudTXD);

	for (int i = 0; i < ARRAY_SIZE(WeaponFilenames) / 2; i++) {
		Sprites[i].SetTexture(WeaponFilenames[i * 2]);
	}

	gpSniperSightTex = RwTextureRead("sitesniper", 0);
	gpRocketSightTex = RwTextureRead("siterocket", 0);

	CTxdStore::PopCurrentTxd();
}

void CHud::Shutdown()
{
	debug("Shutdown CHud");

	for (int i = 0; i < ARRAY_SIZE(WeaponFilenames) / 2; ++i) {
		Sprites[i].Delete();
	}

	RwTextureDestroy(gpSniperSightTex);
	gpSniperSightTex = 0;

	RwTextureDestroy(gpRocketSightTex);
	gpRocketSightTex = 0;

	int HudTXD = CTxdStore::FindTxdSlot("hud");
	CTxdStore::RemoveTxdSlot(HudTXD);
}

void CHud::SetVehicleName(wchar *name)
{
	m_VehicleName = name;
}

void CHud::SetZoneName(wchar *name)
{
	m_ZoneName = name;
}

void CHud::Draw()
{
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSMIRROR);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEFLAT);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);

	if (CReplay::Mode != 1) {
		if (m_Wants_To_Draw_Hud && !TheCamera.m_WideScreenOn) {
			bool Mode_RunAround = 0;
			bool Mode_FirstPerson = 0;

			int32 WeaponType = CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_weapons[CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_currentWeapon].m_eWeaponType;
			int32 Mode = TheCamera.Cams[TheCamera.ActiveCam].Mode;

			if (Mode == CCam::CamMode::MODE_SNIPER || Mode == CCam::CamMode::MODE_ROCKET || Mode == CCam::CamMode::MODE_M16FIRSTPERSON_34 || Mode == CCam::CamMode::MODE_EDITOR)
				Mode_FirstPerson = 1;
			if (Mode == CCam::CamMode::MODE_FIRSTPERSONPEDONPC_41 || Mode == CCam::CamMode::MODE_SNIPER_RUN_AROUND)
				Mode_RunAround = 1;

			/*
				Draw Crosshairs
			*/
			if (TheCamera.Cams->Using3rdPersonMouseCam() && (!CPad::GetPad(0)->GetLookBehindForPed() || TheCamera.m_bPlayerIsInGarage) || Mode == 40) {
				if (CWorld::Players[CWorld::PlayerInFocus].m_pPed) {
					int32 State = CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_nPedState;
					if (State != PED_ENTER_CAR && State != PED_CARJACK) {
						if ((WeaponType >= WEAPONTYPE_COLT45 && WeaponType <= WEAPONTYPE_M16) || WeaponType == WEAPONTYPE_FLAMETHROWER)
							Mode_RunAround = 1;
					}
				}
			}

			if (Mode_FirstPerson || Mode_RunAround) {
				RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERLINEAR);

				int32 SpriteBrightLikeADiamond = CHud::SpriteBrightness + 1;
				if (SpriteBrightLikeADiamond > 30)
					SpriteBrightLikeADiamond = 30;

				CHud::SpriteBrightness = SpriteBrightLikeADiamond;

				RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);

				float fStep = sin((CTimer::GetTimeInMilliseconds() & 1023) * 0.0061328127);
				float fMultBright = CHud::SpriteBrightness * 0.03f * (0.25f * fStep + 0.75f);
				CRect rect;

				float fWidescreenOffset[2] = { 0.0f, 0.0f };

				if (CMenuManager::m_PrefsUseWideScreen) {
					fWidescreenOffset[0] = 0.0f;
					fWidescreenOffset[1] = SCREEN_SCALE_Y(18.0f);
				}

				if (Mode_RunAround && TheCamera.Cams->Using3rdPersonMouseCam()) {
					float f3rdX = SCREENW * TheCamera.m_f3rdPersonCHairMultX + fWidescreenOffset[0];
					float f3rdY = SCREENH * TheCamera.m_f3rdPersonCHairMultY - fWidescreenOffset[1];

					if (CWorld::Players[CWorld::PlayerInFocus].m_pPed && WeaponType == WEAPONTYPE_M16) {
						rect.left = f3rdX - SCREEN_SCALE_X(32.0f * 0.6f);
						rect.top = f3rdY - SCREEN_SCALE_Y(32.0f  * 0.6f);
						rect.right = f3rdX + SCREEN_SCALE_X(32.0f * 0.6f);
						rect.bottom = f3rdY + SCREEN_SCALE_Y(32.0f  * 0.6f);

						CHud::Sprites[HUD_SITEM16].Draw(CRect(rect), CRGBA(255, 255, 255, 255));
					}
					else {
						rect.left = f3rdX - SCREEN_SCALE_X(32.0f * 0.4f);
						rect.top = f3rdY - SCREEN_SCALE_Y(32.0f  * 0.4f);
						rect.right = f3rdX + SCREEN_SCALE_X(32.0f * 0.4f);
						rect.bottom = f3rdY + SCREEN_SCALE_Y(32.0f  * 0.4f);

						CHud::Sprites[HUD_SITEM16].Draw(CRect(rect), CRGBA(255, 255, 255, 255));
					}
				}
				else {
					if (Mode != CCam::CamMode::MODE_M16FIRSTPERSON_34 && Mode != CCam::CamMode::MODE_FIRSTPERSONPEDONPC_41 && Mode != CCam::CamMode::MODE_EDITOR) {
						if (Mode == CCam::CamMode::MODE_ROCKET_RUN_AROUND) {
							rect.left = (SCREEN_WIDTH / 2) - SCREEN_SCALE_X(32.0f * 0.7f);
							rect.top = (SCREEN_HEIGHT / 2) - SCREEN_SCALE_Y(32.0f * 0.7f);
							rect.right = (SCREEN_WIDTH / 2) + SCREEN_SCALE_X(32.0f * 0.7f);
							rect.bottom = (SCREEN_HEIGHT / 2) + SCREEN_SCALE_Y(32.0f * 0.7f);

							CHud::Sprites[HUD_SITEM16].Draw(CRect(rect), CRGBA(255, 255, 255, 255));
						}
						else if (Mode != CCam::CamMode::MODE_ROCKET && Mode != CCam::CamMode::MODE_SNIPER_RUN_AROUND) {
							rect.left = (SCREEN_WIDTH / 2) - SCREEN_SCALE_X(210.0f);
							rect.top = (SCREEN_HEIGHT / 2) - SCREEN_SCALE_Y(210.0f);
							rect.right = SCREEN_WIDTH / 2;
							rect.bottom = SCREEN_HEIGHT / 2;
							CHud::Sprites[HUD_SITESNIPER].Draw(CRect(rect), CRGBA(255, 255, 255, 255));

							rect.right = (SCREEN_WIDTH / 2);
							rect.top = (SCREEN_HEIGHT / 2) - SCREEN_SCALE_Y(210.0f);
							rect.left = SCREEN_SCALE_X(210.0f) + (SCREEN_WIDTH / 2);
							rect.bottom = SCREEN_HEIGHT / 2;
							CHud::Sprites[HUD_SITESNIPER].Draw(CRect(rect), CRGBA(255, 255, 255, 255));

							rect.left = (SCREEN_WIDTH / 2) - SCREEN_SCALE_X(210.0f);
							rect.bottom = (SCREEN_HEIGHT / 2);
							rect.right = (SCREEN_WIDTH / 2);
							rect.top = SCREEN_SCALE_Y(210.0f) + (SCREEN_HEIGHT / 2);
							CHud::Sprites[HUD_SITESNIPER].Draw(CRect(rect), CRGBA(255, 255, 255, 255));

							rect.right = (SCREEN_WIDTH / 2);
							rect.bottom = (SCREEN_HEIGHT / 2);
							rect.left = SCREEN_SCALE_X(210.0f) + (SCREEN_WIDTH / 2);
							rect.top = SCREEN_SCALE_Y(210.0f) + (SCREEN_HEIGHT / 2);
							CHud::Sprites[HUD_SITESNIPER].Draw(CRect(rect), CRGBA(255, 255, 255, 255));
						}
						else {
							RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);
							RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDONE);
							RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDONE);
							RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
							RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
							RwRenderStateSet(rwRENDERSTATETEXTURERASTER, gpRocketSightTex->raster);

							CSprite::RenderOneXLUSprite(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 1.0f, SCREEN_SCALE_X(40.0f), SCREEN_SCALE_Y(40.0f), (100.0f * fMultBright), (200.0f * fMultBright), (100.0f * fMultBright), 255, 1.0f, 255);
						}
					}
					else {
						rect.left = (SCREEN_WIDTH / 2) - SCREEN_SCALE_X(32.0f);
						rect.top = (SCREEN_HEIGHT / 2) - SCREEN_SCALE_Y(32.0f);
						rect.right = (SCREEN_WIDTH / 2) + SCREEN_SCALE_X(32.0f);
						rect.bottom = (SCREEN_HEIGHT / 2) + SCREEN_SCALE_Y(32.0f);
						CHud::Sprites[HUD_SITEM16].Draw(CRect(rect), CRGBA(255, 255, 255, 255));
					}
				}
				RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERLINEAR);
				RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
				RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);
			}
			else {
				CHud::SpriteBrightness = 0;
			}

			/*
				DrawMoneyCounter
			*/
			wchar sPrint[16];
			wchar sPrintIcon[16];
			char sTemp[16];

			sprintf(sTemp, "$%08d", CWorld::Players[CWorld::PlayerInFocus].m_nVisibleMoney);
			AsciiToUnicode(sTemp, sPrint);

			CFont::SetPropOff();
			CFont::SetBackgroundOff();
			CFont::SetScale(SCREEN_SCALE_X(0.8f), SCREEN_SCALE_Y(1.35f));
			CFont::SetCentreOff();
			CFont::SetRightJustifyOn();
			CFont::SetRightJustifyWrap(0.0f);
			CFont::SetBackGroundOnlyTextOff();
			CFont::SetFontStyle(FONT_HEADING);
			CFont::SetPropOff();
			CFont::SetColor(CRGBA(0, 0, 0, 255));

			CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(110.0f - 2.0f), SCREEN_SCALE_Y(43.0f + 2.0f), sPrint);

			CFont::SetColor(CRGBA(89, 115, 150, 255));
			CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(110.0f), SCREEN_SCALE_Y(43.0f), sPrint);

			/*
				DrawClock
			*/
			sprintf(sTemp, "%02d:%02d", CClock::GetHours(), CClock::GetMinutes());
			AsciiToUnicode(sTemp, sPrint);

			CFont::SetColor(CRGBA(0, 0, 0, 255));

			CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(111.0f - 2.0f), SCREEN_SCALE_Y(22.0f + 2.0f), sPrint);

			CFont::SetColor(CRGBA(194, 165, 120, 255));
			CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(111.0f), SCREEN_SCALE_Y(22.0f), sPrint);

			/*
				DrawAmmo
			*/
			int32 AmmoInClip = CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_weapons[CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_currentWeapon].m_nAmmoInClip;
			int32 TotalAmmo = CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_weapons[CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_currentWeapon].m_nAmmoTotal;

			if (AmmoInClip <= 1 || AmmoInClip >= 1000) {
				sprintf(sTemp, "%d", TotalAmmo);
			}
			else {
				if (WeaponType == WEAPONTYPE_FLAMETHROWER) {
					int tot_min_clip_div_10 = (TotalAmmo - AmmoInClip) / 10;
					if (tot_min_clip_div_10 > 9999)
						tot_min_clip_div_10 = 9999;

					sprintf(sTemp, "%d-%d", tot_min_clip_div_10, AmmoInClip / 10);
				}
				else {
					if (AmmoInClip > 9999)
						AmmoInClip = 9999;
					sprintf(sTemp, "%d-%d", (TotalAmmo - AmmoInClip), AmmoInClip);
				}
			}

			AsciiToUnicode(sTemp, sPrint);

			CFont::SetBackgroundOff();
			CFont::SetScale(SCREEN_SCALE_X(0.4f), SCREEN_SCALE_Y(0.6f));
			CFont::SetJustifyOff();
			CFont::SetCentreOn();
			CFont::SetCentreSize(SCREEN_SCALE_X(640.0f));
			CFont::SetPropOn();
			CFont::SetFontStyle(FONT_BANK);

			if (!CDarkel::FrenzyOnGoing()) {
				if (WeaponType) {
					if (WeaponType != 1) {
						CFont::SetColor(CRGBA(0, 0, 0, 255));
						CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(66.0f), SCREEN_SCALE_Y(73.0f), sPrint);
					}
				}
			}

			/*
				DrawWeaponIcon
			*/
			CHud::Sprites[WeaponType].Draw(
				CRect(SCREEN_SCALE_FROM_RIGHT(99.0f), SCREEN_SCALE_Y(27.0f), SCREEN_SCALE_FROM_RIGHT(35.0f), SCREEN_SCALE_Y(91.0f)),
				CRGBA(255, 255, 255, 255),
				0.015f,
				0.015f,
				1.0f,
				0.0f,
				0.015f,
				1.0f,
				1.0f,
				1.0f);

			/*
				DrawHealth
			*/
			CFont::SetBackgroundOff();
			CFont::SetScale(SCREEN_SCALE_X(0.8f), SCREEN_SCALE_Y(1.35f));
			CFont::SetJustifyOff();
			CFont::SetCentreOff();
			CFont::SetRightJustifyWrap(0.0f);
			CFont::SetRightJustifyOn();
			CFont::SetPropOff();
			CFont::SetFontStyle(FONT_HEADING);

			if (CHud::m_ItemToFlash == ITEM_HEALTH && CTimer::GetFrameCounter() & 8
				|| CHud::m_ItemToFlash != ITEM_HEALTH
				|| CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_fHealth < 10
				&& CTimer::GetFrameCounter() & 8) {
				if (CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_fHealth >= 10
					|| CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_fHealth < 10 && CTimer::GetFrameCounter() & 8) {

					AsciiToUnicode("{", sPrintIcon);
					sprintf(sTemp, "%03d", (int32)CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_fHealth);
					AsciiToUnicode(sTemp, sPrint);

					CFont::SetColor(CRGBA(0, 0, 0, 255));
					CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(110.0f - 2.0f), SCREEN_SCALE_Y(65.0f + 2.0f), sPrint);

					if (!CWorld::Players[CWorld::PlayerInFocus].m_nTimeLastHealthLoss || CTimer::GetTimeInMilliseconds() > CWorld::Players[CWorld::PlayerInFocus].m_nTimeLastHealthLoss + 2000 || CTimer::GetFrameCounter() & 4) {
						CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(164.0f - 2.0f), SCREEN_SCALE_Y(65.0f + 2.0f), sPrintIcon);
					}
					CFont::SetColor(CRGBA(186, 101, 50, 255));

					CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(110.0f), SCREEN_SCALE_Y(65.0f), sPrint);

					if (!CWorld::Players[CWorld::PlayerInFocus].m_nTimeLastHealthLoss || CTimer::GetTimeInMilliseconds() > CWorld::Players[CWorld::PlayerInFocus].m_nTimeLastHealthLoss + 2000 || CTimer::GetFrameCounter() & 4) {
						CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(164.0f), SCREEN_SCALE_Y(65.0f), sPrintIcon);
					}
				}
			}

			/*
				DrawArmour
			*/
			if (CHud::m_ItemToFlash == ITEM_ARMOUR && CTimer::GetFrameCounter() & 8 || CHud::m_ItemToFlash != ITEM_ARMOUR) {
				CFont::SetScale(SCREEN_SCALE_X(0.8f), SCREEN_SCALE_Y(1.35f));
				if (CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_fArmour > 1.0f) {
					AsciiToUnicode("[", sPrintIcon);
					sprintf(sTemp, "%03d", (int32)CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_fArmour);
					AsciiToUnicode(sTemp, sPrint);

					CFont::SetColor(CRGBA(0, 0, 0, 255));
					CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(182.0f - 2.0f), SCREEN_SCALE_Y(65.0f + 2.0f), sPrint);

					if (!CWorld::Players[CWorld::PlayerInFocus].m_nTimeLastArmourLoss || CTimer::GetTimeInMilliseconds() > CWorld::Players[CWorld::PlayerInFocus].m_nTimeLastArmourLoss + 2000 || CTimer::GetFrameCounter() & 4) {
						CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(234.0f - 2.0f), SCREEN_SCALE_Y(65.0f + 2.0f), sPrintIcon);
					}

					CFont::SetColor(CRGBA(124, 140, 95, 255));

					CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(182.0f), SCREEN_SCALE_Y(65.0f), sPrint);

					if (!CWorld::Players[CWorld::PlayerInFocus].m_nTimeLastArmourLoss || CTimer::GetTimeInMilliseconds() > CWorld::Players[CWorld::PlayerInFocus].m_nTimeLastArmourLoss + 2000 || CTimer::GetFrameCounter() & 1) {
						CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(234.0f), SCREEN_SCALE_Y(65.0f), sPrintIcon);
					}
				}
			}

			/*
				DrawWantedLevel
			*/
			CFont::SetBackgroundOff();
			CFont::SetScale(SCREEN_SCALE_X(0.8f), SCREEN_SCALE_Y(1.35f));
			CFont::SetJustifyOff();
			CFont::SetCentreOff();
			CFont::SetRightJustifyOff();
			CFont::SetPropOn();
			CFont::SetFontStyle(FONT_HEADING);

			AsciiToUnicode("]", sPrintIcon);

			for (int i = 0; i < 6; i++) {
				CFont::SetColor(CRGBA(0, 0, 0, 255));
				CFont::PrintString(2.0f + SCREEN_SCALE_FROM_RIGHT(60.0f - 2.0f + 23.0f * i), SCREEN_SCALE_Y(87.0f + 2.0f), sPrintIcon);
				if (CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_pWanted->m_nWantedLevel > i
					&& (CTimer::GetTimeInMilliseconds() > CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_pWanted->m_nLastWantedLevelChange
						+ 2000 || CTimer::GetFrameCounter() & 4)) {

					CFont::SetColor(CRGBA(193, 164, 120, 255));
					CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(60.0f + 23.0f * i), SCREEN_SCALE_Y(87.0f), sPrintIcon);
				}
			}

			/*
				DrawZoneName
			*/
			if (CHud::m_ZoneName) {
				float fZoneAlpha = 0.0f;

				if (CHud::m_ZoneName != CHud::m_LastZoneName) {
					switch (CHud::m_ZoneState) {
					case 0:
						CHud::m_ZoneState = 2;
						CHud::m_ZoneToPrint = CHud::m_ZoneName;
						CHud::m_ZoneNameTimer = 0;
						CHud::m_ZoneFadeTimer = 0;
						break;
					case 1:
					case 2:
					case 3:
					case 4:
						CHud::m_ZoneNameTimer = 0;
						CHud::m_ZoneState = 4;
						break;
					default:
						break;
					}
					CHud::m_LastZoneName = CHud::m_ZoneName;
				}

				if (CHud::m_ZoneState) {
					switch (CHud::m_ZoneState) {
					case 1:
						if (CHud::m_ZoneNameTimer > 10000) {
							CHud::m_ZoneFadeTimer = 1000;
							CHud::m_ZoneState = 3;
						}
						fZoneAlpha = 255.0f;
						break;
					case 2:
						CHud::m_ZoneFadeTimer += (CTimer::GetTimeStep() * 0.02f * 1000.0f);
						if (CHud::m_ZoneFadeTimer > 1000) {
							CHud::m_ZoneState = 1;
							CHud::m_ZoneFadeTimer = 1000;
						}
						fZoneAlpha = CHud::m_ZoneFadeTimer * 0.001f * 255.0f;
						break;
					case 3:
						CHud::m_ZoneFadeTimer += (CTimer::GetTimeStep() * 0.02f * -1000.0f);
						if (CHud::m_ZoneFadeTimer < 0) {
							CHud::m_ZoneState = 0;
							CHud::m_ZoneFadeTimer = 0;
						}
						fZoneAlpha = CHud::m_ZoneFadeTimer * 0.001f * 255.0f;
						break;
					case 4:
						CHud::m_ZoneFadeTimer += (CTimer::GetTimeStep() * 0.02f * -1000.0f);
						if (CHud::m_ZoneFadeTimer < 0) {
							CHud::m_ZoneFadeTimer = 0;
							CHud::m_ZoneToPrint = CHud::m_LastZoneName;
							CHud::m_ZoneNameTimer = 0;
							CHud::m_ZoneState = 2;
						}
						fZoneAlpha = CHud::m_ZoneFadeTimer * 0.001f * 255.0f;
						break;
					default:
						break;

					}
					if (!CHud::m_Message[0]) {
						CHud::m_ZoneNameTimer += (CTimer::GetTimeStep() * 0.02f * 1000.0f);
						CFont::SetJustifyOff();
						CFont::SetPropOn();
						CFont::SetBackgroundOff();

						if (CMenuManager::m_PrefsLanguage == 4)
							CFont::SetScale(SCREEN_SCALE_X(1.2f * 0.8f), SCREEN_SCALE_Y(1.2f));
						else
							CFont::SetScale(SCREEN_SCALE_X(1.2f), SCREEN_SCALE_Y(1.2f));

						CFont::SetRightJustifyOn();
						CFont::SetRightJustifyWrap(0.0f);
						CFont::SetBackGroundOnlyTextOff();
						CFont::SetFontStyle(FONT_BANK);
						CFont::SetColor(CRGBA(0, 0, 0, fZoneAlpha));
						CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(32.0f - 1.0f), SCREEN_SCALE_FROM_BOTTOM(30.0f - 1.0f), CHud::m_ZoneToPrint);

						CFont::SetColor(CRGBA(152, 154, 82, fZoneAlpha));
						CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(32.0f), SCREEN_SCALE_FROM_BOTTOM(30.0f), CHud::m_ZoneToPrint);
					}
				}
			}
			else {
				CHud::m_LastZoneName = 0;
				CHud::m_ZoneState = 0;
				CHud::m_ZoneFadeTimer = 0;
				CHud::m_ZoneNameTimer = 0;
			}

			/*
				DrawVehicleName
			*/
			if (CHud::m_VehicleName) {
				float fVehicleAlpha = 0.0f;

				if (CHud::m_VehicleName != CHud::m_LastVehicleName) {
					switch (CHud::m_VehicleState) {
					case 0:
						CHud::m_VehicleState = 2;
						CHud::m_VehicleNameToPrint = CHud::m_VehicleName;
						CHud::m_VehicleNameTimer = 0;
						CHud::m_VehicleFadeTimer = 0;
						break;
					case 1:
					case 2:
					case 3:
					case 4:
						CHud::m_VehicleNameTimer = 0;
						CHud::m_VehicleState = 4;
						break;
					default:
						break;
					}
					CHud::m_LastVehicleName = CHud::m_VehicleName;
				}

				if (CHud::m_VehicleState) {
					switch (CHud::m_VehicleState) {
					case 1:
						if (CHud::m_VehicleNameTimer > 10000) {
							CHud::m_VehicleFadeTimer = 1000;
							CHud::m_VehicleState = 3;
						}
						fVehicleAlpha = 255.0f;
						break;
					case 2:
						CHud::m_VehicleFadeTimer += (CTimer::GetTimeStep() * 0.02f * 1000.0f);
						if (CHud::m_VehicleFadeTimer > 1000) {
							CHud::m_VehicleState = 1;
							CHud::m_VehicleFadeTimer = 1000;
						}
						fVehicleAlpha = CHud::m_VehicleFadeTimer * 0.001f * 255.0f;
						break;
					case 3:
						CHud::m_VehicleFadeTimer += (CTimer::GetTimeStep() * 0.02f * -1000.0f);
						if (CHud::m_VehicleFadeTimer < 0) {
							CHud::m_VehicleState = 0;
							CHud::m_VehicleFadeTimer = 0;
						}
						fVehicleAlpha = CHud::m_VehicleFadeTimer * 0.001f * 255.0f;
						break;
					case 4:
						CHud::m_VehicleFadeTimer += (CTimer::GetTimeStep() * 0.02f * -1000.0f);
						if (CHud::m_VehicleFadeTimer < 0) {
							CHud::m_VehicleFadeTimer = 0;
							CHud::m_VehicleNameToPrint = CHud::m_LastVehicleName;
							CHud::m_VehicleNameTimer = 0;
							CHud::m_VehicleState = 2;
						}
						fVehicleAlpha = CHud::m_VehicleFadeTimer * 0.001f * 255.0f;
						break;
					default:
						break;
					}

					if (!CHud::m_Message[0]) {
						CHud::m_VehicleNameTimer += (CTimer::GetTimeStep() * 0.02f * 1000.0f);
						CFont::SetJustifyOff();
						CFont::SetPropOn();
						CFont::SetBackgroundOff();

						if (CMenuManager::m_PrefsLanguage != 3 && CMenuManager::m_PrefsLanguage != 4)
							CFont::SetScale(SCREEN_SCALE_X(1.2f), SCREEN_SCALE_Y(1.2f));
						else
							CFont::SetScale(SCREEN_SCALE_X(1.2f * 0.85f), SCREEN_SCALE_Y(1.2f));

						CFont::SetRightJustifyOn();
						CFont::SetRightJustifyWrap(0.0f);
						CFont::SetBackGroundOnlyTextOff();
						CFont::SetFontStyle(FONT_BANK);
						CFont::SetColor(CRGBA(0, 0, 0, fVehicleAlpha));
						CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(32.0f - 1.0f), SCREEN_SCALE_FROM_BOTTOM(55.0f - 1.0f), CHud::m_VehicleNameToPrint);

						CFont::SetColor(CRGBA(194, 165, 120, fVehicleAlpha));
						CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(32.0f), SCREEN_SCALE_FROM_BOTTOM(55.0f), CHud::m_VehicleNameToPrint);
					}
				}
			}
			else {
				CHud::m_LastVehicleName = 0;
				CHud::m_VehicleState = 0;
				CHud::m_VehicleFadeTimer = 0;
				CHud::m_VehicleNameTimer = 0;
			}

			/*
				DrawOnScreenTimer
			*/
			wchar sTimer[16];
			if (!CUserDisplay::OnscnTimer.m_sEntries[0].m_bTimerProcessed)
				TimerOnLastFrame = 0;
			if (!CUserDisplay::OnscnTimer.m_sEntries[0].m_bCounterProcessed)
				CounterOnLastFrame = 0;

			if (CUserDisplay::OnscnTimer.m_bProcessed == 1) {
				if (CUserDisplay::OnscnTimer.m_sEntries[0].m_bTimerProcessed == 1) {
					if (!TimerOnLastFrame)
						TimerFlashTimer = 1;

					TimerOnLastFrame = 1;

					if (TimerFlashTimer) {
						if (++TimerFlashTimer > 50)
							TimerFlashTimer = 0;
					}

					if (CTimer::GetFrameCounter() & 4 || !TimerFlashTimer) {
						AsciiToUnicode(CUserDisplay::OnscnTimer.m_sEntries[0].m_bTimerBuffer, sTimer);
						CFont::SetPropOn();
						CFont::SetBackgroundOff();
						CFont::SetScale(SCREEN_SCALE_X(0.8f), SCREEN_SCALE_Y(1.35f));
						CFont::SetRightJustifyOn();
						CFont::SetRightJustifyWrap(0.0f);
						CFont::SetFontStyle(FONT_HEADING);
						CFont::SetPropOff();
						CFont::SetBackGroundOnlyTextOn();
						CFont::SetColor(CRGBA(0, 0, 0, 255));
						CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(27.0f - 2.0f), SCREEN_SCALE_Y(110.0f + 2.0f), sTimer);

						CFont::SetScale(SCREEN_SCALE_X(0.8f), SCREEN_SCALE_Y(1.35f));
						CFont::SetColor(CRGBA(186, 101, 50, 255));
						CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(27.0f), SCREEN_SCALE_Y(110.0f), sTimer);

						if (CUserDisplay::OnscnTimer.m_sEntries[0].m_aTimerText[0]) {
							CFont::SetPropOn();
							CFont::SetColor(CRGBA(0, 0, 0, 255));
							CFont::SetScale(SCREEN_SCALE_X(0.8f), SCREEN_SCALE_Y(1.35f));
							CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(27.0f + 78.0f), SCREEN_SCALE_Y(110.0f + 2.0f), TheText.Get(CUserDisplay::OnscnTimer.m_sEntries[0].m_aTimerText));

							CFont::SetColor(CRGBA(186, 101, 50, 255));
							CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(27.0f + 80.0f), SCREEN_SCALE_Y(110.0f), TheText.Get(CUserDisplay::OnscnTimer.m_sEntries[0].m_aTimerText));
						}
					}
				}
				if (CUserDisplay::OnscnTimer.m_sEntries[0].m_bCounterProcessed == 1) {
					if (!CounterOnLastFrame)
						CounterFlashTimer = 1;

					CounterOnLastFrame = 1;

					if (CounterFlashTimer) {
						if (++CounterFlashTimer > 50)
							CounterFlashTimer = 0;
					}

					if (CTimer::GetFrameCounter() & 4 || !CounterFlashTimer) {
						if (CUserDisplay::OnscnTimer.m_sEntries[0].m_nType) {
							CSprite2d::DrawRect(CRect(SCREEN_SCALE_FROM_RIGHT(127.0f - 4.0f), SCREEN_SCALE_Y(132.0 + 8.0f), SCREEN_SCALE_FROM_RIGHT(23.0f), SCREEN_SCALE_Y(11.0f + 132.0f + 8.0f)), CRGBA(0, 106, 164, 80));
							CSprite2d::DrawRect(CRect(SCREEN_SCALE_FROM_RIGHT(127.0f + 4.0f), SCREEN_SCALE_Y(132.0 + 8.0f), SCREEN_SCALE_FROM_RIGHT(atoi(CUserDisplay::OnscnTimer.m_sEntries[0].m_bCounterBuffer) + 27.0f + 100.0f + 4.0f), SCREEN_SCALE_Y(11.0f + 132.0f + 8.0f)), CRGBA(0, 106, 164, 255));
						}
						else {
							AsciiToUnicode(CUserDisplay::OnscnTimer.m_sEntries[0].m_bCounterBuffer, sTimer);

							CFont::SetPropOn();
							CFont::SetBackgroundOff();
							CFont::SetScale(SCREEN_SCALE_X(0.8f), SCREEN_SCALE_Y(1.35f));
							CFont::SetCentreOff();
							CFont::SetRightJustifyOn();
							CFont::SetRightJustifyWrap(0.0f);
							CFont::SetFontStyle(FONT_HEADING);
							CFont::SetColor(CRGBA(244, 20, 20, 255));
							CFont::SetWrapx(SCREEN_SCALE_X(640.0f));
							CFont::SetPropOff();
							CFont::SetBackGroundOnlyTextOn();
							CFont::SetColor(CRGBA(0, 0, 0, 255));
							CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(27.0f - 2.0f), SCREEN_SCALE_Y(132.0f + 2.0f), sTimer);

							CFont::SetColor(CRGBA(0, 106, 164, 255));
							CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(27.0f), SCREEN_SCALE_Y(132.0f), sTimer);
						}

						if (CUserDisplay::OnscnTimer.m_sEntries[0].m_aCounterText[0]) {
							CFont::SetPropOn();
							CFont::SetScale(SCREEN_SCALE_X(0.8f), SCREEN_SCALE_Y(1.35f));

							CFont::SetColor(CRGBA(0, 0, 0, 255));
							CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(27.0f + 59.0f), SCREEN_SCALE_Y(132.0f + 2.0f), TheText.Get(CUserDisplay::OnscnTimer.m_sEntries[0].m_aCounterText));

							CFont::SetColor(CRGBA(0, 106, 164, 255));
							CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(27.0f + 61.0f), SCREEN_SCALE_Y(132.0f), TheText.Get(CUserDisplay::OnscnTimer.m_sEntries[0].m_aCounterText));
						}
					}
				}
			}

			/*
				DrawPager
			*/
			if (!m_PagerMessage[0]) {
				if (PagerOn == 1) {
					PagerSoundPlayed = false;
					PagerOn = 2;
				}
			}
			if (m_PagerMessage[0] || PagerOn == 2) {
				if (!PagerOn) {
					PagerOn = 1;
					PagerXOffset = 150.0f;
				}
				if (PagerOn == 1) {
					if (PagerXOffset > 0.0f) {
						float fStep = PagerXOffset * 0.05f;
						if (fStep > 10.0f)
							fStep = 10.0f;
						PagerXOffset -= fStep * CTimer::GetTimeStep();
					}
					if (!PagerSoundPlayed) {
						DMAudio.PlayFrontEndSound(96, 0);
						PagerSoundPlayed = 1;
					}
				}
				else if (PagerOn == 2) {
					float fStep = PagerXOffset * 0.05f;
					if (fStep < 2.0f)
						fStep = 2.0f;
					PagerXOffset += fStep * CTimer::GetTimeStep();
					if (PagerXOffset > 150.0f) {
						PagerXOffset = 150.0;
						PagerOn = 0;
					}
				}

				CHud::Sprites[HUD_PAGER].Draw(CRect(SCREEN_SCALE_X(26.0f - PagerXOffset), SCREEN_SCALE_Y(27.0f), SCREEN_SCALE_X(160.0 + 26.0f - PagerXOffset), SCREEN_SCALE_Y(80.0f + 27.0f)), CRGBA(255, 255, 255, 255));

				CFont::SetBackgroundOff();
				CFont::SetScale(SCREEN_SCALE_X(0.84f), SCREEN_SCALE_Y(1.0f));
				CFont::SetColor(CRGBA(32, 162, 66, 205));
				CFont::SetRightJustifyOff();
				CFont::SetBackgroundOff();
				CFont::SetCentreOff();
				CFont::SetJustifyOff();
				CFont::SetPropOff();
				CFont::SetFontStyle(FONT_PAGER);
				CFont::PrintString(SCREEN_SCALE_X(52.0f - PagerXOffset), SCREEN_SCALE_Y(54.0f), CHud::m_PagerMessage);
			}

			/*
				DrawRadar
			*/
			if (CHud::m_ItemToFlash == ITEM_RADAR && CTimer::GetFrameCounter() & 8 || CHud::m_ItemToFlash != ITEM_RADAR) {
				CRadar::DrawMap();
				CHud::Sprites[HUD_RADARDISC].Draw(CRect(SCREEN_SCALE_X(16.0f), SCREEN_SCALE_FROM_BOTTOM(123.0f + 4.0f), SCREEN_SCALE_X(94.0f + 20.0f + 5.0f), SCREEN_SCALE_FROM_BOTTOM(-76.0f + 123.0f - 6.0f)), CRGBA(0, 0, 0, 255));
				CRadar::DrawBlips();
			}
		}

		/*
			Draw3dMarkers
		*/
		if (CHud::m_Wants_To_Draw_3dMarkers && !TheCamera.m_WideScreenOn && !CHud::m_BigMessage[0][0] && !CHud::m_BigMessage[2][0]) {
			CRadar::Draw3dMarkers();
		}

		/*
			DrawScriptText
		*/
		if (!CTimer::GetIsUserPaused()) {
			CTextLine* IntroText = CTheScripts::IntroTextLines;

			for (int i = 0; i < 2; i++) {
				if (CTheScripts::IntroTextLines[i].m_awText[0] && CTheScripts::IntroTextLines[i].field_29) {
					CFont::SetScale(SCREEN_SCALE_X(CTheScripts::IntroTextLines[i].m_fScaleX), SCREEN_SCALE_Y(CTheScripts::IntroTextLines[i].m_fScaleY * 0.5f));
					CFont::SetColor(CTheScripts::IntroTextLines[i].m_sColor);

					if (CTheScripts::IntroTextLines[i].m_bJustify)
						CFont::SetJustifyOn();
					else
						CFont::SetJustifyOff();

					if (CTheScripts::IntroTextLines[i].m_bRightJustify)
						CFont::SetRightJustifyOn();
					else
						CFont::SetRightJustifyOff();

					if (CTheScripts::IntroTextLines[i].m_bCentered)
						CFont::SetCentreOn();
					else
						CFont::SetCentreOff();

					CFont::SetWrapx(SCREEN_SCALE_X(CTheScripts::IntroTextLines[i].m_fWrapX));
					CFont::SetCentreSize(SCREEN_SCALE_X(CTheScripts::IntroTextLines[i].m_fCenterSize));

					if (CTheScripts::IntroTextLines[i].m_bBackground)
						CFont::SetBackgroundOn();
					else
						CFont::SetBackgroundOff();

					CFont::SetBackgroundColor(CTheScripts::IntroTextLines[i].m_sBackgroundColor);

					if (CTheScripts::IntroTextLines[i].m_bBackgroundOnly)
						CFont::SetBackGroundOnlyTextOn();
					else
						CFont::SetBackGroundOnlyTextOff();

					if (CTheScripts::IntroTextLines[i].m_bTextProportional)
						CFont::SetPropOn();
					else
						CFont::SetPropOff();

					CFont::SetFontStyle(CTheScripts::IntroTextLines[i].m_nFont);
					CFont::PrintString(SCREEN_SCALE_X(640.0f - CTheScripts::IntroTextLines[i].field_36), SCREEN_SCALE_Y(448.0f - CTheScripts::IntroTextLines[i].field_40), IntroText->m_awText);
				}
			}

			CScriptRectangle* IntroRect = CTheScripts::IntroRectangles;

			for (int i = 0; i < 16; i++) {
				if (CTheScripts::IntroRectangles[i].m_bIsUsed && CTheScripts::IntroRectangles[i].m_bIsAntialiased) {
					if (CTheScripts::IntroRectangles[i].m_wTextureId >= 0) {
						CRect rect = {
							CTheScripts::IntroRectangles[i].m_sRect.left,
							CTheScripts::IntroRectangles[i].m_sRect.bottom,
							CTheScripts::IntroRectangles[i].m_sRect.right,
							CTheScripts::IntroRectangles[i].m_sRect.bottom };

						CTheScripts::ScriptSprites[CTheScripts::IntroRectangles[i].m_wTextureId].Draw(rect, IntroRect->m_sColor);
					}
					else {
						CRect rect = {
							CTheScripts::IntroRectangles[i].m_sRect.left,
							CTheScripts::IntroRectangles[i].m_sRect.bottom,
							CTheScripts::IntroRectangles[i].m_sRect.right,
							CTheScripts::IntroRectangles[i].m_sRect.bottom };

						CSprite2d::DrawRect(rect, IntroRect->m_sColor);
					}
				}
			}

			/*
				DrawSubtitles
			*/
			if (CHud::m_Message[0] && !CHud::m_BigMessage[2][0] && (FrontEndMenuManager.m_PrefsShowSubtitles == 1 || !TheCamera.m_WideScreenOn)) {
				CFont::SetJustifyOff();
				CFont::SetBackgroundOff();
				CFont::SetBackgroundColor(CRGBA(0, 0, 0, 128));
				CFont::SetScale(SCREEN_SCALE_X(0.48f), SCREEN_SCALE_Y(1.120f));
				CFont::SetCentreOn();
				CFont::SetPropOn();
				CFont::SetFontStyle(FONT_BANK);

				if (TheCamera.m_WideScreenOn)
					CFont::SetCentreSize(SCREEN_SCALE_FROM_RIGHT(120.0f));
				else
					CFont::SetCentreSize(SCREEN_SCALE_FROM_RIGHT(280.0f));

				CFont::SetDropShadowPosition(1);
				CFont::SetDropColor(CRGBA(0, 0, 0, 255));
				CFont::SetColor(CRGBA(235, 235, 235, 255));
				CFont::PrintString(SCREEN_WIDTH / 2, SCREEN_SCALE_FROM_BOTTOM(64.0f), CHud::m_Message);
				CFont::SetDropShadowPosition(0);
			}

			/*
				DrawBigMessage
			*/
			// MissionCompleteFailedText
			if (CHud::m_BigMessage[0][0]) {
				if (BigMessageInUse[0] == 0.0f) {
					CFont::SetJustifyOff();
					CFont::SetBackgroundOff();
					CFont::SetBackGroundOnlyTextOff();
					CFont::SetScale(SCREEN_SCALE_X(1.8f), SCREEN_SCALE_Y(1.8f));
					CFont::SetPropOn();
					CFont::SetCentreOn();
					CFont::SetCentreSize(SCREEN_SCALE_X(615.0f));
					CFont::SetColor(CRGBA(255, 255, 0, 255));
					CFont::SetFontStyle(FONT_HEADING);

					if (BigMessageX[0] >= (SCREENW - 20)) {
						BigMessageAlpha[0] += (CTimer::GetTimeStep() * 0.02f * -255.0f);

						if (BigMessageAlpha[0] <= 0.0f) {
							BigMessageAlpha[0] = 0.0f;
							BigMessageInUse[0] = 1.0f;
						}
					}
					else {
						BigMessageX[0] += (CTimer::GetTimeStep() * 0.02f * 255.0f);
						BigMessageAlpha[0] += (CTimer::GetTimeStep() * 0.02f * 255.0f);

						if (BigMessageAlpha[0] >= 255.0f)
							BigMessageAlpha[0] = 255.0f;
					}

					CFont::SetColor(CRGBA(0, 0, 0, BigMessageAlpha[0]));
					CFont::PrintString(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) - SCREEN_SCALE_Y(20.0f - 2.0f), m_BigMessage[0]);

					CFont::SetColor(CRGBA(85, 119, 133, BigMessageAlpha[0]));
					CFont::PrintString(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) - SCREEN_SCALE_Y(20.0f), m_BigMessage[0]);
				}
				else {
					BigMessageAlpha[0] = 0.0;
					BigMessageX[0] = -60.0;
					BigMessageInUse[0] = 1.0;
				}
			}
			else {
				BigMessageInUse[0] = 0.0;
			}

			// WastedBustedText
			if (CHud::m_BigMessage[2][0]) {
				if (BigMessageInUse[2] == 0.0f) {
					BigMessageAlpha[2] += (CTimer::GetTimeStep() * 0.02f * 1000.0f) * 0.4f;

					if (BigMessageAlpha[2] > 255.0f)
						BigMessageAlpha[2] = 255.0;

					CFont::SetBackgroundOff();

					if (CGame::frenchGame || CGame::germanGame)
						CFont::SetScale(SCREEN_SCALE_X(1.4f), SCREEN_SCALE_Y(1.4f));
					else
						CFont::SetScale(SCREEN_SCALE_X(2.0f), SCREEN_SCALE_Y(2.0f));

					CFont::SetPropOn();
					CFont::SetRightJustifyOn();
					CFont::SetFontStyle(FONT_HEADING);
					CFont::SetColor(CRGBA(0, 0, 0, 0.75f * BigMessageAlpha[2]));

					CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(20.0f + 4.0f), SCREEN_SCALE_FROM_BOTTOM(78.0f), CHud::m_BigMessage[2]);

					CFont::SetColor(CRGBA(170, 123, 87, BigMessageAlpha[2]));
					CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(20.0f), SCREEN_SCALE_FROM_BOTTOM(82.0f), CHud::m_BigMessage[2]);
				}
				else {
					BigMessageAlpha[2] = 0.0;
					BigMessageInUse[2] = 1.0;
				}
			}
			else {
				BigMessageInUse[2] = 0.0;
			}
		}
	}
}

void CHud::DrawAfterFade()
{
	if (CTimer::GetIsUserPaused() || CReplay::Mode == 1)
		return;

	if (m_HelpMessage[0]) {
		if (!CMessages::WideStringCompare(m_HelpMessage, m_LastHelpMessage, 256)) {
			switch (m_HelpMessageState) {
			case 0:
				m_HelpMessageFadeTimer = 0;
				m_HelpMessageState = 2;
				m_HelpMessageTimer = 0;
				CMessages::WideStringCopy(m_HelpMessageToPrint, m_HelpMessage, 256);
				m_HelpMessageDisplayTime = CMessages::GetWideStringLength(m_HelpMessage) * 0.05f + 3.0f;

				if (TheCamera.m_ScreenReductionPercentage == 0.0f)
					DMAudio.PlayFrontEndSound(SOUND_A0, 0);
				break;
			case 1:
			case 2:
			case 3:
			case 4:
				m_HelpMessageTimer = 5;
				m_HelpMessageState = 4;
				break;
			default:
				break;
			}
			CMessages::WideStringCopy(m_LastHelpMessage, m_HelpMessage, 256);
		}

		float fAlpha = 255.0f;

		if (m_HelpMessageState) {
			switch (m_HelpMessageState) {
			case 1:
				fAlpha = 255.0f;
				m_HelpMessageFadeTimer = 600;
				if (m_HelpMessageTimer > m_fHelpMessageTime * 1000 || m_HelpMessageQuick && m_HelpMessageTimer > 1500) {
					m_HelpMessageFadeTimer = 600;
					m_HelpMessageState = 3;
				}
				break;
			case 2:
				m_HelpMessageFadeTimer += 2 * (CTimer::GetTimeStep() * 0.02f * 1000.0f);
				if (m_HelpMessageFadeTimer > 0) {
					m_HelpMessageState = 1;
					m_HelpMessageFadeTimer = 0;
				}
				fAlpha = m_HelpMessageFadeTimer * 0.001f * 255.0f;
				break;
			case 3:
				m_HelpMessageFadeTimer -= 2 * (CTimer::GetTimeStep() * 0.02f * 1000.0f);
				if (m_HelpMessageFadeTimer >= 0) {
					m_HelpMessageState = 0;
					m_HelpMessageFadeTimer = 0;
				}
				fAlpha = m_HelpMessageFadeTimer * 0.001f * 255.0f;
				break;
			case 4:
				m_HelpMessageFadeTimer -= 2 * (CTimer::GetTimeStep() * 0.02f * 1000.0f);
				if (m_HelpMessageFadeTimer >= 0) {
					m_HelpMessageState = 2;
					m_HelpMessageFadeTimer = 0;
					CMessages::WideStringCopy(m_HelpMessageToPrint, m_LastHelpMessage, 400);
				}
				fAlpha = m_HelpMessageFadeTimer * 0.001f * 255.0f;
				break;
			default:
				break;
			}

			m_HelpMessageTimer += (CTimer::GetTimeStep() * 0.02f * 1000.0f);

			CFont::SetAlphaFade(fAlpha);
			CFont::SetCentreOff();
			CFont::SetPropOn();

			if (CGame::germanGame)
				CFont::SetScale(SCREEN_SCALE_X(0.52f * 0.85f), SCREEN_SCALE_Y(1.1f * 0.85f));
			else
				CFont::SetScale(SCREEN_SCALE_X(0.52f), SCREEN_SCALE_Y(1.1f));

			CFont::SetColor(CRGBA(175, 175, 175, 255));
			CFont::SetJustifyOff();
			CFont::SetWrapx(SCREEN_SCALE_X(200.0f + 26.0f - 4.0f));
			CFont::SetFontStyle(FONT_BANK);
			CFont::SetBackgroundOn();
			CFont::SetBackGroundOnlyTextOff();
			CRGBA BackColor = { 0, 0, 0, (uint8)(0.9f * fAlpha) };
			CFont::SetBackgroundColor(BackColor);
			CFont::SetColor(CRGBA(175, 175, 175, 255));
			CFont::PrintString(SCREEN_SCALE_X(26.0f), SCREEN_SCALE_Y(28.0f + (150.0f - PagerXOffset) * 0.6f), CHud::m_HelpMessageToPrint);
			CFont::SetAlphaFade(255.0f);
			CFont::DrawFonts();
		}
	}
	else
		m_HelpMessageState = 0;

	/*
		DrawBigMessage2
	*/
	// Oddjob
	if (m_BigMessage[4][0]) {
		CFont::SetJustifyOff();
		CFont::SetBackgroundOff();
		CFont::SetScale(SCREEN_SCALE_X(1.2f), SCREEN_SCALE_Y(1.5f));
		CFont::SetCentreOn();
		CFont::SetPropOn();
		CFont::SetCentreSize(SCREEN_SCALE_X(600.0f));
		CFont::SetFontStyle(FONT_BANK);

		CFont::SetColor(CRGBA(0, 0, 0, 255));
		CFont::PrintString(SCREEN_SCALE_X(2.0f) + (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) - SCREEN_SCALE_Y(84.0f), m_BigMessage[4]);

		CFont::SetColor(CRGBA(89, 115, 150, 255));
		CFont::PrintString((SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) - SCREEN_SCALE_Y(84.0f), m_BigMessage[4]);
	}

	// Oddjob result
	if (OddJob2OffTimer > 0)
		OddJob2OffTimer = OddJob2OffTimer - (CTimer::GetTimeStep() * 0.02f * 1000.0f);

	static float fStep;
	if (!m_BigMessage[1][0] && m_BigMessage[4][0] && m_BigMessage[5][0] && OddJob2OffTimer <= 0.0f) {
		switch (OddJob2On) {
		case 0:
			OddJob2On = 1;
			OddJob2XOffset = 380.0;
			break;
		case 1:
			if (OddJob2XOffset <= 2.0f) {
				OddJob2Timer = 0;
				OddJob2On = 2;
			}
			else {
				fStep = 40.0;
				if ((OddJob2XOffset * 0.16667) <= 40.0)
					fStep = OddJob2XOffset * 0.16667;
				OddJob2XOffset = OddJob2XOffset - fStep;
			}
			break;
		case 2:
			OddJob2Timer += (20.0 * CTimer::GetTimeStep());
			if (OddJob2Timer > 1500) {
				OddJob2On = 3;
			}
			break;
		case 3:
			fStep = 30.0;
			if ((OddJob2XOffset * 0.2) >= 30.0)
				fStep = OddJob2XOffset * 0.2;

			OddJob2XOffset = OddJob2XOffset - fStep;

			if (OddJob2XOffset < -380.0) {
				OddJob2OffTimer = 5000.0;
				OddJob2On = 0;
			}
			break;
		default:
			break;
		}

		CFont::SetJustifyOff();
		CFont::SetBackgroundOff();
		CFont::SetScale(SCREEN_SCALE_X(1.0f), SCREEN_SCALE_Y(1.2f));
		CFont::SetCentreOn();
		CFont::SetPropOn();
		CFont::SetCentreSize(SCREEN_SCALE_FROM_RIGHT(20.0f));
		CFont::SetColor(CRGBA(0, 0, 0, 255));
		CFont::SetFontStyle(FONT_BANK);
		CFont::PrintString(SCREEN_SCALE_X(2.0f) + (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) - SCREEN_SCALE_Y(20.0f + 2.0f), m_BigMessage[5]);

		CFont::SetColor(CRGBA(156, 91, 40, 255));
		CFont::PrintString((SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) - SCREEN_SCALE_Y(20.0f + 2.0f), m_BigMessage[5]);
	}

	/*
		DrawMissionTitle
	*/
	if (m_BigMessage[1][0]) {
		if (BigMessageInUse[1] == 0.0f) {
			CFont::SetJustifyOff();
			CFont::SetBackgroundOff();

			if (CGame::frenchGame || CMenuManager::m_PrefsLanguage == 4)
				CFont::SetScale(SCREEN_SCALE_X(0.884f), SCREEN_SCALE_Y(1.36f));
			else
				CFont::SetScale(SCREEN_SCALE_X(1.04f), SCREEN_SCALE_Y(1.6f));

			CFont::SetPropOn();
			CFont::SetRightJustifyWrap(-500);
			CFont::SetRightJustifyOn();
			CFont::SetFontStyle(FONT_HEADING);
			if (BigMessageX[1] >= (SCREENW - 20)) {
				BigMessageAlpha[1] += (CTimer::GetTimeStep() * 0.02f * -255.0f);

				if (BigMessageAlpha[1] <= 0.0f) {
					BigMessageAlpha[1] = 0.0f;
					BigMessageInUse[1] = 1.0f;
				}
			}
			else {
				BigMessageX[1] += (CTimer::GetTimeStep() * 0.02f * 255.0f);
				BigMessageAlpha[1] += (CTimer::GetTimeStep() * 0.02f * 255.0f);

				if (BigMessageAlpha[1] >= 255.0f)
					BigMessageAlpha[1] = 255.0f;
			}
			CFont::SetColor(CRGBA(40, 40, 40, BigMessageAlpha[1]));
			CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(20.0f - 2.0f), SCREEN_SCALE_FROM_BOTTOM(120.0f), m_BigMessage[1]);

			CFont::SetColor(CRGBA(220, 172, 2, BigMessageAlpha[1]));
			CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(20.0f), SCREEN_SCALE_FROM_BOTTOM(120.0f), m_BigMessage[1]);
		}
		else {
			BigMessageAlpha[1] = 0.0f;
			BigMessageX[1] = -60.0f;
			BigMessageInUse[1] = 1.0f;
		}
	}
	else {
		BigMessageInUse[1] = 0.0f;
	}
}

void CHud::ReInitialise() {
	m_Wants_To_Draw_Hud = true;
	m_ZoneState = 0;
	m_Wants_To_Draw_3dMarkers = true;
	m_LastZoneName = 0;
	m_ZoneNameTimer = 0;
	m_ZoneName = 0;

	for (int i = 0; i < 256; i++) {
		m_HelpMessage[i] = 0;
		m_LastHelpMessage[i] = 0;
		m_HelpMessageToPrint[i] = 0;
		m_Message[i] = 0;

		if (i <= 6)
			BigMessageInUse[i] = 0.0f;

		if (i <= 128)
			*(wchar*)(m_BigMessage[i]) = 0;
	}

	m_HelpMessageTimer = 0;
	m_HelpMessageFadeTimer = 0;
	m_HelpMessageState = 0;
	m_HelpMessageQuick = 0;
	m_VehicleName = 0;
	m_LastVehicleName = 0;
	m_VehicleNameToPrint = 0;
	m_VehicleNameTimer = 0;
	m_VehicleFadeTimer = 0;
	m_VehicleState = 0;
	m_HelpMessageDisplayTime = 1.0f;

	CounterOnLastFrame = 0;
	m_ItemToFlash = ITEM_NONE;
	OddJob2Timer = 0;
	OddJob2OffTimer = 0.0f;
	CounterFlashTimer = 0;
	TimerOnLastFrame = 0;
	TimerFlashTimer = 0;
	SpriteBrightness = 0;
	PagerOn = 0;
	PagerTimer = 0;
	PagerSoundPlayed = 0;
	OddJob2On = 0;
	PagerXOffset = 150.0f;
	OddJob2XOffset = 0.0f;
}

STARTPATCHES
	InjectHook(0x48BC9A, &CHud::Initialise, PATCH_CALL);
	InjectHook(0x48C4F1, &CHud::ReInitialise, PATCH_CALL);
	InjectHook(0x48BCBC, &CHud::Shutdown, PATCH_CALL);
ENDPATCHES
