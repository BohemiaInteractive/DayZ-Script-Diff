class NBCPantsBase extends Pants_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class NBCPantsGray extends NBCPantsBase {};
class NBCPantsWhite extends NBCPantsBase {};