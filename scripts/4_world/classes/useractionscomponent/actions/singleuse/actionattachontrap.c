class ActionAttachOnTrap: ActionAttach
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (target && !target.GetObject().IsInherited(TrapBase))
		{
			return false;
		}
		
		return super.ActionCondition(player, target, item);
	}
}