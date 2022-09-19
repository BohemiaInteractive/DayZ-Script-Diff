class MedicalScrubsPants_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class MedicalScrubsPants_Blue extends MedicalScrubsPants_ColorBase {};
class MedicalScrubsPants_Green extends MedicalScrubsPants_ColorBase {};
class MedicalScrubsPants_White extends MedicalScrubsPants_ColorBase {};