class SportGlasses_ColorBase extends Clothing 
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing mask = Clothing.Cast(parent.FindAttachmentBySlotName("Mask"));
		if ( mask && mask.ConfigGetBool("noEyewear") ) //TODO
		{
			return false;
		}
		
		return true;
	}
};

class SportGlasses_Black extends SportGlasses_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTBLACK;
	}
};

class SportGlasses_Blue extends SportGlasses_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTBLUE;
	}
};

class SportGlasses_Green extends SportGlasses_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTGREEN;
	}
};

class SportGlasses_Orange extends SportGlasses_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTORANGE;
	}
};