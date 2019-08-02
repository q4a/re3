#pragma once

class CSpecialFX
{
public:
	static void Render(void);
};

class CMotionBlurStreaks
{
public:
	static void RegisterStreak(int32 id, uint8 r, uint8 g, uint8 b, CVector p1, CVector p2);
};

struct CBulletTrace
{
	CVector m_vecInf;
	CVector m_vecSup;
	bool m_bInUse;
	uint8 m_framesInUse;
	uint8 m_lifeTime;
};

class CBulletTraces
{
public:
	static CBulletTrace (&aTraces)[16];

	static void Init(void);
};

class CBrightLights
{
public:
	static void RegisterOne(CVector pos, CVector up, CVector right, CVector fwd, uint8 type, uint8 unk1 = 0, uint8 unk2 = 0, uint8 unk3 = 0);
};

class C3dMarkers
{
public:
	static void PlaceMarkerSet(uint32 id, uint16 type, CVector& pos, float size, uint8 r, uint8 g, uint8 b, uint8 a, uint16 pulsePeriod, float pulseFraction, int16 rotateRate);  
};

