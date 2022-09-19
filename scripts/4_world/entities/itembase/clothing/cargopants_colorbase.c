class CargoPants_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class CargoPants_Beige extends CargoPants_ColorBase {};
class CargoPants_Black extends CargoPants_ColorBase {};
class CargoPants_Blue extends CargoPants_ColorBase {};
class CargoPants_Green extends CargoPants_ColorBase {};
class CargoPants_Grey extends CargoPants_ColorBase {};