class Bonfire extends BuildingSuper
{
	const float TIMER_HEATING_UPDATE_INTERVAL		= 2;
	const float PARAM_HEAT_RADIUS					= 15;
	const float PARAM_FULL_HEAT_RADIUS				= 5;
	const float	PARAM_MAX_TRANSFERED_TEMPERATURE	= 25;		//! maximum value for temperature that will be transfered to player (environment)
	const float CONST_FIRE_TEMP						= 1000;

	protected BonfireLight		m_Light;
	protected Particle 			m_ParticleFire;
	protected Particle 			m_ParticleSmoke;
	protected EffectSound 		m_SoundFireLoop;
	
	//Heating related variables
	protected ref Timer m_BonfireHeatingTimer;					//! DEPRECATED
	
	// UniversalTemperatureSource
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaConstant m_UTSLBonfire;
	
	override void EEInit()
	{
		super.EEInit();
		
		#ifndef SERVER
		// object creation - create effects (light, particles, sound)
		
		// light
		if (!m_Light)
		{
			m_Light = BonfireLight.Cast(ScriptedLightBase.CreateLight(BonfireLight, Vector(0,0,0), 1));
			m_Light.AttachOnMemoryPoint(this, m_Light.m_MemoryPoint);
			m_Light.FadeBrightnessTo(m_Light.m_FireBrightness, 0);
			m_Light.FadeRadiusTo(m_Light.m_FireRadius, 0);
		}
		
		// particles
		if (!m_ParticleSmoke)
		{
			m_ParticleSmoke = ParticleManager.GetInstance().PlayOnObject(ParticleList.BONFIRE_SMOKE, this, GetMemoryPointPos("bonfire_smoke"), Vector(0,0,0), true);
		}

		if (!m_ParticleFire)
		{
			m_ParticleFire = ParticleManager.GetInstance().PlayOnObject(ParticleList.BONFIRE_FIRE, this, GetMemoryPointPos("bonfire_fire"), Vector(0,0,0), true);
		}
		
		// sounds
		if (!m_SoundFireLoop)
		{
			PlaySoundSetLoop(m_SoundFireLoop, "HeavyFire_SoundSet", 1.0, 2.0);
		}
		#endif
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
 			m_UTSSettings 					= new UniversalTemperatureSourceSettings();
			m_UTSSettings.m_UpdateInterval	= TIMER_HEATING_UPDATE_INTERVAL;
			m_UTSSettings.m_Updateable		= true;
			m_UTSSettings.m_TemperatureMin	= 0;
			m_UTSSettings.m_TemperatureMax	= CONST_FIRE_TEMP;
			m_UTSSettings.m_TemperatureCap	= PARAM_MAX_TRANSFERED_TEMPERATURE;
			m_UTSSettings.m_RangeFull		= PARAM_FULL_HEAT_RADIUS;
			m_UTSSettings.m_RangeMax		= PARAM_HEAT_RADIUS;
			
			m_UTSLBonfire 					= new UniversalTemperatureSourceLambdaConstant();
			m_UTSource 						= new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLBonfire);
			m_UTSource.SetActive(true);
		}	
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		#ifndef SERVER
		// outside of bubble - do effects cleanup (light, particles, sound)
		
		// light
		if (m_Light)
		{
			m_Light.FadeOut();
		}
		
		// particles 
		if (m_ParticleSmoke)
		{
			m_ParticleSmoke.Stop();
		}

		if (m_ParticleFire)
		{
			m_ParticleFire.Stop();
		}
			
		// sounds 
		if (m_SoundFireLoop)
		{
			m_SoundFireLoop.SoundStop();
		}
		#endif
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.SetActive(false);
		}
	}	
	
	//! DEPRECATED
	protected void BonfireHeating() {};
}