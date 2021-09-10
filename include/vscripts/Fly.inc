/*
**
*/
#if defined _FLY_MM_included
 #endinput
#endif
#define _FLY_MM_included
#include <basic>
#include <vscripts>
#include <adt_array>
#include <float>

float TOASTER_POSITION[] =  { 7063.0, 2329.0, -360.0 };

const int t_len = 6;
float TARGET_NODES[][] = {	
							{-2400.0, 5136.0, -611.0},
							{5824.0, 2337.0, -64.0},
							{3600.0, 4496.0, -256.0},
							{3456.0, 7680.0, -375.98},
							{2080.0, 9790.1, -2696.0},
							{1100.99, 10777.0, -2680.0}
						};

//const int t_len_0 = 6, t_len_1 = 1;
//float TARGET_NODES2[][][] = {
					//{	
						//{5756.0, 4749.0, -390.0},
						//{6000.0, 2336.0, -288.0},
						//{3608.0, 4496.0, -472.0},
						//{3456.0, 7680.0, -375.98},
						//{2080.0, 9790.1, -2696.0},
						//{1100.99, 10777.0, -2680.0}
					//},
					//{
						//{895.99, 11192.0, -2216.0},
						//{0.0, 0.0, 0.0},
						//{0.0, 0.0, 0.0},
						//{0.0, 0.0, 0.0},
						//{0.0, 0.0, 0.0},
						//{0.0, 0.0, 0.0}
					//}
				//};

public bool IsValidPlayer(int player)
{
	return player >= 1 && player <= MAXPLAYERS && IsValidEntity(player) && IsPlayerAlive(player);
}

methodmap Fly_base < Basic
{
	public Fly_base(int entity)
	{
		Basic myclass = new Basic();
		myclass.SetInt("iEntity", entity);
		myclass.SetFloat("fSpeed", 5.0);
		myclass.SetFloat("fRotation_speed", 0.01);
		myclass.SetFloat("fPrevious_distance_to_target", -1.0);
		myclass.SetFloat("fCurrent_distance_to_target", -1.0);
		myclass.SetFloat("fBlocker1", 0.0);
		myclass.SetFloat("fBlocker2", 100.0);
		myclass.SetFloat("fRot_speed_min", 0.01);
		myclass.SetFloat("fRot_speed_max", 0.04);
		myclass.SetFloat("fRot_speed_acceleration", 0.0001);
		myclass.SetFloat("fRot_err", 0.03);
		myclass.SetFloat("fSpeed_acceleration", 0.06);
		myclass.SetFloat("fMax_speed", 10.0);
		myclass.SetBool("bStarted", false);
		return view_as<Fly_base>(myclass);
	}
	
	property bool started
	{
		public get()
		{
			return this.GetBool("bStarted");
		}
		public set(bool val)
		{
			this.SetBool("bStarted", val);
		}
	}
	
	property float max_speed
	{
		public get()
		{
			return this.GetFloat("fMax_speed");
		}
		public set(float val)
		{
			this.SetFloat("fMax_speed", val);
		}
	}
	
	property float speed_acceleration
	{
		public get()
		{
			return this.GetFloat("fSpeed_acceleration");
		}
		public set(float val)
		{
			this.SetFloat("fSpeed_acceleration", val);
		}
	}
	
	property float rot_err
	{
		public get()
		{
			return this.GetFloat("fRot_err");
		}
		public set(float val)
		{
			this.SetFloat("fRot_err", val);
		}
	}
	
	property float rot_speed_acceleration
	{
		public get()
		{
			return this.GetFloat("fRot_speed_acceleration");
		}
		public set(float val)
		{
			this.SetFloat("fRot_speed_acceleration", val);
		}
	}
	
	property float rot_speed_min
	{
		public get()
		{
			return this.GetFloat("fRot_speed_min");
		}
		public set(float val)
		{
			this.SetFloat("fRot_speed_min", val);
		}
	}
	
	property float rot_speed_max
	{
		public get()
		{
			return this.GetFloat("fRot_speed_max");
		}
		public set(float val)
		{
			this.SetFloat("fRot_speed_max", val);
		}
	}
	
	property float blocker1
	{
		public get()
		{
			return this.GetFloat("fBlocker1");
		}
		public set(float val)
		{
			this.SetFloat("fBlocker1", val);
		}
	}
	
	property float blocker2
	{
		public get()
		{
			return this.GetFloat("fBlocker2");
		}
		public set(float val)
		{
			this.SetFloat("fBlocker2", val);
		}
	}
	
	property int entity 
	{
		public get()
		{
			return this.GetInt("iEntity");
		}
	}
	
	property float speed 
	{
		public get()
		{
			return this.GetFloat("fSpeed");
		}
		public set(float val)
		{
			this.SetFloat("fSpeed", val);
		}
	}
	
	property float rotation_speed 
	{
		public get()
		{
			return this.GetFloat("fRotation_speed");
		}
		public set(float val)
		{
			this.SetFloat("fRotation_speed", val);
		}
	}
	
	property float previous_distance_to_target 
	{
		public get()
		{
			return this.GetFloat("fPrevious_distance_to_target");
		}
		public set(float val)
		{
			this.SetFloat("fPrevious_distance_to_target", val);
		}
	}
	
	property float current_distance_to_target
	{
		public get()
		{
			return this.GetFloat("fCurrent_distance_to_target");
		}
		public set(float val)
		{
			this.SetFloat("fCurrent_distance_to_target", val);
		}
	}
	public void MoveTowardsTarget(const float[3] fly, const float[3] target)
	{
		float targetDir[3];
		SubtractVectors(target, fly, targetDir);
		NormalizeVector(targetDir, targetDir);
		float currentDir[3];
		Vscripts_GetForwardVector(this.entity, currentDir);
		float dir[3];
		GetNewDir(this, targetDir, currentDir, dir);
		dir[2] = targetDir[2];
		Vscripts_SetForwardVector(this.entity, dir);
		MoveForward(this, this.blocker1, this.blocker2);
	}
}

methodmap Fly_Small < Fly_base
{
	public Fly_Small(int entity)
	{
		Fly_base myclass = new Fly_base(entity);
		myclass.SetBool("bDead", false);
		myclass.SetInt("iRetarget", 10);
		myclass.SetInt("iTarget", -1);
		return view_as<Fly_Small>(myclass);
	}
	
	property bool dead
	{
		public get()
		{
			return this.GetBool("bDead");
		}
		public set(bool val)
		{
			this.SetBool("bDead", val);
		}
	}
	
	property int retarget
	{
		public get()
		{
			return this.GetInt("iRetarget");
		}
		public set(int val)
		{
			this.SetInt("iRetarget", val);
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
	
	public void Start()
	{
		
		float currentAngle[3];
		Vscripts_GetForwardVector(this.entity, currentAngle);
		currentAngle[2] = 0.0;
		Vscripts_SetForwardVector(this.entity, currentAngle);
		float orig[3];
		Vscripts_GetOrigin(this.entity, orig);
		orig[2] += 40.0;
		Vscripts_SetOrigin(this.entity, orig);
		
		this.started = true;
	}
	
	public void Tick()
	{
		
		float flyPos[3], flyPos_copy[3];
		Vscripts_GetOrigin(this.entity, flyPos);
		Vscripts_GetOrigin(this.entity, flyPos_copy);
		flyPos_copy[2] -= 300.0;
		if (this.dead)
		{	
			float distToFloor = Vscripts_TraceLine(flyPos, flyPos_copy, this.entity);
			if (distToFloor < 0.03)
			{
				
				Vscripts_EntFireByIndex(this.entity, "SetAnimation", "dead", 0.0, -1);
				Vscripts_EntFireByIndex(this.entity, "SetAnimation", "dead_loop", 2.0, -1);
				Vscripts_EntFireByIndex(this.entity, "kill", "", 20.0, -1);
				this.started = false;
				return;
			}
			else
			{
				
				MoveDir(this, {0.0, 0.0, -1.0});
			}
		}
	
		/*** Move towards a targetted player ***/
		else if(IsValidPlayer(this.target))
		{
			
			ChasePlayer(this, flyPos);
		}
	
		/*** Search for a target player ***/
		else
		{
			
			this.target = GetNewTarget(this);
			flyPos_copy[2] += 400.0;
			if (this.target == -1 && Vscripts_TraceLine(flyPos, flyPos_copy, this.entity) >= 1.0)
			{
				MoveDir(this, {0.0, 0.0, 1.0});
			}
		}
	}
	public void Die()
	{
		this.dead = true;
		this.speed = 0.0;
	}
}

methodmap Fly < Fly_Small
{
	public Fly(int entity)
	{
		Fly_Small myclass = new Fly_Small(entity);
		myclass.speed = 7.0;
		myclass.retarget = 8;
		myclass.blocker1 = 60.0;
		myclass.blocker2 = 170.0;
		myclass.rot_speed_min = 0.005;
		myclass.rot_speed_max = 0.02;
		myclass.speed_acceleration = 0.05;
		myclass.max_speed = 15.0;
		myclass.SetBool("bGrabbed_player", false);
		myclass.SetBool("bReturn_to_toaster", false);
		myclass.SetBool("bSpawn_eggs", false);
		myclass.SetInt("iHealth", 1000);
		myclass.SetInt("iPlayers_in_arena", 0);
		myclass.SetInt("iEggs_currently_spawned", 0);
		myclass.SetInt("iMax_spawned_eggs", 0);
		myclass.SetHandle("lGrabbed_players", new ArrayList());
		myclass.SetHandle("lGrabbed_players_dead", new ArrayList());
		return view_as<Fly>(myclass);
	}

	property bool grabbed_player
	{
		public get()
		{
			return this.GetBool("bGrabbed_player");
		}
		public set(bool val)
		{
			this.SetBool("bGrabbed_player", val);
		}
	}
	property bool return_to_toaster
	{
		public get()
		{
			return this.GetBool("bReturn_to_toaster");
		}
		public set(bool val)
		{
			this.SetBool("bReturn_to_toaster", val);
		}
	}
	property bool spawn_eggs
	{
		public get()
		{
			return this.GetBool("bSpawn_eggs");
		}
		public set(bool val)
		{
			this.SetBool("bSpawn_eggs", val);
		}
	}
	property int health
	{
		public get()
		{
			return this.GetInt("iHealth");
		}
		public set(int val)
		{
			this.SetInt("iHealth", val);
		}
	}
	property int players_in_arena
	{
		public get()
		{
			return this.GetInt("iPlayers_in_arena");
		}
		public set(int val)
		{
			this.SetInt("iPlayers_in_arena", val);
		}
	}
	property int eggs_currently_spawned
	{
		public get()
		{
			return this.GetInt("iEggs_currently_spawned");
		}
		public set(int val)
		{
			this.SetInt("iEggs_currently_spawned", val);
		}
	}
	property int max_spawned_eggs
	{
		public get()
		{
			return this.GetInt("iMax_spawned_eggs");
		}
		public set(int val)
		{
			this.SetInt("iMax_spawned_eggs", val);
		}
	}
	property ArrayList grabbed_players
	{
		public get()
		{
			return view_as<ArrayList>(this.GetHandle("lGrabbed_players"));
		}
		public set(ArrayList list)
		{
			this.SetHandle("lGrabbed_players", list);
		}
	}
	property ArrayList grabbed_players_dead
	{
		public get()
		{
			return view_as<ArrayList>(this.GetHandle("lGrabbed_players_dead"));
		}
		public set(ArrayList list)
		{
			this.SetHandle("lGrabbed_players_dead", list);
		}
	}
	
	public void TeleportGrabbedPlayers(const float[3] position)
	{
		for (int i = 0; i < this.grabbed_players.Length; i++)
		{
			if (IsValidPlayer(this.grabbed_players.Get(i)))
			{
				if(GetClientTeam(this.grabbed_players.Get(i)) == 3)
					Vscripts_SetOrigin(this.grabbed_players.Get(i), position);
			}
		}	
	}
	
	public bool IsValidTarget()
	{
		return IsValidPlayer(this.target) && !IsElementInArray(this.target, this.grabbed_players) && !IsElementInArray(this.target, this.grabbed_players_dead);
	}
	
	public void Start()
	{
		this.max_spawned_eggs = this.players_in_arena / 6;
		this.started = true;
		Vscripts_EntFireByIndex(this.entity, "SetAnimation", "fly", 0.00, -1);
	}
	
	public void KillFly()
	{
		delete this.grabbed_players_dead;
		delete this.grabbed_players;
		delete this;
	}
	
	public void Tick()
	{
		float flyPos[3], flyPos_copy[3];
		Vscripts_GetOrigin(this.entity, flyPos);
		Vscripts_GetOrigin(this.entity, flyPos_copy);
		flyPos_copy[2] -= 300.0;
		float distToFloor = Vscripts_TraceLine(flyPos, flyPos_copy, this.entity);
		
		/*****************************/
		/*** Stuff done every tick ***/
		/*****************************/
		
		/*** Grab a near player ***/
		int playerNear;
		float point[3], fwd[3];
		Vscripts_GetForwardVector(this.entity, fwd);
		ScaleVector(fwd, 80.0);
		AddVectors(flyPos, fwd, point);
		
		if(!this.dead && (playerNear = Vscripts_FindEntityByClassnameWithin(playerNear, "player", point, 64.0)) != -1)
		{
			if(GetClientTeam(playerNear) == 3)
			{
				if(!IsElementInArray(playerNear, this.grabbed_players) && !IsElementInArray(playerNear, this.grabbed_players_dead))
				{
					this.grabbed_players.Push(playerNear);
					this.grabbed_players_dead.Push(playerNear);
		
					if ((this.target = GetNewTarget(this)) == -1 || !this.IsValidTarget())
						this.return_to_toaster = true;
					else if (!this.grabbed_player)
						Vscripts_EntFireByIndex(this.entity, "FireUser2", "", 15.0, -1);
					if (playerNear == this.target)
						this.target = -1;
		
					this.grabbed_player = true;
				}
			}
		}

		/*** Teleport grabbed players ***/
		if (!this.dead)
		{
			flyPos_copy[2] += 260;
			this.TeleportGrabbedPlayers(flyPos_copy);
		}
		
		/*** Random chance to spawn an egg ***/
		if (!this.grabbed_player && this.eggs_currently_spawned != this.max_spawned_eggs && GetRandomInt(0, 500) == 0 && distToFloor > 0.1)
		{
			this.spawn_eggs = true;
		}
	
		/*************************/
		/*** Decide what to do ***/
		/*************************/
		
		/*** Fly died - fall to the ground ***/
		if (this.dead)
		{
			
			if (distToFloor < 0.05)
			{
				Vscripts_EntFire("fly_dead", "FireUser1", "", 0.00, -1);
				Vscripts_EntFire("fly", "Kill", "", 0.02, -1);
				return;
			}
			else
			{
				MoveDir(this, {0.0, 0.0, -1.0});
			}
		}
		
		/*** Move to the floor to spawn eggs ***/
		else if (!this.grabbed_player && this.spawn_eggs && this.eggs_currently_spawned < this.max_spawned_eggs && distToFloor < 1.0 && this.speed < this.max_speed / 2.0)
		{
			this.spawn_eggs = false;
			this.eggs_currently_spawned++;
			int eggSpawner = Vscripts_GetEntityIndexByHammerID(1248969, "env_entity_maker");
			TeleportEntity(eggSpawner, flyPos, { 0.0, 0.0, 0.0 }, NULL_VECTOR);
			AcceptEntityInput(eggSpawner, "ForceSpawn");
		}
		
		/*** Move towards a targetted player ***/
		else if(!this.return_to_toaster && IsValidPlayer(this.target))
		{
			ChasePlayer(this, flyPos);
		}
		
		/*** Take the grabbed player(s) above the toaster ***/
		else if(this.return_to_toaster)
		{
			// Fast approx of distance without needing sqrt
			this.current_distance_to_target = FloatAbs(TOASTER_POSITION[0] - flyPos[0]) + FloatAbs(TOASTER_POSITION[1] - flyPos[1]);
			
			float distToToaster[3]; 
			SubtractVectors(flyPos, TOASTER_POSITION, distToToaster);
			if (FloatAbs(distToToaster[0]) < 80.0 && FloatAbs(distToToaster[1]) < 80.0 && FloatAbs(distToToaster[2]) < 80.0)
			{
				this.grabbed_player = false;
				this.return_to_toaster = false;
				this.grabbed_players.Clear();
				this.previous_distance_to_target = -1.0;
			}
			else
			{
				this.MoveTowardsTarget(flyPos, TOASTER_POSITION);
				this.previous_distance_to_target = this.current_distance_to_target;
			}
		}
	
		/*** Search for a target player ***/
		else
		{
			this.target = GetNewTarget(this);
			if (!this.IsValidTarget())
				this.target = -1;
			flyPos_copy[2] += 140;
			if (this.target == -1 && Vscripts_TraceLine(flyPos, flyPos_copy, this.entity) >= 1.0)
				MoveDir(this, { 0.0, 0.0, 1.0 } );
		}
	}
	
	public void SetReturn(bool state)
	{
		this.return_to_toaster = state;
	}
	
	public void AddHealth(int hp)
	{
		this.health += hp;
		this.players_in_arena++;
	}
	
	public void Hit()
	{
		if (this.started && !this.dead)
		{
			this.health--;
			char param[256];
			Format(param, sizeof(param), "message Fly %i HP", this.health);
			Vscripts_EntFire("fly_text", "AddOutput", param, 0.0, -1);
			Vscripts_EntFire("fly_text", "Display", "", 0.01, -1);
		}
		
		if (this.health == 0 && !this.dead)
		{
			Vscripts_EntFire("fly_dead_relay", "Trigger", "", 0.0, -1);
			this.dead = true;
			float flyPos[3];
			Vscripts_GetOrigin(this.entity, flyPos);
			flyPos[2] += 160.0;
			this.TeleportGrabbedPlayers(flyPos);	
			this.grabbed_player = false;
			this.grabbed_players.Clear();
			this.speed = 0.0;	
		}
	}
	
	public void IncrementEggCount(int count)
	{
		this.eggs_currently_spawned += count;
	}
}

methodmap Fly_End < Fly_base 
{
	public Fly_End(int entity)
	{
		Fly_base myclass = new Fly_base(entity);
		myclass.blocker1 = 60.0;
		myclass.blocker2 = 110.0;
		myclass.rot_speed_max = 0.02;
		myclass.rot_speed_min = 0.005;
		myclass.speed_acceleration = 0.05;
		myclass.max_speed = 15.0;
		myclass.SetBool("bEnd", false);
		myclass.SetInt("iCurrent_node", 0);
		myclass.SetHandle("lGrabbed_players", new ArrayList());
		myclass.speed = 7.0;
		return view_as<Fly_End>(myclass);
	}
	property bool end 
	{
		public get()
		{
			return this.GetBool("bEnd");
		}
		public set(bool val)
		{
			this.SetBool("bEnd", val);
		}
	}
	property int current_node
	{
		public get()
		{
			return this.GetInt("iCurrent_node");
		}
		public set(int val)
		{
			this.SetInt("iCurrent_node", val);
		}
	}
	property ArrayList grabbed_players
	{
		public get()
		{
			return view_as<ArrayList>(this.GetHandle("lGrabbed_players"));
		}
		public set(ArrayList list)
		{
			this.SetHandle("lGrabbed_players", list);
		}
	}
	public void TeleportGrabbedPlayers(const float[3] position)
	{
		for (int i = 0; i < this.grabbed_players.Length; i++)
		{
			if (IsValidPlayer(this.grabbed_players.Get(i)))
			{
				if(GetClientTeam(this.grabbed_players.Get(i)) == 3)
					Vscripts_SetOrigin(this.grabbed_players.Get(i), position);
			}
		}	
	}
	
	public void KillFly()
	{
		delete this.grabbed_players;
		delete this;
	}
	
	public void Start()
	{
		Vscripts_EntFireByIndex(this.entity, "SetAnimation", "fly", 0.0, -1);
		this.started = true;
	}
	public void Tick()
	{
		float flyPos[3], flyPos_copy[3];
		Vscripts_GetOrigin(this.entity, flyPos);
		Vscripts_GetOrigin(this.entity, flyPos_copy);
		float targetNode[3];
		MakeVectorFromPoints( { 0.0, 0.0, 0.0 }, TARGET_NODES[this.current_node], targetNode);
		int playerNear;
		float point[3], fwd[3];
		Vscripts_GetForwardVector(this.entity, fwd);
		ScaleVector(fwd, 80.0);
		AddVectors(flyPos, fwd, point);
	
		/*** Kill nearby physboxes ***/
		int physbox;
		if ((physbox = Vscripts_FindEntityByClassnameWithin(physbox, "func_physbox", flyPos, 350.0)) != -1)
		{
			Vscripts_EntFireByIndex(physbox, "FireUser2", "", 0.0, -1);
		}
	
		/*** Grab a near player ***/
		if((playerNear = Vscripts_FindEntityByClassnameWithin(playerNear, "player", point, 100.0)) != -1)
		{	
			if(GetClientTeam(playerNear) == 3)
			{
				if(!IsElementInArray(playerNear, this.grabbed_players))
				{
					this.grabbed_players.Push(playerNear);
				}
			}
		}
	
		if((playerNear = Vscripts_FindEntityByClassnameWithin(playerNear, "player", point, 100.0)) != -1)
		{	
			if(GetClientTeam(playerNear) == 3)
			{
				if(!IsElementInArray(playerNear, this.grabbed_players))
				{
					this.grabbed_players.Push(playerNear);
				}
			}
		}
	
		/*** Teleport grabbed players ***/
		flyPos_copy[2] -= 40.0;
		this.TeleportGrabbedPlayers(flyPos_copy);

		/*** Move towards the next node ***/
		// Fast approx of distance without needing sqrt
		this.current_distance_to_target = FloatAbs(targetNode[0] - flyPos[0]) + FloatAbs(targetNode[1] - flyPos[1]);
	
		float distToTarget[3];
		SubtractVectors(flyPos, targetNode, distToTarget);
	
		if (FloatAbs(distToTarget[0]) < 80.0 && FloatAbs(distToTarget[1]) < 80.0 && FloatAbs(distToTarget[2]) < 80.0)
		{
			if (this.current_node < t_len - 1)
			{
					this.current_node++;
			}
			else
			{
				Vscripts_EntFireByIndex(this.entity, "SetAnimation", "idle", 0.0, -1);
				Vscripts_EntFireByIndex(this.entity, "SetAnimation", "attack", 2.0, -1);
				Vscripts_EntFire("boss_fly_sound_end", "StopSound", "", 0.0, -1);
				//Vscripts_EntFire("4_fly_hovno", "FireUser1", "", "0.50", -1);
				//Vscripts_EntFire("5_fly_hovno", "FireUser1", "", "0.50", -1);
				//Vscripts_EntFire("fly_hovno_sound4", "PlaySound", "", "0.50", -1);
				//Vscripts_EntFire("fly_hovno_sound5", "PlaySound", "", "0.50", -1);
				flyPos_copy[2] += 200;
				this.TeleportGrabbedPlayers(flyPos_copy);
				this.grabbed_players.Clear();
				float angles[3];
				Vscripts_GetForwardVector(this.entity, angles);
				angles[2] = 0.0;
				Vscripts_SetForwardVector(this.entity, angles);
				this.end = true;
				this.started = false;
				return;
			}
			
			this.previous_distance_to_target = -1.0;
		}
		else
		{
			this.MoveTowardsTarget(flyPos, targetNode);
			this.previous_distance_to_target = this.current_distance_to_target;
		}
	}
}

//methodmap Fly_End_Hovno < Fly_End
//{
	//public Fly_End_Hovno(int entity)
	//{
		//Fly_End myclass = new Fly_End(entity);
		//myclass.type = eFlyEndHovno;
		//myclass.rot_speed_acceleration = 0.16;
		//myclass.rot_speed_max = 0.24;
		//myclass.rot_speed_min = 0.16;
		//myclass.max_speed = 120.0;
		//myclass.speed_acceleration = 0.4;
		//myclass.SetBool("bStop_on_last_node", true);
		//myclass.SetInt("iPath", 0);
		//return view_as<Fly_End_Hovno>(myclass);
	//}
	//property bool stop_on_last_node
	//{
		//public get()
		//{
			//return this.GetBool("bStop_on_last_node");
		//}
		//public set(bool val)
		//{
			//this.SetBool("bStop_on_last_node", val);
		//}
	//}
	//property int path 
	//{
		//public get()
		//{
			//return this.GetInt("iPath");
		//}
		//public set(int val)
		//{
			//this.SetInt("iPath", val);
		//}
	//}
	//public void Start(int p, bool stop)
	//{
		//this.path = p;
		//this.stop_on_last_node = stop;
		//Vscripts_EntFireByIndex(this.entity, "SetAnimation", "fly", "0.00", -1);
		//CreateTimer(0.0, Tick_Cb, this, TIMER_FLAG_NO_MAPCHANGE);
	//}
	//
	//public void Tick()
	//{
		//float flyPos[3], flyPos_copy[3];
		//Vscripts_GetOrigin(this.entity, flyPos);
		//Vscripts_GetOrigin(this.entity, flyPos_copy);
		//float targetNode[3];
		//MakeVectorFromPoints( { 0.0, 0.0, 0.0 }, TARGET_NODES2[this.path][this.current_node], targetNode);
		//flyPos_copy[2] -= 40;
		
		
		/*** Teleport grabbed players ***/
		//this.TeleportGrabbedPlayers(flyPos_copy);
	
		/*** Move towards the next node ***/
		// Fast approx of distance without needing sqrt
		//this.current_distance_to_target = FloatAbs(targetNode[0] - flyPos[0]) + FloatAbs(targetNode[1] - flyPos[1]);
		
		//float distToTarget[3]; 
		//SubtractVectors(flyPos, targetNode, distToTarget);
		
		//if (FloatAbs(distToTarget[0]) < 80.0 && FloatAbs(distToTarget[1]) < 80.0 && FloatAbs(distToTarget[2]) < 80.0)
		//{
				//this.previous_distance_to_target = -1.0;
				//int len = (this.path == 0) ? t_len_0:t_len_1;
				//if (this.current_node < len - 1)
				//{
					//this.current_node++;
				//}
				//else if (this.stop_on_last_node)
				//{
					//Vscripts_EntFireByIndex(this.entity, "SetAnimation", "idle", "0.00", -1);
					//flyPos_copy[2] += 200.0;
					//this.TeleportGrabbedPlayers(flyPos_copy);
					//float angles[3]; 
					//Vscripts_GetForwardVector(this.entity, angles);
					//angles[2] = 0.0;
					//Vscripts_SetForwardVector(this.entity, angles);
					//this.end = true;
					//return;
				//}
		//}
		//if (!this.end)
		//{
			//this.MoveTowardsTarget(flyPos, targetNode);
			//this.previous_distance_to_target = this.current_distance_to_target;	
			//CreateTimer(0.0, Tick_Cb, this, TIMER_FLAG_NO_MAPCHANGE);
		//}
	//}
//}


public void ChasePlayer(Fly_Small fly, const float[3] flyPos)
{
	fly.retarget -= 0.01;
	float targetPos[3];
	Vscripts_GetOrigin(fly.target, targetPos);
	targetPos[2] += 48;

	// Fast approx of distance without needing sqrt
	fly.current_distance_to_target = FloatAbs(targetPos[0] - flyPos[0]) + FloatAbs(targetPos[1] - flyPos[1]);
	if(fly.retarget <= 0.0 || !IsTargetInSight(fly.entity, flyPos, targetPos))
	{
		fly.target = -1;
	}
	else
	{
		fly.MoveTowardsTarget(flyPos, targetPos);
		fly.previous_distance_to_target = fly.current_distance_to_target;
	}
}

public bool IsTargetInSight(int entity, const float[3] flyPos, const float[3] targetPos)
{
	return Vscripts_TraceLine(flyPos, targetPos, entity) >= 1.0;    
}


public int GetNewTarget(Fly_Small fly)
{
	if(!IsValidPlayer(fly.target))
	{
		fly.target = -1;
	}
	
	int player = -1;
	ArrayList playerArr = new ArrayList();
	
	while(-1 != (player = FindEntityByClassname(player, "player")))
	{
		if(GetClientTeam(player) == 3)
		{
			float playerPos[3], selfPos[3];
			Vscripts_GetOrigin(fly.entity, selfPos);
			Vscripts_GetOrigin(player, playerPos);
			playerPos[2] += 48;
			if(IsTargetInSight(fly.entity, selfPos, playerPos))
				playerArr.Push(player);
		}
	}
	if(playerArr.Length > 0)
	{
		fly.retarget = 8;
		int target = playerArr.Get(GetRandomInt(0, playerArr.Length - 1));
		delete playerArr;
		return target;
	}
	else
	{
		delete playerArr;
		return -1;
	}
}

public void MoveForward(Fly_base fly, float blocker1, float blocker2)
{
	float pos1[3], pos2[3], selfPos[3], selfFwd[3];
	Vscripts_GetOrigin(fly.entity, selfPos);
	Vscripts_GetForwardVector(fly.entity, selfFwd);
	ScaleVector(selfFwd, blocker1);
	AddVectors(selfPos, selfFwd, pos1);
	Vscripts_GetForwardVector(fly.entity, selfFwd);
	ScaleVector(selfFwd, blocker2);
	AddVectors(selfPos, selfFwd, pos2);
	
	if (Vscripts_TraceLine(pos1, pos2, fly.entity) < 1.0)
	{
		
		fly.speed = 0.0;
	}
	else
	{
		float newpos[3];
		Vscripts_GetForwardVector(fly.entity, selfFwd);
		ScaleVector(selfFwd, fly.speed);
		AddVectors(selfPos, selfFwd, newpos);
		Vscripts_SetOrigin(fly.entity, newpos);
	}
}

public void MoveDir(Fly_base fly, float[3] dir)
{
	if(fly.speed < fly.max_speed) fly.speed += fly.speed_acceleration;
	float currentAngle[3], origin[3], newpos[3], dir_copy[3];
	dir_copy[0] = dir[0];
	dir_copy[1] = dir[1];
	dir_copy[2] = dir[2];
	Vscripts_GetForwardVector(fly.entity, currentAngle);
	currentAngle[2] = 0.0;
	Vscripts_SetForwardVector(fly.entity, currentAngle);
	Vscripts_GetOrigin(fly.entity, origin);
	ScaleVector(dir_copy, fly.speed / 4.0);
	AddVectors(origin, dir_copy, newpos);
	Vscripts_SetOrigin(fly.entity, newpos);
}

public void GetNewDir(Fly_base fly, const float[3] targetDir, const float[3] currentDir, float[3] newDir)
{
	float rotDir = currentDir[0] * targetDir[1] - currentDir[1] * targetDir[0];
	if (FloatAbs(rotDir) > 0.3)
	{
		if (fly.speed > 0)
			fly.speed -= 0.6 * fly.speed_acceleration;
		if (fly.rotation_speed < fly.rot_speed_max)
			fly.rotation_speed += fly.rot_speed_acceleration;			
	}
	else
	{
		if (fly.speed < fly.max_speed)
			fly.speed += fly.speed_acceleration;
		if (fly.rotation_speed > fly.rot_speed_min)
			fly.rotation_speed -= fly.rot_speed_acceleration;
	}
	
	if (rotDir > fly.rot_err || rotDir >= 0 && fly.previous_distance_to_target < fly.current_distance_to_target)
	{
		Rotate2D(currentDir, fly.rotation_speed, newDir);
	}
	else if (rotDir < -fly.rot_err || rotDir < 0 && fly.previous_distance_to_target < fly.current_distance_to_target)
	{
		Rotate2D(currentDir, -fly.rotation_speed, newDir);
	}
	else
	{
		newDir[0] = currentDir[0];
		newDir[1] = currentDir[1];
		newDir[2] = currentDir[2];
	}
	
}

public void Rotate2D(const float[3] vector, const float angle, float[3] buffer)
{
	buffer[0] = vector[0] * Cosine(angle) - vector[1] * Sine(angle);
	buffer[1] = vector[0] * Sine(angle) + vector[1] * Cosine(angle);
}

public int GetRandomValue(const int max){
	return GetRandomInt(0, max + 1);
}

public bool IsElementInArray(const any element, const ArrayList arr)
{
	for (int i = 0; i < arr.Length; i++)
	{
		if (element == arr.Get(i))
			return true;
	}
	return false;
}

methodmap Microwave < Basic
{
	public Microwave(int entity)
	{
		Basic myclass = new Basic();
		myclass.SetInt("iEntity", entity);
		myclass.SetBool("bDead", false);
		myclass.SetBool("bStarted", false);
		myclass.SetInt("iHealth", 69);
		return view_as<Microwave>(myclass);
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

	property bool dead
	{
		public get()
		{
			return this.GetBool("bDead");
		}
		public set(bool val)
		{
			this.SetBool("bDead", val);
		}
	}
	
	property bool started
	{
		public get()
		{
			return this.GetBool("bStarted");
		}
		public set(bool val)
		{
			this.SetBool("bStarted", val);
		}
	}
	
	property int health
	{
		public get()
		{
			return this.GetInt("iHealth");
		}
		public set(int val)
		{
			this.SetInt("iHealth", val);
		}
	}
	
	public void Start()
	{
		this.started = true;
	}
	
	public void AddHealth(int hp)
	{
		this.health += hp;
	}
	
	public void Hit(int hp)
	{
		if (this.started && !this.dead)
		{
			this.health -= hp;
			if (this.health < 0)
			{
				this.health = 0;
			}
			char msg[250];
			Format(msg, sizeof(msg), "message Microwave %d HP", this.health);
			Vscripts_EntFire("fly_text", "AddOutput", msg, 0.0, -1);
			Vscripts_EntFire("fly_text", "Display", "", 0.01, -1);
		}
		
		if (this.health <= 0 && !this.dead)
		{
			Vscripts_EntFire("microwave_dead_relay", "Trigger", "", 0.0, -1);
			this.dead = true;
		}
	}
}