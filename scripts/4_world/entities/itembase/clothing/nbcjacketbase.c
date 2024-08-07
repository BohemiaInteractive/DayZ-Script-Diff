class NBCJacketBase extends Top_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class NBCJacketGray extends NBCJacketBase {};
class NBCJacketWhite extends NBCJacketBase {};