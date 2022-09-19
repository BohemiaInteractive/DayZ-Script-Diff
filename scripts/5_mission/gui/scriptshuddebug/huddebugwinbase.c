class HudDebugWinBase
{
	Widget m_WgtRoot;
	protected bool m_Show;
	protected bool m_RPCSent;
	
	//============================================
	// Constructor
	//============================================
	void HudDebugWinBase( Widget widget_root )
	{
		m_WgtRoot = widget_root;
	}

	//============================================
	// GetType
	//============================================
	int GetType()
	{
		return HudDebug.HUD_WIN_UNDEFINED;
	}

	void Init( Widget widget_root );
	
	void SetUpdate( bool state );
	
	void Update()
	{
		if(m_Show && !m_RPCSent)
		{
			SetUpdate(true);
		}
	}
	
	
	//============================================
	// SetUpdate
	//============================================
	void SetRPCSent()
	{
		m_RPCSent = true;
	}
	
	
	//============================================
	// Show
	//============================================
	void Show()
	{
		m_WgtRoot.Show(true);
		m_Show = true;
	}
	
	//============================================
	// Hide
	//============================================
	void Hide()
	{
		m_WgtRoot.Show(false);
		m_Show = false;
	}
	
	//============================================
	// Is visible
	//============================================
	bool IsVisible()
	{
		return m_WgtRoot.IsVisible();
	}

	//============================================
	// FitWindowByContent
	//============================================
	void FitWindowByContent( TextListboxWidget wgt )
	{	
		int size_of_value = 17;
		SetValueListSize( 250, size_of_value * wgt.GetNumItems(), wgt );
	}

	//============================================
	// SetSize
	//============================================
	void SetValueListSize( int x, int y, TextListboxWidget wgt )
	{
		int offset = 10;	
		int size_of_title = 20;
		
		wgt.SetSize( x, y );
		
		int root_size_x = x + ( 2 * offset );
		int root_size_y = y + ( 2 * offset ) + size_of_title;
		
		m_WgtRoot.SetSize( root_size_x, root_size_y );
	}
}
