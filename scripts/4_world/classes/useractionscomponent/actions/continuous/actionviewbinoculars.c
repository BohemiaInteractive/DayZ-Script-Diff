class ActionViewBinoculars : ActionViewOptics
{
	bool m_workingRangefinder;

	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Rangefinder rangefinder = Rangefinder.Cast(item);
		if ( rangefinder )
		{
			m_Text = "#use_range_finder";
		}
		else
		{
			m_Text = "#Look_Through";
		}
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemOptics optic;
		if( Class.CastTo(optic, item) && !optic.IsInOptics() && !player.IsNVGLowered() )
		{
			return true;
		}
		return false;
	}
	
	override void EnterOptics(ItemOptics optic, PlayerBase player)
	{
		player.SetHandheldOpticsInUse(true);
		player.SetOptics(true);
		optic.EnterOptics();
		optic.HideSelection("hide");
		if ( NVGoggles.Cast(optic) && optic.ConfigIsExisting("simpleHiddenSelections") ) //HACK
		{
			optic.SetSimpleHiddenSelectionState(0,false);
		}
		
		if ( optic.HasEnergyManager() )
		{
			PoweredOptic_Base rf = PoweredOptic_Base.Cast(optic);
			if (!rf)
				return;
			
			rf.SetPlayer( player );
			if ( GetGame().IsServer() )
				rf.GetCompEM().SwitchOn();
		}
	}
	
	override void ExitOptics(ItemOptics optic, PlayerBase player)
	{
		optic.ShowSelection("hide");
		if ( NVGoggles.Cast(optic) && optic.ConfigIsExisting("simpleHiddenSelections") ) //HACK
		{
			optic.SetSimpleHiddenSelectionState(0,true);
		}
		optic.ExitOptics();
		player.SetHandheldOpticsInUse(false);
		player.ExitSights();
		if ( optic.HasEnergyManager() )
		{
			PoweredOptic_Base rf = PoweredOptic_Base.Cast(optic);
			if (!rf)
				return;
			
			rf.SetPlayer( null );
			if ( GetGame().IsServer() )
				rf.GetCompEM().SwitchOff();
		}
	}
}