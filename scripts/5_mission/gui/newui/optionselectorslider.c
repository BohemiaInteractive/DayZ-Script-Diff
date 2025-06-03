class OptionSelectorSlider extends OptionSelectorSliderSetup
{
	protected bool m_Changed;
	protected EditBoxWidget m_ValueText;
	protected bool m_ShowEditbox;
	protected float m_LastValue;

	void OptionSelectorSlider(Widget parent, float value, ScriptedWidgetEventHandler parent_menu, bool disabled, float min, float max, bool showEditbox = false)
	{
		if (!showEditbox)
		{
			m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/option_slider.layout", parent);
		}
		else
		{
			m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/option_slider_editbox.layout", parent);
			m_ValueText	= EditBoxWidget.Cast(m_Root.FindAnyWidget("option_value_text"));
			m_ValueText.Enable(false);
			#ifdef PLATFORM_CONSOLE
			m_ValueText.SetFlags(WidgetFlags.IGNOREPOINTER);
			#endif
		}

		#ifdef PLATFORM_CONSOLE
		m_Parent = parent.GetParent().GetParent();
		#else
		#ifdef PLATFORM_WINDOWS
		m_Parent = parent.GetParent();
		#endif
		#endif
		
		m_SelectorType = 1;
		m_ParentClass = parent_menu;
		m_Slider = SliderWidget.Cast(m_Root.FindAnyWidget("option_value"));
		m_Slider.SetCurrent(value);
		
		m_MinValue = min;
		m_MaxValue = max;
		m_LastValue = value;
		m_ShowEditbox = showEditbox;
		
		SetValue(value);
		Enable();
		
		if (showEditbox)
		{
			SetValueText();
		}

		m_Parent.SetHandler(this);
	}
	
	void SetValueText()
	{
		float percentage = Math.Round(GetRangePercantageByValue());
		m_ValueText.SetText(percentage.ToString());
	}
	
	void Refresh(float defaultValue = -1)
	{
		if (m_ShowEditbox)
		{
			if (m_ValueText.GetText() == "" && defaultValue > -1)
			{
				SetValue(defaultValue);
				SetValueText();
			}
		}
		
		m_Changed = false;
	}
	
	float GetRangePercantageByValue()
	{
		float percentage = ((GetValue() - m_MinValue) * 100) / (m_MaxValue - m_MinValue);
		return percentage;
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		float value;
		if (w == m_Slider)
		{
			if (m_ShowEditbox)
			{
				SetValueText();
			}
			
			value = GetValue();
			m_OptionChanged.Invoke(value);
			m_LastValue = value;
			return true;
		}
		else if (m_ShowEditbox && w == m_ValueText)
		{
			if (IsValidEditboxValue(m_ValueText.GetText()))
			{
				value = (m_ValueText.GetText().ToFloat() * (m_MaxValue - m_MinValue) / 100) + m_MinValue;
				m_Slider.SetCurrent(NormalizeInput(value));
				m_OptionChanged.Invoke(GetValue());
				return true;
			}
			else
			{
				m_ValueText.SetText("");
				Refresh();
				return true;
			}
		}
		return false;
	}
	
	bool Changed()
	{
		return m_Changed;
	}
	
	bool IsValidEditboxValue(string text)
	{
		TStringArray allowedCharacters = {"0","1","2","3","4","5","6","7","8","9"};
		for (int i = 0; i < text.Length(); i++)
		{
			int foundIndex = allowedCharacters.Find(text.Get(i));
			if (foundIndex == -1)
				return false;
			
			// make sure user input value can not contain zeros only
			if (i > 0 && text.ToInt() == 0)
			{
				return false;
			}
		}
		
		// make sure user input value is in valid percantage range (0 - 100)
		if (text.ToInt() > 100 || text.ToInt() < 0)
		{
			return false;
		}

		return true;
	}
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		#ifndef PLATFORM_CONSOLE	
		if (m_ShowEditbox && w == m_ValueText)
		{
			m_ValueText.Enable(true);
			SetFocus(m_ValueText);
			m_LastValue = GetValue();
			m_ValueText.SetText("");
			m_ValueText.SetTextColor(ARGB(255, 255, 0, 0));
			SetActiveOption();
			return true;
		}
		#endif
		
		return false;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		if (m_ShowEditbox)
		{
			m_LastValue = GetValue();
			UpdateActiveOption();
			SetActiveOption();
		}
		#ifdef PLATFORM_CONSOLE
		return super.OnFocus(m_Parent, x, y);
		#else
		return false;
		#endif
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (m_ShowEditbox && w == m_ValueText)
		{
			m_ValueText.Enable(false);
			UpdateActiveOption();
		}
		
		return super.OnFocusLost(w, x, y);
	}
	
	override bool IsFocusable(Widget w)
	{
		if (m_ShowEditbox && w)
		{
			return (w == m_Parent || w == m_Slider || w == m_ValueText);
		}
		return super.IsFocusable(w);
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		#ifdef PLATFORM_CONSOLE
		if (ButtonWidget.Cast(w))
		{
			ColorNormalConsole(w);
		}
		#else
		if (m_ShowEditbox && m_ValueText.GetText() == "")
		{
			Refresh(m_LastValue);
		}
		#endif
			
		return super.OnMouseLeave(w, enterW, x, y);
	}
	
	void SetActiveOption()
	{
		OptionsMenuControls menuControls = OptionsMenuControls.Cast(m_ParentClass);
		if (menuControls)
		{
			menuControls.SetActiveOption(this);
		}
	}
	
	void UpdateActiveOption()
	{
		OptionsMenuControls menuControls = OptionsMenuControls.Cast(m_ParentClass);
		if (menuControls && menuControls.GetActiveOption())
		{
			OptionSelectorSlider optionSlider = OptionSelectorSlider.Cast(menuControls.GetActiveOption());
			if (!optionSlider || !optionSlider.IsValueTextVisible())
				return;

			optionSlider.Update();
		}
	}
	
	void Update()
	{
		m_ValueText.SetTextColor(ARGB(255, 255, 255, 255));
		if (m_ValueText.GetText() == "")
			Refresh(m_LastValue);
	}
	
	bool IsValueTextVisible()
	{
		return m_ShowEditbox;
	}
}