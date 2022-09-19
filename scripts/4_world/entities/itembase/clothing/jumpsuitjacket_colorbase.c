class JumpsuitJacket_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class JumpsuitJacket_Blue extends JumpsuitJacket_ColorBase {};
class JumpsuitJacket_Gray extends JumpsuitJacket_ColorBase {};
class JumpsuitJacket_Green extends JumpsuitJacket_ColorBase {};
class JumpsuitJacket_Red extends JumpsuitJacket_ColorBase {};