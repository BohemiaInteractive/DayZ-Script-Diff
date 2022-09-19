class NotificationData
{
	string m_Icon;
	string m_TitleText;
	string m_DescriptionText;
	
	void NotificationData( string icon, string title_text, string desc_text = "" )
	{
		m_Icon = icon;
		m_TitleText = title_text;
		if( desc_text != "" )
			m_DescriptionText = desc_text;
	}
}