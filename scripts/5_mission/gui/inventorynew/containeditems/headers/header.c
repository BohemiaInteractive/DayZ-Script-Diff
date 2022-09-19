class Header: LayoutHolder
{
	protected int			m_DefaultColor;
	protected int			m_DefaultFontSize;
	
	protected EntityAI		m_Entity;
	
	protected Widget		m_CollapseButton;
	protected Widget		m_UpArrowButton;
	protected Widget		m_DownArrowButton;
	protected TextWidget	m_HeaderText;
	
	void Header( LayoutHolder parent, string function_name )
	{
		m_CollapseButton	= GetMainWidget().FindAnyWidget( "collapse_button" );
		m_UpArrowButton		= GetMainWidget().FindAnyWidget( "opened" );
		m_DownArrowButton	= GetMainWidget().FindAnyWidget( "closed" );
		
		#ifdef PLATFORM_CONSOLE
			m_CollapseButton.Show( false );
		#else
		WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown( m_CollapseButton,  m_Parent, function_name );
		WidgetEventHandler.GetInstance().RegisterOnDropReceived( m_CollapseButton,  m_Parent, "OnDropReceivedFromHeader" );
		#endif
		WidgetEventHandler.GetInstance().RegisterOnDropReceived( GetMainWidget(),  m_Parent, "OnDropReceivedFromHeader" );
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver( GetMainWidget(),  this, "DraggingOverHeader" );
		
		m_HeaderText		= TextWidget.Cast( GetMainWidget().FindAnyWidget( "TextWidget0" ) );
		
		m_DefaultColor		= GetRootWidget().GetColor();
	}
	
	void SetName( string name )
	{
		name.ToUpper();
		m_HeaderText.SetText( name );
	}
	
	void SetItemPreview( EntityAI entity_ai )
	{
		#ifndef PLATFORM_CONSOLE
		m_Entity = entity_ai;
		ItemPreviewWidget item_preview = ItemPreviewWidget.Cast( GetMainWidget().FindAnyWidget( "Render" ) );
		item_preview.SetItem(entity_ai);
		item_preview.SetView( entity_ai.GetViewIndex() );
		#endif
	}
	
	void DraggingOverHeader( Widget w, int x, int y, Widget receiver )
	{
		if ( w == null )
			return;

		Container parent_container = Container.Cast( m_Parent );
		parent_container.DraggingOverHeader( w, x, y, receiver );
	}

	void OnDropReceivedFromHeader( Widget w, int x, int y, Widget receiver )
	{
		Container parent_container = Container.Cast( m_Parent );
		parent_container.OnDropReceivedFromHeader( w, x, y, receiver );
	}
	
	override void SetActive( bool active )
	{
		super.SetActive( active );
		
		if ( active )
			GetMainWidget().SetColor( ARGBF( 1, 1, 0, 0 ) );
		else
			GetMainWidget().SetColor( m_DefaultColor );
	}
	
	void ShowCollapseButton(bool state)
	{
		m_CollapseButton.Show(state);
	}
}
