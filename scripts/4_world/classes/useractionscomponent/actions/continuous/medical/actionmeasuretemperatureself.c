class ActionMeasureTemperatureSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.MEASURE_TEMP);
	}
};

class ActionMeasureTemperatureSelf: ActionContinuousBase
{
	void ActionMeasureTemperatureSelf()
	{
		m_CallbackClass = ActionMeasureTemperatureSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_TAKETEMPSELF;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_TAKETEMPSELF;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#measure_temperature";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}
	
	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override void OnFinishProgressClient( ActionData action_data )
	{	
		Thermometer thermometer = Thermometer.Cast(action_data.m_MainItem);
		
		if (thermometer)
		{
			float value = thermometer.GetTemperatureValue(action_data.m_Player);
			action_data.m_Player.m_Hud.SetTemperature(value.ToString() + "#degrees_celsius");
		}
		
	}
};