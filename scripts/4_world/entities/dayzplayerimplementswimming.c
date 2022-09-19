
class DayZPlayerImplementSwimming
{

	DayZPlayer 					m_pPlayer;
	SHumanCommandSwimSettings	m_pSettings;
	bool						m_bWasSwimming = false; // important for shallow water, so we know if we should get back into swimming when other fullbody command (like damage) finished

	void DayZPlayerImplementSwimming(DayZPlayer pPlayer)
	{
		m_pPlayer 	= pPlayer;
		m_pSettings	= pPlayer.GetDayZPlayerType().CommandSwimSettingsW();
	}

	float  GetWaterDepth()
	{
		vector 	pp = m_pPlayer.GetPosition();
		vector  wl = HumanCommandSwim.WaterLevelCheck(m_pPlayer, pp);

		return wl[0];
	}

	float  GetCharacterDepth()
	{
		vector 	pp = m_pPlayer.GetPosition();
		vector  wl = HumanCommandSwim.WaterLevelCheck(m_pPlayer, pp);

		return wl[1];
	}

	bool CheckSwimmingStart(out vector waterLevel)
	{
		return DayZPlayerUtils.CheckWaterLevel(m_pPlayer,waterLevel) == EWaterLevels.LEVEL_SWIM_START;
	}

	//! ha
	bool HandleSwimming(int pCurrentCommandID, HumanCommandMove pCMove, HumanMovementState pState)
	{	
		if ( pCurrentCommandID == DayZPlayerConstants.COMMANDID_UNCONSCIOUS || pCurrentCommandID == DayZPlayerConstants.COMMANDID_DAMAGE )
			return false;
		
		m_bWasSwimming = false;

		//! get water level 
		if (pCurrentCommandID != DayZPlayerConstants.COMMANDID_SWIM)
		{
			vector wl;
			if ( CheckSwimmingStart(wl) )
			{
				m_pPlayer.StartCommand_Swim();
				m_bWasSwimming = true;
				return true;
			}
			
			//! now movement
			if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_MOVE)
			{
				pCMove.SetCurrentWaterLevel(wl[1]);
				
				if (wl[1] > m_pSettings.m_fToErectLevel && pState.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_ERECT && pState.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_RAISEDERECT )
				{
					if ( DayZPlayerUtils.PlayerCanChangeStance(m_pPlayer, DayZPlayerConstants.STANCEIDX_ERECT) )
					{
						pCMove.ForceStanceUp(DayZPlayerConstants.STANCEIDX_ERECT);
					}
					else
					{
						m_pPlayer.StartCommand_Swim();
						m_bWasSwimming = true;
						return true;
					}
				}
				else if (wl[1] > m_pSettings.m_fToCrouchLevel && (pState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE || pState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE))
				{
					if ( DayZPlayerUtils.PlayerCanChangeStance(m_pPlayer, DayZPlayerConstants.STANCEIDX_CROUCH) )
					{
						pCMove.ForceStanceUp(DayZPlayerConstants.STANCEIDX_CROUCH);
					}
					else
					{
						m_pPlayer.StartCommand_Swim();
						m_bWasSwimming = true;
						return true;
					}
				}
			}

			// not handled 
			return false;
		}
		else // if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_SWIM)
		{
			if (GetWaterDepth() < m_pSettings.m_fWaterLevelOut)
			{
				if (DayZPlayerUtils.PlayerCanChangeStance(m_pPlayer, DayZPlayerConstants.STANCEIDX_ERECT, true))
				{
					HumanCommandSwim hcs = m_pPlayer.GetCommand_Swim();
					hcs.StopSwimming();
					m_bWasSwimming = false;
					return true;
				}
			}
		
			//! handled !
			m_bWasSwimming = true;
			return true;
		}
	}
}