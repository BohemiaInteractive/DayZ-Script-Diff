
//! Data for one attachment group
class TypeAttachmentGroupData
{
	protected string m_GroupName; //cfg class name
	protected string m_Path;
	protected bool m_IsValid;
	
	//config params
	string m_Name; //! Displayable, unlocalized name
	string m_IconPath; //! in imageset format
	string m_SelectionName; //! Usually matched to some geometry selection or memory point, depends on the system
	ref array<string> m_AttSlots;
	
	void TypeAttachmentGroupData(string groupName, string pathBase)
	{
		m_GroupName = groupName;
		m_Path = "" + pathBase + " " + m_GroupName;
		
		Init();
	}
	
	protected void Init()
	{
		InitRequired();
		InitOptional();
	}
	
	protected void InitRequired()
	{
		m_IsValid = true;
		
		if (!g_Game.ConfigIsExisting("" + m_Path + " attachmentSlots"))
		{
			m_IsValid = false;
			return;
		}
		m_AttSlots = new array<string>();
		g_Game.ConfigGetTextArrayRaw("" + m_Path + " attachmentSlots", m_AttSlots);
	}
	
	protected void InitOptional()
	{
		m_Name = "";
		m_IconPath = "set:dayz_inventory image:missing";
		m_SelectionName = "";
		
		RefreshLocalizedName();
		
		if (g_Game.ConfigIsExisting("" + m_Path + " icon"))
			g_Game.ConfigGetTextRaw("" + m_Path + " icon", m_IconPath);
		
		if (g_Game.ConfigIsExisting("" + m_Path + " selection"))
			g_Game.ConfigGetTextRaw("" + m_Path + " selection", m_SelectionName);
	}
	
	string GetAttachmentGroupClassName()
	{
		return m_GroupName;
	}
	
	bool IsValid()
	{
		return m_IsValid;
	}
	
	//! Re-reads only the localized display name (safe to call on language change)
	void RefreshLocalizedName()
	{
		if (g_Game.ConfigIsExisting("" + m_Path + " name"))
			g_Game.ConfigGetText("" + m_Path + " name", m_Name);
	}
}