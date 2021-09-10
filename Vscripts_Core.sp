#pragma semicolon 1

#define PLUGIN_AUTHOR "Cloud Strife"
#define PLUGIN_VERSION "1.00"

#include <sourcemod>
#include <vscripts>
#include <dhooks>

#pragma newdecls required

public Plugin myinfo = 
{
	name = "Vscripts Core",
	author = PLUGIN_AUTHOR,
	description = "",
	version = PLUGIN_VERSION,
	url = "https://steamcommunity.com/id/cloudstrifeua/"
};

#define nullptr Address_Null

ArrayList g_aVscriptTimers = null;

bool g_bEventQueue = false;

Handle g_hGetModel = null, g_hGetModelType = null;
Handle g_hTemplateCreateInstace = null;

Address g_pModelInfo = nullptr;

int g_iSolidType, g_iOwnerEntity, g_iCollisionGroup;
int g_iUtlVectorSize = -1;

GlobalForward g_fwdTemplateCreateInstace;

enum modtype_t
{
	mod_bad = 0, 
	mod_brush, 
	mod_sprite, 
	mod_studio
}

enum SolidType_t
{
	SOLID_NONE			= 0,	// no solid model
	SOLID_BSP			= 1,	// a BSP tree
	SOLID_BBOX			= 2,	// an AABB
	SOLID_OBB			= 3,	// an OBB (not implemented yet)
	SOLID_OBB_YAW		= 4,	// an OBB, constrained so that it can only yaw
	SOLID_CUSTOM		= 5,	// Always call into the entity for tests
	SOLID_VPHYSICS		= 6,	// solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST,
}

enum Collision_Group_t
{
	COLLISION_GROUP_NONE  = 0,
	COLLISION_GROUP_DEBRIS,			// Collides with nothing but world and static stuff
	COLLISION_GROUP_DEBRIS_TRIGGER, // Same as debris, but hits triggers
	COLLISION_GROUP_INTERACTIVE_DEBRIS,	// Collides with everything except other interactive debris or debris
	COLLISION_GROUP_INTERACTIVE,	// Collides with everything except interactive debris or debris
	COLLISION_GROUP_PLAYER,
	COLLISION_GROUP_BREAKABLE_GLASS,
	COLLISION_GROUP_VEHICLE,
	COLLISION_GROUP_PLAYER_MOVEMENT,  // For HL2, same as Collision_Group_Player, for
										// TF2, this filters out other players and CBaseObjects
	COLLISION_GROUP_NPC,			// Generic NPC group
	COLLISION_GROUP_IN_VEHICLE,		// for any entity inside a vehicle
	COLLISION_GROUP_WEAPON,			// for any weapons that need collision detection
	COLLISION_GROUP_VEHICLE_CLIP,	// vehicle clip brush to restrict vehicle movement
	COLLISION_GROUP_PROJECTILE,		// Projectiles!
	COLLISION_GROUP_DOOR_BLOCKER,	// Blocks entities not permitted to get near moving doors
	COLLISION_GROUP_PASSABLE_DOOR,	// Doors that the player shouldn't collide with
	COLLISION_GROUP_DISSOLVING,		// Things that are dissolving are in this group
	COLLISION_GROUP_PUSHAWAY,		// Nonsolid on client and server, pushaway in player code
	COLLISION_GROUP_NPC_ACTOR,		// Used so NPCs in scripts ignore the player.
	COLLISION_GROUP_NPC_SCRIPTED,	// USed for NPCs in scripts that should not collide with each other
	LAST_SHARED_COLLISION_GROUP
}

methodmap VscriptTimer < StringMap 
{
	public VscriptTimer(Handle plugin, float time, VscriptTimerCallback cb, any data)
	{
		StringMap myclass = new StringMap();
		float trigger_time = GetGameTime() + time;
		myclass.SetValue("m_fTriggerTime", trigger_time);
		PrivateForward cb_fwd = new PrivateForward(ET_Ignore, Param_Any);
		cb_fwd.AddFunction(plugin, cb);
		myclass.SetValue("m_hCallback", cb_fwd);
		myclass.SetValue("m_Data", data);
		
		bool inserted = false;
		for(int i = 0; i < g_aVscriptTimers.Length; ++i)
		{	
			VscriptTimer cur = g_aVscriptTimers.Get(i);
			float cur_t;
			cur.GetValue("m_fTriggerTime", cur_t);
			if(cur_t > trigger_time)
			{
				inserted = true;
				g_aVscriptTimers.ShiftUp(i);
				g_aVscriptTimers.Set(i, myclass);
				break;
			}
		}
		if(!inserted)
			g_aVscriptTimers.Push(myclass);
			
		return view_as<VscriptTimer>(myclass);
	}
	
	property float time 
	{
		public get()
		{
			float t;
			this.GetValue("m_fTriggerTime", t);
			return t;
		}
	}
	
	property any data 
	{
		public get()
		{
			any data;
			this.GetValue("m_Data", data);
			return data;
		}
	}
	
	public void Kill()
	{
		PrivateForward cb_fwd;
		this.GetValue("m_hCallback", cb_fwd);
		delete cb_fwd;
		delete this;
	}
	
	public void Trigger()
	{
		PrivateForward cb_fwd;
		this.GetValue("m_hCallback", cb_fwd);
		any data = this.data;
		delete this;
		
		Call_StartForward(cb_fwd);
		Call_PushCell(data);
		Call_Finish();
		
		delete cb_fwd;
	}
}

public void OnGameFrame()
{
	for(int i = 0; i < g_aVscriptTimers.Length; ++i)
	{
		VscriptTimer cur = g_aVscriptTimers.Get(i);
		if(cur.time <= GetGameTime())
		{
			g_aVscriptTimers.Erase(i--);
			cur.Trigger();
		}
		else break;
	}
}

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
	CreateNative("Vscripts_CreateTimer", Native_CreateTimer);
	CreateNative("Vscripts_IsEventQueueLoaded", Native_IsEventQueueLoaded);
	CreateNative("Vscripts_TraceFilterSimple", Native_TraceFilterSimple);
	RegPluginLibrary("vscripts");
	return APLRes_Success;
}

public any Native_CreateTimer(Handle plugin, int numParams)
{
	new VscriptTimer(plugin, view_as<float>(GetNativeCell(1)), view_as<VscriptTimerCallback>(GetNativeFunction(2)), GetNativeCell(3));
}

public int Native_IsEventQueueLoaded(Handle plugin, int numParams)
{
	return g_bEventQueue;
}

public void OnAllPluginsLoaded()
{
	g_bEventQueue = LibraryExists("Entity Events Queue");
}

//public void OnLibraryAdded(const char[] name)
//{
	//if(StrEqual(name, "Entity Events Queue"))
	//{
		//g_bEventQueue = true;
	//}
//}

public void OnLibraryRemoved(const char[] name)
{
	if(StrEqual(name, "Entity Events Queue"))
	{
		g_bEventQueue = false;
	}
}

public void OnPluginStart()
{
	g_aVscriptTimers = new ArrayList();
	
	Handle hGameData = LoadGameConfigFile("Vscripts_Core.games");
	if(!hGameData)
		SetFailState("Failed to load Vscripts_Core gamedata.");
		
	StartPrepSDKCall(SDKCall_Entity);
	if(!PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, "CBaseEntity::GetModel"))
	{
		delete hGameData;
		SetFailState("PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, \"CBaseEntity::GetModel\") failed!");
	}
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
	g_hGetModel = EndPrepSDKCall();
	
	StartPrepSDKCall(SDKCall_Raw);
	if(!PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, "CModelInfo::GetModelType"))
	{
		delete hGameData;
		SetFailState("PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, \"CModelInfo::GetModelType\") failed!");
	}
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
	g_hGetModelType = EndPrepSDKCall();
	
	g_pModelInfo = GameConfGetAddress(hGameData, "modelinfo");
	if(!g_pModelInfo)
	{
		delete hGameData;
		SetFailState("Couldn't load modelinfo address!");
	}
	
	g_iUtlVectorSize = GameConfGetOffset(hGameData, "CUtlVector::m_iSize");
	if(g_iUtlVectorSize == -1)
	{
		delete hGameData;
		SetFailState("Couldn't load CUtlVector::m_iSize offset!");
	}

	Address pCreateInstance = GameConfGetAddress(hGameData, "CPointTemplate::CreateInstance");
	if(!pCreateInstance)
	{
		delete hGameData;
		SetFailState("Couldn't load CPointTemplate::CreateInstance address!");
	}

	g_hTemplateCreateInstace = DHookCreateDetour(pCreateInstance, CallConv_THISCALL, ReturnType_Bool, ThisPointer_CBaseEntity);
	DHookAddParam(g_hTemplateCreateInstace, HookParamType_VectorPtr);
	DHookAddParam(g_hTemplateCreateInstace, HookParamType_VectorPtr);
	DHookAddParam(g_hTemplateCreateInstace, HookParamType_ObjectPtr);
	if(!DHookEnableDetour(g_hTemplateCreateInstace, true, CPointTemplate_CreateInstance))
	{
		delete hGameData;
		SetFailState("Couldn't create detour on CPointTemplate::CreateInstance!");
	}

	delete hGameData;

	g_fwdTemplateCreateInstace = new GlobalForward("Vscritps_OnTemplateInstanceCreated", ET_Ignore, Param_Cell, Param_Array, Param_Cell);
}

public MRESReturn CPointTemplate_CreateInstance(int entity, DHookReturn hReturn, DHookParam hParams)
{
	if( view_as<bool>(DHookGetReturn(hReturn)) == false )
	{
		return MRES_Ignored;
	}
	int iSize = DHookGetParamObjectPtrVar(hParams, 3, g_iUtlVectorSize, ObjectValueType_Int);
	Address pBegin = view_as<Address>(DHookGetParamObjectPtrVar(hParams, 3, 0, ObjectValueType_Int));
	int[] createdEntities = new int[iSize];
	for(int i = 0; i < iSize; ++i)
	{
		createdEntities[i] = GetEntityFromAddress(view_as<Address>(LoadFromAddress(pBegin + view_as<Address>(i*4), NumberType_Int32)));
	}
	Call_StartForward(g_fwdTemplateCreateInstace);
	Call_PushCell(entity);
	Call_PushArray(createdEntities, iSize);
	Call_PushCell(iSize);
	Call_Finish();
	return MRES_Ignored;
}

public void OnMapStart()
{
	g_iSolidType = FindDataMapInfo(0, "m_nSolidType");
	g_iOwnerEntity = FindDataMapInfo(0, "m_hOwnerEntity");
	g_iCollisionGroup = FindDataMapInfo(0, "m_CollisionGroup");
}

public modtype_t GetModelType(int entity)
{
	Address pModel = view_as<Address>(SDKCall(g_hGetModel, entity));
	return view_as<modtype_t>(SDKCall(g_hGetModelType, LoadFromAddress(g_pModelInfo, NumberType_Int32), pModel));
}

public bool StandardFilterRules(int entity, int contentsMask)
{
	if(!IsValidEntity(entity))
		return true;
	
	SolidType_t solid = view_as<SolidType_t>(GetEntData(entity, g_iSolidType));
	modtype_t model = GetModelType(entity);
	
	if((model != mod_brush) || (solid != SOLID_BSP && solid != SOLID_VPHYSICS))
	{
		if((contentsMask & CONTENTS_MONSTER) == 0)
			return false;
	}
	
	if(!(contentsMask & CONTENTS_WINDOW) && (GetEntityRenderMode(entity) != RENDER_NORMAL))
		return false;
	
	if(!(contentsMask & CONTENTS_MOVEABLE) && (GetEntityMoveType(entity) == MOVETYPE_PUSH))
		return false;
	
	return true;
}

public bool PassServerEntityFilter(int entity, int 	pass)
{
	if(!IsValidEntity(pass) || !IsValidEntity(entity))
		return true;
	
	if(entity == pass)
		return false;
	
	int owner = GetEntDataEnt2(entity, g_iOwnerEntity);
	if(owner == pass)
		return false;
	
	owner = GetEntDataEnt2(pass, g_iOwnerEntity);
	if(owner == entity)
		return false;
		
	return true;
}

public bool GameRules_ShouldCollide(Collision_Group_t collisionGroup0, Collision_Group_t collisionGroup1)
{
	if ( collisionGroup0 > collisionGroup1 )
	{
		// swap so that lowest is always first
		Collision_Group_t tmp = collisionGroup0;
		collisionGroup0 = collisionGroup1;
		collisionGroup1 = tmp;
	}

	if ( (collisionGroup0 == COLLISION_GROUP_PLAYER || collisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT) &&
		collisionGroup1 == COLLISION_GROUP_PUSHAWAY )
	{
		return false;
	}

	if ( collisionGroup0 == COLLISION_GROUP_DEBRIS && collisionGroup1 == COLLISION_GROUP_PUSHAWAY )
	{
		// let debris and multiplayer objects collide
		return true;
	}
	
	// --------------------------------------------------------------------------
	// NOTE: All of this code assumes the collision groups have been sorted!!!!
	// NOTE: Don't change their order without rewriting this code !!!
	// --------------------------------------------------------------------------
	// Don't bother if either is in a vehicle...
	if (( collisionGroup0 == COLLISION_GROUP_IN_VEHICLE ) || ( collisionGroup1 == COLLISION_GROUP_IN_VEHICLE ))
		return false;

	if ( ( collisionGroup1 == COLLISION_GROUP_DOOR_BLOCKER ) && ( collisionGroup0 != COLLISION_GROUP_NPC ) )
		return false;

	if ( ( collisionGroup0 == COLLISION_GROUP_PLAYER ) && ( collisionGroup1 == COLLISION_GROUP_PASSABLE_DOOR ) )
		return false;

	if ( collisionGroup0 == COLLISION_GROUP_DEBRIS || collisionGroup0 == COLLISION_GROUP_DEBRIS_TRIGGER )
	{
		// put exceptions here, right now this will only collide with COLLISION_GROUP_NONE
		return false;
	}

	// Dissolving guys only collide with COLLISION_GROUP_NONE
	if ( (collisionGroup0 == COLLISION_GROUP_DISSOLVING) || (collisionGroup1 == COLLISION_GROUP_DISSOLVING) )
	{
		if ( collisionGroup0 != COLLISION_GROUP_NONE )
			return false;
	}

	// doesn't collide with other members of this group
	// or debris, but that's handled above
	if ( collisionGroup0 == COLLISION_GROUP_INTERACTIVE_DEBRIS && collisionGroup1 == COLLISION_GROUP_INTERACTIVE_DEBRIS )
		return false;
		
	// This change was breaking HL2DM
	// Adrian: TEST! Interactive Debris doesn't collide with the player.
	if ( collisionGroup0 == COLLISION_GROUP_INTERACTIVE_DEBRIS && ( collisionGroup1 == COLLISION_GROUP_PLAYER_MOVEMENT || collisionGroup1 == COLLISION_GROUP_PLAYER ) )
		 return false;
		 
	if ( collisionGroup0 == COLLISION_GROUP_BREAKABLE_GLASS && collisionGroup1 == COLLISION_GROUP_BREAKABLE_GLASS )
		return false;

	// interactive objects collide with everything except debris & interactive debris
	if ( collisionGroup1 == COLLISION_GROUP_INTERACTIVE && collisionGroup0 != COLLISION_GROUP_NONE )
		return false;

	// Projectiles hit everything but debris, weapons, + other projectiles
	if ( collisionGroup1 == COLLISION_GROUP_PROJECTILE )
	{
		if ( collisionGroup0 == COLLISION_GROUP_DEBRIS || 
			collisionGroup0 == COLLISION_GROUP_WEAPON ||
			collisionGroup0 == COLLISION_GROUP_PROJECTILE )
		{
			return false;
		}
	}

	// Don't let vehicles collide with weapons
	// Don't let players collide with weapons...
	// Don't let NPCs collide with weapons
	// Weapons are triggers, too, so they should still touch because of that
	if ( collisionGroup1 == COLLISION_GROUP_WEAPON )
	{
		if ( collisionGroup0 == COLLISION_GROUP_VEHICLE || 
			collisionGroup0 == COLLISION_GROUP_PLAYER ||
			collisionGroup0 == COLLISION_GROUP_NPC )
		{
			return false;
		}
	}

	// collision with vehicle clip entity??
	if ( collisionGroup0 == COLLISION_GROUP_VEHICLE_CLIP || collisionGroup1 == COLLISION_GROUP_VEHICLE_CLIP )
	{
		// yes then if it's a vehicle, collide, otherwise no collision
		// vehicle sorts lower than vehicle clip, so must be in 0
		if ( collisionGroup0 == COLLISION_GROUP_VEHICLE )
			return true;
		// vehicle clip against non-vehicle, no collision
		return false;
	}

	return true;
}

public bool ShouldCollide(int entity, Collision_Group_t collisionGroup, int contentsMask)
{
	Collision_Group_t m_collisionGroup  = view_as<Collision_Group_t>(GetEntData(entity, g_iCollisionGroup));
	if(m_collisionGroup == COLLISION_GROUP_DEBRIS)
	{
		if(!(contentsMask & CONTENTS_DEBRIS))
			return false;
	}
	return true;
}

public int Native_TraceFilterSimple(Handle plugin, int numParams)
{
	int entity = GetNativeCell(1);
	int contentsMask = GetNativeCell(2);
	int ignore = GetNativeCell(3);
	
	if(!IsValidEntity(entity))
		return false;
	
	if(!StandardFilterRules(entity, contentsMask))
		return false;
	
	if(ignore != -1)
	{
		if(!PassServerEntityFilter(entity, ignore))
			return false;
	}
	
	if(!ShouldCollide(entity, COLLISION_GROUP_NONE, contentsMask))
		return false;
	if(!GameRules_ShouldCollide(COLLISION_GROUP_NONE, view_as<Collision_Group_t>(GetEntData(entity, g_iCollisionGroup))))
		return false;
		
	return true;
}

public void OnMapEnd()
{
	for(int i = 0; i < g_aVscriptTimers.Length; ++i)
	{
		VscriptTimer cur = g_aVscriptTimers.Get(i);
		cur.Kill();
	}
	g_aVscriptTimers.Clear();
}

//Copied from https://github.com/nosoop/stocksoup/blob/master/memory.inc
/**
 * Retrieves an entity index from a raw entity handle address.
 * 
 * Note that SourceMod's entity conversion routine is an implementation detail that may change.
 * 
 * @param addr			Address to a memory location.
 * @return				Entity index, or -1 if not valid.
 */
stock int LoadEntityHandleFromAddress(Address addr) {
	return EntRefToEntIndex(LoadFromAddress(addr, NumberType_Int32) | (1 << 31));
}

/**
 * Returns an entity index from its address by attempting to read the
 * CBaseEntity::m_RefEHandle member.  This assumes the address of a CBaseEntity is
 * passed in.
 * 
 * @param pEntity		Address of an entity.
 * @return				Entity index, or -1 if not valid.
 */
stock int GetEntityFromAddress(Address pEntity) {
	static int offs_RefEHandle;
	if (offs_RefEHandle) {
		return LoadEntityHandleFromAddress(pEntity + view_as<Address>(offs_RefEHandle));
	}
	
	// if we don't have it already, attempt to lookup offset based on SDK information
	// CWorld is derived from CBaseEntity so it should have both offsets
	int offs_angRotation = FindDataMapInfo(0, "m_angRotation"),
			offs_vecViewOffset = FindDataMapInfo(0, "m_vecViewOffset");
	if (offs_angRotation == -1) {
		ThrowError("Could not find offset for ((CBaseEntity) CWorld)::m_angRotation");
	} else if (offs_vecViewOffset == -1) {
		ThrowError("Could not find offset for ((CBaseEntity) CWorld)::m_vecViewOffset");
	} else if ((offs_angRotation + 0x0C) != (offs_vecViewOffset - 0x04)) {
		char game[32];
		GetGameFolderName(game, sizeof(game));
		ThrowError("Could not confirm offset of CBaseEntity::m_RefEHandle "
				... "(incorrect assumption for game '%s'?)", game);
	}
	
	// offset seems right, cache it for the next call
	offs_RefEHandle = offs_angRotation + 0x0C;
	return GetEntityFromAddress(pEntity);
}
