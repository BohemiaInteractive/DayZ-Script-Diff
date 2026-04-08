enum InjectTypes
{
    PLAYER_TO_ITEM, 
	ITEM_TO_PLAYER,
	PLAYER_AIR_PLAYER,
};

/**
 * \brief Plugin interface for controlling of agent pool system
 *		- access to specific agent attributes
 *		- various transmission operations
 */
class PluginTransmissionAgents extends PluginBase
{
	static ref map<int, ref AgentBase> m_AgentList =  new map<int, ref AgentBase>();
	ref map<int, string> m_SimpleAgentList = new map<int, string>; //! simple <eAgents, agentName> pair
	bool m_IsConstructed = false;
	
	void PluginTransmissionAgents()
	{
		//add new agents here
		RegisterAgent(new InfluenzaAgent);
		RegisterAgent(new CholeraAgent);
		RegisterAgent(new SalmonellaAgent);
		RegisterAgent(new BrainAgent);
		RegisterAgent(new FoodPoisonAgent);
		RegisterAgent(new ChemicalAgent);
		RegisterAgent(new WoundAgent);
		RegisterAgent(new NerveAgent);
		RegisterAgent(new HeavyMetalAgent);
	}

	/**
	 * \brief Registers new agent into system
	 * @param agent New agent class
	 */	
	void RegisterAgent(AgentBase agent)
	{
		m_AgentList.Insert(agent.GetAgentType(), agent);
	}

	/**
	 * \brief Builds simplified list of agents in <id, name> format
	 */
	void ConstructSimpleAgentList()
	{
		string agent_name;
		int agent_type;
		
		for(int i = 0; i < m_AgentList.Count();i++)
		{
			AgentBase agent = m_AgentList.GetElement(i);
			agent_name = agent.GetName();
			agent_type = agent.GetAgentType();
			m_SimpleAgentList.Insert(agent_type, agent_name);
		}
	}

	/**
	 * \brief Returns map of all registered agent classes
	 * \return map<int, ref AgentBase> map of agent classes
	 */		
	map<int, ref AgentBase> GetAgentList()
	{
		return	m_AgentList;
	}

	/**
	 * \brief Returns map of all registered agents in simplified format(for non-gameplay purposas mainly)
	 * \return map<int, ref AgentBase> map of agent classes
	 */
	map<int, string> GetSimpleAgentList()
	{
		if( !m_IsConstructed )
		{
			ConstructSimpleAgentList();
			m_IsConstructed	= true;
		}
		return m_SimpleAgentList;
	}

	/**
	 * \brief Returns agent's name from given id
	 * @param agent_id Id of agent (see eAgents enum)
	 * \return Agent name
	 */		
	static string GetNameByID(int agent_id)
	{
		return m_AgentList.Get(agent_id).GetName();
	}

	/**
	 * \brief Removes all agents from given entity
	 * @param target Entity to remove agents from
	 */			
	void RemoveAllAgents(EntityAI target)
	{
		target.RemoveAllAgents();
	}

	/**
	 * \brief Removes given agent from given entity
	 * @param target Entity to remove agents from
	 * @param agent_id Id of agent (see eAgents enum)
	 */		
	static void RemoveAgent(EntityAI target, int agent_id)
	{
		target.RemoveAgent(agent_id);
	}	

	/**
	 * \brief Returns transferabilityIn attribute for given agent
	 * @param agent_id Id of agent (see eAgents enum)
	 * \return AgentBase::m_TransferabilityIn
	 */	
	protected float GetAgentTransferabilityIn(int agent_id)
	{
		if( !m_AgentList.Get(agent_id) ) return 0;
		return m_AgentList.Get(agent_id).GetTransferabilityIn();
	}
	
	bool GrowDuringMedicalDrugsAttack(int agentId, EMedicalDrugsType drugType, PlayerBase player)
	{
		AgentBase agent = m_AgentList.Get(agentId);
		if (!agent)
			return true;

		return agent.GrowDuringMedicalDrugsAttack(drugType, player);
	}
	
	/**
	 * \brief Returns dieOfSpeed attribute for given agent (see GetDieOffSpeedEx())
	 */	
	float GetDieOffSpeed( int agent_id )
	{
		if( !m_AgentList.Get(agent_id) ) 
			return 0;
		return m_AgentList.Get(agent_id).GetDieOffSpeed();
	}

	/**
	 * \brief Returns dieOfSpeed attribute for given agent
	 * @param agent_id Id of agent (see eAgents enum)
	 * @param player Actual player reference
	 * \return AgentBase::m_DieOffSpeed
	 */	
	float GetAgentDieOffSpeedEx(int agent_id, PlayerBase player)
	{
		if( !m_AgentList.Get(agent_id) ) 
			return true;
		return m_AgentList.Get(agent_id).GetDieOffSpeedEx(player);
	}

	/**
	 * \brief Returns potency attribute for given agent (see GetAgentPotencyEx())
	 */		
	EStatLevels GetPotency( int agent_id )
	{
		if( !m_AgentList.Get(agent_id) ) 
			return 0;
		return m_AgentList.Get(agent_id).GetPotency();
	}

	/**
	 * \brief Returns potency attribute for given agent
	 * @param agent_id Id of agent (see eAgents enum)
	 * @param player Actual player reference
	 * \return AgentBase::m_Potency
	 */	
	EStatLevels GetAgentPotencyEx(int agent_id, PlayerBase player)
	{
		if( !m_AgentList.Get(agent_id) ) 
			return true;
		return m_AgentList.Get(agent_id).GetPotencyEx(player);
	}

	/**
	 * \brief Returns invasibility attribute for given agent
	 * @param agent_id Id of agent (see eAgents enum)
	 * @param player Actual player reference
	 * \return AgentBase::m_Invasibility
	 */	
	float GetAgentInvasibilityEx(int agent_id, PlayerBase player)
	{
		if( !m_AgentList.Get(agent_id) ) 
			return true;
		return m_AgentList.Get(agent_id).GetInvasibilityEx(player);
	}
	
	/**
	* Returns the resistance to the specific drug 
	*/
	int GetAgentSpecificDrugResistance(int agent_id, EMedicalDrugsType drugType, PlayerBase player)
	{
		if(!m_AgentList.Get(agent_id)) 
			return 0;
		return m_AgentList.Get(agent_id).GetDrugResistance(drugType, player);
	}
	

	/**
	 * \brief Returns antibiotics resistance attribute for given agent see GetAgentAntiboticsResistanceEx()
	 */
	float GetAgentAntiboticsResistance( int agent_id )
	{
		if( !m_AgentList.Get(agent_id) ) 
			return 0;
		return m_AgentList.Get(agent_id).GetAntiboticsResistance();
	}

	/**
	 * \brief Returns antibiotics resistance attribute for given agent
	 * @param agent_id Id of agent (see eAgents enum)
	 * @param player Actual player reference
	 * \return AgentBase::m_AntibioticsResistance
	 */	
	float GetAgentAntiboticsResistanceEx( int agent_id , PlayerBase player)
	{
		if( !m_AgentList.Get(agent_id) ) 
			return 0;
		return m_AgentList.Get(agent_id).GetAntibioticsResistanceEx(player);
	}

	/**
	 * \brief Returns transferabilityOut attribute for given agent
	 * @param agent_id Id of agent (see eAgents enum)
	 * \return AgentBase::m_TransferabilityOut
	 */		
	protected float GetAgentTransferabilityOut( int agent_id )
	{
		if(!m_AgentList.Get(agent_id)) 
			return 0;
		return m_AgentList.Get(agent_id).GetTransferabilityOut();
	}	

	/**
	 * \brief Returns transferabilitAiryOut attribute for given agent
	 * @param agent_id Id of agent (see eAgents enum)
	 * \return AgentBase::m_TransferabilityAirOut
	 */	
	protected float GetAgentTransferabilityAirOut( int agent_id )
	{
		if(!m_AgentList.Get(agent_id)) 
			return 0;
		return m_AgentList.Get(agent_id).GetTransferabilityAirOut();
	}

	/**
	 * \brief Returns invasibility attribute for given agent
	 * @param agent_id Id of agent (see eAgents enum)
	 * \return AgentBase::m_Invasibility
	 */	
	float GetAgentInvasibility( int agent_id )
	{
		if( !m_AgentList.Get(agent_id) ) 
			return 0;
		return m_AgentList.Get(agent_id).GetInvasibility();
	}

	/**
	 * \brief Returns stomach digetibility attribute for given agent (see GetAgentDigestibilityEx())
	 */		
	float GetAgentDigestibility( int agent_id )
	{
		if( !m_AgentList.Get(agent_id) ) 
			return 0;
		return m_AgentList.Get(agent_id).GetDigestibility();
	}

	/**
	 * \brief Returns stomach digetibility attribute for given agent
	 * @param agent_id Id of agent (see eAgents enum)
	 * @param player Actual player reference
	 * \return AgentBase::m_Digestibility
	 */	
	float GetAgentDigestibilityEx(int agent_id, PlayerBase player)
	{
		if (!m_AgentList.Get(agent_id))
			return 0;
		return m_AgentList.Get(agent_id).GetDigestibilityEx(player);
	}

	/**
	 * \brief Returns max count attribute for given agent
	 * @param agent_id Id of agent (see eAgents enum)
	 * \return AgentBase::m_MaxCount
	 */	
	static int GetAgentMaxCount( int agent_id )
	{
		if( !m_AgentList.Get(agent_id) ) 
			return 0;
		return m_AgentList.Get(agent_id).GetMaxCount();
	}

	/**
	 * \brief Process transmission of agents between entities (for specified transmission type)
	 * @param source Entity to transfer agents from
	 * @param target Entity to transfer agents to
	 * @param pathway Type of transmission used
	 * @param dose_size Number of agents to be transmitted
	 * @param agents Bit mask specifing agents to transmit
	 * \return count Number of transmitted agents
	 */
	float TransmitAgentsEx(EntityAI source, EntityAI target, int pathway, int dose_size = 1000, int agents = 0)
	{
		//Debug.Log("Transmitting agents for source: " +source.ToString()+", target: " +target.ToString(),"Agents");
		int sourceAgents = agents;
		int targetAgents;
		if(!sourceAgents && source) sourceAgents = source.GetAgents();//do not set sourceAgents again if already set
		if(target) targetAgents = target.GetAgents();
		int pollution = g_Game.GetMission().GetWorldData().GetPollution();
		
		float count = 0;
		
		switch (pathway)
		{
			case AGT_INV_OUT: //item leaving inventory
				break;
				 
			case AGT_INV_IN: //item entering inventory
				break;
				
			case AGT_UACTION_TOUCH: //player touched the item
				//InjectAgents( source, targetAgents ,GetProtectionLevel(DEF_BIOLOGICAL,InventorySlots.GLOVES, player) );
				break;

			case AGT_WATER_POND:
				if (pollution & EPollution.HEAVYMETAL)
				{
					sourceAgents = sourceAgents | eAgents.HEAVYMETAL;
				}
				sourceAgents = sourceAgents | eAgents.CHOLERA;
				InjectAgentsWithPlayer( target, sourceAgents , 0, 1, InjectTypes.ITEM_TO_PLAYER );
				break;
			
			case AGT_WATER_HOT_SPRING:
				sourceAgents = sourceAgents | eAgents.FOOD_POISON | eAgents.HEAVYMETAL;
				InjectAgentsWithPlayer( target, sourceAgents , 0, 1, InjectTypes.ITEM_TO_PLAYER );
				break;
			
			case AGT_SNOW:
				if (pollution & EPollution.HEAVYMETAL)
				{
					sourceAgents = sourceAgents | eAgents.HEAVYMETAL;
				}
				InjectAgentsWithPlayer( target, sourceAgents , 0, 1, InjectTypes.ITEM_TO_PLAYER );
				break;
				
			case AGT_UACTION_CONSUME:
				//InjectAgentsWithPlayer( target, sourceAgents , 0, dose_size, InjectTypes.ITEM_TO_PLAYER );
				InjectAgentsWithPlayer( source, targetAgents , 0, 1, InjectTypes.PLAYER_TO_ITEM );
				break;
			
			case AGT_UACTION_TO_PLAYER: //user action of a consumption, only from item to player
				InjectAgentsWithPlayerCount( target, sourceAgents , 0, dose_size, InjectTypes.ITEM_TO_PLAYER );
				break;

			case AGT_UACTION_TO_ITEM: //to transfer from the player to the consumed item
				InjectAgentsWithPlayer( target, sourceAgents , 0, 1, InjectTypes.PLAYER_TO_ITEM );
				break;
				
			case AGT_TRANSFER_COPY: //transferring liquid
				InjectAgentsWithoutPlayer( target, sourceAgents );
				break;	
			
			case AGT_ITEM_TO_FLESH: //transferring liquid
				InjectAgentsWithPlayer( target, sourceAgents , 0, 1, InjectTypes.ITEM_TO_PLAYER);
				break;

			case AGT_AIRBOURNE_BIOLOGICAL:
				float prot_level_mask_target = GetProtectionLevel(DEF_BIOLOGICAL,InventorySlots.MASK, Man.Cast( target ));
				float prot_level_mask_source = GetProtectionLevel(DEF_BIOLOGICAL,InventorySlots.MASK, Man.Cast( source ));
				float prot_level_headgear_target = GetProtectionLevel(DEF_BIOLOGICAL,InventorySlots.HEADGEAR, Man.Cast( target ));
				float prot_level_headgear_source = GetProtectionLevel(DEF_BIOLOGICAL,InventorySlots.HEADGEAR, Man.Cast( source ));
				float prot_level_target = Math.Max(prot_level_mask_target, prot_level_headgear_target);//find the bigger of the 2, TODO: should be improved
				float prot_level_source = Math.Max(prot_level_mask_source, prot_level_headgear_source);//find the bigger of the 2, TODO: should be improved
				float prot_level_combined = 1 - (1 - prot_level_target) * (1 - prot_level_source);
				InjectAgentsWithPlayer( target, sourceAgents , prot_level_combined, 1, InjectTypes.PLAYER_AIR_PLAYER );
				break;
			case AGT_AIRBOURNE_CHEMICAL:
				float prot_level_mask_target2 = GetProtectionLevel(DEF_CHEMICAL,InventorySlots.MASK, Man.Cast( target ));

				count = InjectAgentWithPlayerDose( target, agents , prot_level_mask_target2, dose_size, InjectTypes.PLAYER_AIR_PLAYER );
				break;
			default:
				break;
		}
		return count;
	}
	
	/**
	 * \brief Process transmission of agents between entities (for specified transmission type) (see TransmitAgentsEx()))
	 */
	void TransmitAgents(EntityAI source, EntityAI target, int pathway, int dose_size = 1000)
	{
		 TransmitAgentsEx(source, target, pathway, dose_size);
	}

	/**
	 * \brief Injects specified agents directly to target
	 * @param target Entity to inject agents to
	 * @param agents Bit mask with information about agents
	 */	
	protected void InjectAgentsWithoutPlayer(EntityAI target, int agents)
	{
		if( target.IsItemBase() )
		{
			ItemBase ib_target = ItemBase.Cast( target );
			ib_target.TransferAgents(agents);
		}
	}

	/**
	 * \brief Injects agents to a given target, using chance of transmission and full dose size if chance succeeds
	 * @param target Entity to inject agents to
	 * @param agents Bit mask with information about agents
	 * @param protection Protection size used for chance generation <0.0; 1.0>
	 * @param dose_size Number of agents to be transmitted
	 * @param inject_type Type of transmission
	 */	
	protected void InjectAgentsWithPlayer(EntityAI target, int agents, float protection, int dose_size, int inject_type)//target,array_of_agents,protection_lvl
	{
		if(target && (agents != 0) && target.IsEntityAI() )
		{
			int bit_count = Math.GetNumberOfSetBits(agents);
			
			for (int i = 0; i < bit_count; i++)
			{
				int agent_bit = Math.Pow(2,Math.GetNthBitSet(agents,i));
				if( DetermineChanceToTransmit(agent_bit, protection, inject_type))
				{
					target.InsertAgent(agent_bit,dose_size);
				}
			}
		}
	}	

	/**
	 * \brief Injects agents to a given target, with no probability, but the dose size is modified by m_TransferabilityOut of the agent
	 * @param target Entity to inject agents to
	 * @param agents Bit mask with information about agents
	 * @param protection Protection size used for chance generation <0.0; 1.0>
	 * @param dose_size Number of agents to be transmitted
	 * @param inject_type Type of transmission
	 */	
	protected void InjectAgentsWithPlayerCount(EntityAI target, int agents, float protection, int dose_size, int inject_type)//target,array_of_agents,protection_lvl
	{
		if(target && (agents != 0) && target.IsEntityAI() )
		{
			int bit_count = Math.GetNumberOfSetBits(agents);
			
			for (int i = 0; i < bit_count; i++)
			{
				int agent_bit = Math.Pow(2,Math.GetNthBitSet(agents,i));
				float count = CalculateAgentsToTransmit(agent_bit, protection, dose_size, inject_type);
				target.InsertAgent(agent_bit,count);
			}
		}
	}

	/**
	 * \brief Injects agent to a given target
	 * @param target Entity to inject agents to
	 * @param agents Bit mask with information about agents
	 * @param protection Protection size used for chance generation <0.0; 1.0>
	 * @param dose_size Number of agents to be transmitted
	 * @param inject_type Type of transmission
	 * \return count Number of injected agents (dose_size might be shrinked by chance from protection)
	 */	
	protected float InjectAgentWithPlayerDose(EntityAI target, int agent, float protection, float dose_size, int inject_type)//target,array_of_agents,protection_lvl
	{
		float count = CalculateAgentsToTransmit(agent, protection, dose_size, inject_type);
		{
			if(count > 0)
			{
				target.InsertAgent(agent, count);
				return count;
			}
		}
		return 0;
	}
	
	// !performance hog, avoid
	static void BuildAgentArray(int agents, array<int> agents_out)
	{
		int mask = 1;
		for(int i = 0; i < BIT_INT_SIZE; i++)
		{
			if( mask & agents ) 
				agents_out.Insert(mask);
			mask = mask * 2;
		}
	}

	/**
	 * \brief Protection level of an attachment against enviromental hazard (mask/filters for example)
	 * @param type Type of protection (see DEF_BIOLOGICAL in constants.c for example)
	 * @param slot Inventory slot id
	 * @param player Player reference
	 * @param consider_filter (unused parameter for now)
	 * @param system (unused parameter for now)
	 * \return Attachment protechtion level
	 */
	static float GetProtectionLevelEx(int type, int slot, Man player, bool consider_filter = true, int system = 0)
	{
		ItemBase attachment = ItemBase.Cast(player.GetInventory().FindAttachment(slot));
		
		if(!attachment) 
			return 0;
		
		return attachment.GetProtectionLevel(type, consider_filter, system);
		
	}

	/**
	 * \brief Protection level of an attachment against enviromental hazard (mask/filters for example) (see GetProtectionLevelEx())
	 */	
	protected float GetProtectionLevel(int type, int slot, Man player)
	{
		return GetProtectionLevelEx(type, slot, player);
	}

	//------------------------------------------------------------------------------------------------------

	/**
	 * \brief Calculates number of agents that can be transmitted (based on given dose_size)
	 * @param agent_id Id of agent (see eAgents enum)
	 * @param protection Protection size used for chance generation <0.0; 1.0>
	 * @param dose_size Number of agents to be transmitted
	 * @param inject_type Type of transmission
	 * \return Number of agents that will be transmitted based on the processing of attributes
	 */	
	protected float CalculateAgentsToTransmit(int agent_id, float protection, int dose_size, int inject_type)
	{

		//Debug.Log("protection: "+protection.ToString());
		//reverse the value (in config, the higher the value, the higher the protection: 0 - 1) so that we can easily interpolate between 0 and 1 by multiplication
		float prot = 1 - protection;
		//Debug.Log("prot: "+prot.ToString(), "Agents");
		float transf;
		
		if( inject_type == InjectTypes.PLAYER_TO_ITEM )
		{
			transf = GetAgentTransferabilityOut(agent_id);
		}
		else if( inject_type == InjectTypes.ITEM_TO_PLAYER )
		{
			transf = GetAgentTransferabilityIn(agent_id);
		}
		else if( inject_type == InjectTypes.PLAYER_AIR_PLAYER )
		{
			transf = GetAgentTransferabilityAirOut(agent_id);
		}

		//Debug.Log("transf: "+transf.ToString(), "Agents");
		//float result =  GetAgentInitialCount(agent_id) * prot * transf * dose_size;//final formula
		float result =  1 * prot * transf * dose_size;//final formula
		//result = Math.Ceil(result);
		//Debug.Log("result: "+result.ToString(), "Agents");
		return result;
	}
	
	//------------------------------------------------------------------------------------------------------
	/**
	 * \brief Agent transmission chance processing
	 * @param agent_id Id of agent (see eAgents enum)
	 * @param protection Protection size used for chance generation <0.0; 1.0>
	 * @param inject_type Type of transmission
	 * \return true if there is a chance to transmit agent
	 */
	protected bool DetermineChanceToTransmit(int agent_id, float protection, int inject_type)
	{

		//Debug.Log("protection: "+protection.ToString());
		//reverse the value (in config, the higher the value, the higher the protection: 0 - 1) so that we can easily interpolate between 0 and 1 by multiplication
		float prot = 1 - protection;
		//Debug.Log("prot: "+prot.ToString(), "Agents");
		float transf;
		
		if( inject_type == InjectTypes.PLAYER_TO_ITEM )
		{
			transf = GetAgentTransferabilityOut(agent_id);
		}
		else if( inject_type == InjectTypes.ITEM_TO_PLAYER )
		{
			transf = GetAgentTransferabilityIn(agent_id);
		}
		else if( inject_type == InjectTypes.PLAYER_AIR_PLAYER )
		{
			transf = GetAgentTransferabilityAirOut(agent_id);
		}
		#ifdef DEVELOPER
		//Debug.Log("transf: "+transf.ToString(), "Agents");
		#endif
		//float result =  GetAgentInitialCount(agent_id) * prot * transf * dose_size;//final formula
		bool dice = Math.RandomFloat01() < (prot * transf);
		//result = Math.Ceil(result);

		return dice;
	}
	//------------------------------------------------------------------------------------------------------


	//! DEPRECATED
	bool GrowDuringAntibioticsAttack(int agent_id, PlayerBase player)
	{
		if (!m_AgentList.Get(agent_id))
			return true;

		return m_AgentList.Get(agent_id).GrowDuringMedicalDrugsAttack(EMedicalDrugsType.ANTIBIOTICS, player);
	}
}
