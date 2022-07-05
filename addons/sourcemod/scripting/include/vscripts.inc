/*
**
*/
#if defined _ENT_INDEX_included
 #endinput
#endif
#define _ENT_INDEX_included


public int GetEntityIndexByHammerID(int HammerID, const char[] classname) {
	int i = -1;
	while((i = FindEntityByClassname(i,classname))!= -1){
		if(IsValidEntity(i)){
			if (GetEntProp(i, Prop_Data, "m_iHammerID") == HammerID) return i;
		}
	}
	return -1;
}

public int GetEntityIndexByName(const char[] name, const char[] classname){
	int i = -1;
	char buffer[500];
	while((i = FindEntityByClassname(i, classname))!= -1){
		if(IsValidEntity(i)){
			GetEntPropString(i, Prop_Data, "m_iName", buffer, sizeof(buffer));
			if (strcmp(name, buffer, false) == 0)
				return i;
		}
	}
	return -1;
}

public int FindEntityByClassnameWithin(int startEnt, const char[] classname, const float origin[3], const float radius){ //Tested. Works properly.
	float torigin[3];
	while((startEnt = FindEntityByClassname(startEnt,classname))!= -1){
		GetOrigin(startEnt, torigin);
		if (GetVectorDistance(torigin, origin) <= radius)return startEnt;
	}
	return -1;
}

public float TraceLine(const float origin[3], const float v2[3], int entity){		//Tested. Works properly
	Handle trace = TR_TraceRayFilterEx(origin, v2, CONTENTS_SOLID, RayType_EndPoint, TraceEntityFilterSelf, entity);
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
	return (data != entity && entity > 64);
} 

public void GetOrigin(int entity, float buffer[3]){ 			//Tested. Works properly.
	if(IsValidEntity(entity))
		GetEntPropVector(entity, Prop_Send, "m_vecOrigin", buffer);
}

public void GetAngles(int entity, float buffer[3]){				//Gives you wrong values after spawn it via env_entity_maker
	if(IsValidEntity(entity))
		GetEntPropVector(entity, Prop_Send, "m_angRotation", buffer);
}

public void SetAngles(int entity, const float buffer[3]){	//Tested. Works properly.
	if(IsValidEntity(entity))
		TeleportEntity(entity, NULL_VECTOR, buffer, NULL_VECTOR);
}

public void SetOrigin(int entity, const float buffer[3]){	//Tested. Works properly.
	if(IsValidEntity(entity))
		TeleportEntity(entity, buffer, NULL_VECTOR, NULL_VECTOR);
}

public void GetForwardVector(int entity, float buffer[3])	//Tested. Works properly.
{
	if(IsValidEntity(entity))
	{
		float tmp[3];
		GetAngles(entity, tmp);
		GetAngleVectors(tmp, buffer, NULL_VECTOR, NULL_VECTOR);
	}
}

public void SetForwardVector(int entity, const float buffer[3])	//Tested. Works properly.
{
	if(IsValidEntity(entity))
	{
		float tmp[3];
		//GetAngles(entity, tmp);
		//tmp[0] = RadToDeg(-ArcSine(buffer[2]));
		//tmp[1] = RadToDeg(ArcTangent2(buffer[1], buffer[0]));
		GetVectorAngles(buffer, tmp);
		SetAngles(entity, tmp);
	}
}

public void EntFire(const char[] target, const char[] input, const char[] parametr, const char[] delay, int activator){ //Tested. Works properly.
	char output[500];
	int tmp = CreateEntityByName("info_target");
	Format(output, sizeof(output), "OnUser1 %s:%s:%s:%s", target, input, parametr, delay);
	SetVariantString(output);
	AcceptEntityInput(tmp, "AddOutput");
	AcceptEntityInput(tmp, "FireUser1", activator);
	AcceptEntityInput(tmp, "Kill");
}

public void EntFireByIndex(int target, const char[] input, const char[] parametr, const char[] delay, int activator){  //Tested. Works properly.
	if(!IsValidEntity(target))
		return;
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
	char input[250], parametr[250];
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
	return Plugin_Continue;
}
