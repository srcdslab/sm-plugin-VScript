/*
**
*/
#if defined _AESTHETIC_included
 #endinput
#endif
#define _AESTHETIC_included
#include <basic>
#include <vscripts>
#define CELL_SIZE 128.0

methodmap Chess < Basic
{
	public Chess(int greenStart, int greenEnd, int purpleStart, int purpleEnd)
	{
		Basic myclass = new Basic();
		myclass.SetFloat("fGreenX", 2.0);
		myclass.SetFloat("fGreenY", 6.0);
		myclass.SetFloat("fPurpleX", 4.0);
		myclass.SetFloat("fPurpleY", 1.0);
		myclass.SetInt("iGreenStart", greenStart);
		myclass.SetInt("iGreenEnd", greenEnd);
		myclass.SetInt("iPurpleStart", purpleStart);
		myclass.SetInt("iPurpleEnd", purpleEnd);
		myclass.SetBool("bDead", false);
		return view_as<Chess>(myclass);
	}
	
	property bool bDead
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
	
	property float greenX
	{
		public get()
		{
			return this.GetFloat("fGreenX");
		}
		public set(float val)
		{
			this.SetFloat("fGreenX", val);
		}
	}
	
	property float greenY
	{
		public get()
		{
			return this.GetFloat("fGreenY");
		}
		public set(float val)
		{
			this.SetFloat("fGreenY", val);
		}
	}
	
	property float purpleX
	{
		public get()
		{
			return this.GetFloat("fPurpleX");
		}
		public set(float val)
		{
			this.SetFloat("fPurpleX", val);
		}
	}
	
	property float purpleY
	{
		public get()
		{
			return this.GetFloat("fPurpleY");
		}
		public set(float val)
		{
			this.SetFloat("fPurpleY", val);
		}
	}
	
	property int greenStart
	{
		public get()
		{
			return this.GetInt("iGreenStart");
		}
	}
	
	property int greenEnd
	{
		public get()
		{
			return this.GetInt("iGreenEnd");
		}
	}
	
	property int purpleStart
	{
		public get()
		{
			return this.GetInt("iPurpleStart");
		}
	}
	
	property int purpleEnd
	{
		public get()
		{
			return this.GetInt("iPurpleEnd");
		}
	}
	
	public bool Check(float x, float y) 
	{
		return (-1.0 < x && x < 8.0 && -1.0 < y && y < 8.0 && !((x == this.greenX && y == this.greenY) || (x == this.purpleX && y == this.purpleY)));
	}
	
	public void UpdateGreen(float dx, float dy) 
	{
		this.greenX += dx; 
		this.greenY += dy;
	}
	
	public void UpdateGreenEnd(float dx, float dy) 
	{
		float currentPos[3]; 
		Vscripts_GetOrigin(this.greenEnd, currentPos);
		float moveVector[3] =  { 0.0, ... };
		moveVector[0] = dx * CELL_SIZE;
		moveVector[1] = dy * CELL_SIZE;
		float tmp[3];
		AddVectors(currentPos, moveVector, tmp);
		Vscripts_SetOrigin(this.greenEnd, tmp);
	}
	
	public void MoveGreen(float dx, float dy) 
	{
		if (this.Check(this.greenX + dx, this.greenY + dy)) 
		{
			this.UpdateGreen(dx, dy); 
			this.UpdateGreenEnd(dx, dy);
		}
	}
	
	public void MoveStartToEnd(int start, int end) 
	{
		float e_Orig[3];
		Vscripts_GetOrigin(end, e_Orig);
		Vscripts_SetOrigin(start, e_Orig);
	}
	
	public void UpdatePurple(float dx, float dy) 
	{
		this.purpleX += dx;
		this.purpleY += dy;
	}
	
	public void UpdatePurpleEnd(float dx, float dy) 
	{
		float currentPos[3]; 
		Vscripts_GetOrigin(this.purpleEnd, currentPos);
		float moveVector[3] =  { 0.0, ... };
		moveVector[0] = dx * CELL_SIZE;
		moveVector[1] = dy * CELL_SIZE;
		float tmp[3];
		AddVectors(currentPos, moveVector, tmp);
		Vscripts_SetOrigin(this.purpleEnd, tmp);
	}
	
	public void MovePurple(float dx, float dy) 
	{
		if (this.Check(this.purpleX + dx, this.purpleY + dy)) 
		{
			this.UpdatePurple(dx, dy); 
			this.UpdatePurpleEnd(dx, dy);
		}
	}
	
	public void CheckInLove() 
	{
		if(this.bDead)
			return;
		float dx = this.greenX - this.purpleX; 
		float dy = this.greenY - this.purpleY; 
		if (dx < 0.0) dx *= -1.0; 
		if (dy < 0.0) dy *= -1.0; 
		if (dx + dy == 1.0)
		{
			this.bDead = true;
			Vscripts_EntFire("TotalTrigger", "FireUser2", "", 0.0, -1);
		}
	}
}

const int DEFAULT_RETARGET_TICKS = 200;
const float DEFAULT_MAX_VEL = 16.0;
const float DEFAULT_MAX_ACC = 0.1;
const float DEFAULT_RANGE = 100000.0;
const float DEFAULT_PER_FRAME_MAX_VEL_DELTA = 0.0003333; // 2.0 / (120.0 * 50.0);
const float DEFAULT_PER_FRAME_MAX_ACC_DELTA = 0.0000125; // 0.075 / (120.0 * 50.0);
#define PI 3.14159

public bool SphereCollideWithWorld(const float[3] orig, const float radius, float degree, float[3] buffer, int entity)
{
	float inc = PI / degree;
	float  phiStop = PI - inc;

	float minDist = 1.0;
	for (float phi = 0.0; phi < PI; phi += inc) 
	{
        // Avoid computing the full circle on the edge cases
        float theta = 0.0;
        if (phi == 0.0 || phi == phiStop) 
            theta = PI - inc;

        float cp = Cosine(phi);
        float sp = Sine(phi);
        for (; theta < 2 * PI; theta += inc) 
        {
            float ct = Cosine(theta);
            float st = Sine(theta);
            float vec[3];
            vec[0] = ct * sp; 
            vec[1] = st * sp; 
            vec[2] = cp;
            ScaleVector(vec, radius);

            float end[3];
            AddVectors(orig, vec, end);
            float dist = Vscripts_TraceLine(orig, end, entity);
            if ( 0 < dist && dist < minDist)
            {
            	ScaleVector(vec, dist);
            	AddVectors(orig, vec, buffer);
            	return true;
            }
        } 
    } 
    return false;
}

methodmap Eye < Basic
{
	public Eye(int entity)
	{
		Basic myclass = new Basic();
		myclass.SetInt("iEntity", entity);
		myclass.SetInt("iTarget", -1);
		myclass.SetFloat("fBossRadius", 72.0);
		myclass.SetInt("iBossTeamTarget", 3);
		myclass.SetInt("iRetargetTicks", 0);
		myclass.SetFloat("fMaxVel", DEFAULT_MAX_VEL);
		myclass.SetFloat("fMaxAcc", DEFAULT_MAX_ACC);
		myclass.SetVector("vVec", { 0.0, 0.0, 0.0 } );
		myclass.SetVector("vAcc", { 0.0, 0.0, 0.0 } );
		myclass.SetFloat("fSavedVel", 0.0);
		myclass.SetFloat("fSavedAcc", 0.0);
		return view_as<Eye>(myclass);
	}
	
	property int entity 
	{
		public get()
		{
			return this.GetInt("iEntity");
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
	
	property float bossRadius
	{
		public get()
		{
			return this.GetFloat("fBossRadius");
		}
		public set(float val)
		{
			this.SetFloat("fBossRadius", val);
		}
	}
	
	property int bossTeamTarget
	{
		public get()
		{
			return this.GetInt("iBossTeamTarget");
		}
		public set(int val)
		{
			this.SetInt("iBossTeamTarget", val);
		}
	}
	
	property int retargetTicks
	{
		public get()
		{
			return this.GetInt("iRetargetTicks");
		}
		public set(int val)
		{
			this.SetInt("iRetargetTicks", val);
		}
	}
	
	property float maxVel
	{
		public get()
		{
			return this.GetFloat("fMaxVel");
		}
		public set(float val)
		{
			this.SetFloat("fMaxVel", val);
		}
	}
	
	property float maxAcc
	{
		public get()
		{
			return this.GetFloat("fMaxAcc");
		}
		public set(float val)
		{
			this.SetFloat("fMaxAcc", val);
		}
	}
	
	property float savedVel
	{
		public get()
		{
			return this.GetFloat("fSavedVel");
		}
		public set(float val)
		{
			this.SetFloat("fSavedVel", val);
		}
	}
	
	property float savedAcc
	{
		public get()
		{
			return this.GetFloat("fSavedAcc");
		}
		public set(float val)
		{
			this.SetFloat("fSavedAcc", val);
		}
	}
	
	public void GetVel(float[3] vel)
	{
		this.GetVector("vVel", vel);
	}
	
	public void SetVel(const float[3] vel)
	{
		this.SetVector("vVel", vel);
	}
	
	public void GetAcc(float[3] acc)
	{
		this.GetVector("vAcc", acc);
	}
	
	public void SetAcc(const float[3] acc)
	{
		this.SetVector("vAcc", acc);
	}
	
	public void Retarget()
	{
    	// Start the gig
    	this.retargetTicks = 0;
    	int tempPlayer = -1;
    	int currentPlayer = -1;
    	int checkedPlayers = 0;
    	float distance = DEFAULT_RANGE;

    	// While there is no target or 
    	// players found as potential targets aren't humans
    	while (checkedPlayers < MaxClients) {
    	    currentPlayer = FindEntityByClassname(currentPlayer, "player");
	
    	    // Only humans are valid
    	    if(currentPlayer != -1 && 
    	       IsPlayerAlive(currentPlayer) && 
    	       GetClientTeam(currentPlayer) == this.bossTeamTarget)
    	    {
    	        float tmp[3], s_orig[3], p_orig[3];
    	        Vscripts_GetOrigin(this.entity, s_orig);
    	        Vscripts_GetOrigin(currentPlayer, p_orig);
    	        SubtractVectors(s_orig, p_orig, tmp);
    	        float length = GetVectorLength(tmp);

    	        // Make sure we're close enough
    	        if (length < distance) 
    	        {
    	            tempPlayer = currentPlayer;
    	            distance = length;
    	        }
    	    }
    	    // Make sure we don't stay here forever
    	    checkedPlayers++;
    	}

    	// Try the last bet if no player was found
    	this.target = tempPlayer;
    	
	}
	
	public void Stop()
	{
    	// Save the values
    	this.savedVel = this.maxVel;
    	this.savedAcc = this.maxAcc;

    	// Block movement (with a bit of velocity to avoid spinning)
    	this.maxVel = 0.01;
    	this.maxAcc = 0.0001;
	}
	
	public void AllowMovement()
	{
    	// Set either default or saved vals
    	if (this.savedVel == 0.0) this.maxVel = DEFAULT_MAX_VEL;
    	else this.maxVel = this.savedVel;
	
    	if (this.savedAcc == 0.0) this.maxAcc = DEFAULT_MAX_ACC;
    	else this.maxAcc = this.savedAcc;
	
    	// Reset saved values
    	this.savedVel = 0.0;
    	this.savedAcc = 0.0;
	}
	
	public void FakePhysicsMovement()
	{
		
    	// Get the center and end point
    	float cnt[3];
    	Vscripts_GetOrigin(this.entity, cnt);
	
    	// Make the boss face the target
    	float tmp[3];
    	this.GetVel(tmp);
    	Vscripts_SetForwardVector(this.entity, tmp);
	
    	// If there is an impact, bounce
    	float collPoint[3];
    	if (SphereCollideWithWorld(cnt, this.bossRadius, 5.0, collPoint, this.entity))
    	{
    		float vec[3];
    		SubtractVectors(cnt, collPoint, vec);
    		float norm[3], norm_copy[3];
    		NormalizeVector(vec, norm);
    		norm_copy[0] = norm[0];
    		norm_copy[1] = norm[1];
    		norm_copy[2] = norm[2];
    		// Reflect velocity in a hacky way (not plane-accurate)
    		float nn[3];
    		NormalizeVector(norm, nn);
    		this.GetVel(tmp);
    		float dot = GetVectorDotProduct(tmp, nn);
    		ScaleVector(norm_copy, 2 * dot);
    		SubtractVectors(tmp, norm_copy, tmp);
    		this.SetVel(tmp);
    		Vscripts_EntFireByIndex(this.entity, "FireUser1", "", 0.0, this.target);
    		// Move as far as we can
    		ScaleVector(norm, this.bossRadius);
    		AddVectors(cnt, norm, tmp);
    		Vscripts_SetOrigin(this.entity, tmp);
    	}
    	else
    	{	
    		this.GetVel(tmp);
    		AddVectors(cnt, tmp, tmp);
    		Vscripts_SetOrigin(this.entity, tmp);
    	}
    }
	
	public void Move()
	{
		
    	// If there is no target or the target is gone/dead, retarget
    	if (this.target == -1 || 
    	    !IsValidEntity(this.target) || 
    	    !IsPlayerAlive(this.target) || 
    	    this.retargetTicks++ >= DEFAULT_RETARGET_TICKS) this.Retarget();
	
    	// Perform the update with the data from the previous moment
    	float vel[3], tmp[3], acc[3];
    	this.GetVel(vel);
    	this.GetAcc(acc);
    	AddVectors(vel, acc, tmp);
    	this.SetVel(tmp);
    	float length = GetVectorLength(tmp);
    	if (length > this.maxVel)
    	{
    		ScaleVector(tmp, this.maxVel / length);
    		this.SetVel(tmp);
    	}
    	this.SetAcc( { 0.0, 0.0, 0.0 } );
	
    	// Apply the higher accuracy movement deltas
    	this.maxVel += DEFAULT_PER_FRAME_MAX_VEL_DELTA;
    	this.maxAcc += DEFAULT_PER_FRAME_MAX_ACC_DELTA;
	
    	// Try to move, finally
    	this.FakePhysicsMovement();
	
    	// Keep going if no target's there still
    	if (this.target == -1) return;
	
    	// Try moving towards our target if it exists, otherwise don't
    	
    	float tgtPos[3];
    	Vscripts_GetOrigin(this.target, tgtPos);
    	float bssPos[3];
    	Vscripts_GetOrigin(this.entity, bssPos);
    	float dir[3];
    	SubtractVectors(tgtPos, bssPos, dir);
	
    	// Compute the length for later and normalize it if needed
    	
    	length = GetVectorLength(dir);
    	if (length > this.maxVel)
    		ScaleVector(dir, this.maxVel / length);
	
    	// Compute the acceleration
    	
    	this.GetVel(vel);
    	SubtractVectors(dir, vel, acc);
    	length = GetVectorLength(acc);
    	if (length > this.maxAcc)
    	{	
    		ScaleVector(acc, this.maxAcc / length);
    		this.SetAcc(acc);
    	}
	}
}