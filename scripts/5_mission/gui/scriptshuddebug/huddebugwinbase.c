class HudDebugWinBase
{
	Widget m_WgtRoot;
	protected bool m_Show;
	protected bool m_RPCSent;
	
	PluginConfigDebugProfile m_ConfigDebugProfile;
	bool m_IsDragging;
	vector m_DraggingOffset;
	
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
	
	void Update(float timeslice)
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
	void SetValueListSize(int x, int y, Widget wgt)
	{
		int offset = 10;	
		int sizeOfTitle = 20;
		
		wgt.SetSize(x, y);
		
		int rootSizex = x + (2 * offset);
		int rootSizeY = y + (2 * offset) + sizeOfTitle;
		
		m_WgtRoot.SetSize(rootSizex, rootSizeY);
	}
	
	void SetDragging(bool enable)
	{
		if(enable && !m_IsDragging)
			OnDraggingStart();
		else if (!enable && m_IsDragging)
			OnDraggingEnd();
		m_IsDragging = enable;
	}
	
	void OnDraggingStart()
	{
		int mx, my;
		float wx, wy;
		GetMousePos(mx, my);
		m_WgtRoot.GetScreenPos(wx, wy);
		m_DraggingOffset[0] = wx - mx;
		m_DraggingOffset[1] = wy - my;
	}
	
	void OnDraggingEnd();
}
