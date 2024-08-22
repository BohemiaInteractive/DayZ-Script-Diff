class ActionUnplugThisByCord : ActionInteractBase
{
	void ActionUnplugThisByCord()
	{
		m_Text = "#unplug_by_cord";
	}

	override bool HasProgress()
	{
		return false;
	}
	
	override bool DisplayTargetInActionText()
	{
		return true;
	}

	override string GetTargetName(PlayerBase player, ActionTarget target)
	{
		ItemBase targetItem = ItemBase.Cast(target.GetObject());
		if (targetItem && targetItem.HasEnergyManager())
		{
			string selection = targetItem.GetActionComponentName(target.GetComponentIndex());

			if (targetItem.GetCompEM() && targetItem.GetCompEM().GetPlugOwner(selection))
				return targetItem.GetCompEM().GetPlugOwner(selection).GetDisplayName();
		}
		
		return super.GetTargetName(player, target);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ItemBase targetItem = ItemBase.Cast(target.GetObject());
		
		if (targetItem && targetItem.HasEnergyManager())
		{
			string selection = targetItem.GetActionComponentName(target.GetComponentIndex());

			if (selection == ComponentEnergyManager.SEL_CORD_PLUGGED)
				return true;
			
			// Special case for unfolded spotlight
			if (selection == Spotlight.SEL_CORD_PLUGGED_U)
				return true;
		}
		
		return false;
	}
	
	override void OnExecuteServer(ActionData action_data)
	{	
		ItemBase targetItem = ItemBase.Cast(action_data.m_Target.GetObject());
		targetItem.GetCompEM().UnplugThis();
		
		if (targetItem.IsInherited(Spotlight))
		{
			targetItem.HideSelection(Spotlight.SEL_CORD_PLUGGED_U);
			targetItem.ShowSelection(Spotlight.SEL_CORD_FOLDED_U);
		}
	}
}
