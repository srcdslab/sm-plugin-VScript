#if defined _MovingNPC_included
 #endinput
#endif
#define _MovingNPC_included
#include <basic>
#include <vscripts>
#include <float>

stock float FloatMod(float num, float denom)
{
    return num - denom * RoundToFloor(num / denom);
}

stock float operator%(float oper1, float oper2)
{
    return FloatMod(oper1, oper2);
}

public bool IsValidPlayer(int player)
{
	return player >= 1 && player <= MAXPLAYERS && IsValidEntity(player) && IsPlayerAlive(player);
}

public float GetDistance(const float[3] v1, const float[3] v2)
{
	return SquareRoot((v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[1] - v2[1]) * (v1[1] - v2[1]) + (v1[2] - v2[2]) * (v1[2] - v2[2]));
}

methodmap MovingNpc < Basic
{
	public MovingNpc(int entity, float tickrate = 0.1, float distance = 5000.0, float retarget = 7.5, float forward_factor = 1.0, float turning_factor = 0.5, float lifetime = 0.0)
	{
		Basic myclass = new Basic();
		myclass.SetFloat("fRate", tickrate);
		myclass.SetFloat("fDistance", distance);
		myclass.SetFloat("fRetarget", retarget);
		myclass.SetFloat("fForward", forward_factor);
		myclass.SetFloat("fTurning", turning_factor);
		myclass.SetFloat("fLifetime", lifetime);
		myclass.SetInt("iEntity", entity);
		myclass.SetInt("iTarget", -1);
		myclass.SetInt("iTf", -1);
		myclass.SetInt("iTs", -1);
		myclass.SetFloat("fTtime", 0.0);
		myclass.SetBool("bTicking", false);
		myclass.SetHandle("hLifeTimer", null);
		myclass.SetBool("bKill", false);
		return view_as<MovingNpc>(myclass);
	}
	property bool kill
	{
		public get()
		{
			return this.GetBool("bKill");
		}
		public set(bool val)
		{
			this.SetBool("bKill", val);
		}
	}
	property Handle lifetimer
	{
		public get()
		{
			return this.GetHandle("hLifeTimer");
		}
		public set(Handle val)
		{
			this.SetHandle("hLifeTimer", val);
		}
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
	property int entity
	{
		public get()
		{
			return this.GetInt("iEntity");
		}
		public set(int val)
		{
			this.SetInt("iEntity", val);
		}
	}
	property int target
	{
		public get()
		{
			return this.GetInt("iTarget");
		}
		public set(int val)
		{
			this.SetInt("iTarget", val);
		}
	}
	property int tf
	{
		public get()
		{
			return this.GetInt("iTf");
		}
		public set(int val)
		{
			this.SetInt("iTf", val);
		}
	}
	property int ts
	{
		public get()
		{
			return this.GetInt("iTs");
		}
		public set(int val)
		{
			this.SetInt("iTs", val);
		}
	}
	property float ttime
	{
		public get()
		{
			return this.GetFloat("fTtime");
		}
		public set(float val)
		{
			this.SetFloat("fTtime", val);
		}
	}
	property bool ticking
	{
		public get()
		{
			return this.GetBool("bTicking");
		}
		public set(bool val)
		{
			this.SetBool("bTicking", val);
		}
	}

	public void Start()
	{
		if(!this.ticking)
		{
			this.ticking = true;
			Vscripts_CreateTimer(this.rate, Tick_Cb, this);
		}
	}
	
	public void Stop()
	{
		if(this.ticking)
		{
			this.ticking = false;
		}
	}
	
	public float GetTargetYaw(const float[3] start, const float[3] target)
	{
		float yaw = 0.00;
		float v[3];
		SubtractVectors(start, target, v);
		float vl = SquareRoot(v[0] * v[0] + v[1] * v[1]);
		yaw = 180.0 * ArcCosine(v[0] / vl) / 3.14159;
		if (v[1] < 0.0)
			yaw = -yaw;
		return yaw;
	}
	
	public void SetThruster(bool fwd, int caller)
	{
		if(fwd)
			this.tf = caller;
		else 
			this.ts = caller;
	}
	
	public void SearchTarget()
	{
		this.ttime = 0.00;
		this.target = -1;
		int h = -1;
		ArrayList candidates = new ArrayList();
		float orig[3];
		Vscripts_GetOrigin(this.entity, orig);
		while (-1 != (h = Vscripts_FindEntityByClassnameWithin(h, "player", orig, this.distance)))
		{
			//check if target is a valid player + CT team(3) + health above 0 (not dead)
			if (GetClientTeam(h) == 3 && IsPlayerAlive(h))
			{
				//check if the target is in sight of the npc (this physbox origin+48 height)
				float t_orig[3];
				Vscripts_GetOrigin(this.entity, orig);
				orig[2] += 40.0;
				Vscripts_GetOrigin(h, t_orig);
				t_orig[2] += 48.0;
				if (Vscripts_TraceLine(orig, t_orig, this.entity) == 1.00)
					candidates.Push(h);	//if everything required is OK, add the target to the list of candidates
			}
		}
		if(candidates.Length == 0)
		{
			delete candidates;
			return;
		}
		this.target = candidates.Get(GetRandomInt(0, candidates.Length - 1));
		
		delete candidates;
	}
	
	public void Tick()
	{
		Vscripts_EntFireByIndex(this.tf, "Deactivate", "", 0.0, -1);
		Vscripts_EntFireByIndex(this.ts, "Deactivate", "", 0.0, -1);
		if (!IsValidPlayer(this.target) || GetClientTeam(this.target) != 3 || this.ttime >= this.retarget)
		{
			this.SearchTarget();
		}
		this.ttime+=this.rate;
		Vscripts_EntFireByIndex(this.tf, "Activate", "", 0.02, -1);
		Vscripts_EntFireByIndex(this.ts, "Activate", "", 0.02, -1);
		if(!IsValidPlayer(this.target))
		{
			Vscripts_CreateTimer(this.rate, Tick_Cb, this);
			return;
		}
		float angl[3], s_orig[3], t_orig[3];
		Vscripts_GetAngles(this.entity, angl);
		Vscripts_GetOrigin(this.entity, s_orig);
		Vscripts_GetOrigin(this.target, t_orig);
		float sa = angl[1];
		float ta = this.GetTargetYaw(s_orig, t_orig);
		float ang = FloatAbs((sa - ta + 360.0) % 360.0);
		if (ang >= 180.0)
			Vscripts_EntFireByIndex(this.ts, "AddOutput", "angles 0 270 0", 0.0, -1);
		else 
			Vscripts_EntFireByIndex(this.ts, "AddOutput", "angles 0 90 0", 0.0, -1);
		float angdif = (sa - ta - 180.0);
		while (angdif > 360.0) { angdif -= 180.0; }
		while (angdif < -180.0) { angdif += 360.0; }
		angdif = FloatAbs(angdif);
		char input[MAX_INPUT_NAME];
		Format(input, sizeof(input), "force %.4f", 3000.0 * this.forward_factor);
		Vscripts_EntFireByIndex(this.tf, "AddOutput", input, 0.0, -1);
		Format(input, sizeof(input), "force %.4f", (3.0 * this.turning_factor) * angdif);
		Vscripts_EntFireByIndex(this.ts, "AddOutput", input, 0.0, -1);
		Vscripts_CreateTimer(this.rate, Tick_Cb, this);
	}
}

public void Tick_Cb(MovingNpc npc)
{
	if(npc.kill)
	{	
		if(npc.lifetimer)
			KillTimer(npc.lifetimer);
		delete npc;
		return;
	}
	
	if(npc.ticking)
	{
		npc.Tick();
	}
	else
	{
		Vscripts_EntFireByIndex(npc.tf, "Deactivate", "", 0.0, -1);
		Vscripts_EntFireByIndex(npc.ts, "Deactivate", "", 0.0, -1);
	}
}
