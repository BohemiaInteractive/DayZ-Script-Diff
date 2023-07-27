class SpookyTrigger extends EffectTrigger
{

}

class SpookyTrigger23 extends SpookyTrigger
{
	protected static ref SpookyTriggerEventsHandler m_EventsHandler;

	override void OnEnterClientEvent( TriggerInsider insider )
	{
		super.OnEnterClientEvent( insider );
		//Print("------------------------ >Entering ");
		PlayerBase player = PlayerBase.Cast(insider.GetObject());
		m_EventsHandler = new SpookyTriggerEventsHandler(player);
	}
	
	
	override void OnLeaveClientEvent( TriggerInsider insider )
	{
		super.OnLeaveClientEvent( insider );
		//Print("------------------------ >Leaving ");
		m_EventsHandler = null;
	}
	
	override protected void OnStayClientEvent(TriggerInsider insider, float deltaTime) 
	{
		super.OnStayClientEvent(insider, deltaTime);
		//this should be getting called only on the controlled player, as the controlled player should be the only entity, which gets added as insider client-side
		if (m_EventsHandler)
			m_EventsHandler.Update(deltaTime);
	}
}