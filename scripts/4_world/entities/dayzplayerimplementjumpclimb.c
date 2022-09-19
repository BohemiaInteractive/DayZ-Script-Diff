class DayZPlayerImplementJumpClimb
{
	void DayZPlayerImplementJumpClimb(DayZPlayerImplement pPlayer)
	{
		m_Player = pPlayer;
	}
	
	void JumpOrClimb()
	{
		SHumanCommandClimbSettings hcls = m_Player.GetDayZPlayerType().CommandClimbSettingsW();
		
		if ( m_Player.m_MovementState.m_iMovement != DayZPlayerConstants.MOVEMENTIDX_IDLE )
			hcls.m_fFwMaxDistance = 2.5;
		else
			hcls.m_fFwMaxDistance = 1.2;
		
		SHumanCommandClimbResult climbRes = new SHumanCommandClimbResult();
		
		HumanCommandClimb.DoClimbTest(m_Player, climbRes, 0);
		if ( climbRes.m_bIsClimb || climbRes.m_bIsClimbOver )
		{
			int climbType = GetClimbType(climbRes.m_fClimbHeight);

			if ( !m_Player.CanClimb( climbType,climbRes ) )
				return;

			if ( Climb(climbRes) )
			{
				if ( climbType == 1 )
					m_Player.DepleteStamina(EStaminaModifiers.VAULT);
				else if ( climbType == 2 )
					m_Player.DepleteStamina(EStaminaModifiers.CLIMB);

				return;
			}
		}
		
		if ( !m_Player.CanJump() )
			return;
		
		Jump();
		m_Player.DepleteStamina(EStaminaModifiers.JUMP);
	}
	
	void CheckAndFinishJump(int pLandType = 0)
	{
		if ( m_bIsJumpInProgress )
		{
			m_bIsJumpInProgress = false;
			m_Player.OnJumpEnd(pLandType);
		}
	}
	
	private bool Climb(SHumanCommandClimbResult pClimbRes)
	{
		int climbType = GetClimbType(pClimbRes.m_fClimbHeight);	
		if( climbType != -1 )
		{
			m_Player.StartCommand_Climb(pClimbRes, climbType);
			m_Player.StopHandEvent();
		}
		
		return climbType != -1;
	}
	
	private void Jump()
	{
		m_bIsJumpInProgress = true;
		m_Player.SetFallYDiff(m_Player.GetPosition()[1]);

		m_Player.OnJumpStart();
		m_Player.StartCommand_Fall(2.6);
		m_Player.StopHandEvent();
	}
	
	private int GetClimbType(float pHeight)
	{
		int climbType = -1;
        if( pHeight < 1.1 )
            climbType = 0;
        else if( pHeight >= 1.1 && pHeight < 1.7 )
            climbType = 1;
        else if( pHeight >= 1.7 && pHeight < 2.75 )
            climbType = 2;    
        
        return climbType;
	}
		
	// Public variable members
	bool m_bIsJumpInProgress = false;	
	
	// Private variable members
	private DayZPlayerImplement m_Player;
}
