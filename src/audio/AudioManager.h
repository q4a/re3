#pragma once

#include "AudioSamples.h"
#include "common.h"

class tActiveSample
{
public:
	int m_nEntityIndex;
	int field_4;
	int m_nSampleIndex;
	char m_bBankIndex;
	char m_bIsDistant;
	char field_14;
	char field_15;
	int field_16;
	int m_nFrequency;
	uint8 m_bVolume;
	char field_25;
	char field_26;
	char field_27;
	float m_fDistance;
	int m_nLoopCount;
	int m_nLoopStart;
	int m_nLoopEnd;
	uint8 m_bEmittingVolume;
	char field_45;
	char field_46;
	char field_47;
	float field_48;
	float m_fSoundIntensity;
	char field_56;
	char field_57;
	char field_58;
	char field_59;
	CVector m_vecPos;
	char m_bReverbFlag;
	char m_bLoopsRemaining;
	char m_bRequireReflection;
	uint8 m_bOffset;
	int field_76;
	char m_bIsProcessed;
	char m_bLoopEnded;
	char field_82;
	char field_83;
	int field_84;
	char field_88;
	char field_89;
	char field_90;
	char field_91;
};

static_assert(sizeof(tActiveSample) == 0x5c, "tActiveSample: error");

enum eAudioType : int32 {
	AUDIOTYPE_PHYSICAL = 0,
	AUDIOTYPE_EXPLOSION = 1,
	AUDIOTYPE_FIRE = 2,
	AUDIOTYPE_WEATHER = 3,
	AUDIOTYPE_CRANE = 4,
	AUDIOTYPE_ONE_SHOT = 5,
	AUDIOTYPE_BRIDGE = 6,
	AUDIOTYPE_COLLISION = 7,
	AUDIOTYPE_FRONTEND = 8,
	AUDIOTYPE_PROJECTILE = 9,
	AUDIOTYPE_GARAGE = 10,
	AUDIOTYPE_HYDRANT = 11,
	AUDIOTYPE_WATER_CANNON = 12,
	AUDIOTYPE_D = 13,
	TOTAL_AUDIO_TYPES = 14,
};

class tAudioEntity
{
public:
	eAudioType m_nType;
	void *m_pEntity;
	char m_bIsUsed;
	char m_bStatus;
	__int16 m_awAudioEvent[4];
	char gap_18[2];
	float m_afVolume[4];
	char field_24;
	char field_25[3];
};

static_assert(sizeof(tAudioEntity) == 0x28, "tAudioEntity: error");

class tPedComment
{
public:
	int m_nSampleIndex;
	int field_4;
	CVector m_vecPos;
	float m_fDistance;
	char m_bVolume;
	char field_25;
	char gap_26[2];
};

static_assert(sizeof(tPedComment) == 0x1c, "tPedComment: error");

class cPedComments {
public:
	tPedComment m_asPedComments[40];
	char field_1120[40];
	char field_1160[2];
	char field_1162;
	char gap_1163[1];
};

static_assert(sizeof(cPedComments) == 0x48c, "cPedComments: error");

class CEntity;

class cAudioCollision {
public:
	CEntity *m_pEntity1;
	CEntity *m_pEntity2;
	char m_bSurface1;
	char m_bSurface2;
	char field_10;
	char field_11;
	float m_fIntensity1;
	float m_fIntensity2;
	CVector m_vecPosition;
	float m_fDistance;
	int m_nBaseVolume;
};

static_assert(sizeof(cAudioCollision) == 0x28, "cAudioCollision: error");

class cAudioCollisionManager {
public:
	cAudioCollision m_asCollisions1[10];
	cAudioCollision m_asCollisions2[10];
	char m_bIndicesTable[10];
	char m_bCollisionsInQueue;
	char gap_811;
	cAudioCollision m_sQueue;
};

static_assert(sizeof(cAudioCollisionManager) == 0x354,
              "cAudioCollisionManager: error");

struct cMissionAudio {
	CVector m_vecPos;
	char field_12;
	char gap_13[3];
	int m_nSampleIndex;
	char m_bLoadingStatus;
	char m_bPlayStatus;
	char field_22;
	char field_23;
	int field_24;
	char m_bIsPlayed;
	char field_29;
	char field_30;
	char field_31;
};

static_assert(sizeof(cMissionAudio) == 0x20, "cMissionAudio: error");

class cAudioManager
{
public:
	bool m_bIsInitialised;
	char field_1;
	char field_2;
	char m_bActiveSamples;
	char field_4;
	char m_bDynamicAcousticModelingStatus;
	char field_6;
	char field_7;
	float field_8;
	bool m_bTimerJustReset;
	char field_13;
	char field_14;
	char field_15;
	int m_nTimer;
	tActiveSample m_sQueueSample;
	char m_bActiveSampleQueue;
	char gap_109[3];
	tActiveSample m_asSamples[54];
	char m_abSampleQueueIndexTable[54];
	char m_bSampleRequestQueuesStatus[2];
	tActiveSample m_asActiveSamples[27];
	tAudioEntity m_asAudioEntities[200];
	int m_anAudioEntityIndices[200];
	int m_nAudioEntitiesTotal;
	CVector m_avecReflectionsPos[5];
	float m_afReflectionsDistances[5];
	int m_anScriptObjectEntityIndices[40];
	int m_nScriptObjectEntityTotal;
	cPedComments m_sPedComments;
	int m_nFireAudioEntity;
	int m_nWaterCannonEntity;
	int m_nPoliceChannelEntity;
	char gap45B8[444];
	int m_nFrontEndEntity;
	int m_nCollisionEntity;
	cAudioCollisionManager m_sCollisionManager;
	int m_nProjectileEntity;
	int m_nBridgeEntity;
	cMissionAudio m_sMissionAudio;
	int m_anRandomTable[5];
	char field_19192;
	char m_bUserPause;
	char m_bPreviousUserPause;
	char field_19195;
	int m_nTimeOfRecentCrime;

	void PlayerJustLeftCar(void);
	void Service();
	void GetPhrase(uint32 *a2, uint32 *a3, eAudioSamples sample,
	               uint32 maxOffset);
};

static_assert(sizeof(cAudioManager) == 0x4B14, "cAudioManager: error");

extern cAudioManager &AudioManager;
