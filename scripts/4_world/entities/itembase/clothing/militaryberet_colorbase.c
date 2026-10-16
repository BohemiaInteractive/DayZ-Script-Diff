class MilitaryBeret_ColorBase extends HeadGear_Base
{	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class MilitaryBeret_Red extends MilitaryBeret_ColorBase {};
class MilitaryBeret_UN extends MilitaryBeret_ColorBase {};
class MilitaryBeret_NZ extends MilitaryBeret_ColorBase {};
class MilitaryBeret_ChDKZ extends MilitaryBeret_ColorBase {};
class MilitaryBeret_CDF extends MilitaryBeret_ColorBase {};
class MilitaryBeret_TK extends MilitaryBeret_ColorBase {};