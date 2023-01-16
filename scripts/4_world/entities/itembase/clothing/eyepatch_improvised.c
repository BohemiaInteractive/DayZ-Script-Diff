class EyePatch_Improvised extends Clothing
{
	override array<int> GetEffectWidgetTypes()
	{
		return {EffectWidgetsTypes.EYEPATCH_OCCLUDER};
	}
	
	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (!super.CanPutAsAttachment(parent))
			return false;
		
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
		AddAction(ActionWringClothes);
	}
};

