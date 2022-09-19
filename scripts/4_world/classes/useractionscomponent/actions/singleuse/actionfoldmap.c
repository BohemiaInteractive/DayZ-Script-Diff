
//dummy action. Unfold 'looped action' canceled from elswhere
class ActionFoldMap: ActionBase
{
	void ActionFoldMap()
	{
		m_Text = "#fold_map";
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (player.m_hac && player.IsMapOpen())
		{
			return true;
		}
		return false;
	}
	
	override int GetActionCategory()
	{
		return AC_SINGLE_USE;
	}
};