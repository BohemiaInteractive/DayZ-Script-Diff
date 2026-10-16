//! We use this as an argument in the PlayParticle function, simply for readability's sake
class DustKickupParams
{
	Object object;
	string surfaceName;
	vector position;
	vector orientation;
	int playerStance;
	float hitFraction;
	string ammoType;
	float traceDepth;
}

//! The data that one particle holds
class DustKickupParticleData
{
	void DustKickupParticleData(string path, int id, string className)
	{
		m_Path = path;
		m_ID = id;
		m_ParticleClass = className;
	}
	
	string m_Path;
	string m_ParticleClass;
	int m_ID;
}

class DustKickupEffectDistanceData
{
	void DustKickupEffectDistanceData(float minDist, float maxDist, array<string> variants)
	{
		m_MinDistance = minDist;
		m_MaxDistance = maxDist;
		m_Variants 	  = variants;
	}
	
	float m_MinDistance;
	float m_MaxDistance;
	ref array<string> m_Variants;
};

class DustKickupEffects
{
	private string m_DustKickupSoundSetName = "KickupDustEffect_SoundSet";
	
	protected ref map<string, ref map<string, ref DustKickupParticleData>> m_DustEffPaths = new map<string, ref map<string, ref DustKickupParticleData>>();
	
	protected ref map<string, ref array<ref DustKickupEffectDistanceData>> m_EffectRanges = new map<string, ref array<ref DustKickupEffectDistanceData>>();

	private static ref DustKickupEffects m_DustKickupEffects = NULL;
	
	private const float MAX_DISTANCE_FOR_EFFECT = 2.0;
	
	//! vlaues for the cooldown are in seconds
	private const float EFFECT_COOLDOWN_MIN = 0.5;
	private const float EFFECT_COOLDOWN_MAX = 1.0;
	
	private float m_TimeUntilNextEffect = 0.0;
	
	void DustKickupEffects()
	{
		Init();
	}
	
	void Init()
	{
		LoadAmmoTypes();
		LoadDustEffectPaths();
	}
	
	static DustKickupEffects GetInstance()
	{
		if (!m_DustKickupEffects)
		{
			m_DustKickupEffects = new DustKickupEffects();
		}
		
		return m_DustKickupEffects;
	}
	
	static void CleanupInstance()
	{
		if (m_DustKickupEffects)
			delete m_DustKickupEffects;
		m_DustKickupEffects = null;
	}
	
	private void LoadAmmoTypes()
	{
		string configPath = "CfgMagazines ";
		string childClass = "";
		
		int count = g_Game.ConfigGetChildrenCount(configPath);
		
		//! Iterate over each ammo type in the config
		for (int i = 0; i < count; ++i)
		{
			g_Game.ConfigGetChildName(configPath, i, childClass);
			if (!childClass.Contains("Ammo")) 
				continue;

			string ammoType = "";
			
			int effCount = g_Game.ConfigGetChildrenCount(configPath + childClass + " Effects");
			array<ref DustKickupEffectDistanceData> dataArray = new array <ref DustKickupEffectDistanceData>();
			
			//!Iterate over each of the different stances to extract the distance values and the effects for each stance
			for (int j = 0; j < effCount; ++j)
			{
				string stance;
				g_Game.ConfigGetChildName(configPath + childClass + " Effects", j, stance);
				
				string effectPath = configPath + childClass + " Effects" + " " + stance;
				
				float minDist;
				float maxDist;
				array<string> effects = new array<string>();
				
				minDist = g_Game.ConfigGetFloat(effectPath + " minDist");
				maxDist = g_Game.ConfigGetFloat(effectPath + " maxDist");
				g_Game.ConfigGetTextArray(effectPath + " Effects", effects);
				
				dataArray.Insert(new DustKickupEffectDistanceData(minDist, maxDist, effects));
			}
			
			if (effCount > 0)
			{
				m_EffectRanges.Insert(childClass, dataArray);
			}
		}
	}
	
	//! Going over all surfaces, extracting all dust particles paths from each surface
	private void LoadDustEffectPaths()
	{
		string configPath = "CfgSurfaces ";
		string childClass = "";
		
		int count = g_Game.ConfigGetChildrenCount(configPath);
		
		//! Iterating over each surface that is in the config
		for (int i = 0; i < count; ++i)
		{
			g_Game.ConfigGetChildName(configPath, i, childClass);
			
			map<string, ref DustKickupParticleData> dustEffectParticles = new map <string, ref DustKickupParticleData>();
			
			int effectsCount = g_Game.ConfigGetChildrenCount(configPath + childClass + " DustEffects");
			
			//! Iterating over every effect that is inside the DustEffects class
			for (int j = 0; j < effectsCount; j++)
			{
				string dustPath 	= " ";
				string effectClass 	= " ";
				
				g_Game.ConfigGetChildName(configPath + childClass + " DustEffects", j, effectClass);
				
				//! Getting the path from a specific effect stored inside the DUstEffects class in the surface
				g_Game.ConfigGetText(configPath + childClass + " DustEffects" + " " + effectClass + " path", dustPath);
				
				if (dustPath.Length() > 1)
				{
					int particleID = ParticleList.RegisterParticleByFullPath(dustPath);
					dustEffectParticles.Insert(effectClass, new DustKickupParticleData(dustPath, particleID, effectClass));
				}
			}
			if (effectsCount > 0)
			{
				m_DustEffPaths.Insert(childClass, dustEffectParticles);
			}
		}	
	}
	
	//! Determines what particles should play based on the ammo type and distance of the weapon from the ground
	private array<string> DetermineParticles(string ammoType, float fraction, float depth)
	{
		//! calculating the actual distance from the hir fraction * the depth of the trace
		float distance = fraction * depth;
		array<ref DustKickupEffectDistanceData> distances = m_EffectRanges.Get(ammoType);
		if (!distances)
			return null;
		
		foreach (DustKickupEffectDistanceData data : distances)
		{
			if (distance >= data.m_MinDistance && distance < data.m_MaxDistance)
				return data.m_Variants;
		}
		
		return null;
	}
	
	void PlaySoundEffect(DustKickupParams params)
	{
		EffectSound dustKickupSound = SEffectManager.CreateSound(m_DustKickupSoundSetName, params.position);
		
		if (!dustKickupSound)
		{
			string errorMsgSound = "Soundset is invalid, please check the value of m_DustKickupSoundSetName, as well as the config to see if the soundset is correct";
			ErrorEx(string.Format("%1: SoundSetName: '%2' :: %3", this, m_DustKickupSoundSetName, errorMsgSound));
		}

		params.object.PlaySoundSet(dustKickupSound, m_DustKickupSoundSetName, 0.0, 0.0); //the two zeros are fade in/out values, will leave them as is and add constants in the future if needed
		dustKickupSound.SetPosition(params.position);
		dustKickupSound.SetAutodestroy(true);
	}
	
	void PlayParticle(DustKickupParams params)
	{
		float now = g_Game.GetTickTime();
		
		if (now < m_TimeUntilNextEffect)
			return;
		
		float distanceMeters = params.hitFraction * params.traceDepth;
		
		if (distanceMeters >= MAX_DISTANCE_FOR_EFFECT)
			return;
		
		array<string> particleNames = DetermineParticles(params.ammoType, params.hitFraction, params.traceDepth);
		if (!particleNames)
			return;
		
		map<string, ref DustKickupParticleData> surfaceVariants = m_DustEffPaths.Get(params.surfaceName);
		if (!surfaceVariants)
			return;
		
		bool effectPlayed = false;
		
		foreach (string name : particleNames)
		{
			DustKickupParticleData data = surfaceVariants.Get(name);
			
			if (!data)
			{
				string errorMsg = "No such particles exists. Check to see if the name of the particle inside the ammo config is correct";
				ErrorEx(string.Format("Particle: '%1'", name, errorMsg));
				continue;
			}
			
			ParticleSource particle = ParticleManager.GetInstance().CreateParticleEx(data.m_ID, params.position);
			particle.SetOrientation(params.orientation); 
			particle.SetParticleAutoDestroyFlags(ParticleAutoDestroyFlags.ON_END);
			particle.PlayParticle();
			
			effectPlayed = true;
		}
		
		if (effectPlayed)
		{
			m_TimeUntilNextEffect = now + Math.RandomFloat(EFFECT_COOLDOWN_MIN, EFFECT_COOLDOWN_MAX);
		}
		
		PlaySoundEffect(params);
	}
};