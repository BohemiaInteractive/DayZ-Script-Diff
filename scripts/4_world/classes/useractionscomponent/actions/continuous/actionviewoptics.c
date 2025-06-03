class ActionViewOptics : ActionContinuousBase
{
	void ActionViewOptics()
	{
		m_CallbackClass 	= ActionRaiseAndViewCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_GESTUREFB_LOOKOPTICS;
		m_CommandUIDProne 	= DayZPlayerConstants.CMD_GESTUREFB_LOOKOPTICS;

		m_Text = "#Look_Through";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNone();
	}
	
	override bool IsFullBody(PlayerBase player)
	{
		return true;
	}
	
	override bool IsCameraLockOnPerform()
	{
		return false;
	}
	
	override bool CanBeUsedLeaning()
	{
		return false;
	}
	
	override bool HasProgress()
	{
		return false;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ItemOptics optic;
		if (Class.CastTo(optic, item) && !optic.IsInOptics() && !player.IsNVGLowered())
		{
			return true;
		}
		return false;
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		return true;
	}
	
	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);
		
		GetGame().GetMission().AddActiveInputExcludes({"actonViewOpticExcl"});
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		if (!GetGame().IsMultiplayer())
			GetGame().GetMission().AddActiveInputExcludes({"actonViewOpticExcl"});
	}
	
	override void OnStartAnimationLoopServer(ActionData action_data)
	{
		ItemOptics optic;
		if (Class.CastTo(optic, action_data.m_MainItem))
		{
			if (!optic.IsInOptics())
			{
				EnterOptics(optic, action_data.m_Player);
			}
		}
	}
	
	override void OnStartAnimationLoopClient(ActionData action_data)
	{
		ItemOptics optic;
		if (Class.CastTo(optic, action_data.m_MainItem))
		{
			if (!optic.IsInOptics())
			{
				EnterOptics(optic, action_data.m_Player);
			}
		}
	}
	
	override void OnEndClient(ActionData action_data)
	{
		ItemOptics optic;
		if (Class.CastTo(optic, action_data.m_MainItem) && optic.IsInOptics())
		{
			ExitOptics(optic, action_data.m_Player);
		}
		GetGame().GetMission().RemoveActiveInputExcludes({"actonViewOpticExcl"});
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		ItemOptics optic;
		if (Class.CastTo(optic, action_data.m_MainItem) && optic.IsInOptics())
		{
			ExitOptics(optic, action_data.m_Player);
		}
		
		if (!GetGame().IsMultiplayer())
			GetGame().GetMission().RemoveActiveInputExcludes({"actonViewOpticExcl"});
	}
	
	override void OnEndAnimationLoopClient(ActionData action_data)
	{
		ItemOptics optic;
		if (Class.CastTo(optic, action_data.m_MainItem))
		{
			ExitOptics(optic, action_data.m_Player);
		}
	}
	
	override void OnEndAnimationLoopServer(ActionData action_data)
	{
		ItemOptics optic;
		if (Class.CastTo(optic, action_data.m_MainItem))
		{
			ExitOptics(optic, action_data.m_Player);
		}
	}
	
	bool CanWork(ItemBase item)
	{
		if (!item.HasEnergyManager())
		{
			return true;
		}
		else if (item.GetCompEM() && item.GetCompEM().CanWork())
		{
			return true;
		}
		
		return false;
	}
	
	void EnterOptics(ItemOptics optic, PlayerBase player)
	{
		player.SetIronsights(false);
		player.SetHandheldOpticsInUse(true);
		player.SetOptics(true);

		optic.EnterOptics();
		optic.HideSelection("hide");

		if (optic.HasEnergyManager())
			optic.GetCompEM().SwitchOn();

		player.GetAimingModel().SetAimNoiseAllowed(false);
	}
	
	void ExitOptics(ItemOptics optic, PlayerBase player)
	{
		player.ExitSights();
		player.SetHandheldOpticsInUse(false);

		optic.ShowSelection("hide");
		optic.ExitOptics();

		if (optic.HasEnergyManager())
			optic.GetCompEM().SwitchOff();

		player.GetAimingModel().SetAimNoiseAllowed(true);
	}
}
