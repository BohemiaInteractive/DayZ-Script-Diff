class PoweredOptic_Base extends ItemOptics
{
	protected PlayerBase 				m_Player;
	
	void SetPlayer( PlayerBase player )
	{
		m_Player = player;
	}
	
	PlayerBase GetPlayer()
	{
		return m_Player;
	}
}
