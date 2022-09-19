class UIPopupScriptPositionManager extends UIPopupScript
{
	private TextListboxWidget	m_LstPositionList;
	private EditBoxWidget m_TxtSelectedX;
	private EditBoxWidget m_TxtSelectedY;
	private EditBoxWidget m_TxtCurrentX;
	private EditBoxWidget m_TxtCurrentY;
	private PluginConfigDebugProfileFixed m_ConfigDebugProfileFixed;
	private ButtonWidget m_TeleportButton;
	private ButtonWidget m_CancelButton;
			
	//================================================
	// UIPopupScriptPositionManager
	//================================================	
	void UIPopupScriptPositionManager( Widget wgt )
	{
		m_ConfigDebugProfileFixed	= PluginConfigDebugProfileFixed.Cast( GetPlugin( PluginConfigDebugProfileFixed ) );
		m_LstPositionList	= TextListboxWidget.Cast( wgt.FindAnyWidget("tls_ppp_pm_positions_list") );
		m_TxtSelectedX = EditBoxWidget.Cast( wgt.FindAnyWidget("pnl_ppp_pm_selected_x_value") );
		m_TxtSelectedY = EditBoxWidget.Cast( wgt.FindAnyWidget("pnl_ppp_pm_selected_y_value") );
		m_TxtCurrentX = EditBoxWidget.Cast( wgt.FindAnyWidget("pnl_ppp_pm_current_x_value") );
		m_TxtCurrentY = EditBoxWidget.Cast( wgt.FindAnyWidget("pnl_ppp_pm_current_y_value") );
		m_TeleportButton = ButtonWidget.Cast( wgt.FindAnyWidget("btn_ppp_pm_teleport") );
		m_CancelButton = ButtonWidget.Cast( wgt.FindAnyWidget("btn_ppp_pm_cancel") );
		
		TStringArray positions_array = new TStringArray;
		m_ConfigDebugProfileFixed.GetAllPositionsNames( positions_array );
		
		for ( int i = 0; i < positions_array.Count(); i++ )
		{
			m_LstPositionList.AddItem( positions_array.Get(i), new PresetParams ( positions_array.Get(i), true, false), 0);
		}
	}
	
	//================================================
	// OnOpen
	//================================================	
	override void OnOpen( Param param )
	{		
		if( PluginSceneManager.GetInstance() )
		{
			if( PluginSceneManager.GetInstance().GetSelectedSceneObject() )
			{
				vector player_pos = PluginSceneManager.GetInstance().GetSelectedSceneObject().GetPosition();
			
				m_TxtCurrentX.SetText( player_pos[0].ToString() );
				m_TxtCurrentY.SetText( player_pos[2].ToString() );
			}
		}
	}
	
	//================================================
	// OnClick
	//================================================
	override bool OnClick( Widget w, int x, int y, int button )
	{
		if ( w == m_TeleportButton )
		{
			float pos_x = m_TxtSelectedX.GetText().ToFloat();
			float pos_z = m_TxtSelectedY.GetText().ToFloat();
			
			PluginSceneManager.GetInstance().SelectedObjectSetPosX( pos_x );
			PluginSceneManager.GetInstance().SelectedObjectSetPosZ( pos_z );
			PopupBack();
			return true;
		}
		else if ( w == m_CancelButton )
		{
			PopupBack();
			return true;
		}
		return false;
	}
	
	void OnItemSelected( Widget w, int x, int y, int row, int column, int oldRow, int oldColumn )
	{	
		vector position = m_ConfigDebugProfileFixed.GetPositionByName( GetCurrentPositionName() );
		m_TxtSelectedX.SetText( position[0].ToString() );
		m_TxtSelectedY.SetText( position[2].ToString() );
	}
	
	string GetCurrentPositionName()
	{
		if ( m_LstPositionList.GetSelectedRow() != -1 )
		{
			string position_name;
			m_LstPositionList.GetItemText( m_LstPositionList.GetSelectedRow(), 0, position_name );
			return position_name;
		}
		return "";
	}
}
