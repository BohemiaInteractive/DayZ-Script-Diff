class MilitarySweater_ColorBase : Top_Base 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class MilitarySweater_Chernarus : MilitarySweater_ColorBase {};
class MilitarySweater_Livonia : MilitarySweater_ColorBase {};
class MilitarySweater_NoPatch : MilitarySweater_ColorBase {};