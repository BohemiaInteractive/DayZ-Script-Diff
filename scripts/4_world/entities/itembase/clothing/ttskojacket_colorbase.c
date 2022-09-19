class TTsKOJacket_ColorBase extends Clothing 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class TTsKOJacket_Camo extends TTsKOJacket_ColorBase {};