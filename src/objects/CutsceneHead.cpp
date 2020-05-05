#include "common.h"
#include <rpskin.h>

#include "main.h"
#include "RwHelper.h"
#include "RpAnimBlend.h"
#include "AnimBlendClumpData.h"
#include "Bones.h"
#include "Directory.h"
#include "CutsceneMgr.h"
#include "Streaming.h"
#include "CutsceneHead.h"
#include "CdStream.h"


CCutsceneHead::CCutsceneHead(CObject *obj)
{
	RpAtomic *atm;

	assert(RwObjectGetType(obj->m_rwObject) == rpCLUMP);
#ifdef PED_SKIN
	unk1 = 0;
	bIsSkinned = false;
	m_parentObject = (CCutsceneObject*)obj;
	// Hide original head
	if(IsClumpSkinned(obj->GetClump())){
		m_parentObject->SetRenderHead(false);
		bIsSkinned = true;
	}else
#endif
	{
		m_pHeadNode = RpAnimBlendClumpFindFrame((RpClump*)obj->m_rwObject, "Shead")->frame;
		atm = (RpAtomic*)GetFirstObject(m_pHeadNode);
		if(atm){
			assert(RwObjectGetType((RwObject*)atm) == rpATOMIC);
			RpAtomicSetFlags(atm, RpAtomicGetFlags(atm) & ~rpATOMICRENDER);
		}
	}
}

void
CCutsceneHead::CreateRwObject(void)
{
	RpAtomic *atm;

	CEntity::CreateRwObject();
	assert(RwObjectGetType(m_rwObject) == rpCLUMP);
	atm = GetFirstAtomic((RpClump*)m_rwObject);
	RpSkinAtomicSetHAnimHierarchy(atm, RpHAnimFrameGetHierarchy(GetFirstChild(RpClumpGetFrame((RpClump*)m_rwObject))));
}

void
CCutsceneHead::DeleteRwObject(void)
{
	CEntity::DeleteRwObject();
}

void
CCutsceneHead::ProcessControl(void)
{
	RpAtomic *atm;
	RpHAnimHierarchy *hier;

	// android/xbox calls is at the end
	CPhysical::ProcessControl();

#ifdef PED_SKIN
	if(bIsSkinned){
		UpdateRpHAnim();
		UpdateRwFrame();

		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(m_parentObject->GetClump());
		int idx = RpHAnimIDGetIndex(hier, BONE_head);
		RwMatrix *mat = &RpHAnimHierarchyGetMatrixArray(hier)[idx];
		if(RwV3dLength(&mat->pos) > 100.0f){
			m_matrix.SetRotateY(PI/2);
			m_matrix = CMatrix(mat) * m_matrix;
		}
	}else
#endif
	{
		m_matrix.SetRotateY(PI/2);
		m_matrix = CMatrix(RwFrameGetLTM(m_pHeadNode)) * m_matrix;
		UpdateRwFrame();	// android/xbox don't call this
	}

	assert(RwObjectGetType(m_rwObject) == rpCLUMP);
	atm = GetFirstAtomic((RpClump*)m_rwObject);
	hier = RpSkinAtomicGetHAnimHierarchy(atm);
	RpHAnimHierarchyAddAnimTime(hier, CTimer::GetTimeStepNonClipped()/50.0f);
}

void
CCutsceneHead::Render(void)
{
	RpAtomic *atm;

#ifdef PED_SKIN
	if(bIsSkinned){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(m_parentObject->GetClump());
		RpHAnimHierarchyUpdateMatrices(hier);
		int idx = RpHAnimIDGetIndex(hier, BONE_head);
		RwMatrix *mat = &RpHAnimHierarchyGetMatrixArray(hier)[idx];
		if(RwV3dLength(&mat->pos) > 100.0f){
			m_matrix.SetRotateY(PI/2);
			m_matrix = CMatrix(mat) * m_matrix;
		}
		RenderLimb(BONE_Lhand);
		RenderLimb(BONE_Rhand);
	}else
#endif
	{
		m_matrix.SetRotateY(PI/2);
		m_matrix = CMatrix(RwFrameGetLTM(m_pHeadNode)) * m_matrix;
	}

	UpdateRwFrame();

	assert(RwObjectGetType(m_rwObject) == rpCLUMP);
	atm = GetFirstAtomic((RpClump*)m_rwObject);
	RpHAnimHierarchyUpdateMatrices(RpSkinAtomicGetHAnimHierarchy(atm));

	CObject::Render();
}

#ifdef PED_SKIN
void
CCutsceneHead::RenderLimb(int32 bone)
{
	RpAtomic *atomic;
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(m_parentObject->GetClump());
	int idx = RpHAnimIDGetIndex(hier, bone);
	RwMatrix *mats = RpHAnimHierarchyGetMatrixArray(hier);
	CPedModelInfo *mi = (CPedModelInfo *)CModelInfo::GetModelInfo(GetModelIndex());
	switch(bone){
	case BONE_Lhand:
		atomic = mi->getLeftHand();
		break;
	case BONE_Rhand:
		atomic = mi->getRightHand();
		break;
	default:
		return;
	}
	if(atomic){
		RwFrame *frame = RpAtomicGetFrame(atomic);
		RwMatrixTransform(RwFrameGetMatrix(frame), &mats[idx], rwCOMBINEREPLACE);
		RwFrameUpdateObjects(frame);
		RpAtomicRender(atomic);
	}
}
#endif

void
CCutsceneHead::PlayAnimation(const char *animName)
{
	RpAtomic *atm;
	RpHAnimHierarchy *hier;
	RpHAnimAnimation *anim;
	uint32 offset, size;
	RwStream *stream;

	assert(RwObjectGetType(m_rwObject) == rpCLUMP);
	atm = GetFirstAtomic((RpClump*)m_rwObject);
	hier = RpSkinAtomicGetHAnimHierarchy(atm);

	sprintf(gString, "%s.anm", animName);

	if(CCutsceneMgr::ms_pCutsceneDir->FindItem(gString, offset, size)){
		stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, "ANIM\\CUTS.IMG");
		assert(stream);

		CStreaming::MakeSpaceFor(size * CDSTREAM_SECTOR_SIZE);
		CStreaming::ImGonnaUseStreamingMemory();

		RwStreamSkip(stream, offset*2048);
		if(RwStreamFindChunk(stream, rwID_HANIMANIMATION, nil, nil)){
			anim = RpHAnimAnimationStreamRead(stream);
			RpHAnimHierarchySetCurrentAnim(hier, anim);
		}

		CStreaming::IHaveUsedStreamingMemory();

		RwStreamClose(stream, nil);
	}
}
