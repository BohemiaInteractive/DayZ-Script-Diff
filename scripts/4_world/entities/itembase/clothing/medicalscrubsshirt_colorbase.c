class MedicalScrubsShirt_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class MedicalScrubsShirt_Blue extends MedicalScrubsShirt_ColorBase {};
class MedicalScrubsShirt_Green extends MedicalScrubsShirt_ColorBase {};
class MedicalScrubsShirt_White extends MedicalScrubsShirt_ColorBase {};