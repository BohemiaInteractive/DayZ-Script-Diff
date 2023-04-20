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
	
	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_HEADGEAR_HELMET_0);
		
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_MASK_0);
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_MASK_1);
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_MASK_2);
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_HEADSTRAP_0);
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