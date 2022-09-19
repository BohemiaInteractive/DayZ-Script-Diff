class TendencyThirst extends DisplayElementTendency 
{
	void TendencyThirst(PlayerBase player)
	{
		m_Type	=	eDisplayElements.DELM_TDCY_WATER;
		m_Key = NTFKEY_THIRSTY;
	}
}