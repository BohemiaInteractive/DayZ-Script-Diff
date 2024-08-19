class ActionRaiseMegaphoneCB : ActionContinuousBaseCB
{
	private const float REPEAT_AFTER_SEC = -1.0;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( -1 );
	}
}

class ActionRaiseMegaphone: ActionContinuousBase
{
	void ActionRaiseMegaphone()
	{
		m_CallbackClass = ActionRaiseMegaphoneCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_RAISEITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_RAISEITEM;	
		m_Text = "#raise_megaphone";	
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

	override bool HasProgress()
	{
		return false;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if ( item.IsTransmitter() ) 
		{
			ItemMegaphone megaphone = ItemMegaphone.Cast( item );
			
			if ( megaphone && megaphone.GetCompEM().IsWorking() ) 
			{
				return true;
			}
		}
		
		return false;
	}

	override void OnStartServer( ActionData action_data )
	{
		ItemMegaphone megaphone = ItemMegaphone.Cast( action_data.m_MainItem );
		megaphone.SetCanSpeak( true );
	}
		
	override void OnEndServer( ActionData action_data )
	{
		ItemMegaphone megaphone = ItemMegaphone.Cast( action_data.m_MainItem );
		megaphone.SetCanSpeak( false );
	}
	
	override void OnStartClient( ActionData action_data )
	{
		ItemMegaphone megaphone = ItemMegaphone.Cast( action_data.m_MainItem );
		megaphone.SetCanSpeak( true );
	}
		
	override void OnEndClient( ActionData action_data )
	{
		ItemMegaphone megaphone = ItemMegaphone.Cast( action_data.m_MainItem );
		megaphone.SetCanSpeak( false );
	}
	
}