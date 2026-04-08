class DebugMonitor
{
	protected bool m_IsUsingKBM;
	
	private Widget m_WidgetRoot;
	private TextWidget m_WindowLabelText;
	
	private TextWidget m_VersionValue;
	private TextWidget m_HealthValue;
	private TextWidget m_BloodValue;
	private TextWidget m_DmgSourceValue;
	private TextWidget m_MapTileValue;
	private TextWidget m_PositionValue;
	private TextWidget m_CopyPositionInfo;
	
	private TextWidget m_FPSValue;
	private TextWidget m_FPSMinValue;
	private TextWidget m_FPSMaxValue;
	private TextWidget m_FPSAvgValue;
	
	private int m_FPSTextDefaultColor;
		
	void DebugMonitor()
	{		
		m_WidgetRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/debug/day_z_debug_monitor.layout");
		m_WidgetRoot.Show(false);
		
		m_VersionValue = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("VersionValue"));
		m_HealthValue = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("HealthValue"));
		m_BloodValue = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("BloodValue"));
		m_DmgSourceValue = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("DmgSourceValue"));
		m_MapTileValue = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("MapTileValue"));
		m_PositionValue = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("PositionValue"));
		m_CopyPositionInfo = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("CopyPositionInfo"));
		
		m_FPSValue = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("FPSCurrentValue"));
		m_FPSMinValue = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("FPSMinValue"));
		m_FPSMaxValue = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("FPSMaxValue"));
		m_FPSAvgValue = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("FPSAvgValue"));
		
		m_FPSTextDefaultColor = m_FPSValue.GetColor();
	}
	
	void Init()
	{
		string version;
		g_Game.GetVersion(version);
		m_VersionValue.SetText(" " + version);
		
		g_Game.GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		if (g_Game.GetInput().IsKeyboardConnected())
			m_IsUsingKBM = true;
		
		m_WidgetRoot.Show(true);
	}

	void SetHealth(float value)
	{
		string health = string.Format(" %1", value.ToString());
		m_HealthValue.SetText(health);
	}

	void SetBlood(float value)
	{
		string blood = string.Format(" %1", value.ToString());
		m_BloodValue.SetText(blood);
	}
	
	void SetLastDamage(string value)
	{
		if (value != "")
			m_DmgSourceValue.SetText(" " + value);
		else
			m_DmgSourceValue.SetText(" -");
	}
	
	/*!		
		Update the framerate values displayed within the debug monitor with new ones.
	*/
	void SetFramerate(float current, float min, float max, float avg)
	{
		SetFramerateText(m_FPSValue, current);
		SetFramerateText(m_FPSMinValue, min);
		SetFramerateText(m_FPSMaxValue, max);
		SetFramerateText(m_FPSAvgValue, avg);
	}
	
	/*!
		Sets the provided text `widget` to display framerate `value`.
		Formats and colors the widget according to the value.
	*/
	protected void SetFramerateText(TextWidget widget, float value)
	{
		// Ideally we would poll the refresh rate and base the values as
		// percentage thereof, but there is no such API in scripts.
		
		#ifdef PLATFORM_CONSOLE		
			// default [30, inf] ; orange [20, 29] ; red [0, 19]
			if (value > 29)
				widget.SetColor(m_FPSTextDefaultColor);
			else if (value > 19)
				widget.SetColor(0xFFFF8000); // COLOR_ORANGE
			else
				widget.SetColor(COLOR_RED);	
		#else
			// default [60, inf] ; yellow [40, 59] ; orange [30, 39] ; red [0, 29]
			if (value > 59)
				widget.SetColor(m_FPSTextDefaultColor);
			else if (value > 39)
				widget.SetColor(COLOR_YELLOW);
			else if (value > 29)
				widget.SetColor(0xFFFF8000); // COLOR_ORANGE
			else
				widget.SetColor(COLOR_RED);	
		#endif
		
		widget.SetTextFormat("%1", Math.Round(value));
	}
	
	void SetPosition(vector value)
	{
		m_MapTileValue.SetText(" " + CalculateMapTile(value));		
		string position = string.Format(" %1 %2 %3", value[0].ToString(), value[1].ToString(), value[2].ToString());
		m_PositionValue.SetText(position);
		
		if (GetUApi().GetInputByID(UAUICopyDebugMonitorPos).LocalPress())
		{
			string adjusted = (value[0] + 200000).ToString() + " " + value[2].ToString();
			g_Game.CopyToClipboard(adjusted);
		}
		
		if (m_IsUsingKBM)
			m_CopyPositionInfo.SetText(" (P to clipboard)");
		else
			m_CopyPositionInfo.SetText("");
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
		float worldSize = g_Game.GetWorld().GetWorldSize();
		
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
	
	bool IsVisible()
	{
		return m_WidgetRoot.IsVisible();
	}
};
	
