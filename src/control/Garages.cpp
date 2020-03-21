#include "common.h"
#include "patcher.h"
#include "main.h"
#include "ModelIndices.h"
#include "Garages.h"
#include "Timer.h"
#include "Font.h"
#include "Messages.h"
#include "PlayerPed.h"
#include "Text.h"
#include "World.h"

int32 &CGarages::BankVansCollected = *(int32 *)0x8F1B34;
bool &CGarages::BombsAreFree = *(bool *)0x95CD7A;
bool &CGarages::RespraysAreFree = *(bool *)0x95CD1D;
int32 &CGarages::CarsCollected = *(int32 *)0x880E18;
int32 (&CGarages::CarTypesCollected)[TOTAL_COLLECTCARS_GARAGES] = *(int32 (*)[TOTAL_COLLECTCARS_GARAGES])*(uintptr*)0x8E286C;
int32 &CGarages::CrushedCarId = *(int32 *)0x943060;
uint32 &CGarages::LastTimeHelpMessage = *(uint32 *)0x8F1B58;
int32 &CGarages::MessageNumberInString = *(int32 *)0x885BA8;
const char *CGarages::MessageIDString = (const char *)0x878358;
int32 &CGarages::MessageNumberInString2 = *(int32 *)0x8E2C14;
uint32 &CGarages::MessageStartTime = *(uint32 *)0x8F2530;
uint32 &CGarages::MessageEndTime = *(uint32 *)0x8F597C;
uint32 &CGarages::NumGarages = *(uint32 *)0x8F29F4;
bool &CGarages::PlayerInGarage = *(bool *)0x95CD83;
int32 &CGarages::PoliceCarsCollected = *(int32 *)0x941444;
uint32 &CGarages::GarageToBeTidied = *(uint32 *)0x623570;

CGarage(&CGarages::Garages)[NUM_GARAGES] = *(CGarage(*)[NUM_GARAGES])*(uintptr*)0x72BCD0;

WRAPPER void CGarages::Init(void) { EAXJMP(0x421C60); }
WRAPPER void CGarages::Update(void) { EAXJMP(0x421E40); }
WRAPPER void CGarages::Load(uint8* buf, uint32 size) { EAXJMP(0x428940); }
WRAPPER void CGarages::Save(uint8* buf, uint32 *size) { EAXJMP(0x4284e0); }

bool
CGarages::IsModelIndexADoor(uint32 id)
{
	return id == MI_GARAGEDOOR1 ||
		id == MI_GARAGEDOOR2 ||
		id == MI_GARAGEDOOR3 ||
		id == MI_GARAGEDOOR4 ||
		id == MI_GARAGEDOOR5 ||
		id == MI_GARAGEDOOR6 ||
		id == MI_GARAGEDOOR7 ||
		id == MI_GARAGEDOOR9 ||
		id == MI_GARAGEDOOR10 ||
		id == MI_GARAGEDOOR11 ||
		id == MI_GARAGEDOOR12 ||
		id == MI_GARAGEDOOR13 ||
		id == MI_GARAGEDOOR14 ||
		id == MI_GARAGEDOOR15 ||
		id == MI_GARAGEDOOR16 ||
		id == MI_GARAGEDOOR17 ||
		id == MI_GARAGEDOOR18 ||
		id == MI_GARAGEDOOR19 ||
		id == MI_GARAGEDOOR20 ||
		id == MI_GARAGEDOOR21 ||
		id == MI_GARAGEDOOR22 ||
		id == MI_GARAGEDOOR23 ||
		id == MI_GARAGEDOOR24 ||
		id == MI_GARAGEDOOR25 ||
		id == MI_GARAGEDOOR26 ||
		id == MI_GARAGEDOOR27 ||
		id == MI_GARAGEDOOR28 ||
		id == MI_GARAGEDOOR29 ||
		id == MI_GARAGEDOOR30 ||
		id == MI_GARAGEDOOR31 ||
		id == MI_GARAGEDOOR32 ||
		id == MI_CRUSHERBODY ||
		id == MI_CRUSHERLID;
}

bool CGarages::HasCarBeenCrushed(int32 handle)
{
	return CrushedCarId == handle;
}

WRAPPER void CGarages::TriggerMessage(const char *text, int16, uint16 time, int16) { EAXJMP(0x426B20); }
WRAPPER bool CGarages::IsPointWithinHideOutGarage(CVector&) { EAXJMP(0x428260); }
WRAPPER bool CGarages::IsPointWithinAnyGarage(CVector&) { EAXJMP(0x428320); }
WRAPPER void CGarages::PlayerArrestedOrDied() { EAXJMP(0x427F60); }
WRAPPER int16 CGarages::AddOne(float, float, float, float, float, float, uint8, uint32) { EAXJMP(0x421FA0); }
WRAPPER void CGarages::SetTargetCarForMissonGarage(int16, CVehicle*) { EAXJMP(0x426BD0); }
WRAPPER bool CGarages::HasCarBeenDroppedOffYet(int16) { EAXJMP(0x426C20); }
WRAPPER void CGarages::DeActivateGarage(int16) { EAXJMP(0x426C40); }
WRAPPER void CGarages::ActivateGarage(int16) { EAXJMP(0x426C60); }

int32 CGarages::QueryCarsCollected(int16 garage)
{
	return 0;
}

void CGarages::GivePlayerDetonator()
{
	FindPlayerPed()->GiveWeapon(WEAPONTYPE_DETONATOR, 1);
	FindPlayerPed()->GetWeapon(FindPlayerPed()->GetWeaponSlot(WEAPONTYPE_DETONATOR)).m_eWeaponState = WEAPONSTATE_READY;
}

WRAPPER bool CGarages::HasThisCarBeenCollected(int16 garage, uint8 id) { EAXJMP(0x426D50); }
WRAPPER void CGarages::ChangeGarageType(int16 garage, eGarageType type, int32 mi) { EAXJMP(0x4222A0); }
WRAPPER bool CGarages::HasResprayHappened(int16 garage) { EAXJMP(0x4274F0); }
WRAPPER bool CGarages::IsThisCarWithinGarageArea(int16 garage, CEntity* pCar) { EAXJMP(0x427570); }

void CGarage::OpenThisGarage()
{
	if (m_eGarageState == GS_FULLYCLOSED || m_eGarageState == GS_CLOSING || m_eGarageState == GS_CLOSEDCONTAINSCAR)
		m_eGarageState = GS_OPENING;
}

bool CGarages::IsGarageOpen(int16 garage)
{
	return Garages[garage].IsOpen();
}

bool CGarages::IsGarageClosed(int16 garage)
{
	return Garages[garage].IsClosed();
}

void CGarage::CloseThisGarage()
{
	if (m_eGarageState == GS_OPENED || m_eGarageState == GS_OPENING)
		m_eGarageState = GS_CLOSING;
}

void CGarages::SetGarageDoorToRotate(int16 garage)
{
	if (Garages[garage].m_bRotatedDoor)
		return;
	Garages[garage].m_bRotatedDoor = true;
	Garages[garage].m_fDoorHeight /= 2.0f;
	Garages[garage].m_fDoorHeight -= 0.1f;
}

bool CGarages::HasImportExportGarageCollectedThisCar(int16 garage, int8 car)
{
	return CarTypesCollected[GetCarsCollectedIndexForGarageType(Garages[garage].m_eGarageType)] & (1 << car);
}

void CGarages::SetLeaveCameraForThisGarage(int16 garage)
{
	Garages[garage].m_bCameraFollowsPlayer = true;
}

#if 0
WRAPPER void CGarages::PrintMessages(void) { EAXJMP(0x426310); }
#else
void CGarages::PrintMessages()
{
	if (CTimer::GetTimeInMilliseconds() > MessageStartTime && CTimer::GetTimeInMilliseconds() < MessageEndTime) {
		CFont::SetScale(SCREEN_SCALE_X(1.2f), SCREEN_SCALE_Y(1.5f)); // BUG: game doesn't use macro here.
		CFont::SetPropOn();
		CFont::SetJustifyOff();
		CFont::SetBackgroundOff();
		CFont::SetCentreSize(SCREEN_SCALE_FROM_RIGHT(50.0f));
		CFont::SetCentreOn();
		CFont::SetFontStyle(FONT_BANK);
		CFont::SetColor(CRGBA(0, 0, 0, 255));

		float y_offset = SCREEN_HEIGHT / 3; // THIS is PS2 calculation
		// y_offset = SCREEN_HEIGHT / 2 - SCREEN_SCALE_Y(84.0f); // This is PC and results in text being written over some HUD elements

		if (MessageNumberInString2 < 0) {
			if (MessageNumberInString < 0) {
				CFont::PrintString(SCREEN_WIDTH / 2 - SCREEN_SCALE_X(2.0f), y_offset - SCREEN_SCALE_Y(2.0f), TheText.Get(MessageIDString));

				CFont::SetColor(CRGBA(89, 115, 150, 255));
				CFont::PrintString(SCREEN_WIDTH / 2, y_offset, TheText.Get(MessageIDString));
			}
			else {
				CMessages::InsertNumberInString(TheText.Get(MessageIDString), MessageNumberInString, -1, -1, -1, -1, -1, gUString);

				CFont::PrintString(SCREEN_WIDTH / 2 + SCREEN_SCALE_X(2.0f), y_offset - SCREEN_SCALE_Y(40.0f) + SCREEN_SCALE_Y(2.0f), gUString);

				CFont::SetColor(CRGBA(89, 115, 150, 255));
				CFont::PrintString(SCREEN_WIDTH / 2, y_offset - SCREEN_SCALE_Y(40.0f), gUString);
			}
		}
		else {
			CMessages::InsertNumberInString(TheText.Get(MessageIDString), MessageNumberInString, MessageNumberInString2, -1, -1, -1, -1, gUString);
			CFont::PrintString(SCREEN_WIDTH / 2 + SCREEN_SCALE_X(2.0f), y_offset - SCREEN_SCALE_Y(40.0f) + SCREEN_SCALE_Y(2.0f), gUString);

			CFont::SetColor(CRGBA(89, 115, 150, 255));
			CFont::PrintString(SCREEN_WIDTH / 2, y_offset - SCREEN_SCALE_Y(40.0f), gUString);
		}
	}
}
#endif