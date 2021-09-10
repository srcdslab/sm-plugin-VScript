#pragma semicolon 1

#define DEBUG

#define PLUGIN_AUTHOR "Cloud Strife"
#define PLUGIN_VERSION "1.00"
#define MAP_NAME "ze_a_e_s_t_h_e_t_i_c_v1_1s"

#include <sourcemod>
#include <sdktools>
#include <vscripts/Aesthetic>

#pragma newdecls required

bool bValidMap = false;
Chess g_Chess = null;
ArrayList g_aEyes = null;

public Plugin myinfo = 
{
	name = "Aesthetic vscripts",
	author = PLUGIN_AUTHOR,
	description = "",
	version = PLUGIN_VERSION,
	url = "https://steamcommunity.com/id/cloudstrifeua/"
};

public void OnMapStart()
{
	char sCurMap[256];
	GetCurrentMap(sCurMap, sizeof(sCurMap));
	bValidMap = (strcmp(sCurMap, MAP_NAME, false) == 0);
	if(bValidMap)
	{
		HookEvent("round_start", OnRoundStart, EventHookMode_PostNoCopy);
		HookEvent("round_end", OnRoundEnd, EventHookMode_PostNoCopy);
		g_aEyes = new ArrayList();
	}
	else
    {
        GetPluginFilename(INVALID_HANDLE, sCurMap, sizeof(sCurMap));

        ServerCommand("sm plugins unload %s", sCurMap);
    }
}

public void OnRoundStart(Event event, const char[] name, bool dontBroadcast)
{
	if(!bValidMap)
		return;
	g_Chess = new Chess(Vscripts_GetEntityIndexByName("GreenStart", "path_track"), Vscripts_GetEntityIndexByName("GreenEnd", "path_track"), 
						Vscripts_GetEntityIndexByName("PurpleStart", "path_track"), Vscripts_GetEntityIndexByName("PurpleEnd", "path_track"));
	int tmp = Vscripts_GetEntityIndexByName("GreenEnd", "path_track");
	
	HookSingleEntityOutput(tmp, "OnPass", OnGreenEndPass);
	tmp = Vscripts_GetEntityIndexByName("PurpleEnd", "path_track");
	
	HookSingleEntityOutput(tmp, "OnPass", OnPurpleEndPass);
	tmp = Vscripts_GetEntityIndexByHammerID(92835, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnPurpleButton1);
	tmp = Vscripts_GetEntityIndexByHammerID(92838, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnPurpleButton2);
	tmp = Vscripts_GetEntityIndexByHammerID(92841, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnPurpleButton3);
	tmp = Vscripts_GetEntityIndexByHammerID(92844, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnPurpleButton4);
	tmp = Vscripts_GetEntityIndexByHammerID(92850, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnPurpleButton5);
	tmp = Vscripts_GetEntityIndexByHammerID(92847, "func_button");
	HookSingleEntityOutput(tmp, "OnPressed", OnPurpleButton6);
	tmp = Vscripts_GetEntityIndexByHammerID(92853, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnPurpleButton7);
	tmp = Vscripts_GetEntityIndexByHammerID(92856, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnPurpleButton8);
	tmp = Vscripts_GetEntityIndexByHammerID(92013, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnGreenButton1);
	tmp = Vscripts_GetEntityIndexByHammerID(92084, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnGreenButton2);
	tmp = Vscripts_GetEntityIndexByHammerID(91906, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnGreenButton3);
	tmp = Vscripts_GetEntityIndexByHammerID(92081, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnGreenButton4);
	tmp = Vscripts_GetEntityIndexByHammerID(92016, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnGreenButton5);
	tmp = Vscripts_GetEntityIndexByHammerID(92075, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnGreenButton6);
	tmp = Vscripts_GetEntityIndexByHammerID(89814, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnGreenButton7);
	tmp = Vscripts_GetEntityIndexByHammerID(92046, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnGreenButton8);
	tmp = Vscripts_GetEntityIndexByHammerID(89040, "func_button");
	
	HookSingleEntityOutput(tmp, "OnPressed", OnPressedTrig, true);
	tmp = Vscripts_GetEntityIndexByHammerID(490936, "trigger_push");
	
	HookSingleEntityOutput(tmp, "OnStartTouch", OnPushTrigger);
	tmp = Vscripts_GetEntityIndexByHammerID(445713, "trigger_push");
	
	HookSingleEntityOutput(tmp, "OnStartTouch", OnPushTrigger2);
}

public void OnPushTrigger2(const char[] output, int caller, int activator, float delay)
{
	float tmp[3];
	Vscripts_GetOrigin(activator, tmp);
	tmp[2] += 2.0;
	Vscripts_SetOrigin(activator, tmp);
}

public void OnPushTrigger(const char[] output, int caller, int activator, float delay)
{
	float tmp[3];
	Vscripts_GetOrigin(activator, tmp);
	tmp[2] += 4.0;
	Vscripts_SetOrigin(activator, tmp);
}

public void OnPressedTrig(const char[] output, int caller, int activator, float delay)
{
	delete g_Chess;
}

public void OnGreenButton8(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MoveGreen(-1.0, -1.0);
	}
}

public void OnGreenButton7(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MoveGreen(0.0, -1.0);
	}
}

public void OnGreenButton6(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MoveGreen(1.0, -1.0);
	}
}

public void OnGreenButton5(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MoveGreen(1.0, 0.0);
	}
}

public void OnGreenButton4(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{	
		g_Chess.MoveGreen(1.0, 1.0);
	}
}

public void OnGreenButton3(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MoveGreen(0.0, 1.0);
	}
}

public void OnGreenButton2(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MoveGreen(-1.0, 1.0);
	}
}

public void OnGreenButton1(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MoveGreen(-1.0, 0.0);
	}
}

public void OnPurpleButton8(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MovePurple(-1.0, -1.0);
	}
}

public void OnPurpleButton7(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MovePurple(0.0, -1.0);
	}
}

public void OnPurpleButton6(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MovePurple(1.0, -1.0);
	}
}

public void OnPurpleButton5(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MovePurple(1.0, 0.0);
	}
}

public void OnPurpleButton4(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MovePurple(1.0, 1.0);
	}
}

public void OnPurpleButton3(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MovePurple(0.0, 1.0);
	}
}

public void OnPurpleButton2(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MovePurple(-1.0, 1.0);
	}
}

public void OnPurpleButton1(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MovePurple(-1.0, 0.0);
	}
}

public void OnGreenEndPass(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MoveStartToEnd(g_Chess.greenStart, g_Chess.greenEnd);
		g_Chess.CheckInLove();
	}
}

public void OnPurpleEndPass(const char[] output, int caller, int activator, float delay)
{
	if(g_Chess)
	{
		g_Chess.MoveStartToEnd(g_Chess.purpleStart, g_Chess.purpleEnd);
		g_Chess.CheckInLove();
	}
}

public void OnEntitySpawned(int entity, const char[] classname)
{
	if(!bValidMap)
		return;
	if(IsValidEntity(entity))
	{
		if(strcmp(classname, "func_breakable") == 0)
		{
			char sName[128];
			GetEntPropString(entity, Prop_Data, "m_iName", sName, sizeof(sName));
			if(!sName[0])
				return;
			int pos = FindCharInString(sName, '&', true);
			if(pos != -1)
			{	sName[pos] = 0;
				if(strcmp(sName, "EyeBoss") == 0)
				{
					
					Eye eye = new Eye(entity);
					HookSingleEntityOutput(entity, "OnUser2", OnEyeMove);
					g_aEyes.Push(eye);
				}
			}
		}
	}
}

public void OnEyeMove(const char[] output, int caller, int activator, float delay)
{
	for (int i = 0; i < g_aEyes.Length; i++)
	{
		Eye tmp = view_as<Eye>(g_aEyes.Get(i));
		if(tmp.entity == caller)
		{
			tmp.Move();
		}
	}
}

public void OnEntityDestroyed(int entity)
{
	if(!bValidMap)
		return;
	if(IsValidEntity(entity))
	{
		char sClassname[64];
		GetEntityClassname(entity, sClassname, sizeof(sClassname));
		if(strcmp(sClassname, "func_breakable") == 0)
		{
			char sName[128];
			GetEntPropString(entity, Prop_Data, "m_iName", sName, sizeof(sName));
			if(!sName[0])
				return;
			if(StrContains(sName, "EyeBoss") != -1)
			{
				for (int i = 0; i < g_aEyes.Length; i++)
				{
					Eye tmp = view_as<Eye>(g_aEyes.Get(i));
					if(entity == tmp.entity)
					{
						delete tmp;
						g_aEyes.Erase(i);
					}
				}
			}
		}
	}
}

public void Cleanup()
{
	if(!bValidMap)
		return;
	
	delete g_Chess;
	for (int i = 0; i < g_aEyes.Length; i++)
	{
		Eye tmp = view_as<Eye>(g_aEyes.Get(i));
		delete tmp;
		g_aEyes.Erase(i);
	}
}

public void OnRoundEnd(Event event, const char[] name, bool dontBroadcast)
{
	Cleanup();
}

public void OnMapEnd()
{
	Cleanup();
	if(bValidMap)
	{
		UnhookEvent("round_start", OnRoundStart, EventHookMode_PostNoCopy);
		UnhookEvent("round_end", OnRoundEnd, EventHookMode_PostNoCopy);
	}
	bValidMap = false;
}