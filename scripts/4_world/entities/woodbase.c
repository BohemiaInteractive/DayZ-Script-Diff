enum EHarvestType
{
	NORMAL,
	BARK
}

class WoodBase extends Plant
{
	
	static bool 	m_IsCuttable;
	static int 		m_PrimaryDropsAmount = -1;
	static int 		m_SecondaryDropsAmount = -1;
	static float 	m_ToolDamage = -1.0;
	static float 	m_CycleTimeOverride = -1.0;
	static string 	m_PrimaryOutput = ""; //some nonsensical item for debugging purposes
	static string 	m_SecondaryOutput = ""; //some nonsensical item for debugging purposes
	static string 	m_BarkType = "";
	
	/*
	 bool 	m_IsCuttable;
	 int 	m_PrimaryDropsAmount = -1;
	 int 	m_SecondaryDropsAmount = -1;
	 float 	m_ToolDamage = -1.0;
	 float 	m_CycleTimeOverride = -1.0;
	 string 	m_PrimaryOutput = ""; //some nonsensical item for debugging purposes
	 string 	m_SecondaryOutput = ""; //some nonsensical item for debugging purposes
	 string 	m_BarkType = "";
	*/
	
	void WoodBase()
	{
		//InitMiningValues();
	}
	
	
	void InitMiningValues()
	{
		//m_IsCuttable = false;
	};
	
	override bool IsWoodBase()
	{
		return true;
	}

	override bool IsCuttable()
	{
		return ConfigGetBool("isCuttable");
	}
	
	bool HasPlayerCollisionParticle()
	{
		return true;
	}
	
	bool HasPlayerCollisionSound()
	{
		return true;
	}
	
	int GetPrimaryDropsAmount()
	{
		return ConfigGetInt("primaryDropsAmount");
	}
	
	int GetSecondaryDropsAmount()
	{
		return ConfigGetInt("secondaryDropsAmount");
	}
	
	float GetToolDamage()
	{
		return ConfigGetFloat("toolDamage");
	}
	
	float GetCycleTimeOverride()
	{
		return ConfigGetFloat("cycleTimeOverride");
	}
	
	string GetPrimaryOutput()
	{
		return ConfigGetString("primaryOutput");
	}
	
	string GetSecondaryOutput()
	{
		return ConfigGetString("secondaryOutput");
	}
	
	string GetBarkType()
	{
		return ConfigGetString("barkType");
	}
	
	
	
	int GetAmountOfDrops(ItemBase item)
	{
		if ( GetPrimaryDropsAmount() > 0 )
		{
			if ( IsTree() && item && ( item.KindOf("Knife") || item.IsInherited(Screwdriver) ) )
			{
				return -1;
			}
			else
			{
				return GetPrimaryDropsAmount(); 
			}
		}
		else
		{
			if ( item && ( item.KindOf("Knife") || item.IsInherited(Screwdriver) ) )
			{
				return -1;
			}
			else if ( item && item.KindOf("Axe") )
			{
				return 3;
			}
			else
			{
				return 100;
			}
		}
	}
	
	int GetAmountOfDropsEx(ItemBase item, EHarvestType type)
	{
		if ( GetPrimaryDropsAmount() > 0 )
		{
			if ( IsTree() && item && type == EHarvestType.BARK )
			{
				return -1;
			}
			else
			{
				return GetPrimaryDropsAmount(); 
			}
		}
		else
		{
			if ( item && type == EHarvestType.BARK )
			{
				return -1;
			}
			else if ( item &&  type == EHarvestType.NORMAL )
			{
				return 3;
			}
			else
			{
				return 100;
			}
		}
	}
	
	void GetMaterialAndQuantityMap(ItemBase item, out map<string,int> output_map)
	{
		if ( IsTree() && item && ( item.KindOf("Knife") || item.IsInherited(Screwdriver) ) && GetBarkType() != "" )
		{
			output_map.Insert(GetBarkType(),1);
		}
		else
		{
			output_map.Insert(GetPrimaryOutput(),1);
		}
	}
	
	void GetMaterialAndQuantityMapEx(ItemBase item, out map<string,int> output_map, EHarvestType type)
	{
		if ( IsTree() && item && type == EHarvestType.BARK && GetBarkType() != "" )
		{
			output_map.Insert(GetBarkType(),1);
		}
		else
		{
			output_map.Insert(GetPrimaryOutput(),1);
		}
	}
	
	float GetDamageToMiningItemEachDrop(ItemBase item)
	{
		if (GetToolDamage() > -1)
			return GetToolDamage();
		
		if ( IsTree() )
		{
			if ( item && item.KindOf("Knife") )
			{
				return 20;
			}
			else if ( item && item.KindOf("Axe") )
			{
				return 20;
			}
			else
			{
				return 0; 
			}
		}
		else
		{
			if ( item && item.KindOf("Knife") )
			{
				return 30;
			}
			else if ( item && item.KindOf("Axe") )
			{
				return 30;
			}
			else
			{
				return 0;
			}
		}
	}
	
	float GetDamageToMiningItemEachDropEx(ItemBase item, EHarvestType type)
	{
		if (GetToolDamage() > -1)
			return GetToolDamage();
		
		if ( IsTree() )
		{
			if ( item && type == EHarvestType.BARK )
			{
				return 20;
			}
			else if ( item && type == EHarvestType.NORMAL )
			{
				return 20;
			}
			else
			{
				return 0; 
			}
		}
		else
		{
			if ( item && type == EHarvestType.BARK )
			{
				return 30;
			}
			else if ( item && type == EHarvestType.NORMAL )
			{
				return 30;
			}
			else
			{
				return 0;
			}
		}
	}
	
	override bool CanBeActionTarget()
	{
		return super.CanBeActionTarget() && !IsDamageDestroyed();
	}
}

class TreeEffecterParameters : EffecterParameters
{
	float m_Radius;

	void TreeEffecterParameters(string type, float lifespan, int radius)
	{
		m_Radius = radius;
	}
}

class TreeEffecter : EffecterBase
{
	protected ref array<EffectParticleGeneral> m_Effects = null;
	private float m_Radius = -1;
	private float m_RadiusSync = -1;
	
	void TreeEffecter()
	{
		if (!g_Game.IsServer() || !g_Game.IsMultiplayer())
		{
			m_Effects = new array<EffectParticleGeneral>;
		}
		
		RegisterNetSyncVariableFloat("m_RadiusSync");
	}
	
	override void Init(int id, EffecterParameters parameters)
	{
		super.Init(id, parameters);
		TreeEffecterParameters par = TreeEffecterParameters.Cast(parameters);
		SetRadius(par.m_Radius);
	}
	
	void SetRadius(float radius)
	{
		m_RadiusSync = radius;
		Process();
	}
	
	override void OnVariablesSynchronized()
	{	
		if (m_RadiusSync != m_Radius)
		{
			array<Object> objects = new array<Object>;
			array<CargoBase> proxies = new array<CargoBase>;
			EffectParticleGeneral newEffect;
			
			foreach (EffectParticleGeneral oldEffect : m_Effects)
			{
				SEffectManager.DestroyEffect(oldEffect);
			}
			
			m_Effects.Clear();
		
			g_Game.GetObjectsAtPosition(GetWorldPosition(), m_RadiusSync, objects, proxies);

			for (int i = 0; i < objects.Count(); i++)
			{
				WoodBase plant = WoodBase.Cast(objects[i]);
				if (plant)
				{
					string particle;
					int particleID;
					string configPath = "CfgNonAIVehicles " + plant.GetType() + " FxFallingParticleEffect particle";
					g_Game.ConfigGetText(configPath, particle);
					
					if (particle != "")
					{
						particleID = ParticleList.RegisterParticle(particle);
					}
					
					if (particleID > 0)
					{
						LOD lod = plant.GetLODByName(LOD.NAME_MEMORY);
						Selection selection = lod.GetSelectionByName("ptcFalling");
						if (selection)
						{
							vector selectionPos;
							for (int j = 0; j < selection.GetVertexCount(); j++)
							{
								newEffect = new EffectParticleGeneral();
								newEffect.SetParticle(particleID);
								selectionPos = selection.GetVertexPosition(lod, j);
								SEffectManager.PlayInWorld(newEffect, plant.GetPosition() + selectionPos);
								m_Effects.Insert(newEffect);
							}
							
							//play accompanying sounds
							selectionPos = ModelToWorld(selectionPos);
							EffectSound sound = SEffectManager.PlaySoundEnviroment("snow_fallen_trees_SoundSet", selectionPos);
							sound.SetAutodestroy(true);
						}
					}
				}
			}
		}
				
		if (m_CommandSync != m_Command)
		{
			foreach (EffectParticleGeneral effect : m_Effects)
			{
				switch (m_CommandSync)
				{
					case EffecterCommands.START:
						if (effect && !effect.IsPlaying())
						{
							effect.SetParticle(effect.m_LastParticleID);
							effect.Start();
						}
						break;
			
					case EffecterCommands.STOP:
						if (effect && effect.IsPlaying())
						{
							effect.Stop();
						}
						break;
			
					case EffecterCommands.REACTIVATE0:
					case EffecterCommands.REACTIVATE1:
						if (effect)
						{
							effect.SetParticle(effect.m_LastParticleID);
						}
						if (!effect.IsPlaying())
						{
							effect.Start();
						}
						break;
			
					default:
						break;
				}
			}
			m_Command = m_CommandSync;
		}
	}
	
	void ~TreeEffecter()
	{
		if (m_Effects)
		{
			foreach (EffectParticleGeneral effect : m_Effects)
			{
				SEffectManager.DestroyEffect(effect);
			}
		}
	}
}