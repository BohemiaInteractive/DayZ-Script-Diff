class SizeToChild extends ScriptedWidgetEventHandler
{
	reference string		m_ChildName;
	reference float			m_HorizontalOffset;
	reference float			m_VerticalOffset;
	reference bool			m_ResizeHorizontal;
	reference bool			m_ResizeVertical;
	
	protected Widget		m_Root;
	protected Widget		m_Child;
	
	protected static bool	m_IgnoredBool;
	
	void OnWidgetScriptInit(Widget w)
	{
		m_Root	= w;
		
		m_Child	= m_Root.FindAnyWidget( m_ChildName );
		if ( m_Child )
		{
			ResizeParentToChild();
		}
	}
	
	bool ResizeParentToChild()
	{
		return ResizeParentToChild( m_IgnoredBool, -1, false );
	}
	
	bool ResizeParentToChild( out bool changed_size, int limit = -1, bool immedUpdate = true )
	{
		float x, y, o_x, o_y, new_x, new_y;
		if ( m_Child )
		{
			m_Child.Update();
			m_Child.GetScreenSize( x, y );
			m_Root.GetScreenSize( new_x, new_y );
			m_Root.GetSize( o_x, o_y );
			
			bool changed	= false;
			bool hit_limit	= false;
			
			if ( m_ResizeHorizontal && x != new_x )
			{
				new_x = x + m_HorizontalOffset;
				changed = true;
			}
			else
				new_x = o_x;
			
			if ( m_ResizeVertical && y != new_y )
			{
				new_y = y + m_VerticalOffset;
				changed = true;
			}
			else
				new_y = o_y;
			
			if ( limit > 0 && new_y > limit )
			{
				new_y = limit;
				hit_limit = true;
			}
			
			if ( changed )
			{
				m_Root.SetSize( new_x, new_y, immedUpdate );
			}
			
			changed_size = changed;
			return hit_limit;
		}
		else
		{
			m_Child	= m_Root.FindAnyWidget( m_ChildName );
			if ( !m_Child )
			{
				Print( "Error in size to child, " + m_Root.GetName() + " has no child named " + m_ChildName );
			}
		}
		
		return false;
	}
}

class SizeToParent extends ScriptedWidgetEventHandler
{
	reference bool		m_ResizeHorizontal;
	reference bool		m_ResizeVertical;
	
	protected Widget	m_Root;
	protected Widget	m_Parent;
	
	void OnWidgetScriptInit(Widget w)
	{
		m_Root	= w;
		
		if ( m_ResizeHorizontal )
			m_Root.ClearFlags( WidgetFlags.HEXACTSIZE );
		if ( m_ResizeVertical )
			m_Root.ClearFlags( WidgetFlags.VEXACTSIZE );
		
		m_Parent	= m_Root.GetParent();
		
		Refresh();
	}
	
	void Refresh()
	{
		float x, y, o_x, o_y, new_x, new_y;
		m_Parent.Update();
		m_Parent.GetScreenSize( x, y );
		m_Root.GetScreenSize( new_x, new_y );
		m_Root.GetSize( o_x, o_y );
		
		bool changed = false;
		
		if ( m_ResizeHorizontal && x != new_x )
		{
			new_x = x;
			changed = true;
		}
		else
			new_x = o_x;
		
		if ( m_ResizeVertical && y != new_y )
		{
			new_y = y;
			changed = true;
		}
		else
			new_y = o_y;
		
		if ( changed )
			m_Root.SetSize( new_x, new_y );
	}
}