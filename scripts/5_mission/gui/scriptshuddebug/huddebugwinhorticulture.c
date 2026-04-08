class HudDebugWinHorticulture : HudDebugWinBase
{
	protected EditBoxWidget m_FullMaturityEditBox;
	protected EditBoxWidget m_SpoilEditBox;
	protected EditBoxWidget m_SpoilRemoveEditBox;
	protected EditBoxWidget m_DryRemoveEditBox;
	protected EditBoxWidget m_SetAllEditBox;
	protected EditBoxWidget m_SpeedEditBox;
	protected ButtonWidget 	m_ButtonUpdate;
	protected ButtonWidget 	m_ButtonUpdateTarget;
	protected ButtonWidget 	m_ButtonReset;
	protected ButtonWidget 	m_ButtonSetAll;
	protected ButtonWidget 	m_ButtonSpeed;
	
	protected PluginDeveloperSync	m_DevSyncPlugin;
	
	void HudDebugWinHorticulture(Widget widget_root)
	{	
		m_WgtRoot = widget_root;
	}
	
	override int GetType()
	{
		return HudDebug.HUD_WIN_HORTICULTURE;
	}
	
	override void Show()
	{
		super.Show();
		
		if (!m_DevSyncPlugin)
			m_DevSyncPlugin = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
	}
	
	//! Called from HudDebug.c
	bool OnClick( Widget w, int x, int y, int button )
	{	
		if (!w)
			return false;
				
		if (!m_ButtonUpdate)
		{
			m_ButtonUpdate = ButtonWidget.Cast(m_WgtRoot.FindAnyWidget("ButtonUpdate"));
			m_ButtonUpdateTarget = ButtonWidget.Cast(m_WgtRoot.FindAnyWidget("ButtonUpdateTarget"));
			m_ButtonReset = ButtonWidget.Cast(m_WgtRoot.FindAnyWidget("ButtonReset"));
			m_ButtonSetAll = ButtonWidget.Cast(m_WgtRoot.FindAnyWidget("ButtonSetAll"));
			m_ButtonSpeed = ButtonWidget.Cast(m_WgtRoot.FindAnyWidget("ButtonSpeed"));
			m_SetAllEditBox = EditBoxWidget.Cast(m_WgtRoot.FindAnyWidget("SetAllEBox"));
			m_FullMaturityEditBox = EditBoxWidget.Cast(m_WgtRoot.FindAnyWidget("FullMaturityEBox"));
			m_SpoilEditBox = EditBoxWidget.Cast(m_WgtRoot.FindAnyWidget("SpoilEBox"));
			m_SpoilRemoveEditBox = EditBoxWidget.Cast(m_WgtRoot.FindAnyWidget("SpoilRemoveEBox"));
			m_DryRemoveEditBox = EditBoxWidget.Cast(m_WgtRoot.FindAnyWidget("DryRemoveEBox"));
			m_SpeedEditBox = EditBoxWidget.Cast(m_WgtRoot.FindAnyWidget("SpeedEBox"));
		}
		
		if (w == m_ButtonUpdate)
		{
			SendRPCUpdate(false);
			return true;
		}
		
		if (w == m_ButtonUpdateTarget)
		{
			SendRPCUpdate(true);
			return true;
		}
		
		if (w == m_ButtonSpeed)
		{
			float speedMult = m_SpeedEditBox.GetText().ToFloat();
			SendRPCSpeed(speedMult);
			return true;
		}
		
		if (w == m_ButtonSetAll)
		{
			int setAll = m_SetAllEditBox.GetText().ToInt();
			m_FullMaturityEditBox.SetText(setAll.ToString());
			m_SpoilEditBox.SetText(setAll.ToString());
			m_SpoilRemoveEditBox.SetText(setAll.ToString());
			m_DryRemoveEditBox.SetText(setAll.ToString());
			
			return true;
		}
		
		if (w == m_ButtonReset)
		{
			m_FullMaturityEditBox.SetText("0");
			m_SpoilEditBox.SetText("0");
			m_SpoilRemoveEditBox.SetText("0");
			m_DryRemoveEditBox.SetText("0");
			m_SpeedEditBox.SetText("1");
			SendRPCUpdate(false);
			SendRPCSpeed(1);
			
			return true;
		}
		
		return false;
	}
	
	protected void SendRPCSpeed(float multiplier)
	{
		multiplier =  Math.Clamp(multiplier, 0.1, 1000);
		Param1<float> param = new Param1<float>(multiplier);

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		
		if (!g_Game.IsMultiplayer())
			PlantBase.DebugSetTickSpeedMultiplier(multiplier);
		else
			player.RPCSingleParam(ERPCs.DEV_RPC_HORTICULTURE_SPEED, param, true);
	}
	
	protected void SendRPCUpdate(bool isTarget)
	{
		int slotID = -1;
		GardenBase garden;
		int maturity = m_FullMaturityEditBox.GetText().ToInt();
		int spoil = m_SpoilEditBox.GetText().ToInt();
		int spoilRemove = m_SpoilRemoveEditBox.GetText().ToInt();
		int dryRemove = m_DryRemoveEditBox.GetText().ToInt();
		
		Param6<int, GardenBase, int, int, int, int> params;
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		
		if (isTarget && player && player.GetActionManager())
		{
			ActionTarget at = player.GetActionManager().FindActionTarget();
			if (at)
				garden = GardenBase.Cast(at.GetObject());
			
			if (garden)
			{
				array<string> selections = new array<string>;
				garden.GetActionComponentNameList(at.GetComponentIndex(), selections);
	
				for (int s = 0; s < selections.Count(); s++)
				{
					string selection = selections[s];
					Slot slot = garden.GetSlotBySelection( selection );
					if (slot && slot.GetPlant())
					{
						slotID = slot.GetSlotIndex();
					}
				}
			}				
		}
				
		params = new Param6<int, GardenBase, int, int, int, int>(slotID, garden, maturity, spoil, spoilRemove, dryRemove);
		
		if (!g_Game.IsMultiplayer())
		{
			if (slotID != -1 && garden)
			{
				Slot gSlot = garden.GetSlotByIndex(slotID);
				if (gSlot && gSlot.GetPlant())
					gSlot.GetPlant().DebugSetTimes(maturity, spoil, spoilRemove, dryRemove);
			}
			else
				PlantBase.DebugSetGlobalTimes(params.param3, params.param4, params.param5, params.param6);
		}
		else if (player)
			player.RPCSingleParam(ERPCs.DEV_RPC_HORTICULTURE_UPDATE, params, true);
	}
}