enum EBoatSpeed
{
	UNSET,
	SLOWEST,
	SLOW,
	MEDIUM,
	FAST
}

class EffectBoatWaterFront : EffectBoatWaterBase
{
	protected const int EMITORS_FAST = 9; 	// emitors below this ID only play when fast enough
	
	void EffectBoatWaterFront()
	{
		m_EmitorCount = 13;
		m_SpeedSlow = 5;
		m_SpeedMedium = 20;
		m_SpeedFast = 40;
		
		SetParticleState( ParticleList.BOAT_WATER_FRONT );
	}
	
	override protected void UpdateSpeedState(Particle ptc, float speed)
	{
		EBoatSpeed speedStatePast = m_SpeedState;
		
		if (speed > m_SpeedFast)
			m_SpeedState = EBoatSpeed.FAST;
		else if (speed > m_SpeedMedium)
			m_SpeedState = EBoatSpeed.MEDIUM;
		else if (speed > m_SpeedSlow)
			m_SpeedState = EBoatSpeed.SLOW;
		else 
			m_SpeedState = EBoatSpeed.SLOWEST;
		
		if (m_SpeedState == speedStatePast)
			return;
		
		for (int i = 0; i < m_EmitorCount; i++)
		{
			if (i == 5 || i == 6)
			{
				if (m_SpeedState <= EBoatSpeed.MEDIUM) // fast drops
					EnableEmitor(ptc, i, false);
				else 
					EnableEmitor(ptc, i, true);
			}
			else if (i == 9 || i == 10)	// slow drops
			{
				if (m_SpeedState != EBoatSpeed.MEDIUM)
					EnableEmitor(ptc, i, false);
				else 
					EnableEmitor(ptc, i, true);
			}
			else if (i < 5 || i == 7 || i == 8)	// rest of front ptc
			{
				if (m_SpeedState <= EBoatSpeed.SLOW)
					EnableEmitor(ptc, i, false);
				else 
					EnableEmitor(ptc, i, true);
			}
		}
	}
		
	override void Update(float timeSlice = 0)
	{
		if (m_Boat.GetCurrentGear() == 0)
		{
			if (IsPlaying())
			{
				GetParticle().SetParticleParam(EmitorParam.BIRTH_RATE, 0);
				GetParticle().SetParticleParam(EmitorParam.BIRTH_RATE_RND, 0);
				Stop();
			}
						
			return;
		}
		
		super.Update(timeSlice);
				
		Particle ptc = GetParticle();
		float lerp1, lerp2;
		
		vector velocity = dBodyGetVelocityAt(m_Boat, m_Boat.CoordToParent(GetLocalPosition()));
		float speed = velocity.Normalize() * 3.6; // to km/h
		UpdateSpeedState(ptc, speed);
		
		UpdatePosToSeaLevel(timeSlice, -0.2);
		
		lerp1 = Math.InverseLerp(0, m_SpeedFast, speed);
		lerp2 = Math.InverseLerp(m_SpeedMedium - 5, m_SpeedFast + 10, speed);
		
		for (int i = 0; i < m_EmitorCount; i++)
		{
			if (i == 11 || i == 12)	// waves
			{
				ptc.SetParameter(i, EmitorParam.VELOCITY, ptc.GetParameterOriginal(i, EmitorParam.VELOCITY) * lerp1);
			}	
			else if (i < 5 || i == 7 || i == 8)	// splashes
			{
				if (m_SpeedState > EBoatSpeed.SLOW)
				{				
					if (g_Game.GetWaterDepth(m_Boat.CoordToParent(m_MemPointPos)) < -0.1)	// when the front of the boat is above water, disable the emitor
						EnableEmitor(ptc, i, false);
					else 
						EnableEmitor(ptc, i, true);
					
					ptc.SetParameter(i, EmitorParam.SIZE, ptc.GetParameterOriginal(i, EmitorParam.SIZE) * lerp2);
				}
			}
		}
	}
}

class EffectBoatWaterBack : EffectBoatWaterBase
{
	protected const int EMITORS_FAST = 5; 		// emitors below this ID only play when fast enough
		
	void EffectBoatWaterBack()
	{
		m_EmitorCount = 10;
		m_SpeedSlow = 25;
		m_SpeedMedium = 50;
		m_SpeedFast = 150;
		
		SetParticleState( ParticleList.BOAT_WATER_BACK );
	}
	
	override protected void UpdateSpeedState(Particle ptc, float speed)
	{
		EBoatSpeed speedStatePast = m_SpeedState;
		
		if (speed > m_SpeedFast)
			m_SpeedState = EBoatSpeed.FAST;
		else if (speed > m_SpeedMedium)
			m_SpeedState = EBoatSpeed.MEDIUM;
		else if (speed > m_SpeedSlow)
			m_SpeedState = EBoatSpeed.SLOW;
		else 
			m_SpeedState = EBoatSpeed.SLOWEST;
		
		if (m_SpeedState == speedStatePast)
			return;
		
		for (int i = 0; i < m_EmitorCount; i++)
		{
			if (i < EMITORS_FAST)	// big splash
			{
				if (m_SpeedState <= EBoatSpeed.SLOW)
					EnableEmitor(ptc, i, false);
				else 
					EnableEmitor(ptc, i, true);
			}
			
			if (i >= EMITORS_FAST && i != 9)
			{
				if (m_SpeedState == EBoatSpeed.SLOWEST)
					EnableEmitor(ptc, i, false);
				else 
					EnableEmitor(ptc, i, true);
			}
		}
	}
	
	override void Update(float timeSlice = 0)
	{
		if (m_Boat.GetCurrentGear() == 0)
		{
			if (IsPlaying())
			{
				GetParticle().SetParticleParam(EmitorParam.BIRTH_RATE, 0);
				GetParticle().SetParticleParam(EmitorParam.BIRTH_RATE_RND, 0);
				Stop();
			};
			
			return;
		}
		
		super.Update(timeSlice);
		Particle ptc = GetParticle();
		
		float speed = m_Boat.PropellerGetAngularVelocity();
		UpdateSpeedState(ptc, speed);
		
		float lerp = Math.InverseLerp(0, m_SpeedFast, speed);
		
		for (int i = 0; i < m_EmitorCount; i++)
		{
			if (i < EMITORS_FAST && m_SpeedState > EBoatSpeed.SLOW)
			{
				ptc.SetParameter(i, EmitorParam.SIZE, ptc.GetParameterOriginal(i, EmitorParam.SIZE) * lerp);
				ptc.SetParameter(i, EmitorParam.BIRTH_RATE, ptc.GetParameterOriginal(i, EmitorParam.BIRTH_RATE) * lerp);
			}
		}
	}
}

class EffectBoatWaterSide : EffectBoatWaterBase
{
	protected const int EMITORS_FAST = 3;	 // emitors below this ID only play when fast enough
	
	void EffectBoatWaterSide()
	{
		m_EmitorCount = 5;
		m_SpeedSlow = 5;
		m_SpeedMedium = 20;
		m_SpeedFast = 40;
		
		SetParticleState( ParticleList.BOAT_WATER_SIDE );
	}
		
	override protected void UpdateSpeedState(Particle ptc, float speed)
	{
		EBoatSpeed speedStatePast = m_SpeedState;
		
		if (speed > m_SpeedFast)
			m_SpeedState = EBoatSpeed.FAST;
		else if (speed > m_SpeedMedium)
			m_SpeedState = EBoatSpeed.MEDIUM;
		else if (speed > m_SpeedSlow)
			m_SpeedState = EBoatSpeed.SLOW;
		else 
			m_SpeedState = EBoatSpeed.SLOWEST;
		
		if (m_SpeedState == speedStatePast)
			return;
		
		for (int i = 0; i < m_EmitorCount; i++)
		{
			if (i < EMITORS_FAST)	// splashes
			{
				if (m_SpeedState <= EBoatSpeed.SLOW)
					EnableEmitor(ptc, i, false);
				else 
					EnableEmitor(ptc, i, true);
			}
		}
	}
	
	override void Update(float timeSlice = 0)
	{
		super.Update(timeSlice);
		
		Particle ptc = GetParticle();
		
		vector velocity = dBodyGetVelocityAt(m_Boat, m_Boat.CoordToParent(GetLocalPosition()));
		float speed = velocity.Normalize() * 3.6; // to km/h
		UpdateSpeedState(ptc, speed);
		
		UpdatePosToSeaLevel(timeSlice, -0.2);
		
		float lerp = Math.InverseLerp(0, m_SpeedFast, speed);
		
		for (int i = 0; i < m_EmitorCount; i++)
		{
			if (i < EMITORS_FAST)	// splashes
			{
				if (m_SpeedState > EBoatSpeed.SLOW)
					ptc.SetParameter(i, EmitorParam.SIZE, ptc.GetParameterOriginal(i, EmitorParam.SIZE) * lerp);	
			}
			else 	// waves
			{
				ptc.SetParameter(i, EmitorParam.SIZE, ptc.GetParameterOriginal(i, EmitorParam.SIZE) * lerp);
				ptc.SetParameter(i, EmitorParam.VELOCITY, ptc.GetParameterOriginal(i, EmitorParam.VELOCITY) * lerp);
			}	
		}
	}
}

class EffectBoatWaterBase : EffectParticle
{	
	const float POS_UPDATE_THROTTLE = 0.2; // seconds, controls how often can boat update somewhat expensive particle local reposition
	
	protected int m_EmitorCount;
	protected int m_SpeedSlow;
	protected int m_SpeedMedium;
	protected int m_SpeedFast;
	protected float m_PosUpdateTimer; 
	
	protected EBoatSpeed m_SpeedState;
	protected BoatScript m_Boat;
	protected vector m_MemPointPos;
	
	override void AttachTo(Object obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0", bool force_rotation_to_world = false)
	{
		m_Boat  = BoatScript.Cast(obj);
		m_MemPointPos = local_pos;
		super.AttachTo(obj, local_pos, local_ori, force_rotation_to_world);
	}
	
	void SetParticleState(int state)
	{
		bool was_playing = IsPlaying();
	
		Stop();
		
		SetParticleID(state);
		
		if (was_playing)
		{
			Start(); // resume effect
		}
	}
		
	protected void EnableEmitor(Particle ptc, int id, bool enable)
	{
		if (enable)
		{
			ptc.SetParameter(id, EmitorParam.BIRTH_RATE, ptc.GetParameterOriginal(id, EmitorParam.BIRTH_RATE));
			ptc.SetParameter(id, EmitorParam.BIRTH_RATE_RND, ptc.GetParameterOriginal(id, EmitorParam.BIRTH_RATE_RND));
		}
		else
		{
			ptc.SetParameter(id, EmitorParam.BIRTH_RATE, 0);
			ptc.SetParameter(id, EmitorParam.BIRTH_RATE_RND, 0);
		}	
	}
				
	// Update on state change
	protected void UpdateSpeedState(Particle ptc, float speed)
	{}
	
	// Update runs every frame
	void Update(float timeSlice = 0)
	{
		if (!IsPlaying())
		{
			m_SpeedState = EBoatSpeed.UNSET; // reinit
			Start();
		}
	};
		
	// Adjust position to sea level
	protected void UpdatePosToSeaLevel(float timeSlice = 0, float surfaceOffset = 0)
	{
		m_PosUpdateTimer += timeSlice;
		if (m_PosUpdateTimer > POS_UPDATE_THROTTLE)
		{
			m_PosUpdateTimer = 0;
			
			vector posAdjusted = m_Boat.CoordToParent(m_MemPointPos);
			posAdjusted[1] = g_Game.SurfaceGetSeaLevel() + surfaceOffset;
			posAdjusted = m_Boat.CoordToLocal(posAdjusted);
			SetCurrentLocalPosition(posAdjusted);
		}
	}
}