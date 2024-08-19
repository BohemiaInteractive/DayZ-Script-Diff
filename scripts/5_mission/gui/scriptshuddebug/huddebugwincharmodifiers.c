class DebugModifierData
{
	string m_Name;
	int m_ID;
	
	void DebugModifierData( string name, int id )
	{
		m_Name = name;
		m_ID = id;
	}
	
	string GetName()
	{
		return m_Name;
	}
	
	int GetID()
	{
		return m_ID;
	}
}

class HudDebugWinCharModifiers extends HudDebugWinBase
{	
	protected Widget										m_WgtModifiersContent;
	protected ref array<ref Widget>							m_ModifierWidgets;
	protected ref map<Widget, ref DebugModifierData>		m_ModifierWidgetData;
	protected PluginDeveloperSync m_PluginDeveloperSync;
	protected Widget 										m_WgtDetailedInfo;
	protected TextWidget 									m_WgtDetailedInfoText;
	protected int 											m_DetailedInfoIndex;

//m_RPCSent
	
	//============================================
	// HudDebugWinCharModifiers
	//============================================
	void HudDebugWinCharModifiers( Widget widget_root )
	{	
		m_WgtRoot = widget_root;
		m_WgtModifiersContent = Widget.Cast( m_WgtRoot.FindAnyWidget( "pnl_CharModifiers_Values" ) );
		m_ModifierWidgets = new array<ref Widget>;
		m_ModifierWidgetData = new map<Widget, ref DebugModifierData>;
		m_PluginDeveloperSync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
	}

	void ~HudDebugWinCharModifiers()
	{	
		SetUpdate( false );
	}

	//============================================
	// GetWinType
	//============================================
	override int GetType()
	{
		return HudDebug.HUD_WIN_CHAR_MODIFIERS;
	}
	
	//============================================
	// Update
	//============================================
	override void SetUpdate( bool state )
	{
		//Disable update on server (PluginDeveloperSync)
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		//if client, send RPC
		if ( GetGame().IsClient() )
		{
			ref Param1<bool> params = new Param1<bool>( state );
			if ( player )
			{
				player.RPCSingleParam( ERPCs.DEV_MODS_UPDATE, params, true );
				SetRPCSent();
			}
		}
		//else set directly
		else
		{
			if ( m_PluginDeveloperSync )
			{
				m_PluginDeveloperSync.EnableUpdate( state, ERPCs.DEV_MODS_UPDATE, player );	
			}
		}
	}

	override void Update()
	{
		super.Update();
		
		Refresh();
	}
	
	//============================================
	// Show / Hide
	//============================================
	override void Show()
	{
		super.Show();
		
		//Print("Show()");
		
		SetUpdate( true );
	}

	override void Hide()
	{
		super.Hide();
		
		//Print("Hide()");
		
		SetUpdate( false );
	}
	
	void Refresh()
	{
		SetModifiers();
		if(m_WgtDetailedInfo && m_WgtDetailedInfo.IsVisible())
		{
			if(!m_WgtDetailedInfoText)
				m_WgtDetailedInfoText = TextWidget.Cast(m_WgtDetailedInfo.FindAnyWidget( "TextWidget" ));
			m_WgtDetailedInfoText.SetText(m_PluginDeveloperSync.m_PlayerModsDetailedSynced);
		}
	}
	
	//============================================
	// FitWindow
	//============================================
	void FitWindow()
	{
		float title_size = 20;
		float spacing = 20;
		
		//get wgt content size values
		float wgt_content_size_x;
		float wgt_content_size_y;
		m_WgtModifiersContent.GetSize( wgt_content_size_x, wgt_content_size_y );
		
		//get wgt root size values
		float wgt_root_size_x;
		float wgt_root_size_y;
		m_WgtRoot.GetSize( wgt_root_size_x, wgt_root_size_y );
		
		//calculate new Y size
		float new_size_y = title_size + wgt_content_size_y + spacing;
		
		//set size
		m_WgtRoot.SetSize( wgt_root_size_x, new_size_y );
	}
	
	//============================================
	// Display Modifiers
	//============================================	
	void SetModifiers()
	{
		//clear window
		ClearModifiers();
		
		if ( m_PluginDeveloperSync.m_PlayerModsSynced.Count() > 0 )
		{
			//set active mods
			for ( int i = 0; i < m_PluginDeveloperSync.m_PlayerModsSynced.Count(); ++i )
			{
				SyncedValueModifier synced_value = m_PluginDeveloperSync.m_PlayerModsSynced.Get( i );
				AddModifier( synced_value.GetName(), synced_value.GetID(), synced_value.GetActive(),synced_value.GetLocked() );
			}
		}
		
		FitWindow();
	}
	
	void AddModifier( string name, int id, bool active, bool locked )
	{
		//create widget
		Widget widget = GetGame().GetWorkspace().CreateWidgets( "gui/layouts/debug/day_z_hud_debug_modifier.layout", m_WgtModifiersContent );
		
		//add to widget array (for clearing purposes)
		m_ModifierWidgets.Insert( widget );
		
		//set widget name
		ButtonWidget mod_name_text = ButtonWidget.Cast( widget.FindAnyWidget( "TextModifierName" ) );
		mod_name_text.SetText( name );
		if ( active )
		{
			mod_name_text.SetTextColor( ARGB( 255, 0, 255, 0 ) );
		}
		else
		{
			mod_name_text.SetTextColor( ARGB( 255, 255, 0, 0 ) );
		}
		
		//set set data for interactive parts (modifier ID should be enough)
		DebugModifierData data = new DebugModifierData( name, id );
		
		Widget modifier_button = widget.FindAnyWidget( "TextModifierName" );
		m_ModifierWidgetData.Insert( modifier_button, data );
		//Activate button
		Widget activate_button = widget.FindAnyWidget( "ButtonModifierActivate" );
		m_ModifierWidgetData.Insert( activate_button, data );
		
		//Deactivate button
		Widget deactivate_button = widget.FindAnyWidget( "ButtonModifierDeactivate" );
		m_ModifierWidgetData.Insert( deactivate_button, data );

		//Lock checkbox
		Widget checkbox_widget = widget.FindAnyWidget( "CheckBoxLock" );
		m_ModifierWidgetData.Insert( checkbox_widget, data );
		//set lock based on checkbox value
		CheckBoxWidget checkbox = CheckBoxWidget.Cast( checkbox_widget );
		checkbox.SetChecked( locked );
		
		AutoHeightSpacer WgtModifiersContent_panel_script;
		m_WgtModifiersContent.GetScript( WgtModifiersContent_panel_script );
		WgtModifiersContent_panel_script.Update();
	}

	void ClearModifiers()
	{
		//clear widget data
		m_ModifierWidgetData.Clear();
		
		//destroy all modifier widgets
		for ( int i = 0; i < m_ModifierWidgets.Count(); ++i )
		{
			delete m_ModifierWidgets.Get( i );
		}
		m_ModifierWidgets.Clear();
	}
	
	//============================================
	// OnClick
	//============================================
	bool OnClick( Widget w, int x, int y, int button )
	{	
		if ( w )
		{
			if ( w.GetName() == "TextModifierName" )
			{
				//Print("clicked");
				DebugModifierData bc_data = m_ModifierWidgetData.Get( w );
				
				//Print( bc_data.GetID() );
				
				if(bc_data.GetID() == m_DetailedInfoIndex)//repeated request --> hide
				{
					if(m_WgtDetailedInfo && m_WgtDetailedInfo.IsVisible())
					{
						m_WgtDetailedInfo.Show(false);
					}
					m_DetailedInfoIndex = 0;
				}
				else
				{
					if(!m_WgtDetailedInfo)
						m_WgtDetailedInfo = GetGame().GetWorkspace().CreateWidgets( "gui/layouts/debug/day_z_hud_debug_modifier_detailed.layout");
					if(!m_WgtDetailedInfo.IsVisible())
					{
						m_WgtDetailedInfo.Show(true);
					}
					m_DetailedInfoIndex = bc_data.GetID();
				}
				if( m_WgtDetailedInfoText )
					m_WgtDetailedInfoText.SetText("");
				m_PluginDeveloperSync.m_PlayerModsDetailedSynced = "";
				RequestDetailedInfo( bc_data.GetID());
				return true;
			}
			//Button activate
			if ( w.GetName() == "ButtonModifierActivate" )
			{
				DebugModifierData ba_data = m_ModifierWidgetData.Get( w );
				
				//activate
				ActivateModifier( ba_data.GetID() );
				
				//force update
				m_PluginDeveloperSync.Update();
				
				return true;
			}
			//Button deactivate
			else if ( w.GetName() == "ButtonModifierDeactivate" )
			{
				DebugModifierData bd_data = m_ModifierWidgetData.Get( w );
				
				//deactivate
				DeactivateModifier( bd_data.GetID() );

				//force update
				m_PluginDeveloperSync.Update();
				
				return true;
			}
			//Lock checkbox
			else if ( w.GetName() == "CheckBoxLock" )
			{
				DebugModifierData lcb_data = m_ModifierWidgetData.Get( w );
				CheckBoxWidget checkbox = CheckBoxWidget.Cast( w );
				
				//set lock
				LockModifier( lcb_data.GetID(), checkbox.IsChecked() );
				
				//force update
				m_PluginDeveloperSync.Update();
				
				return true;
			}
			else if ( w.GetName() == "ResetModifiers" )
			{
				
				ResetModifiers();
				return true;
			}				
		}
		
		return false;
	}
	
	//============================================
	// Actions
	//============================================
	
	void ResetModifiers()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		//if client, send RPC

		ref Param1<bool> params = new Param1<bool>( false );
		if ( player )
		{
			player.RPCSingleParam( ERPCs.DEV_RPC_MODS_RESET, params, true );
		}


	}
	
	
  	void RequestDetailedInfo( int id )
	{
		//Disable update on server (PluginDeveloperSync)
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		//if client, send RPC
		if ( GetGame().IsClient() )
		{
			ref Param1<int> params = new Param1<int>( id );
			if ( player )
			{
				player.RPCSingleParam( ERPCs.DEV_RPC_MODS_DETAILED, params, true );
			}
		}
		//else set directly
		else
		{
			m_PluginDeveloperSync.RequestDetailedInfo( id , player);
		}
	}
	
	void ActivateModifier( int id )
	{
		//Disable update on server (PluginDeveloperSync)
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		//if client, send RPC
		if ( GetGame().IsClient() )
		{
			ref Param1<int> params = new Param1<int>( id );
			if ( player )
			{
				player.RPCSingleParam( ERPCs.DEV_RPC_MODS_ACTIVATE, params, true );
			}
		}
		//else set directly
		else
		{
			m_PluginDeveloperSync.ActivateModifier( id );
		}
	}
	
	void DeactivateModifier( int id )
	{
		//Disable update on server (PluginDeveloperSync)
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		//if client, send RPC
		if ( GetGame().IsClient() )
		{
			ref Param1<int> params = new Param1<int>( id );
			if ( player )
			{
				player.RPCSingleParam( ERPCs.DEV_RPC_MODS_DEACTIVATE, params, true );
			}
		}
		//else set directly
		else
		{
			m_PluginDeveloperSync.DeactivateModifier( id );
		}
	}
	
	void LockModifier( int id, bool state )
	{
		//Disable update on server (PluginDeveloperSync)
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		//if client, send RPC
		if ( GetGame().IsClient() )
		{
			ref Param2<int, bool> params = new Param2<int, bool>( id, state );
			if ( player )
			{
				player.RPCSingleParam( ERPCs.DEV_RPC_MODS_LOCK, params, true );
			}
		}
		//else set directly
		else
		{
			m_PluginDeveloperSync.LockModifier( id, state );
		}
	}
}
