class JumpsuitPants_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class JumpsuitPants_Blue extends JumpsuitPants_ColorBase {};
class JumpsuitPants_Green extends JumpsuitPants_ColorBase {};
class JumpsuitPants_Grey extends JumpsuitPants_ColorBase {};
class JumpsuitPants_Red extends JumpsuitPants_ColorBase {};