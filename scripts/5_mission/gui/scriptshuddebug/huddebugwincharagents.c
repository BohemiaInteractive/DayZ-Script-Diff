class HudDebugWinCharAgentsEntry extends ScriptedWidgetEventHandler
{	
	protected Widget m_Root;
	protected ButtonWidget m_ActivateButton;
	protected ButtonWidget m_DeactivateButton;
	protected HudDebugWinCharAgents m_HudDebugAgents;
	protected int m_ID;
		
	void HudDebugWinCharAgentsEntry(HudDebugWinCharAgents hudDebugAgents, Widget parent, string title, string value, int id, float temporaryResistance)
	{
		m_HudDebugAgents = hudDebugAgents;
		m_ID = id;
		
		m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/debug/day_z_hud_debug_agent.layout", parent);
		
		TextWidget txt = TextWidget.Cast(m_Root.FindAnyWidget("TextAgentName"));
		TextWidget countWidget = TextWidget.Cast(m_Root.FindAnyWidget("TextWidgetAgentCount"));
		TextWidget tempResistanceWidget = TextWidget.Cast(m_Root.FindAnyWidget("TextWidgetAgentTempResistanceTime"));
		m_ActivateButton = ButtonWidget.Cast(m_Root.FindAnyWidget("ButtonAgentActivate"));
		m_DeactivateButton = ButtonWidget.Cast(m_Root.FindAnyWidget("ButtonAgentDeactivate"));
		
		countWidget.SetText(value);	
		txt.SetText(title);
		if (temporaryResistance > 0.0)
			tempResistanceWidget.SetText(string.Format("(R-%1s)", Math.Round(temporaryResistance).ToString()));
		else
			tempResistanceWidget.SetText("");
		
		m_Root.SetHandler(this);
	}
	
	void DestroyEntry()
	{
		m_Root.Unlink();
		delete m_Root;
	}
	
	void DebugGrowAgentsRequest(int agentId, bool shouldGrow)
	{
		if(!shouldGrow)//id is minus value to mark killing instead of growing
		{
			agentId *= -1;
		}
		
		Param1<int> p1 = new Param1<int>(agentId);
		Man man = GetGame().GetPlayer();
		man.RPCSingleParam(ERPCs.DEV_AGENT_GROW, p1, true, man.GetIdentity());
	}
		
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w == m_ActivateButton)
		{
			DebugGrowAgentsRequest(m_ID, true);
			return true;
		}
		else if (w == m_DeactivateButton)
		{
			DebugGrowAgentsRequest(m_ID, false);
			return true;
		}
		
		return false;
	}
}

class HudDebugWinCharAgents extends HudDebugWinBase
{
	protected const float AGENTS_UPDATE_TIME = 0.500;
	
	protected GridSpacerWidget m_WgtAgents;
	protected Widget m_WgtHeader;
	protected ref array<ref HudDebugWinCharAgentsEntry> m_AgentEntries = new array<ref HudDebugWinCharAgentsEntry>;
	
	protected float m_UpdateAgentsTimer;
	
	void HudDebugWinCharAgents(Widget widget_root)
	{
		m_WgtRoot = widget_root;
		m_WgtAgents = GridSpacerWidget.Cast(m_WgtRoot.FindAnyWidget("AgentList"));
		m_WgtHeader = m_WgtRoot.FindAnyWidget("header_CharacterAgents");
		
		OnInit();
	}
	
	void OnInit()
	{
		m_ConfigDebugProfile = PluginConfigDebugProfile.Cast(GetPlugin(PluginConfigDebugProfile));
		if (m_ConfigDebugProfile)
		{
			vector pos = m_ConfigDebugProfile.GetCharacterAgentsPos();
			if (m_WgtRoot && pos != vector.Zero)
			{
				float wx = pos[0];
				float wy = pos[1];
				m_WgtRoot.SetPos(wx, wy);
			}
		}
	}

	void ~HudDebugWinCharAgents()
	{	
		SetUpdate(false);
	}
	
	override int GetType()
	{
		return HudDebug.HUD_WIN_CHAR_AGENTS;
	}
	
	override void SetUpdate(bool state)
	{
		//Disable update on server (PluginDeveloperSync)
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		PluginDeveloperSync developer_sync = PluginDeveloperSync.Cast(GetPlugin(PluginDeveloperSync));
		
		//if client, send RPC
		if ( g_Game.IsClient() )
		{
			Param1<bool> params = new Param1<bool>(state);
			if (player)
			{
				player.RPCSingleParam(ERPCs.DEV_AGENTS_UPDATE, params, true);
				SetRPCSent();
			}
		}
		//else set directly
		else
		{
			if (developer_sync)
			{
				developer_sync.EnableUpdate(state, ERPCs.DEV_AGENTS_UPDATE, player);	
			}
		}
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		if (m_IsDragging)
		{
			int x,y;
			GetMousePos(x,y);
			m_WgtRoot.SetPos(x + m_DraggingOffset[0], y + m_DraggingOffset[1]);
		}
		else
		{
			m_UpdateAgentsTimer += timeslice;
			if (m_UpdateAgentsTimer >= AGENTS_UPDATE_TIME)
			{
				m_UpdateAgentsTimer = 0;
				SetAgents();
			}
		}
	}
	
	override void Show()
	{
		super.Show();		
		
		SetUpdate(true);
	}

	override void Hide()
	{
		super.Hide();
		
		SetUpdate(false);
	}
	
	void SetAgents()
	{
		PluginDeveloperSync developerSync = PluginDeveloperSync.Cast(GetPlugin(PluginDeveloperSync));
		
		//clear window
		ClearAgents();
			
		//set agents
		int agentsCount = developerSync.m_PlayerAgentsSynced.Count();
		if (agentsCount > 0)
		{
			foreach (SyncedValueAgent agent : developerSync.m_PlayerAgentsSynced)
			{
				AddAgent(agent.GetName(), agent.GetValue(), agent.GetID(), agent.GetTemporaryResistanceTime());
			}
		}
	}
	
	bool OnClick(Widget w, int x, int y, int button)
	{
		SetDragging(false);
				
		if (w.GetName() == "ResetAgents")
		{
			DebugRemoveAgentsRequest();
			return true;
		}

		return false;
	}
		
	void DebugRemoveAgentsRequest()
	{
		Param1<bool> p1 = new Param1<bool>(false);
		Man man = g_Game.GetPlayer();
		man.RPCSingleParam(ERPCs.DEV_RPC_AGENT_RESET, p1, true, man.GetIdentity());
	}
				
	void AddAgent(string title, string value, int id, float temporaryResistance)
	{
		HudDebugWinCharAgentsEntry agentEntry = new HudDebugWinCharAgentsEntry(this, m_WgtAgents, title, value, id, temporaryResistance);
		m_AgentEntries.Insert(agentEntry);
	}

	void ClearAgents()
	{
		for (int i = 0; i < m_AgentEntries.Count(); i++)
		{
			HudDebugWinCharAgentsEntry entry = m_AgentEntries[i];
			entry.DestroyEntry();
			delete entry;
		}
				
		m_AgentEntries.Clear();
	}
	
	bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{		
		if (button == 0 && (w == m_WgtRoot || w == m_WgtHeader))
		{
			SetDragging(true);
			return true;
		}
				
		return false;
	}
	
	bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if (button == 0 && m_IsDragging)
		{
			SetDragging(false);
			return true;
		}
				
		return false;
	}
	
	override void OnDraggingEnd()
	{
		float wx, wy;
		m_WgtRoot.GetScreenPos(wx,wy);
		if (m_ConfigDebugProfile)
		{
			m_ConfigDebugProfile.SetCharacterAgentsPos(Vector(wx,wy,0));
		}
	}
}
