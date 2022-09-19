class BleedingSourcesManagerServer extends BleedingSourcesManagerBase
{
	const float TICK_INTERVAL_SEC = 3;
	float 	m_Tick;
	bool	m_DisableBloodLoss = false;
	ref array<int> m_DeleteList = new array<int>;
	
	const int STORAGE_VERSION = 103;
	
	protected BleedingSourceZone GetBleedingSourceZone(int bit)
	{
		return m_BleedingSourceZone.Get(GetSelectionNameFromBit(bit));
	}
	
	int GetStorageVersion()
	{
		return STORAGE_VERSION;
	}
	
	void RequestDeletion(int bit)
	{
		m_DeleteList.Insert(bit);
	}
	
	override protected void AddBleedingSource(int bit)
	{
		m_Player.SetBleedingBits(m_Player.GetBleedingBits() | bit );
		super.AddBleedingSource(bit);
	}
	
	override protected bool RemoveBleedingSource(int bit)
	{
		if(!super.RemoveBleedingSource(bit))
		{
			Error("Failed to remove bleeding source:" + bit);
		}
		
		int inverse_bit_mask = ~bit;
		m_Player.SetBleedingBits(m_Player.GetBleedingBits() & inverse_bit_mask );
		
		
		//infection moved here to allow proper working in singleplayer
		float chanceToInfect;
		
		if (m_Item)
		{
			chanceToInfect = m_Item.GetInfectionChance(0, CachedObjectsParams.PARAM1_BOOL);
		}
		else
		{
			chanceToInfect = PlayerConstants.BLEEDING_SOURCE_CLOSE_INFECTION_CHANCE;
		}
		float diceRoll = Math.RandomFloat01();
		if (diceRoll < chanceToInfect)
		{
			m_Player.InsertAgent(eAgents.WOUND_AGENT);
		}
		
		m_Item = null;//reset, so that next call, if induced by self-healing, will have no item
		
		return true;
	}
	
	void RemoveAnyBleedingSource()
	{
		int bleeding_sources_bits = m_Player.GetBleedingBits();
		int rightmost_bit = bleeding_sources_bits & (-bleeding_sources_bits);
		
		RemoveBleedingSource(rightmost_bit);
	}
	
	void RemoveMostSignificantBleedingSource()
	{
		int bit = GetMostSignificantBleedingSource();
		if( bit != 0)
			RemoveBleedingSource(bit);
	}
	
	void RemoveMostSignificantBleedingSourceEx(ItemBase item)
	{
		SetItem(item);
		RemoveMostSignificantBleedingSource();
	}
	
	int GetMostSignificantBleedingSource()
	{
		int bleeding_sources_bits = m_Player.GetBleedingBits();
		
		float highest_flow;
		int highest_flow_bit;
		int bit_offset;
		
		for(int i = 0; i < BIT_INT_SIZE; i++)
		{
			int bit = 1 << bit_offset;
			
			if( (bit & bleeding_sources_bits) != 0 )
			{
				BleedingSourceZone meta = GetBleedingSourceMeta(bit);
				if(meta)
				{
					if( meta.GetFlowModifier() > highest_flow )
					{
						highest_flow = meta.GetFlowModifier();
						highest_flow_bit = bit;
						//Print(meta.GetSelectionName());
					}
				}
			}
			bit_offset++;
		}
		return highest_flow_bit;
	}
	
	void OnTick(float delta_time)
	{
		m_Tick += delta_time;
		if( m_Tick > TICK_INTERVAL_SEC )
		{
			while( m_DeleteList.Count() > 0 )
			{
				RemoveBleedingSource(m_DeleteList.Get(0));
				m_DeleteList.Remove(0);
			} 
			
			float blood_scale = Math.InverseLerp(PlayerConstants.BLOOD_THRESHOLD_FATAL, PlayerConstants.BLEEDING_LOW_PRESSURE_BLOOD, m_Player.GetHealth( "GlobalHealth", "Blood" ));
			blood_scale = Math.Clamp( blood_scale, PlayerConstants.BLEEDING_LOW_PRESSURE_MIN_MOD, 1 );

			for(int i = 0; i < m_BleedingSources.Count(); i++)
			{
				m_BleedingSources.GetElement(i).OnUpdateServer( m_Tick, blood_scale, m_DisableBloodLoss );
			}
			m_Tick = 0;
		}
	}
	
	void ActivateAllBS()
	{
		for(int i = 0; i < m_BleedingSourceZone.Count(); i++)
		{
			int bit = m_BleedingSourceZone.GetElement(i).GetBit();
			if( CanAddBleedingSource(bit) )
			{
				AddBleedingSource(bit);
			}
		}
	}
	
	//damage must be to "Blood" healthType
	void ProcessHit(float damage, EntityAI source, int component, string zone, string ammo, vector modelPos)
	{
		float dmg_max = m_Player.GetMaxHealth(zone, "Blood");
		float dmg = damage;
		float bleed_threshold = GetGame().ConfigGetFloat( "CfgAmmo " + ammo + " DamageApplied " + "bleedThreshold" );
		bleed_threshold = Math.Clamp(bleed_threshold,0,1);
		//Print("dmg_max = " + dmg_max);
		//Print("dmg = " + dmg);
		//Print("bleed_threshold = " + bleed_threshold);

		//hackerino for zombino:
		if (source && source.IsZombie())
		{
			int chance = Math.RandomInt(0,100); //Cast die, probability comes from Blood damage in infected ammo config
			if (chance <= damage)
			{
				AttemptAddBleedingSource(component);
			}
		}
		else if ( dmg > (dmg_max * (1 - bleed_threshold)) )
		{
			AttemptAddBleedingSource(component);
			//Print("BLEEDING");
		}

	}
	
	void DebugActivateBleedingSource(int source)
	{
		RemoveAllSources();
		
		if(source >= m_BleedingSourceZone.Count() || !m_BleedingSourceZone.GetElement(source)) return;
		
		int bit = m_BleedingSourceZone.GetElement(source).GetBit();
		
		if( bit && CanAddBleedingSource(bit) )
		{
			AddBleedingSource(bit);
		}
	}
	
	void SetBloodLoss(bool status)
	{
		m_DisableBloodLoss = status;
	}
	
	void OnStoreSave( ParamsWriteContext ctx )
	{
		//int count = m_BleedingSources.Count();
		int active_bits = m_Player.GetBleedingBits();
		ctx.Write(active_bits);
		
		int bit_offset = 0;
		for(int i = 0; i < BIT_INT_SIZE; i++)
		{
			int bit = 1 << bit_offset;
			if( (bit & active_bits) != 0 )
			{
				int active_time = GetBleedingSourceActiveTime(bit);
				eBleedingSourceType type = GetBleedingSourceType(bit);
				
				ctx.Write(active_time);
				ctx.Write(type);
			}
			bit_offset++;
		}
	}

	bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		int active_bits;
		if(!ctx.Read(active_bits))
		{
			return false;
		}
	
		int bit_offset = 0;
		for(int i = 0; i < BIT_INT_SIZE; i++)
		{
			int bit = 1 << bit_offset;
			if( (bit & active_bits) != 0 && CanAddBleedingSource(bit))
			{
				AddBleedingSource(bit);
				int active_time = 0;
				if(!ctx.Read(active_time))
				{
					return false;
				}
				else
				{
					SetBleedingSourceActiveTime(bit,active_time);
				}
				if( version >= 121 )//type was added in this version
				{
					eBleedingSourceType type = eBleedingSourceType.NORMAL;
					if (!ctx.Read(type))
					{
						return false;
					}
					else
					{
						SetBleedingSourceType(bit,type);
					}
				}
			}
			bit_offset++;
		}
		return true;
	}
	
	
	void ~BleedingSourcesManagerServer()
	{
		if (m_Player && !m_Player.IsAlive())
			RemoveAllSources();
	}
}