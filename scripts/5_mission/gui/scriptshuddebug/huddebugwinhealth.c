class HudDebugWinHealthEntry
{
	string 		m_Zone;
	string 		m_HealthMode;
	bool 		m_IsTitleEntry;

	Widget 		m_EntryRootW;
	TextWidget	m_EntryZoneW;
	TextWidget 	m_EntryValueW;
	Widget m_SpacerLine;
}

class HudDebugWinHealth : HudDebugWinBase
{	
	protected bool 			m_IsInit;
	protected bool 			m_IsModeSelf = true;
	protected GridSpacerWidget	 m_WgtPanel;
	protected ScrollWidget	m_WgtPanelScroll;
	protected TextWidget 	m_TargetDisplayNameW;
	protected ButtonWidget 	m_ButtonSelf;
	protected ButtonWidget	m_ButtonTarget;
	protected Widget m_WgtHeader;
	
	protected EntityAI 					m_TargetEntity;
	protected PluginDeveloperSync		m_DevSyncPlugin;
	
	protected ref array<ref HudDebugWinHealthEntry> m_Entries = {};
	protected float m_ContenHeight;
	
	//============================================
	void HudDebugWinHealth(Widget widget_root)
	{	
		m_WgtRoot 			= widget_root;
		m_WgtPanel 			= GridSpacerWidget.Cast(m_WgtRoot.FindAnyWidget("HealthPanel") );
		m_WgtPanelScroll 	= ScrollWidget.Cast(m_WgtRoot.FindAnyWidget("HealthPanelScroll") );
		m_WgtHeader = m_WgtRoot.FindAnyWidget("header_Health");
		
		m_ConfigDebugProfile = PluginConfigDebugProfile.Cast(GetPlugin(PluginConfigDebugProfile));
		if (m_ConfigDebugProfile)
		{
			vector pos = m_ConfigDebugProfile.GetHealthDebugPos();
			if (m_WgtRoot && pos != vector.Zero)
			{
				float wx = pos[0];
				float wy = pos[1];
				m_WgtRoot.SetPos(wx, wy);
			}
		}
	}

	void ~HudDebugWinHealth()
	{
		SetUpdate(false);
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
			if (!m_IsInit)
				InitDebug();
			
			if (m_DevSyncPlugin.GetIsTargetChanged() || !m_TargetEntity)
			{
				m_DevSyncPlugin.SetTargetChanged(false);
				CleanupEntries();
				
				if (m_IsModeSelf)
				m_TargetEntity = g_Game.GetPlayer();
				#ifdef DEVELOPER
				else 
					m_TargetEntity = EntityAI.Cast(_item);
				#endif
				
				InitEntityEntries();
			}
			
			if (m_TargetEntity)
				UpdateZones();
			else 
				m_TargetDisplayNameW.SetText("No target selected");
		}
	}
	
	override int GetType()
	{
		return HudDebug.HUD_WIN_HEALTH;
	}
	
	override void Show()
	{
		super.Show();
		SetUpdate(true);

		if (!m_DevSyncPlugin)
			m_DevSyncPlugin = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
	}

	override void Hide()
	{
		super.Hide();
				
		CleanupEntries();
		m_IsInit = false;
		m_TargetEntity = null;
		SetUpdate(false);
	}
	
	override void SetUpdate( bool state )
	{
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		
		if (g_Game.IsClient())
		{
			Param1<bool> params = new Param1<bool>( state );
			if (player)
			{
				player.RPCSingleParam( ERPCs.DEV_HEALTH_UPDATE, params, true );
				SetRPCSent();
			}
		}
		else 	// set directly
		{
			if (m_DevSyncPlugin)
				m_DevSyncPlugin.EnableUpdate( state, ERPCs.DEV_HEALTH_UPDATE, player );	
		}
	}
	
	void InitDebug()
	{
		if (!m_ButtonSelf)
			m_ButtonSelf = ButtonWidget.Cast(m_WgtRoot.FindAnyWidget("ButtonSelf"));
	
		if (!m_ButtonTarget)
			m_ButtonTarget = ButtonWidget.Cast(m_WgtRoot.FindAnyWidget("ButtonTarget"));
		
		if (!m_TargetDisplayNameW)
			m_TargetDisplayNameW = TextWidget.Cast(m_WgtRoot.FindAnyWidget("txt_TargetName"));
		
		if (m_IsModeSelf)					// initial RPC set so the server knows which mode is the deubg in
			OnClick(m_ButtonSelf, 0, 0, 0);
		else
			OnClick(m_ButtonTarget, 0, 0, 0);

		m_IsInit = true;
	}
	
	//! Called from HudDebug.c
	bool OnClick( Widget w, int x, int y, int button )
	{
		SetDragging(false);
		
		if (!w)
			return false;
		
		HudDebugWinHealthEntry clickedEntry;
		
		foreach (HudDebugWinHealthEntry entry : m_Entries)
		{
			if (entry.m_EntryRootW == w.GetParent())
				clickedEntry = entry;
		}
				
		int buttonID;
		
		if (w.GetName() == "ButtonMin")
		{
			buttonID = 0;
			RunRPC(clickedEntry, buttonID);
			return true;
		}
		else if (w.GetName() == "ButtonMax")
		{
			buttonID = 1;
			RunRPC(clickedEntry, buttonID);
			return true;
		}
		else if (w.GetName() == "ButtonAdd")
		{
			buttonID = 2;
			RunRPC(clickedEntry, buttonID);
			return true;
		}
		else if (w.GetName() == "ButtonSubtract")
		{
			buttonID = 3;
			RunRPC(clickedEntry, buttonID);
			return true;
		}
		else if (w == m_ButtonTarget)
		{
			#ifdef DEVELOPER
			buttonID = 4;
			HandleTargetState(false);
			RunRPC(clickedEntry, buttonID);
			return true;
			#endif
		}
		else if (w == m_ButtonSelf)
		{
			buttonID = 5;
			HandleTargetState(true);
			RunRPC(clickedEntry, buttonID);
			return true;
		}
		
		return false;
	}
	
	void RunRPC(HudDebugWinHealthEntry entry, int buttonID)
	{
		Param3<int , string, string> params;
		if (entry)
			params = new Param3<int , string, string>( buttonID, entry.m_Zone, entry.m_HealthMode);
		else 
			params = new Param3<int , string, string>( buttonID, "", "");
		
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		if ( player )
			player.RPCSingleParam( ERPCs.DEV_RPC_HEALTH_SET, params, true );
	}
	
	// Self/target button visuals | true = self, false = target
	void HandleTargetState(bool button)
	{				
		if (button)
		{
			m_IsModeSelf = true;
			m_ButtonSelf.SetTextColor(COLOR_YELLOW);
			m_ButtonTarget.SetTextColor(0xFF808080); // grey
		}
		else 
		{
			m_IsModeSelf = false;
			m_ButtonTarget.SetTextColor(COLOR_YELLOW);
			m_ButtonSelf.SetTextColor(0xFF808080); // grey
		}
	}
	
	// Setup widgets for health zones
	void InitEntityEntries()
	{
		if (!m_TargetEntity)
			return;
				
		m_TargetDisplayNameW.SetText(m_TargetEntity.ClassName());
		
		array<ref SyncedValue> healthSynched = m_DevSyncPlugin.GetHealthZonesSynched();
		
		foreach (SyncedValue data : healthSynched)
		{
			InitEntry(data);
		}
		
		m_WgtPanel.Update();
		
		float widthS, heightS;
		m_WgtPanelScroll.GetSize(widthS, heightS);
		m_WgtPanelScroll.SetSize(widthS, m_ContenHeight);
		m_WgtPanelScroll.VScrollToPos(0);
	}
	
	void InitEntry(SyncedValue data)
	{
		HudDebugWinHealthEntry entry = new HudDebugWinHealthEntry();
		entry.m_EntryRootW = g_Game.GetWorkspace().CreateWidgets( "gui/layouts/debug/day_z_hud_debug_health.layout", m_WgtPanel );
		entry.m_EntryZoneW = TextWidget.Cast(entry.m_EntryRootW.FindAnyWidget("Name"));
		entry.m_SpacerLine = entry.m_EntryRootW.FindAnyWidget("SpacerLine");
		
		if (data.m_ValueNorm == 1)
			entry.m_HealthMode = "Shock";
		else if (data.m_ValueNorm == 2)
			entry.m_HealthMode = "Blood";
		else 
			entry.m_HealthMode = "Health";
		
		entry.m_IsTitleEntry = data.m_State;
		
		if (entry.m_IsTitleEntry)	//title entry with disabled buttons
		{
			entry.m_EntryZoneW.SetText(entry.m_HealthMode);
			entry.m_EntryZoneW.SetColor(0xFF00FFFF); // blue
			entry.m_SpacerLine.Show(true);
			
			entry.m_EntryRootW.FindAnyWidget("Value").Show(false);
			entry.m_EntryRootW.FindAnyWidget("Value").Enable(false);
			entry.m_EntryRootW.FindAnyWidget("ButtonMin").Show(false);
			entry.m_EntryRootW.FindAnyWidget("ButtonMin").Enable(false);
			entry.m_EntryRootW.FindAnyWidget("ButtonMax").Show(false);
			entry.m_EntryRootW.FindAnyWidget("ButtonMax").Enable(false);
			entry.m_EntryRootW.FindAnyWidget("ButtonAdd").Show(false);
			entry.m_EntryRootW.FindAnyWidget("ButtonAdd").Enable(false);
			entry.m_EntryRootW.FindAnyWidget("ButtonSubtract").Show(false);
			entry.m_EntryRootW.FindAnyWidget("ButtonSubtract").Enable(false);
		}
		else 
		{
			entry.m_EntryZoneW.SetText(data.m_Name);
			entry.m_Zone = data.m_Name;
			entry.m_EntryValueW = TextWidget.Cast(entry.m_EntryRootW.FindAnyWidget("Value"));
		}
		
		float width, height;
		entry.m_EntryRootW.GetSize(width, height);
		m_ContenHeight += height;
				
		m_Entries.Insert(entry);
	}

	void UpdateZones()
	{
		if (m_Entries.Count() == 0 || !m_TargetEntity)
			return;
		
		array<ref SyncedValue> healthSynched = m_DevSyncPlugin.GetHealthZonesSynched();
		float roundedHealth, zoneMax;
		
		foreach (int i, SyncedValue value : healthSynched)
		{
			HudDebugWinHealthEntry entry = m_Entries.Get(i);
			
			if (entry.m_IsTitleEntry)
				continue;
			
			if (entry.m_Zone == "Global")
				zoneMax = m_TargetEntity.GetMaxHealth("", entry.m_HealthMode);
			else
				zoneMax = m_TargetEntity.GetMaxHealth(entry.m_Zone, entry.m_HealthMode);
			
			roundedHealth = Math.Round(value.m_Value * 100) * 0.01;
			entry.m_EntryValueW.SetText(roundedHealth.ToString() + "/" + zoneMax.ToString());
			
			if (roundedHealth == zoneMax)
				entry.m_EntryValueW.SetColor(COLOR_GREEN);
			else if (roundedHealth < zoneMax * 0.3)
				entry.m_EntryValueW.SetColor(COLOR_RED);
			else 
				entry.m_EntryValueW.SetColor(COLOR_YELLOW);
		}
	}
	
	void CleanupEntries()
	{
		foreach (HudDebugWinHealthEntry entry : m_Entries)
		{
			entry.m_EntryRootW.Unlink();
		}
		
		m_Entries.Clear();
		m_ContenHeight = 0;
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
			m_ConfigDebugProfile.SetHealthDebugPos(Vector(wx,wy,0));
		}
	}
}