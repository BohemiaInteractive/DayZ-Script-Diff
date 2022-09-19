class ActionCloseBarrelHoles: ActionInteractBase
{
	void ActionCloseBarrelHoles()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#close";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_object = target.GetObject();
		if ( target_object.IsItemBase() )
		{
			BarrelHoles_ColorBase ntarget = BarrelHoles_ColorBase.Cast( target_object );
			if( ntarget )
			{			
				if ( ntarget.IsOpen() )
				{
					return true;
				}
			}
		}
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		Object target_object = action_data.m_Target.GetObject();
		BarrelHoles_ColorBase ntarget = BarrelHoles_ColorBase.Cast( target_object );
		
		if( ntarget )
		{
			ntarget.Close();
		}
	}
	
	override void OnEndServer( ActionData action_data )
	{
		Object target_object = action_data.m_Target.GetObject();
		BarrelHoles_ColorBase ntarget = BarrelHoles_ColorBase.Cast( target_object );
		if( ntarget )
		{
			ntarget.SoundSynchRemoteReset();
		}
	}
}