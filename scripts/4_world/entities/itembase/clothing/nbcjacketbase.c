class NBCJacketBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class NBCJacketGray extends NBCJacketBase {};