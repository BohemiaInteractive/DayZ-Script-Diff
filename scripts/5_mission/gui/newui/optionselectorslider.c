class OptionSelectorSlider extends OptionSelectorSliderSetup
{
	void OptionSelectorSlider(Widget parent, float value, ScriptedWidgetEventHandler parent_menu, bool disabled, float min, float max)
	{
		m_Root					= GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/option_slider.layout", parent);
		#ifdef PLATFORM_CONSOLE
			m_Parent				= parent.GetParent().GetParent();
		#else
		#ifdef PLATFORM_WINDOWS
			m_Parent				= parent.GetParent();
		#endif
		#endif
		
		m_SelectorType				= 1;
		m_ParentClass				= parent_menu;
		m_Slider					= SliderWidget.Cast(m_Root.FindAnyWidget("option_value"));
		m_Slider.SetCurrent(value);
		
		m_MinValue					= min;
		m_MaxValue					= max;
		
		SetValue(value);
		Enable();
		
		m_Parent.SetHandler(this);
	}
}