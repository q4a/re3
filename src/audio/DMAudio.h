#pragma once

#include "audio_enums.h"
#include "CrimeType.h"

enum eSound : int16
{
	SOUND_CAR_DOOR_CLOSE_BONNET = 0,
	SOUND_CAR_DOOR_CLOSE_BUMPER = 1,
	SOUND_CAR_DOOR_CLOSE_FRONT_LEFT = 2,
	SOUND_CAR_DOOR_CLOSE_FRONT_RIGHT = 3,
	SOUND_CAR_DOOR_CLOSE_BACK_LEFT = 4,
	SOUND_CAR_DOOR_CLOSE_BACK_RIGHT = 5,
	SOUND_CAR_DOOR_OPEN_BONNET = 6,
	SOUND_CAR_DOOR_OPEN_BUMPER = 7,
	SOUND_CAR_DOOR_OPEN_FRONT_LEFT = 8,
	SOUND_CAR_DOOR_OPEN_FRONT_RIGHT = 9,
	SOUND_CAR_DOOR_OPEN_BACK_LEFT = 10,
	SOUND_CAR_DOOR_OPEN_BACK_RIGHT = 11,
	SOUND_CAR_WINDSHIELD_CRACK = 12,
	SOUND_CAR_JUMP = 13,
	SOUND_E = 14,
	SOUND_F = 15,
	SOUND_CAR_ENGINE_START = 16,
	SOUND_CAR_LIGHT_BREAK = 17,
	SOUND_CAR_HYDRAULIC_1 = 18,
	SOUND_CAR_HYDRAULIC_2 = 19,
	SOUND_CAR_HYDRAULIC_3 = 20,
	SOUND_CAR_JERK = 21,
	SOUND_CAR_SPLASH = 22,
	SOUND_17 = 23,
	SOUND_18 = 24,
	SOUND_19 = 25,
	SOUND_CAR_TANK_TURRET_ROTATE = 26,
	SOUND_CAR_BOMB_TICK = 27,
	SOUND_PLANE_ON_GROUND = 28,
	SOUND_STEP_START = 29,
	SOUND_STEP_END = 30,
	SOUND_FALL_LAND = 31,
	SOUND_FALL_COLLAPSE = 32,
	SOUND_FIGHT_PUNCH_33 = 33,
	SOUND_FIGHT_KICK_34 = 34,
	SOUND_FIGHT_HEADBUTT_35 = 35,
	SOUND_FIGHT_PUNCH_36 = 36,
	SOUND_FIGHT_PUNCH_37 = 37,
	SOUND_FIGHT_CLOSE_PUNCH_38 = 38,
	SOUND_FIGHT_PUNCH_39 = 39,
	SOUND_FIGHT_PUNCH_OR_KICK_BELOW_40 = 40,
	SOUND_FIGHT_PUNCH_41 = 41,
	SOUND_FIGHT_PUNCH_FROM_BEHIND_42 = 42,
	SOUND_FIGHT_KNEE_OR_KICK_43 = 43,
	SOUND_FIGHT_KICK_44 = 44,
	SOUND_2D = 45,
	SOUND_WEAPON_BAT_ATTACK = 46,
	SOUND_WEAPON_SHOT_FIRED = 47,
	SOUND_WEAPON_RELOAD = 48,
	SOUND_WEAPON_AK47_BULLET_ECHO = 49,
	SOUND_WEAPON_UZI_BULLET_ECHO = 50,
	SOUND_WEAPON_M16_BULLET_ECHO = 51,
	SOUND_WEAPON_FLAMETHROWER_FIRE = 52,
	SOUND_WEAPON_SNIPER_SHOT_NO_ZOOM = 53,
	SOUND_WEAPON_ROCKET_SHOT_NO_ZOOM = 54,
	SOUND_WEAPON_HIT_PED = 55,
	SOUND_WEAPON_HIT_VEHICLE = 56,
	SOUND_GARAGE_NO_MONEY = 57,
	SOUND_GARAGE_BAD_VEHICLE = 58,
	SOUND_GARAGE_OPENING = 59,
	SOUND_GARAGE_BOMB_ALREADY_SET = 60,
	SOUND_GARAGE_BOMB1_SET = 61,
	SOUND_GARAGE_BOMB2_SET = 62,
	SOUND_GARAGE_BOMB3_SET = 63,
	SOUND_40 = 64,
	SOUND_41 = 65,
	SOUND_GARAGE_VEHICLE_DECLINED = 66,
	SOUND_GARAGE_VEHICLE_ACCEPTED = 67,
	SOUND_GARAGE_DOOR_CLOSED = 68,
	SOUND_GARAGE_DOOR_OPENED = 69,
	SOUND_CRANE_PICKUP = 70,
	SOUND_PICKUP_WEAPON_BOUGHT = 71,
	SOUND_PICKUP_WEAPON = 72,
	SOUND_PICKUP_HEALTH = 73,
	SOUND_4A = 74,
	SOUND_4B = 75,
	SOUND_PICKUP_ADRENALINE = 76,
	SOUND_PICKUP_ARMOUR = 77,
	SOUND_PICKUP_BONUS = 78,
	SOUND_PICKUP_MONEY = 79,
	SOUND_PICKUP_HIDDEN_PACKAGE = 80,
	SOUND_PICKUP_PACMAN_PILL = 81,
	SOUND_PICKUP_PACMAN_PACKAGE = 82,
	SOUND_PICKUP_FLOAT_PACKAGE = 83,
	SOUND_BOMB_TIMED_ACTIVATED = 84,
	SOUND_55 = 85,
	SOUND_BOMB_ONIGNITION_ACTIVATED = 86,
	SOUND_BOMB_TICK = 87,
	SOUND_RAMPAGE_START = 88,
	SOUND_RAMPAGE_ONGOING = 89,
	SOUND_RAMPAGE_PASSED = 90,
	SOUND_RAMPAGE_FAILED = 91,
	SOUND_RAMPAGE_KILL = 92,
	SOUND_RAMPAGE_CAR_BLOWN = 93,
	SOUND_EVIDENCE_PICKUP = 94,
	SOUND_UNLOAD_GOLD = 95,
	SOUND_PAGER = 96,
	SOUND_PED_DEATH = 97, // 103 in VC
	SOUND_PED_DAMAGE = 98, // 104 in VC
	SOUND_PED_HIT = 99, // 105 in VC
	SOUND_PED_LAND = 100, // hopefully 106 in VC
	SOUND_PED_BULLET_HIT = 101,
	SOUND_PED_BOMBER = 102,
	SOUND_PED_BURNING = 103, // 108 in VC
	SOUND_PED_ARREST_FBI = 104,
	SOUND_PED_ARREST_SWAT = 105,
	SOUND_PED_ARREST_COP = 106,
	SOUND_PED_HELI_PLAYER_FOUND = 107,
	SOUND_PED_HANDS_UP = 108,
	SOUND_PED_HANDS_COWER = 109,
	SOUND_PED_FLEE_SPRINT = 110, // 120 in VC
	SOUND_PED_CAR_JACKING = 111,
	SOUND_PED_MUGGING = 112,
	SOUND_PED_CAR_JACKED = 113,
	SOUND_PED_ROBBED = 114,
	SOUND_PED_TAXI_WAIT = 115, // 137 in VC
	SOUND_PED_ATTACK = 116,
	SOUND_PED_DEFEND = 117,
	SOUND_PED_PURSUIT_ARMY = 118,
	SOUND_PED_PURSUIT_FBI = 119,
	SOUND_PED_PURSUIT_SWAT = 120,
	SOUND_PED_PURSUIT_COP = 121,
	SOUND_PED_HEALING = 122,
	SOUND_PED_7B = 123,
	SOUND_PED_LEAVE_VEHICLE = 124,
	SOUND_PED_EVADE = 125, // 142 in VC
	SOUND_PED_FLEE_RUN = 126,
	SOUND_PED_CAR_COLLISION = 127, // 144-145-146 in VC
	SOUND_PED_SOLICIT = 128,
	SOUND_PED_EXTINGUISHING_FIRE = 129,
	SOUND_PED_WAIT_DOUBLEBACK = 130,
	SOUND_PED_CHAT_SEXY = 131,
	SOUND_PED_CHAT_EVENT = 132,
	SOUND_PED_CHAT = 133,
	SOUND_PED_BODYCAST_HIT = 134,
	SOUND_PED_TAXI_CALL = 135,
	SOUND_INJURED_PED_MALE_OUCH = 136,
	SOUND_INJURED_PED_FEMALE = 137,
	SOUND_8A = 138,
	SOUND_RACE_START_3 = 139,
	SOUND_RACE_START_2 = 140,
	SOUND_RACE_START_1 = 141,
	SOUND_RACE_START_GO = 142,
	SOUND_SPLASH = 143,
	SOUND_WATER_FALL = 144,
	SOUND_SPLATTER = 145,
	SOUND_CAR_PED_COLLISION = 146,
	SOUND_CLOCK_TICK = 147,
	SOUND_PART_MISSION_COMPLETE = 148,
	SOUND_FRONTEND_MENU_STARTING = 149,
	SOUND_FRONTEND_MENU_COMPLETED = 150,
	SOUND_FRONTEND_MENU_DENIED = 151,
	SOUND_FRONTEND_MENU_SUCCESS = 152,
	SOUND_FRONTEND_EXIT = 153,
	SOUND_9A = 154,
	SOUND_9B = 155,
	SOUND_FRONTEND_AUDIO_TEST = 156,
	SOUND_FRONTEND_FAIL = 157,
	SOUND_FRONTEND_NO_RADIO = 158,
	SOUND_FRONTEND_RADIO_CHANGE = 159,
	SOUND_A0 = 160,
	SOUND_AMMUNATION_WELCOME_1 = 161,
	SOUND_AMMUNATION_WELCOME_2 = 162,
	SOUND_AMMUNATION_WELCOME_3 = 163,
	SOUND_LIGHTNING = 164,
	SOUND_A5 = 165,
	SOUND_TOTAL_SOUNDS = 166,
	SOUND_TOTAL_PED_SOUNDS = 167,
};

#define AEHANDLE_IS_FAILED(h) ((h)<0)
#define AEHANDLE_IS_OK(h)     ((h)>=0)

class cAudioScriptObject;
class CEntity;
enum eCrimeType;

class cDMAudio
{
public:
	~cDMAudio()
	{ }

	void Initialise(void);
	void Terminate(void);
	void Service(void);
	
	int32 CreateEntity(int32 type, void *UID);
	void DestroyEntity(int32 audioEntity);
	void SetEntityStatus(int32 audioEntity, uint8 status);
	void PlayOneShot(int32 audioEntity, uint16 oneShot, float volume);
	void DestroyAllGameCreatedEntities(void);
	
	void SetEffectsMasterVolume(uint8 volume);
	void SetMusicMasterVolume(uint8 volume);
	void SetEffectsFadeVol(uint8 volume);
	void SetMusicFadeVol(uint8 volume);
	
	uint8 GetNum3DProvidersAvailable(void);
	char *Get3DProviderName(uint8 id);
	
	int8 GetCurrent3DProviderIndex(void);
	int8 SetCurrent3DProvider(uint8 which);
	
	void SetSpeakerConfig(int32 config);
	
	bool IsMP3RadioChannelAvailable(void);
	
	void ReleaseDigitalHandle(void);
	void ReacquireDigitalHandle(void);
	
	void SetDynamicAcousticModelingStatus(uint8 status);
	
	bool CheckForAnAudioFileOnCD(void);
	
	char GetCDAudioDriveLetter(void);
	bool IsAudioInitialised(void);
	
	void ReportCrime(eCrimeType crime, CVector const &pos);
	
	int32 CreateLoopingScriptObject(cAudioScriptObject *scriptObject);
	void DestroyLoopingScriptObject(int32 audioEntity);
	void CreateOneShotScriptObject(cAudioScriptObject *scriptObject);
	
	void PlaySuspectLastSeen(float x, float y, float z);
	
	void ReportCollision(CEntity *entityA, CEntity *entityB, uint8 surfaceTypeA, uint8 surfaceTypeB, float collisionPower, float velocity);
	
	void PlayFrontEndSound(uint16 frontend, uint32 volume);
	void PlayRadioAnnouncement(uint8 announcement);
	void PlayFrontEndTrack(uint8 track, uint8 frontendFlag);
	void StopFrontEndTrack(void);
	
	void ResetTimers(uint32 time);
	
	void ChangeMusicMode(uint8 mode);
	
	void PreloadCutSceneMusic(uint8 track);
	void PlayPreloadedCutSceneMusic(void);
	void StopCutSceneMusic(void);
	
	void PreloadMissionAudio(char *missionAudio);
	uint8 GetMissionAudioLoadingStatus(void);
	void SetMissionAudioLocation(float x, float y, float z);
	void PlayLoadedMissionAudio(void);
	bool IsMissionAudioSampleFinished(void);
	void ClearMissionAudio(void);

	uint8 GetRadioInCar(void);
	void SetRadioInCar(uint32 radio);
	void SetRadioChannel(int8 radio, int32 pos);
};
extern cDMAudio DMAudio;
