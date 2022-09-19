class DropdownPrefab extends ScriptedWidgetEventHandler
{
	protected Widget				m_Root;
	protected Widget				m_Parent;
	protected ScrollWidget			m_Scroller;
	protected Widget				m_ContentContainer;
	protected ref array<Widget>		m_Content = new array<Widget>;
	
	protected Widget				m_Button;
	protected Widget				m_Holder;
	protected TextWidget			m_Text;
	protected ImageWidget			m_ImageExpand;
	protected ImageWidget			m_ImageCollapse;
	
	protected bool					m_IsExpanded;
	ref ScriptInvoker				m_OnSelectItem	= new ScriptInvoker();

	void DropdownPrefab( Widget root, string text = "" )
	{
		m_Parent = root;
		m_Root				= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/dropdown_prefab/dropdown_prefab.layout", root );
		
		m_Scroller			= ScrollWidget.Cast( m_Root.FindAnyWidget( "dropdown_container" ) );
		m_ContentContainer	= m_Root.FindAnyWidget( "dropdown_content" );
		m_Text				= TextWidget.Cast( m_Root.FindAnyWidget( "dropdown_text" ) );
		m_Holder			= m_Root.FindAnyWidget( "holder" );
		SetText( text );
		
		m_Button			= m_Root.FindAnyWidget( "dropdown_selector_button" );
		m_ImageExpand		= ImageWidget.Cast( m_Root.FindAnyWidget( "expand_image" ) );
		m_ImageCollapse		= ImageWidget.Cast( m_Root.FindAnyWidget( "collapse_image" ) );
		
		m_Root.SetHandler( this );
		
		RefreshContent();
	}
	
	void RefreshContent()
	{
		Widget child = m_ContentContainer.GetChildren();
		while( child )
		{
			if( m_Content.Find( child ) > -1 )
			{
				m_Content.Insert( child );
			}
		}
		
		m_ContentContainer.Update();
		m_Root.Update();
		
		float x, y;
		m_ContentContainer.GetScreenSize( x, y );
		if( y > m_Scroller.GetContentHeight() )
		{
			m_Scroller.SetAlpha( 1 );
		}
		else
		{
			m_Scroller.SetAlpha( 0 );
		}
	}
	
	int AddElement( string text, Widget content = null )
	{
		ButtonWidget element = ButtonWidget.Cast( GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/dropdown_prefab/dropdown_element.layout", m_ContentContainer ) );
		RichTextWidget textWidget = RichTextWidget.Cast(element.FindAnyWidget("dropdown_element_text"));
		textWidget.SetText( text );
		
		if( content )
		{
			element.AddChild( content, false );
		}
		m_ContentContainer.Update();
		textWidget.Update();
		m_Root.Update();
		element.Update();
		
		m_Content.Insert( element );
		return m_Content.Count() - 1;
	}
	
	void RemoveElement( int index )
	{
		if( 0 < index && index < m_Content.Count() )
		{
			delete m_Content.Get( index );
			m_ContentContainer.Update();
			m_Root.Update();
			m_Parent.Update();
		}
	}
	
	void Close()
	{
		if( m_IsExpanded )
		{
			m_IsExpanded = !m_IsExpanded;
			m_Scroller.Show( m_IsExpanded );
			m_ImageExpand.Show( !m_IsExpanded );
			m_ImageCollapse.Show( m_IsExpanded );
			
			m_Root.Update();
			m_Parent.Update();
		}
	}
	
	void SetText( string text )
	{
		m_Text.SetText( text );
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		int index = m_Content.Find( w );
		if ( index > -1 )
		{
			m_OnSelectItem.Invoke( index );
			m_IsExpanded = false;
			m_Scroller.Show( m_IsExpanded );
			m_ImageExpand.Show( !m_IsExpanded );
			m_ImageCollapse.Show( m_IsExpanded );
			return true;
		}
		return false;
	}
	
	override bool OnMouseButtonDown( Widget w, int x, int y, int button )
	{
		if ( (w == m_Button || w == m_Text || w == m_Holder) && button == MouseState.LEFT )
		{
			m_IsExpanded = !m_IsExpanded;
			m_Scroller.Show( m_IsExpanded );
			m_ImageExpand.Show( !m_IsExpanded );
			m_ImageCollapse.Show( m_IsExpanded );
			
			m_Root.Update();
			m_Parent.Update();
			m_ContentContainer.Update();
			
			return true;
		}
		return false;
	}
}