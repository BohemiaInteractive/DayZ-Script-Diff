class CollapsibleHeader: Header
{
	void CollapsibleHeader( LayoutHolder parent, string function_name )
	{
		m_DefaultFontSize = 24;
	}

	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.CollapsibleHeader;
	}
	
	void SetArrowButtonOpened(bool state)
	{
		m_UpArrowButton.Show(state);
		m_DownArrowButton.Show(!state);
	}
	
	bool IsHeaderVisible()
	{
		return m_MainWidget.IsVisible(); //m_MainWidget.IsVisibleHierarchy()
	}
	
	void SetHeaderVisible(bool show)
	{
		m_MainWidget.Show(show);
	}
}
