class BomberJacket_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class BomberJacket_Black extends BomberJacket_ColorBase {};
class BomberJacket_Brown extends BomberJacket_ColorBase {};
class BomberJacket_Blue extends BomberJacket_ColorBase {};
class BomberJacket_Grey extends BomberJacket_ColorBase {};
class BomberJacket_Maroon extends BomberJacket_ColorBase {};
class BomberJacket_Olive extends BomberJacket_ColorBase {};
class BomberJacket_SkyBlue extends BomberJacket_ColorBase {};