class AgentBase extends MessageReceiverBase
{
	float	m_Type = 0;
	float	m_Invasibility;					//how fast the agent grows when potent enough to grow
	float 	m_TransferabilityIn;			//to the player
	float	m_TransferabilityOut;			//from the player
	float	m_Digestibility = 0.1;			//multiplier for agents digested in the player stomach from an infected item(agents_transfered = digested_amount(in grams or mls) * m_Digestibility)
	int		m_MaxCount = 1;
	float 	m_AutoinfectProbability = CalculateAutoinfectProbability(0);	// [0..1], 0 = zero chance, 1 = 100% chance of getting this agent once per hour
	float 	m_TransferabilityAirOut; 		// transferibility airborne out
	float 	m_AntibioticsResistance = 1;	//[0..1], 0 means antibiotics have full effect, 1 means no effect
	
	EStatLevels m_Potency = EStatLevels.MEDIUM;		//grow when player's immune system is at this level or lower
	float m_DieOffSpeed = 1;						//how fast the agent dies off when not potent enough to grow(per sec)
	
	void AgentBase()
	{
		Init();
	}
	
	void Init();
	
	int GetAgentType()
	{
		return m_Type;
	}
	
	float GetDieOffSpeedEx(PlayerBase player)
	{
		return GetDieOffSpeed();
	}
	
	EStatLevels GetPotencyEx(PlayerBase player)
	{
		return GetPotency();
	}
	
	float GetInvasibilityEx(PlayerBase player)
	{
		return GetInvasibility();
	}
	
	EStatLevels GetPotency()
	{
		return m_Potency;
	}	
	
	float GetDieOffSpeed()
	{
		return m_DieOffSpeed;
	}
	
	float GetAntiboticsResistance()
	{
		return m_AntibioticsResistance;
	}
	float GetAntibioticsResistanceEx(PlayerBase player)
	{
		return GetAntiboticsResistance();
	}
	
	float GetInvasibility()
	{
		return m_Invasibility;
	}
	//should this agent grow based on invasibility even during antibiotics attack
	bool GrowDuringAntibioticsAttack(PlayerBase player)
	{
		return true;//for legacy balancing reasons, set to true by default
	}
	
	float GetDigestibility()
	{
		return m_Digestibility;
	}

	float CalculateAutoinfectProbability( float userprob )
	{
		return ( 1 - Math.Pow( 1 - userprob, ( 1 / 1200 ) ) );
	}

	bool AutoinfectCheck(float deltaT, PlayerBase player)
	{
		if ( m_AutoinfectProbability == 0.0 ) 
			return false;
		
		float dice_throw = Math.RandomFloat01();
		
		if ( dice_throw < m_AutoinfectProbability )
		{
			return CanAutoinfectPlayer(player);
		}
		else 
		{
			return false;
		}
	}

	bool CanAutoinfectPlayer(PlayerBase player)
	{
		return false;
	}
	
	float GetTransferabilityIn()
	{
		return m_TransferabilityIn;
	}

	float GetTransferabilityOut()
	{
		return m_TransferabilityOut;
	}	
	
	float GetTransferabilityAirOut()
	{
		return m_TransferabilityAirOut;
	}

	int GetMaxCount()
	{
		return m_MaxCount;
	}

	string GetName()
	{
		return ClassName();
	}
}