class ParamedicPants_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class ParamedicPants_Blue extends ParamedicPants_ColorBase {};
class ParamedicPants_Crimson extends ParamedicPants_ColorBase {};
class ParamedicPants_Green extends ParamedicPants_ColorBase {};