class GulagJacket_ColorBase : Top_Base 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class GulagJacket_Grey : GulagJacket_ColorBase {};