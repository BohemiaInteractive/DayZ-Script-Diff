class NBCPantsBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class NBCPantsGray extends NBCPantsBase {};