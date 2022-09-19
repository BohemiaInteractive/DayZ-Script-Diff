class ModsMenuDetailed extends ScriptedWidgetEventHandler
{
	protected Widget											m_Root;
	protected Widget											m_Content;
	protected Widget											m_CloseButton;
	protected ScrollWidget										m_Scroll;
	protected ref map<ref ModInfo, ref ModsMenuDetailedEntry>	m_Data;
	
	protected ModInfo											m_Highlighted;
	
	//protected MainMenu											m_Menu;
	protected UIScriptedMenu											m_Menu;
	protected ModsMenuTooltip									m_Tooltip;
	protected ref Timer											m_TooltipTimer;
	protected ModInfo											m_TooltipMod;

	void ModsMenuDetailed(array<ref ModInfo> data, Widget parent, ModsMenuTooltip tooltip, UIScriptedMenu menu_parent)
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/mods_menu/mods_menu_detailed.layout", parent);
		m_Content = m_Root.FindAnyWidget("ModsDetailedContent");
		m_Scroll = ScrollWidget.Cast(m_Root.FindAnyWidget("ModsDetailedScroller"));
		m_CloseButton = m_Root.FindAnyWidget("ModsDetailedHeaderButton");
		
		m_Menu = menu_parent;
		m_Data = new map<ref ModInfo, ref ModsMenuDetailedEntry>;
		m_Tooltip = tooltip;
		
		m_Root.SetHandler(this);

		LoadEntries(data);
	}
	
	void ~ModsMenuDetailed()
	{
		delete m_Root;
	}
	
	void Open()
	{
		if( !m_Root.IsVisible() )
			m_Scroll.VScrollToPos( 0 );
		m_Root.Show( true );
	}
	
	void Close()
	{
		Highlight( null );
		m_Root.Show( false );
	}
	
	bool IsOpen()
	{
		return m_Root.IsVisible();
	}
	
	ModInfo GetHighlighted()
	{
		return m_Highlighted;
	}
	
	void HighlightFirst()
	{
		Highlight( m_Data.GetKey( 0 ) );
	}
	
	void Highlight( ModInfo mod_ref )
	{
		if( m_Highlighted )
		{
			m_Data.Get( m_Highlighted ).Deselect();
			m_Content.Update();
		}
		
		m_Highlighted = mod_ref;
		if( m_Highlighted )
		{
			m_Data.Get( m_Highlighted ).Select();
			m_Content.Update();
			ScrollToMod( m_Highlighted );
		}
	}
	
	void ScrollToMod( ModInfo mod_ref )
	{
		/*
		if( mod_ref )
		{
			float scroll_pos_x, scroll_pos_y;
			float scroll_size_x, scroll_size_y;
			float mod_pos_x, mod_pos_y;
			float mod_size_x, mod_size_y;
			
			Widget mod_widget = m_Data.Get( mod_ref ).GetWidget();
			if( mod_widget )
			{
				m_Content.Update();
				m_Scroll.Update();
				m_Scroll.GetScreenPos( scroll_pos_x, scroll_pos_y );
				m_Scroll.GetScreenSize( scroll_size_x, scroll_size_y );
				mod_widget.GetScreenPos( mod_pos_x, mod_pos_y );
				mod_widget.GetScreenSize( mod_size_x, mod_size_y );
				if( mod_pos_y + mod_size_y >= scroll_pos_y + scroll_size_y )
				{
					m_Scroll.VScrollToPos( mod_pos_y + mod_size_y - scroll_pos_y );
				}
				else if( mod_pos_y <= scroll_pos_y )
				{
					m_Scroll.VScrollToPos( mod_pos_y - scroll_pos_y );
				}
				m_Scroll.VScrollToPos( mod_pos_y - scroll_pos_y );
			}
		}
		*/
	}
	
	void Select( ModInfo mod_ref, bool show )
	{
		if( mod_ref )
		{
			if( show )
			{
				m_Highlighted = mod_ref;
				m_Data.Get( mod_ref ).Select();
			}
			else
			{
				m_Data.Get( mod_ref ).Deselect();
				if( m_Highlighted == mod_ref )
				{
					m_Highlighted = null;
				}
			}
		}
		
		ScrollToMod( m_Highlighted );
	}
	
	void PrepareTooltip( ModInfo mod_ref )
	{
		if( m_Tooltip )
		{
			m_TooltipMod = mod_ref;
			if( !m_TooltipTimer )
				m_TooltipTimer = new Timer(CALL_CATEGORY_GUI);
			
			m_TooltipTimer.Run( 1, this, "ShowTooltip" );
		}
	}
	
	void ShowTooltip()
	{
		if( m_Tooltip )
			m_Tooltip.ShowTooltip( m_TooltipMod );
	}
	
	void HideTooltip()
	{
		if( m_TooltipTimer )
			m_TooltipTimer.Stop();
		
		m_TooltipMod = null;
		if( m_Tooltip )
			m_Tooltip.HideTooltip();
	}

	void LoadEntries( array<ref ModInfo> data )
	{
		foreach(ModInfo var : data)
		{
			ref ModsMenuDetailedEntry entry = new ModsMenuDetailedEntry(var, m_Content, this);
			m_Data.Insert(var, entry);
		}
		
		m_Content.Update();
		float y_c = m_Scroll.GetContentHeight();
		float x, y;
		m_Content.GetScreenSize( x, y );
		if( y > y_c )
		{
			m_Scroll.SetAlpha( 1 );
		}
	}
	
	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if( w == m_CloseButton )
		{
			Close();
			return true;
		}
		return false;
	}
}