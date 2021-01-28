/*
**
*/
#if defined _VSCRIPTS_included
 #endinput
#endif
#define _VSCRIPTS_included
#include <sdktools>

#define MAX_ENT_NAME 128
#define MAX_INPUT_NAME 64

public int GetEntityIndexByHammerID(int HammerID, const char[] classname)
{
	int i = -1;
	while((i = FindEntityByClassname(i,classname))!= -1){
		if(IsValidEntity(i)){
			if (GetEntProp(i, Prop_Data, "m_iHammerID") == HammerID) return i;
		}
	}
	return -1;
}

public int GetEntityIndexByName(const char[] name, const char[] classname)
{
	int i = -1;
	char buffer[MAX_ENT_NAME];
	while((i = FindEntityByClassname(i, classname))!= -1){
		if(IsValidEntity(i)){
			GetEntPropString(i, Prop_Data, "m_iName", buffer, sizeof(buffer));
			if (strcmp(name, buffer, false) == 0)
				return i;
		}
	}
	return -1;
}

public int FindEntityByClassnameWithin(int startEnt, const char[] classname, const float[3] origin, const float radius)
{
	float torigin[3];
	while((startEnt = FindEntityByClassname(startEnt,classname))!= -1){
		if(IsValidEntity(startEnt))
		{
			GetOrigin(startEnt, torigin);
			if(GetVectorDistance(torigin, origin) <= radius) return startEnt;
		}
	}
	return -1;
}

public float TraceLine(const float[3] origin, const float[3] v2, int entity)
{
	Handle trace = TR_TraceRayFilterEx(origin, v2, CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW, RayType_EndPoint, TraceEntityFilterSelf, entity);
	float pos[3], dist = 1.0;
	if(TR_DidHit(trace)){
		TR_GetEndPosition(pos, trace);
		dist = GetVectorDistance(origin, pos)/GetVectorDistance(origin, v2);
	}
	CloseHandle(trace);
	return dist;
}

public bool TraceEntityFilterSelf(int entity, int contentsMask, any data)
{
	return (data != entity && entity > MAXPLAYERS);
} 

public void GetOrigin(int entity, float[3] buffer)
{
	GetEntPropVector(entity, Prop_Send, "m_vecOrigin", buffer);
}

public void GetAngles(int entity, float[3] buffer)
{
	GetEntPropVector(entity, Prop_Send, "m_angRotation", buffer);
}

public void SetAngles(int entity, const float[3] buffer)
{
	TeleportEntity(entity, NULL_VECTOR, buffer, NULL_VECTOR);
}

public void SetOrigin(int entity, const float[3] buffer)
{
	TeleportEntity(entity, buffer, NULL_VECTOR, NULL_VECTOR);
}

public void GetForwardVector(int entity, float[3] buffer)
{
	float tmp[3];
	GetAngles(entity, tmp);
	GetAngleVectors(tmp, buffer, NULL_VECTOR, NULL_VECTOR);
}

public void SetForwardVector(int entity, const float[3] buffer)
{
	float tmp[3];
	GetVectorAngles(buffer, tmp);
	SetAngles(entity, tmp);
}

public void EntFire(const char[] target, const char[] input, const char[] parametr, const char[] delay, int activator)
{
	char output[2*(MAX_ENT_NAME + MAX_INPUT_NAME) + 4];
	int tmp = CreateEntityByName("info_target");
	Format(output, sizeof(output), "OnUser1 %s:%s:%s:%s", target, input, parametr, delay);
	SetVariantString(output);
	AcceptEntityInput(tmp, "AddOutput");
	AcceptEntityInput(tmp, "FireUser1", activator);
	AcceptEntityInput(tmp, "Kill");
}

public void EntFireByIndex(int target, const char[] input, const char[] parametr, const char[] delay, int activator)
{
	float fDelay = StringToFloat(delay);
	DataPack data = CreateDataPack();
	data.WriteString(input);
	data.WriteString(parametr);
	data.WriteCell(activator);
	data.WriteCell(EntIndexToEntRef(target));
	CreateTimer(fDelay, InputDelay, data, TIMER_DATA_HNDL_CLOSE|TIMER_FLAG_NO_MAPCHANGE);
}

public Action InputDelay(Handle timer, DataPack data)
{
	char input[MAX_INPUT_NAME], parametr[MAX_ENT_NAME];
	data.Reset();
	data.ReadString(input, sizeof(input));
	data.ReadString(parametr, sizeof(parametr));
	int activator = data.ReadCell();
	int target = EntRefToEntIndex(data.ReadCell());
	data.Reset(true);
	if(IsValidEntity(target))
	{
		SetVariantString(parametr);
		AcceptEntityInput(target, input, activator);
	}
}