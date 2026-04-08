/*enum AnimSurfaces
{
	cp_concrete1,
	cp_concrete2,

	cp_dirt,

	cp_broadleaf_dense1,
	cp_broadleaf_dense2,
	cp_broadleaf_sparse1,
	cp_broadleaf_sparse2,

	cp_conifer_common1,
	cp_conifer_common2,
	cp_conifer_moss1,
	cp_conifer_moss2,

	cp_grass,
	cp_grass_tall,

	cp_gravel,
	cp_rock,

	asphalt_ext,
	asphalt_int,

	asphalt_destroyed_ext,
	asphalt_destroyed_int,

	concrete_ext,
	concrete_int,

	stone_ext,
	stone_int,

	gravel_large_ext,
	gravel_large_int,

	gravel_small_ext,
	gravel_small_int,

	sand_ext,
	sand_int,

	dirt_ext,
	dirt_int,

	rubble_large_ext,
	rubble_large_int,

	rubble_small_ext,
	rubble_small_int,

	trash_ext,
	trash_int,

	grass_dry_ext,
	grass_dry_int,

	metal_thick_ext,
	metal_thick_int,

	metal_thin_ext,
	metal_thin_int,

	metal_thin_mesh_ext,
	metal_thin_mesh_int,

	asphalt_felt_ext,
	asphalt_felt_int,

	ceramic_tiles_ext,
	ceramic_tiles_int,

	lino_ext,
	lino_int,

	textile_carpet_ext,
	textile_carpet_int,

	wood_parquet_ext,
	wood_parquet_int,

	wood_planks_ext,
	wood_planks_int,

	concrete_stairs_ext,
	concrete_stairs_int,

	metal_stairs_ext,
	metal_stairs_int,

	wood_planks_stairs_ext,
	wood_planks_stairs_int
}*/

enum AnimBootsType
{
	None,
	Sneakers,
	Boots
}

enum AnimUpperBodyType
{
	None 				= 4107064,//string hash, because we can't make compile time constant
	NylonJacket			= 1228024514,
	TShirt				= 1690896446,
	WoolShirt			= 1060939383,
	HeavyJacket			= 1544363355,
	LeatherJacket		= 2008520095,
	Coat				= 3549415,
	ChemlonDress		= -1491825621,
	Ghillie				= 602194810,
	Chainmail			= -563873622,
}

enum AnimBackType
{
	None 				= -1,
	Small 				= 161213437,
	Military 			= 1935514591,
	Outdoor				= 574855932,
	Ghillie				= 602194810
}

enum AnimRangedWeaponType
{
	None 		= 5727960,
	Rifle 		= 219116654,
	Shotgun 	= 1836650908
}

class AnimSoundEvent
{
	int m_iID;
	ref SoundObjectBuilder 			m_SoundObjectBuilder;
	ref SoundParams 				m_SoundParams;
	autoptr NoiseParams 			m_NoiseParams;
	bool 							m_IsValid = false;
	SoundLookupTable 				m_Table;
	
	
	void AnimSoundEvent(string soundPath)
	{
		m_iID = g_Game.ConfigGetInt(soundPath + "id");

		#ifndef SERVER

		string soundSetName;
		if (g_Game.ConfigGetText(soundPath + "soundSet", soundSetName))
		{
			m_SoundParams = new SoundParams(soundSetName);
			if (m_SoundParams.IsValid())
			{
				m_SoundObjectBuilder = new SoundObjectBuilder(m_SoundParams);
				m_IsValid = true;
			}
		}
		
		string tableName;
		if (g_Game.ConfigGetText(soundPath + "soundLookupTable", tableName))
		{
			m_Table = AnimSoundLookupTableBank.GetInstance().GetActionTable(tableName);
			if (m_Table)
			{
				m_IsValid = true;
				//Print("Found lookup table '"+tableName +"' for anim event-----------------------------------------> " + m_iID);
			}
		}
		#endif

		
		
		
		if ( g_Game.IsServer() )
		{
			string noiseName;
			if (g_Game.ConfigGetText(soundPath + "noise", noiseName))
			{
				m_NoiseParams = new NoiseParams();
				m_NoiseParams.Load(noiseName);
				m_IsValid = true;
			}
			else
			{
				//Print("AnimSoundEvent: \"" + soundPath + "\" doesn't have defined \"noise\"");
			}
		}
	}
	
	bool IsValid()
	{
		return m_IsValid;
	}
	
	SoundObjectBuilder GetSoundBuilder()
	{
		return m_SoundObjectBuilder;
	}
	
	SoundObjectBuilder GetSoundBuilderEx(int paramHash = 0)
	{
		if (m_Table && paramHash)
		{
			return m_Table.GetSoundBuilder(paramHash);
		}
		return m_SoundObjectBuilder;
	}

	SoundObject GetSoundObject(vector position)
	{
		GetSoundBuilderEx().AddEnvSoundVariables(position);
		return GetSoundBuilderEx().BuildSoundObject();
	}
}

class AnimSoundVoiceEvent
{
	int m_iID;
	ref SoundObjectBuilder m_SoundObjectBuilder;
	ref SoundParams m_SoundParams;
	autoptr NoiseParams m_NoiseParams;
	bool m_IsValid = false;

	void AnimSoundVoiceEvent(string soundPath)
	{
		m_iID = g_Game.ConfigGetInt(soundPath + "id");

		if ( !g_Game.IsDedicatedServer() )
		{
			string soundSetName;
			g_Game.ConfigGetText(soundPath + "soundSet", soundSetName);
			m_SoundParams = new SoundParams(soundSetName);
			if ( m_SoundParams.IsValid() )
			{
				m_SoundObjectBuilder = new SoundObjectBuilder(m_SoundParams);
				m_IsValid = true;
			}
		}
		
		if ( g_Game.IsServer() )
		{
			string noiseName;
			if (g_Game.ConfigGetText(soundPath + "noise", noiseName))
			{
				m_NoiseParams = new NoiseParams();
				m_NoiseParams.Load(noiseName);
				m_IsValid = true;
			}
			else
			{
				//Print("AnimSoundVoiceEvent: \"" + soundPath + "\" doesn't have defined \"noise\"");
			}
		}
	}
	
	bool IsValid()
	{
		return m_IsValid;
	}
	
	SoundObjectBuilder GetSoundBuilder()
	{
		return m_SoundObjectBuilder;
	}

	SoundObject GetSoundObject(vector position)
	{
		m_SoundObjectBuilder.AddEnvSoundVariables(position);
		return m_SoundObjectBuilder.BuildSoundObject();
	}
}

class AnimStepEvent
{
	int m_iID;
	string m_sSoundLookupTableName;
	StepSoundLookupTable m_soundLookupTable;
	autoptr NoiseParams m_NoiseParams;

	void AnimStepEvent(string stepPath)
	{
		m_iID = g_Game.ConfigGetInt(stepPath + "id");

		if ( !g_Game.IsDedicatedServer() )
		{
			g_Game.ConfigGetText(stepPath + "soundLookupTable", m_sSoundLookupTableName);
			m_soundLookupTable = StepSoundLookupTable.Cast( AnimSoundLookupTableBank.GetInstance().GetStepTable(m_sSoundLookupTableName) );
		}
		
		if ( g_Game.IsServer() )
		{
			string noiseName;
			if (g_Game.ConfigGetText(stepPath + "noise",noiseName))
			{
				m_NoiseParams = new NoiseParams();
				m_NoiseParams.Load(noiseName);
			}
		}
	}
	
	SoundObjectBuilder GetSoundBuilder(int surfaceHash)
	{
		return m_soundLookupTable.GetSoundBuilder(surfaceHash);
	}
}

class AnimDamageEvent
{
	int m_iID;
	autoptr AnimDamageParams m_DamageParams;

	void AnimDamageEvent(string damagePath)
	{
		m_iID = g_Game.ConfigGetInt(damagePath + "id");
		
		string damageName;
		g_Game.ConfigGetText(damagePath + "damage", damageName);
		m_DamageParams = new AnimDamageParams(damageName);
	}
}

class AnimDamageParams
{
	string m_sName;
	string m_sBoneName;
	string m_sAmmoName;	
	float m_fRadius;
	float m_fDuration;
	bool m_bInvertTeams;

	static const string DAMAGE_CFG_CLASS = "CfgDamages ";
	void AnimDamageParams(string damageName)
	{
		m_sName = damageName;
		string damagePath = DAMAGE_CFG_CLASS + damageName + " ";

		g_Game.ConfigGetText(damagePath + "bone", m_sBoneName);
		g_Game.ConfigGetText(damagePath + "ammo", m_sAmmoName);
		m_fRadius = g_Game.ConfigGetFloat(damagePath + "radius");
		m_fDuration = g_Game.ConfigGetFloat(damagePath + "duration");
		
		m_bInvertTeams = false;
		string str_invert_teams_cfg;
		g_Game.ConfigGetText(damagePath + "invertTeams", str_invert_teams_cfg);
		str_invert_teams_cfg.ToLower();
		if (str_invert_teams_cfg == "true")
		{
			m_bInvertTeams = true;
		}
	}
}

class AnimEffectParams
{
	string m_sName;

	static const string EFFECT_CFG_CLASS = "CfgEffects ";
	void AnimEffectParams(string effectName)
	{
		m_sName = effectName;
		string effectPath = EFFECT_CFG_CLASS + effectName + " ";
		//TODO load params
	}
}
