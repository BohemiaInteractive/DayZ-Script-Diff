class NVGHeadstrap extends Clothing
{
	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (!super.CanPutAsAttachment(parent))
			return false;
		
		Clothing helmet = Clothing.Cast(parent.FindAttachmentBySlotName( "Headgear" ));
		if (helmet && helmet.ConfigGetBool("noNVStrap"))
		{
			return false;
		}
		
		Clothing mask = Clothing.Cast(parent.FindAttachmentBySlotName("Mask"));
		if (mask && mask.ConfigGetBool("noEyewear"))
		{
			return false;
		}
		
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionToggleNVG);
	}
};