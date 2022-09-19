class ActionTuneFrequencyCB : ActionContinuousBaseCB
{
	private const float REPEAT_AFTER_SEC = 1.0;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(REPEAT_AFTER_SEC);
	}
}

class ActionTuneFrequency: ActionContinuousBase
{
	void ActionTuneFrequency()
	{
		m_CallbackClass = ActionTuneFrequencyCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_TUNE;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_TUNE;		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		
		m_Text = "#tune_frequency";
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
		if ( item.IsTransmitter() ) 
		{
			TransmitterBase transmitter = TransmitterBase.Cast( item );
			
			if ( transmitter.GetCompEM().IsWorking() ) 
			{
				//transmitter.DisplayRadioInfo( transmitter.GetTunedFrequency().ToString(), player );
				
				return true;
			}
		}
		
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		TransmitterBase transmitter =  TransmitterBase.Cast( action_data.m_MainItem );
		transmitter.SetNextFrequency( action_data.m_Player );
	}
}