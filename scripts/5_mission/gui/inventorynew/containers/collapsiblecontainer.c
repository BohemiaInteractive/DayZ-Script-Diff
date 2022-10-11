class CollapsibleContainer: Container
{
	protected ref CollapsibleHeader		m_CollapsibleHeader;
	protected bool						m_Hidden;
	
	void CollapsibleContainer( LayoutHolder parent, int sort = -1 )
	{
		m_MainWidget.Show( true );

		if ( sort > -1 )
			m_MainWidget.SetSort( sort + 2 );
		
		m_MainWidget = m_MainWidget.FindAnyWidget( "header" );
		
		m_Body = new array<ref LayoutHolder>;
		m_CollapsibleHeader = new CollapsibleHeader( this, "CollapseButtonOnMouseButtonDown" );
	}
	
	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.CollapsibleContainer;
	}

	EntityAI GetEntity()
	{
		return m_Entity;
	}

	override void OnDropReceivedFromHeader( Widget w, int x, int y, Widget receiver ) { }

	override void Refresh()
	{
		super.Refresh();
	}

	override void OnShow()
	{
		super.OnShow();
		m_Hidden = false;
		for (int i = 0; i < m_Body.Count(); i++)
		{
			m_Body.Get( i ).OnShow();
			Container c = Container.Cast(m_Body.Get(i));
			if (c)
			{
				c.Open();
			}
		}
		SetCollapsibleHeaderArrowState(m_Hidden);
	}
	
	override void OnHide()
	{
		m_Hidden = true;
		if (m_CollapsibleHeader)
		{
			SetCollapsibleHeaderArrowState(m_Hidden);
		}
		else
		{
			super.OnHide();
		}
		
	}
	
	override void Insert( LayoutHolder container, int pos = -1, bool immedUpdate = true )
	{
		super.Insert( container, pos, immedUpdate );
		
		//if ( immedUpdate )
		//{
			RecomputeOpenedContainers();
			UpdateCollapseButtons();
		//}
		
		SetCollapsibleHeaderArrowState(m_Hidden);
	}
	
	override void Remove( LayoutHolder container )
	{
		super.Remove( container );
		RecomputeOpenedContainers();
		UpdateCollapseButtons();
	}
	
	bool CanDisplayAnyCategory()
	{
		return false;
	}
	
	void UpdateCollapseButtons()
	{
		#ifndef PLATFORM_CONSOLE
		if (m_Body.Count() < 2)
		{
			if (m_CollapsibleHeader)
			{
				m_CollapsibleHeader.ShowCollapseButton(false);
			}
		}
		else
		{
			if (m_CollapsibleHeader)
			{
				m_CollapsibleHeader.ShowCollapseButton(true);
			}
		}
		#endif
	}
	
	void LoadDefaultState()
	{
		m_Hidden = !ItemManager.GetInstance().GetDefaultHeaderOpenState( "VICINITY" );
		//m_Closed = m_Hidden;
		
		if ( m_Hidden )
		{
			OnHide();
		}
		else
		{
			OnShow();
		}
	}
	
	bool IsHidden()
	{
		return m_Hidden;
	}
	
	void CollapseButtonOnMouseButtonDown( Widget w )
	{
		Toggle();
	}
	
	override void Toggle()
	{
		if (!m_Hidden)
		{
			for (int i = 1; i < m_Body.Count(); i++)
			{
				m_Body.Get( i ).OnHide();
				Container c = Container.Cast(m_Body.Get(i));
				if (c)
				{
					c.Close();
				}
			}
			OnHide();
		}
		else
		{
			OnShow();
		}
		m_Closed = m_Hidden;
		
		SetCollapsibleHeaderArrowState(m_Hidden);

		UpdateCollapseButtons();
	}
	
	override bool OnChildRemove( Widget w, Widget child )
	{
		if( w == GetMainWidget() )
			GetMainWidget().Update();
		return true;
	}
	
	override bool OnChildAdd( Widget w, Widget child )
	{
		if( w == GetMainWidget() )
			GetMainWidget().Update();
		return true;
	}
	
	override Header GetHeader()
	{
		return m_CollapsibleHeader;
	}
	
	override void SetHeader(Header header)
	{
		m_CollapsibleHeader = CollapsibleHeader.Cast(header);
	}
	
	void SetCollapsibleHeaderArrowState(bool open)
	{
		if (m_CollapsibleHeader)
		{
			m_CollapsibleHeader.SetArrowButtonOpened(open);
		}
	}
}
