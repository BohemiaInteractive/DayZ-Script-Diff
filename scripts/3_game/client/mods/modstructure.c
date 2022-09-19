class ModStructure
{
	protected int		m_ModIndex;
	protected string	m_ModPath;
	protected string	m_ModName;
	protected string	m_ModLogo;
	protected string	m_ModLogoSmall;
	protected string	m_ModLogoOver;
	protected string	m_ModActionURL;
	protected string	m_ModTooltip;
	protected string	m_ModOverview;
	
	void ModStructure( int index, string path )
	{
		m_ModIndex	= index;
		m_ModPath	= path;
		LoadData();
	}
	
	void LoadData()
	{
		if( GetGame().ConfigIsExisting( m_ModPath ) )
		{
			GetGame().ConfigGetText( m_ModPath + " name", m_ModName );
			GetGame().ConfigGetText( m_ModPath + " picture", m_ModLogo );
			GetGame().ConfigGetText( m_ModPath + " logo", m_ModLogoSmall );
			GetGame().ConfigGetText( m_ModPath + " logoSmall", m_ModLogoOver );
			GetGame().ConfigGetText( m_ModPath + " logoOver", m_ModActionURL );
			GetGame().ConfigGetText( m_ModPath + " tooltip", m_ModTooltip );
			GetGame().ConfigGetText( m_ModPath + " overview", m_ModOverview );
		}
	}
	
	string GetModName()
	{
		return m_ModName;
	}
	
	string GetModLogo()
	{
		return m_ModName;
	}
	
	string GetModLogoSmall()
	{
		return m_ModName;
	}
	
	string GetModLogoOver()
	{
		return m_ModName;
	}
	
	string GetModActionURL()
	{
		return m_ModName;
	}
	
	string GetModToltip()
	{
		return m_ModName;
	}
	
	string GetModOverview()
	{
		return m_ModName;
	}
}