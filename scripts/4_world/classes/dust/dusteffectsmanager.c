class DustEffectManager
{
	
	protected static ref DustEffectManager m_Instance = NULL;
	
	static DustEffectManager GetDustEffectManager()
	{
		if(!m_Instance)
			m_Instance = new DustEffectManager();

		return m_Instance;
	}
	
	PlayerBase m_Player;
	
	protected bool m_EnableCloudEffect;
	protected int m_CloudEffectId; 
	protected float m_CloudEffectBaseDistance;
	protected float m_CloudEffectCooldown;
	protected float m_CloudEffectCooldownMin;
	protected float m_CloudEffectCooldownMax;
	protected float m_CloudEffectThreshold;
	protected float m_CloudEffectDurationMin;
	protected float m_CloudEffectDurationMax;
	protected float m_CloudEffectDuration;
	protected bool m_CloudEffectReadFromSurface;
	
	
	protected bool m_EnableGroundEffect;
	protected float m_GroundEffectRadius;
	protected float m_GroundEffectThreshold;
	protected float m_GroundEffectIntensity;
	protected float m_TimeFromLastGroundEffect;
	protected float m_GroundEffectWindWeight;
	protected bool m_GroundEffectReadFromSurface;
	protected bool m_GroundEffectTestEndPoint;
	protected float m_GroundEffectLifetimeMin;
	protected float m_GroundEffectLifetimeMax;
	protected float m_GroundEffectSizeMin;
	protected float m_GroundEffectSizeMax;
	
	protected bool m_EnableCeilingDustEffect = true; //'on' by default
	
	protected ref static map<int, string> m_ParticleIDEffectPairing; //<particleID, effect type>
	
	protected const float CLOUD_EFFECT_ORIGIN_HEIGHT = 1.5;
	protected const float CLOUD_EFFECT_SPEED_ADJUSTMENT = 3.5;
	protected const float GROUND_EFFECT_SPEED_COEFICIENT = 20;
	protected const float GROUND_EFFECT_SPEED_ADJUSTMENT = 3;
	protected const float GROUND_EFFECT_END_TEST_DISTANCE = 3;
	protected const float GROUND_EFFECT_WIND_BIAS_SPREAD_SCALE = 0.6;
	protected const float GROUND_EFFECT_WIND_BIAS_VARIATION_RANGE = 0.4;
	
	
	float m_Intensity;
	
	
	float m_TimeFromLastEffect;
	
	float m_ActualGroundEffectPeriod;
	
	const float BASE_FREQUENCY = 500;

	//Cloud
	
	const float CLOUD_BASE_FREQUENCY = 10;
	const float CLOUD_BASE_DURATION = 20;
	
	protected float m_TimeEffectCloud;
	protected float m_ActualDurationCloud;
	protected bool m_ActiveCloudEffect;

	void DustEffectManager()
	{
	}
	
	void Init(PlayerBase player)
	{
		m_Player = player;
		
		DustEffectsSettings dustEffectsSettings = GetGame().GetMission().GetWorldData().GetDustEffectsSettings();
				
		if (dustEffectsSettings.m_GroundEffectEnable)
		{
			
			InitGroundEffect(dustEffectsSettings);
			SetGroundEffectEnable(true);
		}
		else
		{
			SetGroundEffectEnable(false);
		}
		
		if (dustEffectsSettings.m_CloudEffectEnable)
		{
			InitCloudEffect(dustEffectsSettings);
			SetCloudEffectEnable(true);
		}
		else
		{
			SetCloudEffectEnable(false);
		}
		//InitGroundEffect(50, 5, 0, 0);
		//InitCloudEffect(20, 1, 1, 0);
		
		InitCeilingEffect();
	}
	
	void InitGroundEffect(DustEffectsSettings dustEffectsSettings)
	{
		m_EnableGroundEffect = true;

		SetGroundEffectThreshold(dustEffectsSettings.m_GroundEffectMinWindStrength);
		SetGroundEffectIntensity(dustEffectsSettings.m_GroundEffectFrequency);
		SetGroundEffectRadius(dustEffectsSettings.m_GroundEffectDistance);
		SetGroundEffectWindWeight(dustEffectsSettings.m_GroundEffectWindWeight);
		SetGroundEffectEndPointTest(dustEffectsSettings.m_GroundEffectEndPoindTest);
		m_GroundEffectLifetimeMin = dustEffectsSettings.m_GroundEffectLifetimeMin;
		m_GroundEffectLifetimeMax = dustEffectsSettings.m_GroundEffectLifetimeMax;
		m_GroundEffectSizeMin = dustEffectsSettings.m_GroundEffectSizeMin;
		m_GroundEffectSizeMax = dustEffectsSettings.m_GroundEffectSizeMax;

		m_GroundEffectReadFromSurface = true;
		m_TimeFromLastGroundEffect = 0.0;
	}
	
	void InitCloudEffect(DustEffectsSettings dustEffectsSettings)
	{
		m_EnableCloudEffect = true;
		
		SetCloudEffectDistance(dustEffectsSettings.m_CloudEffectDistance);
		SetCloudEffectCooldownMin(dustEffectsSettings.m_CloudEffectCooldownMin);
		SetCloudEffectCooldownMax(dustEffectsSettings.m_CloudEffectCooldownMax);
		SetCloudEffectCooldown(dustEffectsSettings.m_CloudEffectCooldownMin);
		SetCloudEffectDurationMin(dustEffectsSettings.m_CloudEffectDurationMin);
		SetCloudEffectDurationMax(dustEffectsSettings.m_CloudEffectDurationMax);
		SetCloudEffectDuration(dustEffectsSettings.m_CloudEffectDurationMin);
		SetCloudEffectThreshold(dustEffectsSettings.m_CloudEffectMinWindStrength);
		SetCloudEffectReadFromSurfaceEnable(dustEffectsSettings.m_CloudEffectFromSurface);
		
		m_ActiveCloudEffect = false;
		m_TimeEffectCloud = 0.0;
		
		m_CloudEffectReadFromSurface = true;
		m_CloudEffectId = -1;
	}
	
	protected void InitCeilingEffect()
	{
		m_ParticleIDEffectPairing = new map<int, string>();
		
		RegisterCeilingPair(ParticleList.CEILING_WOOD, "CeilingDustEffectWood");
	}
	
	protected void RegisterCeilingPair(int particleID, string effectType)
	{
		m_ParticleIDEffectPairing.Set(particleID, effectType);
	}
	
	void Update(float deltaT)
	{
		if (m_EnableGroundEffect || m_EnableCloudEffect)
		{
			EUndergroundPresence presence = m_Player.GetUndergroundPresence();
			bool temporaryDisable = false;
			
			if (presence != EUndergroundPresence.NONE)
			{
				StopCloudEffect();
			}
			else
			{
				vector wind = GetGame().GetWeather().GetWind();
				float windStr = wind.Normalize();
				
				if(m_EnableGroundEffect)
					UpdateGroundEffect(deltaT, windStr);
				if(m_EnableCloudEffect)
					UpdateCloudEffect(deltaT, windStr);
			}
		}
	}
	
	void UpdateGroundEffect(float deltaT, float windStrength)
	{
		if(windStrength > m_GroundEffectThreshold)
		{
			m_TimeFromLastGroundEffect += deltaT;
			float tresholdModificator = 1 + ((windStrength - 10) * m_GroundEffectWindWeight / 10);

			if((m_TimeFromLastGroundEffect * tresholdModificator) > m_ActualGroundEffectPeriod)
			{
				CreateGroundEffect();
				m_TimeFromLastGroundEffect = 0.0;
			}
		}
	}
	
	void UpdateCloudEffect(float deltaT, float windStrength)
	{
		m_TimeEffectCloud += deltaT;
		
		
		if( m_ActiveCloudEffect )
		{
			if( m_TimeEffectCloud > m_CloudEffectDuration )
			{
				StopCloudEffect();
				SetCloudEffectCooldown(ComputeCloudEffectCooldown());
				m_TimeEffectCloud = 0;
			}
			else
			{
				UpdateActiveCloudEffect();
			}
		}
		else
		{
			if( windStrength > m_CloudEffectThreshold )
			{
				if( m_TimeEffectCloud > m_CloudEffectCooldown )
				{
					SetCloudEffectDuration(ComputeCloudEffectDuration());
					CreateCloudEffect();
					m_TimeEffectCloud = 0;
				}
			}
		}
	}
	
	void UpdateActiveCloudEffect()
	{
		if (SEffectManager.IsEffectExist(m_CloudEffectId))
		{
			Effect effect = SEffectManager.GetEffectByID(m_CloudEffectId);
			vector dustPosition;
			vector dustRotation;
			
			CalculateCloudEffectPosRot(dustPosition, dustRotation);
			
			effect.SetLocalPosition(dustPosition);
			effect.SetAttachedLocalOri(dustRotation);
		}
	}
	
	protected void CreateGroundEffect()
	{
		bool canCreateEffect = true;
		int particleID = ParticleList.NONE;
		vector wind = g_Game.GetWeather().GetWind();
		float windStrength = wind.Normalize();
		vector spawnPoint = GetGroundEffectPotentialPosition();
		
		SurfaceInfo surfaceInfo;
		float spawnPointHeight;
		surfaceInfo = g_Game.GetSurfaceInfoOn(spawnPoint[0], spawnPoint[2], spawnPointHeight);
		spawnPoint[1] = spawnPointHeight;

		if (m_GroundEffectTestEndPoint)
		{
			vector endPoint;
			endPoint = spawnPoint + (wind * GROUND_EFFECT_END_TEST_DISTANCE);
			SurfaceInfo endPointSurfaceInfo;
			float endPointSurfaceHeight;
			endPointSurfaceInfo = g_Game.GetSurfaceInfoOn(endPoint[0], endPoint[2], endPointSurfaceHeight);
		
			if (surfaceInfo.GetSurfaceType() != endPointSurfaceInfo.GetSurfaceType())
			{
				canCreateEffect = false;
			}
		}

		if (canCreateEffect)
		{
			particleID = surfaceInfo.GetGroundEffectParticleId();
			if (particleID == ParticleList.NONE || particleID == ParticleList.INVALID)
			{
				canCreateEffect = false;
			}
		}

		if (canCreateEffect)
		{
			spawnPoint[1] = GetGame().SurfaceY(spawnPoint[0],spawnPoint[2]);
			
			ParticleSource source = ParticleManager.GetInstance().PlayInWorldEx(particleID, null, spawnPoint, "0 0 0", true);

			
			float output;
			
			GetParticleParm(source, -1, EmitorParam.LIFETIME,  output);
			GetParticleParm(source, -1, EmitorParam.SIZE,  output);
			
			
			if (m_GroundEffectLifetimeMin >= 0 && m_GroundEffectLifetimeMax >= 0)
				SetParticleParm(source, -1, EmitorParam.LIFETIME, ComputeWindBiasedValue(windStrength, m_GroundEffectLifetimeMin, m_GroundEffectLifetimeMax));
			if (m_GroundEffectSizeMin >= 0 && m_GroundEffectSizeMax >= 0)
				SetParticleParm(source, -1, EmitorParam.SIZE, ComputeWindBiasedValue(windStrength, m_GroundEffectSizeMin, m_GroundEffectSizeMax));

			vector dustOrientation = wind;
			
			dustOrientation[0] = wind[0];
			dustOrientation[1] = wind[2];
			
			float yaw = dustOrientation.VectorToYaw();
			
			vector surfaceNormal = GetGame().SurfaceGetNormal(spawnPoint[0],spawnPoint[2]);
			float dotWithForward = vector.Dot(surfaceNormal, wind);
			
			float pitch = Math.Atan2(-dotWithForward, surfaceNormal[1]) * Math.RAD2DEG;
			
			dustOrientation[0] = -yaw;
			dustOrientation[1] = 0;
			dustOrientation[2] = -pitch;
			
			source.SetOrientation(dustOrientation);
		}
	}
	
	protected vector GetGroundEffectPotentialPosition()
	{
		vector PlayerPos = m_Player.GetPosition();
		vector PlayerSpeed = GetPlayerSpeed();
		float PlayerSpeedStr = PlayerSpeed.Normalize();
		
		PlayerSpeed[1] = PlayerSpeed[2];
		float yaw = PlayerSpeed.VectorToYaw();
		
		float coef = 1 + (PlayerSpeedStr / GROUND_EFFECT_SPEED_COEFICIENT);

		float x = Math.RandomFloat(-coef * m_GroundEffectRadius, coef * m_GroundEffectRadius) + (PlayerSpeedStr * GROUND_EFFECT_SPEED_ADJUSTMENT);
		float z = Math.RandomFloat(-m_GroundEffectRadius / coef, m_GroundEffectRadius / coef);
		
		vector point = Vector(x, 0.0, z);
		
		point = vector.RotateAroundZeroDeg(point, vector.Up, -yaw);
		
		return PlayerPos + point;
	}
	
	protected vector GetPlayerSpeed()
	{
		vector playerSpeed;
		
		if (m_Player.IsInVehicle())
		{
			HumanCommandVehicle vehCommand = m_Player.GetCommand_Vehicle();
			if (vehCommand)
			{
				Transport trans = vehCommand.GetTransport();
				if (trans)
				{
					playerSpeed = trans.GetSpeed();
				}
			}
		}
		else
		{
			m_Player.PhysicsGetVelocity(playerSpeed);
		}
		
		return playerSpeed;
	}
	
	protected bool CreateCloudEffect()
	{
		if (!SEffectManager.IsEffectExist(m_CloudEffectId))
		{
			SurfaceInfo surfaceInfo = g_Game.GetSurfaceInfoUnderObject(m_Player);
			
			EffCloudDust eff = new EffCloudDust();
			if (m_CloudEffectReadFromSurface)
				eff.SelectFromSurface(surfaceInfo);
			else
				eff.SetDefault();
			
			vector dustPosition;
			vector dustRotation;
			
			CalculateCloudEffectPosRot(dustPosition, dustRotation);
			m_CloudEffectId = SEffectManager.PlayOnObject(eff, m_Player, dustPosition, dustRotation, true);
		}
		m_ActiveCloudEffect = true;
		return true;
	}
	
	void CalculateCloudEffectPosRot(out vector cloudPositon, out vector cloudRotation)
	{
		vector wind = GetGame().GetWeather().GetWind();
		float windStr = wind.Normalize();
		
		//Rotation calculation
		vector rotation = wind;	
		
		rotation[1] = rotation[2];	
		float yaw = rotation.VectorToYaw();
			
		rotation[0] = -yaw;
		rotation[1] = 0;
		rotation[2] = 0;
		//End Roration calculation
		
		//Position calculation
		vector playerSpeed = GetPlayerSpeed();

		float playerSpeedStr = playerSpeed.Normalize();
		float CloudEffectDistance = m_CloudEffectBaseDistance;
		
		vector pos = -(wind * m_CloudEffectBaseDistance);
		pos = pos + (playerSpeed * playerSpeedStr * CLOUD_EFFECT_SPEED_ADJUSTMENT);
		pos[1] + CLOUD_EFFECT_ORIGIN_HEIGHT;
		//End position calculation
		
		
		cloudPositon = pos;
		cloudRotation = rotation;
	}
	
	void StopCloudEffect()
	{
		if(SEffectManager.IsEffectExist( m_CloudEffectId ))
		{
			SEffectManager.Stop(m_CloudEffectId);	
		}
		m_CloudEffectId = -1;
		m_ActiveCloudEffect = false;
	}
	
	
	//#ifdef DIAG_DEVELOPER
	void SetCloudEffectEnable(bool enable)
	{
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		DiagMenu.SetValue(DiagMenuIDs.MISC_DUST_EFFECTS_CLOUD_ENABLE, enable);
		#endif
		#endif
		m_EnableCloudEffect = enable;
		StopCloudEffect();
	}
	
	bool IsCloudEffectEnable()
	{
		return m_EnableCloudEffect;
	}
	
	void SetGroundEffectEnable(bool enable)
	{
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		DiagMenu.SetValue(DiagMenuIDs.MISC_DUST_EFFECTS_GROUND_ENABLE, enable);
		#endif
		#endif
		m_EnableGroundEffect = enable;
	}
	
	bool IsGroundEffectEnable()
	{
		return m_EnableGroundEffect;
	}

	void SetCloudEffectThreshold(float threshold)
	{
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		DiagMenu.SetRangeValue(DiagMenuIDs.MISC_DUST_EFFECTS_CLOUD_THRESHOLD, threshold);
		#endif
		#endif
		m_CloudEffectThreshold = threshold;
	}
	
	float GetCloudEffectThreshold()
	{
		return m_CloudEffectThreshold;
	}
	
	void SetCloudEffectDistance(float distance)
	{
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		DiagMenu.SetRangeValue(DiagMenuIDs.MISC_DUST_EFFECTS_CLOUD_DISTANCE, distance);
		#endif
		#endif
		m_CloudEffectBaseDistance = distance;
	}
	
	float GetCloudEffectDistance()
	{
		return m_CloudEffectBaseDistance;
	}
	
	float GetCloudEffectCooldownMin()
	{
		return m_CloudEffectCooldownMin;
	}
	
	void SetCloudEffectCooldownMin(float cooldown)
	{
		m_CloudEffectCooldownMin = cooldown;
	}
	
	void SetCloudEffectCooldownMax(float cooldown)
	{
		m_CloudEffectCooldownMax = cooldown;
	}
	
	void SetCloudEffectCooldown(float cooldown)
	{
		m_CloudEffectCooldown = cooldown;
	}
	
	float GetCloudEffectCooldown()
	{
		return m_CloudEffectCooldown;
	}
	
	float ComputeCloudEffectCooldown()
	{
		float randMulti = Math.RandomFloat01();
		float cooldown = m_CloudEffectCooldownMin + (m_CloudEffectCooldownMax - m_CloudEffectCooldownMin) * randMulti;
		return cooldown;
	}
	
	float GetCloudEffectDurationMin()
	{
		return m_CloudEffectDurationMin;
	}
	
	void SetCloudEffectDurationMin(float duration)
	{
		m_CloudEffectDurationMin = duration;
	}
	
	void SetCloudEffectDurationMax(float duration)
	{
		m_CloudEffectDurationMax = duration;
	}
	
	void SetCloudEffectDuration(float duration)
	{
		m_CloudEffectDuration = duration;
	}
	
	float GetCloudEffectDuration()
	{
		return m_CloudEffectDuration;
	}
	
	float ComputeCloudEffectDuration()
	{
		float randMulti = Math.RandomFloat01();
		float duration = m_CloudEffectDurationMin + (m_CloudEffectDurationMax - m_CloudEffectDurationMin) * randMulti;
		return duration;
	}
	
	void SetCloudEffectReadFromSurfaceEnable(bool enable)
	{
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		DiagMenu.SetValue(DiagMenuIDs.MISC_DUST_EFFECTS_CLOUD_GETFROMSURFACE, enable);
		#endif
		#endif
		m_CloudEffectReadFromSurface = enable;
	}
	
	bool IsCloudEffectReadFromSurfaceEnable()
	{
		return m_CloudEffectReadFromSurface;
	}
	
	void SetGroundEffectThreshold(float threshold)
	{
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		DiagMenu.SetRangeValue(DiagMenuIDs.MISC_DUST_EFFECTS_GROUND_THRESHOLD, threshold);
		#endif
		#endif
		m_GroundEffectThreshold = threshold;
	}
	
	float GetGroundEffectThreshold()
	{
		return m_GroundEffectThreshold;
	}
	
	void SetGroundEffectIntensity(float intensity)
	{
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		DiagMenu.SetRangeValue(DiagMenuIDs.MISC_DUST_EFFECTS_GROUND_INTESITY, intensity);
		#endif
		#endif
		m_GroundEffectIntensity = intensity;
		
		int intensityLevel = m_GroundEffectIntensity / 10;
		int intensityMod = Math.ModFloat( m_GroundEffectIntensity, 10);
		
		float frequency = Math.Pow(10, - intensityLevel);
		
		m_ActualGroundEffectPeriod = frequency * (10 - intensityMod);
	}
	
	float GetGroundEffectIntensity()
	{
		return m_GroundEffectIntensity;
	}
	
	void SetGroundEffectRadius(float radius)
	{
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		DiagMenu.SetRangeValue(DiagMenuIDs.MISC_DUST_EFFECTS_GROUND_RADIUS, radius);
		#endif
		#endif
		m_GroundEffectRadius = radius;
	}
	
	float GetGroundEffectRadius()
	{
		return m_GroundEffectRadius;
	}
	
	void SetGroundEffectWindWeight(float windWeight)
	{
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		DiagMenu.SetRangeValue(DiagMenuIDs.MISC_DUST_EFFECTS_GROUND_WINDWEIGHT, windWeight);
		#endif
		#endif
		m_GroundEffectWindWeight = windWeight / 10;
	}
	
	void SetGroundEffectEndPointTest(bool endPointtest)
	{
		m_GroundEffectTestEndPoint = endPointtest;
	}
	
	float GetGroundEffectWindWeight()
	{
		return m_GroundEffectWindWeight * 10;
	}
	
	void SetGroundEffectReadFromSurfaceEnable(bool enable)
	{
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		DiagMenu.SetValue(DiagMenuIDs.MISC_DUST_EFFECTS_GROUND_GETFROMSURFACE, enable);
		#endif
		#endif
		m_GroundEffectReadFromSurface = enable;
	}
	
	bool IsGroundEffectReadFromSurfaceEnable()
	{
		return m_GroundEffectReadFromSurface;
	}
	
	// Returns a value in [minVal, maxVal] biased toward max by wind strength.
	// At low wind: result is closer to min with high spread.
	// At high wind: result is closer to max with lower spread.
	protected float ComputeWindBiasedValue(float windStrength, float minVal, float maxVal)
	{
		float windT = Math.Clamp(windStrength / GROUND_EFFECT_SPEED_COEFICIENT, 0, 1);
		float spreadScale = 1 - windT * GROUND_EFFECT_WIND_BIAS_SPREAD_SCALE;
		float variation = Math.RandomFloat(-GROUND_EFFECT_WIND_BIAS_VARIATION_RANGE, GROUND_EFFECT_WIND_BIAS_VARIATION_RANGE) * spreadScale;
		float t = Math.Clamp(windT + variation, 0, 1);
		return minVal + t * (maxVal - minVal);
	}

	void SetGroundEffectLifetimeRange(float minLifetime, float maxLifetime)
	{
		m_GroundEffectLifetimeMin = minLifetime;
		m_GroundEffectLifetimeMax = maxLifetime;
	}

	float GetGroundEffectLifetimeMin()
	{
		return m_GroundEffectLifetimeMin;
	}

	float GetGroundEffectLifetimeMax()
	{
		return m_GroundEffectLifetimeMax;
	}

	void SetGroundEffectSizeRange(float minSize, float maxSize)
	{
		m_GroundEffectSizeMin = minSize;
		m_GroundEffectSizeMax = maxSize;
	}

	float GetGroundEffectSizeMin()
	{
		return m_GroundEffectSizeMin;
	}

	float GetGroundEffectSizeMax()
	{
		return m_GroundEffectSizeMax;
	}

	//-----------------------------------------
	//ceiling dust part
	
	//! optional adjustment of frequency by particleID
	static void AdjustCeilingDustEffectFrequency(string surfaceType, int particleID, inout int min, inout int max)
	{
		switch (particleID)
		{
			case ParticleList.CEILING_WOOD:
			{
				min = 5;
				max = 10;
				return;
			}
		}
	}
	
	//! Returns effect type paired to the particleID
	static string GetCeilingEffectType(int particleID)
	{
		string ret = m_ParticleIDEffectPairing.Get(particleID);
		if (ret == string.Empty)
		{
			ErrorEx("No 'CeilingDustEffectBase'-type paired to particle ID " + particleID + ", using default value. Please add registration into the 'InitCeilingEffect' method.");
			ret = "CeilingDustEffecter";
		}
		return ret;
	}
	
	int IsCeilingDustEffectEnabled()
	{
		return m_EnableCeilingDustEffect;
	}
	
	void SetCeilingDustEffectEnable(bool enable)
	{
		m_EnableCeilingDustEffect = enable;
	}
	//-----------------------------------------
	
	//#endif
}
