class BeanieHat_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class BeanieHat_Beige extends BeanieHat_ColorBase {};
class BeanieHat_Black extends BeanieHat_ColorBase {};
class BeanieHat_Blue extends BeanieHat_ColorBase {};
class BeanieHat_Brown extends BeanieHat_ColorBase {};
class BeanieHat_Green extends BeanieHat_ColorBase {};
class BeanieHat_Grey extends BeanieHat_ColorBase {};
class BeanieHat_Pink extends BeanieHat_ColorBase {};
class BeanieHat_Red extends BeanieHat_ColorBase {};
