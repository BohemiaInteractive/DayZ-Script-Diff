class ActionWetClothingInHandsBaseCB : ActionContinuousBaseCB
{
	float m_WetGainPerStep;

	override void CreateActionComponent()
	{
		float wetMax    = 1.0;
		float remaining = 1.0;
		ItemBase clothing = ItemBase.Cast(m_ActionData.m_MainItem);
		if (clothing)
		{
			wetMax    = clothing.GetWetMax();
			remaining = wetMax - clothing.GetWet();
		}

		float stepTime = Math.Clamp(wetMax * 0.2, 0.1, remaining) * UATimeSpent.WRING;
		m_WetGainPerStep = stepTime / UATimeSpent.WRING;
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(stepTime);
	}
}

class ActionWetClothingInHandsBase : ActionContinuousBase
{
	void ActionWetClothingInHandsBase()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_WASHFACEPOND;
		m_FullBody   = true;
		m_Text       = "#STR_Wet_Clothing";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		ActionWetClothingInHandsBaseCB cb = ActionWetClothingInHandsBaseCB.Cast(action_data.m_Callback);
		if (!cb) return;

		ItemBase clothing = action_data.m_MainItem;
		if (!clothing) return;

		clothing.SetWet(Math.Clamp(clothing.GetWet() + cb.m_WetGainPerStep, 0.0, clothing.GetWetMax()));
		action_data.m_Player.GetThermalBiasHandler().Add(-PlayerConstants.THERMAL_BIAS_WASH_HEAD_DECREMENT * cb.m_WetGainPerStep);
	}

	override void OnEndServer(ActionData action_data)
	{
		ActionWetClothingInHandsBaseCB cb = ActionWetClothingInHandsBaseCB.Cast(action_data.m_Callback);
		if (!cb) return;

		float partial = cb.m_WetGainPerStep * action_data.m_ActionComponent.GetProgress();
		if (partial <= 0) return;

		ItemBase clothing = action_data.m_MainItem;
		if (!clothing) return;

		clothing.SetWet(Math.Clamp(clothing.GetWet() + partial, 0.0, clothing.GetWetMax()));
		action_data.m_Player.GetThermalBiasHandler().Add(-PlayerConstants.THERMAL_BIAS_WASH_HEAD_DECREMENT * partial);
	}

	override bool IsLockTargetOnUse()
	{
		return false;
	}
}
