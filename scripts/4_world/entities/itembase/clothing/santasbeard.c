class SantasBeard extends Clothing
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing headgear = Clothing.Cast(parent.FindAttachmentBySlotName("Headgear"));
		if ( headgear && (headgear.ConfigGetBool("noMask") && !PumpkinHelmet.Cast(headgear) && !WeldingMask.Cast(headgear)) )
		{
			return false;
		}
		
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
}