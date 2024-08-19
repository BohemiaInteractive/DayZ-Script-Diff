class ActionTuneRadioStationCB : ActionContinuousBaseCB
{
	private const float REPEAT_AFTER_SEC = 1.0;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(REPEAT_AFTER_SEC);
	}
}

class ActionTuneRadioStation: ActionContinuousBase
{
	void ActionTuneRadioStation()
	{
		m_CallbackClass = ActionTuneRadioStationCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_TUNE;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_TUNE;	
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		
		m_Text = "#tune_radio_station";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNone;
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool HasProneException()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}	

	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		Radio radio = Radio.Cast( item );
		
		if ( radio.CanOperate() ) 
		{
			return true;
		}
		
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		Radio radio = Radio.Cast( action_data.m_MainItem );
		radio.TuneNextStation();
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
}