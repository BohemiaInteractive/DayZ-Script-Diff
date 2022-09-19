class ActionTuneFrequencyOnGround : ActionInteractBase
{
	string m_RadioFreq;

	void ActionTuneFrequencyOnGround()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_RadioFreq 		= string.Empty;
		m_Text = "#tune_frequency";
	}

	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_object = target.GetObject();
		Land_Radio_PanelBig transmitter = Land_Radio_PanelBig.Cast( target.GetObject() );
		m_Text = "#tune_frequency | " + transmitter.GetTunedFrequency().ToString() + " MHz";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool HasProgress()
	{
		return false;
	}
		
	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_object = target.GetObject();
		Land_Radio_PanelBig transmitter = Land_Radio_PanelBig.Cast( target_object );
		string selection = target_object.GetActionComponentName( target.GetComponentIndex() );
		
		if ( transmitter.GetCompEM().IsWorking() && selection == "control_panel" )
		{
			//transmitter.DisplayRadioInfo( transmitter.GetTunedFrequency().ToString(), player );
			//m_RadioFreq = transmitter.GetTunedFrequency().ToString();
			
			return true;
		}
		
		return false;
	}
		
	override void OnExecuteServer( ActionData action_data )
	{
		Land_Radio_PanelBig transmitter = Land_Radio_PanelBig.Cast( action_data.m_Target.GetObject() );
		transmitter.SetNextFrequency( action_data.m_Player );
	}
}