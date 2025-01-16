class PlayerAgentPool
{
	const int STORAGE_VERSION = 137;

	int m_AgentMask;	
	float m_LastTicked;
	float m_TotalAgentCount;
	PlayerBase m_Player;
	
	ref map<int,float> m_VirusPool;
	
	protected ref map<int, float> m_AgentTemporaryResistance; 
	
	PluginTransmissionAgents m_PluginTransmissionAgents = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
	
	void PlayerAgentPool(PlayerBase player)
	{
		m_Player 			= player;
		m_LastTicked 		= 0;
		m_VirusPool 		= new map<int,float>();
		
		m_AgentTemporaryResistance = new map<int,float>();
		
		//! DEPRECATED
		m_VirusPoolArray 	= new array<int>();
	}
	
	int GetStorageVersion()
	{
		return STORAGE_VERSION;
	}
	
	void PrintAgents()
	{
		if (m_VirusPool)
		{
			for (int i = 0; i < m_VirusPool.Count(); ++i)
			{
				Debug.Log("Agent: "+ m_VirusPool.GetKey(i).ToString(), "Agents");
				Debug.Log("Count: "+ m_VirusPool.GetElement(i).ToString(), "Agents");
			}
		}
	}

	void ImmuneSystemTick(float value, float deltaT)//this is a regular tick induced in the the player's immune system
	{
		ProcessTemporaryResistance(deltaT);
		SpawnAgents(deltaT);
		GrowAgents(deltaT);
	}
	
	void GrowAgents(float deltaT)
	{
		if (!IsPluginManagerExists())
			return;
		
		EStatLevels immunityLevel = m_Player.GetImmunityLevel();
		
		m_TotalAgentCount = 0;
		for (int i = 0; i < m_VirusPool.Count(); i++)
		{	
			int agentId = m_VirusPool.GetKey(i);
			int maxCount = m_PluginTransmissionAgents.GetAgentMaxCount(agentId);

			EStatLevels agentPotency = m_PluginTransmissionAgents.GetAgentPotencyEx(agentId, m_Player);
			
			float growDelta;

			if (agentPotency <= immunityLevel)
			{
				float temporaryResistance = GetTemporaryResistance(agentId);
				if (temporaryResistance > 1.0)
					continue;

				if (m_Player.IsAntibioticsActive() && !m_PluginTransmissionAgents.GrowDuringMedicalDrugsAttack(agentId, EMedicalDrugsType.CHELATION, m_Player))
					continue;
				
				if (m_Player.IsChelationActive() && !m_PluginTransmissionAgents.GrowDuringMedicalDrugsAttack(agentId, EMedicalDrugsType.CHELATION, m_Player))
					continue

				float invasibility = m_PluginTransmissionAgents.GetAgentInvasibilityEx(agentId, m_Player);
				growDelta = invasibility * deltaT;
			}
			else
			{
				float dieOffSpeed = m_PluginTransmissionAgents.GetAgentDieOffSpeedEx(agentId, m_Player);
				growDelta = -dieOffSpeed * deltaT;
			}
			
			float oldCount = m_VirusPool.Get(agentId);
			float newCount = oldCount + growDelta;
			newCount = Math.Clamp(newCount, 0, maxCount);
			
			m_TotalAgentCount += newCount;
			SetAgentCount(agentId, newCount);
		}
	}
	
	protected void ProcessTemporaryResistance(float deltaTime)
	{
		foreach (int agentId, float timeResistance : m_AgentTemporaryResistance)
		{
			float temporaryResistance = GetTemporaryResistance(agentId);
			
			if (temporaryResistance > 1.0)
			{
				float newResistanceValue = temporaryResistance - deltaTime;
				SetTemporaryResistance(agentId, newResistanceValue);
			}
			else
				SetTemporaryResistance(agentId, 0.0);
		}	
	}

	void OnStoreSave(ParamsWriteContext ctx)
	{
		int count = m_PluginTransmissionAgents.GetAgentList().Count();
		array<int> agentList = m_PluginTransmissionAgents.GetAgentList().GetKeyArray();
		foreach (int agentId : agentList)
		{
			ctx.Write(agentId);
			ctx.Write(GetSingleAgentCount(agentId));
			ctx.Write(GetTemporaryResistance(agentId));
		}
	}
	
	bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		int count;
		if (version >= 137)
		{
			count = m_PluginTransmissionAgents.GetAgentList().Count();
		}
		else
		{
			if (!ctx.Read(count))
				return false;
		}

		
		for (int i = 0; i < count; ++i)
		{
			int agentId;
			if (!ctx.Read(agentId))
				return false;

			int agentCount;			
			if (!ctx.Read(agentCount))
				return false;

			if (version >= 137)
			{
				float agentTemporaryResistanceTime;
				if (!ctx.Read(agentTemporaryResistanceTime))
					return false;
				
				SetTemporaryResistance(agentId, agentTemporaryResistanceTime);
			}
			
			SetAgentCount(agentId, agentCount);
		}
	
		return true;
	}
	void DigestAgent(int agent_id, float count)
	{
		AddAgent(agent_id, m_PluginTransmissionAgents.GetAgentDigestibilityEx(agent_id, m_Player) * count);
	}	
	
	void AddAgent(int agent_id, float count)
	{
		if (GetTemporaryResistance(agent_id) > 0)
			return;

		int max_count = m_PluginTransmissionAgents.GetAgentMaxCount(agent_id);
		
		if (!m_VirusPool.Contains(agent_id) && count > 0)//if it contains, maybe add count only ?
		{
			SetAgentCount(agent_id,count);
		}
		else
		{
			float newValue = m_VirusPool.Get(agent_id) + count;
			SetAgentCount(agent_id, newValue);
		}
	}

	void RemoveAgent(int agent_id)
	{
		SetAgentCount(agent_id, 0);
	}
	
	void RemoveAllAgents()
	{
		m_AgentMask = 0;
		m_VirusPool.Clear();

		ResetTemporaryResistance();
	}
	
	void ReduceAgent(int id, float percent)
	{
		percent = Math.Clamp(percent, 0, 100);
		float reduction = percent * 0.01;
		
		int agentCount = GetSingleAgentCount(id);
		agentCount -= agentCount * reduction;
		
		SetAgentCount(id, agentCount);
	}
	
	int GetAgents()
	{
		return m_AgentMask;
	}

	int GetSingleAgentCount(int agent_id)
	{
		if (m_VirusPool.Contains(agent_id))
			return m_VirusPool.Get(agent_id);

		return 0;
	}
	
	float GetTotalAgentCount()
	{
		float agentCount;
		for (int i = 0; i < m_VirusPool.Count(); i++)
			agentCount += m_VirusPool.GetElement(i);

		return agentCount;
	}
	
	void SpawnAgents(float deltaT)
	{
		int count = m_PluginTransmissionAgents.GetAgentList().Count();
		for (int i = 0; i < count; ++i)
		{
			AgentBase agent = m_PluginTransmissionAgents.GetAgentList().GetElement(i);
			int agentId = agent.GetAgentType();

			if (GetSingleAgentCount(agentId) == 0 && agent.AutoinfectCheck(deltaT, m_Player))
				AddAgent(agentId, agent.GetAutoinfectCount());
		}
	}

	void SetAgentCount(int agent_id, float count)
	{
		if (count > 0)
		{
			//Debug.Log("+ growing agent"+ agent_id.ToString() +"to count: "+count.ToString(), "Agents");
			m_VirusPool.Set(agent_id, count);
			m_AgentMask = m_AgentMask | agent_id;
		}
		else
		{
			//Debug.Log("- REMOVING agent"+ agent_id.ToString(), "Agents");
			m_VirusPool.Remove(agent_id);
			m_AgentMask = m_AgentMask & ~agent_id;
		}

		if (m_Player.m_Agents != m_AgentMask)
		{
			m_Player.m_Agents = m_AgentMask;
			m_Player.SetSynchDirty();
		}
	}

	void AntibioticsAttack(float attack_value)
	{
		for (int i = 0; i < m_VirusPool.Count(); ++i)
		{
			int agentId = m_VirusPool.GetKey(i);
			float resistance = 1 - m_PluginTransmissionAgents.GetAgentAntiboticsResistanceEx(agentId, m_Player);
			float delta = attack_value * resistance;
			float actualAgentCount = m_VirusPool.Get(agentId);
			float newAgentCount = actualAgentCount - delta;
			SetAgentCount(agentId, newAgentCount);
		}
	}

	void DrugsAttack(EMedicalDrugsType drugType, float attackValue)
	{
		switch (drugType)
		{
			case EMedicalDrugsType.ANTIBIOTICS:
				AntibioticsAttack(attackValue);
				break;
			default:
				for (int i = 0; i < m_VirusPool.Count(); ++i)
				{
					int agentId = m_VirusPool.GetKey(i);
					float actualAgentCount = m_VirusPool.Get(agentId);
					float newAgentCount = actualAgentCount - attackValue;
					SetAgentCount(agentId, newAgentCount);
				}		
		}
	}
	
	void SetTemporaryResistance(int agentId, float time)
	{
		m_AgentTemporaryResistance[agentId] = Math.Clamp(time, 0.0, int.MAX);
	}
	
	float GetTemporaryResistance(int agentId)
	{
		if (m_AgentTemporaryResistance.Contains(agentId))
			return m_AgentTemporaryResistance[agentId];
		
		return 0.0;
	}
	
	private void ResetTemporaryResistance()
	{
		foreach (int agentId, float value : m_AgentTemporaryResistance)
			SetTemporaryResistance(agentId, 0.0);
	}
	
	void RemoteGrowRequestDebug(ParamsReadContext ctx)
	{
		ctx.Read(CachedObjectsParams.PARAM1_INT);
		int id = CachedObjectsParams.PARAM1_INT.param1;
		int max = m_PluginTransmissionAgents.GetAgentMaxCount(Math.AbsInt(id));
		int grow = max / 10;

		if (id > 0)
			AddAgent(id, grow);
		else if (id < 0)
			AddAgent(-id, -grow);
	}
	
	void GetDebugObject(array<ref Param> object_out)
	{
		int count = m_PluginTransmissionAgents.GetAgentList().Count();
		for (int i = 0; i < count; i++)
		{
			AgentBase agent 	= m_PluginTransmissionAgents.GetAgentList().GetElement(i);
			string agentName	= agent.GetName();
			int agentId 		= agent.GetAgentType();
			int maxAgents 		= m_PluginTransmissionAgents.GetAgentMaxCount(agentId);
			string amount 		= GetSingleAgentCount(agentId).ToString() + "/" + maxAgents.ToString();

			float tempResistance = GetTemporaryResistance(agentId);

			object_out.Insert(new Param4<string,string, int, float>(agentName, amount, agentId, tempResistance));
		}

		object_out.InsertAt(new Param1<int>(count) ,0);
	}

	//! DEPRECATED
	ref array<int> m_VirusPoolArray;
}