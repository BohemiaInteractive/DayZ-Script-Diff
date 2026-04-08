/**
 * \brief Keeps track of agents and their simulation
 *		- adding/reducing of agents
 *		- autoinfection
 *		- temporary resistance for agents
 *		- reaction on drugs
 */
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
	}
	
	int GetStorageVersion()
	{
		return STORAGE_VERSION;
	}

	/**
	 * \brief Agent pool simulation entry point
	 * @param value Immunity value (deprecated, used in previous versions)
	 * @param deltaT tick
	 */
	void ImmuneSystemTick(float value, float deltaT)//this is a regular tick induced in the the player's immune system
	{
		ProcessTemporaryResistance(deltaT);
		SpawnAgents(deltaT);
		GrowAgents(deltaT);
	}

	/**
	 * \brief Agent's growth/death simulation
	 *		- based on the potency, invasibility
	 * 		- takes into account the temporary resistance
	 * @param deltaT tick
	 */
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

				if (m_Player.IsAntibioticsActive() && !m_PluginTransmissionAgents.GrowDuringMedicalDrugsAttack(agentId, EMedicalDrugsType.ANTIBIOTICS, m_Player))
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

	/**
	 * \brief Temporary resistance simulation
	 */
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
	
	/**
	 * \brief Digest (add) agent from food/drink in PlayerStomach into Agent Pool
	 * @param agent_id Id of agent (see eAgents enum)
	 * @param count Amount of agents to add
	 */
	void DigestAgent(int agent_id, float count)
	{
		AddAgent(agent_id, m_PluginTransmissionAgents.GetAgentDigestibilityEx(agent_id, m_Player) * count);
	}	

	/**
	 * \brief Add agent into Agent Pool
	 * @param agent_id Id of agent (see eAgents enum)
	 * @param count Amount of agents to add
	 */
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

	/**
	 * \brief Remove agent from Agent Pool
	 * @param agent_id Id of agent (see eAgents enum)
	 */
	void RemoveAgent(int agent_id)
	{
		SetAgentCount(agent_id, 0);
	}

	/**
	 * \brief Remove all agents from Agent Pool
	 * @param agent_id Id of agent (see eAgents enum)
	 */	
	void RemoveAllAgents()
	{
		m_AgentMask = 0;
		m_VirusPool.Clear();

		ResetTemporaryResistance();
	}

	/**
	 * \brief Reduce count of specified agent by a given percentage from Agent Pool
	 * @param agent_id Id of agent (see eAgents enum)
	 * @param percent How many percents of the agents should be reduced
	 */	
	void ReduceAgent(int id, float percent)
	{
		percent = Math.Clamp(percent, 0, 100);
		float reduction = percent * 0.01;
		
		int agentCount = GetSingleAgentCount(id);
		agentCount -= agentCount * reduction;
		
		SetAgentCount(id, agentCount);
	}
	/**
	 * \brief Reduce bitmask of currently active agents
	 */		
	int GetAgents()
	{
		return m_AgentMask;
	}

	/**
	 * \brief Number of agents of specified id
	 * @param agent_id Id of agent to add into pool (see eAgents)
	 * \return Count of agents specified in param
	 */	
	int GetSingleAgentCount(int agent_id)
	{
		if (m_VirusPool.Contains(agent_id))
			return m_VirusPool.Get(agent_id);

		return 0;
	}

	/**
	 * \brief Total number of agents active
	 * \return Agents count
	 */	
	float GetTotalAgentCount()
	{
		float agentCount;
		for (int i = 0; i < m_VirusPool.Count(); i++)
			agentCount += m_VirusPool.GetElement(i);

		return agentCount;
	}

	/**
	 * \brief Autoinfection mechanism for agents with that attribute enabled
	 * @param deltaT tick
	 */	
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

	/**
	 * \brief Directly set the count of agents for give id in pool
	 * @param agent_id Id of agent to add into pool (see eAgents)
	 * @param count Number of agents to be set
	 */	
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

	/**
	 * \brief Antibiotics treatment agains agents which are not resistent to it (see agent attributes)
	 * @param attack_value Strength of the anitibiotics attack
	 */	
	void AntibioticsAttack(float attack_value)
	{
		AntibioticsAttackEx(attack_value, EMedicalDrugsType.ANTIBIOTICS);
	}
	
	void AntibioticsAttackEx(float attack_value, EMedicalDrugsType drugType)
	{
		for (int i = 0; i < m_VirusPool.Count(); ++i)
		{
			int agentId = m_VirusPool.GetKey(i);
			float resistance = 1 - m_PluginTransmissionAgents.GetAgentSpecificDrugResistance(agentId, drugType, m_Player);
			float delta = attack_value * resistance;
			float actualAgentCount = m_VirusPool.Get(agentId);
			float newAgentCount = actualAgentCount - delta;
			SetAgentCount(agentId, newAgentCount);
		}
	}

	/**
	 * \brief Drugs attack calculation
	 * @param drugType Type of drug used (see EMedicalDrugsType enum)
	 * @param attackValue Strength of the drug attack
	 */	
	void DrugsAttack(EMedicalDrugsType drugType, float attackValue)
	{
		for (int i = 0; i < m_VirusPool.Count(); ++i)
		{
			//getting the immunity of the agent and exiting the function early if that agent is immune to the drug
			int agentId = m_VirusPool.GetKey(i);	
			float resistance = 1 - m_PluginTransmissionAgents.GetAgentSpecificDrugResistance(agentId, drugType, m_Player);
			float delta = attackValue * resistance;
			float actualAgentCount = m_VirusPool.Get(agentId);
			float newAgentCount = actualAgentCount - delta;
			SetAgentCount(agentId, newAgentCount);
		}
	}

	/**
	 * \brief Sets temporary resistance time against specified agent contraction
	 * @param @param agent_id Id of agent to add into pool (see eAgents)
	 * @param time Length of resistance in seconds
	 */	
	void SetTemporaryResistance(int agentId, float time)
	{
		m_AgentTemporaryResistance[agentId] = Math.Clamp(time, 0.0, int.MAX);
	}

	/**
	 * \brief Returns remaining temporary resistance time for specified agent
	 * @param @param agent_id Id of agent to add into pool (see eAgents)
	 * \return time in seconds
	 */	
	float GetTemporaryResistance(int agentId)
	{
		if (m_AgentTemporaryResistance.Contains(agentId))
			return m_AgentTemporaryResistance[agentId];
		
		return 0.0;
	}

	/**
	 * \brief Resets temporary resistance for all agents (internal usage only)
	 */
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

	//! DEPRECATED
	ref array<int> m_VirusPoolArray = new array<int>();
}