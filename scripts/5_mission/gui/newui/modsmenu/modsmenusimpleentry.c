class ModsMenuSimpleEntry extends ScriptedWidgetEventHandler
{
	protected ButtonWidget		m_ModButton;
	protected ImageWidget		m_Icon;
	protected Widget			m_Hover;

	protected bool				m_HasLogoOver;
	protected ModInfo			m_Data;
	protected ModsMenuSimple	m_ParentMenu;

	void ModsMenuSimpleEntry(ModInfo data, int index, Widget parent, ModsMenuSimple parent_menu)
	{
		m_ModButton		= ButtonWidget.Cast(GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/mods_menu/mods_menu_simple_entry.layout", parent));
		m_Icon			= ImageWidget.Cast(m_ModButton.FindAnyWidget("Icon"));
		m_Hover			= m_ModButton.FindAnyWidget("Overlay");
		m_Data			= data;
		m_ParentMenu	= parent_menu;
		
		m_ModButton.SetSort(index);
		m_ModButton.SetHandler(this);
		
		LoadData();
	}
	
	void ~ModsMenuSimpleEntry()
	{
		delete m_ModButton;
	}

	void LoadData()
	{
		string logo = m_Data.GetLogo();
		string logo_over = m_Data.GetLogoOver();
		
		if (logo != "")
		{
			m_Icon.LoadImageFile(0, logo);
		}
		else
		{
			m_Icon.LoadImageFile(0, ModInfo.DEFAULT_LOGO);
		}
		
		if (logo_over != "")
		{
			m_Icon.LoadImageFile(1, logo_over);
		}
		else
		{
			m_Icon.LoadImageFile(1, ModInfo.DEFAULT_LOGO_OVER);
		}
		m_HasLogoOver = true;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if( w == m_ModButton )
		{
			m_ParentMenu.Select( m_Data );
			return true;
		}
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if( w == m_ModButton )
		{
			if( m_HasLogoOver )
				m_Icon.SetImage( 1 );
			m_Hover.Show( true );
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if( enterW != m_ModButton )
		{
			if( m_HasLogoOver )
				m_Icon.SetImage( 0 );
			m_Hover.Show( false );
			return true;
		}
		return false;
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		if( w == m_ModButton )
		{
			if( m_HasLogoOver )
				m_Icon.SetImage( 1 );
			m_Hover.Show( true );
			return true;
		}
		return false;
	}
	
	override bool OnFocusLost( Widget w, int x, int y )
	{
		if( w == m_ModButton )
		{
			if( m_HasLogoOver )
				m_Icon.SetImage( 0 );
			m_Hover.Show( false );
			return true;
		}
		return false;
	}
}