class ScriptConsoleSelector: OptionSelectorSliderSetup
{
	protected ScriptConsoleTabBase m_ParentTab;
	
	void ScriptConsoleSelector(Widget parent, float value, ScriptConsoleTabBase parent_tab, bool disabled, float min, float max)
	{
		m_Root = g_Game.GetWorkspace().CreateWidgets(GetLayoutFile(), parent);		
		m_Parent = parent;
		m_SelectorType = 1;
		m_ParentTab	= parent_tab;
		m_Slider = SliderWidget.Cast(m_Root.FindAnyWidget("option_value"));
		m_Slider.SetCurrent(value);

		m_MinValue = min;
		m_MaxValue = max;
		
		SetValue(value);
		Enable();
		
		m_Parent.SetHandler(this);
	}
	
	string GetLayoutFile()
	{
		return "gui/layouts/new_ui/script_console_slider.layout";
	}
	
	void SetColor(int color)
	{
		m_Slider.SetColor(color);
	}
	
	Widget GetLayoutRoot()
	{
		return m_Root;
	}
	
	ScriptConsoleTabBase GetTab()
	{
		return m_ParentTab;
	}
}