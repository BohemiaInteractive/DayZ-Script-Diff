class OptionSelectorSlider extends OptionSelectorSliderSetup
{
	protected EditBoxWidget m_ValueText;
	protected bool m_Changed;
	
	void OptionSelectorSlider(Widget parent, float value, ScriptedWidgetEventHandler parent_menu, bool disabled, float min, float max, bool showEditbox = false)
	{
		if (!showEditbox)
		{
			m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/option_slider.layout", parent);
		}
		else
		{
			m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/option_slider_editbox.layout", parent);
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
		if (showEditbox)
		{
			m_ValueText	= EditBoxWidget.Cast(m_Root.FindAnyWidget("option_value_text"));
			#ifdef PLATFORM_CONSOLE
			m_ValueText.SetFlags(WidgetFlags.IGNOREPOINTER);
			#endif
		}

		if (min > 0 && max > 0)
		{
			m_Slider.SetMinMax(min, max);
		}
		m_Slider.SetCurrent(value);

		m_MinValue = min;
		m_MaxValue = max;
		
		SetValue(value);
		if (showEditbox)
		{
			m_ValueText.Show(true);
			SetValueText();
		}

		m_Changed = false;
		Enable();
		
		m_Parent.SetHandler(this);
	}
	
	int GetRangePercantageByValue(float value, float rangeMin, float rangeMax)
	{
		int percentage = ((value - rangeMin) * 100) / (rangeMax - rangeMin);
		if (percentage > 100) 
		{
			percentage = 100;
		} 
		else if (percentage < 0)
		{
			percentage = 0;
		}

		return percentage;
	}
	
	float GetRangeValueByPercantage(int percantage, float rangeMin, float rangeMax)
	{
		float value = ((percantage * (rangeMax - rangeMin) / 100) + rangeMin;
		if (value < rangeMin)
		{
			value = rangeMin;
		}
		else if (value > rangeMax)
		{
			value = rangeMax;
		}
		
		return value;
	}
	
	float GetSliderValue()
	{
		return m_Slider.GetCurrent();
	}
	
	bool IsValueTextVisible()
	{
		if (m_ValueText)
			return m_ValueText.IsVisible();
		
		return false;
	}
	
	void SetValueText()
	{
		int percentage = GetRangePercantageByValue(GetSliderValue(), m_MinValue, m_MaxValue);
		m_ValueText.SetText(percentage.ToString());
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

	void Refresh(float defaultValue = -1)
	{
		if (IsValueTextVisible())
		{
			if (m_ValueText.GetText() == "" && defaultValue > -1)
			{
				SetValue(defaultValue);
				SetValueText();
				return;
			}
		}
		
		m_Changed = false;
	}
	
	bool Changed()
	{
		return m_Changed;
	}
	
	void SetChanged(bool state)
	{
		m_Changed = state;
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w == m_Slider)
		{
			m_OptionChanged.Invoke(GetValue());
			if (IsValueTextVisible())
			{
				SetValueText();
			}
			m_Changed = true;
			return true;
		}
		else if (IsValueTextVisible() && w == m_ValueText)
		{
			if (IsValidEditboxValue(m_ValueText.GetText()))
			{
				float value = GetRangeValueByPercantage(m_ValueText.GetText().ToInt(), m_MinValue, m_MaxValue);
				m_Slider.SetCurrent(value);
				m_OptionChanged.Invoke(GetValue());
				m_Changed = true;
				return true;
			}
			else
			{
				m_ValueText.SetText("");
				Refresh();
			}
		}
		
		return false;
	}
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (IsValueTextVisible() && w == m_Parent)
		{
			SetFocus(m_ValueText);
			m_ValueText.SetText("");
			#ifndef PLATFORM_CONSOLE
			m_ValueText.SetTextColor(ARGB(255, 255, 0, 0));
			#endif
			return true;
		}
		
		return false;
	}
	
	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if (IsValueTextVisible() && w == m_ValueText)
		{
			m_ValueText.SetText("");
			m_ValueText.SetTextColor(ARGB(255, 255, 0, 0));
			return true;
		}
		
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		super.OnFocusLost(w, x, y);
		
		if (IsValueTextVisible() && w)
		{
			if (m_ValueText.GetText() == "")
			{
				SetValueText();
			}

			m_ValueText.SetTextColor(ARGB(255, 255, 255, 255));
			return true;
		}
		
		return super.OnFocusLost(w, x, y);
	}
	
	override bool IsFocusable(Widget w)
	{
		if (IsValueTextVisible() && w)
		{
			return (w == m_Parent || w == m_Slider || w == m_ValueText);
		}
		return super.IsFocusable(w);
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		super.OnMouseLeave(w, enterW, x, y);
 
		#ifdef PLATFORM_CONSOLE
		if (ButtonWidget.Cast(w))
		{
			ColorNormalConsole(w);
		}
		#endif
			
		return true;
	}
}