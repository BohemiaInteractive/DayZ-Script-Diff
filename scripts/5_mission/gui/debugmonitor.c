class DebugMonitor
{
	protected bool m_IsUsingKBM;
	
	private Widget m_WidgetRoot;
	private TextWidget m_pWindowLabelText;
	private TextListboxWidget m_pNamesListboxWidget;
	private TextListboxWidget m_pValuesListboxWidget;
	private MultilineTextWidget m_pModifiersMultiTextWidget;
		
	void DebugMonitor()
	{		
		m_WidgetRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/debug/day_z_debug_monitor.layout");
		m_WidgetRoot.Show(false);
		
		m_pWindowLabelText = TextWidget.Cast( m_WidgetRoot.FindAnyWidget("WindowLabel") );
		m_pNamesListboxWidget = TextListboxWidget.Cast( m_WidgetRoot.FindAnyWidget("NamesListboxWidget") );
		m_pValuesListboxWidget = TextListboxWidget.Cast( m_WidgetRoot.FindAnyWidget("ValuesListboxWidget") );
		m_pModifiersMultiTextWidget = MultilineTextWidget.Cast( m_WidgetRoot.FindAnyWidget("ModifiersMultilineTextWidget") );
	}
	
	void Init()
	{
		string version;
		g_Game.GetVersion(version);
		
		m_pNamesListboxWidget.AddItem("EXECUTABLE: ", NULL, 0);
		m_pValuesListboxWidget.AddItem("", NULL, 0);
		m_pValuesListboxWidget.SetItem(0, " " + version, NULL, 0);
		
		m_pNamesListboxWidget.AddItem("SCRIPTS: ", NULL, 0);
		m_pValuesListboxWidget.AddItem("", NULL, 0);
		m_pValuesListboxWidget.SetItem(1, " " + GetPBOAPI().GetPBOVersion("dta\\scripts.pbo"), NULL, 0);

		m_pNamesListboxWidget.AddItem("HEALTH:", NULL, 0);
		m_pValuesListboxWidget.AddItem("", NULL, 0);

		m_pNamesListboxWidget.AddItem("BLOOD:", NULL, 0);
		m_pValuesListboxWidget.AddItem("", NULL, 0);

		/*m_pNamesListboxWidget.AddItem("BODY TEMP:", NULL, 0);
		m_pValuesListboxWidget.AddItem("", NULL, 0);*/
		
		m_pNamesListboxWidget.AddItem("LAST DAMAGE:", NULL, 0);
		m_pValuesListboxWidget.AddItem("", NULL, 0);
		
		m_pNamesListboxWidget.AddItem("MAP TILE:", NULL, 0);
		m_pValuesListboxWidget.AddItem("", NULL, 0);
		
		m_pNamesListboxWidget.AddItem("POSITION:", NULL, 0);
		m_pValuesListboxWidget.AddItem("", NULL, 0);
		
		m_pNamesListboxWidget.AddItem("", NULL, 0);
		m_pValuesListboxWidget.AddItem("", NULL, 0);
		
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		if (GetGame().GetInput().IsKeyboardConnected())
			m_IsUsingKBM = true;

		/*m_pNamesListboxWidget.AddItem("MODIFIERS:", NULL, 0);
		m_pValuesListboxWidget.AddItem("", NULL, 0);*/
		
		//m_DebugFloatsListWidget.AddItem(key,NULL,0);
		//m_DebugFloatsListWidget.SetItem(i,value.ToString(),NULL,1);	
			
		m_WidgetRoot.Show(true);
	}

	void SetHealth(float value)
	{
		string health = string.Format(" %1", value.ToString());
		m_pValuesListboxWidget.SetItem(2, health, NULL, 0);
	}

	void SetBlood(float value)
	{
		string blood = string.Format(" %1", value.ToString());
		m_pValuesListboxWidget.SetItem(3, blood, NULL, 0);
	}
	
	void SetLastDamage(string value)
	{
		string lastDamage = string.Format(" %1", value);
		m_pValuesListboxWidget.SetItem(4, lastDamage, NULL, 0);
	}
	
	void SetPosition(vector value)
	{		
		m_pValuesListboxWidget.SetItem(5, " " + CalculateMapTile(value), NULL, 0);
		
		string position = string.Format(" %1 %2 %3", value[0].ToString(), value[1].ToString(), value[2].ToString());
		m_pValuesListboxWidget.SetItem(6, position, NULL, 0);
		
		if (GetUApi().GetInputByID(UAUICopyDebugMonitorPos).LocalPress())
		{
			string adjusted = (value[0] + 200000).ToString() + " " + value[2].ToString();
			GetGame().CopyToClipboard(adjusted);
		}
		
		if (m_IsUsingKBM)
			m_pValuesListboxWidget.SetItem(7," (P to clipboard)", NULL, 0);
		else 
			m_pValuesListboxWidget.SetItem(7,"", NULL, 0);
	}
		
	void Show()
	{
		m_WidgetRoot.Show(true);
	}
	
	void Hide()
	{
		m_WidgetRoot.Show(false);
	}
	
	string CalculateMapTile(vector pos)
	{
		string tile;
		float worldSize = GetGame().GetWorld().GetWorldSize();
		
		float tileX = Math.InverseLerp(0, worldSize, pos[0]);
		float tileY = Math.InverseLerp(0, worldSize, pos[2]);
		
		tile = GetTileFomFraction(tileX).ToString() + GetTileFomFraction(tileY).ToString();
		
		return tile;
	}
	
	int GetTileFomFraction(float fraction)
	{
		if (fraction < 0.25)
			return 0;
		else if (fraction < 0.5)
			return 1;
		else if (fraction < 0.75)
			return 2;
		else 
			return 3;
	
	}
	
	void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		if (pInputDeviceType == EInputDeviceType.MOUSE_AND_KEYBOARD)
			m_IsUsingKBM = true;
		else 
			m_IsUsingKBM = false;
	}
};
	
