class PlayerAgentPool
{
	ref map<int,float> m_VirusPool = new map<int,float>;
	ref array<int> m_VirusPoolArray = new array<int>;
	float m_LastTicked = 0;
	float m_TotalAgentCount;
	PlayerBase m_Player;
	int m_AgentMask;
		
	const int STORAGE_VERSION = 100;
	
	PluginTransmissionAgents m_PluginTransmissionAgents = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
	
	void PlayerAgentPool(PlayerBase player)
	{
		m_Player = player;
	}

	void ~PlayerAgentPool()
	{
		
	}
	
	int GetStorageVersion()
	{
		return STORAGE_VERSION;
	}
	
	void PrintAgents()
	{
		if( m_VirusPool )
		{
			for(int i = 0; i < m_VirusPool.Count(); i++)
			{
				Debug.Log("Agent: "+ m_VirusPool.GetKey(i).ToString(), "Agents");
				Debug.Log("Count: "+ m_VirusPool.GetElement(i).ToString(), "Agents");
			}
		
		}
	}

	void ImmuneSystemTick(float value, float deltaT)//this is a regular tick induced in the the player's immune system
	{
		SpawnAgents(deltaT);
		GrowAgents(deltaT);
	}
	
	void GrowAgents(float deltaT)
	{
		if ( !IsPluginManagerExists() )//check if modules are running
			return;
		
		EStatLevels immunity_level = m_Player.GetImmunityLevel();
		
		m_TotalAgentCount = 0;
		for(int i = 0; i < m_VirusPool.Count(); i++)
		{	
			int agent_id = m_VirusPool.GetKey(i);
			int max_count = m_PluginTransmissionAgents.GetAgentMaxCount( agent_id );

			EStatLevels agent_potency = m_PluginTransmissionAgents.GetAgentPotencyEx( agent_id, m_Player );
			
			float grow_delta;

			if( agent_potency <= immunity_level )
			{
				bool grow_during_antibiotics = m_PluginTransmissionAgents.GrowDuringAntibioticsAttack(agent_id, m_Player);
				if (m_Player.IsAntibioticsActive() && !grow_during_antibiotics)
					continue;
				float invasibility = m_PluginTransmissionAgents.GetAgentInvasibilityEx( agent_id, m_Player );
				grow_delta = invasibility * deltaT;
			}
			else
			{
				float dieoff_speed = m_PluginTransmissionAgents.GetAgentDieOffSpeedEx( agent_id, m_Player );
				grow_delta = -dieoff_speed * deltaT;
			}

			//Print( agent_id );
			//Print( grow_delta );
			
			float old_count = m_VirusPool.Get( agent_id );
			float new_count = old_count + grow_delta;
			new_count = Math.Clamp(new_count, 0,max_count);
			
			m_TotalAgentCount += new_count;
			SetAgentCount(agent_id, new_count);
		}
	}

	void OnStoreSave(ParamsWriteContext ctx)
	{
		//Debug.Log("PlayerAgentPool OnStoreSave called", "Agents");
		
		ctx.Write( m_VirusPool.Count() );
		for(int i = 0; i < m_VirusPool.Count();i++)
		{
			int key = m_VirusPool.GetKey(i);
			int value = m_VirusPool.GetElement(i);
			ctx.Write( key );
			ctx.Write( value );
		}
	}
	
	bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		//Debug.Log("PlayerAgentPool OnStoreLoad called", "Agents");
		int count;
		
		if(!ctx.Read(count))
		{
			return false;
		}
	
		for(int i = 0; i < count;i++)
		{
			int key;
			int value;
			if(!ctx.Read(key))
			{
				return false;
			}
			
			if(!ctx.Read(value))
			{
				return false;
			}
			
			SetAgentCount( key,value );
		}
	
		return true;
	}
	void DigestAgent(int agent_id, float count)
	{
		AddAgent(agent_id, m_PluginTransmissionAgents.GetAgentDigestibility(agent_id) * count);
	}	
	
	void AddAgent(int agent_id, float count)
	{
		int max_count = m_PluginTransmissionAgents.GetAgentMaxCount(agent_id);
		
		if(	!m_VirusPool.Contains(agent_id) && count > 0 )//if it contains, maybe add count only ?
		{
			//m_VirusPool.Insert( agent_id, Math.Clamp(count,0,max_count) );
			SetAgentCount(agent_id,count);
		}
		else
		{
			float new_value = m_VirusPool.Get(agent_id) + count;
			//Print(new_value);
			SetAgentCount(agent_id,new_value);
		}
	}

/*
	void RemoveAgent(int agent_id)
	{
		if(	m_VirusPool.Contains(agent_id) )
		{
			m_VirusPool.Remove( agent_id );
		}
	}
	*/
	void RemoveAgent(int agent_id)
	{
		SetAgentCount(agent_id, 0);
	}
	
	void RemoveAllAgents()
	{
		m_AgentMask = 0;
		m_VirusPool.Clear();
	}
	
	/*
	array<int GetAgents()
	{
		m_VirusPoolArray.Clear();
		
		for(int i = 0; i < m_VirusPool.Count();i++)
		{
			m_VirusPoolArray.Insert( m_VirusPool.GetKey(i) );
		}
		
		return m_VirusPoolArray;
	}
	*/
	
	int GetAgents()
	{
		return m_AgentMask;
	}

	int GetSingleAgentCount(int agent_id)
	{
		if(	m_VirusPool.Contains(agent_id) )
		{
			return m_VirusPool.Get( agent_id );
		}
		else return 0;
	}
	
	float GetTotalAgentCount()
	{
		float agent_count;
		for(int i = 0; i < m_VirusPool.Count(); i++)
		{
			agent_count += m_VirusPool.GetElement(i);
		}
		return agent_count;
	}
	
	void SpawnAgents(float deltaT)
	{
		int count = m_PluginTransmissionAgents.GetAgentList().Count();
		for(int i = 0; i < count;i++)
		{
			AgentBase agent = m_PluginTransmissionAgents.GetAgentList().GetElement(i);
			int agent_id = agent.GetAgentType();
			
			if( GetSingleAgentCount(agent_id)==0 && agent.AutoinfectCheck(deltaT, m_Player) )
			{
				AddAgent(agent_id,100);
			}
		}
	}
	

	void SetAgentCount(int agent_id, float count)
	{
		if(count > 0)
		{
			//Debug.Log("+ growing agent"+ agent_id.ToString() +"to count: "+count.ToString(), "Agents");
			m_VirusPool.Set( agent_id, count);
			m_AgentMask = m_AgentMask | agent_id;
		}
		else
		{
			//Debug.Log("- REMOVING agent"+ agent_id.ToString(), "Agents");
			m_VirusPool.Remove( agent_id );
			m_AgentMask = m_AgentMask & ~agent_id;
		}			
		if(m_Player.m_Agents != m_AgentMask)
		{
			m_Player.m_Agents = m_AgentMask;
			m_Player.SetSynchDirty();
		}
	}

	void AntibioticsAttack(float attack_value)
	{
		for(int i = 0; i < m_VirusPool.Count(); i++)
		{
			int agent_id = m_VirusPool.GetKey(i);
			float antibiotics_resistance = 1 - m_PluginTransmissionAgents.GetAgentAntiboticsResistanceEx(agent_id, m_Player);
			float delta = attack_value * antibiotics_resistance;
			float old_count = m_VirusPool.Get( agent_id );
			float new_count = old_count - delta;
			//PrintString("delta:"+delta.ToString());
			//PrintString("old_count:"+old_count.ToString());
			//PrintString("new_count:"+new_count.ToString());
			SetAgentCount(agent_id, new_count);
		}
	}

	
	void RemoteGrowRequestDebug(ParamsReadContext ctx)
	{
		ctx.Read(CachedObjectsParams.PARAM1_INT);
		int id = CachedObjectsParams.PARAM1_INT.param1;
		int max = m_PluginTransmissionAgents.GetAgentMaxCount(Math.AbsInt(id));
		int grow = max / 10;
		if(id > 0)
		{
			AddAgent(id, grow);
		}
		else if( id < 0)
		{
			AddAgent(-id, -grow);
		}
	}
	
	void GetDebugObject(array<ref Param> object_out)
	{
		int count = m_PluginTransmissionAgents.GetAgentList().Count();
		for(int i = 0; i < count;i++)
		{
			AgentBase agent = m_PluginTransmissionAgents.GetAgentList().GetElement(i);
			string agent_name = agent.GetName();
			int agent_id = agent.GetAgentType();
			int max_agents = m_PluginTransmissionAgents.GetAgentMaxCount(agent_id);
			string amount = GetSingleAgentCount(agent_id).ToString() + "/" + max_agents.ToString();
			object_out.Insert( new Param3<string,string, int>(agent_name, amount, agent_id) );
		}
		object_out.InsertAt(new Param1<int>(count) ,0);
	}
}