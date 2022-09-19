class DarkMotoHelmet_ColorBase extends HelmetBase
{
	override array<int> GetEffectWidgetTypes()
	{
		return {EffectWidgetsTypes.MOTO_OCCLUDER/*,EffectWidgetsTypes.MOTO_BREATH*/};
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
		if ( mask && mask.ConfigGetBool("noHelmet") ) //TODO
		{
			return false;
		}
		
		return true;
	}
	
	override bool IsObstructingVoice()
	{
		return true;
	}
	
	override int GetVoiceEffect()
	{
		return VoiceEffectObstruction;
	}
	
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_MOTOHELMETBLACK;		
	}
}

class DarkMotoHelmet_Black extends DarkMotoHelmet_ColorBase {};
class DarkMotoHelmet_Grey extends DarkMotoHelmet_ColorBase {};
class DarkMotoHelmet_Green extends DarkMotoHelmet_ColorBase {};
class DarkMotoHelmet_Lime extends DarkMotoHelmet_ColorBase {};
class DarkMotoHelmet_Blue extends DarkMotoHelmet_ColorBase {};
class DarkMotoHelmet_Red extends DarkMotoHelmet_ColorBase {};
class DarkMotoHelmet_White extends DarkMotoHelmet_ColorBase {};
class DarkMotoHelmet_Yellow extends DarkMotoHelmet_ColorBase {};
class DarkMotoHelmet_YellowScarred extends DarkMotoHelmet_ColorBase {};