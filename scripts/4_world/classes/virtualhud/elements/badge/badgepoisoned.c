class BadgePoisoned extends DisplayElementBadge 
{
	void BadgePoisoned(PlayerBase player)
	{
		NUM_OF_BITS = 1;
		m_Key 	= 	NTFKEY_POISONED;
		m_Type	=	eDisplayElements.DELM_BADGE_POISONED;
	}
}