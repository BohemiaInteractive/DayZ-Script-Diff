class PetushokHat_ColorBase extends HeadGear_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class PetushokHat_Yellow extends PetushokHat_ColorBase {};
class PetushokHat_Black extends PetushokHat_ColorBase {};
class PetushokHat_Green extends PetushokHat_ColorBase {};
