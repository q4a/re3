#include "common.h"
#include "patcher.h"
#include "Stats.h"

WRAPPER void CStats::SaveStats(uint8 *buf, uint32 *size) { EAXJMP(0x4ab3e0); }

int32 &CStats::DaysPassed = *(int32*)0x8F2BB8;
int32 &CStats::HeadsPopped = *(int32*)0x8F647C;
bool& CStats::CommercialPassed = *(bool*)0x8F4334;
bool& CStats::IndustrialPassed = *(bool*)0x8E2A68;
bool& CStats::SuburbanPassed = *(bool*)0x8F2740;
int32 &CStats::NumberKillFrenziesPassed = *(int32*)0x8E287C;
int32 &CStats::PeopleKilledByOthers = *(int32*)0x8E2C50;
int32 &CStats::HelisDestroyed = *(int32*)0x8E2A64;
int32(&CStats::PedsKilledOfThisType)[NUM_PEDTYPES] = *(int32(*)[NUM_PEDTYPES]) * (uintptr*)0x880DBC;
int32 &CStats::TimesDied = *(int32*)0x8E2BDC;
int32 &CStats::TimesArrested = *(int32*)0x8E2BEC;
int32 &CStats::KillsSinceLastCheckpoint = *(int32*)0x8F2C8C;
int32& CStats::DistanceTravelledInVehicle = *(int32*)0x940574;
int32& CStats::DistanceTravelledOnFoot = *(int32*)0x941518;
int32 &CStats::ProgressMade = *(int32*)0x8F6224;
int32 &CStats::TotalProgressInGame = *(int32*)0x885B2C;
int32& CStats::CarsExploded = *(int32*)0x941288;
int32& CStats::PeopleKilledByPlayer = *(int32*)0x8F1B7C;
float &CStats::MaximumJumpDistance = *(float*)0x8F2BDC;
float &CStats::MaximumJumpHeight = *(float*)0x940564;
int32 &CStats::MaximumJumpFlips = *(int32*)0x8F2524;
int32 &CStats::MaximumJumpSpins = *(int32*)0x8F29B0;
int32 &CStats::BestStuntJump = *(int32*)0x885B50;
int32 &CStats::NumberOfUniqueJumpsFound = *(int32*)0x885B74;
int32 &CStats::TotalNumberOfUniqueJumps = *(int32*)0x8E2DC0;
int32 &CStats::PassengersDroppedOffWithTaxi = *(int32*)0x940724;
int32 &CStats::MoneyMadeWithTaxi = *(int32*)0x941544;
int32 &CStats::MissionsGiven = *(int32*)0x9430E8;
int32 &CStats::MissionsPassed = *(int32*)0x940768;
char(&CStats::LastMissionPassedName)[8] = *(char(*)[8])*(uintptr*)0x70D828;
int32 &CStats::TotalLegitimateKills = *(int32*)0x8F6004;
int32 &CStats::ElBurroTime = *(int32*)0x8E2A6C;
int32& CStats::Record4x4One = *(int32*)0x940570;
int32& CStats::Record4x4Two = *(int32*)0x94058C;
int32& CStats::Record4x4Three = *(int32*)0x880FA8;
int32& CStats::Record4x4Mayhem = *(int32*)0x885B70;
int32& CStats::LivesSavedWithAmbulance = *(int32*)0x8F57E0;
int32& CStats::CriminalsCaught = *(int32*)0x8F2518;
int32& CStats::HighestLevelAmbulanceMission = *(int32*)0x8F2A04;
int32& CStats::FiresExtinguished = *(int32*)0x8F5FEC;
int32& CStats::LongestFlightInDodo = *(int32*)0x8F5FE4;
int32& CStats::TimeTakenDefuseMission = *(int32*)0x880E24;
int32& CStats::TotalNumberKillFrenzies = *(int32*)0x8E2884;
int32& CStats::TotalNumberMissions = *(int32*)0x8E2820;
int32& CStats::ShotsMade = *(int32*)0x8E2BE8;
int32& CStats::KgsOfExplosivesUsed = *(int32*)0x8F2510;
int32& CStats::InstantHitsFiredByPlayer = *(int32*)0x943070;
int32& CStats::InstantHitsHitByPlayer = *(int32*)0x95CB8C;
int32& CStats::BestTimeBombDefusal = *(int32*)0x880E24;
int32& CStats::mmRain = *(int32*)0x8F2C98;
int32& CStats::CarsCrushed = *(int32*)0x943050;
int32(&CStats::FastestTimes)[CStats::TOTAL_FASTEST_TIMES] = *(int32(*)[CStats::TOTAL_FASTEST_TIMES])*(uintptr*)0x6E9128;
int32(&CStats::HighestScores)[CStats::TOTAL_HIGHEST_SCORES] = *(int32(*)[CStats::TOTAL_HIGHEST_SCORES]) * (uintptr*)0x8622B0;

void CStats::Init()
{
	PeopleKilledByOthers = 0;
	PeopleKilledByPlayer = 0;
	ShotsMade = 0;
    CarsExploded = 0;
    HelisDestroyed = 0;
    ProgressMade = 0;
    KgsOfExplosivesUsed = 0;
    InstantHitsFiredByPlayer = 0;
    InstantHitsHitByPlayer = 0;
    CarsCrushed = 0;
    HeadsPopped = 0;
    TimesArrested = 0;
    TimesDied = 0;
    DaysPassed = 0;
    NumberOfUniqueJumpsFound = 0;
    mmRain = 0;
    MaximumJumpFlips = 0;
    MaximumJumpSpins = 0;
    MaximumJumpDistance = 0;
    MaximumJumpHeight = 0;
    BestStuntJump = 0;
    TotalNumberOfUniqueJumps = 0;
    Record4x4One = 0;
    LongestFlightInDodo = 0;
    Record4x4Two = 0;
    PassengersDroppedOffWithTaxi = 0;
    Record4x4Three = 0;
    MoneyMadeWithTaxi = 0;
    Record4x4Mayhem = 0;
    LivesSavedWithAmbulance = 0;
    ElBurroTime = 0;
    CriminalsCaught = 0;
    MissionsGiven = 0;
    HighestLevelAmbulanceMission = 0;
    MissionsPassed = 0;
    FiresExtinguished = 0;
    DistanceTravelledOnFoot = 0;
    TimeTakenDefuseMission = 0;
    NumberKillFrenziesPassed = 0;
    DistanceTravelledInVehicle = 0;
    TotalNumberKillFrenzies = 0;
    TotalNumberMissions = 0;
    KillsSinceLastCheckpoint = 0;
    TotalLegitimateKills = 0;
    for (int i = 0; i < TOTAL_FASTEST_TIMES; i++)
    {
        FastestTimes[i] = 0;
    }
    for (int i = 0; i < TOTAL_HIGHEST_SCORES; i++)
    {
        HighestScores[i] = 0;
    }
    for (int i = 0; i < NUM_PEDTYPES; i++)
    {
        PedsKilledOfThisType[i] = 0;
    }
    IndustrialPassed = 0;
    CommercialPassed = 0;
    SuburbanPassed = 0;
}

void CStats::RegisterFastestTime(int32 index, int32 time)
{
	assert(index >= 0 && index < TOTAL_FASTEST_TIMES);
	if (FastestTimes[index] == 0)
		FastestTimes[index] = time;
	else
		FastestTimes[index] = min(FastestTimes[index], time);
}

void CStats::RegisterHighestScore(int32 index, int32 score)
{
	assert(index >= 0 && index < TOTAL_HIGHEST_SCORES);
	HighestScores[index] = max(HighestScores[index], score);
}

void CStats::RegisterElBurroTime(int32 time)
{
	ElBurroTime = (ElBurroTime && ElBurroTime < time) ? ElBurroTime : time;
}

void CStats::Register4x4OneTime(int32 time)
{
	Record4x4One = (Record4x4One && Record4x4One < time) ? Record4x4One : time;
}

void CStats::Register4x4TwoTime(int32 time)
{
	Record4x4Two = (Record4x4Two && Record4x4Two < time) ? Record4x4Two : time;
}

void CStats::Register4x4ThreeTime(int32 time)
{
	Record4x4Three = (Record4x4Three && Record4x4Three < time) ? Record4x4Three : time;
}

void CStats::Register4x4MayhemTime(int32 time)
{
	Record4x4Mayhem = (Record4x4Mayhem && Record4x4Mayhem < time) ? Record4x4Mayhem : time;
}

void CStats::AnotherLifeSavedWithAmbulance()
{
	++LivesSavedWithAmbulance;
}

void CStats::AnotherCriminalCaught()
{
	++CriminalsCaught;
}

void CStats::RegisterLevelAmbulanceMission(int32 level)
{
	HighestLevelAmbulanceMission = max(HighestLevelAmbulanceMission, level);
}

void CStats::AnotherFireExtinguished()
{
	++FiresExtinguished;
}

void CStats::RegisterLongestFlightInDodo(int32 time)
{
	LongestFlightInDodo = max(LongestFlightInDodo, time);
}

void CStats::RegisterTimeTakenDefuseMission(int32 time)
{
	TimeTakenDefuseMission = (TimeTakenDefuseMission && TimeTakenDefuseMission < time) ? TimeTakenDefuseMission : time;
}

void CStats::AnotherKillFrenzyPassed()
{
	++NumberKillFrenziesPassed;
}

void CStats::SetTotalNumberKillFrenzies(int32 total)
{
	TotalNumberKillFrenzies = total;
}

void CStats::SetTotalNumberMissions(int32 total)
{
	TotalNumberMissions = total;
}

//WRAPPER void CStats::Init() { EAXJMP(0x4AAC60); }

STARTPATCHES
	InjectHook(0x48C5A3, CStats::Init, PATCH_JUMP); // CGame::ReInitGameObjectVariables
ENDPATCHES