class WeldingMask extends ClothingBase
{
	override array<int> GetEffectWidgetTypes()
	{
		return {EffectWidgetsTypes.HELMET_OCCLUDER/*,EffectWidgetsTypes.HELMET_BREATH*/};
	}
	
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing eyewear = Clothing.Cast(parent.FindAttachmentBySlotName("Eyewear"));
		if ( eyewear && eyewear.ConfigGetBool("isStrap") )
		{
			return false;
		}
		
		Clothing mask = Clothing.Cast(parent.FindAttachmentBySlotName("Mask"));
		if ( mask && mask.ConfigGetBool("noHelmet") && !SantasBeard.Cast(mask) )
		{
			return false;
		}
		
		return true;
	}
	
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESWELDING;
	}
}