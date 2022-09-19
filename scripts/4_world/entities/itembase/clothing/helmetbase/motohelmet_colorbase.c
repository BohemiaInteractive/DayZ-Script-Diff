class MotoHelmet_ColorBase extends HelmetBase
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
}

class MotoHelmet_Black extends MotoHelmet_ColorBase {};
class MotoHelmet_Grey extends MotoHelmet_ColorBase {};
class MotoHelmet_Green extends MotoHelmet_ColorBase {};
class MotoHelmet_Lime extends MotoHelmet_ColorBase {};
class MotoHelmet_Blue extends MotoHelmet_ColorBase {};
class MotoHelmet_Red extends MotoHelmet_ColorBase {};
class MotoHelmet_White extends MotoHelmet_ColorBase {};
class MotoHelmet_Yellow extends MotoHelmet_ColorBase {};