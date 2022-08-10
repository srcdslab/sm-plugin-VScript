#pragma semicolon 1

#define PLUGIN_AUTHOR "Cloud Strife"
#define PLUGIN_VERSION "1.0b"

#include <sourcemod>
#include <vscripts/MovingNPC>

#pragma newdecls required

ArrayList g_aMovingNpc = null;
ArrayList g_aNpcConfigNT = null;
StringMap g_mNpcConfig = null;

public Plugin myinfo = 
{
	name = "MovingNPC vscripts",
	author = PLUGIN_AUTHOR,
	description = "MovingNPC vscripts",
	version = PLUGIN_VERSION,
	url = "https://steamcommunity.com/id/cloudstrifeua/"
};

//TODO: Add start and stop triggers 

methodmap MovingNpcConfig < Basic
{
	public MovingNpcConfig()
	{
		Basic myclass = new Basic();
		myclass.SetFloat("fRate", 0.1);
		myclass.SetFloat("fDistance", 5000.0);
		myclass.SetFloat("fRetarget", 7.5);
		myclass.SetFloat("fForward", 1.0);
		myclass.SetFloat("fTurning", 0.5);
		myclass.SetFloat("fLifetime", 0.0);
		myclass.SetString("sThrusterFwd", "");
		myclass.SetString("sThrusterSide", "");
		myclass.SetString("sAttachment", "");
		myclass.SetString("sTemplate", "");
		return view_as<MovingNpcConfig>(myclass);
	}
	property float lifetime
	{
		public get()
		{
			return this.GetFloat("fLifetime");
		}
		public set(float val)
		{
			this.SetFloat("fLifetime", val);
		}
	}
	property float rate
	{
		public get()
		{
			return this.GetFloat("fRate");
		}
		public set(float val)
		{
			this.SetFloat("fRate", val);
		}
	}
	property float distance
	{
		public get()
		{
			return this.GetFloat("fDistance");
		}
		public set(float val)
		{
			this.SetFloat("fDistance", val);
		}
	}
	property float retarget
	{
		public get()
		{
			return this.GetFloat("fRetarget");
		}
		public set(float val)
		{
			this.SetFloat("fRetarget", val);
		}
	}
	property float forward_factor
	{
		public get()
		{
			return this.GetFloat("fForward");
		}
		public set(float val)
		{
			this.SetFloat("fForward", val);
		}
	}
	property float turning_factor
	{
		public get()
		{
			return this.GetFloat("fTurning");
		}
		public set(float val)
		{
			this.SetFloat("fTurning", val);
		}
	}
	public int GetThrusterFwd(char[] buffer, int size)
	{
		return this.GetString("sThrusterFwd", buffer, size);
	}
	public void SetThrusterFwd(const char[] sThruster)
	{
		this.SetString("sThrusterFwd", sThruster);
	}
	public int GetThrusterSide(char[] buffer, int size)
	{
		return this.GetString("sThrusterSide", buffer, size);
	}
	public void SetThrusterSide(const char[] sThruster)
	{
		this.SetString("sThrusterSide", sThruster);
	}
	public int GetAttachment(char[] buffer, int size)
	{
		return this.GetString("sAttachment", buffer, size);
	}
	public void SetAttachment(const char[] sAttachment)
	{
		this.SetString("sAttachment", sAttachment);
	}
	public int GetTemplate(char[] buffer, int size)
	{
		return this.GetString("sTemplate", buffer, size);
	}
	public void SetTemplate(const char[] sTemplate)
	{
		this.SetString("sTemplate", sTemplate);
	}
	public void Delete()
	{
		delete this;
	}
}

stock int GetEntityIndex(int entity, const char[] name, const char[] classname = "*")
{
	if(name[0] == '#')
	{
		return Vscripts_GetEntityIndexByHammerID(StringToInt(name[1]), classname, entity);
	}
	else
	{
		return Vscripts_GetEntityIndexByName(name, classname, entity);
	}
}

public bool IsMovingNpcExists(int entity)
{
	for(int i = 0; i < g_aMovingNpc.Length; ++i)
	{
		MovingNpc npc = g_aMovingNpc.Get(i);
		if(npc.entity == entity || npc.tf == entity || npc.ts == entity)
			return true;
	}
	return false;
}

public void KillNpc(MovingNpc npc)
{
	for(int i = 0; i < g_aMovingNpc.Length; ++i)
	{
		MovingNpc cur = g_aMovingNpc.Get(i);
		if(cur.entity == npc.entity)
		{
			npc.Stop();
			g_aMovingNpc.Erase(i);
			break;
		}
	}
	npc.kill = true;
}

public Action OnMovingNpcTimeout(Handle timer, MovingNpc npc)
{
	KillTimer(timer);
	npc.lifetimer = null;
	KillNpc(npc);
	return Plugin_Stop;
}

public void StartNpc(MovingNpc npc)
{
	npc.Start();
	if(npc.lifetime > 0 && !npc.lifetimer)
		npc.lifetimer = CreateTimer(npc.lifetime, OnMovingNpcTimeout, npc, TIMER_FLAG_NO_MAPCHANGE);
}

public int GetEntName(int entity, char[] buffer, int size)
{
	return GetEntPropString(entity, Prop_Data, "m_iName", buffer, size);
}

public int GetEntHammerID(int entity)
{
	return GetEntProp(entity, Prop_Data, "m_iHammerID");
}


stock bool MatchTrigger(int entity, const char[] sTrigger, bool namefixup = false)
{
	if(sTrigger[0] == '#')
	{
		return StringToInt(sTrigger[1]) == GetEntHammerID(entity);
	} else
	{
		char name[MAX_ENT_NAME];
		GetEntName(entity, name, sizeof(name));
		if(!name[0])
			return false;
		if(namefixup)
		{
			int c = FindCharInString(name, '&', true);
			if(c != -1)
			{
				name[c] = '\0';
			}
		}
		return StrEqual(name, sTrigger);
	}
}

public void OnMapStart()
{
	char sConfigPath[PLATFORM_MAX_PATH];
	char sCurMap[PLATFORM_MAX_PATH];
	GetCurrentMap(sCurMap, sizeof(sCurMap));
	BuildPath(Path_SM, sConfigPath, sizeof(sConfigPath), "configs/movingnpc/%s.cfg", sCurMap);
	if(!FileExists(sConfigPath))
	{
		GetPluginFilename(INVALID_HANDLE, sCurMap, sizeof(sCurMap));
		ServerCommand("sm plugins unload %s", sCurMap);
		return;
	}
	KeyValues Config = new KeyValues("npc");
	if(!Config.ImportFromFile(sConfigPath))
	{
		LogMessage("ImportFromFile() failed for map %s!", sCurMap);
		GetPluginFilename(INVALID_HANDLE, sCurMap, sizeof(sCurMap));
		ServerCommand("sm plugins unload %s", sCurMap);
		return;
	}
	Config.Rewind();
	if(!Config.GotoFirstSubKey(true))
	{
		LogMessage("The current map does not have any moving npcs configured.");
		GetPluginFilename(INVALID_HANDLE, sCurMap, sizeof(sCurMap));
		ServerCommand("sm plugins unload %s", sCurMap);
		return;
	}
	g_mNpcConfig = new StringMap();
	g_aNpcConfigNT = new ArrayList();
	do
	{
		MovingNpcConfig NpcConf = new MovingNpcConfig();
		char buffer[MAX_ENT_NAME + MAX_INPUT_NAME];
		Config.GetString("thruster_forward", buffer, sizeof(buffer), "");
		if(!buffer[0])
		{
			delete NpcConf;
			LogMessage("Could not find \"thruster_forward\" in config for map %s", sCurMap);
			continue;
		}
		NpcConf.SetThrusterFwd(buffer);
		
		Config.GetString("thruster_side", buffer, sizeof(buffer), "");
		if(!buffer[0])
		{
			delete NpcConf;
			LogMessage("Could not find \"thruster_side\" in config for map %s", sCurMap);
			continue;
		}
		NpcConf.SetThrusterSide(buffer);
		
		Config.GetString("attachment", buffer, sizeof(buffer), "");
		if(!buffer[0])
		{
			delete NpcConf;
			LogMessage("Could not find \"attachment\" in config for map %s", sCurMap);
			continue;
		}
		NpcConf.SetAttachment(buffer);
		
		char sTemplate[MAX_ENT_NAME];
		Config.GetString("template", sTemplate, sizeof(sTemplate), "");
		NpcConf.SetTemplate(sTemplate);
		NpcConf.rate = Config.GetFloat("tickrate", 0.1);
		NpcConf.distance = Config.GetFloat("distance", 5000.0);
		NpcConf.retarget = Config.GetFloat("retarget", 7.5);
		NpcConf.forward_factor = Config.GetFloat("forward_factor", 1.0);
		NpcConf.turning_factor = Config.GetFloat("turning_factor", 0.5);
		NpcConf.lifetime = Config.GetFloat("lifetime", 0.0);
		if(sTemplate[0])
		{
			ArrayList tmp;
			if(!g_mNpcConfig.GetValue(sTemplate, tmp))
			{
				tmp = new ArrayList();
			}
			tmp.Push(NpcConf);
			g_mNpcConfig.SetValue(sTemplate, tmp, true);
		} else
		{
			g_aNpcConfigNT.Push(NpcConf);
		}
	} while(Config.GotoNextKey(true));
	delete Config;
	g_aMovingNpc = new ArrayList();
	HookEvent("round_start", OnRoundStart, EventHookMode_PostNoCopy);
}

public void OnRoundStart(Event event, const char[] name, bool dontBroadcast)
{
	for(int i = 0; i < g_aNpcConfigNT.Length; ++i)
	{
		MovingNpcConfig NpcConf = g_aNpcConfigNT.Get(i);
		char sThrusterFwd[MAX_ENT_NAME], sThrusterSide[MAX_ENT_NAME], sAttachment[MAX_ENT_NAME];
		NpcConf.GetThrusterFwd(sThrusterFwd, sizeof(sThrusterFwd));
		NpcConf.GetThrusterSide(sThrusterSide, sizeof(sThrusterSide));
		NpcConf.GetAttachment(sAttachment, sizeof(sAttachment));
		int thruster_fwd = -1, thruster_side = -1, attachment = -1;
		while((thruster_fwd = GetEntityIndex(thruster_fwd, sThrusterFwd, "phys_thruster")) != -1)
		{
			if(IsMovingNpcExists(thruster_fwd))
				continue;
			do {
				thruster_side = GetEntityIndex(thruster_side, sThrusterSide, "phys_thruster");
			} while (thruster_side != -1 && IsMovingNpcExists(thruster_side));
			
			do {
				attachment = GetEntityIndex(attachment, sAttachment);
			} while (attachment != -1 && IsMovingNpcExists(attachment));

			if(thruster_side != -1 && attachment != -1)
			{
				NewMovingNpc(NpcConf, attachment, thruster_fwd, thruster_side);
			}
		}
	}	
}

stock void NewMovingNpc(MovingNpcConfig NpcConf, int attachment, int thruster_fwd, int thruster_side)
{
	MovingNpc npc = new MovingNpc(attachment, NpcConf.rate, NpcConf.distance, NpcConf.retarget, NpcConf.forward_factor, NpcConf.turning_factor, NpcConf.lifetime);
	npc.SetThruster(true, thruster_fwd);
	npc.SetThruster(false, thruster_side);
	StartNpc(npc);
	g_aMovingNpc.Push(npc);
}

public void Vscritps_OnTemplateInstanceCreated(int template, const int[] createdEntities, int size)
{
	if(!g_mNpcConfig)
		return;
	
	char sTemplate[MAX_ENT_NAME];
	GetEntName(template, sTemplate, sizeof(sTemplate));
	ArrayList configs;
	if(!g_mNpcConfig.GetValue(sTemplate, configs))
	{
		Format(sTemplate, sizeof(sTemplate), "#%d", GetEntHammerID(template));
		if(!g_mNpcConfig.GetValue(sTemplate, configs))
		{
			return;
		}
	}
	for(int i = 0; i < configs.Length; ++i)
	{
		int attachment = -1, thruster_fwd = -1, thruster_side = -1;
		MovingNpcConfig npcConf = configs.Get(i);
		char sAttachment[MAX_ENT_NAME], sThrusterFwd[MAX_ENT_NAME], sThrusterSide[MAX_ENT_NAME];
		npcConf.GetAttachment(sAttachment, sizeof(sAttachment));
		npcConf.GetThrusterFwd(sThrusterFwd, sizeof(sThrusterFwd));
		npcConf.GetThrusterSide(sThrusterSide, sizeof(sThrusterSide));
		for(int e = 0; e < size; ++e)
		{
			int entity = createdEntities[e];
			if(MatchTrigger(entity, sAttachment, true))
			{
				attachment = entity;
			} else if(MatchTrigger(entity, sThrusterFwd, true))
			{
				thruster_fwd = entity;
			} else if(MatchTrigger(entity, sThrusterSide, true))
			{
				thruster_side = entity;
			}
		}
		if(attachment != -1 && thruster_fwd != -1 && thruster_side != -1)
		{
			NewMovingNpc(npcConf, attachment, thruster_fwd, thruster_side);
			break;
		}
	}
}

public void OnEntityDestroyed(int entity)
{
	if(!g_aMovingNpc)
		return;
	for (int i = 0; i < g_aMovingNpc.Length; ++i)
	{
		MovingNpc npc = g_aMovingNpc.Get(i);
		if(npc.entity == entity || npc.tf == entity || npc.ts == entity)
		{	
			npc.Stop();
			npc.kill = true;
			g_aMovingNpc.Erase(i);
			break;
		}
	}
}


public void OnMapEnd()
{
	UnhookEvent("round_start", OnRoundStart, EventHookMode_PostNoCopy);
	if(g_aMovingNpc)
	{
		for (int i = 0; i < g_aMovingNpc.Length; ++i)
		{
			MovingNpc npc = g_aMovingNpc.Get(i);
			npc.Stop();
			npc.kill = true;
		}
		delete g_aMovingNpc;
	}
	if(g_aNpcConfigNT)
	{
		for (int i = 0; i < g_aNpcConfigNT.Length; ++i)
		{
			MovingNpcConfig NpcConf = g_aNpcConfigNT.Get(i);
			NpcConf.Delete();
		}
		delete g_aNpcConfigNT;
	}
	if(g_mNpcConfig)
	{
		StringMapSnapshot ms = g_mNpcConfig.Snapshot();
		char sTemplate[MAX_ENT_NAME];
		for(int i = 0; i < ms.Length; ++i)
		{
			ms.GetKey(i, sTemplate, sizeof(sTemplate));
			ArrayList configs;
			g_mNpcConfig.GetValue(sTemplate, configs);
			for(int c = 0; c < configs.Length; ++c)
			{
				MovingNpcConfig NpcConf = configs.Get(c);
				NpcConf.Delete();
			}
			delete configs;
		}
		delete ms;
		g_mNpcConfig.Clear();
		delete g_mNpcConfig;
	}
}
