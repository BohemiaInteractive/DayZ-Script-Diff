//TODO: move to entitytype part data later
class ConstructionPartEffectsData
{
	protected bool m_IsValid;
	
	protected ref array<string> m_SoundEffectMemoryPointNames = new array<string>();
	protected ref array<string> m_ParticleEffectMemoryPointNames = new array<string>();
	
	protected string m_PartBuildSoundset;
	protected string m_PartDismantleSoundset;
	protected string m_PartDestroySoundset;
	protected int m_PartDestroyParticleID;
	
	/////////
	//inits//
	/////////
	void Init(string partPath, ConstructionPartTypeData typeDataObject)
	{
		string effectPath = partPath + " EffectsData";
		if (!g_Game.ConfigIsExisting(effectPath)) //none of this is valid without an existing class
			return;
		
		int materialType = typeDataObject.GetMaterialType();
		
		//none of this is valid without SOME defined position points..
		if (g_Game.ConfigIsExisting(effectPath + " soundEffectPositions")) 
		{
			g_Game.ConfigGetTextArray(effectPath + " soundEffectPositions", m_SoundEffectMemoryPointNames);
			
			if (m_SoundEffectMemoryPointNames.Count() > 0)
			{
				m_IsValid = true;
				InitConstructionSoundsData(effectPath, materialType);
			}
		}
		
		if (g_Game.ConfigIsExisting(effectPath + " particleEffectPositions")) 
		{
			g_Game.ConfigGetTextArray(effectPath + " particleEffectPositions", m_ParticleEffectMemoryPointNames);
			
			if (m_ParticleEffectMemoryPointNames.Count() > 0)
			{
				m_IsValid = true;
				InitConstructionParticleData(effectPath, materialType);
			}
		}
	}
	
	protected void InitConstructionSoundsData(string effectPath, int materialType)
	{
		InitBuildSoundset(effectPath + " build_soundset_override", materialType);
		InitDismantleSoundset(effectPath + " dismantle_soundset_override", materialType);
		InitDestroySoundset(effectPath + " destroy_soundset_override", materialType);
	}
	
	protected void InitConstructionParticleData(string effectPath, int materialType)
	{
		InitDestroyParticle(effectPath + " destroy_particle_override", materialType);
	}
	
	protected void InitBuildSoundset(string path, int materialType)
	{
		if (g_Game.ConfigIsExisting(path))
		{
			m_PartBuildSoundset = g_Game.ConfigGetTextOut(path);
		}
		else
		{
			m_PartBuildSoundset = DefaultBuildSoundset(materialType);
		}
	}
	
	protected void InitDismantleSoundset(string path, int materialType)
	{
		if (g_Game.ConfigIsExisting(path))
		{
			m_PartDismantleSoundset = g_Game.ConfigGetTextOut(path);
		}
		else
		{
			m_PartDismantleSoundset = DefaultDismantleSoundset(materialType);
		}
	}
	
	protected void InitDestroySoundset(string path, int materialType)
	{
		if (g_Game.ConfigIsExisting(path))
		{
			m_PartDestroySoundset = g_Game.ConfigGetTextOut(path);
		}
		else
		{
			m_PartDestroySoundset = DefaultDestroySoundset(materialType);
		}
	}
	
	protected void InitDestroyParticle(string path, int materialType)
	{
		if (g_Game.ConfigIsExisting(path))
		{
			m_PartDestroyParticleID = ParticleList.RegisterParticleByFullPath(g_Game.ConfigGetTextOut(path)); //look for warnings in the log if your particle does not play
		}
		else
		{
			m_PartDestroyParticleID = DefaultDestroyParticle(materialType);
		}
	}
	
	////////////
	//defaults//
	////////////
	
	protected string DefaultBuildSoundset(int materialType)
	{
		string soundset = "";
		switch (materialType)
		{
			case ConstructionMaterialType.MATERIAL_LOG:
				soundset = BaseBuildingBase.SOUND_BUILD_WOOD_LOG;
				break;
			
			case ConstructionMaterialType.MATERIAL_WOOD:
				soundset = "BuildWood_SoundSet";
				break;
			
			case ConstructionMaterialType.MATERIAL_STAIRS:
				soundset = BaseBuildingBase.SOUND_BUILD_WOOD_STAIRS;
				break;
			
			case ConstructionMaterialType.MATERIAL_METAL:
				soundset = "BuildMetal_SoundSet";
				break;
			
			case ConstructionMaterialType.MATERIAL_WIRE:
				soundset = BaseBuildingBase.SOUND_BUILD_WIRE;
				break;
			
			case ConstructionMaterialType.MATERIAL_BRICK:
				soundset = "BuildBrick_SoundSet";
				break;
			
			case ConstructionMaterialType.MATERIAL_RUBBLE:
				soundset = "ClearRubble_SoundSet";
				break;
		}
		
		return soundset;
	}
	
	protected string DefaultDismantleSoundset(int materialType)
	{
		string soundset = "";
		switch (materialType)
		{
			case ConstructionMaterialType.MATERIAL_LOG:
				soundset = BaseBuildingBase.SOUND_DISMANTLE_WOOD_LOG;
				break;
			
			case ConstructionMaterialType.MATERIAL_WOOD:
				soundset = "DismantleWood_SoundSet";
				break;
			
			case ConstructionMaterialType.MATERIAL_STAIRS:
				soundset = BaseBuildingBase.SOUND_DISMANTLE_WOOD_STAIRS;
				break;
			
			case ConstructionMaterialType.MATERIAL_METAL:
				soundset = "DismantleMetal_SoundSet";
				break;
			
			case ConstructionMaterialType.MATERIAL_WIRE:
				soundset = BaseBuildingBase.SOUND_DISMANTLE_WIRE;
				break;
			
			case ConstructionMaterialType.MATERIAL_BRICK:
				soundset = "DismantleBrick_SoundSet";
				break;
		}
		
		return soundset;
	}
	
	protected string DefaultDestroySoundset(int materialType)
	{
		string soundset = "";
		switch (materialType)
		{
			case ConstructionMaterialType.MATERIAL_LOG:
				soundset = "DestroyWood_SoundSet";
				break;
			
			case ConstructionMaterialType.MATERIAL_WOOD:
				soundset = "DestroyWood_SoundSet";
				break;
			
			case ConstructionMaterialType.MATERIAL_STAIRS:
				soundset = "DestroyWood_SoundSet";
				break;
			
			case ConstructionMaterialType.MATERIAL_METAL:
				soundset = "DestroyMetal_SoundSet";
				break;
			
			case ConstructionMaterialType.MATERIAL_WIRE:
				soundset = "DestroyMetal_SoundSet";
				break;
			
			case ConstructionMaterialType.MATERIAL_BRICK:
				soundset = "DestroyBrick_SoundSet";
				break;
		}
		
		return soundset;
	}
	
	protected int DefaultDestroyParticle(int materialType)
	{
		switch (materialType)
		{
			case ConstructionMaterialType.MATERIAL_LOG:
			case ConstructionMaterialType.MATERIAL_WOOD:
			case ConstructionMaterialType.MATERIAL_STAIRS:
				return ParticleList.BUILDNG_DESTRUCT_WOOD;
			
			case ConstructionMaterialType.MATERIAL_METAL:
			case ConstructionMaterialType.MATERIAL_WIRE:
				return ParticleList.BUILDNG_DESTRUCT_METAL;
			
			case ConstructionMaterialType.MATERIAL_BRICK:
				return ParticleList.BUILDNG_DESTRUCT_BRICK;
		}
		
		//default
		return ParticleList.BUILDNG_DESTRUCT_WOOD;
	}
	
	///////////
	//getters//
	///////////
	bool IsValid()
	{
		return m_IsValid;
	}
	
	string GetSoundsetBuild()
	{
		return m_PartBuildSoundset;
	}
	
	string GetSoundsetDismantle()
	{
		return m_PartDismantleSoundset;
	}
	
	string GetSoundsetDestroy()
	{
		return m_PartDestroySoundset;
	}
	
	int GetParticleDestroyID()
	{
		return m_PartDestroyParticleID;
	}
	
	array<string> GetSoundEffectMemoryPointNames()
	{
		return m_SoundEffectMemoryPointNames;
	}
	
	array<string> GetParticleEffectMemoryPointNames()
	{
		return m_ParticleEffectMemoryPointNames;
	}
}
