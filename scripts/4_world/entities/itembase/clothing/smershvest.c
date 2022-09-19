class SmershVest extends Clothing
{
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item,slot_name);
		
		if (SmershBag.Cast(item))
		{
			ShowSelection("Buttpack");
		}
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item,slot_name);
		
		if (SmershBag.Cast(item))
		{
			HideSelection("Buttpack");
		}
	}
	
};