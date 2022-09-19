class ActionViewOptics : ActionContinuousBase
{
	void ActionViewOptics()
	{
		m_CallbackClass = ActionRaiseAndViewCB;
		m_CommandUID = DayZPlayerConstants.CMD_GESTUREFB_LOOKOPTICS;
		m_CommandUIDProne = DayZPlayerConstants.CMD_GESTUREFB_LOOKOPTICS;
		m_Text = "#Look_Through";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool IsFullBody(PlayerBase player)
	{
		return true;
	}
	
	override bool HasProgress()
	{
		return false;
	}

	override bool HasTarget()
	{
		return false;
	}

	/*override bool HasProneException()
	{
		return true;
	}*/
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemOptics optic;
		if( Class.CastTo(optic, item) && !optic.IsInOptics() && !player.IsNVGLowered() )
		{
			return true;
		}
		return false;
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		/*if ( !CanWork(ItemOptics.Cast(action_data.m_MainItem)) )
		{
			return false;
		}*/
		//if (action_data.m_Callback && action_data.m_Callback.GetState() == HumanCommandActionCallback.STATE_LOOP_END)
			//return false;
		return true;
	}
	
	/*override void OnExecuteClient( ActionData action_data )
	{
		ItemOptics optic;
		if( Class.CastTo(optic, action_data.m_MainItem) )
		{
			if (!optic.IsInOptics())
			{
				EnterOptics(optic, action_data.m_Player);
			}
			else
			{
				//ExitOptics(optic, action_data.m_Player);
			}
		}
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		ItemOptics optic;
		if( Class.CastTo(optic, action_data.m_MainItem) )
		{
			if (!optic.IsInOptics())
			{
				EnterOptics(optic, action_data.m_Player);
			}
			else
			{
				//ExitOptics(optic, action_data.m_Player);
			}
		}
	}*/
	
	override void OnStartAnimationLoopServer( ActionData action_data )
	{
		ItemOptics optic;
		if( Class.CastTo(optic, action_data.m_MainItem) )
		{
			if (!optic.IsInOptics())
			{
				EnterOptics(optic, action_data.m_Player);
			}
		}
	}
	
	override void OnStartAnimationLoopClient( ActionData action_data )
	{
		ItemOptics optic;
		if( Class.CastTo(optic, action_data.m_MainItem) )
		{
			if (!optic.IsInOptics())
			{
				EnterOptics(optic, action_data.m_Player);
			}
		}
	}
	
	override void OnEndClient( ActionData action_data )
	{
		ItemOptics optic;
		if( Class.CastTo(optic, action_data.m_MainItem) && optic.IsInOptics() )
		{
			ExitOptics(optic, action_data.m_Player);
		}
	}
	
	override void OnEndServer( ActionData action_data )
	{
		ItemOptics optic;
		if( Class.CastTo(optic, action_data.m_MainItem) && optic.IsInOptics() )
		{
			ExitOptics(optic, action_data.m_Player);
		}
	}
	
	override void OnEndAnimationLoopClient( ActionData action_data )
	{
		//Print("OnCompleteLoopClient");
		ItemOptics optic;
		if( Class.CastTo(optic, action_data.m_MainItem) )
		{
			ExitOptics(optic, action_data.m_Player);
		}
	}
	
	override void OnEndAnimationLoopServer( ActionData action_data )
	{
		//Print("OnCompleteLoopServer");
		ItemOptics optic;
		if( Class.CastTo(optic, action_data.m_MainItem) )
		{
			ExitOptics(optic, action_data.m_Player);
		}
	}
	
	bool CanWork(ItemBase item)
	{
		//return true;
		// no energy needed, can work
		if ( !item.HasEnergyManager() )
		{
			return true;
		}
		else if ( item.GetCompEM() && item.GetCompEM().CanWork() )
		{
			return true;
		}
		
		return false;
	}
	
	void EnterOptics(ItemOptics optic, PlayerBase player)
	{
		player.SetHandheldOpticsInUse(true);
		player.SetOptics(true);
		optic.EnterOptics();
		optic.HideSelection("hide");
		if (optic.HasEnergyManager()) 	optic.GetCompEM().SwitchOn();
		player.GetAimingModel().SetAimNoiseAllowed(false);
	}
	
	void ExitOptics(ItemOptics optic, PlayerBase player)
	{
		optic.ShowSelection("hide");
		player.ExitSights();
		optic.ExitOptics(); 
		player.SetHandheldOpticsInUse(false);
		if (optic.HasEnergyManager()) 	optic.GetCompEM().SwitchOff();
		player.GetAimingModel().SetAimNoiseAllowed(true);
	}
}