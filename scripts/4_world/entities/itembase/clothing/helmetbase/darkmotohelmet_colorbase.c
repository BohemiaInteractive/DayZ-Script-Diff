class DarkMotoHelmet_ColorBase extends HelmetBase
{
	override array<int> GetEffectWidgetTypes()
	{
		return {EffectWidgetsTypes.MOTO_OCCLUDER/*,EffectWidgetsTypes.MOTO_BREATH*/};
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
	
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		if (slotId == InventorySlots.HEADGEAR)
		{
			ret.Insert(EAttExclusions.EXCLUSION_HEADGEAR_HELMET_0);
			
			ret.Insert(EAttExclusions.EXCLUSION_MASK_0);
			ret.Insert(EAttExclusions.EXCLUSION_MASK_1);
			ret.Insert(EAttExclusions.EXCLUSION_MASK_2);
			ret.Insert(EAttExclusions.EXCLUSION_HEADSTRAP_0);
			
			ret.Insert(EAttExclusions.SHAVING_HEADGEAR_ATT_0);
		}
		return ret;
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