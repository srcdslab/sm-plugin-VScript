#pragma semicolon 1

#define PLUGIN_AUTHOR "Cloud Strife"
#define PLUGIN_VERSION "1.00"

#include <vscripts>

#define MAP_NAME "ze_geometric_v1_4s"

#pragma newdecls required

bool bValidMap = false;

public Plugin myinfo = 
{
	name = "Geometric vscripts",
	author = PLUGIN_AUTHOR,
	description = "Geometric vscripts",
	version = PLUGIN_VERSION,
	url = "https://steamcommunity.com/id/cloudstrifeua/"
};

void RandomSpawn(int entity, int _x1, int _x2, int _y1, int _y2, int _z1, int _z2, float _c1, float _c2, float _c3)
{
	float orig[3], buf[3];
	Vscripts_GetOrigin(entity, orig);
	buf[0] = GetRandomInt(_x1, _x2) * _c1;
	buf[1] = GetRandomInt(_y1, _y2) * _c2;
	buf[2] = GetRandomInt(_z1, _z2) * _c3;
	AddVectors(orig, buf, buf);
	Vscripts_SetOrigin(entity, buf);
	AcceptEntityInput(entity, "ForceSpawn");
	Vscripts_SetOrigin(entity, orig);
}

public void OnMapStart()
{
	char sCurMap[256];
	GetCurrentMap(sCurMap, sizeof(sCurMap));
	bValidMap = (strcmp(sCurMap, MAP_NAME, false) == 0);
	if(bValidMap)
	{
		HookEvent("round_start", OnRoundStart, EventHookMode_PostNoCopy);
	}
	else
    {
        GetPluginFilename(INVALID_HANDLE, sCurMap, sizeof(sCurMap));

        ServerCommand("sm plugins unload %s", sCurMap);
    }
}

public void OnRoundStart(Event event, const char[] name, bool dontBroadcast)
{
	int tmp = Vscripts_GetEntityIndexByName("boss_skill04_maker");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn1);
	tmp = Vscripts_GetEntityIndexByName("sct_final_danmaku_maker");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn2);
	tmp = Vscripts_GetEntityIndexByName("sct_final_circle_maker");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn2);
	tmp = Vscripts_GetEntityIndexByName("boss_skill07_maker");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn1);
	tmp = Vscripts_GetEntityIndexByName("boss_skill03_maker");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn1);
	tmp = Vscripts_GetEntityIndexByName("sct_low_speed_maker");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn3);
	tmp = Vscripts_GetEntityIndexByName("square_laser_maker1");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn4);
	tmp = Vscripts_GetEntityIndexByName("square_laser_maker2");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn5);
	tmp = Vscripts_GetEntityIndexByName("square_laser_maker3");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn4);
	tmp = Vscripts_GetEntityIndexByName("square_laser_maker4");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn5);
	tmp = Vscripts_GetEntityIndexByName("triangel_final_maker");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn6);
	tmp = Vscripts_GetEntityIndexByName("circle_final_maker");
	HookSingleEntityOutput(tmp, "OnUser1", OnRandomSpawn6);
}

public void OnRandomSpawn1(const char[] output, int caller, int activator, float delay)
{
	RandomSpawn(caller, -1088, 1088, -1088, 1088, 0, 0, 1.0, 1.0, 1.0);
}

public void OnRandomSpawn2(const char[] output, int caller, int activator, float delay)
{
	RandomSpawn(caller, 0, 0, -256, 256, 0, 0, 1.0, 1.0, 1.0);
}

public void OnRandomSpawn3(const char[] output, int caller, int activator, float delay)
{
	RandomSpawn(caller, -224, 224, 0, 0, 0, 0, 1.0, 1.0, 1.0);
}

public void OnRandomSpawn4(const char[] output, int caller, int activator, float delay)
{
	RandomSpawn(caller, 0, 0, -448, 448, 0, 0, 1.0, 1.0, 1.0);
}

public void OnRandomSpawn5(const char[] output, int caller, int activator, float delay)
{
	RandomSpawn(caller, -448, 448, 0, 0, 0, 0, 1.0, 1.0, 1.0);
}

public void OnRandomSpawn6(const char[] output, int caller, int activator, float delay)
{
	RandomSpawn(caller, -256, 256, -256, 256, 0, 0, 1.0, 1.0, 1.0);
}

public void OnMapEnd()
{
	if(bValidMap)
	{
		UnhookEvent("round_start", OnRoundStart, EventHookMode_PostNoCopy);
	}
	bValidMap = false;
}