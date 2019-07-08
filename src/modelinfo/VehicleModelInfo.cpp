#include "common.h"
#include <rpmatfx.h>
#include "patcher.h"
#include "RwHelper.h"
#include "General.h"
#include "NodeName.h"
#include "TxdStore.h"
#include "Weather.h"
#include "HandlingMgr.h"
#include "VisibilityPlugins.h"
#include "FileMgr.h"
#include "World.h"
#include "Vehicle.h"
#include "ModelIndices.h"
#include "ModelInfo.h"

int8 *CVehicleModelInfo::ms_compsToUse = (int8*)0x5FF2EC;	// -2, -2
int8 *CVehicleModelInfo::ms_compsUsed = (int8*)0x95CCB2;
RwTexture **CVehicleModelInfo::ms_pEnvironmentMaps = (RwTexture **)0x8F1A30;
RwRGBA *CVehicleModelInfo::ms_vehicleColourTable = (RwRGBA*)0x86BA88;
RwTexture **CVehicleModelInfo::ms_colourTextureTable = (RwTexture**)0x711C40;

RwTexture *&gpWhiteTexture = *(RwTexture**)0x64C4F8;
RwFrame *&pMatFxIdentityFrame = *(RwFrame**)0x64C510;

enum {
	VEHICLE_FLAG_COLLAPSE	= 0x2,
	VEHICLE_FLAG_ADD_WHEEL	= 0x4,
	VEHICLE_FLAG_POS	= 0x8,
	VEHICLE_FLAG_DOOR	= 0x10,
	VEHICLE_FLAG_LEFT	= 0x20,
	VEHICLE_FLAG_RIGHT	= 0x40,
	VEHICLE_FLAG_FRONT	= 0x80,
	VEHICLE_FLAG_REAR	= 0x100,
	VEHICLE_FLAG_COMP	= 0x200,
	VEHICLE_FLAG_DRAWLAST	= 0x400,
	VEHICLE_FLAG_WINDSCREEN	= 0x800,
	VEHICLE_FLAG_ANGLECULL	= 0x1000,
	VEHICLE_FLAG_REARDOOR	= 0x2000,
	VEHICLE_FLAG_FRONTDOOR	= 0x4000,
};

RwObjectNameIdAssocation carIds[] = {
	{ "wheel_rf_dummy",	CAR_WHEEL_RF,	VEHICLE_FLAG_RIGHT | VEHICLE_FLAG_ADD_WHEEL },
	{ "wheel_rm_dummy",	CAR_WHEEL_RM,	VEHICLE_FLAG_RIGHT | VEHICLE_FLAG_ADD_WHEEL },
	{ "wheel_rb_dummy",	CAR_WHEEL_RB,	VEHICLE_FLAG_RIGHT | VEHICLE_FLAG_ADD_WHEEL },
	{ "wheel_lf_dummy",	CAR_WHEEL_LF,	VEHICLE_FLAG_LEFT | VEHICLE_FLAG_ADD_WHEEL },
	{ "wheel_lm_dummy",	CAR_WHEEL_LM,	VEHICLE_FLAG_LEFT | VEHICLE_FLAG_ADD_WHEEL },
	{ "wheel_lb_dummy",	CAR_WHEEL_LB,	VEHICLE_FLAG_LEFT | VEHICLE_FLAG_ADD_WHEEL },
	{ "bump_front_dummy",	CAR_BUMP_FRONT,	VEHICLE_FLAG_FRONT | VEHICLE_FLAG_COLLAPSE },
	{ "bonnet_dummy",	CAR_BONNET,	VEHICLE_FLAG_COLLAPSE },
	{ "wing_rf_dummy",	CAR_WING_RF,	VEHICLE_FLAG_COLLAPSE },
	{ "wing_rr_dummy",	CAR_WING_RR,	VEHICLE_FLAG_RIGHT | VEHICLE_FLAG_COLLAPSE },
	{ "door_rf_dummy",	CAR_DOOR_RF,	VEHICLE_FLAG_FRONTDOOR | VEHICLE_FLAG_ANGLECULL | VEHICLE_FLAG_RIGHT | VEHICLE_FLAG_DOOR | VEHICLE_FLAG_COLLAPSE },
	{ "door_rr_dummy",	CAR_DOOR_RR,	VEHICLE_FLAG_REARDOOR | VEHICLE_FLAG_ANGLECULL | VEHICLE_FLAG_REAR | VEHICLE_FLAG_RIGHT | VEHICLE_FLAG_DOOR | VEHICLE_FLAG_COLLAPSE },
	{ "wing_lf_dummy",	CAR_WING_LF,	VEHICLE_FLAG_COLLAPSE },
	{ "wing_lr_dummy",	CAR_WING_LR,	VEHICLE_FLAG_LEFT | VEHICLE_FLAG_COLLAPSE },
	{ "door_lf_dummy",	CAR_DOOR_LF,	VEHICLE_FLAG_FRONTDOOR | VEHICLE_FLAG_ANGLECULL | VEHICLE_FLAG_LEFT | VEHICLE_FLAG_DOOR | VEHICLE_FLAG_COLLAPSE },
	{ "door_lr_dummy",	CAR_DOOR_LR,	VEHICLE_FLAG_REARDOOR | VEHICLE_FLAG_ANGLECULL | VEHICLE_FLAG_REAR | VEHICLE_FLAG_LEFT | VEHICLE_FLAG_DOOR | VEHICLE_FLAG_COLLAPSE },
	{ "boot_dummy",		CAR_BOOT,	VEHICLE_FLAG_REAR | VEHICLE_FLAG_COLLAPSE },
	{ "bump_rear_dummy",	CAR_BUMP_REAR,	VEHICLE_FLAG_REAR | VEHICLE_FLAG_COLLAPSE },
	{ "windscreen_dummy",	CAR_WINDSCREEN,	VEHICLE_FLAG_WINDSCREEN | VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_FRONT | VEHICLE_FLAG_COLLAPSE },

	{ "ped_frontseat",	CAR_POS_FRONTSEAT,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "ped_backseat",	CAR_POS_BACKSEAT,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "headlights",		CAR_POS_HEADLIGHTS,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "taillights",		CAR_POS_TAILLIGHTS,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "exhaust",		CAR_POS_EXHAUST,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "extra1",		0, VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ "extra2",		0, VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ "extra3",		0, VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ "extra4",		0, VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ "extra5",		0, VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ "extra6",		0, VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ nil, 0, 0 }
};

RwObjectNameIdAssocation boatIds[] = {
	{ "boat_moving_hi",	1,	VEHICLE_FLAG_COLLAPSE },
	{ "boat_rudder_hi",	3,	VEHICLE_FLAG_COLLAPSE },
	{ "windscreen",		2,	VEHICLE_FLAG_WINDSCREEN | VEHICLE_FLAG_COLLAPSE },
	{ "ped_frontseat",	0,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ nil, 0, 0 }
};

RwObjectNameIdAssocation trainIds[] = {
	{ "door_lhs_dummy",	1,	VEHICLE_FLAG_LEFT | VEHICLE_FLAG_COLLAPSE },
	{ "door_rhs_dummy",	2,	VEHICLE_FLAG_LEFT | VEHICLE_FLAG_COLLAPSE },
	{ "light_front",	0,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "light_rear",		1,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "ped_left_entry",	2,	VEHICLE_FLAG_DOOR | VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "ped_mid_entry",	3,	VEHICLE_FLAG_DOOR | VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "ped_right_entry",	4,	VEHICLE_FLAG_DOOR | VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ nil, 0, 0 }
};

RwObjectNameIdAssocation heliIds[] = {
	{ "chassis_dummy",	1,	VEHICLE_FLAG_COLLAPSE },
	{ "toprotor",		2,	0 },
	{ "backrotor",		3,	0 },
	{ "tail",		4,	0 },
	{ "topknot",		5,	0 },
	{ "skid_left",		6,	0 },
	{ "skid_right",		7,	0 },
	{ nil, 0, 0 }
};

RwObjectNameIdAssocation planeIds[] = {
	{ "wheel_front_dummy",	2,	0 },
	{ "wheel_rear_dummy",	3,	0 },
	{ "light_tailplane",	2,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "light_left",		0,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "light_right",	1,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ nil, 0, 0 }
};

RwObjectNameIdAssocation bikeIds[] = {
	{ "chassis_dummy",	1,	0 },
	{ "forks_front",	2,	0 },
	{ "forks_rear",		3,	0 },
	{ "wheel_front",	4,	0 },
	{ "wheel_rear",		5,	0 },
	{ "mudguard",		6,	0 },
	{ "ped_frontseat",	2,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "headlights",		0,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "taillights",		1,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "exhaust",		9,	VEHICLE_FLAG_POS | CLUMP_FLAG_NO_HIERID },
	{ "extra1",		0,	VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ "extra2",		0,	VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ "extra3",		0,	VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ "extra4",		0,	VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ "extra5",		0,	VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ "extra6",		0,	VEHICLE_FLAG_DRAWLAST | VEHICLE_FLAG_COMP | CLUMP_FLAG_NO_HIERID },
	{ nil, 0, 0 }
};

RwObjectNameIdAssocation *CVehicleModelInfo::ms_vehicleDescs[] = {
	carIds,
	boatIds,
	trainIds,
	heliIds,
	planeIds,
	bikeIds
};


CVehicleModelInfo::CVehicleModelInfo(void)
 : CClumpModelInfo(MITYPE_VEHICLE)
{
	int32 i;
	for(i = 0; i < NUM_VEHICLE_POSITIONS; i++){
		m_positions[i].x = 0.0f;
		m_positions[i].y = 0.0f;
		m_positions[i].z = 0.0f;
	}
	m_numColours = 0;
}

void
CVehicleModelInfo::DeleteRwObject(void)
{
	int32 i;
	RwFrame *f;

	for(i = 0; i < m_numComps; i++){
		f = RpAtomicGetFrame(m_comps[i]);
		RpAtomicDestroy(m_comps[i]);
		RwFrameDestroy(f);
	}
	m_numComps = 0;
	CClumpModelInfo::DeleteRwObject();
}

RwObject*
CVehicleModelInfo::CreateInstance(void)
{
	RpClump *clump;
	RpAtomic *atomic;
	RwFrame *clumpframe, *f;
	int32 comp1, comp2;

	clump = (RpClump*)CClumpModelInfo::CreateInstance();
	if(m_numComps != 0){
		clumpframe = RpClumpGetFrame(clump);

		comp1 = ChooseComponent();
		if(comp1 != -1){
			atomic = RpAtomicClone(m_comps[comp1]);
			f = RwFrameCreate();
			RwFrameTransform(f,
				RwFrameGetMatrix(RpAtomicGetFrame(m_comps[comp1])),
				rwCOMBINEREPLACE);
			RpAtomicSetFrame(atomic, f);
			RpClumpAddAtomic(clump, atomic);
			RwFrameAddChild(clumpframe, f);
		}
		ms_compsUsed[0] = comp1;

		comp2 = ChooseSecondComponent();
		if(comp2 != -1){
			atomic = RpAtomicClone(m_comps[comp2]);
			f = RwFrameCreate();
			RwFrameTransform(f,
				RwFrameGetMatrix(RpAtomicGetFrame(m_comps[comp2])),
				rwCOMBINEREPLACE);
			RpAtomicSetFrame(atomic, f);
			RpClumpAddAtomic(clump, atomic);
			RwFrameAddChild(clumpframe, f);
		}
		ms_compsUsed[1] = comp2;
	}else{
		ms_compsUsed[0] = -1;
		ms_compsUsed[1] = -1;
	}
	return (RwObject*)clump;
}

void
CVehicleModelInfo::SetClump(RpClump *clump)
{
	CClumpModelInfo::SetClump(clump);
	SetAtomicRenderCallbacks();
	SetFrameIds(ms_vehicleDescs[m_vehicleType]);
	PreprocessHierarchy();
	FindEditableMaterialList();
	m_envMap = nil;
	SetEnvironmentMap();
}

RwFrame*
CVehicleModelInfo::CollapseFramesCB(RwFrame *frame, void *data)
{
	RwFrameForAllChildren(frame, CollapseFramesCB, data);
	RwFrameForAllObjects(frame, MoveObjectsCB, data);
	RwFrameDestroy(frame);
	return frame;
}

RwObject*
CVehicleModelInfo::MoveObjectsCB(RwObject *object, void *data)
{
	RpAtomicSetFrame((RpAtomic*)object, (RwFrame*)data);
	return object;
}

RpAtomic*
CVehicleModelInfo::HideDamagedAtomicCB(RpAtomic *atomic, void *data)
{
	if(strstr(GetFrameNodeName(RpAtomicGetFrame(atomic)), "_dam")){
		RpAtomicSetFlags(atomic, 0);
		CVisibilityPlugins::SetAtomicFlag(atomic, ATOMIC_FLAG_DAM);
	}else if(strstr(GetFrameNodeName(RpAtomicGetFrame(atomic)), "_ok"))
		CVisibilityPlugins::SetAtomicFlag(atomic, ATOMIC_FLAG_OK);
	return atomic;
}

RpAtomic*
CVehicleModelInfo::HideAllComponentsAtomicCB(RpAtomic *atomic, void *data)
{
	if(CVisibilityPlugins::GetAtomicId(atomic) & (int)data)
		RpAtomicSetFlags(atomic, 0);
	else
		RpAtomicSetFlags(atomic, rpATOMICRENDER);
	return atomic;
}

RpMaterial*
CVehicleModelInfo::HasAlphaMaterialCB(RpMaterial *material, void *data)
{
	if(RpMaterialGetColor(material)->alpha != 0xFF){
		*(bool*)data = true;
		return nil;
	}
	return material;
}


RpAtomic*
CVehicleModelInfo::SetAtomicRendererCB(RpAtomic *atomic, void *data)
{
	RpClump *clump;
	char *name;
	bool alpha;

	clump = (RpClump*)data;
	name = GetFrameNodeName(RpAtomicGetFrame(atomic));
	alpha = false;
	RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), HasAlphaMaterialCB, &alpha);
	if(strstr(name, "_hi") || strncmp(name, "extra", 5) == 0){
		if(alpha || strncmp(name, "windscreen", 10) == 0)
			CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailAlphaCB);
		else
			CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailCB);
	}else if(strstr(name, "_lo")){
		RpClumpRemoveAtomic(clump, atomic);
		RpAtomicDestroy(atomic);
		return atomic;		// BUG: not done by gta
	}else if(strstr(name, "_vlo"))
		CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleReallyLowDetailCB);
	else
		CVisibilityPlugins::SetAtomicRenderCallback(atomic, nil);
	HideDamagedAtomicCB(atomic, nil);
	return atomic;
}

RpAtomic*
CVehicleModelInfo::SetAtomicRendererCB_BigVehicle(RpAtomic *atomic, void *data)
{
	char *name;
	bool alpha;

	name = GetFrameNodeName(RpAtomicGetFrame(atomic));
	alpha = false;
	RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), HasAlphaMaterialCB, &alpha);
	if(strstr(name, "_hi") || strncmp(name, "extra", 5) == 0){
		if(alpha)
			CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_BigVehicle);
		else
			CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailCB_BigVehicle);
	}else if(strstr(name, "_lo")){
		if(alpha)
			CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleLowDetailAlphaCB_BigVehicle);
		else
			CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleLowDetailCB_BigVehicle);
	}else if(strstr(name, "_vlo"))
		CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle);
	else
		CVisibilityPlugins::SetAtomicRenderCallback(atomic, nil);
	HideDamagedAtomicCB(atomic, nil);
	return atomic;
}

RpAtomic*
CVehicleModelInfo::SetAtomicRendererCB_Train(RpAtomic *atomic, void *data)
{
	char *name;
	bool alpha;

	name = GetFrameNodeName(RpAtomicGetFrame(atomic));
	alpha = false;
	RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), HasAlphaMaterialCB, &alpha);
	if(strstr(name, "_hi")){
		if(alpha)
			CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderTrainHiDetailAlphaCB);
		else
			CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderTrainHiDetailCB);
	}else if(strstr(name, "_vlo"))
		CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle);
	else
		CVisibilityPlugins::SetAtomicRenderCallback(atomic, nil);
	HideDamagedAtomicCB(atomic, nil);
	return atomic;
}

RpAtomic*
CVehicleModelInfo::SetAtomicRendererCB_Boat(RpAtomic *atomic, void *data)
{
	RpClump *clump;
	char *name;

	clump = (RpClump*)data;
	name = GetFrameNodeName(RpAtomicGetFrame(atomic));
	if(strcmp(name, "boat_hi") == 0 || strncmp(name, "extra", 5) == 0)
		CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailCB_Boat);
	else if(strstr(name, "_hi"))
		CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailCB);
	else if(strstr(name, "_lo")){
		RpClumpRemoveAtomic(clump, atomic);
		RpAtomicDestroy(atomic);
		return atomic;		// BUG: not done by gta
	}else if(strstr(name, "_vlo"))
		CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle);
	else
		CVisibilityPlugins::SetAtomicRenderCallback(atomic, nil);
	HideDamagedAtomicCB(atomic, nil);
	return atomic;
}

RpAtomic*
CVehicleModelInfo::SetAtomicRendererCB_Heli(RpAtomic *atomic, void *data)
{
	CVisibilityPlugins::SetAtomicRenderCallback(atomic, nil);
	return atomic;
}

void
CVehicleModelInfo::SetAtomicRenderCallbacks(void)
{
	switch(m_vehicleType){
	case VEHICLE_TYPE_TRAIN:
		RpClumpForAllAtomics(m_clump, SetAtomicRendererCB_Train, nil);
		break;
	case VEHICLE_TYPE_HELI:
		RpClumpForAllAtomics(m_clump, SetAtomicRendererCB_Heli, nil);
		break;
	case VEHICLE_TYPE_PLANE:
		RpClumpForAllAtomics(m_clump, SetAtomicRendererCB_BigVehicle, nil);
		break;
	case VEHICLE_TYPE_BOAT:
		RpClumpForAllAtomics(m_clump, SetAtomicRendererCB_Boat, m_clump);
		break;
	default:
		RpClumpForAllAtomics(m_clump, SetAtomicRendererCB, m_clump);
		break;
	}
}

RwObject*
CVehicleModelInfo::SetAtomicFlagCB(RwObject *object, void *data)
{
	RpAtomic *atomic = (RpAtomic*)object;
	assert(RwObjectGetType(object) == rpATOMIC);
	CVisibilityPlugins::SetAtomicFlag(atomic, (int)data);
	return object;
}

RpAtomic*
CVehicleModelInfo::ClearAtomicFlagCB(RpAtomic *atomic, void *data)
{
	CVisibilityPlugins::ClearAtomicFlag(atomic, (int)data);
	return atomic;
}

RwObject*
GetOkAndDamagedAtomicCB(RwObject *object, void *data)
{
	RpAtomic *atomic = (RpAtomic*)object;
	if(CVisibilityPlugins::GetAtomicId(atomic) & ATOMIC_FLAG_OK)
		((RpAtomic**)data)[0] = atomic;
	else if(CVisibilityPlugins::GetAtomicId(atomic) & ATOMIC_FLAG_DAM)
		((RpAtomic**)data)[1] = atomic;
	return object;
}

void
CVehicleModelInfo::PreprocessHierarchy(void)
{
	int32 i;
	RwObjectNameIdAssocation *desc;
	RwFrame *f;
	RpAtomic *atomic;
	RwV3d *rwvec;

	desc = ms_vehicleDescs[m_vehicleType];
	m_numDoors = 0;
	m_numComps = 0;

	for(i = 0; desc[i].name; i++){
		RwObjectNameAssociation assoc;

		if((desc[i].flags & (VEHICLE_FLAG_COMP|VEHICLE_FLAG_POS)) == 0)
			continue;
		assoc.frame = nil;
		assoc.name = desc[i].name;
		RwFrameForAllChildren(RpClumpGetFrame(m_clump),
			FindFrameFromNameWithoutIdCB, &assoc);
		if(assoc.frame == nil)
			continue;

		if(desc[i].flags & VEHICLE_FLAG_DOOR)
			m_numDoors++;

		if(desc[i].flags & VEHICLE_FLAG_POS){
			f = assoc.frame;
			rwvec = (RwV3d*)&m_positions[desc[i].hierId];
			*rwvec = *RwMatrixGetPos(RwFrameGetMatrix(f));
			for(f = RwFrameGetParent(f); f; f = RwFrameGetParent(f))
				RwV3dTransformPoints(rwvec, rwvec, 1, RwFrameGetMatrix(f));
			RwFrameDestroy(assoc.frame);
		}else{
			atomic = (RpAtomic*)GetFirstObject(assoc.frame);
			RpClumpRemoveAtomic(m_clump, atomic);
			RwFrameRemoveChild(assoc.frame);
			SetVehicleComponentFlags(assoc.frame, desc[i].flags);
			m_comps[m_numComps++] = atomic;
		}
	}

	for(i = 0; desc[i].name; i++){
		RwObjectIdAssociation assoc;

		if(desc[i].flags & (VEHICLE_FLAG_COMP|VEHICLE_FLAG_POS))
			continue;
		assoc.frame = nil;
		assoc.id = desc[i].hierId;
		RwFrameForAllChildren(RpClumpGetFrame(m_clump),
			FindFrameFromIdCB, &assoc);
		if(assoc.frame == nil)
			continue;

		if(desc[i].flags & VEHICLE_FLAG_DOOR)
			m_numDoors++;

		if(desc[i].flags & VEHICLE_FLAG_COLLAPSE){
			RpAtomic *okdam[2] = { nil, nil };
			RwFrameForAllChildren(assoc.frame, CollapseFramesCB, assoc.frame);
			RwFrameUpdateObjects(assoc.frame);
			RwFrameForAllObjects(assoc.frame, GetOkAndDamagedAtomicCB, okdam);
			if(okdam[0] && okdam[1])
				RpAtomicSetRenderCallBack(okdam[1], RpAtomicGetRenderCallBack(okdam[0]));
		}

		SetVehicleComponentFlags(assoc.frame, desc[i].flags);

		if(desc[i].flags & VEHICLE_FLAG_ADD_WHEEL){
			if(m_wheelId == -1)
				RwFrameDestroy(assoc.frame);
			else{
				RwV3d scale;
				atomic = (RpAtomic*)CModelInfo::GetModelInfo(m_wheelId)->CreateInstance();
				RwFrameDestroy(RpAtomicGetFrame(atomic));
				RpAtomicSetFrame(atomic, assoc.frame);
				RpClumpAddAtomic(m_clump, atomic);
				CVisibilityPlugins::SetAtomicRenderCallback(atomic,
					CVisibilityPlugins::RenderWheelAtomicCB);
				scale.x = m_wheelScale;
				scale.y = m_wheelScale;
				scale.z = m_wheelScale;
				RwFrameScale(assoc.frame, &scale, rwCOMBINEPRECONCAT);
			}
		}
	}
}

void
CVehicleModelInfo::SetVehicleComponentFlags(RwFrame *frame, uint32 flags)
{
	tHandlingData *handling;

	handling = mod_HandlingManager.GetHandlingData((eHandlingId)m_handlingId);

#define SETFLAGS(f) RwFrameForAllObjects(frame, SetAtomicFlagCB, (void*)(f))

	if(flags & VEHICLE_FLAG_WINDSCREEN){
		if(this == CModelInfo::GetModelInfo(MI_RHINO))
			return;
		SETFLAGS(ATOMIC_FLAG_WINDSCREEN);
	}

	if(flags & VEHICLE_FLAG_ANGLECULL)
		SETFLAGS(ATOMIC_FLAG_ANGLECULL);

	if(flags & VEHICLE_FLAG_FRONT)
		SETFLAGS(ATOMIC_FLAG_FRONT);
	else if(flags & VEHICLE_FLAG_REAR && (handling->Flags & HANDLING_IS_VAN || (flags & (VEHICLE_FLAG_LEFT|VEHICLE_FLAG_RIGHT)) == 0))
		SETFLAGS(ATOMIC_FLAG_REAR);
	if(flags & VEHICLE_FLAG_LEFT)
		SETFLAGS(ATOMIC_FLAG_LEFT);
	if(flags & VEHICLE_FLAG_RIGHT)
		SETFLAGS(ATOMIC_FLAG_RIGHT);

	if(flags & VEHICLE_FLAG_REARDOOR)
		SETFLAGS(ATOMIC_FLAG_REARDOOR);
	else if(flags & VEHICLE_FLAG_FRONTDOOR)
		SETFLAGS(ATOMIC_FLAG_FRONTDOOR);

	if(flags & VEHICLE_FLAG_DRAWLAST)
		SETFLAGS(ATOMIC_FLAG_DRAWLAST);
}

#define COMPRULE_RULE(comprule) (((comprule) >> 12) & 0xF)
#define COMPRULE_COMPS(comprule) ((comprule) & 0xFFF)
#define COMPRULE_COMPN(comps, n) (((comps) >> 4*(n)) & 0xF)
#define COMPRULE2_RULE(comprule) (((comprule) >> (12+16)) & 0xF)
#define COMPRULE2_COMPS(comprule) ((comprule >> 16) & 0xFFF)
#define COMPRULE2_COMPN(comps, n) (((comps >> 16) >> 4*(n)) & 0xF)

bool
IsValidCompRule(int rule)
{
	if(rule == 2)
		return CWeather::OldWeatherType == WEATHER_RAINY ||
		       CWeather::NewWeatherType == WEATHER_RAINY;
	return true;
}

int32
CountCompsInRule(int comps)
{
	int32 n;
	for(n = 0; comps != 0; comps >>= 4)
		if((comps & 0xF) != 0xF)
			n++;
	return n;
}

int32
ChooseComponent(int32 rule, int32 comps)
{
	int32 n;
	switch(rule){
	// identical cases....
	case 1:
		n = CGeneral::GetRandomNumberInRange(0, CountCompsInRule(comps));
		return COMPRULE_COMPN(comps, n);
	case 2:
		// only valid in rain
		n = CGeneral::GetRandomNumberInRange(0, CountCompsInRule(comps));
		return COMPRULE_COMPN(comps, n);
	}
	return -1;
}

int32
GetListOfComponentsNotUsedByRules(uint32 comprules, int32 numComps, int32 *comps)
{
	int32 i, n;
	int32 unused[6] = { 0, 1, 2, 3, 4, 5 };

	// first comprule
	if(COMPRULE_RULE(comprules) && IsValidCompRule(COMPRULE_RULE(comprules)))
		for(i = 0; i < 3; i++){
			n = COMPRULE_COMPN(comprules, i);
			if(n != 0xF)
				unused[n] = 0xF;
		}
	// second comprule
	comprules >>= 16;
	if(COMPRULE_RULE(comprules) && IsValidCompRule(COMPRULE_RULE(comprules)))
		for(i = 0; i < 3; i++){
			n = COMPRULE_COMPN(comprules, i);
			if(n != 0xF)
				unused[n] = 0xF;
		}

	n = 0;
	for(i = 0; i < numComps; i++)
		if(unused[i] != 0xF)
			comps[n++] = unused[i];
	return n;
}

int32 wheelIds[] = { CAR_WHEEL_LF, CAR_WHEEL_LB, CAR_WHEEL_RF, CAR_WHEEL_RB };

void
CVehicleModelInfo::GetWheelPosn(int32 n, CVector &pos)
{
	RwMatrix *m = RwFrameGetMatrix(GetFrameFromId(m_clump, wheelIds[n]));
	pos.x = RwMatrixGetPos(m)->x;
	pos.y = RwMatrixGetPos(m)->y;
	pos.z = RwMatrixGetPos(m)->z;
}


int32
CVehicleModelInfo::ChooseComponent(void)
{
	int32 comp;
	int32 comps[8];
	int32 n;

	comp = -1;
	if(ms_compsToUse[0] == -2){
		if(COMPRULE_RULE(m_compRules) && IsValidCompRule(COMPRULE_RULE(m_compRules)))
			comp = ::ChooseComponent(COMPRULE_RULE(m_compRules), COMPRULE_COMPS(m_compRules));
		else if(CGeneral::GetRandomNumberInRange(0, 3) < 2){
			n = GetListOfComponentsNotUsedByRules(m_compRules, m_numComps, comps);
			if(n)
				comp = comps[(int)CGeneral::GetRandomNumberInRange(0, n)];
		}
	}else{
		comp = ms_compsToUse[0];
		ms_compsToUse[0] = -2;
	}
	return comp;
}

int32
CVehicleModelInfo::ChooseSecondComponent(void)
{
	int32 comp;
	int32 comps[8];
	int32 n;

	comp = -1;
	if(ms_compsToUse[1] == -2){
		if(COMPRULE2_RULE(m_compRules) && IsValidCompRule(COMPRULE2_RULE(m_compRules)))
			comp = ::ChooseComponent(COMPRULE2_RULE(m_compRules), COMPRULE2_COMPS(m_compRules));
		else if(COMPRULE_RULE(m_compRules) && IsValidCompRule(COMPRULE_RULE(m_compRules)) &&
		        CGeneral::GetRandomNumberInRange(0, 3) < 2){

			n = GetListOfComponentsNotUsedByRules(m_compRules, m_numComps, comps);
			if(n)
				comp = comps[(int)CGeneral::GetRandomNumberInRange(0, n)];
		}
	}else{
		comp = ms_compsToUse[1];
		ms_compsToUse[1] = -2;
	}
	return comp;
}

struct editableMatCBData
{
	CVehicleModelInfo *vehicle;
	int32 numMats1;
	int32 numMats2;
};

RpMaterial*
CVehicleModelInfo::GetEditableMaterialListCB(RpMaterial *material, void *data)
{
	static RwRGBA white = { 255, 255, 255, 255 };
	RwRGBA *col;
	editableMatCBData *cbdata;

	cbdata = (editableMatCBData*)data;
	col = RpMaterialGetColor(material);
	if(col->red == 0x3C && col->green == 0xFF && col->blue == 0){
		cbdata->vehicle->m_materials1[cbdata->numMats1++] = material;
		RpMaterialSetColor(material, &white);
	}else if(col->red == 0xFF && col->green == 0 && col->blue == 0xAF){
		cbdata->vehicle->m_materials2[cbdata->numMats2++] = material;
		RpMaterialSetColor(material, &white);
	}
	return material;
}

RpAtomic*
CVehicleModelInfo::GetEditableMaterialListCB(RpAtomic *atomic, void *data)
{
	RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), GetEditableMaterialListCB, data);
	return atomic;
}

void
CVehicleModelInfo::FindEditableMaterialList(void)
{
	editableMatCBData cbdata;
	int32 i;

	cbdata.vehicle = this;
	cbdata.numMats1 = 0;
	cbdata.numMats2 = 0;
	RpClumpForAllAtomics(m_clump, GetEditableMaterialListCB, &cbdata);
	for(i = 0; i < m_numComps; i++)
		GetEditableMaterialListCB(m_comps[i], &cbdata);
	m_materials1[cbdata.numMats1] = nil;
	m_materials2[cbdata.numMats2] = nil;
	m_currentColour1 = -1;
	m_currentColour2 = -1;
}

void
CVehicleModelInfo::SetVehicleColour(uint8 c1, uint8 c2)
{
	RwRGBA col, *colp;
	RwTexture *coltex;
	RpMaterial **matp;

	if(c1 != m_currentColour1){
		col = ms_vehicleColourTable[c1];
		coltex = ms_colourTextureTable[c1];
		for(matp = m_materials1; *matp; matp++){
			if(RpMaterialGetTexture(*matp) && RpMaterialGetTexture(*matp)->name[0] != '@'){
				colp = RpMaterialGetColor(*matp);
				colp->red = col.red;
				colp->green = col.green;
				colp->blue = col.blue;
			}else
				RpMaterialSetTexture(*matp, coltex);
		}
		m_currentColour1 = c1;
	}

	if(c2 != m_currentColour2){
		col = ms_vehicleColourTable[c2];
		coltex = ms_colourTextureTable[c2];
		for(matp = m_materials2; *matp; matp++){
			if(RpMaterialGetTexture(*matp) && RpMaterialGetTexture(*matp)->name[0] != '@'){
				colp = RpMaterialGetColor(*matp);
				colp->red = col.red;
				colp->green = col.green;
				colp->blue = col.blue;
			}else
				RpMaterialSetTexture(*matp, coltex);
		}
		m_currentColour2 = c2;
	}
}

void
CVehicleModelInfo::ChooseVehicleColour(uint8 &col1, uint8 &col2)
{
	if(m_numColours == 0){
		col1 = 0;
		col2 = 0;
	}else{
		m_lastColorVariation = (m_lastColorVariation+1) % m_numColours;
		col1 = m_colours1[m_lastColorVariation];
		col2 = m_colours2[m_lastColorVariation];
		if(m_numColours > 1){
			CVehicle *veh = FindPlayerVehicle();
			if(veh && CModelInfo::GetModelInfo(veh->GetModelIndex()) == this &&
			   veh->m_currentColour1 == col1 &&
			   veh->m_currentColour2 == col2){
				m_lastColorVariation = (m_lastColorVariation+1) % m_numColours;
				col1 = m_colours1[m_lastColorVariation];
				col2 = m_colours2[m_lastColorVariation];
			}
		}
	}
}

void
CVehicleModelInfo::AvoidSameVehicleColour(uint8 *col1, uint8 *col2)
{
	int i, n;

	if(m_numColours > 1)
		for(i = 0; i < 8; i++){
			if(*col1 != m_lastColour1 || *col2 != m_lastColour2)
				break;
			n = CGeneral::GetRandomNumberInRange(0, m_numColours);
			*col1 = m_colours1[n];
			*col2 = m_colours2[n];
		}
	m_lastColour1 = *col1;
	m_lastColour2 = *col2;
}

RwTexture*
CreateCarColourTexture(uint8 r, uint8 g, uint8 b)
{
	RwImage *img;
	RwRaster *ras;
	RwTexture *tex;
	RwUInt8 *pixels;
	RwInt32 width, height, depth, format;

	img = RwImageCreate(2, 2, 32);
	pixels = (RwUInt8*)RwMalloc(2*2*4);
	pixels[0] = r;
	pixels[1] = g;
	pixels[2] = b;
	pixels[3] = 0xFF;
	pixels[4] = r;
	pixels[5] = g;
	pixels[6] = b;
	pixels[7] = 0xFF;
	pixels[8] = r;
	pixels[9] = g;
	pixels[10] = b;
	pixels[11] = 0xFF;
	pixels[12] = r;
	pixels[13] = g;
	pixels[14] = b;
	pixels[15] = 0xFF;
	RwImageSetPixels(img, pixels);
	RwImageSetStride(img, 8);
	RwImageSetPalette(img, nil);
	RwImageFindRasterFormat(img, rwRASTERTYPETEXTURE, &width, &height, &depth, &format);
	ras = RwRasterCreate(width, height, depth, format);
	RwRasterSetFromImage(ras, img);
	RwImageDestroy(img);
	RwFree(pixels);
	tex = RwTextureCreate(ras);
	tex->name[0] = '@';
	return tex;
}

void
CVehicleModelInfo::LoadVehicleColours(void)
{
	int fd;
	int i;
	char line[1024];
	int start, end;
	int section, numCols;
	enum {
		NONE,
		COLOURS,
		CARS
	};
	int r, g, b;
	char name[64];
	int colors[16];
	int n;

	CFileMgr::ChangeDir("\\DATA\\");
	fd = CFileMgr::OpenFile("CARCOLS.DAT", "r");
	CFileMgr::ChangeDir("\\");

	for(i = 0; i < 256; i++)
		ms_colourTextureTable[i] = nil;

	section = 0;
	numCols = 0;
	while(CFileMgr::ReadLine(fd, line, sizeof(line))){
		// find first valid character in line
		for(start = 0; ; start++)
			if(line[start] > ' ' || line[start] == '\0' || line[start] == '\n')
				break;
		// find end of line
		for(end = start; ; end++){
			if(line[end] == '\0' || line[end] == '\n')
				break;
			if(line[end] == ',' || line[end] == '\r')
				line[end] = ' ';
		}
		line[end] = '\0';

		// empty line
		if(line[start] == '#' || line[start] == '\0')
			continue;

		if(section == NONE){
			if(strncmp(&line[start], "col", 3) == 0)
				section = COLOURS;
			else if(strncmp(&line[start], "car", 3) == 0)
				section = CARS;
		}else if(strncmp(&line[start], "end", 3) == 0){
			section = NONE;
		}else if(section == COLOURS){
			sscanf(&line[start],	// BUG: games doesn't add start
				"%d %d %d", &r, &g, &b);
			ms_vehicleColourTable[numCols].red = r;
			ms_vehicleColourTable[numCols].green = g;
			ms_vehicleColourTable[numCols].blue = b;
			ms_vehicleColourTable[numCols].alpha = 0xFF;
			ms_colourTextureTable[numCols] = CreateCarColourTexture(r, g, b);
			numCols++;
		}else if(section == CARS){
			n = sscanf(&line[start],	// BUG: games doesn't add start
				"%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
				name,
				&colors[0], &colors[1],
				&colors[2], &colors[3],
				&colors[4], &colors[5],
				&colors[6], &colors[7],
				&colors[8], &colors[9],
				&colors[10], &colors[11],
				&colors[12], &colors[13],
				&colors[14], &colors[15]);
			CVehicleModelInfo *mi = (CVehicleModelInfo*)CModelInfo::GetModelInfo(name, nil);
			assert(mi);
			mi->m_numColours = (n-1)/2;
			for(i = 0; i < mi->m_numColours; i++){
				mi->m_colours1[i] = colors[i*2 + 0];
				mi->m_colours2[i] = colors[i*2 + 1];
			}
		}
	}

	CFileMgr::CloseFile(fd);
}

void
CVehicleModelInfo::DeleteVehicleColourTextures(void)
{
	int i;

	for(i = 0; i < 256; i++){
		if(ms_colourTextureTable[i]){
			RwTextureDestroy(ms_colourTextureTable[i]);
			ms_colourTextureTable[i] = nil;
		}
	}
}

RpMaterial*
CVehicleModelInfo::HasSpecularMaterialCB(RpMaterial *material, void *data)
{
	if(RpMaterialGetSurfaceProperties(material)->specular <= 0.0f)
		return material;
	*(bool*)data = true;
	return nil;
}

RpMaterial*
CVehicleModelInfo::SetEnvironmentMapCB(RpMaterial *material, void *data)
{
	float spec;

	spec = RpMaterialGetSurfaceProperties(material)->specular;
	if(spec <= 0.0f)
		RpMatFXMaterialSetEffects(material, rpMATFXEFFECTNULL);
	else{
		if(RpMaterialGetTexture(material) == nil)
			RpMaterialSetTexture(material, gpWhiteTexture);
		RpMatFXMaterialSetEffects(material, rpMATFXEFFECTENVMAP);
#ifndef PS2_MATFX
		spec *= 0.5f;	// Tone down a bit for PC
#endif
		RpMatFXMaterialSetupEnvMap(material, (RwTexture*)data, pMatFxIdentityFrame, false, spec);
	}
	return material;
}

RpAtomic*
CVehicleModelInfo::SetEnvironmentMapCB(RpAtomic *atomic, void *data)
{
	bool hasSpec;
	RpGeometry *geo;

	geo = RpAtomicGetGeometry(atomic);
	hasSpec = 0;
	RpGeometryForAllMaterials(geo, HasSpecularMaterialCB, &hasSpec);
	if(hasSpec){
		RpGeometryForAllMaterials(geo, SetEnvironmentMapCB, data);
		RpGeometrySetFlags(geo, RpGeometryGetFlags(geo) | rpGEOMETRYMODULATEMATERIALCOLOR);
		RpMatFXAtomicEnableEffects(atomic);
		// PS2 sets of PS2Manager lighting CB here
	}
	return atomic;
}

void
CVehicleModelInfo::SetEnvironmentMap(void)
{
	CSimpleModelInfo *wheelmi;
	int32 i;

	if(pMatFxIdentityFrame == nil){
		pMatFxIdentityFrame = RwFrameCreate();
		RwMatrixSetIdentity(RwFrameGetMatrix(pMatFxIdentityFrame));
		RwFrameUpdateObjects(pMatFxIdentityFrame);
		RwFrameGetLTM(pMatFxIdentityFrame);
	}

	if(m_envMap != ms_pEnvironmentMaps[0]){
		m_envMap = ms_pEnvironmentMaps[0];
		RpClumpForAllAtomics(m_clump, SetEnvironmentMapCB, m_envMap);
		if(m_wheelId != -1){
			wheelmi = (CSimpleModelInfo*)CModelInfo::GetModelInfo(m_wheelId);
			for(i = 0; i < wheelmi->m_numAtomics; i++)
				SetEnvironmentMapCB(wheelmi->m_atomics[i], m_envMap);
		}
	}
}

void
CVehicleModelInfo::LoadEnvironmentMaps(void)
{
	char *texnames[] = {
		"reflection01",		// only one used
		"reflection02",
		"reflection03",
		"reflection04",
		"reflection05",
		"reflection06",
	};
	int32 txdslot;
	int32 i;

	txdslot = CTxdStore::FindTxdSlot("particle");
	CTxdStore::PushCurrentTxd();
	CTxdStore::SetCurrentTxd(txdslot);
	for(i = 0; i < NUM_VEHICLE_ENVMAPS; i++){
		ms_pEnvironmentMaps[i] = RwTextureRead(texnames[i], nil);
		RwTextureSetFilterMode(ms_pEnvironmentMaps[i], rwFILTERLINEAR);
	}
	if(gpWhiteTexture == nil){
		gpWhiteTexture = RwTextureRead("white", nil);
		gpWhiteTexture->name[0] = '@';
		RwTextureSetFilterMode(gpWhiteTexture, rwFILTERLINEAR);
	}
	CTxdStore::PopCurrentTxd();
}

void
CVehicleModelInfo::ShutdownEnvironmentMaps(void)
{
	int32 i;

	// ignoring "initialised" as that's a PS2 thing only
	RwTextureDestroy(gpWhiteTexture);
	gpWhiteTexture = nil;
	for(i = 0; i < NUM_VEHICLE_ENVMAPS; i++)
		if(ms_pEnvironmentMaps[i])
			RwTextureDestroy(ms_pEnvironmentMaps[i]);
	RwFrameDestroy(pMatFxIdentityFrame);
	pMatFxIdentityFrame = nil;
}

int
CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors(int id)
{
	int n;

	switch(id){
	case MI_TRAIN:
		n = 3;
		break;
	case MI_FIRETRUCK:
		n = 2;
		break;
	default:
		n = ((CVehicleModelInfo*)CModelInfo::GetModelInfo(id))->m_numDoors;
	}

	if(n == 0)
		return id == MI_RCBANDIT ? 0 : 1;

	if(id == MI_COACH)
		return 8;

	return n - 1;
}

class CVehicleModelInfo_ : public CVehicleModelInfo
{
public:
	void DeleteRwObject_(void) { CVehicleModelInfo::DeleteRwObject(); }
	RwObject *CreateInstance_(void) { return CVehicleModelInfo::CreateInstance(); }
	void SetClump_(RpClump *clump) { CVehicleModelInfo::SetClump(clump); }
};

STARTPATCHES
	InjectHook(0x51FDC0, &CVehicleModelInfo_::DeleteRwObject_, PATCH_JUMP);
	InjectHook(0x51FCB0, &CVehicleModelInfo_::CreateInstance_, PATCH_JUMP);
	InjectHook(0x51FC60, &CVehicleModelInfo_::SetClump_, PATCH_JUMP);

	InjectHook(0x51FE10, &CVehicleModelInfo::CollapseFramesCB, PATCH_JUMP);
	InjectHook(0x51FE50, &CVehicleModelInfo::MoveObjectsCB, PATCH_JUMP);
	InjectHook(0x51FE70, &CVehicleModelInfo::HideDamagedAtomicCB, PATCH_JUMP);
	InjectHook(0x51FED0, &CVehicleModelInfo::HideAllComponentsAtomicCB, PATCH_JUMP);
	InjectHook(0x51FEF0, &CVehicleModelInfo::HasAlphaMaterialCB, PATCH_JUMP);

	InjectHook(0x51FF10, &CVehicleModelInfo::SetAtomicRendererCB, PATCH_JUMP);
	InjectHook(0x520030, &CVehicleModelInfo::SetAtomicRendererCB_BigVehicle, PATCH_JUMP);
	InjectHook(0x520230, &CVehicleModelInfo::SetAtomicRendererCB_Train, PATCH_JUMP);
	InjectHook(0x520120, &CVehicleModelInfo::SetAtomicRendererCB_Boat, PATCH_JUMP);
	InjectHook(0x520210, &CVehicleModelInfo::SetAtomicRendererCB_Heli, PATCH_JUMP);
	InjectHook(0x5202C0, &CVehicleModelInfo::SetAtomicRenderCallbacks, PATCH_JUMP);

	InjectHook(0x520340, &CVehicleModelInfo::SetAtomicFlagCB, PATCH_JUMP);
	InjectHook(0x520360, &CVehicleModelInfo::ClearAtomicFlagCB, PATCH_JUMP);

	InjectHook(0x5204D0, &CVehicleModelInfo::PreprocessHierarchy, PATCH_JUMP);
	InjectHook(0x5203C0, &CVehicleModelInfo::SetVehicleComponentFlags, PATCH_JUMP);

	InjectHook(0x520840, &CVehicleModelInfo::GetWheelPosn, PATCH_JUMP);

	InjectHook(0x520880, IsValidCompRule, PATCH_JUMP);
	InjectHook(0x520990, CountCompsInRule, PATCH_JUMP);
	InjectHook(0x5209C0, ChooseComponent, PATCH_JUMP);
	InjectHook(0x5208C0, GetListOfComponentsNotUsedByRules, PATCH_JUMP);
	InjectHook(0x520AB0, &CVehicleModelInfo::ChooseComponent, PATCH_JUMP);
	InjectHook(0x520BE0, &CVehicleModelInfo::ChooseSecondComponent, PATCH_JUMP);

	InjectHook(0x520DC0, (RpAtomic *(*)(RpAtomic*, void*))CVehicleModelInfo::GetEditableMaterialListCB, PATCH_JUMP);
	InjectHook(0x520D30, (RpMaterial *(*)(RpMaterial*, void*))CVehicleModelInfo::GetEditableMaterialListCB, PATCH_JUMP);
	InjectHook(0x520DE0, &CVehicleModelInfo::FindEditableMaterialList, PATCH_JUMP);
	InjectHook(0x520E70, &CVehicleModelInfo::SetVehicleColour, PATCH_JUMP);
	InjectHook(0x520FD0, &CVehicleModelInfo::ChooseVehicleColour, PATCH_JUMP);
	InjectHook(0x5210A0, &CVehicleModelInfo::AvoidSameVehicleColour, PATCH_JUMP);
	InjectHook(0x521260, &CVehicleModelInfo::LoadVehicleColours, PATCH_JUMP);
	InjectHook(0x521650, &CVehicleModelInfo::DeleteVehicleColourTextures, PATCH_JUMP);

	InjectHook(0x5219D0, &CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors, PATCH_JUMP);

	InjectHook(0x521820, (RpAtomic *(*)(RpAtomic*, void*))CVehicleModelInfo::SetEnvironmentMapCB, PATCH_JUMP);
	InjectHook(0x5217A0, (RpMaterial *(*)(RpMaterial*, void*))CVehicleModelInfo::SetEnvironmentMapCB, PATCH_JUMP);
	InjectHook(0x521770, CVehicleModelInfo::HasSpecularMaterialCB, PATCH_JUMP);
	InjectHook(0x521890, &CVehicleModelInfo::SetEnvironmentMap, PATCH_JUMP);
	InjectHook(0x521680, CVehicleModelInfo::LoadEnvironmentMaps, PATCH_JUMP);
	InjectHook(0x521720, CVehicleModelInfo::ShutdownEnvironmentMaps, PATCH_JUMP);
ENDPATCHES
