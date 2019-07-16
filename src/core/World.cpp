#include "common.h"
#include "patcher.h"
#include "Entity.h"
#include "Ped.h"
#include "PlayerPed.h"
#include "Vehicle.h"
#include "Object.h"
#include "Camera.h"
#include "DMAudio.h"
#include "CarCtrl.h"
#include "Garages.h"
#include "TempColModels.h"
#include "World.h"
#include "ModelIndices.h"

CPtrList *CWorld::ms_bigBuildingsList = (CPtrList*)0x6FAB60;
CPtrList &CWorld::ms_listMovingEntityPtrs = *(CPtrList*)0x8F433C;
CSector (*CWorld::ms_aSectors)[NUMSECTORS_X] = (CSector (*)[NUMSECTORS_Y])0x665608;
uint16 &CWorld::ms_nCurrentScanCode = *(uint16*)0x95CC64;
CColPoint &CWorld::ms_testSpherePoint = *(CColPoint*)0x6E64C0;

uint8 &CWorld::PlayerInFocus = *(uint8 *)0x95CD61;
CPlayerInfo *CWorld::Players = (CPlayerInfo *)0x9412F0;
bool &CWorld::bNoMoreCollisionTorque = *(bool*)0x95CDCC;
CEntity *&CWorld::pIgnoreEntity	= *(CEntity**)0x8F6494;
bool &CWorld::bIncludeDeadPeds = *(bool*)0x95CD8F;
bool &CWorld::bSecondShift = *(bool*)0x95CD54;
bool &CWorld::bForceProcessControl = *(bool*)0x95CD6C;
bool &CWorld::bProcessCutsceneOnly = *(bool*)0x95CD8B;

WRAPPER void CWorld::RemoveReferencesToDeletedObject(CEntity*) { EAXJMP(0x4B3BF0); }

void
CWorld::Add(CEntity *ent)
{
	if(ent->IsVehicle() || ent->IsPed())
		DMAudio.SetEntityStatus(((CPhysical*)ent)->m_audioEntityId, true);

	if(ent->bIsBIGBuilding)
		ms_bigBuildingsList[ent->m_level].InsertItem(ent);
	else
		ent->Add();

	if(ent->IsBuilding() || ent->IsDummy())
		return;

	if(!ent->bIsStatic)
		((CPhysical*)ent)->AddToMovingList();
}

void
CWorld::Remove(CEntity *ent)
{
	if(ent->IsVehicle() || ent->IsPed())
		DMAudio.SetEntityStatus(((CPhysical*)ent)->m_audioEntityId, false);

	if(ent->bIsBIGBuilding)
		ms_bigBuildingsList[ent->m_level].RemoveItem(ent);
	else
		ent->Remove();

	if(ent->IsBuilding() || ent->IsDummy())
		return;

	if(!ent->bIsStatic)
		((CPhysical*)ent)->RemoveFromMovingList();
}

void
CWorld::ClearScanCodes(void)
{
	CPtrNode *node;
	for(int i = 0; i < NUMSECTORS_Y; i++)
	for(int j = 0; j < NUMSECTORS_X; j++){
		CSector *s = &ms_aSectors[i][j];
		for(node = s->m_lists[ENTITYLIST_BUILDINGS].first; node; node = node->next)
			((CEntity*)node->item)->m_scanCode = 0;
		for(node = s->m_lists[ENTITYLIST_VEHICLES].first; node; node = node->next)
			((CEntity*)node->item)->m_scanCode = 0;
		for(node = s->m_lists[ENTITYLIST_PEDS].first; node; node = node->next)
			((CEntity*)node->item)->m_scanCode = 0;
		for(node = s->m_lists[ENTITYLIST_OBJECTS].first; node; node = node->next)
			((CEntity*)node->item)->m_scanCode = 0;
		for(node = s->m_lists[ENTITYLIST_DUMMIES].first; node; node = node->next)
			((CEntity*)node->item)->m_scanCode = 0;
	}
}

bool
CWorld::CameraToIgnoreThisObject(CEntity *ent)
{
	if(CGarages::IsModelIndexADoor(ent->GetModelIndex()))
		return false;
	return ((CObject*)ent)->m_bCameraToAvoidThisObject != 1;
}

bool
CWorld::ProcessLineOfSight(const CVector &point1, const CVector &point2, CColPoint &point, CEntity *&entity, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies, bool ignoreSeeThrough, bool ignoreSomeObjects)
{
	int x, xstart, xend;
	int y, ystart, yend;
	int y1, y2;
	float dist;

	AdvanceCurrentScanCode();

	entity = nil;
	dist = 1.0f;

	xstart = GetSectorIndexX(point1.x);
	ystart = GetSectorIndexX(point1.y);
	xend = GetSectorIndexX(point2.x);
	yend = GetSectorIndexX(point2.y);

#define LOSARGS CColLine(point1, point2), point, dist, entity, checkBuildings, checkVehicles, checkPeds, checkObjects, checkDummies, ignoreSeeThrough, ignoreSomeObjects

	if(xstart == xend && ystart == yend){
		// Only one sector
		return ProcessLineOfSightSector(*GetSector(xstart, ystart), LOSARGS);
	}else if(xstart == xend){
		// Only step in y
		if(ystart < yend)
			for(y = ystart; y <= yend; y++)
				ProcessLineOfSightSector(*GetSector(xstart, y), LOSARGS);
		else
			for(y = ystart; y >= yend; y--)
				ProcessLineOfSightSector(*GetSector(xstart, y), LOSARGS);
		return dist < 1.0f;
	}else if(ystart == yend){
		// Only step in x
		if(xstart < xend)
			for(x = xstart; x <= xend; x++)
				ProcessLineOfSightSector(*GetSector(x, ystart), LOSARGS);
		else
			for(x = xstart; x >= xend; x--)
				ProcessLineOfSightSector(*GetSector(x, ystart), LOSARGS);
		return dist < 1.0f;
	}else{
		if(point1.x < point2.x){
			// Step from left to right
			float m = (point2.y - point1.y) / (point2.x - point1.x);

			y1 = ystart;
			y2 = GetSectorIndexY((GetWorldX(xstart+1) - point1.x)*m + point1.y);
			if(y1 < y2)
				for(y = y1; y <= y2; y++)
					ProcessLineOfSightSector(*GetSector(xstart, y), LOSARGS);
			else
				for(y = y1; y >= y2; y--)
					ProcessLineOfSightSector(*GetSector(xstart, y), LOSARGS);

			for(x = xstart+1; x < xend; x++){
				y1 = y2;
				y2 = GetSectorIndexY((GetWorldX(x+1) - point1.x)*m + point1.y);
				if(y1 < y2)
					for(y = y1; y <= y2; y++)
						ProcessLineOfSightSector(*GetSector(x, y), LOSARGS);
				else
					for(y = y1; y >= y2; y--)
						ProcessLineOfSightSector(*GetSector(x, y), LOSARGS);
			}

			y1 = y2;
			y2 = yend;
			if(y1 < y2)
				for(y = y1; y <= y2; y++)
					ProcessLineOfSightSector(*GetSector(xend, y), LOSARGS);
			else
				for(y = y1; y >= y2; y--)
					ProcessLineOfSightSector(*GetSector(xend, y), LOSARGS);
		}else{
			// Step from right to left
			float m = (point2.y - point1.y) / (point2.x - point1.x);

			y1 = ystart;
			y2 = GetSectorIndexY((GetWorldX(xstart) - point1.x)*m + point1.y);
			if(y1 < y2)
				for(y = y1; y <= y2; y++)
					ProcessLineOfSightSector(*GetSector(xstart, y), LOSARGS);
			else
				for(y = y1; y >= y2; y--)
					ProcessLineOfSightSector(*GetSector(xstart, y), LOSARGS);

			for(x = xstart-1; x > xend; x--){
				y1 = y2;
				y2 = GetSectorIndexY((GetWorldX(x) - point1.x)*m + point1.y);
				if(y1 < y2)
					for(y = y1; y <= y2; y++)
						ProcessLineOfSightSector(*GetSector(x, y), LOSARGS);
				else
					for(y = y1; y >= y2; y--)
						ProcessLineOfSightSector(*GetSector(x, y), LOSARGS);
			}

			y1 = y2;
			y2 = yend;
			if(y1 < y2)
				for(y = y1; y <= y2; y++)
					ProcessLineOfSightSector(*GetSector(xend, y), LOSARGS);
			else
				for(y = y1; y >= y2; y--)
					ProcessLineOfSightSector(*GetSector(xend, y), LOSARGS);
		}
		return dist < 1.0f;
	}

#undef LOSARGS
}

bool
CWorld::ProcessLineOfSightSector(CSector &sector, const CColLine &line, CColPoint &point, float &dist, CEntity *&entity, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies, bool ignoreSeeThrough, bool ignoreSomeObjects)
{
	float mindist = dist;
	bool deadPeds = !!bIncludeDeadPeds;
	bIncludeDeadPeds = false;

	if(checkBuildings){
		ProcessLineOfSightSectorList(sector.m_lists[ENTITYLIST_BUILDINGS], line, point, mindist, entity, ignoreSeeThrough);
		ProcessLineOfSightSectorList(sector.m_lists[ENTITYLIST_BUILDINGS_OVERLAP], line, point, mindist, entity, ignoreSeeThrough);
	}

	if(checkVehicles){
		ProcessLineOfSightSectorList(sector.m_lists[ENTITYLIST_VEHICLES], line, point, mindist, entity, ignoreSeeThrough);
		ProcessLineOfSightSectorList(sector.m_lists[ENTITYLIST_VEHICLES_OVERLAP], line, point, mindist, entity, ignoreSeeThrough);
	}

	if(checkPeds){
		if(deadPeds)
			bIncludeDeadPeds = true;
		ProcessLineOfSightSectorList(sector.m_lists[ENTITYLIST_PEDS], line, point, mindist, entity, ignoreSeeThrough);
		ProcessLineOfSightSectorList(sector.m_lists[ENTITYLIST_PEDS_OVERLAP], line, point, mindist, entity, ignoreSeeThrough);
		bIncludeDeadPeds = false;
	}

	if(checkObjects){
		ProcessLineOfSightSectorList(sector.m_lists[ENTITYLIST_OBJECTS], line, point, mindist, entity, ignoreSeeThrough, ignoreSomeObjects);
		ProcessLineOfSightSectorList(sector.m_lists[ENTITYLIST_OBJECTS_OVERLAP], line, point, mindist, entity, ignoreSeeThrough, ignoreSomeObjects);
	}

	if(checkDummies){
		ProcessLineOfSightSectorList(sector.m_lists[ENTITYLIST_DUMMIES], line, point, mindist, entity, ignoreSeeThrough);
		ProcessLineOfSightSectorList(sector.m_lists[ENTITYLIST_DUMMIES_OVERLAP], line, point, mindist, entity, ignoreSeeThrough);
	}

	bIncludeDeadPeds = deadPeds;

	if(mindist < dist){
		dist = mindist;
		return true;
	}else
		return false;
}

bool
CWorld::ProcessLineOfSightSectorList(CPtrList &list, const CColLine &line, CColPoint &point, float &dist, CEntity *&entity, bool ignoreSeeThrough, bool ignoreSomeObjects)
{
	bool deadPeds = false;
	float mindist = dist;
	CPtrNode *node;
	CEntity *e;
	CColModel *colmodel;

	if(list.first && bIncludeDeadPeds && ((CEntity*)list.first->item)->IsPed())
		deadPeds = true;

	for(node = list.first; node; node = node->next){
		e = (CEntity*)node->item;
		if(e->m_scanCode != GetCurrentScanCode() &&
		   e != pIgnoreEntity &&
		   (e->bUsesCollision || deadPeds) &&
		   !(ignoreSomeObjects && CameraToIgnoreThisObject(e))){
			colmodel = nil;
			e->m_scanCode = GetCurrentScanCode();

			if(e->IsPed()){
				if(e->bUsesCollision ||
				   deadPeds && ((CPed*)e)->m_nPedState == PED_DEAD){
					if(((CPed*)e)->UseGroundColModel())
						colmodel = &CTempColModels::ms_colModelPedGroundHit;
					else
						colmodel = ((CPedModelInfo*)CModelInfo::GetModelInfo(e->GetModelIndex()))->GetHitColModel();
				}else
					colmodel = nil;
			}else if(e->bUsesCollision)
				colmodel = CModelInfo::GetModelInfo(e->GetModelIndex())->GetColModel();

			if(colmodel &&
			   CCollision::ProcessLineOfSight(line, e->GetMatrix(), *colmodel, point, dist, ignoreSeeThrough))
				entity = e;
		}
	}

	if(mindist < dist){
		dist = mindist;
		return true;
	}else
		return false;
}

bool 
CWorld::ProcessVerticalLine(const CVector &point1, float z2, CColPoint &point, CEntity *&entity, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies, bool ignoreSeeThrough, CStoredCollPoly *poly)
{
	AdvanceCurrentScanCode();
	CVector point2(point1.x, point1.y, z2);
	return CWorld::ProcessVerticalLineSector(*GetSector(GetSectorIndexX(point1.x), GetSectorIndexX(point1.y)),
		CColLine(point1, point2), point, entity,
		checkBuildings, checkVehicles, checkPeds, checkObjects, checkDummies, ignoreSeeThrough, poly);
}

bool
CWorld::ProcessVerticalLineSector(CSector &sector, const CColLine &line, CColPoint &point, CEntity *&entity, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies, bool ignoreSeeThrough, CStoredCollPoly *poly)
{
	float mindist = 1.0f;

	if(checkBuildings){
		ProcessVerticalLineSectorList(sector.m_lists[ENTITYLIST_BUILDINGS], line, point, mindist, entity, ignoreSeeThrough, poly);
		ProcessVerticalLineSectorList(sector.m_lists[ENTITYLIST_BUILDINGS_OVERLAP], line, point, mindist, entity, ignoreSeeThrough, poly);
	}

	if(checkVehicles){
		ProcessVerticalLineSectorList(sector.m_lists[ENTITYLIST_VEHICLES], line, point, mindist, entity, ignoreSeeThrough, poly);
		ProcessVerticalLineSectorList(sector.m_lists[ENTITYLIST_VEHICLES_OVERLAP], line, point, mindist, entity, ignoreSeeThrough, poly);
	}

	if(checkPeds){
		ProcessVerticalLineSectorList(sector.m_lists[ENTITYLIST_PEDS], line, point, mindist, entity, ignoreSeeThrough, poly);
		ProcessVerticalLineSectorList(sector.m_lists[ENTITYLIST_PEDS_OVERLAP], line, point, mindist, entity, ignoreSeeThrough, poly);
	}

	if(checkObjects){
		ProcessVerticalLineSectorList(sector.m_lists[ENTITYLIST_OBJECTS], line, point, mindist, entity, ignoreSeeThrough, poly);
		ProcessVerticalLineSectorList(sector.m_lists[ENTITYLIST_OBJECTS_OVERLAP], line, point, mindist, entity, ignoreSeeThrough, poly);
	}

	if(checkDummies){
		ProcessVerticalLineSectorList(sector.m_lists[ENTITYLIST_DUMMIES], line, point, mindist, entity, ignoreSeeThrough, poly);
		ProcessVerticalLineSectorList(sector.m_lists[ENTITYLIST_DUMMIES_OVERLAP], line, point, mindist, entity, ignoreSeeThrough, poly);
	}

	return mindist < 1.0f;
}

bool
CWorld::ProcessVerticalLineSectorList(CPtrList &list, const CColLine &line, CColPoint &point, float &dist, CEntity *&entity, bool ignoreSeeThrough, CStoredCollPoly *poly)
{
	float mindist = dist;
	CPtrNode *node;
	CEntity *e;
	CColModel *colmodel;

	for(node = list.first; node; node = node->next){
		e = (CEntity*)node->item;
		if(e->m_scanCode != GetCurrentScanCode() &&
		   e->bUsesCollision){
			e->m_scanCode = GetCurrentScanCode();

			colmodel = CModelInfo::GetModelInfo(e->GetModelIndex())->GetColModel();
			if(CCollision::ProcessVerticalLine(line, e->GetMatrix(), *colmodel, point, dist, ignoreSeeThrough, poly))
				entity = e;
		}
	}

	if(mindist < dist){
		dist = mindist;
		return true;
	}else
		return false;
}

bool
CWorld::GetIsLineOfSightClear(const CVector &point1, const CVector &point2, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies, bool ignoreSeeThrough, bool ignoreSomeObjects)
{
	int x, xstart, xend;
	int y, ystart, yend;
	int y1, y2;

	AdvanceCurrentScanCode();

	xstart = GetSectorIndexX(point1.x);
	ystart = GetSectorIndexX(point1.y);
	xend = GetSectorIndexX(point2.x);
	yend = GetSectorIndexX(point2.y);

#define LOSARGS CColLine(point1, point2), checkBuildings, checkVehicles, checkPeds, checkObjects, checkDummies, ignoreSeeThrough, ignoreSomeObjects

	if(xstart == xend && ystart == yend){
		// Only one sector
		return GetIsLineOfSightSectorClear(*GetSector(xstart, ystart), LOSARGS);
	}else if(xstart == xend){
		// Only step in y
		if(ystart < yend){
			for(y = ystart; y <= yend; y++)
				if(!GetIsLineOfSightSectorClear(*GetSector(xstart, y), LOSARGS))
					return false;
		}else{
			for(y = ystart; y >= yend; y--)
				if(!GetIsLineOfSightSectorClear(*GetSector(xstart, y), LOSARGS))
					return false;
		}
	}else if(ystart == yend){
		// Only step in x
		if(xstart < xend){
			for(x = xstart; x <= xend; x++)
				if(!GetIsLineOfSightSectorClear(*GetSector(x, ystart), LOSARGS))
					return false;
		}else{
			for(x = xstart; x >= xend; x--)
				if(!GetIsLineOfSightSectorClear(*GetSector(x, ystart), LOSARGS))
					return false;
		}
	}else{
		if(point1.x < point2.x){
			// Step from left to right
			float m = (point2.y - point1.y) / (point2.x - point1.x);

			y1 = ystart;
			y2 = GetSectorIndexY((GetWorldX(xstart+1) - point1.x)*m + point1.y);
			if(y1 < y2){
				for(y = y1; y <= y2; y++)
					if(!GetIsLineOfSightSectorClear(*GetSector(xstart, y), LOSARGS))
						return false;
			}else{
				for(y = y1; y >= y2; y--)
					if(!GetIsLineOfSightSectorClear(*GetSector(xstart, y), LOSARGS))
						return false;
			}

			for(x = xstart+1; x < xend; x++){
				y1 = y2;
				y2 = GetSectorIndexY((GetWorldX(x+1) - point1.x)*m + point1.y);
				if(y1 < y2){
					for(y = y1; y <= y2; y++)
						if(!GetIsLineOfSightSectorClear(*GetSector(x, y), LOSARGS))
							return false;
				}else{
					for(y = y1; y >= y2; y--)
						if(!GetIsLineOfSightSectorClear(*GetSector(x, y), LOSARGS))
							return false;
				}
			}

			y1 = y2;
			y2 = yend;
			if(y1 < y2){
				for(y = y1; y <= y2; y++)
					if(!GetIsLineOfSightSectorClear(*GetSector(xend, y), LOSARGS))
						return false;
			}else{
				for(y = y1; y >= y2; y--)
					if(!GetIsLineOfSightSectorClear(*GetSector(xend, y), LOSARGS))
						return false;
			}
		}else{
			// Step from right to left
			float m = (point2.y - point1.y) / (point2.x - point1.x);

			y1 = ystart;
			y2 = GetSectorIndexY((GetWorldX(xstart) - point1.x)*m + point1.y);
			if(y1 < y2){
				for(y = y1; y <= y2; y++)
					if(!GetIsLineOfSightSectorClear(*GetSector(xstart, y), LOSARGS))
						return false;
			}else{
				for(y = y1; y >= y2; y--)
					if(!GetIsLineOfSightSectorClear(*GetSector(xstart, y), LOSARGS))
						return false;
			}

			for(x = xstart-1; x > xend; x--){
				y1 = y2;
				y2 = GetSectorIndexY((GetWorldX(x) - point1.x)*m + point1.y);
				if(y1 < y2){
					for(y = y1; y <= y2; y++)
						if(!GetIsLineOfSightSectorClear(*GetSector(x, y), LOSARGS))
							return false;
				}else{
					for(y = y1; y >= y2; y--)
						if(!GetIsLineOfSightSectorClear(*GetSector(x, y), LOSARGS))
							return false;
				}
			}

			y1 = y2;
			y2 = yend;
			if(y1 < y2){
				for(y = y1; y <= y2; y++)
					if(!GetIsLineOfSightSectorClear(*GetSector(xend, y), LOSARGS))
						return false;
			}else{
				for(y = y1; y >= y2; y--)
					if(!GetIsLineOfSightSectorClear(*GetSector(xend, y), LOSARGS))
						return false;
			}
		}
	}

	return true;

#undef LOSARGS
}

bool
CWorld::GetIsLineOfSightSectorClear(CSector &sector, const CColLine &line, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies, bool ignoreSeeThrough, bool ignoreSomeObjects)
{
	if(checkBuildings){
		if(!GetIsLineOfSightSectorListClear(sector.m_lists[ENTITYLIST_BUILDINGS], line, ignoreSeeThrough))
			return false;
		if(!GetIsLineOfSightSectorListClear(sector.m_lists[ENTITYLIST_BUILDINGS_OVERLAP], line, ignoreSeeThrough))
			return false;
	}

	if(checkVehicles){
		if(!GetIsLineOfSightSectorListClear(sector.m_lists[ENTITYLIST_VEHICLES], line, ignoreSeeThrough))
			return false;
		if(!GetIsLineOfSightSectorListClear(sector.m_lists[ENTITYLIST_VEHICLES_OVERLAP], line, ignoreSeeThrough))
			return false;
	}

	if(checkPeds){
		if(!GetIsLineOfSightSectorListClear(sector.m_lists[ENTITYLIST_PEDS], line, ignoreSeeThrough))
			return false;
		if(!GetIsLineOfSightSectorListClear(sector.m_lists[ENTITYLIST_PEDS_OVERLAP], line, ignoreSeeThrough))
			return false;
	}

	if(checkObjects){
		if(!GetIsLineOfSightSectorListClear(sector.m_lists[ENTITYLIST_OBJECTS], line, ignoreSeeThrough, ignoreSomeObjects))
			return false;
		if(!GetIsLineOfSightSectorListClear(sector.m_lists[ENTITYLIST_OBJECTS_OVERLAP], line, ignoreSeeThrough, ignoreSomeObjects))
			return false;
	}

	if(checkDummies){
		if(!GetIsLineOfSightSectorListClear(sector.m_lists[ENTITYLIST_DUMMIES], line, ignoreSeeThrough))
			return false;
		if(!GetIsLineOfSightSectorListClear(sector.m_lists[ENTITYLIST_DUMMIES_OVERLAP], line, ignoreSeeThrough))
			return false;
	}

	return true;
}

bool
CWorld::GetIsLineOfSightSectorListClear(CPtrList &list, const CColLine &line, bool ignoreSeeThrough, bool ignoreSomeObjects)
{
	CPtrNode *node;
	CEntity *e;
	CColModel *colmodel;

	for(node = list.first; node; node = node->next){
		e = (CEntity*)node->item;
		if(e->m_scanCode != GetCurrentScanCode() &&
		   e->bUsesCollision){

			e->m_scanCode = GetCurrentScanCode();

			if(e != pIgnoreEntity &&
			   !(ignoreSomeObjects && CameraToIgnoreThisObject(e))){

				colmodel = CModelInfo::GetModelInfo(e->GetModelIndex())->GetColModel();

				if(CCollision::TestLineOfSight(line, e->GetMatrix(), *colmodel, ignoreSeeThrough))
					return false;
			}
		}
	}

	return true;
}

void
CWorld::FindObjectsInRangeSectorList(CPtrList &list, CVector &centre, float distance, bool ignoreZ, short *nextObject, short lastObject, CEntity **objects)
{
	float distSqr = distance * distance;
	float objDistSqr;

	for (CPtrNode *node = list.first; node; node = node->next) {
		CEntity *object = (CEntity*)node->item;
		if (object->m_scanCode != CWorld::GetCurrentScanCode()) {
			object->m_scanCode = CWorld::GetCurrentScanCode();

			CVector diff = centre - object->GetPosition();
			if (ignoreZ)
				objDistSqr = diff.MagnitudeSqr2D();
			else
				objDistSqr = diff.MagnitudeSqr();

			if (objDistSqr < distSqr && *nextObject < lastObject) {
				if (objects) {
					objects[*nextObject] = object;
				}
				(*nextObject)++;
			}
		}
	}
}

void
CWorld::FindObjectsInRange(CVector &centre, float distance, bool ignoreZ, short *nextObject, short lastObject, CEntity **objects, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies)
{
	int minX = GetSectorIndexX(centre.x - distance);
	if (minX <= 0)
		minX = 0;

	int minY = GetSectorIndexY(centre.y - distance);
	if (minY <= 0)
		minY = 0;

	int maxX = GetSectorIndexX(centre.x + distance);
	if (maxX >= 100)
		maxX = 100;

	int maxY = GetSectorIndexY(centre.y + distance);
	if (maxY >= 100)
		maxY = 100;
	
	AdvanceCurrentScanCode();

	*nextObject = 0;
	for(int curY = minY; curY <= maxY; curY++) {
		for(int curX = minX; curX <= maxX; curX++) {
			CSector *sector = GetSector(curX, curY);
			if (checkBuildings) {
				FindObjectsInRangeSectorList(sector->m_lists[ENTITYLIST_BUILDINGS], centre, distance, ignoreZ, nextObject, lastObject, objects);
				FindObjectsInRangeSectorList(sector->m_lists[ENTITYLIST_BUILDINGS_OVERLAP], centre, distance, ignoreZ, nextObject, lastObject, objects);
			}
			if (checkVehicles) {
				FindObjectsInRangeSectorList(sector->m_lists[ENTITYLIST_VEHICLES], centre, distance, ignoreZ, nextObject, lastObject, objects);
				FindObjectsInRangeSectorList(sector->m_lists[ENTITYLIST_VEHICLES_OVERLAP], centre, distance, ignoreZ, nextObject, lastObject, objects);
			}
			if (checkPeds) {
				FindObjectsInRangeSectorList(sector->m_lists[ENTITYLIST_PEDS], centre, distance, ignoreZ, nextObject, lastObject, objects);
				FindObjectsInRangeSectorList(sector->m_lists[ENTITYLIST_PEDS_OVERLAP], centre, distance, ignoreZ, nextObject, lastObject, objects);
			}
			if (checkObjects) {
				FindObjectsInRangeSectorList(sector->m_lists[ENTITYLIST_OBJECTS], centre, distance, ignoreZ, nextObject, lastObject, objects);
				FindObjectsInRangeSectorList(sector->m_lists[ENTITYLIST_OBJECTS_OVERLAP], centre, distance, ignoreZ, nextObject, lastObject, objects);
			}
			if (checkDummies) {
				FindObjectsInRangeSectorList(sector->m_lists[ENTITYLIST_DUMMIES], centre, distance, ignoreZ, nextObject, lastObject, objects);
				FindObjectsInRangeSectorList(sector->m_lists[ENTITYLIST_DUMMIES_OVERLAP], centre, distance, ignoreZ, nextObject, lastObject, objects);
			}
		}
	}
}

CEntity*
CWorld::TestSphereAgainstWorld(CVector centre, float distance, CEntity* entityToIgnore, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies, bool ignoreSomeObjects)
{
	CEntity* foundE = nil;

	int minX = GetSectorIndexX(centre.x - distance);
	if (minX <= 0)
		minX = 0;

	int minY = GetSectorIndexY(centre.y - distance);
	if (minY <= 0)
		minY = 0;

	int maxX = GetSectorIndexX(centre.x + distance);
	if (maxX >= 100)
		maxX = 100;

	int maxY = GetSectorIndexY(centre.y + distance);
	if (maxY >= 100)
		maxY = 100;

	AdvanceCurrentScanCode();

	for (int curY = minY; curY <= maxY; curY++) {
		for (int curX = minX; curX <= maxX; curX++) {
			CSector* sector = GetSector(curX, curY);
			if (checkBuildings) {
				foundE = TestSphereAgainstSectorList(sector->m_lists[ENTITYLIST_BUILDINGS], centre, distance, entityToIgnore, false);
				if (foundE)
					return foundE;

				foundE = TestSphereAgainstSectorList(sector->m_lists[ENTITYLIST_BUILDINGS_OVERLAP], centre, distance, entityToIgnore, false);
				if (foundE)
					return foundE;
			}
			if (checkVehicles) {
				foundE = TestSphereAgainstSectorList(sector->m_lists[ENTITYLIST_VEHICLES], centre, distance, entityToIgnore, false);
				if (foundE)
					return foundE;

				foundE = TestSphereAgainstSectorList(sector->m_lists[ENTITYLIST_VEHICLES_OVERLAP], centre, distance, entityToIgnore, false);
				if (foundE)
					return foundE;
			}
			if (checkPeds) {
				foundE = TestSphereAgainstSectorList(sector->m_lists[ENTITYLIST_PEDS], centre, distance, entityToIgnore, false);
				if (foundE)
					return foundE;

				foundE = TestSphereAgainstSectorList(sector->m_lists[ENTITYLIST_PEDS_OVERLAP], centre, distance, entityToIgnore, false);
				if (foundE)
					return foundE;
			}
			if (checkObjects) {
				foundE = TestSphereAgainstSectorList(sector->m_lists[ENTITYLIST_OBJECTS], centre, distance, entityToIgnore, ignoreSomeObjects);
				if (foundE)
					return foundE;

				foundE = TestSphereAgainstSectorList(sector->m_lists[ENTITYLIST_OBJECTS_OVERLAP], centre, distance, entityToIgnore, ignoreSomeObjects);
				if (foundE)
					return foundE;
			}
			if (checkDummies) {
				foundE = TestSphereAgainstSectorList(sector->m_lists[ENTITYLIST_DUMMIES], centre, distance, entityToIgnore, false);
				if (foundE)
					return foundE;

				foundE = TestSphereAgainstSectorList(sector->m_lists[ENTITYLIST_DUMMIES_OVERLAP], centre, distance, entityToIgnore, false);
				if (foundE)
					return foundE;
			}
		}
	}
	return foundE;
}

CEntity*
CWorld::TestSphereAgainstSectorList(CPtrList &list, CVector spherePos, float radius, CEntity *entityToIgnore, bool ignoreSomeObjects)
{
	static CColModel sphereCol;

	sphereCol.boundingSphere.center.x = 0.0f;
	sphereCol.boundingSphere.center.y = 0.0f;
	sphereCol.boundingSphere.center.z = 0.0f;
	sphereCol.boundingSphere.radius = radius;
	sphereCol.boundingBox.min.x = -radius;
	sphereCol.boundingBox.min.y = -radius;
	sphereCol.boundingBox.min.z = -radius;
	sphereCol.boundingBox.max.x = radius;
	sphereCol.boundingBox.max.y = radius;
	sphereCol.boundingBox.max.z = radius;
	sphereCol.numSpheres = 1;
	sphereCol.spheres = &sphereCol.boundingSphere;
	sphereCol.numLines = 0;
	sphereCol.numBoxes = 0;
	sphereCol.numTriangles = 0;
	sphereCol.ownsCollisionVolumes = false;

	CMatrix sphereMat;
	sphereMat.SetTranslate(spherePos);

	for(CPtrNode *node=list.first; node; node = node->next) {
		CEntity *e = (CEntity*)node->item;

		if (e->m_scanCode != GetCurrentScanCode()) {
			e->m_scanCode = GetCurrentScanCode();

			if (e != entityToIgnore && e->bUsesCollision && !(ignoreSomeObjects && CameraToIgnoreThisObject(e))) {
				CVector diff = spherePos - e->GetPosition();
				float distance = diff.Magnitude();

				if (e->GetBoundRadius() + radius > distance) {
					CColModel *eCol = CModelInfo::GetModelInfo(e->m_modelIndex)->GetColModel();
					int collidedSpheres = CCollision::ProcessColModels(sphereMat, sphereCol, e->GetMatrix(),
						*eCol, &ms_testSpherePoint, nil, nil);

					if (collidedSpheres != 0 ||
						(e->IsVehicle() && ((CVehicle*)e)->m_vehType == VEHICLE_TYPE_CAR &&
						e->m_modelIndex != MI_DODO && radius + eCol->boundingBox.max.x > distance)) {
						return e;
					}
				}
			}
		}
	}

	return nil;
}

float
CWorld::FindGroundZForCoord(float x, float y)
{
	CColPoint point;
	CEntity *ent;
	if(ProcessVerticalLine(CVector(x, y, 1000.0f), -1000.0f, point, ent, true, false, false, false, true, false, nil))
		return point.point.z;
	else
		return 20.0f;
}

float
CWorld::FindGroundZFor3DCoord(float x, float y, float z, bool *found)
{
	CColPoint point;
	CEntity *ent;
	if(ProcessVerticalLine(CVector(x, y, z), -1000.0f, point, ent, true, false, false, false, false, false, nil)){
		if(found)
			*found = true;
		return point.point.z;
	}else{
		if(found)
			*found = false;
		return 0.0f;
	}
}

float
CWorld::FindRoofZFor3DCoord(float x, float y, float z, bool *found)
{
	CColPoint point;
	CEntity *ent;
	if(ProcessVerticalLine(CVector(x, y, z), 1000.0f, point, ent, true, false, false, false, true, false, nil)){
		if(found)
			*found = true;
		return point.point.z;
	}else{
		if(found == nil)
			printf("THERE IS NO MAP BELOW THE FOLLOWING COORS:%f %f %f. (FindGroundZFor3DCoord)\n", x, y, z);
		if(found)
			*found = false;
		return 20.0f;
	}
}

CPlayerPed*
FindPlayerPed(void)
{
	return CWorld::Players[CWorld::PlayerInFocus].m_pPed;
}

CVehicle*
FindPlayerVehicle(void)
{
	CPlayerPed *ped = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
	if(ped->bInVehicle && ped->m_pMyVehicle)
		return ped->m_pMyVehicle;
	else
		return nil;
}

CVehicle*
FindPlayerTrain(void)
{
	if(FindPlayerVehicle() && FindPlayerVehicle()->IsTrain())
		return FindPlayerVehicle();
	else
		return nil;
}

CEntity*
FindPlayerEntity(void)
{
	CPlayerPed *ped = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
	if(ped->bInVehicle && ped->m_pMyVehicle)
		return ped->m_pMyVehicle;
	else
		return ped;
}

CVector
FindPlayerCoors(void)
{
	CPlayerPed *ped = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
	if(ped->bInVehicle && ped->m_pMyVehicle)
		return ped->m_pMyVehicle->GetPosition();
	else
		return ped->GetPosition();
}

CVector&
FindPlayerSpeed(void)
{
	CPlayerPed *ped = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
	if(ped->bInVehicle && ped->m_pMyVehicle)
		return ped->m_pMyVehicle->m_vecMoveSpeed;
	else
		return ped->m_vecMoveSpeed;
}

CVector&
FindPlayerCentreOfWorld(int32 player)
{
	if(CCarCtrl::bCarsGeneratedAroundCamera)
		return TheCamera.GetPosition();
	if(CWorld::Players[player].m_pRemoteVehicle)
		return CWorld::Players[player].m_pRemoteVehicle->GetPosition();
	if(FindPlayerVehicle())
		return FindPlayerVehicle()->GetPosition();
	return CWorld::Players[player].m_pPed->GetPosition();
}

CVector&
FindPlayerCentreOfWorld_NoSniperShift(void)
{
	if(CCarCtrl::bCarsGeneratedAroundCamera)
		return TheCamera.GetPosition();
	if(CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle)
		return CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle->GetPosition();
	if(FindPlayerVehicle())
		return FindPlayerVehicle()->GetPosition();
	return CWorld::Players[CWorld::PlayerInFocus].m_pPed->GetPosition();
}

float
FindPlayerHeading(void)
{
	if(CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle)
		return CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle->GetForward().Heading();
	if(FindPlayerVehicle())
		return FindPlayerVehicle()->GetForward().Heading();
	return CWorld::Players[CWorld::PlayerInFocus].m_pPed->GetForward().Heading();
}

STARTPATCHES
	InjectHook(0x4AE930, CWorld::Add, PATCH_JUMP);
	InjectHook(0x4AE9D0, CWorld::Remove, PATCH_JUMP);
	InjectHook(0x4B1F60, CWorld::ClearScanCodes, PATCH_JUMP);
	InjectHook(0x4AF970, CWorld::ProcessLineOfSight, PATCH_JUMP);
	InjectHook(0x4B0A80, CWorld::ProcessLineOfSightSector, PATCH_JUMP);
	InjectHook(0x4B0C70, CWorld::ProcessLineOfSightSectorList, PATCH_JUMP);
	InjectHook(0x4B0DE0, CWorld::ProcessVerticalLine, PATCH_JUMP);
	InjectHook(0x4B0EF0, CWorld::ProcessVerticalLineSector, PATCH_JUMP);
	InjectHook(0x4B1090, CWorld::ProcessVerticalLineSectorList, PATCH_JUMP);
	InjectHook(0x4AEAA0, CWorld::GetIsLineOfSightClear, PATCH_JUMP);
	InjectHook(0x4B2000, CWorld::GetIsLineOfSightSectorClear, PATCH_JUMP);
	InjectHook(0x4B2160, CWorld::GetIsLineOfSightSectorListClear, PATCH_JUMP);

	InjectHook(0x4B2200, CWorld::FindObjectsInRange, PATCH_JUMP);
	InjectHook(0x4B2540, CWorld::FindObjectsInRangeSectorList, PATCH_JUMP);
	InjectHook(0x4B4AC0, CWorld::TestSphereAgainstSectorList, PATCH_JUMP);
	InjectHook(0x4B4710, CWorld::TestSphereAgainstWorld, PATCH_JUMP);
	InjectHook(0x4B3A80, CWorld::FindGroundZForCoord, PATCH_JUMP);
	InjectHook(0x4B3AE0, CWorld::FindGroundZFor3DCoord, PATCH_JUMP);
	InjectHook(0x4B3B50, CWorld::FindRoofZFor3DCoord, PATCH_JUMP);
ENDPATCHES
