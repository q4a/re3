#include "common.h"
#include "patcher.h"
#include "Weather.h"

int32 &CWeather::SoundHandle = *(int32*)0x5FFBC4;

int32 &CWeather::WeatherTypeInList = *(int32*)0x8F626C;
int16 &CWeather::OldWeatherType = *(int16*)0x95CCEC;
int16 &CWeather::NewWeatherType = *(int16*)0x95CC70;
int16 &CWeather::ForcedWeatherType = *(int16*)0x95CC80;

bool &CWeather::LightningFlash = *(bool*)0x95CDA3;
bool &CWeather::LightningBurst = *(bool*)0x95CDAC;
uint32 &CWeather::LightningStart = *(uint32*)0x8F5F84;
uint32 &CWeather::LightningFlashLastChange = *(uint32*)0x8E2C0C;
uint32 &CWeather::WhenToPlayLightningSound = *(uint32*)0x8F57E4;
uint32 &CWeather::LightningDuration = *(uint32*)0x940578;

float &CWeather::Foggyness = *(float*)0x885AF4;
float &CWeather::CloudCoverage = *(float*)0x8E2818;
float &CWeather::Wind = *(float*)0x8E2BF8;
float &CWeather::Rain = *(float*)0x8E2BFC;
float &CWeather::InterpolationValue = *(float*)0x8F2520;
float &CWeather::WetRoads = *(float*)0x8F5FF8;
float &CWeather::Rainbow = *(float*)0x940598;

bool &CWeather::bScriptsForceRain = *(bool*)0x95CD7D;
bool &CWeather::Stored_StateStored = *(bool*)0x95CDC1;

float &CWeather::Stored_InterpolationValue = *(float*)0x942F54;
int16 &CWeather::Stored_OldWeatherType = *(int16*)0x95CC68;
int16 &CWeather::Stored_NewWeatherType = *(int16*)0x95CCAE;
float &CWeather::Stored_Rain = *(float*)0x885B4C;

WRAPPER void CWeather::RenderRainStreaks(void) { EAXJMP(0x524550); }
WRAPPER void CWeather::Update(void) { EAXJMP(0x522C10); }
WRAPPER void CWeather::Init(void) { EAXJMP(0x522BA0); }

void CWeather::ReleaseWeather()
{
	ForcedWeatherType = -1;
}

void CWeather::ForceWeather(int16 weather)
{
	ForcedWeatherType = weather;
}

void CWeather::ForceWeatherNow(int16 weather)
{
	OldWeatherType = weather;
	NewWeatherType = weather;
	ForcedWeatherType = weather;
}

void CWeather::StoreWeatherState()
{
	Stored_StateStored = true;
	Stored_InterpolationValue = InterpolationValue;
	Stored_Rain = Rain;
	Stored_NewWeatherType = NewWeatherType;
	Stored_OldWeatherType = OldWeatherType;
}

void CWeather::RestoreWeatherState()
{
#ifdef FIX_BUGS // it's not used anyway though
	Stored_StateStored = false;
#endif
	InterpolationValue = Stored_InterpolationValue;
	Rain = Stored_Rain;
	NewWeatherType = Stored_NewWeatherType;
	OldWeatherType = Stored_OldWeatherType;
}