class DebugAgentData
{
	string m_Name;
	int m_ID;
	
	void DebugAgentData( string name, int id )
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


class HudDebugWinCharAgents extends HudDebugWinBase
{	
	protected Widget m_WgtAgents;
	protected ref array<ref Widget>	 m_AgentWidgets = new array<ref Widget>;
	protected ref map<Widget, ref DebugAgentData>		m_AgentWidgetData = new map<Widget, ref DebugAgentData>;
	//============================================
	// HudDebugWinCharAgents
	//============================================
	void HudDebugWinCharAgents( Widget widget_root )
	{	
		m_WgtRoot = widget_root;
		//m_WgtAgents = TextListboxWidget.Cast( m_WgtRoot.FindAnyWidget( "txl_CharAgents_Values" ) );
		m_WgtAgents = m_WgtRoot.FindAnyWidget( "AgentList" );
		
		//FitWindowByContent( m_WgtAgents );
	}

	void ~HudDebugWinCharAgents()
	{	
		SetUpdate( false );
	}

	//============================================
	// GetWinType
	//============================================
	override int GetType()
	{
		return HudDebug.HUD_WIN_CHAR_AGENTS;
	}
	
	//============================================
	// Update
	//============================================
	override void SetUpdate( bool state )
	{
		//Disable update on server (PluginDeveloperSync)
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		PluginDeveloperSync developer_sync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
		
		//if client, send RPC
		if ( GetGame().IsClient() )
		{
			ref Param1<bool> params = new Param1<bool>( state );
			if ( player )
			{
				player.RPCSingleParam( ERPCs.DEV_AGENTS_UPDATE, params, true );
				SetRPCSent();
			}
		}
		//else set directly
		else
		{
			if ( developer_sync )
			{
				developer_sync.EnableUpdate( state, ERPCs.DEV_AGENTS_UPDATE, player );	
			}
		}
	}

	override void Update()
	{
		super.Update();
		
		//Print("Update()");
		
		//refresh notifiers
		SetAgents();
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
	
	void SetAgents()
	{
		PluginDeveloperSync developer_sync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
		
		//clear window
		ClearAgents();
			
		//set agents	
		if ( developer_sync.m_PlayerAgentsSynced.Count() > 0 )
		{
			for ( int i = 0; i < developer_sync.m_PlayerAgentsSynced.Count(); i++ )
			{
				AddAgent( developer_sync.m_PlayerAgentsSynced.Get( i ).GetName(), developer_sync.m_PlayerAgentsSynced.Get( i ).GetValue(), developer_sync.m_PlayerAgentsSynced.Get( i ).GetID() );
			}
		}
		
		//fit to screen
		//FitWindow();
	}
	
	/*
	void AddAgent( string title, string value )
	{
		int index = m_WgtAgents.AddItem( title, NULL, 0 );
		m_WgtAgents.SetItem( index, value, NULL, 1 );
	}
	*/
	bool OnClick( Widget w, int x, int y, int button )
	{	
		//Button activate
		DebugAgentData data;
		if ( w.GetName() == "ButtonAgentActivate" )
		{
			data = m_AgentWidgetData.Get( w );
			DebugGrowAgentsRequest(data.GetID(), true);
			//Print("activate" + data.GetID().ToString());
			return true;
		}
		//Button deactivate
		else if ( w.GetName() == "ButtonAgentDeactivate" )
		{
			data = m_AgentWidgetData.Get( w );
			DebugGrowAgentsRequest(data.GetID(), false);
			//Print("deactivate" + data.GetID().ToString());
			return true;
		}
		else if ( w.GetName() == "ResetAgents" )
		{
			DebugRemoveAgentsRequest();
			return true;
		}
		
		
		return false;
	}
	
	void DebugGrowAgentsRequest(int agent_id, bool should_grow)
	{
		if(!should_grow)//id is minus value to mark killing instead of growing
		{
			agent_id *= -1;
		}
		
		ref Param1<int> p1 = new Param1<int>( agent_id );
		Man man = GetGame().GetPlayer();
		man.RPCSingleParam(ERPCs.DEV_AGENT_GROW, p1, true, man.GetIdentity());
	}
	
	void DebugRemoveAgentsRequest()
	{
		ref Param1<bool> p1 = new Param1<bool>( false );
		Man man = GetGame().GetPlayer();
		man.RPCSingleParam(ERPCs.DEV_RPC_AGENT_RESET, p1, true, man.GetIdentity());
	}
				
	void AddAgent( string title, string value, int id )
	{
		Widget widget = GetGame().GetWorkspace().CreateWidgets( "gui/layouts/debug/day_z_hud_debug_agent.layout", m_WgtAgents );
		ButtonWidget btn = ButtonWidget.Cast( widget.FindAnyWidget( "TextAgentName" ) );
		
	
		DebugAgentData data = new DebugAgentData( "", id );
		m_AgentWidgetData.Insert(btn, data);
		
		Widget activate_button = widget.FindAnyWidget( "ButtonAgentActivate" );
		m_AgentWidgetData.Insert( activate_button, data );
		
		TextWidget count_widget = TextWidget.Cast(widget.FindAnyWidget( "TextWidgetAgentCount" ));
		m_AgentWidgetData.Insert( count_widget, data );
		count_widget.SetText(value);
		
		//Deactivate button
		Widget deactivate_button = widget.FindAnyWidget( "ButtonAgentDeactivate" );
		m_AgentWidgetData.Insert( deactivate_button, data );
		
		btn.SetText(title);
		m_AgentWidgets.Insert(widget);
		
		AutoHeightSpacer WgtModifiersContent_panel_script;
		m_WgtAgents.GetScript( WgtModifiersContent_panel_script );
		WgtModifiersContent_panel_script.Update();
	}

	void ClearAgents()
	{
		m_AgentWidgetData.Clear();
		for ( int i = 0; i < m_AgentWidgets.Count(); i++ )
		{
			delete m_AgentWidgets.Get( i );
		}
		
		m_AgentWidgets.Clear();
	}

	void FitWindow()
	{
		FitWindowByContent( TextListboxWidget.Cast(m_WgtAgents) );
	}
}
