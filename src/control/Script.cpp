#include "common.h"
#include "patcher.h"

#include "Script.h"
#include "ScriptCommands.h"

#include "Boat.h"
#include "Camera.h"
#include "CarCtrl.h"
#include "CarGen.h"
#include "CivilianPed.h"
#include "Clock.h"
#include "CopPed.h"
#include "DMAudio.h"
#include "EmergencyPed.h"
#include "FileMgr.h"
#include "Garages.h"
#include "General.h"
#include "HandlingMgr.h"
#include "Hud.h"
#include "Messages.h"
#include "ModelIndices.h"
#include "Pad.h"
#include "Pickups.h"
#include "PlayerInfo.h"
#include "PlayerPed.h"
#include "Pools.h"
#include "Population.h"
#include "Remote.h"
#include "Replay.h"
#include "Streaming.h"
#include "Text.h"
#include "User.h"
#include "Weather.h"
#include "World.h"
#include "Zones.h"

uint8 (&CTheScripts::ScriptSpace)[SIZE_SCRIPT_SPACE] = *(uint8(*)[SIZE_SCRIPT_SPACE])*(uintptr*)0x74B248;
CRunningScript(&CTheScripts::ScriptsArray)[MAX_NUM_SCRIPTS] = *(CRunningScript(*)[MAX_NUM_SCRIPTS])*(uintptr*)0x6F5C08;
int32(&CTheScripts::BaseBriefIdForContact)[MAX_NUM_CONTACTS] = *(int32(*)[MAX_NUM_CONTACTS])*(uintptr*)0x880200;
int32(&CTheScripts::OnAMissionForContactFlag)[MAX_NUM_CONTACTS] = *(int32(*)[MAX_NUM_CONTACTS])*(uintptr*)0x8622F0;
CTextLine (&CTheScripts::IntroTextLines)[MAX_NUM_INTRO_TEXT_LINES] = *(CTextLine (*)[MAX_NUM_INTRO_TEXT_LINES])*(uintptr*)0x70EA68;
CScriptRectangle (&CTheScripts::IntroRectangles)[MAX_NUM_INTRO_RECTANGLES] = *(CScriptRectangle (*)[MAX_NUM_INTRO_RECTANGLES])*(uintptr*)0x72D108;
CSprite2d (&CTheScripts::ScriptSprites)[MAX_NUM_SCRIPT_SRPITES] = *(CSprite2d(*)[MAX_NUM_SCRIPT_SRPITES])*(uintptr*)0x72B090;
CScriptSphere(&CTheScripts::ScriptSphereArray)[MAX_NUM_SCRIPT_SPHERES] = *(CScriptSphere(*)[MAX_NUM_SCRIPT_SPHERES])*(uintptr*)0x727D60;
tCollectiveData(&CTheScripts::CollectiveArray)[MAX_NUM_COLLECTIVES] = *(tCollectiveData(*)[MAX_NUM_COLLECTIVES])*(uintptr*)0x6FA008;
tUsedObject(&CTheScripts::UsedObjectArray)[MAX_NUM_USED_OBJECTS] = *(tUsedObject(*)[MAX_NUM_USED_OBJECTS])*(uintptr*)0x6E69C8;
int32(&CTheScripts::MultiScriptArray)[MAX_NUM_MISSION_SCRIPTS] = *(int32(*)[MAX_NUM_MISSION_SCRIPTS])*(uintptr*)0x6F0558;
tBuildingSwap(&CTheScripts::BuildingSwapArray)[MAX_NUM_BUILDING_SWAPS] = *(tBuildingSwap(*)[MAX_NUM_BUILDING_SWAPS])*(uintptr*)0x880E30;
CEntity*(&CTheScripts::InvisibilitySettingArray)[MAX_NUM_INVISIBILITY_SETTINGS] = *(CEntity*(*)[MAX_NUM_INVISIBILITY_SETTINGS])*(uintptr*)0x8620F0;
bool &CTheScripts::DbgFlag = *(bool*)0x95CD87;
uint32 &CTheScripts::OnAMissionFlag = *(uint32*)0x8F1B64;
int32 &CTheScripts::StoreVehicleIndex = *(int32*)0x8F5F3C;
bool &CTheScripts::StoreVehicleWasRandom = *(bool*)0x95CDBC;
CRunningScript *&CTheScripts::pIdleScripts = *(CRunningScript**)0x9430D4;
CRunningScript *&CTheScripts::pActiveScripts = *(CRunningScript**)0x8E2BF4;
uint32 &CTheScripts::NextFreeCollectiveIndex = *(uint32*)0x942F98;
int32 &CTheScripts::LastRandomPedId = *(int32*)0x8F251C;
uint16 &CTheScripts::NumberOfUsedObjects = *(uint16*)0x95CC72;
bool &CTheScripts::bAlreadyRunningAMissionScript = *(bool*)0x95CDB3;
bool &CTheScripts::bUsingAMultiScriptFile = *(bool*)0x95CD55;
uint16 &CTheScripts::NumberOfMissionScripts = *(uint16*)0x95CC9A;
uint32 &CTheScripts::LargestMissionScriptSize = *(uint32*)0x9414C8;
uint32 &CTheScripts::MainScriptSize = *(uint32*)0x9405A4;
uint8 &CTheScripts::FailCurrentMission = *(uint8*)0x95CD41;
uint8 &CTheScripts::CountdownToMakePlayerUnsafe = *(uint8*)0x95CD51;
uint8 &CTheScripts::DelayMakingPlayerUnsafeThisTime = *(uint8*)0x95CD88;
uint16 &CTheScripts::NumScriptDebugLines = *(uint16*)0x95CC42;
uint16 &CTheScripts::NumberOfIntroRectanglesThisFrame = *(uint16*)0x95CC88;
uint16 &CTheScripts::NumberOfIntroTextLinesThisFrame = *(uint16*)0x95CC32;
bool &CTheScripts::UseTextCommands = *(bool*)0x95CD57;
CMissionCleanup (&CTheScripts::MissionCleanup) = *(CMissionCleanup*)0x8F2A24;
CUpsideDownCarCheck (&CTheScripts::UpsideDownCars) = *(CUpsideDownCarCheck*)0x6EE450;
CStuckCarCheck (&CTheScripts::StuckCars) = *(CStuckCarCheck*)0x87C588;
uint16 &CTheScripts::CommandsExecuted = *(uint16*)0x95CCA6;
uint16 &CTheScripts::ScriptsUpdated = *(uint16*)0x95CC5E;
int32(&ScriptParams)[32] = *(int32(*)[32])*(uintptr*)0x6ED460;

CMissionCleanup::CMissionCleanup()
{
	Init();
}

void CMissionCleanup::Init()
{
	m_nCount = 0;
	for (int i = 0; i < MAX_CLEANUP; i++){
		m_sEntities[i].type = CLEANUP_UNUSED;
		m_sEntities[i].id = 0;
	}
}

CMissionCleanupEntity* CMissionCleanup::FindFree()
{
	for (int i = 0; i < MAX_CLEANUP; i++){
		if (m_sEntities[i].type == CLEANUP_UNUSED)
			return &m_sEntities[i];
	}
	assert(0);
	return nil;
}

void CMissionCleanup::AddEntityToList(int32 id, uint8 type)
{
	CMissionCleanupEntity* pNew = FindFree();
	if (!pNew)
		return;
	pNew->id = id;
	pNew->type = type;
	m_nCount++;
}

void CMissionCleanup::RemoveEntityFromList(int32 id, uint8 type)
{
	for (int i = 0; i < MAX_CLEANUP; i++){
		if (m_sEntities[i].type == type && m_sEntities[i].id == id){
			m_sEntities[i].id = 0;
			m_sEntities[i].type = CLEANUP_UNUSED;
			m_nCount--;
		}
	}
}

void CMissionCleanup::Process()
{
	CPopulation::m_AllRandomPedsThisType = -1;
	CPopulation::PedDensityMultiplier = 1.0f;
	CCarCtrl::CarDensityMultiplier = 1.0;
	FindPlayerPed()->m_pWanted->m_fCrimeSensitivity = 1.0f;
	TheCamera.Restore();
	TheCamera.SetWidescreenOff();
	DMAudio.ClearMissionAudio();
	CWeather::ReleaseWeather();
	for (int i = 0; i < NUM_OF_SPECIAL_CHARS; i++)
		CStreaming::SetMissionDoesntRequireSpecialChar(i);
	for (int i = 0; i < NUM_OF_CUTSCENE_OBJECTS; i++)
		CStreaming::SetMissionDoesntRequireModel(MI_CUTOBJ01 + i);
	CStreaming::ms_disableStreaming = false;
	CHud::m_ItemToFlash = -1;
	CHud::SetHelpMessage(nil, false);
	CUserDisplay::OnscnTimer.m_bDisabled = false;
	CWorld::Players[0].m_pPed->m_pWanted->m_bIgnoredByCops = false;
	CWorld::Players[0].m_pPed->m_pWanted->m_bIgnoredByEveryone = false;
	CWorld::Players[0].MakePlayerSafe(false);
	CTheScripts::StoreVehicleIndex = -1;
	CTheScripts::StoreVehicleWasRandom = true;
	CTheScripts::UpsideDownCars.Init();
	CTheScripts::StuckCars.Init();
	for (int i = 0; i < MAX_CLEANUP; i++){
		if (m_sEntities[i].type == CLEANUP_UNUSED)
			continue;
		switch (m_sEntities[i].type) {
		case CLEANUP_CAR:
		{
			CVehicle* v = CPools::GetVehiclePool()->GetAt(m_sEntities[i].id);
			if (v)
				CTheScripts::CleanUpThisVehicle(v);
			break;
		}
		case CLEANUP_CHAR:
		{
			CPed* p = CPools::GetPedPool()->GetAt(m_sEntities[i].id);
			if (p)
				CTheScripts::CleanUpThisPed(p);
			break;
		}
		case CLEANUP_OBJECT:
		{
			CObject* o = CPools::GetObjectPool()->GetAt(m_sEntities[i].id);
			if (o)
				CTheScripts::CleanUpThisObject(o);
			break;
		}
		default:
			break;
		}
		m_sEntities[i].id = 0;
		m_sEntities[i].type = CLEANUP_UNUSED;
		m_nCount--;
	}
}

/* NB: CUpsideDownCarCheck is not used by actual script at all
 * It has a weird usage: AreAnyCarsUpsideDown would fail any mission
 * just like death or arrest. */

void CUpsideDownCarCheck::Init()
{
	for (int i = 0; i < MAX_UPSIDEDOWN_CAR_CHECKS; i++){
		m_sCars[i].m_nVehicleIndex = -1;
		m_sCars[i].m_nVehicleIndex = 0;
	}
}

bool CUpsideDownCarCheck::IsCarUpsideDown(int32 id)
{
	CVehicle* v = CPools::GetVehiclePool()->GetAt(id);
	return v->GetUp().z <= -0.97f &&
		v->GetMoveSpeed().Magnitude() < 0.01f &&
		v->GetTurnSpeed().Magnitude() < 0.02f;
}

void CUpsideDownCarCheck::UpdateTimers()
{
	uint32 timeStep = CTimer::GetTimeStepInMilliseconds();
	for (int i = 0; i < MAX_UPSIDEDOWN_CAR_CHECKS; i++){
		CVehicle* v = CPools::GetVehiclePool()->GetAt(m_sCars[i].m_nVehicleIndex);
		if (v){
			if (IsCarUpsideDown(m_sCars[i].m_nVehicleIndex))
				m_sCars[i].m_nUpsideDownTimer += timeStep;
			else
				m_sCars[i].m_nUpsideDownTimer = 0;
		}else{
			m_sCars[i].m_nVehicleIndex = -1;
			m_sCars[i].m_nUpsideDownTimer = 0;
		}
	}
}

bool CUpsideDownCarCheck::AreAnyCarsUpsideDown()
{
	for (int i = 0; i < MAX_UPSIDEDOWN_CAR_CHECKS; i++){
		if (m_sCars[i].m_nVehicleIndex >= 0 && m_sCars[i].m_nUpsideDownTimer > 1000)
			return true;
	}
	return false;
}

void CUpsideDownCarCheck::AddCarToCheck(int32 id)
{
	uint16 index = 0;
	while (index < MAX_UPSIDEDOWN_CAR_CHECKS && m_sCars[index].m_nVehicleIndex >= 0)
		index++;
	if (index >= MAX_UPSIDEDOWN_CAR_CHECKS)
		return;
	m_sCars[index].m_nVehicleIndex = id;
	m_sCars[index].m_nUpsideDownTimer = 0;
}

void CUpsideDownCarCheck::RemoveCarFromCheck(int32 id)
{
	for (int i = 0; i < MAX_UPSIDEDOWN_CAR_CHECKS; i++){
		if (m_sCars[i].m_nVehicleIndex == id){
			m_sCars[i].m_nVehicleIndex = -1;
			m_sCars[i].m_nUpsideDownTimer = 0;
		}
	}
}

bool CUpsideDownCarCheck::HasCarBeenUpsideDownForAWhile(int32 id)
{
	for (int i = 0; i < MAX_UPSIDEDOWN_CAR_CHECKS; i++){
		if (m_sCars[i].m_nVehicleIndex == id)
			return m_sCars[i].m_nUpsideDownTimer > 1000;
	}
	return false;
}

void CStuckCarCheckEntry::Reset()
{
	m_nVehicleIndex = -1;
	m_vecPos = CVector(-5000.0f, -5000.0f, -5000.0f);
	m_nLastCheck = -1;
	m_fRadius = 0.0f;
	m_nStuckTime = 0;
	m_bStuck = false;
}

void CStuckCarCheck::Init()
{
	for (int i = 0; i < MAX_STUCK_CAR_CHECKS; i++) {
		m_sCars[i].Reset();
	}
}

void CStuckCarCheck::Process()
{
	uint32 timer = CTimer::GetTimeInMilliseconds();
	for (int i = 0; i < MAX_STUCK_CAR_CHECKS; i++){
		if (m_sCars[i].m_nVehicleIndex < 0)
			continue;
		if (timer <= m_sCars[i].m_nStuckTime + m_sCars[i].m_nLastCheck)
			continue;
		CVehicle* pv = CPools::GetVehiclePool()->GetAt(m_sCars[i].m_nVehicleIndex);
		if (!pv){
			m_sCars[i].Reset();
			continue;
		}
		float distance = (pv->GetPosition() - m_sCars[i].m_vecPos).Magnitude();
		m_sCars[i].m_bStuck = distance < m_sCars[i].m_fRadius;
		m_sCars[i].m_vecPos = pv->GetPosition();
		m_sCars[i].m_nLastCheck = timer;
	}
}

void CStuckCarCheck::AddCarToCheck(int32 id, float radius, uint32 time)
{
	CVehicle* pv = CPools::GetVehiclePool()->GetAt(id);
	if (!pv)
		return;
	int index = 0;
	while (index < MAX_STUCK_CAR_CHECKS && m_sCars[index].m_nVehicleIndex >= 0)
		index++;
	/* Would be nice to return if index >= MAX_STUCK_CAR_CHECKS... */
	m_sCars[index].m_nVehicleIndex = id;
	m_sCars[index].m_vecPos = pv->GetPosition();
	m_sCars[index].m_nLastCheck = CTimer::GetTimeInMilliseconds();
	m_sCars[index].m_fRadius = radius;
	m_sCars[index].m_nStuckTime = time;
	m_sCars[index].m_bStuck = false;
}

void CStuckCarCheck::RemoveCarFromCheck(int32 id)
{
	for (int i = 0; i < MAX_STUCK_CAR_CHECKS; i++){
		if (m_sCars[i].m_nVehicleIndex == id){
			m_sCars[i].Reset();
		}
	}
}

bool CStuckCarCheck::HasCarBeenStuckForAWhile(int32 id)
{
	for (int i = 0; i < MAX_STUCK_CAR_CHECKS; i++){
		if (m_sCars[i].m_nVehicleIndex == id)
			return m_sCars[i].m_bStuck;
	}
	return false;
}

void CRunningScript::CollectParameters(uint32* pIp, int16 total)
{
	for (int16 i = 0; i < total; i++){
		float tmp;
		switch (CTheScripts::Read1ByteFromScript(pIp))
		{
		case ARGUMENT_INT32:
			ScriptParams[i] = CTheScripts::Read4BytesFromScript(pIp);
			break;
		case ARGUMENT_GLOBALVAR:
			ScriptParams[i] = *((int32*)&CTheScripts::ScriptSpace[CTheScripts::Read2BytesFromScript(pIp)]);
			break;
		case ARGUMENT_LOCALVAR:
			ScriptParams[i] = m_anLocalVariables[CTheScripts::Read2BytesFromScript(pIp)];
			break;
		case ARGUMENT_INT8:
			ScriptParams[i] = CTheScripts::Read1ByteFromScript(pIp);
			break;
		case ARGUMENT_INT16:
			ScriptParams[i] = CTheScripts::Read2BytesFromScript(pIp);
			break;
		case ARGUMENT_FLOAT:
			tmp = CTheScripts::ReadFloatFromScript(pIp);
			ScriptParams[i] = *(int32*)&tmp;
			break;
		default:
			assert(0);
			break;
		}
	}
}

int32 CRunningScript::CollectNextParameterWithoutIncreasingPC(uint32 ip)
{
	uint32* pIp = &ip;
	float tmp;
	switch (CTheScripts::Read1ByteFromScript(pIp))
	{
	case ARGUMENT_INT32:
		return CTheScripts::Read4BytesFromScript(pIp);
	case ARGUMENT_GLOBALVAR:
		return *((int32*)&CTheScripts::ScriptSpace[CTheScripts::Read2BytesFromScript(pIp)]);
	case ARGUMENT_LOCALVAR:
		return m_anLocalVariables[CTheScripts::Read2BytesFromScript(pIp)];
	case ARGUMENT_INT8:
		return CTheScripts::Read1ByteFromScript(pIp);
	case ARGUMENT_INT16:
		return CTheScripts::Read2BytesFromScript(pIp);
	case ARGUMENT_FLOAT:
		tmp = CTheScripts::ReadFloatFromScript(pIp);
		return *(int32*)&tmp;
	default:
		assert(0);
	}
	return -1;
}

void CRunningScript::StoreParameters(uint32* pIp, int16 number)
{
	for (int16 i = 0; i < number; i++){
		switch (CTheScripts::Read1ByteFromScript(pIp)) {
		case ARGUMENT_GLOBALVAR:
			*(int32*)&CTheScripts::ScriptSpace[CTheScripts::Read2BytesFromScript(pIp)] = ScriptParams[i];
			break;
		case ARGUMENT_LOCALVAR:
			m_anLocalVariables[CTheScripts::Read2BytesFromScript(pIp)] = ScriptParams[i];
			break;
		default:
			assert(0);
		}
	}
}

int32 *CRunningScript::GetPointerToScriptVariable(uint32* pIp, int16 type)
{
	switch (CTheScripts::Read1ByteFromScript(pIp))
	{
	case ARGUMENT_GLOBALVAR:
		assert(type == VAR_GLOBAL);
		return (int32*)&CTheScripts::ScriptSpace[CTheScripts::Read2BytesFromScript(pIp)];
	case ARGUMENT_LOCALVAR:
		assert(type == VAR_LOCAL);
		return &m_anLocalVariables[CTheScripts::Read2BytesFromScript(pIp)];
	default:
		assert(0);
	}
	return nil;
}

void CRunningScript::Init()
{
	strcpy(m_abScriptName, "noname");
	next = prev = nil;
	m_nIp = 0;
	for (int i = 0; i < MAX_STACK_DEPTH; i++)
		m_anStack[i] = 0;
	m_nStackPointer = 0;
	m_nWakeTime = 0;
	m_bCondResult = false;
	m_bIsMissionScript = false;
	m_bSkipWakeTime = false;
	for (int i = 0; i < NUM_LOCAL_VARS + NUM_TIMERS; i++)
		m_anLocalVariables[i] = 0;
	m_nAndOrState = 0;
	m_bNotFlag = false;
	m_bDeatharrestEnabled = true;
	m_bDeatharrestExecuted = false;
	m_bMissionFlag = false;
}

#ifdef USE_DEBUG_SCRIPT_LOADER
int open_script()
{
	static int scriptToLoad = 1;

	if (GetAsyncKeyState('G') & 0x8000)
		scriptToLoad = 0;
	if (GetAsyncKeyState('R') & 0x8000)
		scriptToLoad = 1;
	if (GetAsyncKeyState('D') & 0x8000)
		scriptToLoad = 2;

	switch (scriptToLoad) {
	case 0: return CFileMgr::OpenFile("main.scm", "rb");
	case 1: return CFileMgr::OpenFile("main_freeroam.scm", "rb");
	case 2: return CFileMgr::OpenFile("main_d.scm", "rb");
	}
	return CFileMgr::OpenFile("main.scm", "rb");
}
#endif

void CTextLine::Reset()
{
	m_fScaleX = 0.48f;
	m_fScaleY = 1.12f;
	m_sColor = CRGBA(225, 225, 225, 255);
	m_bJustify = false;
	m_bRightJustify = false;
	m_bCentered = false;
	m_bBackground = false;
	m_bBackgroundOnly = false;
	m_fWrapX = 182.0f; /* TODO: scaling as bugfix */
	m_fCenterSize = 640.0f; /* --||-- */
	m_sBackgroundColor = CRGBA(128, 128, 128, 128);
	m_bTextProportional = true;
	m_bTextBeforeFade = false;
	m_nFont = 2; /* enum? */
	m_fAtX = 0.0f;
	m_fAtY = 0.0f;
	memset(&m_Text, 0, sizeof(m_Text));
}

void CTheScripts::Init()
{
	for (int i = 0; i < SIZE_SCRIPT_SPACE; i++)
		ScriptSpace[i] = 0;
	pActiveScripts = pIdleScripts = nil;
	for (int i = 0; i < MAX_NUM_SCRIPTS; i++){
		ScriptsArray[i].Init();
		ScriptsArray[i].AddScriptToList(&pIdleScripts);
	}
	MissionCleanup.Init();
	UpsideDownCars.Init();
	StuckCars.Init();
	CFileMgr::SetDir("data");
#ifdef USE_DEBUG_SCRIPT_LOADER
	int mainf = open_script();
#else
	int mainf = CFileMgr::OpenFile("main.scm", "rb");
#endif
	CFileMgr::Read(mainf, (char*)ScriptSpace, SIZE_MAIN_SCRIPT);
	CFileMgr::CloseFile(mainf);
	CFileMgr::SetDir("");
	StoreVehicleIndex = -1;
	StoreVehicleWasRandom = true;
	OnAMissionFlag = 0;
	for (int i = 0; i < MAX_NUM_CONTACTS; i++){
		BaseBriefIdForContact[i] = 0;
		OnAMissionForContactFlag[i] = 0;
	}
	for (int i = 0; i < MAX_NUM_COLLECTIVES; i++){
		CollectiveArray[i].index = -1;
		CollectiveArray[i].unk_data = 0;
	}
	NextFreeCollectiveIndex = 0;
	LastRandomPedId = -1;
	for (int i = 0; i < MAX_NUM_USED_OBJECTS; i++){
		memset(&UsedObjectArray[i].name, 0, sizeof(UsedObjectArray[i].name));
		UsedObjectArray[i].index = 0;
	}
	NumberOfUsedObjects = 0;
	ReadObjectNamesFromScript();
	UpdateObjectIndices();
	bAlreadyRunningAMissionScript = false;
	bUsingAMultiScriptFile = true;
	for (int i = 0; i < MAX_NUM_MISSION_SCRIPTS; i++)
		MultiScriptArray[i] = 0;
	NumberOfMissionScripts = 0;
	LargestMissionScriptSize = 0;
	MainScriptSize = 0;
	ReadMultiScriptFileOffsetsFromScript();
	FailCurrentMission = 0;
	CountdownToMakePlayerUnsafe = 0;
	DbgFlag = 0;
	DelayMakingPlayerUnsafeThisTime = 0;
	NumScriptDebugLines = 0;
	for (int i = 0; i < MAX_NUM_SCRIPT_SPHERES; i++){
		ScriptSphereArray[i].m_bInUse = false;
		ScriptSphereArray[i].m_Index = 1;
		ScriptSphereArray[i].m_Id = 0;
		ScriptSphereArray[i].m_vecCenter = CVector(0.0f, 0.0f, 0.0f);
		ScriptSphereArray[i].m_fRadius = 0.0f;
	}
	for (int i = 0; i < MAX_NUM_INTRO_TEXT_LINES; i++){
		IntroTextLines[i].Reset();
	}
	NumberOfIntroTextLinesThisFrame = 0;
	UseTextCommands = false;
	for (int i = 0; i < MAX_NUM_INTRO_RECTANGLES; i++){
		IntroRectangles[i].m_bIsUsed = false;
		IntroRectangles[i].m_bBeforeFade = false;
		IntroRectangles[i].m_nTextureId = -1;
		IntroRectangles[i].m_sRect = CRect(0.0f, 0.0f, 0.0f, 0.0f);
		IntroRectangles[i].m_sColor = CRGBA(255, 255, 255, 255);
	}
	NumberOfIntroRectanglesThisFrame = 0;
	for (int i = 0; i < MAX_NUM_BUILDING_SWAPS; i++){
		BuildingSwapArray[i].m_pBuilding = nil;
		BuildingSwapArray[i].m_nNewModel = -1;
		BuildingSwapArray[i].m_nOldModel = -1;
	}
	for (int i = 0; i < MAX_NUM_INVISIBILITY_SETTINGS; i++)
		InvisibilitySettingArray[i] = nil;
}

void CRunningScript::RemoveScriptFromList(CRunningScript** ppScript)
{
	if (prev)
		prev->next = next;
	else
		*ppScript = next;
	if (next)
		next->prev = prev;
}

void CRunningScript::AddScriptToList(CRunningScript** ppScript)
{
	next = *ppScript;
	prev = nil;
	if (*ppScript)
		(*ppScript)->prev = this;
	*ppScript = this;
}

CRunningScript* CTheScripts::StartNewScript(uint32 ip)
{
	CRunningScript* pNew = pIdleScripts;
	assert(pNew);
	pNew->RemoveScriptFromList(&pIdleScripts);
	pNew->Init();
	pNew->SetIP(ip);
	pNew->AddScriptToList(&pActiveScripts);
	return pNew;
}

void CTheScripts::Process()
{
	if (CReplay::IsPlayingBack())
		return;
	CommandsExecuted = 0;
	ScriptsUpdated = 0;
	float timeStep = CTimer::GetTimeStepInMilliseconds();
	UpsideDownCars.UpdateTimers();
	StuckCars.Process();
	DrawScriptSpheres();
	if (FailCurrentMission)
		--FailCurrentMission;
	if (CountdownToMakePlayerUnsafe){
		if (--CountdownToMakePlayerUnsafe == 0)
			CWorld::Players[0].MakePlayerSafe(false);
	}
	if (UseTextCommands){
		for (int i = 0; i < MAX_NUM_INTRO_TEXT_LINES; i++)
			IntroTextLines[i].Reset();
		NumberOfIntroTextLinesThisFrame = 0;
		for (int i = 0; i < MAX_NUM_INTRO_RECTANGLES; i++){
			IntroRectangles[i].m_bIsUsed = false;
			IntroRectangles[i].m_bBeforeFade = false;
		}
		NumberOfIntroRectanglesThisFrame = 0;
		if (UseTextCommands == 1)
			UseTextCommands = 0;
	}
	CRunningScript* script = pActiveScripts;
	while (script != nil){
		CRunningScript* next = script->GetNext();
		++ScriptsUpdated;
		script->UpdateTimers(timeStep);
		script->Process();
		script = next;
	}
	DbgFlag = false;
}

CRunningScript* CTheScripts::StartTestScript()
{
	return StartNewScript(0);
}

bool CTheScripts::IsPlayerOnAMission()
{
	return OnAMissionFlag && *(int32*)&ScriptSpace[OnAMissionFlag] == 1;
}

void CRunningScript::Process()
{
	if (m_bIsMissionScript)
		DoDeatharrestCheck();
	if (m_bMissionFlag && CTheScripts::FailCurrentMission == 1 && m_nStackPointer == 1)
		m_nIp = m_anStack[--m_nStackPointer];
	if (CTimer::GetTimeInMilliseconds() >= m_nWakeTime){
		while (!ProcessOneCommand())
			;
		return;
	}
	if (!m_bSkipWakeTime)
		return;
	if (!CPad::GetPad(0)->GetCrossJustDown())
		return;
	m_nWakeTime = 0;
	for (int i = 0; i < 6; i++){ /* TODO: add constant for number of messages */
		if (CMessages::BIGMessages[i].m_Current.m_pText)
			CMessages::BIGMessages[i].m_Current.m_nStartTime = 0;
		if (CMessages::BriefMessages[0].m_pText)
			CMessages::BriefMessages[0].m_nStartTime = 0;
	}
}

int8 CRunningScript::ProcessOneCommand()
{
	++CTheScripts::CommandsExecuted;
	int32 command = CTheScripts::Read2BytesFromScript(&m_nIp);
	m_bNotFlag = (command & 0x8000);
	command &= 0x7FFF;
	if (command < 100)
		return ProcessCommandsFrom0To99(command);
	if (command < 200)
		return ProcessCommandsFrom100To199(command);
	if (command < 300)
		return ProcessCommandsFrom200To299(command);
	if (command < 400)
		return ProcessCommandsFrom300To399(command);
	if (command < 500)
		return ProcessCommandsFrom400To499(command);
	if (command < 600)
		return ProcessCommandsFrom500To599(command);
	if (command < 700)
		return ProcessCommandsFrom600To699(command);
	if (command < 800)
		return ProcessCommandsFrom700To799(command);
	if (command < 900)
		return ProcessCommandsFrom800To899(command);
	if (command < 1000)
		return ProcessCommandsFrom900To999(command);
	if (command < 1100)
		return ProcessCommandsFrom1000To1099(command);
	if (command < 1200)
		return ProcessCommandsFrom1100To1199(command);
	return -1;
}

int8 CRunningScript::ProcessCommandsFrom0To99(int32 command)
{
	switch (command) {
	case COMMAND_NOP:
		return 0;
	case COMMAND_WAIT:
		CollectParameters(&m_nIp, 1);
		m_nWakeTime = CTimer::GetTimeInMilliseconds() + ScriptParams[0];
		return 1;
	case COMMAND_GOTO:
		CollectParameters(&m_nIp, 1);
		SetIP(ScriptParams[0] >= 0 ? ScriptParams[0] : SIZE_MAIN_SCRIPT - ScriptParams[0]);
		/* Known issue: GOTO to 0. It might have been "better" to use > instead of >= */
		/* simply because it never makes sense to jump to start of the script */
		/* but jumping to start of a custom mission is an issue for simple mission-like scripts */
		/* However, it's not an issue for actual mission scripts, because they follow a structure */
		/* and never start with a loop. */
		return 0;
	case COMMAND_SHAKE_CAM:
		CollectParameters(&m_nIp, 1);
		CamShakeNoPos(&TheCamera, ScriptParams[0] / 1000.0f);
		return 0;
	case COMMAND_SET_VAR_INT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*ptr = ScriptParams[0];
		return 0;
	}
	case COMMAND_SET_VAR_FLOAT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr = *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_SET_LVAR_INT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*ptr = ScriptParams[0];
		return 0;
	}
	case COMMAND_SET_LVAR_FLOAT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr = *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_ADD_VAL_TO_INT_VAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*ptr += ScriptParams[0];
		return 0;
	}
	case COMMAND_ADD_VAL_TO_FLOAT_VAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr += *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_ADD_VAL_TO_INT_LVAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*ptr += ScriptParams[0];
		return 0;
	}
	case COMMAND_ADD_VAL_TO_FLOAT_LVAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr += *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_SUB_VAL_FROM_INT_VAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*ptr -= ScriptParams[0];
		return 0;
	}
	case COMMAND_SUB_VAL_FROM_FLOAT_VAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr -= *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_SUB_VAL_FROM_INT_LVAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*ptr -= ScriptParams[0];
		return 0;
	}
	case COMMAND_SUB_VAL_FROM_FLOAT_LVAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr -= *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_MULT_INT_VAR_BY_VAL:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*ptr *= ScriptParams[0];
		return 0;
	}
	case COMMAND_MULT_FLOAT_VAR_BY_VAL:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr *= *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_MULT_INT_LVAR_BY_VAL:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*ptr *= ScriptParams[0];
		return 0;
	}
	case COMMAND_MULT_FLOAT_LVAR_BY_VAL:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr *= *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_DIV_INT_VAR_BY_VAL:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*ptr /= ScriptParams[0];
		return 0;
	}
	case COMMAND_DIV_FLOAT_VAR_BY_VAL:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr /= *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_DIV_INT_LVAR_BY_VAL:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*ptr /= ScriptParams[0];
		return 0;
	}
	case COMMAND_DIV_FLOAT_LVAR_BY_VAL:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr /= *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_IS_INT_VAR_GREATER_THAN_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*ptr > ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_INT_LVAR_GREATER_THAN_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*ptr > ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_NUMBER_GREATER_THAN_INT_VAR:
	{
		CollectParameters(&m_nIp, 1);
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(ScriptParams[0] > *ptr);
		return 0;
	}
	case COMMAND_IS_NUMBER_GREATER_THAN_INT_LVAR:
	{
		CollectParameters(&m_nIp, 1);
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(ScriptParams[0] > *ptr);
		return 0;
	}
	case COMMAND_IS_INT_VAR_GREATER_THAN_INT_VAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*ptr1 > *ptr2);
		return 0;
	}
	case COMMAND_IS_INT_LVAR_GREATER_THAN_INT_VAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*ptr1 > *ptr2);
		return 0;
	}
	case COMMAND_IS_INT_VAR_GREATER_THAN_INT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*ptr1 > *ptr2);
		return 0;
	}
	case COMMAND_IS_INT_LVAR_GREATER_THAN_INT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*ptr1 > *ptr2);
		return 0;
	}
	case COMMAND_IS_FLOAT_VAR_GREATER_THAN_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*(float*)ptr > *(float*)&ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_FLOAT_LVAR_GREATER_THAN_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*(float*)ptr > *(float*)&ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_NUMBER_GREATER_THAN_FLOAT_VAR:
	{
		CollectParameters(&m_nIp, 1);
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*(float*)&ScriptParams[0] > *(float*)ptr);
		return 0;
	}
	case COMMAND_IS_NUMBER_GREATER_THAN_FLOAT_LVAR:
	{
		CollectParameters(&m_nIp, 1);
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*(float*)&ScriptParams[0] > *(float*)ptr);
		return 0;
	}
	case COMMAND_IS_FLOAT_VAR_GREATER_THAN_FLOAT_VAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*(float*)ptr1 > *(float*)ptr2);
		return 0;
	}
	case COMMAND_IS_FLOAT_LVAR_GREATER_THAN_FLOAT_VAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*(float*)ptr1 > *(float*)ptr2);
		return 0;
	}
	case COMMAND_IS_FLOAT_VAR_GREATER_THAN_FLOAT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*(float*)ptr1 > *(float*)ptr2);
		return 0;
	}
	case COMMAND_IS_FLOAT_LVAR_GREATER_THAN_FLOAT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*(float*)ptr1 > *(float*)ptr2);
		return 0;
	}
	case COMMAND_IS_INT_VAR_GREATER_OR_EQUAL_TO_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*ptr >= ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_INT_LVAR_GREATER_OR_EQUAL_TO_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*ptr >= ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_INT_VAR:
	{
		CollectParameters(&m_nIp, 1);
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(ScriptParams[0] >= *ptr);
		return 0;
	}
	case COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_INT_LVAR:
	{
		CollectParameters(&m_nIp, 1);
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(ScriptParams[0] >= *ptr);
		return 0;
	}
	case COMMAND_IS_INT_VAR_GREATER_OR_EQUAL_TO_INT_VAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*ptr1 >= *ptr2);
		return 0;
	}
	case COMMAND_IS_INT_LVAR_GREATER_OR_EQUAL_TO_INT_VAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*ptr1 >= *ptr2);
		return 0;
	}
	case COMMAND_IS_INT_VAR_GREATER_OR_EQUAL_TO_INT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*ptr1 >= *ptr2);
		return 0;
	}
	case COMMAND_IS_INT_LVAR_GREATER_OR_EQUAL_TO_INT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*ptr1 >= *ptr2);
		return 0;
	}
	case COMMAND_IS_FLOAT_VAR_GREATER_OR_EQUAL_TO_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*(float*)ptr >= *(float*)&ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_FLOAT_LVAR_GREATER_OR_EQUAL_TO_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*(float*)ptr >= *(float*)&ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_FLOAT_VAR:
	{
		CollectParameters(&m_nIp, 1);
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*(float*)&ScriptParams[0] >= *(float*)ptr);
		return 0;
	}
	case COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_FLOAT_LVAR:
	{
		CollectParameters(&m_nIp, 1);
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*(float*)&ScriptParams[0] >= *(float*)ptr);
		return 0;
	}
	case COMMAND_IS_FLOAT_VAR_GREATER_OR_EQUAL_TO_FLOAT_VAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*(float*)ptr1 >= *(float*)ptr2);
		return 0;
	}
	case COMMAND_IS_FLOAT_LVAR_GREATER_OR_EQUAL_TO_FLOAT_VAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*(float*)ptr1 >= *(float*)ptr2);
		return 0;
	}
	case COMMAND_IS_FLOAT_VAR_GREATER_OR_EQUAL_TO_FLOAT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*(float*)ptr1 >= *(float*)ptr2);
		return 0;
	}
	case COMMAND_IS_FLOAT_LVAR_GREATER_OR_EQUAL_TO_FLOAT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*(float*)ptr1 >= *(float*)ptr2);
		return 0;
	}
	case COMMAND_IS_INT_VAR_EQUAL_TO_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*ptr == ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_INT_LVAR_EQUAL_TO_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*ptr == ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_INT_VAR_EQUAL_TO_INT_VAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*ptr1 == *ptr2);
		return 0;
	}
	case COMMAND_IS_INT_VAR_EQUAL_TO_INT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*ptr1 == *ptr2);
		return 0;
	}
	case COMMAND_IS_INT_LVAR_EQUAL_TO_INT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*ptr1 == *ptr2);
		return 0;
	}
	/* Following commands are not implemented, and go to default case
	case COMMAND_IS_INT_VAR_NOT_EQUAL_TO_NUMBER:
	case COMMAND_IS_INT_LVAR_NOT_EQUAL_TO_NUMBER:
	case COMMAND_IS_INT_VAR_NOT_EQUAL_TO_INT_VAR:
	case COMMAND_IS_INT_LVAR_NOT_EQUAL_TO_INT_LVAR:
	case COMMAND_IS_INT_VAR_NOT_EQUAL_TO_INT_LVAR:
	*/
	case COMMAND_IS_FLOAT_VAR_EQUAL_TO_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*(float*)ptr == *(float*)&ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_FLOAT_LVAR_EQUAL_TO_NUMBER:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(*(float*)ptr == *(float*)&ScriptParams[0]);
		return 0;
	}
	case COMMAND_IS_FLOAT_VAR_EQUAL_TO_FLOAT_VAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		UpdateCompareFlag(*(float*)ptr1 == *(float*)ptr2);
		return 0;
	}
	case COMMAND_IS_FLOAT_VAR_EQUAL_TO_FLOAT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*(float*)ptr1 == *(float*)ptr2);
		return 0;
	}
	case COMMAND_IS_FLOAT_LVAR_EQUAL_TO_FLOAT_LVAR:
	{
		int32* ptr1 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		int32* ptr2 = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		UpdateCompareFlag(*(float*)ptr1 == *(float*)ptr2);
		return 0;
	}
	/* Following commands are not implemented, and go to default case
	case COMMAND_IS_FLOAT_VAR_NOT_EQUAL_TO_NUMBER:
	case COMMAND_IS_FLOAT_LVAR_NOT_EQUAL_TO_NUMBER:
	case COMMAND_IS_FLOAT_VAR_NOT_EQUAL_TO_FLOAT_VAR:
	case COMMAND_IS_FLOAT_LVAR_NOT_EQUAL_TO_FLOAT_LVAR:
	case COMMAND_IS_FLOAT_VAR_NOT_EQUAL_TO_FLOAT_LVAR:
	*/
	case COMMAND_GOTO_IF_TRUE:
		CollectParameters(&m_nIp, 1);
		if (m_bCondResult)
			SetIP(ScriptParams[0] >= 0 ? ScriptParams[0] : SIZE_MAIN_SCRIPT - ScriptParams[0]);
		/* Check COMMAND_GOTO note. */
		return 0;
	case COMMAND_GOTO_IF_FALSE:
		CollectParameters(&m_nIp, 1);
		if (!m_bCondResult)
			SetIP(ScriptParams[0] >= 0 ? ScriptParams[0] : SIZE_MAIN_SCRIPT - ScriptParams[0]);
		/* Check COMMAND_GOTO note. */
		return 0;
	case COMMAND_TERMINATE_THIS_SCRIPT:
		if (m_bMissionFlag)
			CTheScripts::bAlreadyRunningAMissionScript = false;
		RemoveScriptFromList(&CTheScripts::pActiveScripts);
		AddScriptToList(&CTheScripts::pIdleScripts);
		return 1;
	case COMMAND_START_NEW_SCRIPT:
	{
		CollectParameters(&m_nIp, 1);
		assert(ScriptParams[0] >= 0);
		CRunningScript* pNew = CTheScripts::StartNewScript(ScriptParams[0]);
		int8 type = CTheScripts::Read1ByteFromScript(&m_nIp);
		float tmp;
		for (int i = 0; type != ARGUMENT_END; type = CTheScripts::Read1ByteFromScript(&m_nIp), i++) {
			switch (type) {
			case ARGUMENT_INT32:
				pNew->m_anLocalVariables[i] = CTheScripts::Read4BytesFromScript(&m_nIp);
				break;
			case ARGUMENT_GLOBALVAR:
				pNew->m_anLocalVariables[i] = *(int32*)&CTheScripts::ScriptSpace[CTheScripts::Read2BytesFromScript(&m_nIp)];
				break;
			case ARGUMENT_LOCALVAR:
				pNew->m_anLocalVariables[i] = m_anLocalVariables[CTheScripts::Read2BytesFromScript(&m_nIp)];
				break;
			case ARGUMENT_INT8:
				pNew->m_anLocalVariables[i] = CTheScripts::Read1ByteFromScript(&m_nIp);
				break;
			case ARGUMENT_INT16:
				pNew->m_anLocalVariables[i] = CTheScripts::Read2BytesFromScript(&m_nIp);
				break;
			case ARGUMENT_FLOAT:
				tmp = CTheScripts::ReadFloatFromScript(&m_nIp);
				pNew->m_anLocalVariables[i] = *(int32*)&tmp;
				break;
			default:
				break;
			}
		}
		return 0;
	}
	case COMMAND_GOSUB:
		CollectParameters(&m_nIp, 1);
		assert(m_nStackPointer < MAX_STACK_DEPTH);
		m_anStack[m_nStackPointer++] = m_nIp;
		SetIP(ScriptParams[0] >= 0 ? ScriptParams[0] : SIZE_MAIN_SCRIPT - ScriptParams[0]);
		return 0;
	case COMMAND_RETURN:
		assert(m_nStackPointer > 0); /* No more SSU */
		SetIP(m_anStack[--m_nStackPointer]);
		return 0;
	case COMMAND_LINE:
		CollectParameters(&m_nIp, 6);
		/* Something must have been here */
		return 0;
	case COMMAND_CREATE_PLAYER:
	{
		CollectParameters(&m_nIp, 4);
		int32 index = ScriptParams[0];
		assert(index < 1); /* Constant? Also no more double player glitch */
		debug("&&&&&&&&&&&&&Creating player: %d\n", index);
		if (!CStreaming::HasModelLoaded(MI_PLAYER)) {
			CStreaming::RequestSpecialModel(MI_PLAYER, "player", STREAMFLAGS_DONT_REMOVE | STREAMFLAGS_DEPENDENCY);
			CStreaming::LoadAllRequestedModels(false);
		}
		CPlayerPed::SetupPlayerPed(index);
		CWorld::Players[index].m_pPed->CharCreatedBy = MISSION_CHAR;
		CPlayerPed::DeactivatePlayerPed(index);
		CVector pos = *(CVector*)&ScriptParams[1];
		if (pos.z <= -100.0f)
			pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		pos.z += CWorld::Players[index].m_pPed->GetDistanceFromCentreOfMassToBaseOfModel();
		CWorld::Players[index].m_pPed->GetPosition() = pos;
		CTheScripts::ClearSpaceForMissionEntity(pos, CWorld::Players[index].m_pPed);
		CPlayerPed::ReactivatePlayerPed(index);
		ScriptParams[0] = index;
		StoreParameters(&m_nIp, 1);
		return 0;
	}
	case COMMAND_GET_PLAYER_COORDINATES:
	{
		CVector pos;
		CollectParameters(&m_nIp, 1);
		if (CWorld::Players[ScriptParams[0]].m_pPed->bInVehicle)
			pos = CWorld::Players[ScriptParams[0]].m_pPed->m_pMyVehicle->GetPosition();
		else
			pos = CWorld::Players[ScriptParams[0]].m_pPed->GetPosition();
		*(CVector*)&ScriptParams[0] = pos;
		StoreParameters(&m_nIp, 3);
		return 0;
	}
	case COMMAND_SET_PLAYER_COORDINATES:
	{
		CollectParameters(&m_nIp, 4);
		CVector pos = *(CVector*)&ScriptParams[1];
		int index = ScriptParams[0];
		if (pos.z <= -100.0f)
			pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		CPlayerPed* ped = CWorld::Players[index].m_pPed;
		if (!ped->bInVehicle) {
			pos.z += ped->GetDistanceFromCentreOfMassToBaseOfModel();
			ped->Teleport(pos);
			CTheScripts::ClearSpaceForMissionEntity(pos, ped);
			return 0;
		}
		pos.z += ped->m_pMyVehicle->GetDistanceFromCentreOfMassToBaseOfModel();
		if (ped->m_pMyVehicle->IsBoat())
			ped->m_pMyVehicle->Teleport(pos);
		else
			ped->m_pMyVehicle->Teleport(pos);
		/* I'll keep this condition here but obviously it is absolutely pointless */
		/* It's clearly present in disassembly so it had to be in original code */
		CTheScripts::ClearSpaceForMissionEntity(pos, ped->m_pMyVehicle);
		return 0;
	}
	case COMMAND_IS_PLAYER_IN_AREA_2D:
	{
		CollectParameters(&m_nIp, 6);
		CPlayerPed* ped = CWorld::Players[ScriptParams[0]].m_pPed;
		float x1, y1, x2, y2;
		x1 = *(float*)&ScriptParams[1];
		y1 = *(float*)&ScriptParams[2];
		x2 = *(float*)&ScriptParams[3];
		y2 = *(float*)&ScriptParams[4];
		if (!ped->bInVehicle)
			UpdateCompareFlag(ped->IsWithinArea(x1, y1, x2, y2));
		else
			UpdateCompareFlag(ped->m_pMyVehicle->IsWithinArea(x1, y1, x2, y2));
		if (!ScriptParams[5])
			return 0;
		CTheScripts::HighlightImportantArea((uint32)this + m_nIp, x1, y1, x2, y2, -100.0f);
		if (CTheScripts::DbgFlag)
			CTheScripts::DrawDebugSquare(x1, y1, x2, y2);
		return 0;
	}
	case COMMAND_IS_PLAYER_IN_AREA_3D:
	{
		CollectParameters(&m_nIp, 8);
		CPlayerPed* ped = CWorld::Players[ScriptParams[0]].m_pPed;
		float x1, y1, z1, x2, y2, z2;
		x1 = *(float*)&ScriptParams[1];
		y1 = *(float*)&ScriptParams[2];
		z1 = *(float*)&ScriptParams[3];
		x2 = *(float*)&ScriptParams[4];
		y2 = *(float*)&ScriptParams[5];
		z2 = *(float*)&ScriptParams[6];
		if (ped->bInVehicle)
			UpdateCompareFlag(ped->m_pMyVehicle->IsWithinArea(x1, y1, z1, x2, y2, z2));
		else
			UpdateCompareFlag(ped->IsWithinArea(x1, y1, z1, x2, y2, z2));
		if (!ScriptParams[7])
			return 0;
		CTheScripts::HighlightImportantArea((uint32)this + m_nIp, x1, y1, x2, y2, (z1 + z2) / 2);
		if (CTheScripts::DbgFlag)
			CTheScripts::DrawDebugCube(x1, y1, z1, x2, y2, z2);
		return 0;
	}
	case COMMAND_ADD_INT_VAR_TO_INT_VAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) += *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_ADD_INT_LVAR_TO_INT_VAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) += *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_ADD_INT_VAR_TO_INT_LVAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) += *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_ADD_INT_LVAR_TO_INT_LVAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) += *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_ADD_FLOAT_VAR_TO_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) += *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_ADD_FLOAT_LVAR_TO_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) += *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_ADD_FLOAT_VAR_TO_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) += *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_ADD_FLOAT_LVAR_TO_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) += *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_SUB_INT_VAR_FROM_INT_VAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) -= *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_SUB_INT_LVAR_FROM_INT_LVAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) -= *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_SUB_FLOAT_VAR_FROM_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) -= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_SUB_FLOAT_LVAR_FROM_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) -= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	default:
		assert(0);
		break;
	}
	return -1;
}

void CRunningScript::UpdateCompareFlag(bool flag)
{
	if (m_bNotFlag)
		flag = !flag;
	if (m_nAndOrState == 0){
		m_bCondResult = flag;
		return;
	}
	if (m_nAndOrState >= 1 && m_nAndOrState <= 8) { /* Maybe enums?*/
		m_bCondResult &= flag;
		if (m_nAndOrState == 1){
			m_nAndOrState = 0;
			return;
		}
	}else if (m_nAndOrState >= 21 && m_nAndOrState <= 28){
		m_bCondResult |= flag;
		if (m_nAndOrState == 21) {
			m_nAndOrState = 0;
			return;
		}
	}else{
		return;
	}
	m_nAndOrState--;
}


int8 CRunningScript::ProcessCommandsFrom100To199(int32 command)
{
	switch (command) {
	case COMMAND_SUB_INT_LVAR_FROM_INT_VAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) -= *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_SUB_INT_VAR_FROM_INT_LVAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) -= *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_SUB_FLOAT_LVAR_FROM_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) -= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_SUB_FLOAT_VAR_FROM_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) -= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_MULT_INT_VAR_BY_INT_VAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) *= *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_MULT_INT_LVAR_BY_INT_VAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) *= *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_MULT_INT_VAR_BY_INT_LVAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) *= *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_MULT_INT_LVAR_BY_INT_LVAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) *= *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_MULT_FLOAT_VAR_BY_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) *= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_MULT_FLOAT_LVAR_BY_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) *= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_MULT_FLOAT_VAR_BY_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) *= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_MULT_FLOAT_LVAR_BY_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) *= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_DIV_INT_VAR_BY_INT_VAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) /= *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_DIV_INT_LVAR_BY_INT_VAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) /= *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_DIV_INT_VAR_BY_INT_LVAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) /= *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_DIV_INT_LVAR_BY_INT_LVAR:
		*GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) /= *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_DIV_FLOAT_VAR_BY_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) /= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_DIV_FLOAT_LVAR_BY_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) /= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_DIV_FLOAT_VAR_BY_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) /= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_DIV_FLOAT_LVAR_BY_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) /= *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_ADD_TIMED_VAL_TO_FLOAT_VAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr += CTimer::GetTimeStep() * *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_ADD_TIMED_VAL_TO_FLOAT_LVAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr += CTimer::GetTimeStep() * *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_ADD_TIMED_FLOAT_VAR_TO_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) += CTimer::GetTimeStep() * *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_ADD_TIMED_FLOAT_LVAR_TO_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) += CTimer::GetTimeStep() * *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_ADD_TIMED_FLOAT_VAR_TO_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) += CTimer::GetTimeStep() * *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_ADD_TIMED_FLOAT_LVAR_TO_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) += CTimer::GetTimeStep() * *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_SUB_TIMED_VAL_FROM_FLOAT_VAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr -= CTimer::GetTimeStep() * *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_SUB_TIMED_VAL_FROM_FLOAT_LVAR:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		CollectParameters(&m_nIp, 1);
		*(float*)ptr -= CTimer::GetTimeStep() * *(float*)&ScriptParams[0];
		return 0;
	}
	case COMMAND_SUB_TIMED_FLOAT_VAR_FROM_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) -= CTimer::GetTimeStep() * *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_SUB_TIMED_FLOAT_LVAR_FROM_FLOAT_VAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) -= CTimer::GetTimeStep() * *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_SUB_TIMED_FLOAT_VAR_FROM_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) -= CTimer::GetTimeStep() * *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	case COMMAND_SUB_TIMED_FLOAT_LVAR_FROM_FLOAT_LVAR:
		*(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL) -= CTimer::GetTimeStep() * *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	case COMMAND_SET_VAR_INT_TO_VAR_INT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		*ptr = *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	}
	case COMMAND_SET_LVAR_INT_TO_VAR_INT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		*ptr = *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	}
	case COMMAND_SET_VAR_INT_TO_LVAR_INT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		*ptr = *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	}
	case COMMAND_SET_LVAR_INT_TO_LVAR_INT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		*ptr = *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	}
	case COMMAND_SET_VAR_FLOAT_TO_VAR_FLOAT:
	{
		float* ptr = (float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		*ptr = *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	}
	case COMMAND_SET_LVAR_FLOAT_TO_VAR_FLOAT:
	{
		float* ptr = (float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		*ptr = *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	}
	case COMMAND_SET_VAR_FLOAT_TO_LVAR_FLOAT:
	{
		float* ptr = (float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		*ptr = *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	}
	case COMMAND_SET_LVAR_FLOAT_TO_LVAR_FLOAT:
	{
		float* ptr = (float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		*ptr = *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	}
	case COMMAND_CSET_VAR_INT_TO_VAR_FLOAT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		*ptr = *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	}
	case COMMAND_CSET_LVAR_INT_TO_VAR_FLOAT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		*ptr = *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	}
	case COMMAND_CSET_VAR_INT_TO_LVAR_FLOAT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		*ptr = *(float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	}
	case COMMAND_CSET_LVAR_INT_TO_LVAR_FLOAT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		*ptr = *(float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	}
	case COMMAND_CSET_VAR_FLOAT_TO_VAR_INT:
	{
		float* ptr = (float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		*ptr = *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	}
	case COMMAND_CSET_LVAR_FLOAT_TO_VAR_INT:
	{
		float* ptr = (float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		*ptr = *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	}
	case COMMAND_CSET_VAR_FLOAT_TO_LVAR_INT:
	{
		float* ptr = (float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		*ptr = *GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		return 0;
	}
	case COMMAND_CSET_LVAR_FLOAT_TO_LVAR_INT:
	{
		float* ptr = (float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		*ptr = *GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		return 0;
	}
	case COMMAND_ABS_VAR_INT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		*ptr = ABS(*ptr);
		return 0;
	}
	case COMMAND_ABS_LVAR_INT:
	{
		int32* ptr = GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		*ptr = ABS(*ptr);
		return 0;
	}
	case COMMAND_ABS_VAR_FLOAT:
	{
		float* ptr = (float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		*ptr = ABS(*ptr);
		return 0;
	}
	case COMMAND_ABS_LVAR_FLOAT:
	{
		float* ptr = (float*)GetPointerToScriptVariable(&m_nIp, VAR_LOCAL);
		*ptr = ABS(*ptr);
		return 0;
	}
	case COMMAND_GENERATE_RANDOM_FLOAT:
	{
		float* ptr = (float*)GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL);
		CGeneral::GetRandomNumber();
		CGeneral::GetRandomNumber();
		CGeneral::GetRandomNumber(); /* To make it EXTRA random! */
		*ptr = CGeneral::GetRandomNumber() / 65536.0f;
		/* Between 0 and 0.5 on PC (oh well...), never used in original script. */
		return 0;
	}
	case COMMAND_GENERATE_RANDOM_INT:
		/* On PC between 0 and 32767, even though script expects values between 0 and 65536 */
		*GetPointerToScriptVariable(&m_nIp, VAR_GLOBAL) = CGeneral::GetRandomNumber();
		return 0;
	case COMMAND_CREATE_CHAR:
	{
		CollectParameters(&m_nIp, 5);
		switch (ScriptParams[1]) {
		case MI_COP:
			if (ScriptParams[0] == PEDTYPE_COP)
				ScriptParams[1] = COP_STREET;
			break;
		case MI_SWAT:
			if (ScriptParams[0] == PEDTYPE_COP)
				ScriptParams[1] = COP_SWAT;
			break;
		case MI_FBI:
			if (ScriptParams[0] == PEDTYPE_COP)
				ScriptParams[1] = COP_FBI;
			break;
		case MI_ARMY:
			if (ScriptParams[0] == PEDTYPE_COP)
				ScriptParams[1] = COP_ARMY;
			break;
		case MI_MEDIC:
			if (ScriptParams[0] == PEDTYPE_EMERGENCY)
				ScriptParams[1] = PEDTYPE_EMERGENCY;
			break;
		case MI_FIREMAN:
			if (ScriptParams[0] == PEDTYPE_FIREMAN)
				ScriptParams[1] = PEDTYPE_FIREMAN;
			break;
		default:
			break;
		}
		CPed* ped;
		if (ScriptParams[0] == PEDTYPE_COP)
			ped = new CCopPed((eCopType)ScriptParams[1]);
		else if (ScriptParams[0] == PEDTYPE_EMERGENCY || ScriptParams[0] == PEDTYPE_FIREMAN)
			ped = new CEmergencyPed(ScriptParams[1]);
		else
			ped = new CCivilianPed(ScriptParams[0], ScriptParams[1]);
		ped->CharCreatedBy = MISSION_CHAR;
		ped->bRespondsToThreats = false;
		ped->m_ped_flagG2 = false;
		CVector pos = *(CVector*)&ScriptParams[2];
		if (pos.z <= -100.0f)
			pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		pos.z += 1.0f;
		ped->GetPosition() = pos;
		ped->SetOrientation(0.0f, 0.0f, 0.0f);
		CTheScripts::ClearSpaceForMissionEntity(pos, ped);
		CWorld::Add(ped);
		ped->m_level = CTheZones::GetLevelFromPosition(pos);
		CPopulation::ms_nTotalMissionPeds++;
		ScriptParams[0] = CPools::GetPedPool()->GetIndex(ped);
		StoreParameters(&m_nIp, 1);
		if (m_bIsMissionScript)
			CTheScripts::MissionCleanup.AddEntityToList(ScriptParams[0], CLEANUP_CHAR);
		return 0;
	}
	case COMMAND_DELETE_CHAR:
	{
		CollectParameters(&m_nIp, 1);
		CPed* ped = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		if (ped) {
			if (ped->bInVehicle && ped->m_pMyVehicle) {
				if (ped->m_pMyVehicle->pDriver == ped) {
					ped->m_pMyVehicle->RemoveDriver();
					ped->m_pMyVehicle->m_status = STATUS_ABANDONED;
					if (ped->m_pMyVehicle->m_nDoorLock == CARLOCK_LOCKED_INITIALLY)
						ped->m_pMyVehicle->m_nDoorLock = CARLOCK_UNLOCKED;
					if (ped->m_nPedType == PEDTYPE_COP && ped->m_pMyVehicle->IsLawEnforcementVehicle())
						ped->m_pMyVehicle->ChangeLawEnforcerState(0);
				}
				else {
					ped->m_pMyVehicle->RemovePassenger(ped);
				}
			}
			CWorld::RemoveReferencesToDeletedObject(ped);
			delete ped;
			--CPopulation::ms_nTotalMissionPeds;
		}
		if (m_bIsMissionScript)
			CTheScripts::MissionCleanup.RemoveEntityFromList(ScriptParams[0], CLEANUP_CHAR);
		return 0;
	}
	case COMMAND_CHAR_WANDER_DIR:
	{
		CollectParameters(&m_nIp, 2);
		CPed* ped = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(ped);
		ped->ClearAll();
		int8 path = ScriptParams[1];
		if (ScriptParams[1] < 0 || ScriptParams[1] > 7)
			path = CGeneral::GetRandomNumberInRange(0, 7);
		ped->SetWanderPath(path);
		return 0;
	}
	/* Not implemented.
	case COMMAND_CHAR_WANDER_RANGE:
	*/
	case COMMAND_CHAR_FOLLOW_PATH:
	{
		CollectParameters(&m_nIp, 4);
		CPed* ped = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(ped);
		CVector pos = *(CVector*)&ScriptParams[1];
		if (pos.z <= -100.0f)
			pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		ped->ClearAll();
		ped->SetFollowPath(pos);
		return 0;
	}
	case COMMAND_CHAR_SET_IDLE:
	{
		CollectParameters(&m_nIp, 1);
		CPed* ped = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(ped);
		ped->bScriptObjectiveCompleted = false;
		ped->SetObjective(OBJECTIVE_IDLE);
		return 0;
	}
	case COMMAND_GET_CHAR_COORDINATES:
	{
		CollectParameters(&m_nIp, 1);
		CPed* ped = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(ped);
		CVehicle* vehicle;
		CVector pos;
		/* Seems a bit clumsy but I'll leave original flow */
		if (ped->bInVehicle)
			vehicle = ped->m_pMyVehicle;
		else
			vehicle = nil;
		if (vehicle)
			pos = vehicle->GetPosition();
		else
			pos = ped->GetPosition();
		*(CVector*)&ScriptParams[0] = pos;
		StoreParameters(&m_nIp, 3);
		return 0;
	}
	case COMMAND_SET_CHAR_COORDINATES:
	{
		CollectParameters(&m_nIp, 4);
		CPed* ped = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(ped);
		CVehicle* vehicle;
		if (ped->bInVehicle)
			vehicle = ped->m_pMyVehicle;
		else
			vehicle = nil;
		CVector pos = *(CVector*)&ScriptParams[1];
		if (pos.z <= -100.0f)
			pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		/* The following block was once again written
		 * by someone not familiar with virtual functions.
		 * It doesn't require any ifs at all.
		 * To keep as close to original as possible, I'll keep it.
		 * Maybe there was more commented out/debug
		 * stuff, but I doubt it.
		 */
		if (!vehicle) {
			pos.z += ped->GetDistanceFromCentreOfMassToBaseOfModel();
			ped->Teleport(pos);
			CTheScripts::ClearSpaceForMissionEntity(pos, ped);
		}
		else if (vehicle->IsBoat()) {
			pos.z += vehicle->GetDistanceFromCentreOfMassToBaseOfModel();
			vehicle->Teleport(pos);
			CTheScripts::ClearSpaceForMissionEntity(pos, vehicle);
		}
		else {
			pos.z += vehicle->GetDistanceFromCentreOfMassToBaseOfModel();
			vehicle->Teleport(pos);
			CTheScripts::ClearSpaceForMissionEntity(pos, vehicle);
		}
		/* Short version of this command.
		 *
		 * CollectParameters(&m_nIp, 4);
		 * CPed* ped = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		 * assert(ped);
		 * CEntity* entityToMove = ped->bInVehicle ? ped->m_pMyVehicle : ped;
		 * CVector pos = *(CVector*)&ScriptParams[1];
		 * if (pos.z <= -100.0f)
		 *	pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		 * pos.z += entityToMove->GetDistanceFromCentreOfMassToBaseOfModel();
		 * entityToMove->Teleport(pos);
		 * CTheScripts::ClearSpaceForMissionEntity(pos, entityToMove);
		 *
		 */
		return 0;
	}
	case COMMAND_IS_CHAR_STILL_ALIVE:
	{
		CollectParameters(&m_nIp, 1);
		CPed* ped = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		UpdateCompareFlag(ped && ped->GetPedState() != PED_DEAD && ped->GetPedState() != PED_DIE);
		return 0;
	}
	case COMMAND_IS_CHAR_IN_AREA_2D:
	{
		CollectParameters(&m_nIp, 6);
		CPed* ped = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(ped);
		CVehicle* vehicle;
		if (ped->bInVehicle)
			vehicle = ped->m_pMyVehicle;
		else
			vehicle = nil;
		float x1, y1, x2, y2;
		x1 = *(float*)&ScriptParams[1];
		y1 = *(float*)&ScriptParams[2];
		x2 = *(float*)&ScriptParams[3];
		y2 = *(float*)&ScriptParams[4];
		if (vehicle)
			UpdateCompareFlag(ped->m_pMyVehicle->IsWithinArea(x1, y1, x2, y2));
		else
			UpdateCompareFlag(ped->IsWithinArea(x1, y1, x2, y2));
		if (!ScriptParams[5])
			return 0;
		CTheScripts::HighlightImportantArea((uint32)this + m_nIp, x1, y1, x2, y2, -100.0f);
		if (CTheScripts::DbgFlag)
			CTheScripts::DrawDebugSquare(x1, y1, x2, y2);
		return 0;
	}
	case COMMAND_IS_CHAR_IN_AREA_3D:
	{
		CollectParameters(&m_nIp, 8);
		CPed* ped = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(ped);
		CVehicle* vehicle;
		if (ped->bInVehicle)
			vehicle = ped->m_pMyVehicle;
		else
			vehicle = nil;
		float x1, y1, z1, x2, y2, z2;
		x1 = *(float*)&ScriptParams[1];
		y1 = *(float*)&ScriptParams[2];
		z1 = *(float*)&ScriptParams[3];
		x2 = *(float*)&ScriptParams[4];
		y2 = *(float*)&ScriptParams[5];
		z2 = *(float*)&ScriptParams[6];
		if (vehicle)
			UpdateCompareFlag(ped->m_pMyVehicle->IsWithinArea(x1, y1, z1, x2, y2, z2));
		else
			UpdateCompareFlag(ped->IsWithinArea(x1, y1, z1, x2, y2, z2));
		if (!ScriptParams[7])
			return 0;
		CTheScripts::HighlightImportantArea((uint32)this + m_nIp, x1, y1, x2, y2, (z1 + z2) / 2);
		if (CTheScripts::DbgFlag)
			CTheScripts::DrawDebugCube(x1, y1, z1, x2, y2, z2);
		return 0;
	}
	case COMMAND_CREATE_CAR:
	{
		CollectParameters(&m_nIp, 4);
		int32 handle;
		if (CModelInfo::IsBoatModel(ScriptParams[0])) {
			CBoat* boat = new CBoat(ScriptParams[0], MISSION_VEHICLE);
			CVector pos = *(CVector*)&ScriptParams[1];
			if (pos.z <= -100.0f)
				pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
			pos.z += boat->GetDistanceFromCentreOfMassToBaseOfModel();
			boat->GetPosition() = pos;
			CTheScripts::ClearSpaceForMissionEntity(pos, boat);
			boat->m_status = STATUS_ABANDONED;
			boat->bIsLocked = true;
			boat->AutoPilot.m_nCarMission = MISSION_NONE;
			boat->AutoPilot.m_nAnimationId = TEMPACT_NONE; /* Animation ID? */
			boat->AutoPilot.m_nCruiseSpeed = boat->AutoPilot.m_fMaxTrafficSpeed = 20.0f;
			CWorld::Add(boat);
			handle = CPools::GetVehiclePool()->GetIndex(boat);
		}
		else {
			CVehicle* car;
			if (!CModelInfo::IsBikeModel(ScriptParams[0]))
				car = new CAutomobile(ScriptParams[0], MISSION_VEHICLE);
			CVector pos = *(CVector*)&ScriptParams[1];
			if (pos.z <= -100.0f)
				pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
			pos.z += car->GetDistanceFromCentreOfMassToBaseOfModel();
			car->GetPosition() = pos;
			CTheScripts::ClearSpaceForMissionEntity(pos, car);
			car->m_status = STATUS_ABANDONED;
			car->bIsLocked = true;
			CCarCtrl::JoinCarWithRoadSystem(car);
			car->AutoPilot.m_nCarMission = MISSION_NONE;
			car->AutoPilot.m_nAnimationId = TEMPACT_NONE; /* Animation ID? */
			car->AutoPilot.m_nDrivingStyle = DRIVINGSTYLE_STOP_FOR_CARS;
			car->AutoPilot.m_nCruiseSpeed = car->AutoPilot.m_fMaxTrafficSpeed = 9.0f;
			car->AutoPilot.m_nPreviousLane = car->AutoPilot.m_nCurrentLane = 0;
			car->bEngineOn = false;
			car->m_level = CTheZones::GetLevelFromPosition(pos);
			car->bHasBeenOwnedByPlayer = true;
			CWorld::Add(car);
			handle = CPools::GetVehiclePool()->GetIndex(car);
		}
		ScriptParams[0] = handle;
		StoreParameters(&m_nIp, 1);
		if (m_bIsMissionScript)
			CTheScripts::MissionCleanup.AddEntityToList(handle, CLEANUP_CAR);
		return 0;
	}
	case COMMAND_DELETE_CAR:
	{
		CollectParameters(&m_nIp, 1);
		CVehicle* car = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		if (car) {
			CWorld::Remove(car);
			CWorld::RemoveReferencesToDeletedObject(car);
			delete car;
		}
		if (m_bIsMissionScript)
			CTheScripts::MissionCleanup.RemoveEntityFromList(ScriptParams[0], CLEANUP_CAR);
		return 0;
	}
	case COMMAND_CAR_GOTO_COORDINATES:
	{
		CollectParameters(&m_nIp, 4);
		CVehicle* car = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(car);
		CVector pos = *(CVector*)&ScriptParams[1];
		if (pos.z <= -100.0f)
			pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		pos.z += car->GetDistanceFromCentreOfMassToBaseOfModel();
		if (CCarCtrl::JoinCarWithRoadSystemGotoCoors(car, pos, false))
			car->AutoPilot.m_nCarMission = MISSION_GOTOCOORDS_STRAIGHT;
		else
			car->AutoPilot.m_nCarMission = MISSION_GOTOCOORDS;
		car->m_status = STATUS_PHYSICS;
		car->bEngineOn = true;
		car->AutoPilot.m_nCruiseSpeed = max(car->AutoPilot.m_nCruiseSpeed, 6);
		car->AutoPilot.m_nTimeToStartMission = CTimer::GetTimeInMilliseconds();
		return 0;
	}
	case COMMAND_CAR_WANDER_RANDOMLY:
	{
		CollectParameters(&m_nIp, 1);
		CVehicle* car = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(car);
		CCarCtrl::JoinCarWithRoadSystem(car);
		car->AutoPilot.m_nCarMission = MISSION_CRUISE;
		car->bEngineOn = true;
		car->AutoPilot.m_nCruiseSpeed = max(car->AutoPilot.m_nCruiseSpeed, 6);
		car->AutoPilot.m_nTimeToStartMission = CTimer::GetTimeInMilliseconds();
		return 0;
	}
	case COMMAND_CAR_SET_IDLE:
	{
		CollectParameters(&m_nIp, 1);
		CVehicle* car = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(car);
		car->AutoPilot.m_nCarMission = MISSION_NONE;
		return 0;
	}
	case COMMAND_GET_CAR_COORDINATES:
	{
		CollectParameters(&m_nIp, 1);
		CVehicle* car = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(car);
		*(CVector*)&ScriptParams[0] = car->GetPosition();
		StoreParameters(&m_nIp, 3);
		return 0;
	}
	case COMMAND_SET_CAR_COORDINATES:
	{
		CollectParameters(&m_nIp, 4);
		CVehicle* car = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(car);
		CVector pos = *(CVector*)&ScriptParams[1];
		if (pos.z <= -100.0f)
			pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		pos.z += car->GetDistanceFromCentreOfMassToBaseOfModel();
		car->bIsStatic = false;
		/* Again weird usage of virtual functions. */
		if (car->IsBoat()) {
			car->Teleport(pos);
			CTheScripts::ClearSpaceForMissionEntity(pos, car);
		}
		else {
			car->Teleport(pos);
			CTheScripts::ClearSpaceForMissionEntity(pos, car);
			/* May the following be inlined CCarCtrl function? */
			switch (car->AutoPilot.m_nCarMission) {
			case MISSION_CRUISE:
				CCarCtrl::JoinCarWithRoadSystem(car);
				break;
			case MISSION_RAMPLAYER_FARAWAY:
			case MISSION_RAMPLAYER_CLOSE:
			case MISSION_BLOCKPLAYER_FARAWAY:
			case MISSION_BLOCKPLAYER_CLOSE:
			case MISSION_BLOCKPLAYER_HANDBRAKESTOP:
				CCarCtrl::JoinCarWithRoadSystemGotoCoors(car, FindPlayerCoors(), false);
				break;
			case MISSION_GOTOCOORDS:
			case MISSION_GOTOCOORDS_STRAIGHT:
				CCarCtrl::JoinCarWithRoadSystemGotoCoors(car, car->AutoPilot.m_vecDestinationCoors, false);
				break;
			case MISSION_GOTOCOORDS_ACCURATE:
			case MISSION_GOTO_COORDS_STRAIGHT_ACCURATE:
				CCarCtrl::JoinCarWithRoadSystemGotoCoors(car, car->AutoPilot.m_vecDestinationCoors, false);
				break;
			case MISSION_RAMCAR_FARAWAY:
			case MISSION_RAMCAR_CLOSE:
			case MISSION_BLOCKCAR_FARAWAY:
			case MISSION_BLOCKCAR_CLOSE:
			case MISSION_BLOCKCAR_HANDBRAKESTOP:
				CCarCtrl::JoinCarWithRoadSystemGotoCoors(car, car->AutoPilot.m_pTargetCar->GetPosition(), false);
				break;
			default:
				break;
			}
		}
		return 0;
	}
	case COMMAND_IS_CAR_STILL_ALIVE:
	{
		CollectParameters(&m_nIp, 1);
		CVehicle* car = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		UpdateCompareFlag(car && car->m_status != STATUS_WRECKED && (car->IsBoat() || !car->bIsInWater));
		return 0;
	}
	case COMMAND_SET_CAR_CRUISE_SPEED:
	{
		CollectParameters(&m_nIp, 2);
		CVehicle* car = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(car);
		car->AutoPilot.m_nCruiseSpeed = min(*(float*)&ScriptParams[1], 60.0f * car->pHandling->Transmission.fUnkMaxVelocity);
		return 0;
	}
	case COMMAND_SET_CAR_DRIVING_STYLE:
	{
		CollectParameters(&m_nIp, 2);
		CVehicle* car = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(car);
		car->AutoPilot.m_nDrivingStyle = (eCarDrivingStyle)ScriptParams[1];
		return 0;
	}
	case COMMAND_SET_CAR_MISSION:
	{
		CollectParameters(&m_nIp, 2);
		CVehicle* car = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(car);
		car->AutoPilot.m_nCarMission = (eCarMission)ScriptParams[1];
		car->AutoPilot.m_nTimeToStartMission = CTimer::GetTimeInMilliseconds();
		car->bEngineOn = true;
		return 0;
	}
	case COMMAND_IS_CAR_IN_AREA_2D:
	{
		CollectParameters(&m_nIp, 6);
		CVehicle* vehicle = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(vehicle);
		float x1, y1, x2, y2;
		x1 = *(float*)&ScriptParams[1];
		y1 = *(float*)&ScriptParams[2];
		x2 = *(float*)&ScriptParams[3];
		y2 = *(float*)&ScriptParams[4];
		UpdateCompareFlag(vehicle->IsWithinArea(x1, y1, x2, y2));
		if (!ScriptParams[5])
			return 0;
		CTheScripts::HighlightImportantArea((uint32)this + m_nIp, x1, y1, x2, y2, -100.0f);
		if (CTheScripts::DbgFlag)
			CTheScripts::DrawDebugSquare(x1, y1, x2, y2);
		return 0;
	}
	case COMMAND_IS_CAR_IN_AREA_3D:
	{
		CollectParameters(&m_nIp, 8);
		CVehicle* vehicle = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(vehicle);
		float x1, y1, z1, x2, y2, z2;
		x1 = *(float*)&ScriptParams[1];
		y1 = *(float*)&ScriptParams[2];
		z1 = *(float*)&ScriptParams[3];
		x2 = *(float*)&ScriptParams[4];
		y2 = *(float*)&ScriptParams[5];
		z2 = *(float*)&ScriptParams[6];
		UpdateCompareFlag(vehicle->IsWithinArea(x1, y1, z1, x2, y2, z2));
		if (!ScriptParams[7])
			return 0;
		CTheScripts::HighlightImportantArea((uint32)this + m_nIp, x1, y1, x2, y2, (z1 + z2) / 2);
		if (CTheScripts::DbgFlag)
			CTheScripts::DrawDebugCube(x1, y1, z1, x2, y2, z2);
		return 0;
	}
	case COMMAND_SPECIAL_0:
	case COMMAND_SPECIAL_1:
	case COMMAND_SPECIAL_2:
	case COMMAND_SPECIAL_3:
	case COMMAND_SPECIAL_4:
	case COMMAND_SPECIAL_5:
	case COMMAND_SPECIAL_6:
	case COMMAND_SPECIAL_7:
		assert(0);
		return 0;
	case COMMAND_PRINT_BIG:
	{
		wchar* key = TheText.Get((char*)&CTheScripts::ScriptSpace[m_nIp]);
		m_nIp += 8;
		CollectParameters(&m_nIp, 2);
		CMessages::AddBigMessage(key, ScriptParams[0], ScriptParams[1] - 1);
		return 0;
	}
	case COMMAND_PRINT:
	{
		wchar* key = TheText.Get((char*)&CTheScripts::ScriptSpace[m_nIp]);
		m_nIp += 8;
		CollectParameters(&m_nIp, 2);
		CMessages::AddMessage(key, ScriptParams[0], ScriptParams[1]);
		return 0;
	}
	case COMMAND_PRINT_NOW:
	{
		wchar* key = TheText.Get((char*)&CTheScripts::ScriptSpace[m_nIp]);
		m_nIp += 8;
		CollectParameters(&m_nIp, 2);
		CMessages::AddMessageJumpQ(key, ScriptParams[0], ScriptParams[1]);
		return 0;
	}
	case COMMAND_PRINT_SOON:
	{
		wchar* key = TheText.Get((char*)&CTheScripts::ScriptSpace[m_nIp]);
		m_nIp += 8;
		CollectParameters(&m_nIp, 2);
		CMessages::AddMessage(key, ScriptParams[0], ScriptParams[1]);
		return 0;
	}
	case COMMAND_CLEAR_PRINTS:
		CMessages::ClearMessages();
		return 0;
	case COMMAND_GET_TIME_OF_DAY:
		ScriptParams[0] = CClock::GetHours();
		ScriptParams[1] = CClock::GetMinutes();
		StoreParameters(&m_nIp, 2);
		return 0;
	case COMMAND_SET_TIME_OF_DAY:
		CollectParameters(&m_nIp, 2);
		CClock::SetGameClock(ScriptParams[0], ScriptParams[1]);
		return 0;
	case COMMAND_GET_MINUTES_TO_TIME_OF_DAY:
		CollectParameters(&m_nIp, 2);
		ScriptParams[0] = CClock::GetGameClockMinutesUntil(ScriptParams[0], ScriptParams[1]);
		StoreParameters(&m_nIp, 1);
		return 0;
	case COMMAND_IS_POINT_ON_SCREEN:
	{
		CollectParameters(&m_nIp, 4);
		CVector pos = *(CVector*)&ScriptParams[0];
		if (pos.z <= -100)
			pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		UpdateCompareFlag(TheCamera.IsSphereVisible(pos, *(float*)&ScriptParams[3]));
	}
	case COMMAND_DEBUG_ON:
		CTheScripts::DbgFlag = true;
		return 0;
	case COMMAND_DEBUG_OFF:
		CTheScripts::DbgFlag = false;
		return 0;
	case COMMAND_RETURN_TRUE:
		UpdateCompareFlag(true);
		return 0;
	case COMMAND_RETURN_FALSE:
		UpdateCompareFlag(false);
		return 0;
	/* Special command only used by compiler. 
	case COMMAND_VAR_INT:
	*/
	default:
		assert(0);
		break;
	}
	return -1;
}

int8 CRunningScript::ProcessCommandsFrom200To299(int32 command)
{ 
	switch (command) {
	/* Special commands. 
	case COMMAND_VAR_FLOAT:
	case COMMAND_LVAR_INT:
	case COMMAND_LVAR_FLOAT:
	case COMMAND_LBRACKET:
	case COMMAND_RBRACKET:
	case COMMAND_REPEAT:
	case COMMAND_ENDREPEAT:
	case COMMAND_IF:
	case COMMAND_IFNOT:
	case COMMAND_ELSE:
	case COMMAND_ENDIF:
	case COMMAND_WHILE:
	case COMMAND_WHILENOT:
	case COMMAND_ENDWHILE:
	*/
	case COMMAND_ANDOR:
		CollectParameters(&m_nIp, 1);
		m_nAndOrState = ScriptParams[0];
		if (m_nAndOrState == ANDOR_NONE){
			m_bCondResult = false; // pointless
		}else if (m_nAndOrState >= ANDS_1 && m_nAndOrState <= ANDS_8){
			m_bCondResult = true;
			m_nAndOrState++;
		}else if (m_nAndOrState >= ORS_1 && m_nAndOrState <= ORS_8){
			m_bCondResult = false;
			m_nAndOrState++;
		}else{
			assert(0 && "COMMAND_ANDOR: invalid ANDOR state");
		}
		return 0;
	case COMMAND_LAUNCH_MISSION:
	{
		CollectParameters(&m_nIp, 1);
		CRunningScript* pNew = CTheScripts::StartNewScript(ScriptParams[0]);
		pNew->m_bIsMissionScript = true;
		return 0;
	}
	case COMMAND_MISSION_HAS_FINISHED:
	{
		if (!m_bIsMissionScript)
			return 0;
		if (strcmp(m_abScriptName, "love3") == 0) /* A Drop in the Ocean */
			CPickups::RemoveAllFloatingPickups();
		CTheScripts::MissionCleanup.Process();
		return 0;
	}
	case COMMAND_STORE_CAR_CHAR_IS_IN:
	{
		CollectParameters(&m_nIp, 1);
		CPed* ped = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(ped);
		CVehicle* pCurrent = nil;
		if (ped->bInVehicle) {
			pCurrent = ped->m_pMyVehicle;
		}
		assert(pCurrent); // GetIndex(0) doesn't look good
		int handle = CPools::GetVehiclePool()->GetIndex(pCurrent);
		if (handle != CTheScripts::StoreVehicleIndex && m_bIsMissionScript){
			CVehicle* pOld = CPools::GetVehiclePool()->GetAt(CTheScripts::StoreVehicleIndex);
			if (pOld){
				CCarCtrl::RemoveFromInterestingVehicleList(pOld);
				if (pOld->VehicleCreatedBy == MISSION_VEHICLE && CTheScripts::StoreVehicleWasRandom){
					pOld->VehicleCreatedBy = RANDOM_VEHICLE;
					pOld->bIsLocked = false;
					CCarCtrl::NumRandomCars++;
					CCarCtrl::NumMissionCars--;
					CTheScripts::MissionCleanup.RemoveEntityFromList(CTheScripts::StoreVehicleIndex, CLEANUP_CAR);
				}
			}
			CTheScripts::StoreVehicleIndex = handle;
			switch (pCurrent->VehicleCreatedBy){
			case RANDOM_VEHICLE:
				pCurrent->VehicleCreatedBy = MISSION_VEHICLE;
				CCarCtrl::NumMissionCars++;
				CCarCtrl::NumRandomCars--;
				CTheScripts::StoreVehicleWasRandom = true;
				CTheScripts::MissionCleanup.AddEntityToList(CTheScripts::StoreVehicleIndex, CLEANUP_CAR);
				break;
			case PARKED_VEHICLE:
				pCurrent->VehicleCreatedBy = MISSION_VEHICLE;
				CCarCtrl::NumMissionCars++;
				CCarCtrl::NumParkedCars--;
				CTheScripts::StoreVehicleWasRandom = true;
				CTheScripts::MissionCleanup.AddEntityToList(CTheScripts::StoreVehicleIndex, CLEANUP_CAR);
				break;
			case MISSION_VEHICLE:
			case PERMANENT_VEHICLE:
				CTheScripts::StoreVehicleWasRandom = false;
				break;
			default:
				break;
			}
		}
		ScriptParams[0] = CTheScripts::StoreVehicleIndex;
		StoreParameters(&m_nIp, 1);
		return 0;
	}
	case COMMAND_STORE_CAR_PLAYER_IS_IN:
	{
		CollectParameters(&m_nIp, 1);
		CPed* ped = CWorld::Players[ScriptParams[0]].m_pPed;
		assert(ped);
		if (!ped->bInVehicle)
			return 0; // No value written to output variable
		CVehicle* pCurrent = ped->m_pMyVehicle;
		assert(pCurrent); // Here pCurrent shouldn't be NULL anyway
		int handle = CPools::GetVehiclePool()->GetIndex(pCurrent);
		if (handle != CTheScripts::StoreVehicleIndex && m_bIsMissionScript) {
			CVehicle* pOld = CPools::GetVehiclePool()->GetAt(CTheScripts::StoreVehicleIndex);
			if (pOld) {
				CCarCtrl::RemoveFromInterestingVehicleList(pOld);
				if (pOld->VehicleCreatedBy == MISSION_VEHICLE && CTheScripts::StoreVehicleWasRandom) {
					pOld->VehicleCreatedBy = RANDOM_VEHICLE;
					pOld->bIsLocked = false;
					CCarCtrl::NumRandomCars++;
					CCarCtrl::NumMissionCars--;
					CTheScripts::MissionCleanup.RemoveEntityFromList(CTheScripts::StoreVehicleIndex, CLEANUP_CAR);
				}
			}
			CTheScripts::StoreVehicleIndex = handle;
			switch (pCurrent->VehicleCreatedBy) {
			case RANDOM_VEHICLE:
				pCurrent->VehicleCreatedBy = MISSION_VEHICLE;
				CCarCtrl::NumMissionCars++;
				CCarCtrl::NumRandomCars--;
				CTheScripts::StoreVehicleWasRandom = true;
				CTheScripts::MissionCleanup.AddEntityToList(CTheScripts::StoreVehicleIndex, CLEANUP_CAR);
				break;
			case PARKED_VEHICLE:
				pCurrent->VehicleCreatedBy = MISSION_VEHICLE;
				CCarCtrl::NumMissionCars++;
				CCarCtrl::NumParkedCars--;
				CTheScripts::StoreVehicleWasRandom = true;
				CTheScripts::MissionCleanup.AddEntityToList(CTheScripts::StoreVehicleIndex, CLEANUP_CAR);
				break;
			case MISSION_VEHICLE:
			case PERMANENT_VEHICLE:
				CTheScripts::StoreVehicleWasRandom = false;
				break;
			default:
				break;
			}
		}
		ScriptParams[0] = CTheScripts::StoreVehicleIndex;
		StoreParameters(&m_nIp, 1);
		return 0;
	}
	case COMMAND_IS_CHAR_IN_CAR:
	{
		CollectParameters(&m_nIp, 2);
		CPed* pPed = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		CVehicle* pCheckedVehicle = CPools::GetVehiclePool()->GetAt(ScriptParams[1]);
		CVehicle* pActualVehicle = pPed->bInVehicle ? pPed->m_pMyVehicle : nil;
		UpdateCompareFlag(pActualVehicle && pActualVehicle == pCheckedVehicle);
		return 0;
	}
	case COMMAND_IS_PLAYER_IN_CAR:
	{
		CollectParameters(&m_nIp, 2);
		CVehicle* pCheckedVehicle = CPools::GetVehiclePool()->GetAt(ScriptParams[1]);
		CPed* pPed = CWorld::Players[ScriptParams[0]].m_pPed;
		UpdateCompareFlag(pPed->bInVehicle && pPed->m_pMyVehicle == pCheckedVehicle);
		return 0;
	}
	case COMMAND_IS_CHAR_IN_MODEL:
	{
		CollectParameters(&m_nIp, 2);
		CPed* pPed = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		CVehicle* pActualVehicle = pPed->bInVehicle ? pPed->m_pMyVehicle : nil;
		UpdateCompareFlag(pActualVehicle && pActualVehicle->GetModelIndex() == ScriptParams[1]);
		return 0;
	}
	case COMMAND_IS_PLAYER_IN_MODEL:
	{
		CollectParameters(&m_nIp, 2);
		CPed* pPed = CWorld::Players[ScriptParams[0]].m_pPed;
		UpdateCompareFlag(pPed->bInVehicle && pPed->m_pMyVehicle->GetModelIndex() == ScriptParams[1]);
		return 0;
	}
	case COMMAND_IS_CHAR_IN_ANY_CAR:
	{
		CollectParameters(&m_nIp, 1);
		CPed* pPed = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		UpdateCompareFlag(pPed->bInVehicle);
		return 0;
	}
	case COMMAND_IS_PLAYER_IN_ANY_CAR:
	{
		CollectParameters(&m_nIp, 1);
		CPed* pPed = CWorld::Players[ScriptParams[0]].m_pPed;
		UpdateCompareFlag(pPed->bInVehicle);
		return 0;
	}
	case COMMAND_IS_BUTTON_PRESSED:
	{
		CollectParameters(&m_nIp, 2);
		bool value = GetPadState(ScriptParams[0], ScriptParams[1]) != 0;
		if (CGame::playingIntro && ScriptParams[0] == 0 && ScriptParams[1] == 12){ /* pad1, start */
			if (CPad::GetPad(0)->GetLeftMouseJustDown() ||
				CPad::GetPad(0)->GetPadEnterJustDown() ||
				CPad::GetPad(0)->GetCharJustDown(' '))
				value = true;
		}
		UpdateCompareFlag(value);
		return 0;
	}
	case COMMAND_GET_PAD_STATE:
	{
		CollectParameters(&m_nIp, 1);
		ScriptParams[0] = GetPadState(ScriptParams[0], ScriptParams[1]);
		StoreParameters(&m_nIp, 1);
		return 0;
	}
	case COMMAND_LOCATE_PLAYER_ANY_MEANS_2D:
	case COMMAND_LOCATE_PLAYER_ON_FOOT_2D:
	case COMMAND_LOCATE_PLAYER_IN_CAR_2D:
	case COMMAND_LOCATE_STOPPED_PLAYER_ANY_MEANS_2D:
	case COMMAND_LOCATE_STOPPED_PLAYER_ON_FOOT_2D:
	case COMMAND_LOCATE_STOPPED_PLAYER_IN_CAR_2D:
		LocatePlayerCommand(command, &m_nIp);
		return 0;
	case COMMAND_LOCATE_PLAYER_ANY_MEANS_CHAR_2D:
	case COMMAND_LOCATE_PLAYER_ON_FOOT_CHAR_2D:
	case COMMAND_LOCATE_PLAYER_IN_CAR_CHAR_2D:
		LocatePlayerCharCommand(command, &m_nIp);
		return 0;
	case COMMAND_LOCATE_CHAR_ANY_MEANS_2D:
	case COMMAND_LOCATE_CHAR_ON_FOOT_2D:
	case COMMAND_LOCATE_CHAR_IN_CAR_2D:
	case COMMAND_LOCATE_STOPPED_CHAR_ANY_MEANS_2D:
	case COMMAND_LOCATE_STOPPED_CHAR_ON_FOOT_2D:
	case COMMAND_LOCATE_STOPPED_CHAR_IN_CAR_2D:
		LocateCharCommand(command, &m_nIp);
		return 0;
	case COMMAND_LOCATE_CHAR_ANY_MEANS_CHAR_2D:
	case COMMAND_LOCATE_CHAR_ON_FOOT_CHAR_2D:
	case COMMAND_LOCATE_CHAR_IN_CAR_CHAR_2D:
		LocateCharCharCommand(command, &m_nIp);
		return 0;
	case COMMAND_LOCATE_PLAYER_ANY_MEANS_3D:
	case COMMAND_LOCATE_PLAYER_ON_FOOT_3D:
	case COMMAND_LOCATE_PLAYER_IN_CAR_3D:
	case COMMAND_LOCATE_STOPPED_PLAYER_ANY_MEANS_3D:
	case COMMAND_LOCATE_STOPPED_PLAYER_ON_FOOT_3D:
	case COMMAND_LOCATE_STOPPED_PLAYER_IN_CAR_3D:
		LocatePlayerCommand(command, &m_nIp);
		return 0;
	case COMMAND_LOCATE_PLAYER_ANY_MEANS_CHAR_3D:
	case COMMAND_LOCATE_PLAYER_ON_FOOT_CHAR_3D:
	case COMMAND_LOCATE_PLAYER_IN_CAR_CHAR_3D:
		LocatePlayerCharCommand(command, &m_nIp);
		return 0;
	case COMMAND_LOCATE_CHAR_ANY_MEANS_3D:
	case COMMAND_LOCATE_CHAR_ON_FOOT_3D:
	case COMMAND_LOCATE_CHAR_IN_CAR_3D:
	case COMMAND_LOCATE_STOPPED_CHAR_ANY_MEANS_3D:
	case COMMAND_LOCATE_STOPPED_CHAR_ON_FOOT_3D:
	case COMMAND_LOCATE_STOPPED_CHAR_IN_CAR_3D:
		LocateCharCommand(command, &m_nIp);
		return 0;
	case COMMAND_LOCATE_CHAR_ANY_MEANS_CHAR_3D:
	case COMMAND_LOCATE_CHAR_ON_FOOT_CHAR_3D:
	case COMMAND_LOCATE_CHAR_IN_CAR_CHAR_3D:
		LocateCharCharCommand(command, &m_nIp);
		return 0;
	case COMMAND_CREATE_OBJECT:
	{
		CollectParameters(&m_nIp, 4);
		int mi = ScriptParams[0] >= 0 ? ScriptParams[0] : CTheScripts::UsedObjectArray[-ScriptParams[0]].index;
		CObject* pObj = new CObject(mi, 0);
		pObj->ObjectCreatedBy = MISSION_OBJECT;
		CVector pos = *(CVector*)&ScriptParams[1];
		if (pos.z <= -100.0f)
			pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		pos.z += pObj->GetDistanceFromCentreOfMassToBaseOfModel();
		pObj->GetPosition() = pos;
		pObj->SetOrientation(0.0f, 0.0f, 0.0f);
		pObj->GetMatrix().UpdateRW();
		pObj->UpdateRwFrame();
		CTheScripts::ClearSpaceForMissionEntity(pos, pObj);
		CWorld::Add(pObj);
		ScriptParams[0] = CPools::GetObjectPool()->GetIndex(pObj);
		StoreParameters(&m_nIp, 1);
		if (m_bIsMissionScript)
			CTheScripts::MissionCleanup.AddEntityToList(ScriptParams[0], CLEANUP_OBJECT);
		return 0;
	}
	case COMMAND_DELETE_OBJECT:
	{
		CollectParameters(&m_nIp, 1);
		CObject* pObj = CPools::GetObjectPool()->GetAt(ScriptParams[0]);
		if (pObj){
			CWorld::Remove(pObj);
			CWorld::RemoveReferencesToDeletedObject(pObj);
			delete pObj;
		}
		if (m_bIsMissionScript)
			CTheScripts::MissionCleanup.RemoveEntityFromList(ScriptParams[0], CLEANUP_OBJECT);
		return 0;
	}
	case COMMAND_ADD_SCORE:
		CollectParameters(&m_nIp, 2);
		CWorld::Players[ScriptParams[0]].m_nMoney += ScriptParams[1];
		return 0;
	case COMMAND_IS_SCORE_GREATER:
		CollectParameters(&m_nIp, 2);
		UpdateCompareFlag(CWorld::Players[ScriptParams[0]].m_nMoney > ScriptParams[1]);
		return 0;
	case COMMAND_STORE_SCORE:
		CollectParameters(&m_nIp, 1);
		ScriptParams[0] = CWorld::Players[ScriptParams[0]].m_nMoney;
		StoreParameters(&m_nIp, 1);
		return 0;
	case COMMAND_GIVE_REMOTE_CONTROLLED_CAR_TO_PLAYER:
	{
		CollectParameters(&m_nIp, 5);
		CVector pos = *(CVector*)&ScriptParams[1];
		if (pos.z <= -100.0f)
			pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		CRemote::GivePlayerRemoteControlledCar(pos.x, pos.y, pos.z, DEGTORAD(*(float*)&ScriptParams[4]), MI_RCBANDIT);
		return 0;
	}
	case COMMAND_ALTER_WANTED_LEVEL:
		CollectParameters(&m_nIp, 2);
		CWorld::Players[ScriptParams[0]].m_pPed->SetWantedLevel(ScriptParams[1]);
		return 0;
	case COMMAND_ALTER_WANTED_LEVEL_NO_DROP:
		CollectParameters(&m_nIp, 2);
		CWorld::Players[ScriptParams[0]].m_pPed->SetWantedLevelNoDrop(ScriptParams[1]);
		return 0;
	case COMMAND_IS_WANTED_LEVEL_GREATER:
		CollectParameters(&m_nIp, 2);
		UpdateCompareFlag(CWorld::Players[ScriptParams[0]].m_pPed->m_pWanted->m_nWantedLevel > ScriptParams[1]);
		return 0;
	case COMMAND_CLEAR_WANTED_LEVEL:
		CollectParameters(&m_nIp, 1);
		CWorld::Players[ScriptParams[0]].m_pPed->SetWantedLevel(0);
		return 0;
	case COMMAND_SET_DEATHARREST_STATE:
		CollectParameters(&m_nIp, 1);
		m_bDeatharrestEnabled = (ScriptParams[0] == 1);
		return 0;
	case COMMAND_HAS_DEATHARREST_BEEN_EXECUTED:
		UpdateCompareFlag(m_bDeatharrestExecuted);
		return 0;
	case COMMAND_ADD_AMMO_TO_PLAYER:
	{
		CollectParameters(&m_nIp, 3);
		CWorld::Players[ScriptParams[0]].m_pPed->GrantAmmo((eWeaponType)ScriptParams[1], ScriptParams[2]);
		return 0;
	}
	case COMMAND_ADD_AMMO_TO_CHAR:
	{
		CollectParameters(&m_nIp, 3);
		CPed* pPed = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(pPed);
		pPed->GrantAmmo((eWeaponType)ScriptParams[1], ScriptParams[2]);
		return 0;
	}
	/* Not implemented
	case COMMAND_ADD_AMMO_TO_CAR:
	case COMMAND_IS_PLAYER_STILL_ALIVE:
	*/
	case COMMAND_IS_PLAYER_DEAD:
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(CWorld::Players[ScriptParams[0]].m_WBState == WBSTATE_WASTED);
		return 0;
	case COMMAND_IS_CHAR_DEAD:
	{
		CollectParameters(&m_nIp, 1);
		CPed* pPed = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		UpdateCompareFlag(!pPed || pPed->GetPedState() == PED_DIE || pPed->GetPedState() == PED_DEAD);
		return 0;
	}
	case COMMAND_IS_CAR_DEAD:
	{
		CollectParameters(&m_nIp, 1);
		CVehicle* pVehicle = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		UpdateCompareFlag(!pVehicle || pVehicle->m_status == STATUS_WRECKED || !pVehicle->IsBoat() && pVehicle->bIsInWater);
		return 0;
	}
	case COMMAND_SET_CHAR_THREAT_SEARCH:
	{
		CollectParameters(&m_nIp, 2);
		CPed* pPed = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(pPed);
		pPed->m_fearFlags |= ScriptParams[1];
		return 0;
	}
	/* Not implemented. 
	case COMMAND_SET_CHAR_THREAT_REACTION:
	*/
	case COMMAND_SET_CHAR_OBJ_NO_OBJ:
	{
		CollectParameters(&m_nIp, 1);
		CPed* pPed = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(pPed);
		pPed->bScriptObjectiveCompleted = false;
		pPed->ClearObjective();
		return 0;
	}
	/* Not implemented. 
	case COMMAND_ORDER_DRIVER_OUT_OF_CAR:
	case COMMAND_ORDER_CHAR_TO_DRIVE_CAR:
	case COMMAND_ADD_PATROL_POINT:
	case COMMAND_IS_PLAYER_IN_GANGZONE:
	*/
	case COMMAND_IS_PLAYER_IN_ZONE:
	{
		CollectParameters(&m_nIp, 1);
		CPlayerInfo* pPlayer = &CWorld::Players[ScriptParams[0]];
		char label[12];
		CTheScripts::ReadTextLabelFromScript(&m_nIp, label);
		int zoneToCheck = CTheZones::FindZoneByLabelAndReturnIndex(label);
		if (zoneToCheck != -1)
			m_nIp += 8; /* why only if zone != 1? */
		CVector pos = pPlayer->GetPos();
		CZone* pZone = CTheZones::GetZone(zoneToCheck);
		UpdateCompareFlag(CTheZones::PointLiesWithinZone(pos, pZone));
		return 0;
	}
	case COMMAND_IS_PLAYER_PRESSING_HORN:
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(CWorld::Players[ScriptParams[0]].m_pPed->GetPedState() == PED_DRIVING &&
			CPad::GetPad(ScriptParams[0])->GetHorn());
		/* Is it correct that same parameter is used both as index of Players */
		/* and as ID of pad? Pratically this parameter is always 0 anyway of course. */
		return 0;
	case COMMAND_HAS_CHAR_SPOTTED_PLAYER:
	{
		CollectParameters(&m_nIp, 2);
		CPed* pPed = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(pPed);
		UpdateCompareFlag(pPed->OurPedCanSeeThisOne(CWorld::Players[ScriptParams[1]].m_pPed));
		return 0;
	}
	/* Not implemented.
	case COMMAND_ORDER_CHAR_TO_BACKDOOR:
	case COMMAND_ADD_CHAR_TO_GANG:
	*/
	case COMMAND_IS_CHAR_OBJECTIVE_PASSED:
	{
		CollectParameters(&m_nIp, 1);
		CPed* pPed = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(pPed);
		UpdateCompareFlag(pPed->bScriptObjectiveCompleted);
		return 0;
	}
	/* Not implemented. 
	case COMMAND_SET_CHAR_DRIVE_AGGRESSION:
	case COMMAND_SET_CHAR_MAX_DRIVESPEED:
	*/
	case COMMAND_CREATE_CHAR_INSIDE_CAR:
	{
		CollectParameters(&m_nIp, 3);
		CVehicle* pVehicle = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(pVehicle);
		switch (ScriptParams[2]) {
		case MI_COP:
			if (ScriptParams[1] == PEDTYPE_COP)
				ScriptParams[2] = COP_STREET;
			break;
		case MI_SWAT:
			if (ScriptParams[1] == PEDTYPE_COP)
				ScriptParams[2] = COP_SWAT;
			break;
		case MI_FBI:
			if (ScriptParams[1] == PEDTYPE_COP)
				ScriptParams[2] = COP_FBI;
			break;
		case MI_ARMY:
			if (ScriptParams[1] == PEDTYPE_COP)
				ScriptParams[2] = COP_ARMY;
			break;
		case MI_MEDIC:
			if (ScriptParams[1] == PEDTYPE_EMERGENCY)
				ScriptParams[2] = PEDTYPE_EMERGENCY;
			break;
		case MI_FIREMAN:
			if (ScriptParams[1] == PEDTYPE_FIREMAN)
				ScriptParams[2] = PEDTYPE_FIREMAN;
			break;
		default:
			break;
		}
		CPed* pPed;
		if (ScriptParams[1] == PEDTYPE_COP)
			pPed = new CCopPed((eCopType)ScriptParams[2]);
		else if (ScriptParams[1] == PEDTYPE_EMERGENCY || ScriptParams[1] == PEDTYPE_FIREMAN)
			pPed = new CEmergencyPed(ScriptParams[2]);
		else
			pPed = new CCivilianPed(ScriptParams[1], ScriptParams[2]);
		pPed->CharCreatedBy = MISSION_CHAR;
		pPed->bRespondsToThreats = false;
		pPed->m_ped_flagG2 = false;
		pPed->GetPosition() = pVehicle->GetPosition();
		pPed->SetOrientation(0.0f, 0.0f, 0.0f);
		pPed->SetPedState(PED_DRIVING);
		CPopulation::ms_nTotalMissionPeds++;
		assert(!pVehicle->pDriver);
		pVehicle->pDriver = pPed;
		pVehicle->pDriver->RegisterReference((CEntity**)&pVehicle->pDriver);
		pPed->m_pMyVehicle = pVehicle;
		pPed->m_pMyVehicle->RegisterReference((CEntity**)&pPed->m_pMyVehicle);
		pPed->bInVehicle = true;
		pVehicle->m_status = STATUS_PHYSICS;
		if (!pVehicle->IsBoat())
			pVehicle->AutoPilot.m_nCarMission = MISSION_CRUISE;
		pVehicle->bEngineOn = true;
		pPed->bUsesCollision = false;
		AnimationId anim = pVehicle->bLowVehicle ? ANIM_CAR_LSIT : ANIM_CAR_SIT;
		pPed->m_pVehicleAnim = CAnimManager::BlendAnimation(pPed->GetClump(), ASSOCGRP_STD, anim, 100.0f);
		pPed->StopNonPartialAnims();
		pPed->m_level = CTheZones::GetLevelFromPosition(pPed->GetPosition());
		CWorld::Add(pPed);
		ScriptParams[0] = CPools::GetPedPool()->GetIndex(pPed);
		StoreParameters(&m_nIp, 1);
		if (m_bIsMissionScript)
			CTheScripts::MissionCleanup.AddEntityToList(ScriptParams[0], CLEANUP_CHAR);
		return 0;
	}
	case COMMAND_WARP_PLAYER_FROM_CAR_TO_COORD:
	{
		CollectParameters(&m_nIp, 4);
		CVector pos = *(CVector*)&ScriptParams[1];
		CPlayerInfo* pPlayer = &CWorld::Players[ScriptParams[0]];
		if (pos.z <= -100.0f)
			pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
		if (pPlayer->m_pPed->bInVehicle){
			assert(pPlayer->m_pPed->m_pMyVehicle);
			if (pPlayer->m_pPed->m_pMyVehicle->bIsBus)
				pPlayer->m_pPed->bRenderPedInCar = true;
			if (pPlayer->m_pPed->m_pMyVehicle->pDriver == pPlayer->m_pPed){
				pPlayer->m_pPed->m_pMyVehicle->RemoveDriver();
				pPlayer->m_pPed->m_pMyVehicle->m_status = STATUS_ABANDONED;
				pPlayer->m_pPed->m_pMyVehicle->bEngineOn = false;
				pPlayer->m_pPed->m_pMyVehicle->AutoPilot.m_nCruiseSpeed = 0;
			}else{
				pPlayer->m_pPed->m_pMyVehicle->RemovePassenger(pPlayer->m_pPed);
			}
		}
		pPlayer->m_pPed->bInVehicle = false;
		pPlayer->m_pPed->m_pMyVehicle = nil;
		pPlayer->m_pPed->SetPedState(PED_IDLE);
		pPlayer->m_pPed->bUsesCollision = true;
		pPlayer->m_pPed->SetMoveSpeed(0.0f, 0.0f, 0.0f);
		pPlayer->m_pPed->AddWeaponModel(CWeaponInfo::GetWeaponInfo(pPlayer->m_pPed->GetWeapon()->m_eWeaponType)->m_nModelId);
		pPlayer->m_pPed->RemoveInCarAnims();
		if (pPlayer->m_pPed->m_pVehicleAnim)
			pPlayer->m_pPed->m_pVehicleAnim->blendDelta = -1000.0f;
		pPlayer->m_pPed->m_pVehicleAnim = nil;
		pPlayer->m_pPed->SetMoveState(PEDMOVE_NONE);
		CAnimManager::BlendAnimation(pPlayer->m_pPed->GetClump(), pPlayer->m_pPed->m_animGroup, ANIM_IDLE_STANCE, 100.0f);
		pPlayer->m_pPed->RestartNonPartialAnims();
		AudioManager.PlayerJustLeftCar();
		pos.z += pPlayer->m_pPed->GetDistanceFromCentreOfMassToBaseOfModel();
		pPlayer->m_pPed->Teleport(pos);
		CTheScripts::ClearSpaceForMissionEntity(pos, pPlayer->m_pPed);
		return 0;
	}
	/* Not implemented. 
	case COMMAND_MAKE_CHAR_DO_NOTHING:
	*/
	default:
		assert(0);
		break;
	}
	return -1;
}

#if 1
WRAPPER int8 CRunningScript::ProcessCommandsFrom300To399(int32 command) { EAXJMP(0x43ED30); }
#else
int8 CRunningScript::ProcessCommandsFrom300To399(int32 command)
{
	switch (command) {
	/* Not implemented.
	case COMMAND_SET_CHAR_INVINCIBLE:
	case COMMAND_SET_PLAYER_INVINCIBLE:
	case COMMAND_SET_CHAR_GRAPHIC_TYPE:
	case COMMAND_SET_PLAYER_GRAPHIC_TYPE:
	*/
	case COMMAND_HAS_PLAYER_BEEN_ARRESTED:
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(CWorld::Players[ScriptParams[0]].m_WBState == WBSTATE_BUSTED);
		return 0;
	/* Not implemented.
	case COMMAND_STOP_CHAR_DRIVING:
	case COMMAND_KILL_CHAR:
	case COMMAND_SET_FAVOURITE_CAR_MODEL_FOR_CHAR:
	case COMMAND_SET_CHAR_OCCUPATION:
	*/
	case COMMAND_CHANGE_CAR_LOCK:
	{
		CollectParameters(&m_nIp, 2);
		CVehicle* pVehicle = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(pVehicle);
		pVehicle->m_nDoorLock = (eCarLock)ScriptParams[1];
		return 0;
	}
	case COMMAND_SHAKE_CAM_WITH_POINT:
		CollectParameters(&m_nIp, 4);
		TheCamera.CamShake(ScriptParams[0] / 1000.0f,
			*(float*)&ScriptParams[1],
			*(float*)&ScriptParams[2],
			*(float*)&ScriptParams[3]);
		return 0;
	case COMMAND_IS_CAR_MODEL:
	{
		CollectParameters(&m_nIp, 2);
		CVehicle* pVehicle = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(pVehicle);
		UpdateCompareFlag(pVehicle->GetModelIndex() == ScriptParams[1]);
	}
	/* Not implemented.
	case COMMAND_IS_CAR_REMAP:
	case COMMAND_HAS_CAR_JUST_SUNK:
	case COMMAND_SET_CAR_NO_COLLIDE:
	*/
	case COMMAND_IS_CAR_DEAD_IN_AREA_2D:
	{
		CollectParameters(&m_nIp, 6);
		CVehicle* pVehicle = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(pVehicle);
		float x1, y1, x2, y2;
		x1 = *(float*)&ScriptParams[1];
		y1 = *(float*)&ScriptParams[2];
		x2 = *(float*)&ScriptParams[3];
		y2 = *(float*)&ScriptParams[4];
		UpdateCompareFlag(pVehicle->m_status == STATUS_WRECKED &&
			pVehicle->IsWithinArea(x1, y1, x2, y2));
		if (!ScriptParams[5])
			return 0;
		CTheScripts::HighlightImportantArea((uint32)this + m_nIp, x1, y1, x2, y2, -100.0f);
		if (CTheScripts::DbgFlag)
			CTheScripts::DrawDebugSquare(x1, y1, x2, y2);
		return 0;
	}
	case COMMAND_IS_CAR_DEAD_IN_AREA_3D:
	{
		CollectParameters(&m_nIp, 8);
		CVehicle* pVehicle = CPools::GetVehiclePool()->GetAt(ScriptParams[0]);
		assert(pVehicle);
		float x1, y1, z1, x2, y2, z2;
		x1 = *(float*)&ScriptParams[1];
		y1 = *(float*)&ScriptParams[2];
		z1 = *(float*)&ScriptParams[3];
		x2 = *(float*)&ScriptParams[4];
		y2 = *(float*)&ScriptParams[5];
		z2 = *(float*)&ScriptParams[6];
		UpdateCompareFlag(pVehicle->m_status == STATUS_WRECKED &&
			pVehicle->IsWithinArea(x1, y1, z1, x2, y2, z2));
		if (!ScriptParams[7])
			return 0;
		CTheScripts::HighlightImportantArea((uint32)this + m_nIp, x1, y1, x2, y2, (z1 + z2) / 2);
		if (CTheScripts::DbgFlag)
			CTheScripts::DrawDebugCube(x1, y1, z1, x2, y2, z2);
		return 0;
	}
	/* Not implemented.
	case COMMAND_IS_TRAILER_ATTACHED:
	case COMMAND_IS_CAR_ON_TRAILER:
	case COMMAND_HAS_CAR_GOT_WEAPON:
	case COMMAND_PARK:
	case COMMAND_HAS_PARK_FINISHED:
	case COMMAND_KILL_ALL_PASSENGERS:
	case COMMAND_SET_CAR_BULLETPROOF:
	case COMMAND_SET_CAR_FLAMEPROOF:
	case COMMAND_SET_CAR_ROCKETPROOF:
	case COMMAND_IS_CARBOMB_ACTIVE:
	case COMMAND_GIVE_CAR_ALARM:
	case COMMAND_PUT_CAR_ON_TRAILER:
	*/
	case COMMAND_IS_CAR_CRUSHED:
		CollectParameters(&m_nIp, 1);
		UpdateCompareFlag(CGarages::HasCarBeenCrushed(ScriptParams[0]));
		return 0;
	/* Not implemented.
	case COMMAND_CREATE_GANG_CAR:
	*/
	case COMMAND_CREATE_CAR_GENERATOR:
		CollectParameters(&m_nIp, 12);
		ScriptParams[0] = CTheCarGenerators::CreateCarGenerator(
			*(float*)&ScriptParams[0], *(float*)&ScriptParams[1], *(float*)&ScriptParams[2], *(float*)&ScriptParams[3],
			ScriptParams[4], ScriptParams[5], ScriptParams[6], ScriptParams[7],
			ScriptParams[8], ScriptParams[9], ScriptParams[10], ScriptParams[11]);
		StoreParameters(&m_nIp, 1);
		return 0;
	case COMMAND_SWITCH_CAR_GENERATOR:
	{
		CollectParameters(&m_nIp, 2);
		CCarGenerator* pCarGen = &CTheCarGenerators::CarGeneratorArray[ScriptParams[0]];
		if (ScriptParams[1] == 0){
			pCarGen->SwitchOff();
		}else if (ScriptParams[1] <= 100){
			pCarGen->SwitchOn();
		}else{
			pCarGen->SwitchOn();
			pCarGen->SetUsesRemaining(ScriptParams[1]);
		}
		return 0;
	}
	case COMMAND_ADD_PAGER_MESSAGE:
	{
		wchar* text = CTheScripts::GetTextByKeyFromScript(&m_nIp);
		CollectParameters(&m_nIp, 3);
		CUserDisplay::Pager.AddMessage(text, ScriptParams[0], ScriptParams[1], ScriptParams[2]);
		return 0;
	}
	case COMMAND_DISPLAY_ONSCREEN_TIMER:
	{
		assert(CTheScripts::ScriptSpace[m_nIp] == ARGUMENT_GLOBALVAR);
		m_nIp++;
		CUserDisplay::OnscnTimer.AddClock(CTheScripts::Read2BytesFromScript(&m_nIp), nil);
		return 0;
	}
	case COMMAND_CLEAR_ONSCREEN_TIMER:
	{
		assert(CTheScripts::ScriptSpace[m_nIp] == ARGUMENT_GLOBALVAR);
		m_nIp++;
		CUserDisplay::OnscnTimer.ClearClock(CTheScripts::Read2BytesFromScript(&m_nIp));
		return 0;
	}
	case COMMAND_DISPLAY_ONSCREEN_COUNTER:
	{
		assert(CTheScripts::ScriptSpace[m_nIp] == ARGUMENT_GLOBALVAR);
		m_nIp++;
		int32 counter = CTheScripts::Read2BytesFromScript(&m_nIp);
		CollectParameters(&m_nIp, 1);
		CUserDisplay::OnscnTimer.AddCounter(counter, ScriptParams[0], nil);
		return 0;
	}
	case COMMAND_CLEAR_ONSCREEN_COUNTER:
	{
		assert(CTheScripts::ScriptSpace[m_nIp] == ARGUMENT_GLOBALVAR);
		m_nIp++;
		CUserDisplay::OnscnTimer.ClearCounter(CTheScripts::Read2BytesFromScript(&m_nIp));
		return 0;
	}
	case COMMAND_SET_ZONE_CAR_INFO:
	{
		char label[12];
		CTheScripts::ReadTextLabelFromScript(&m_nIp, label);
		m_nIp += 8;
		CollectParameters(&m_nIp, 16);
		int zone = CTheZones::FindZoneByLabelAndReturnIndex(label);
		if (zone < 0) {
			debug("Couldn't find zone - %s\n", label);
			return 0;
		}
		CTheZones::SetZoneCarInfo(zone, ScriptParams[0], ScriptParams[1], ScriptParams[2], ScriptParams[3],
			ScriptParams[4], ScriptParams[5], ScriptParams[6], ScriptParams[7], ScriptParams[8], 0, 0,
			ScriptParams[9], ScriptParams[10], ScriptParams[11], ScriptParams[12],
			ScriptParams[13], ScriptParams[14], ScriptParams[15]);
		return 0;
	}
	/* Not implemented.
	case COMMAND_IS_CHAR_IN_GANG_ZONE:
	*/
	case COMMAND_IS_CHAR_IN_ZONE:
	{
		CollectParameters(&m_nIp, 1);
		CPed* pPed = CPools::GetPedPool()->GetAt(ScriptParams[0]);
		assert(pPed);
		char label[12];
		CTheScripts::ReadTextLabelFromScript(&m_nIp, label);
		int zone = CTheZones::FindZoneByLabelAndReturnIndex(label);
		if (zone != -1)
			m_nIp += 8;
		CVector pos = pPed->bInVehicle ? pPed->m_pMyVehicle->GetPosition() : pPed->GetPosition();
		UpdateCompareFlag(CTheZones::PointLiesWithinZone(pos, CTheZones::GetZone(zone)));
		return 0;
	}
	case COMMAND_SET_CAR_DENSITY:
	{
		char label[12];
		CTheScripts::ReadTextLabelFromScript(&m_nIp, label);
		m_nIp += 8;

	}
	case COMMAND_SET_PED_DENSITY:
	case COMMAND_POINT_CAMERA_AT_PLAYER:
	case COMMAND_POINT_CAMERA_AT_CAR:
	case COMMAND_POINT_CAMERA_AT_CHAR:
	case COMMAND_RESTORE_CAMERA:
	case COMMAND_SHAKE_PAD:
	case COMMAND_SET_ZONE_PED_INFO:
	case COMMAND_SET_TIME_SCALE:
	case COMMAND_IS_CAR_IN_AIR:
	case COMMAND_SET_FIXED_CAMERA_POSITION:
	case COMMAND_POINT_CAMERA_AT_POINT:
	case COMMAND_ADD_BLIP_FOR_CAR_OLD:
	case COMMAND_ADD_BLIP_FOR_CHAR_OLD:
	case COMMAND_ADD_BLIP_FOR_OBJECT_OLD:
	case COMMAND_REMOVE_BLIP:
	case COMMAND_CHANGE_BLIP_COLOUR:
	case COMMAND_DIM_BLIP:
	case COMMAND_ADD_BLIP_FOR_COORD_OLD:
	case COMMAND_CHANGE_BLIP_SCALE:
	case COMMAND_SET_FADING_COLOUR:
	case COMMAND_DO_FADE:
	case COMMAND_GET_FADING_STATUS:
	case COMMAND_ADD_HOSPITAL_RESTART:
	case COMMAND_ADD_POLICE_RESTART:
	case COMMAND_OVERRIDE_NEXT_RESTART:
	case COMMAND_DRAW_SHADOW:
	case COMMAND_GET_PLAYER_HEADING:
	case COMMAND_SET_PLAYER_HEADING:
	case COMMAND_GET_CHAR_HEADING:
	case COMMAND_SET_CHAR_HEADING:
	case COMMAND_GET_CAR_HEADING:
	case COMMAND_SET_CAR_HEADING:
	case COMMAND_GET_OBJECT_HEADING:
	case COMMAND_SET_OBJECT_HEADING:
	case COMMAND_IS_PLAYER_TOUCHING_OBJECT:
	case COMMAND_IS_CHAR_TOUCHING_OBJECT:
	case COMMAND_SET_PLAYER_AMMO:
	case COMMAND_SET_CHAR_AMMO:
	case COMMAND_SET_CAR_AMMO:
	case COMMAND_LOAD_CAMERA_SPLINE:
	case COMMAND_MOVE_CAMERA_ALONG_SPLINE:
	case COMMAND_GET_CAMERA_POSITION_ALONG_SPLINE:
	case COMMAND_DECLARE_MISSION_FLAG:
	case COMMAND_DECLARE_MISSION_FLAG_FOR_CONTACT:
	case COMMAND_DECLARE_BASE_BRIEF_ID_FOR_CONTACT:
	case COMMAND_IS_PLAYER_HEALTH_GREATER:
	case COMMAND_IS_CHAR_HEALTH_GREATER:
	case COMMAND_IS_CAR_HEALTH_GREATER:
	case COMMAND_ADD_BLIP_FOR_CAR:
	case COMMAND_ADD_BLIP_FOR_CHAR:
	case COMMAND_ADD_BLIP_FOR_OBJECT:
	case COMMAND_ADD_BLIP_FOR_CONTACT_POINT:
	case COMMAND_ADD_BLIP_FOR_COORD:
	case COMMAND_CHANGE_BLIP_DISPLAY:
	case COMMAND_ADD_ONE_OFF_SOUND:
	case COMMAND_ADD_CONTINUOUS_SOUND:
	case COMMAND_REMOVE_SOUND:
	case COMMAND_IS_CAR_STUCK_ON_ROOF:
	default:
		assert(0);
	}
	return -1;
}
#endif

int16 CRunningScript::GetPadState(uint16 pad, uint16 button)
{
	CPad* pPad = CPad::GetPad(pad);
	switch (button){
	case 0: return pPad->NewState.LeftStickX;
	case 1: return pPad->NewState.LeftStickY;
	case 2: return pPad->NewState.RightStickX;
	case 3: return pPad->NewState.RightStickY;
	case 4: return pPad->NewState.LeftShoulder1;
	case 5: return pPad->NewState.LeftShoulder2;
	case 6: return pPad->NewState.RightShoulder1;
	case 7: return pPad->NewState.RightShoulder2;
	case 8: return pPad->NewState.DPadUp;
	case 9: return pPad->NewState.DPadDown;
	case 10: return pPad->NewState.DPadLeft;
	case 11: return pPad->NewState.DPadRight;
	case 12: return pPad->NewState.Start;
	case 13: return pPad->NewState.Select;
	case 14: return pPad->NewState.Square;
	case 15: return pPad->NewState.Triangle;
	case 16: return pPad->NewState.Cross;
	case 17: return pPad->NewState.Circle;
	case 18: return pPad->NewState.LeftShock;
	case 19: return pPad->NewState.RightShock;
	default: break;
	}
	return 0;
}

WRAPPER int8 CRunningScript::ProcessCommandsFrom400To499(int32 command) { EAXJMP(0x440CB0); }
WRAPPER int8 CRunningScript::ProcessCommandsFrom500To599(int32 command) { EAXJMP(0x4429C0); }
WRAPPER int8 CRunningScript::ProcessCommandsFrom600To699(int32 command) { EAXJMP(0x444B20); }
WRAPPER int8 CRunningScript::ProcessCommandsFrom700To799(int32 command) { EAXJMP(0x4458A0); }
WRAPPER int8 CRunningScript::ProcessCommandsFrom800To899(int32 command) { EAXJMP(0x448240); }
WRAPPER int8 CRunningScript::ProcessCommandsFrom900To999(int32 command) { EAXJMP(0x44CB80); }
WRAPPER int8 CRunningScript::ProcessCommandsFrom1000To1099(int32 command) { EAXJMP(0x588490); }
WRAPPER int8 CRunningScript::ProcessCommandsFrom1100To1199(int32 command) { EAXJMP(0x589D00); }

WRAPPER void CRunningScript::LocatePlayerCommand(int32, uint32*) { EAXJMP(0x44FE10); }
WRAPPER void CRunningScript::LocatePlayerCharCommand(int32, uint32*) { EAXJMP(0x4501E0); }
WRAPPER void CRunningScript::LocatePlayerCarCommand(int32, uint32*) { EAXJMP(0x450540); }
WRAPPER void CRunningScript::LocateCharCommand(int32, uint32*) { EAXJMP(0x450870); }
WRAPPER void CRunningScript::LocateCharCharCommand(int32, uint32*) { EAXJMP(0x450BF0); }
WRAPPER void CRunningScript::LocateCharCarCommand(int32, uint32*) { EAXJMP(0x450F30); }
WRAPPER void CRunningScript::LocateCharObjectCommand(int32, uint32*) { EAXJMP(0x451260); }
WRAPPER void CRunningScript::LocateCarCommand(int32, uint32*) { EAXJMP(0x451590); }
WRAPPER void CRunningScript::LocateSniperBulletCommand(int32, uint32*) { EAXJMP(0x4518A0); }
WRAPPER void CRunningScript::LocatePlayerInAreaCheckCommand(int32, uint32*) { EAXJMP(0x451A60); }
WRAPPER void CRunningScript::LocatePlayerInAngledAreaCheckCommand(int32, uint32*) { EAXJMP(0x451E50); }
WRAPPER void CRunningScript::LocateCharInAreaCheckCommand(int32, uint32*) { EAXJMP(0x4523B0); }
WRAPPER void CRunningScript::LocateCharInAngledAreaCheckCommand(int32, uint32*) { EAXJMP(0x452750); }

WRAPPER void CTheScripts::DrawScriptSpheres() { EAXJMP(0x44FAC0); }
WRAPPER void CRunningScript::DoDeatharrestCheck() { EAXJMP(0x452A30); }
WRAPPER void CTheScripts::DrawDebugSquare(float, float, float, float) { EAXJMP(0x452D00); }
WRAPPER void CTheScripts::DrawDebugCube(float, float, float, float, float, float) { EAXJMP(0x453100); }
WRAPPER void CTheScripts::ScriptDebugLine3D(float x1, float y1, float z1, float x2, float y2, float z2, int col, int col2) { EAXJMP(0x4534E0); }
WRAPPER void CTheScripts::ClearSpaceForMissionEntity(const CVector&, CEntity*) { EAXJMP(0x454060); }
WRAPPER void CTheScripts::HighlightImportantArea(uint32, float, float, float, float, float) { EAXJMP(0x454320); }
WRAPPER void CTheScripts::CleanUpThisVehicle(CVehicle*) { EAXJMP(0x4548D0); }
WRAPPER void CTheScripts::CleanUpThisPed(CPed*) { EAXJMP(0x4547A0); }
WRAPPER void CTheScripts::CleanUpThisObject(CObject*) { EAXJMP(0x454910); }
WRAPPER void CTheScripts::ReadObjectNamesFromScript() { EAXJMP(0x454960); }
WRAPPER void CTheScripts::UpdateObjectIndices() { EAXJMP(0x454AD0); }
WRAPPER void CTheScripts::ReadMultiScriptFileOffsetsFromScript() { EAXJMP(0x454BC0); }

STARTPATCHES
InjectHook(0x437AE0, &CMissionCleanup::Init, PATCH_JUMP);
InjectHook(0x437BA0, &CMissionCleanup::AddEntityToList, PATCH_JUMP);
InjectHook(0x437BD0, &CMissionCleanup::RemoveEntityFromList, PATCH_JUMP);
InjectHook(0x437C10, &CMissionCleanup::Process, PATCH_JUMP);
InjectHook(0x437DC0, &CUpsideDownCarCheck::Init, PATCH_JUMP);
InjectHook(0x437EE0, &CUpsideDownCarCheck::UpdateTimers, PATCH_JUMP);
InjectHook(0x437F80, &CUpsideDownCarCheck::AreAnyCarsUpsideDown, PATCH_JUMP);
InjectHook(0x437FB0, &CUpsideDownCarCheck::AddCarToCheck, PATCH_JUMP);
InjectHook(0x437FE0, &CUpsideDownCarCheck::RemoveCarFromCheck, PATCH_JUMP);
InjectHook(0x438010, &CUpsideDownCarCheck::HasCarBeenUpsideDownForAWhile, PATCH_JUMP);
InjectHook(0x438050, &CStuckCarCheck::Init, PATCH_JUMP);
InjectHook(0x4380A0, &CStuckCarCheck::Process, PATCH_JUMP);
InjectHook(0x4381C0, &CStuckCarCheck::AddCarToCheck, PATCH_JUMP);
InjectHook(0x438240, &CStuckCarCheck::RemoveCarFromCheck, PATCH_JUMP);
InjectHook(0x4382A0, &CStuckCarCheck::HasCarBeenStuckForAWhile, PATCH_JUMP);
InjectHook(0x4382E0, &CRunningScript::CollectParameters, PATCH_JUMP);
InjectHook(0x438460, &CRunningScript::CollectNextParameterWithoutIncreasingPC, PATCH_JUMP);
InjectHook(0x4385A0, &CRunningScript::StoreParameters, PATCH_JUMP);
InjectHook(0x438640, &CRunningScript::GetPointerToScriptVariable, PATCH_JUMP);
InjectHook(0x438790, &CTheScripts::Init, PATCH_JUMP);
InjectHook(0x439000, &CTheScripts::StartNewScript, PATCH_JUMP);
InjectHook(0x439040, &CTheScripts::Process, PATCH_JUMP);
InjectHook(0x439400, &CTheScripts::StartTestScript, PATCH_JUMP);
InjectHook(0x439410, &CTheScripts::IsPlayerOnAMission, PATCH_JUMP);
ENDPATCHES
