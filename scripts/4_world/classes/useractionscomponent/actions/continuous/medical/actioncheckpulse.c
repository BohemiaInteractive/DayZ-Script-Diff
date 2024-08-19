class ActionCheckPulse: ActionInteractBase
{
	const int TARGET_IRREGULAR_PULSE_BIT = 1 << 31;
	void ActionCheckPulse()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#check_pulse_target";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINone;
	}

	static string GetPulseMessage(EPulseType pulse_type, int blood_level)
	{
		//builds string table keys:
		
		//pulse_strong
		//pulse_decent
		//pulse_moderate
		//pulse_weak
		//pulse_faint
		//pulse_strong_irregular
		//pulse_decent_irregular
		//pulse_moderate_irregular
		//pulse_weak_irregular
		//pulse_faint_irregular
		
		string blood_msg = "strong";
		string pulse_msg = "";
		
		if(pulse_type == EPulseType.IRREGULAR)
		{
			pulse_msg = "_irregular";
		}
		if( blood_level == EStatLevels.HIGH )
		{
			blood_msg = "decent";
		}
		if( blood_level == EStatLevels.MEDIUM )
		{
			blood_msg = "moderate";
		}
		if( blood_level == EStatLevels.LOW )
		{
			blood_msg = "weak";
		}
		if( blood_level == EStatLevels.CRITICAL )
		{
			blood_msg = "faint";
		}
		//string message = blood_msg + " " + pulse_msg + " pulse";
		string message = "#"+ "pulse_" + blood_msg + pulse_msg;
		return message;
		
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		PlayerBase ntarget = PlayerBase.Cast(  target.GetObject() );
		if( ntarget && ntarget.IsAlive())
		{
			return true;
		}
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		PlayerBase target_player = PlayerBase.Cast(action_data.m_Target.GetObject());
		if (target_player)
		{
			PlayerBase player = action_data.m_Player;
			CachedObjectsParams.PARAM1_INT.param1 = target_player.GetStatLevelBlood();
			bool pulse_type = target_player.GetPulseType();
			if (pulse_type == EPulseType.IRREGULAR)
				CachedObjectsParams.PARAM1_INT.param1 = CachedObjectsParams.PARAM1_INT.param1 | (1 << TARGET_IRREGULAR_PULSE_BIT);

			GetGame().RPCSingleParam( player ,ERPCs.RPC_CHECK_PULSE, CachedObjectsParams.PARAM1_INT, true, player.GetIdentity() );
		}
	}
	
	
	override void OnExecuteClient( ActionData action_data )
	{
		PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );

		if(ntarget)
		{
			action_data.m_Player.m_CheckPulseLastTarget = ntarget;
		}
	}
	
	override void OnEndServer( ActionData action_data )
	{
		/*
		PlayerBase target_player = PlayerBase.Cast(action_data.m_Target.GetObject());
		PlayerBase player = action_data.m_Player;
		CachedObjectsParams.PARAM1_INT.param1 = target_player.GetStatLevelBlood();
		GetGame().RPCSingleParam( target_player ,ERPCs.RPC_CHECK_PULSE, CachedObjectsParams.PARAM1_INT, true, target_player.GetIdentity() );
		*/
	}
}