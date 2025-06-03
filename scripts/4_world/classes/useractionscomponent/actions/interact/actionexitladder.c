class ActionExitLadder: ActionInteractBase
{
	void ActionExitLadder()
	{
		m_Text = "#exit_ladder";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNone();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
	    HumanCommandLadder cl = player.GetCommand_Ladder();
		if (cl && cl.CanExit())
			return true;

		return false;
	}

	override void Start(ActionData action_data)
	{
		super.Start(action_data);

	    HumanCommandLadder cl = action_data.m_Player.GetCommand_Ladder();
		if (cl)
			cl.Exit();
	}

	override bool IsInstant()
	{
		return true;
	}

	override bool CanBeUsedOnLadder()
	{
		return true;
	}
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
}
