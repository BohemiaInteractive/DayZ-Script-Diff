class BadgeHeartbeat extends DisplayElementBadge 
{
	void BadgeHeartbeat(PlayerBase player)
	{
		NUM_OF_BITS = 2;
		m_Key 	= 	NTFKEY_HEARTBEAT;
		m_Type	=	eDisplayElements.DELM_BADGE_HEARTBEAT;
	}
		
	override void UpdateHUD()
	{
		super.UpdateHUD();
	}
}