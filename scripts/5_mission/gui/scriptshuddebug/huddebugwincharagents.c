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
	
	float GetTemporaryResistance()
	{
		
	}
}


class HudDebugWinCharAgents extends HudDebugWinBase
{	
	protected Widget m_WgtAgents;
	protected ref array<ref Widget>	 m_AgentWidgets = new array<ref Widget>;
	protected ref map<Widget, ref DebugAgentData>		m_AgentWidgetData = new map<Widget, ref DebugAgentData>;
	
	private AutoHeightSpacer WgtModifiersContentPanelScript;
	//============================================
	// HudDebugWinCharAgents
	//============================================
	void HudDebugWinCharAgents( Widget widget_root )
	{	
		m_WgtRoot = widget_root;
		//m_WgtAgents = TextListboxWidget.Cast( m_WgtRoot.FindAnyWidget( "txl_CharAgents_Values" ) );
		m_WgtAgents = m_WgtRoot.FindAnyWidget( "AgentList" );
		
		//FitWindowByContent( m_WgtAgents );

		m_WgtAgents.GetScript(WgtModifiersContentPanelScript);
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
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		PluginDeveloperSync developer_sync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
		
		//if client, send RPC
		if ( g_Game.IsClient() )
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
				SyncedValueAgent syncedValue = SyncedValueAgent.Cast(developer_sync.m_PlayerAgentsSynced.Get(i));
				AddAgent(
					syncedValue.GetName(),
					syncedValue.GetValue(),
					syncedValue.GetID(),
					syncedValue.GetTemporaryResistanceTime(),
				);
			}
		}
	}
	
	bool OnClick( Widget w, int x, int y, int button )
	{	
		DebugAgentData data;
		if (w.GetName() == "ButtonAgentActivate")
		{
			data = m_AgentWidgetData.Get(w);
			DebugGrowAgentsRequest(data.GetID(), true);
			return true;
		}
		else if (w.GetName() == "ButtonAgentDeactivate")
		{
			data = m_AgentWidgetData.Get(w);
			DebugGrowAgentsRequest(data.GetID(), false);
			return true;
		}
		else if (w.GetName() == "ResetAgents")
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
		Man man = g_Game.GetPlayer();
		man.RPCSingleParam(ERPCs.DEV_AGENT_GROW, p1, true, man.GetIdentity());
	}
	
	void DebugRemoveAgentsRequest()
	{
		ref Param1<bool> p1 = new Param1<bool>( false );
		Man man = g_Game.GetPlayer();
		man.RPCSingleParam(ERPCs.DEV_RPC_AGENT_RESET, p1, true, man.GetIdentity());
	}
				
	void AddAgent(string title, string value, int id, float temporaryResistance)
	{
		Widget widget = g_Game.GetWorkspace().CreateWidgets("gui/layouts/debug/day_z_hud_debug_agent.layout", m_WgtAgents);
		ButtonWidget btn = ButtonWidget.Cast( widget.FindAnyWidget( "TextAgentName" ) );
		TextWidget countWidget = TextWidget.Cast(widget.FindAnyWidget("TextWidgetAgentCount"));
		TextWidget tempResistanceWidget = TextWidget.Cast(widget.FindAnyWidget("TextWidgetAgentTempResistanceTime"));
		Widget activateButton = widget.FindAnyWidget("ButtonAgentActivate");
		Widget deactivateButton = widget.FindAnyWidget("ButtonAgentDeactivate");

		countWidget.SetText(value);	
		btn.SetText(title);
		if (temporaryResistance > 0.0)
			tempResistanceWidget.SetText(string.Format("(R-%1s)", Math.Round(temporaryResistance).ToString()));
		else
			tempResistanceWidget.SetText("");
			
		DebugAgentData data = new DebugAgentData("", id);
		m_AgentWidgets.Insert(widget);
		m_AgentWidgetData.Insert(btn, data);
		m_AgentWidgetData.Insert(activateButton, data);		
		m_AgentWidgetData.Insert(countWidget, data);
		m_AgentWidgetData.Insert(tempResistanceWidget, data);
		m_AgentWidgetData.Insert(deactivateButton, data);

		WgtModifiersContentPanelScript.Update();
	}

	void ClearAgents()
	{
		m_AgentWidgetData.Clear();

		for (int i = 0; i < m_AgentWidgets.Count(); i++)
		{
			delete m_AgentWidgets.Get(i);
		}
		
		m_AgentWidgets.Clear();
	}

	void FitWindow()
	{
		FitWindowByContent( TextListboxWidget.Cast(m_WgtAgents) );
	}
}
