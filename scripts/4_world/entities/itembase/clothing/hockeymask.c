class HockeyMask extends Clothing
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing headgear = Clothing.Cast(parent.FindAttachmentBySlotName("Headgear"));
		if ( headgear && (headgear.ConfigGetBool("noMask") && !PumpkinHelmet.Cast(headgear)) )
		{
			return false;
		}
		
		Clothing eyewear = Clothing.Cast(parent.FindAttachmentBySlotName("Eyewear"));
		if ( eyewear && SportGlasses_ColorBase.Cast(eyewear) )
		{
			return false;
		}
		
		return true;
	}
}