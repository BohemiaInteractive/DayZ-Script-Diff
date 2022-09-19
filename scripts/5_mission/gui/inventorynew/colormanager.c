class ColorManager
{
	private ref static ColorManager m_Instance;
	
	static int COLOR_NORMAL_TEXT		= ARGB( 255, 255, 255, 255 );
	static int COLOR_NORMAL_PANEL		= ARGB( 140, 0, 0, 0 );
	static int COLOR_HIGHLIGHT_TEXT		= ARGB( 255, 200, 0, 0 );
	static int COLOR_HIGHLIGHT_PANEL	= ARGB( 255, 0, 0, 0 );
	static int COLOR_SELECTED_TEXT		= ARGB( 255, 255, 255, 255 );
	static int COLOR_SELECTED_PANEL		= ARGB( 255, 200, 0, 0 );
    static int COLOR_DISABLED_TEXT		= ARGB( 100, 255, 255, 255 );
	static int COLOR_DISABLED_PANEL		= ARGB( 140, 0, 0, 0 );
	
	static int ITEM_BACKGROUND_COLOR = ARGB( 50, 255, 255, 255 );
	static int BASE_COLOR = ARGB( 10, 255, 255, 255 );
	static int RED_COLOR = ARGB( 150, 255, 1, 1 );
	static int GREEN_COLOR = ARGB( 150, 1, 255, 1 );
	static int SWAP_COLOR = ARGB( 150, 135, 206, 250 );
	static int FSWAP_COLOR = ARGB( 150, 35, 106, 150 );
	static int COMBINE_COLOR = ARGB( 150, 255, 165, 0 );
	
	void ColorManager()
	{
		m_Instance = this;
	}
	
	static ColorManager GetInstance()
	{
		return m_Instance;
	}
	
	void SetColor( Widget w, int color )
	{
		if( w.FindAnyWidget( "Selected" ) )
		{
			w.FindAnyWidget( "Selected" ).SetColor( color );
		}
		else
		{
			string name = w.GetName();
			name.Replace( "PanelWidget", "Selected" );
			Widget w2 = w.GetParent().FindAnyWidget( name );
			if( w2 )
			{
				w2.SetColor( color );
			}
		}
	}
	
	int GetItemColor( ItemBase item )
	{
		int color = -1;
		ItemBase dragged_item = ItemBase.Cast( ItemManager.GetInstance().GetDraggedItem() );
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		if ( dragged_item )
			return 0;
		
		// if item icon is main icon (view, viewgrid )
		if ( item.GetHierarchyParent() && item.GetHierarchyParent() != player )
		{
			color = -1;
		}

		if ( item ) 
		{
			float temperature = item.GetTemperature();
			if ( temperature )
			{
				color = GetTemperatureColor( temperature );
			}
		}
		return color;
	}
}
