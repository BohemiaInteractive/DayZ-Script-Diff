class Shirt_ColorBase extends Clothing 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class Shirt_BlueCheck extends Shirt_ColorBase {};
class Shirt_BlueCheckBright extends Shirt_ColorBase {};
class Shirt_GreenCheck extends Shirt_ColorBase {};
class Shirt_RedCheck extends Shirt_ColorBase {};
class Shirt_WhiteCheck extends Shirt_ColorBase {};
class Shirt_PlaneBlack extends Shirt_ColorBase {};