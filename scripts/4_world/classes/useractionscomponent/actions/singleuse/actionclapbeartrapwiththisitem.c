class ActionClapBearTrapWithThisItem: ActionSingleUseBase
{
	void ActionClapBearTrapWithThisItem()
	{
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONFB_POKE;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody		= true;
		m_Text			= "#trigger_bear_trap";
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem		= new CCINonRuined;
		m_ConditionTarget	= new CCTCursor;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
		{
			return false;
		}

		BearTrap trap;
		if (Class.CastTo(trap, target.GetObject()))
		{		
			return trap.IsActive() && trap.CanBeDisarmed() && IsInReach(player, target, UAMaxDistances.DEFAULT);
		}

		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		BearTrap trap;
		Class.CastTo(trap, action_data.m_Target.GetObject());
		
		trap.Disarm();
	}
}
