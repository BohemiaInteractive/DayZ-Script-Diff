enum eAnimFinishType
{
	SUCCESS,
	FAILURE,
}

class SmptAnimMetaBase
{
	bool	m_IsPlaying;
	SymptomManager m_Manager;
	PlayerBase m_Player;
	int m_AnimID;
	int m_SymptomType;
	bool m_DestroyRequested;
	bool m_Canceled;
	
	void SmptAnimMetaBase()
	{
	}
	
	void Init(ParamsReadContext ctx, SymptomManager manager, PlayerBase player)
	{
		m_Manager = manager;
		m_Player = player;
	}
	
	bool IsPlaying()
	{
		return m_IsPlaying;
	}
	
	bool IsDestroyReqested()
	{
		return m_DestroyRequested;
	}
	
	void AnimFinished(eAnimFinishType type)
	{
		m_DestroyRequested = true;
		SymptomBase Symptom = m_Manager.GetCurrentPrimaryActiveSymptom();
		
		if( type == eAnimFinishType.FAILURE)//   <--------------- FAILED
		{
			if( m_Player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER || !GetGame().IsMultiplayer())
			{
				if( Symptom ) 
				{
					Symptom.AnimationPlayFailed();
				}
			}
		}
		else if( type == eAnimFinishType.SUCCESS)//   <--------------- SUCCESS
		{
			if( m_Player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER || !GetGame().IsMultiplayer())
			{
				if( Symptom )
				{
					Symptom.AnimationFinish();
				}
			}
		}
	}
	
	EAnimPlayState PlayRequest()
	{
		EAnimPlayState played = Play();
		
		if(played == EAnimPlayState.FAILED)
		{
			m_Manager.OnAnimationFinished(eAnimFinishType.FAILURE);
		}
		return played;
	}
	
	protected EAnimPlayState Play();
	
	void Update(HumanMovementState movement_state);

};

class SmptAnimMetaFB extends SmptAnimMetaBase
{
	int m_StanceMask;
	float m_Duration;
	
	void ~SmptAnimMetaFB()
	{
		if(m_Player)
			m_Player.m_isFBsymptomPlaying = false;
	}
	
	override void Init(ParamsReadContext ctx, SymptomManager manager, PlayerBase player)
	{
		super.Init(ctx, manager, player);
		DayZPlayerSyncJunctures.ReadPlayerSymptomFBParams( ctx,  m_AnimID, m_StanceMask, m_Duration);
	}
	
	override EAnimPlayState Play()
	{
		m_Canceled = false;
		
		HumanCommandActionCallback callback = m_Player.GetCommand_Action();
		
		if (!callback)
		{
			callback = m_Player.GetCommandModifier_Action();
		}
		if( callback )
		{
			callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
			m_Canceled = true;
		}
		
		if(!m_Canceled)
		{
			SymptomCB anim_callback = SymptomCB.Cast(m_Player.StartCommand_Action(m_AnimID, SymptomCB, m_StanceMask));
			
			if(anim_callback)
			{
				anim_callback.Init(m_Duration, m_Player);
				m_IsPlaying = true;
				m_Player.m_isFBsymptomPlaying = true;
				return EAnimPlayState.OK;
			}
			else
			{
				return EAnimPlayState.FAILED;
			}
		}
		return EAnimPlayState.POSTPONED;
	}
}

class SmptAnimMetaADD extends SmptAnimMetaBase
{
	bool m_CalledAnimStart;
	override void Init(ParamsReadContext ctx, SymptomManager manager, PlayerBase player)
	{
		super.Init(ctx, manager, player);
		DayZPlayerSyncJunctures.ReadPlayerSymptomADDParams( ctx, m_AnimID);
	}
	
	override EAnimPlayState Play()
	{
		HumanCommandActionCallback callback = m_Player.GetCommand_Action();
		if (!callback)
		{
			callback = m_Player.GetCommandModifier_Action();
		}
		if( !callback )
		{
			HumanCommandWeapons hcw = m_Player.GetCommandModifier_Weapons();
			if (hcw && !hcw.IsActionFinished())
				return EAnimPlayState.FAILED;
			
			HumanCommandAdditives ad = m_Player.GetCommandModifier_Additives();
			if(ad)
			{
				//Print("------------------ Playing -----------------");
				ad.StartModifier(m_AnimID);
				m_IsPlaying = true;

				return EAnimPlayState.OK;
			}
		}
		return EAnimPlayState.FAILED;
	}
	
	override void Update(HumanMovementState movement_state)
	{
		if( m_IsPlaying )
		{
			HumanCommandAdditives ad = m_Player.GetCommandModifier_Additives();
			if(!ad || !ad.IsModifierActive())
			{
				//Print("------------------ Not Playing -----------------");
				m_Manager.OnAnimationFinished();
			}
			else
			{
				if(!m_CalledAnimStart)
				{
					m_Manager.OnAnimationStarted();
					m_CalledAnimStart = true;
				}
			}
		}
	}
}


class HeatComfortmMetaADD extends SmptAnimMetaADD
{
	override void Update(HumanMovementState movement_state)
	{
		//if( movement_state.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_WALK || movement_state.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_RUN || movement_state.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT)
		if( movement_state.m_iMovement != DayZPlayerConstants.MOVEMENTIDX_IDLE || movement_state.IsInProne())
		{
			HumanCommandAdditives ad = m_Player.GetCommandModifier_Additives();
			if(ad && ad.IsModifierActive())
			{
				ad.CancelModifier();
			}
		}
		
		super.Update(movement_state);
	}
}