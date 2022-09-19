class AITargetCallbacksPlayer : AITargetCallbacks
{
	void AITargetCallbacksPlayer(PlayerBase pPlayer)
	{
		m_Player = pPlayer;
		
		m_iHeadBoneIndex = m_Player.GetBoneIndexByName("Head");
		m_iChestBoneIndex = m_Player.GetBoneIndexByName("Spine3");
	}
	
	override vector GetHeadPositionWS() 
	{ 
		if( m_iHeadBoneIndex != -1 )
			return m_Player.GetBonePositionWS(m_iHeadBoneIndex);
		else
			return m_Player.GetPosition() + "0 1.7 0";
	}
	
	override vector GetVisionPointPositionWS(EntityAI pApplicant) 
	{ 
		DayZInfected infected = DayZInfected.Cast(pApplicant);
		if( infected )
		{
			DayZInfectedInputController ic = infected.GetInputController();
			if( ic )
			{
				int mindState = ic.GetMindState();
				if( mindState >= DayZInfectedConstants.MINDSTATE_ALERTED )
				{
					return GetHeadPositionWS();
				}
				else
				{
					if( m_iChestBoneIndex != -1 )
						return m_Player.GetBonePositionWS(m_iChestBoneIndex);
				}				
			}
		}
		
		return m_Player.GetPosition() + "0 1 0";
	}
	
	override float GetMaxVisionRangeModifier(EntityAI pApplicant)
	{ 
		HumanMovementState state = new HumanMovementState;
		m_Player.GetMovementState(state);
		
		float mod = 1.0;
		float speedCoef = PlayerConstants.AI_VISIBILITY_RUN;
		float stanceCoef = PlayerConstants.AI_VISIBILITY_STANDING;
		float playerVisCoef = 1.0; //Use disabled since 1.12
		
		//! player speed mofifications
		switch(AITargetCallbacksPlayer.StanceToMovementIdxTranslation(state))
		{
			case DayZPlayerConstants.MOVEMENTIDX_WALK:
				speedCoef = PlayerConstants.AI_VISIBILITY_WALK;
				break;
				
			case DayZPlayerConstants.MOVEMENTIDX_IDLE:
				speedCoef = PlayerConstants.AI_VISIBILITY_IDLE;
				break;
		}
		
		//! stance modification
		switch(state.m_iStanceIdx)
		{
			case DayZPlayerConstants.STANCEIDX_CROUCH:
			case DayZPlayerConstants.STANCEIDX_RAISEDCROUCH:
				stanceCoef = PlayerConstants.AI_VISIBILITY_CROUCH;
				break;
				
			case DayZPlayerConstants.STANCEIDX_PRONE:
			case DayZPlayerConstants.STANCEIDX_RAISEDPRONE:
				stanceCoef = PlayerConstants.AI_VISIBILITY_PRONE;
				break;
		}
		
		//! player visibility modification (from cloths)
		//playerVisCoef = m_Player.GetVisibilityCoef();
		
		//! mean value of the coefs
		mod = (speedCoef + stanceCoef /*+ playerVisCoef*/) / 2;
		
		return mod;
	}
	
	//! Translates players speed (idx) and corrects it by current stance
	//! used mainly for visibility/audibility of player to AI
	static int StanceToMovementIdxTranslation(HumanMovementState pState)
	{
		int movementSpeed = 0;

		switch (pState.m_iStanceIdx)
		{
		case DayZPlayerConstants.STANCEIDX_CROUCH:
			switch (pState.m_iMovement)
			{
			case DayZPlayerConstants.MOVEMENTIDX_IDLE:
				movementSpeed = DayZPlayerConstants.MOVEMENTIDX_IDLE;
				break;
			case DayZPlayerConstants.MOVEMENTIDX_WALK:
			case DayZPlayerConstants.MOVEMENTIDX_RUN:
				movementSpeed = DayZPlayerConstants.MOVEMENTIDX_WALK;
				break;
			case DayZPlayerConstants.MOVEMENTIDX_SPRINT:
				movementSpeed = DayZPlayerConstants.MOVEMENTIDX_CROUCH_RUN;
				break;
			}
			break;
		case DayZPlayerConstants.STANCEIDX_PRONE:
			switch (pState.m_iMovement)
			{
			case DayZPlayerConstants.MOVEMENTIDX_IDLE:
				movementSpeed = DayZPlayerConstants.MOVEMENTIDX_IDLE;
				break;
			default:
				movementSpeed = DayZPlayerConstants.MOVEMENTIDX_WALK;
				break;
			}
			break;
		default:
			movementSpeed = pState.m_iMovement;
		}
		
		//Print("MovementState: " + movementSpeed);		
		
		return movementSpeed;
	}
	
	private PlayerBase m_Player;
	private int m_iHeadBoneIndex;
	private int m_iChestBoneIndex;
};