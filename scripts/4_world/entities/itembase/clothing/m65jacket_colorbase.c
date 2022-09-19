class M65Jacket_ColorBase extends Clothing 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class M65Jacket_Black extends M65Jacket_ColorBase {};
class M65Jacket_Khaki extends M65Jacket_ColorBase {};
class M65Jacket_Tan extends M65Jacket_ColorBase {};
class M65Jacket_Olive extends M65Jacket_ColorBase {};