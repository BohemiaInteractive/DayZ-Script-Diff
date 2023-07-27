class MotoHelmet_ColorBase extends HelmetBase
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

class MotoHelmet_Black extends MotoHelmet_ColorBase {};
class MotoHelmet_Grey extends MotoHelmet_ColorBase {};
class MotoHelmet_Green extends MotoHelmet_ColorBase {};
class MotoHelmet_Lime extends MotoHelmet_ColorBase {};
class MotoHelmet_Blue extends MotoHelmet_ColorBase {};
class MotoHelmet_Red extends MotoHelmet_ColorBase {};
class MotoHelmet_White extends MotoHelmet_ColorBase {};
class MotoHelmet_Yellow extends MotoHelmet_ColorBase {};