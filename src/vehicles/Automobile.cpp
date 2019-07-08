#include "common.h"
#include "patcher.h"
#include "General.h"
#include "ModelIndices.h"
#include "VisibilityPlugins.h"
#include "DMAudio.h"
#include "World.h"
#include "SurfaceTable.h"
#include "HandlingMgr.h"
#include "CarCtrl.h"
#include "Ped.h"
#include "Object.h"
#include "Automobile.h"

bool &CAutomobile::m_sAllTaxiLights = *(bool*)0x95CD21;

WRAPPER CAutomobile* CAutomobile::ctor(int, uint8) { EAXJMP(0x52C6B0); }

CAutomobile::CAutomobile(int mi, uint8 owner)
{
	ctor(mi, owner);
}


void
CAutomobile::SetModelIndex(uint32 id)
{
	CVehicle::SetModelIndex(id);
	SetupModelNodes();
}

WRAPPER void CAutomobile::ProcessControl(void) { EAXJMP(0x531470); }

void
CAutomobile::Teleport(CVector pos)
{
	CWorld::Remove(this);

	GetPosition() = pos;
	SetOrientation(0.0f, 0.0f, 0.0f);
	SetMoveSpeed(0.0f, 0.0f, 0.0f);
	SetTurnSpeed(0.0f, 0.0f, 0.0f);

	ResetSuspension();

	CWorld::Add(this);
}

WRAPPER void CAutomobile::PreRender(void) { EAXJMP(0x535B40); }
WRAPPER void CAutomobile::Render(void) { EAXJMP(0x539EA0); }


WRAPPER void CAutomobile::ProcessControlInputs(uint8) { EAXJMP(0x53B660); }

void
CAutomobile::GetComponentWorldPosition(int32 component, CVector &pos)
{
	if(m_aCarNodes[component] == nil){
		printf("CarNode missing: %d %d\n", GetModelIndex(), component);
		return;
	}
	RwMatrix *ltm = RwFrameGetLTM(m_aCarNodes[component]);
	pos = *RwMatrixGetPos(ltm);
}

bool
CAutomobile::IsComponentPresent(int32 comp)
{
	return m_aCarNodes[comp] != nil;
}

void
CAutomobile::SetComponentRotation(int32 component, CVector rotation)
{
	CMatrix mat(RwFrameGetMatrix(m_aCarNodes[component]));
	CVector pos = mat.GetPosition();
	// BUG: all these set the whole matrix
	mat.SetRotateX(DEGTORAD(rotation.x));
	mat.SetRotateY(DEGTORAD(rotation.y));
	mat.SetRotateZ(DEGTORAD(rotation.z));
	mat.Translate(pos);
	mat.UpdateRW();
}

void
CAutomobile::OpenDoor(int32 component, eDoors door, float openRatio)
{
	CMatrix mat(RwFrameGetMatrix(m_aCarNodes[component]));
	CVector pos = mat.GetPosition();
	float axes[3] = { 0.0f, 0.0f, 0.0f };
	float wasClosed = false;

	if(Doors[door].IsClosed()){
		// enable angle cull for closed doors
		RwFrameForAllObjects(m_aCarNodes[component], CVehicleModelInfo::ClearAtomicFlagCB, (void*)ATOMIC_FLAG_NOCULL);
		wasClosed = true;
	}

	Doors[door].Open(openRatio);

	if(wasClosed && Doors[door].RetAngleWhenClosed() != Doors[door].m_fAngle){
		// door opened
		HideAllComps();
		// turn off angle cull for swinging door
		RwFrameForAllObjects(m_aCarNodes[component], CVehicleModelInfo::SetAtomicFlagCB, (void*)ATOMIC_FLAG_NOCULL);
		DMAudio.PlayOneShot(m_audioEntityId, SOUND_CAR_DOOR_OPEN_BONNET + door, 0.0f);
	}

	if(!wasClosed && openRatio == 0.0f){
		// door closed
		if(Damage.GetDoorStatus(door) == DOOR_STATUS_SWINGING)
			Damage.SetDoorStatus(door, DOOR_STATUS_OK);	// huh?
		ShowAllComps();
		DMAudio.PlayOneShot(m_audioEntityId, SOUND_CAR_DOOR_CLOSE_BONNET + door, 0.0f);
	}

	axes[Doors[door].m_nAxis] = Doors[door].m_fAngle;
	mat.SetRotate(axes[0], axes[1], axes[2]);
	mat.Translate(pos);
	mat.UpdateRW();
}

WRAPPER void CAutomobile::ProcessOpenDoor(uint32, uint32, float) { EAXJMP(0x52E910); }

bool
CAutomobile::IsDoorReady(eDoors door)
{
	if(Doors[door].IsClosed() || IsDoorMissing(door))
		return true;
	int doorflag = 0;
	// TODO: enum?
	switch(door){
	case DOOR_FRONT_LEFT: doorflag = 1; break;
	case DOOR_FRONT_RIGHT: doorflag = 4; break;
	case DOOR_REAR_LEFT: doorflag = 2; break;
	case DOOR_REAR_RIGHT: doorflag = 8; break;
	}
	return (doorflag & m_nGettingInFlags) == 0;
}

bool
CAutomobile::IsDoorFullyOpen(eDoors door)
{
	return Doors[door].IsFullyOpen() || IsDoorMissing(door);
}

bool
CAutomobile::IsDoorClosed(eDoors door)
{
	return !!Doors[door].IsClosed();
}

bool
CAutomobile::IsDoorMissing(eDoors door)
{
	return Damage.GetDoorStatus(door) == DOOR_STATUS_MISSING;
}

void
CAutomobile::RemoveRefsToVehicle(CEntity *ent)
{
	int i;
	for(i = 0; i < 4; i++)
		if(m_aGroundPhysical[i] == ent)
			m_aGroundPhysical[i] = nil;
}

WRAPPER void CAutomobile::BlowUpCar(CEntity *ent) { EAXJMP(0x53BC60); }

bool
CAutomobile::SetUpWheelColModel(CColModel *colModel)
{
	CVehicleModelInfo *mi = (CVehicleModelInfo*)CModelInfo::GetModelInfo(GetModelIndex());
	CColModel *vehColModel = mi->GetColModel();

	colModel->boundingSphere = vehColModel->boundingSphere;
	colModel->boundingBox = vehColModel->boundingBox;

	CMatrix mat;
	mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_LF]));
	colModel->spheres[0].Set(mi->m_wheelScale, mat.GetPosition(), SURFACE_TIRE, CAR_PIECE_WHEEL_LF);
	mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_LB]));
	colModel->spheres[1].Set(mi->m_wheelScale, mat.GetPosition(), SURFACE_TIRE, CAR_PIECE_WHEEL_LR);
	mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_RF]));
	colModel->spheres[2].Set(mi->m_wheelScale, mat.GetPosition(), SURFACE_TIRE, CAR_PIECE_WHEEL_RF);
	mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_RB]));
	colModel->spheres[3].Set(mi->m_wheelScale, mat.GetPosition(), SURFACE_TIRE, CAR_PIECE_WHEEL_RR);

	if(m_aCarNodes[CAR_WHEEL_LM] != nil && m_aCarNodes[CAR_WHEEL_RM] != nil){
		mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_LM]));
		colModel->spheres[4].Set(mi->m_wheelScale, mat.GetPosition(), SURFACE_TIRE, CAR_PIECE_WHEEL_RF);
		mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_RM]));
		colModel->spheres[5].Set(mi->m_wheelScale, mat.GetPosition(), SURFACE_TIRE, CAR_PIECE_WHEEL_RR);
		colModel->numSpheres = 6;
	}else
		colModel->numSpheres = 4;

	return true;
}

// this probably isn't used in III yet
void
CAutomobile::BurstTyre(uint8 wheel)
{
	switch(wheel){
	case CAR_PIECE_WHEEL_LF: wheel = VEHWHEEL_FRONT_LEFT; break;
	case CAR_PIECE_WHEEL_LR: wheel = VEHWHEEL_REAR_LEFT; break;
	case CAR_PIECE_WHEEL_RF: wheel = VEHWHEEL_FRONT_RIGHT; break;
	case CAR_PIECE_WHEEL_RR: wheel = VEHWHEEL_REAR_RIGHT; break;
	}

	int status = Damage.GetWheelStatus(wheel);
	if(status == WHEEL_STATUS_OK){
		Damage.SetWheelStatus(wheel, WHEEL_STATUS_BURST);

		if(m_status == STATUS_SIMPLE){
			m_status = STATUS_PHYSICS;
			CCarCtrl::SwitchVehicleToRealPhysics(this);
		}

		ApplyMoveForce(GetRight() * CGeneral::GetRandomNumberInRange(-0.3f, 0.3f));
		ApplyTurnForce(GetRight() * CGeneral::GetRandomNumberInRange(-0.3f, 0.3f), GetForward());
	}
}

WRAPPER bool CAutomobile::IsRoomForPedToLeaveCar(uint32, CVector *) { EAXJMP(0x53C5B0); }

float
CAutomobile::GetHeightAboveRoad(void)
{
	return m_fHeightAboveRoad;
}

void
CAutomobile::PlayCarHorn(void)
{
	int r;

	if(m_nCarHornTimer != 0)
		return;

	r = CGeneral::GetRandomNumber() & 7;
	if(r < 2){
		m_nCarHornTimer = 45;
	}else if(r < 4){
		if(pDriver)
			pDriver->Say(SOUND_PED_CAR_COLLISION);
		m_nCarHornTimer = 45;
	}else{
		if(pDriver)
			pDriver->Say(SOUND_PED_CAR_COLLISION);
	}
}


void
CAutomobile::ResetSuspension(void)
{
	int i;
	for(i = 0; i < 4; i++){
		m_aSuspensionSpringRatio[i] = 1.0f;
		m_aWheelSkidThing[i] = 0.0f;
		m_aWheelRotation[i] = 0.0f;
		m_aWheelState[i] = 0;	// TODO: enum?
	}
}

void
CAutomobile::ProcessSwingingDoor(int32 component, eDoors door)
{
	if(Damage.GetDoorStatus(door) != DOOR_STATUS_SWINGING)
		return;

	CMatrix mat(RwFrameGetMatrix(m_aCarNodes[component]));
	CVector pos = mat.GetPosition();
	float axes[3] = { 0.0f, 0.0f, 0.0f };

	Doors[door].Process(this);
	axes[Doors[door].m_nAxis] = Doors[door].m_fAngle;
	mat.SetRotate(axes[0], axes[1], axes[2]);
	mat.Translate(pos);
	mat.UpdateRW();
}

void
CAutomobile::Fix(void)
{
	int component;

	Damage.ResetDamageStatus();

	if(m_handling->Flags & HANDLING_NO_DOORS){
		Damage.SetDoorStatus(DOOR_FRONT_LEFT, DOOR_STATUS_MISSING);
		Damage.SetDoorStatus(DOOR_FRONT_RIGHT, DOOR_STATUS_MISSING);
		Damage.SetDoorStatus(DOOR_REAR_LEFT, DOOR_STATUS_MISSING);
		Damage.SetDoorStatus(DOOR_REAR_RIGHT, DOOR_STATUS_MISSING);
	}

	bIsDamaged = false;
	RpClumpForAllAtomics((RpClump*)m_rwObject, CVehicleModelInfo::HideAllComponentsAtomicCB, (void*)ATOMIC_FLAG_DAM);

	for(component = CAR_BUMP_FRONT; component < NUM_CAR_NODES; component++){
		if(m_aCarNodes[component]){
			CMatrix mat(RwFrameGetMatrix(m_aCarNodes[component]));
			mat.SetTranslate(mat.GetPosition());
			mat.UpdateRW();
		}
	}
}

void
CAutomobile::SetupDamageAfterLoad(void)
{
	if(m_aCarNodes[CAR_BUMP_FRONT])
		SetBumperDamage(CAR_BUMP_FRONT, VEHBUMPER_FRONT);
	if(m_aCarNodes[CAR_BONNET])
		SetDoorDamage(CAR_BONNET, DOOR_BONNET);
	if(m_aCarNodes[CAR_BUMP_REAR])
		SetBumperDamage(CAR_BUMP_REAR, VEHBUMPER_REAR);
	if(m_aCarNodes[CAR_BOOT])
		SetDoorDamage(CAR_BOOT, DOOR_BOOT);
	if(m_aCarNodes[CAR_DOOR_LF])
		SetDoorDamage(CAR_DOOR_LF, DOOR_FRONT_LEFT);
	if(m_aCarNodes[CAR_DOOR_RF])
		SetDoorDamage(CAR_DOOR_RF, DOOR_FRONT_RIGHT);
	if(m_aCarNodes[CAR_DOOR_LR])
		SetDoorDamage(CAR_DOOR_LR, DOOR_REAR_LEFT);
	if(m_aCarNodes[CAR_DOOR_RR])
		SetDoorDamage(CAR_DOOR_RR, DOOR_REAR_RIGHT);
	if(m_aCarNodes[CAR_WING_LF])
		SetPanelDamage(CAR_WING_LF, VEHPANEL_FRONT_LEFT);
	if(m_aCarNodes[CAR_WING_RF])
		SetPanelDamage(CAR_WING_RF, VEHPANEL_FRONT_RIGHT);
	if(m_aCarNodes[CAR_WING_LR])
		SetPanelDamage(CAR_WING_LR, VEHPANEL_REAR_LEFT);
	if(m_aCarNodes[CAR_WING_RR])
		SetPanelDamage(CAR_WING_RR, VEHPANEL_REAR_RIGHT);
}

RwObject*
GetCurrentAtomicObjectCB(RwObject *object, void *data)
{
	RpAtomic *atomic = (RpAtomic*)object;
	assert(RwObjectGetType(object) == rpATOMIC);
	if(RpAtomicGetFlags(atomic) & rpATOMICRENDER)
		*(RpAtomic**)data = atomic;
	return object;
}

CColPoint aTempPedColPts[32];	// this name doesn't make any sense

CObject*
CAutomobile::SpawnFlyingComponent(int32 component, uint32 type)
{
	RpAtomic *atomic;
	RwFrame *frame;
	RwMatrix *matrix;
	CObject *obj;

	if(CObject::nNoTempObjects >= NUMTEMPOBJECTS)
		return nil;

	atomic = nil;
	RwFrameForAllObjects(m_aCarNodes[component], GetCurrentAtomicObjectCB, &atomic);
	if(atomic == nil)
		return nil;

	obj = new CObject;
	if(obj == nil)
		return nil;

	if(component == CAR_WINDSCREEN){
		obj->SetModelIndexNoCreate(MI_CAR_BONNET);
	}else switch(type){
	case COMPGROUP_BUMPER:
		obj->SetModelIndexNoCreate(MI_CAR_BUMPER);
		break;
	case COMPGROUP_WHEEL:
		obj->SetModelIndexNoCreate(MI_CAR_WHEEL);
		break;
	case COMPGROUP_DOOR:
		obj->SetModelIndexNoCreate(MI_CAR_DOOR);
		obj->SetCenterOfMass(0.0f, -0.5f, 0.0f);
		break;
	case COMPGROUP_BONNET:
		obj->SetModelIndexNoCreate(MI_CAR_BONNET);
		obj->SetCenterOfMass(0.0f, 0.4f, 0.0f);
		break;
	case COMPGROUP_BOOT:
		obj->SetModelIndexNoCreate(MI_CAR_BOOT);
		obj->SetCenterOfMass(0.0f, -0.3f, 0.0f);
		break;
	case COMPGROUP_PANEL:
	default:
		obj->SetModelIndexNoCreate(MI_CAR_PANEL);
		break;
	}

	// object needs base model
	obj->RefModelInfo(GetModelIndex());

	// create new atomic
	matrix = RwFrameGetLTM(m_aCarNodes[component]);
	frame = RwFrameCreate();
	atomic = RpAtomicClone(atomic);
	*RwFrameGetMatrix(frame) = *matrix;
	RpAtomicSetFrame(atomic, frame);
	CVisibilityPlugins::SetAtomicRenderCallback(atomic, nil);
	obj->AttachToRwObject((RwObject*)atomic);

	// init object
	obj->m_fMass = 10.0f;
	obj->m_fTurnMass = 25.0f;
	obj->m_fAirResistance = 0.97f;
	obj->m_fElasticity = 0.1f;
	obj->m_fBuoyancy = obj->m_fMass*GRAVITY/0.75f;
	obj->ObjectCreatedBy = TEMP_OBJECT;
	obj->bIsStatic = true;
	obj->bIsPickup = false;
	obj->bUseVehicleColours = true;
	obj->m_colour1 = m_currentColour1;
	obj->m_colour2 = m_currentColour2;

	// life time - the more objects the are, the shorter this one will live
	CObject::nNoTempObjects++;
	if(CObject::nNoTempObjects > 20)
		obj->m_nEndOfLifeTime = CTimer::GetTimeInMilliseconds() + 20000/5.0f;
	else if(CObject::nNoTempObjects > 10)
		obj->m_nEndOfLifeTime = CTimer::GetTimeInMilliseconds() + 20000/2.0f;
	else
		obj->m_nEndOfLifeTime = CTimer::GetTimeInMilliseconds() + 20000;

	obj->m_vecMoveSpeed = m_vecMoveSpeed;
	if(obj->m_vecMoveSpeed.z > 0.0f){
		obj->m_vecMoveSpeed.z *= 1.5f;
	}else if(GetUp().z > 0.0f &&
	         (component == COMPGROUP_BONNET || component == COMPGROUP_BOOT || component == CAR_WINDSCREEN)){
		obj->m_vecMoveSpeed.z *= -1.5f;
		obj->m_vecMoveSpeed.z += 0.04f;
	}else{
		obj->m_vecMoveSpeed.z *= 0.25f;
	}
	obj->m_vecMoveSpeed.x *= 0.75f;
	obj->m_vecMoveSpeed.y *= 0.75f;

	obj->m_vecTurnSpeed = m_vecTurnSpeed*2.0f;

	// push component away from car
	CVector dist = obj->GetPosition() - GetPosition();
	dist.Normalise();
	if(component == COMPGROUP_BONNET || component == COMPGROUP_BOOT || component == CAR_WINDSCREEN){
		// push these up some
		dist += GetUp();
		if(GetUp().z > 0.0f){
			// simulate fast upward movement if going fast
			float speed = CVector2D(m_vecMoveSpeed).MagnitudeSqr();
			obj->GetPosition() += GetUp()*speed;
		}
	}
	obj->ApplyMoveForce(dist);

	if(type == COMPGROUP_WHEEL){
		obj->m_fTurnMass = 5.0f;
		obj->m_vecTurnSpeed.x = 0.5f;
		obj->m_fAirResistance = 0.99f;
	}

	if(CCollision::ProcessColModels(obj->GetMatrix(), *CModelInfo::GetModelInfo(obj->GetModelIndex())->GetColModel(),
			this->GetMatrix(), *CModelInfo::GetModelInfo(this->GetModelIndex())->GetColModel(),
			aTempPedColPts, nil, nil) > 0)
		obj->m_pCollidingEntity = this;

	if(bRenderScorched)
		obj->bRenderScorched = true;

	CWorld::Add(obj);

	return obj;
}

CObject*
CAutomobile::RemoveBonnetInPedCollision(void)
{
	CObject *obj;

	if(Damage.GetDoorStatus(DOOR_BONNET) != DOOR_STATUS_SWINGING &&
	   Doors[DOOR_BONNET].RetAngleWhenOpen()*0.4f < Doors[DOOR_BONNET].m_fAngle){
		// BUG? why not COMPGROUP_BONNET?
		obj = SpawnFlyingComponent(CAR_BONNET, COMPGROUP_DOOR);
		// make both doors invisible on car
		SetComponentVisibility(m_aCarNodes[CAR_BONNET], ATOMIC_FLAG_NONE);
		Damage.SetDoorStatus(DOOR_BONNET, DOOR_STATUS_MISSING);
		return obj;
	}
	return nil;
}

void
CAutomobile::SetPanelDamage(int32 component, ePanels panel, bool noFlyingComponents)
{
	int status = Damage.GetPanelStatus(panel);
	if(m_aCarNodes[component] == nil)
		return;
	if(status == PANEL_STATUS_SMASHED1){
		// show damaged part
		SetComponentVisibility(m_aCarNodes[component], ATOMIC_FLAG_DAM);
	}else if(status == PANEL_STATUS_MISSING){
		if(!noFlyingComponents)
			SpawnFlyingComponent(component, COMPGROUP_PANEL);
		// hide both
		SetComponentVisibility(m_aCarNodes[component], ATOMIC_FLAG_NONE);
	}
}

void
CAutomobile::SetBumperDamage(int32 component, ePanels panel, bool noFlyingComponents)
{
	int status = Damage.GetPanelStatus(panel);
	if(m_aCarNodes[component] == nil){
		printf("Trying to damage component %d of %s\n",
			component, CModelInfo::GetModelInfo(GetModelIndex())->GetName());
		return;
	}
	if(status == PANEL_STATUS_SMASHED1){
		// show damaged part
		SetComponentVisibility(m_aCarNodes[component], ATOMIC_FLAG_DAM);
	}else if(status == PANEL_STATUS_MISSING){
		if(!noFlyingComponents)
			SpawnFlyingComponent(component, COMPGROUP_BUMPER);
		// hide both
		SetComponentVisibility(m_aCarNodes[component], ATOMIC_FLAG_NONE);
	}
}

void
CAutomobile::SetDoorDamage(int32 component, eDoors door, bool noFlyingComponents)
{
	int status = Damage.GetDoorStatus(door);
	if(m_aCarNodes[component] == nil){
		printf("Trying to damage component %d of %s\n",
			component, CModelInfo::GetModelInfo(GetModelIndex())->GetName());
		return;
	}

	if(door == DOOR_BOOT && status == DOOR_STATUS_SWINGING && m_handling->Flags & HANDLING_NOSWING_BOOT){
		Damage.SetDoorStatus(DOOR_BOOT, DOOR_STATUS_MISSING);
		status = DOOR_STATUS_MISSING;
	}

	if(status == DOOR_STATUS_SMASHED){
		// show damaged part
		SetComponentVisibility(m_aCarNodes[component], ATOMIC_FLAG_DAM);
	}else if(status == DOOR_STATUS_SWINGING){
		// turn off angle cull for swinging doors
		RwFrameForAllObjects(m_aCarNodes[component], CVehicleModelInfo::SetAtomicFlagCB, (void*)ATOMIC_FLAG_NOCULL);
	}else if(status == DOOR_STATUS_MISSING){
		if(!noFlyingComponents){
			if(door == DOOR_BONNET)
				SpawnFlyingComponent(component, COMPGROUP_BONNET);
			else if(door == DOOR_BOOT)
				SpawnFlyingComponent(component, COMPGROUP_BOOT);
			else
				SpawnFlyingComponent(component, COMPGROUP_DOOR);
		}
		// hide both
		SetComponentVisibility(m_aCarNodes[component], ATOMIC_FLAG_NONE);
	}
}


static RwObject*
SetVehicleAtomicVisibilityCB(RwObject *object, void *data)
{
	uint32 flags = (uint32)(uintptr)data;
	RpAtomic *atomic = (RpAtomic*)object;
	if((CVisibilityPlugins::GetAtomicId(atomic) & (ATOMIC_FLAG_OK|ATOMIC_FLAG_DAM)) == flags)
		RpAtomicSetFlags(atomic, rpATOMICRENDER);
	else
		RpAtomicSetFlags(atomic, 0);
	return object;
}

void
CAutomobile::SetComponentVisibility(RwFrame *frame, uint32 flags)
{
	HideAllComps();
	bIsDamaged = true;
	RwFrameForAllObjects(frame, SetVehicleAtomicVisibilityCB, (void*)flags);
}

void
CAutomobile::SetupModelNodes(void)
{
	int i;
	for(i = 0; i < NUM_CAR_NODES; i++)
		m_aCarNodes[i] = nil;
	CClumpModelInfo::FillFrameArray((RpClump*)m_rwObject, m_aCarNodes);
}

void
CAutomobile::SetTaxiLight(bool light)
{
	bTaxiLight = light;
}

bool
CAutomobile::GetAllWheelsOffGround(void)
{
	return m_nWheelsOnGround == 0;
}

void
CAutomobile::HideAllComps(void)
{
	// empty
}

void
CAutomobile::ShowAllComps(void)
{
	// empty
}

void
CAutomobile::ReduceHornCounter(void)
{
	if(m_nCarHornTimer != 0)
		m_nCarHornTimer--;
}

void
CAutomobile::SetAllTaxiLights(bool set)
{
	m_sAllTaxiLights = set;
}

class CAutomobile_ : public CAutomobile
{
public:
	void dtor() { CAutomobile::~CAutomobile(); }
	void SetModelIndex_(uint32 id) { CAutomobile::SetModelIndex(id); }
	void ProcessControl_(void) { CAutomobile::ProcessControl(); }
	void Teleport_(CVector v) { CAutomobile::Teleport(v); }
	void PreRender_(void) { CAutomobile::PreRender(); }
	void Render_(void) { CAutomobile::Render(); }

	void ProcessControlInputs_(uint8 x) { CAutomobile::ProcessControlInputs(x); }
	void GetComponentWorldPosition_(int32 component, CVector &pos) { CAutomobile::GetComponentWorldPosition(component, pos); }
	bool IsComponentPresent_(int32 component) { return CAutomobile::IsComponentPresent(component); }
	void SetComponentRotation_(int32 component, CVector rotation) { CAutomobile::SetComponentRotation(component, rotation); }
	void OpenDoor_(int32 component, eDoors door, float ratio) { CAutomobile::OpenDoor(component, door, ratio); }
	void ProcessOpenDoor_(uint32 component, uint32 anim, float time) { CAutomobile::ProcessOpenDoor(component, anim, time); }
	bool IsDoorReady_(eDoors door) { return CAutomobile::IsDoorReady(door); }
	bool IsDoorFullyOpen_(eDoors door) { return CAutomobile::IsDoorFullyOpen(door); }
	bool IsDoorClosed_(eDoors door) { return CAutomobile::IsDoorClosed(door); }
	bool IsDoorMissing_(eDoors door) { return CAutomobile::IsDoorMissing(door); }
	void RemoveRefsToVehicle_(CEntity *ent) { CAutomobile::RemoveRefsToVehicle(ent); }
	void BlowUpCar_(CEntity *ent) { CAutomobile::BlowUpCar(ent); }
	bool SetUpWheelColModel_(CColModel *colModel) { return CAutomobile::SetUpWheelColModel(colModel); }
	void BurstTyre_(uint8 tyre) { CAutomobile::BurstTyre(tyre); }
	bool IsRoomForPedToLeaveCar_(uint32 door, CVector *pos) { return CAutomobile::IsRoomForPedToLeaveCar(door, pos); }
	float GetHeightAboveRoad_(void) { return CAutomobile::GetHeightAboveRoad(); }
	void PlayCarHorn_(void) { CAutomobile::PlayCarHorn(); }
};

STARTPATCHES
	InjectHook(0x52D170, &CAutomobile_::dtor, PATCH_JUMP);
	InjectHook(0x52D190, &CAutomobile_::SetModelIndex_, PATCH_JUMP);
	InjectHook(0x535180, &CAutomobile_::Teleport_, PATCH_JUMP);
	InjectHook(0x52E5F0, &CAutomobile_::GetComponentWorldPosition_, PATCH_JUMP);
	InjectHook(0x52E660, &CAutomobile_::IsComponentPresent_, PATCH_JUMP);
	InjectHook(0x52E680, &CAutomobile_::SetComponentRotation_, PATCH_JUMP);
	InjectHook(0x52E750, &CAutomobile_::OpenDoor_, PATCH_JUMP);
	InjectHook(0x52EF10, &CAutomobile_::IsDoorReady_, PATCH_JUMP);
	InjectHook(0x52EF90, &CAutomobile_::IsDoorFullyOpen_, PATCH_JUMP);
	InjectHook(0x52EFD0, &CAutomobile_::IsDoorClosed_, PATCH_JUMP);
	InjectHook(0x52F000, &CAutomobile_::IsDoorMissing_, PATCH_JUMP);
	InjectHook(0x53BF40, &CAutomobile_::RemoveRefsToVehicle_, PATCH_JUMP);
	InjectHook(0x53BF70, &CAutomobile_::SetUpWheelColModel_, PATCH_JUMP);
	InjectHook(0x53C0E0, &CAutomobile_::BurstTyre_, PATCH_JUMP);
	InjectHook(0x437690, &CAutomobile_::GetHeightAboveRoad_, PATCH_JUMP);
	InjectHook(0x53C450, &CAutomobile_::PlayCarHorn_, PATCH_JUMP);
	InjectHook(0x5353A0, &CAutomobile::ResetSuspension, PATCH_JUMP);
	InjectHook(0x535250, &CAutomobile::ProcessSwingingDoor, PATCH_JUMP);
	InjectHook(0x53C240, &CAutomobile::Fix, PATCH_JUMP);
	InjectHook(0x53C310, &CAutomobile::SetupDamageAfterLoad, PATCH_JUMP);
	InjectHook(0x530300, &CAutomobile::SpawnFlyingComponent, PATCH_JUMP);
	InjectHook(0x535320, &CAutomobile::RemoveBonnetInPedCollision, PATCH_JUMP);
	InjectHook(0x5301A0, &CAutomobile::SetPanelDamage, PATCH_JUMP);
	InjectHook(0x530120, &CAutomobile::SetBumperDamage, PATCH_JUMP);
	InjectHook(0x530200, &CAutomobile::SetDoorDamage, PATCH_JUMP);
	InjectHook(0x5300E0, &CAutomobile::SetComponentVisibility, PATCH_JUMP);
	InjectHook(0x52D1B0, &CAutomobile::SetupModelNodes, PATCH_JUMP);
	InjectHook(0x53C420, &CAutomobile::SetTaxiLight, PATCH_JUMP);
	InjectHook(0x53BC40, &CAutomobile::GetAllWheelsOffGround, PATCH_JUMP);
	InjectHook(0x5308C0, &CAutomobile::ReduceHornCounter, PATCH_JUMP);
	InjectHook(0x53C440, &CAutomobile::SetAllTaxiLights, PATCH_JUMP);
ENDPATCHES
