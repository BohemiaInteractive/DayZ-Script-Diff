class GorkaEJacket_ColorBase extends Clothing 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class GorkaEJacket_Summer extends GorkaEJacket_ColorBase {};
class GorkaEJacket_Flat extends GorkaEJacket_ColorBase {};
class GorkaEJacket_Autumn extends GorkaEJacket_ColorBase {};
class GorkaEJacket_PautRev extends GorkaEJacket_ColorBase {};