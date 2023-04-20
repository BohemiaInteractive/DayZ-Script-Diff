class MedicalScrubsHat_ColorBase extends HeadGear_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class MedicalScrubsHat_Blue extends MedicalScrubsHat_ColorBase {};
class MedicalScrubsHat_White extends MedicalScrubsHat_ColorBase {};
class MedicalScrubsHat_Green extends MedicalScrubsHat_ColorBase {};