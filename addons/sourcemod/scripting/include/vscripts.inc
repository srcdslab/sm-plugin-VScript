/*
**
*/
#if defined _VSCRIPTS_included
 #endinput
#endif
#define _VSCRIPTS_included
#include <sdktools>
#undef REQUIRE_EXTENSIONS
#include <eventqueue>
#define REQUIRE_EXTENSIONS

#define MAX_ENT_NAME 32
#define MAX_INPUT_NAME 32

typedef VscriptTimerCallback = function void(any data);

native void Vscripts_CreateTimer(float interval, VscriptTimerCallback func, any data = INVALID_HANDLE);
native bool Vscripts_IsEventQueueLoaded();
native bool Vscripts_TraceFilterSimple(int entity, int contentsMask, int ignore = -1);

forward void Vscritps_OnTemplateInstanceCreated(int template, const int[] createdEntities, int size);

stock int Vscripts_GetEntityIndexByHammerID(int HammerID, const char[] classname = "*", int startEnt = -1)
{
	while((startEnt = FindEntityByClassname(startEnt,classname))!= -1) {
		if (GetEntProp(startEnt, Prop_Data, "m_iHammerID") == HammerID)
			return startEnt;
	}
	return -1;
}

stock int Vscripts_GetEntityIndexByName(const char[] name, const char[] classname = "*", int startEnt = -1)
{
	char buffer[MAX_ENT_NAME];
	int ch = FindCharInString(name, '*', true);
	while((startEnt = FindEntityByClassname(startEnt, classname))!= -1){
		GetEntityClassname(startEnt, buffer, sizeof(buffer));
		if (strcmp(name, buffer) == 0)
			return startEnt;
			
		GetEntPropString(startEnt, Prop_Data, "m_iName", buffer, sizeof(buffer));
		if(strncmp(name, buffer, ch) == 0)
			return startEnt;
	}
	return -1;
}

public int Vscripts_FindEntityByClassnameWithin(int startEnt, const char[] classname, const float[3] origin, const float radius)
{
	float torigin[3];
	while((startEnt = FindEntityByClassname(startEnt,classname))!= -1){
		Vscripts_GetOrigin(startEnt, torigin);
		if(GetVectorDistance(torigin, origin) <= radius) return startEnt;
	}
	return -1;
}

public float Vscripts_TraceLine(const float[3] origin, const float[3] v2, int entity)
{
	TR_TraceRayFilter(origin, v2, MASK_NPCWORLDSTATIC, RayType_EndPoint, TraceEntityFilterSelf, entity);
	float fraction_left = TR_GetFractionLeftSolid(INVALID_HANDLE), fraction = 0.0;
	if (fraction_left && TR_StartSolid(INVALID_HANDLE)) {
		fraction = 1.0 - fraction_left;
	}
	else {
		fraction = TR_GetFraction(INVALID_HANDLE);
	}
	return fraction;
}

bool TraceEntityFilterSelf(int entity, int contentsMask, int data)
{
	return Vscripts_TraceFilterSimple(entity, contentsMask, data);
} 

public void Vscripts_GetOrigin(int entity, float[3] buffer)
{
	GetEntPropVector(entity, Prop_Send, "m_vecOrigin", buffer);
}

public void Vscripts_GetAngles(int entity, float[3] buffer)
{
	GetEntPropVector(entity, Prop_Send, "m_angRotation", buffer);
}

public void Vscripts_SetAngles(int entity, const float[3] buffer)
{
	TeleportEntity(entity, NULL_VECTOR, buffer, NULL_VECTOR);
}

public void Vscripts_SetOrigin(int entity, const float[3] buffer)
{
	TeleportEntity(entity, buffer, NULL_VECTOR, NULL_VECTOR);
}

public void Vscripts_GetForwardVector(int entity, float[3] buffer)
{
	float tmp[3];
	Vscripts_GetAngles(entity, tmp);
	GetAngleVectors(tmp, buffer, NULL_VECTOR, NULL_VECTOR);
}

public void Vscripts_SetForwardVector(int entity, const float[3] buffer)
{
	float tmp[3];
	GetVectorAngles(buffer, tmp);
	Vscripts_SetAngles(entity, tmp);
}

stock void Vscripts_EntFire(const char[] target, const char[] input, const char[] parametr, float delay, int activator = -1)
{
	if(Vscripts_IsEventQueueLoaded())
		EQ_AddEventByName(target, input, parametr, delay, activator);
	else
	{
		DataPack data = CreateDataPack();
		data.WriteString(input);
		data.WriteString(parametr);
		data.WriteCell(activator >= 0 ? EntIndexToEntRef(activator) : -1);
		data.WriteCell(true);
		data.WriteString(target);
		Vscripts_CreateTimer(delay, InputDelay, data);
	}
}

stock void Vscripts_EntFireByIndex(int target, const char[] input, const char[] parametr, float delay, int activator = -1)
{
	if(Vscripts_IsEventQueueLoaded())
		EQ_AddEvent(target, input, parametr, delay, activator);
	else
	{
		DataPack data = CreateDataPack();
		data.WriteString(input);
		data.WriteString(parametr);
		data.WriteCell(activator >= 0 ? EntIndexToEntRef(activator) : -1);
		data.WriteCell(false);
		data.WriteCell(EntIndexToEntRef(target));
		Vscripts_CreateTimer(delay, InputDelay, data);
	}
}

void InputDelay(DataPack data)
{
	char input[MAX_INPUT_NAME], parametr[2*MAX_ENT_NAME];
	data.Reset();
	data.ReadString(input, sizeof(input));
	data.ReadString(parametr, sizeof(parametr));
	int activator = data.ReadCell();
	int target = -1;
	if(view_as<bool>(data.ReadCell()) == true)
	{
		char name[MAX_ENT_NAME];
		data.ReadString(name, sizeof(name));
		while((target = Vscripts_GetEntityIndexByName(name, _, target)) != -1)
		{
			SetVariantString(parametr);
			AcceptEntityInput(target, input, activator != -1 ? EntRefToEntIndex(activator) : -1);
		}
	}
	else
	{
		target = EntRefToEntIndex(data.ReadCell());
		if(IsValidEntity(target))
		{
			SetVariantString(parametr);
			AcceptEntityInput(target, input, activator != -1 ? EntRefToEntIndex(activator) : -1);
		}
	}
	data.Reset(true);
	delete data;
}

public SharedPlugin __pl_vscripts= 
{
	name = "vscripts",
	file = "Vscripts_Core.smx",
#if defined REQUIRE_PLUGIN
	required = 1
#else
	required = 0
#endif
};

#if !defined REQUIRE_PLUGIN
public void __pl_vscripts_SetNTVOptional()
{
	MarkNativeAsOptional("Vscripts_CreateTimer");
	MarkNativeAsOptional("Vscripts_IsEventQueueLoaded");
	MarkNativeAsOptional("Vscripts_TraceFilterSimple");
}
#endif