class HandsHeader: Header
{
	protected bool		m_ForceHideCollapseButtons;
	protected Widget	m_ItemHeader;
	
	void HandsHeader( LayoutHolder parent, string function_name )
	{
		m_DefaultFontSize	= 24;
		m_ItemHeader		= m_MainWidget.FindAnyWidget( "hands_item_header" );
		
		m_MainWidget		= GetMainWidget().FindAnyWidget( "height_wrapper" );
		m_DefaultColor		= GetMainWidget().GetColor();
		#ifdef PLATFORM_CONSOLE
		ShowCollapseButtons( false, true );
		#endif
	}
	
	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.HandsHeader;
	}
	
	override void UpdateInterval()
	{
		PlayerBase p = PlayerBase.Cast( GetGame().GetPlayer() );
		if (!p)
			return;
		
		EntityAI item = p.GetHumanInventory().GetEntityInHands();
		if ( !m_ForceHideCollapseButtons )
		{
			if ( item && !item.GetInventory().IsInventoryLockedForLockType( HIDE_INV_FROM_SCRIPT ) && item.CanDisplayCargo() )
			{
				ShowCollapseButtons( true );
			}
			else
			{
				ShowCollapseButtons( false );
			}
		}
		
		m_ItemHeader.Show( item != null );
	}
	
	void ShowCollapseButtons( bool show, bool force = false )
	{
		m_ForceHideCollapseButtons = force;
		#ifdef PLATFORM_CONSOLE
		m_CollapseButton.Show( false );
		#else
		m_CollapseButton.Show( show );
		#endif
	}
}