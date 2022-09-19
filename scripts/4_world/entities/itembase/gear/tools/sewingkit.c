class SewingKit: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionRepairTent);
		AddAction(ActionRepairShelter);
		AddAction(ActionSewTarget);
		AddAction(ActionSewSelf);
	}

	override float GetInfectionChance(int system = 0, Param param = null)
	{
		if(m_Cleanness == 1)
		{
			return 0;
		}
		else
		{
			return 0.3;
		}
		
	}
	
	override bool CanBeDisinfected()
	{
		return true;
	}
	
	override void OnCombine(ItemBase other_item)
	{
		if(m_Cleanness == 1 && other_item.m_Cleanness == 0)
			SetCleanness(0);
	}
};
