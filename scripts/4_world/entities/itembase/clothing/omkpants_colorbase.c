class OMKPants_ColorBase extends Pants_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class OMKPants_Navy extends OMKPants_ColorBase {};