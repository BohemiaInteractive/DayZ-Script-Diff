class TendencyHunger extends DisplayElementTendency 
{
	void TendencyHunger(PlayerBase player)
	{
		m_Type	=	eDisplayElements.DELM_TDCY_ENERGY;
		m_Key = NTFKEY_HUNGRY;
	}
}