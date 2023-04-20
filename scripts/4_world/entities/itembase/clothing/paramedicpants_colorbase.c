class ParamedicPants_ColorBase extends Pants_Base
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