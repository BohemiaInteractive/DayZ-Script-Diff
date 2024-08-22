class ScriptConsoleWeatherTab : ScriptConsoleTabBase
{
	protected ref Timer m_RPCTimer = new Timer();
	protected const int INTERPOLATION_MAX = 920;
	protected const int DURATION_MAX = 920;
	
	// FOG
	protected SliderWidget m_FogValueSetSlider;
	protected SliderWidget m_FogValueGetSlider;
	
	protected TextWidget m_FogValueSetText;

	protected SliderWidget m_FogInterpolationTimeSlider;
	protected SliderWidget m_FogDurationTimeSlider;
	
	
	protected TextWidget m_FogInterpolationMaxText;
	protected TextWidget m_FogDurationMaxText;
	
	protected EditBoxWidget m_FogValueEditbox;
	protected EditBoxWidget m_FogInterpolationEditbox;
	protected EditBoxWidget m_FogDurationEditbox;
	
	protected static float m_FogValue = -1;
	protected static float m_FogInterpolationTimeValue;
	protected static float m_FogDurationTimeValue = -1;
	
	
	// OVERCAST
	protected SliderWidget m_OvercastValueSetSlider;
	protected SliderWidget m_OvercastValueGetSlider;
	
	protected TextWidget m_OvercastValueSetText;

	protected SliderWidget m_OvercastInterpolationTimeSlider;
	protected SliderWidget m_OvercastDurationTimeSlider;
	
	
	protected TextWidget m_OvercastInterpolationMaxText;
	protected TextWidget m_OvercastDurationMaxText;
	
	protected EditBoxWidget m_OvercastValueEditbox;
	protected EditBoxWidget m_OvercastInterpolationEditbox;
	protected EditBoxWidget m_OvercastDurationEditbox;
	
	protected static float m_OvercastValue = -1;
	protected static float m_OvercastInterpolationTimeValue;
	protected static float m_OvercastDurationTimeValue = -1;
	
	
	// Rain
	protected SliderWidget m_RainValueSetSlider;
	protected SliderWidget m_RainValueGetSlider;
	
	protected TextWidget m_RainValueSetText;

	protected SliderWidget m_RainInterpolationTimeSlider;
	protected SliderWidget m_RainDurationTimeSlider;
	
	
	protected TextWidget m_RainInterpolationMaxText;
	protected TextWidget m_RainDurationMaxText;
	
	protected EditBoxWidget m_RainValueEditbox;
	protected EditBoxWidget m_RainInterpolationEditbox;
	protected EditBoxWidget m_RainDurationEditbox;
	
	protected static float m_RainValue = -1;
	protected static float m_RainInterpolationTimeValue;
	protected static float m_RainDurationTimeValue = -1;
	
	// Snowfall
	protected SliderWidget m_SnowfallValueSetSlider;
	protected SliderWidget m_SnowfallValueGetSlider;
	
	protected TextWidget m_SnowfallValueSetText;

	protected SliderWidget m_SnowfallInterpolationTimeSlider;
	protected SliderWidget m_SnowfallDurationTimeSlider;
	
	
	protected TextWidget m_SnowfallInterpolationMaxText;
	protected TextWidget m_SnowfallDurationMaxText;
	
	protected EditBoxWidget m_SnowfallValueEditbox;
	protected EditBoxWidget m_SnowfallInterpolationEditbox;
	protected EditBoxWidget m_SnowfallDurationEditbox;
	
	protected static float m_SnowfallValue = -1;
	protected static float m_SnowfallInterpolationTimeValue;
	protected static float m_SnowfallDurationTimeValue = -1;
	
	
	// Other
	protected ButtonWidget m_ResetButton;
	protected ButtonWidget m_UpdateButton;
	protected ButtonWidget m_ApplyButton;
	
	protected ButtonWidget m_CopyButton;
	protected ButtonWidget m_PasteButton;
	
	
	
	void ~ScriptConsoleWeatherTab()
	{
		// FOG
		m_FogValue = m_FogValueSetSlider.GetCurrent() / 100;
		m_FogInterpolationTimeValue = m_FogInterpolationTimeSlider.GetCurrent() / 100 * INTERPOLATION_MAX;
		m_FogDurationTimeValue = m_FogDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		// OVERCAST
		m_OvercastValue = m_OvercastValueSetSlider.GetCurrent() / 100;
		m_OvercastInterpolationTimeValue = m_OvercastInterpolationTimeSlider.GetCurrent() / 100 * INTERPOLATION_MAX;
		m_OvercastDurationTimeValue = m_OvercastDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		// Rain
		m_RainValue = m_RainValueSetSlider.GetCurrent() / 100;
		m_RainInterpolationTimeValue = m_RainInterpolationTimeSlider.GetCurrent() / 100 * INTERPOLATION_MAX;
		m_RainDurationTimeValue = m_RainDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		// Snowfall
		m_SnowfallValue = m_SnowfallValueSetSlider.GetCurrent() / 100;
		m_SnowfallInterpolationTimeValue = m_SnowfallInterpolationTimeSlider.GetCurrent() / 100 * INTERPOLATION_MAX;
		m_SnowfallDurationTimeValue = m_SnowfallDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
	}
	
	void ScriptConsoleWeatherTab(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
	{
		// FOG
		m_FogValueSetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderFogValue"));
		m_FogValueGetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderFogActualValue"));	
		m_FogValueSetText = TextWidget.Cast(root.FindAnyWidget("TextFogValueActual"));	
		
		m_FogInterpolationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderFogTarget"));	
		m_FogDurationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderFogDuration"));
		
		m_FogInterpolationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxFogTarget"));
		m_FogDurationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxFogDuration"));
		
		m_FogValueEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxFogValue"));
		m_FogInterpolationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxFogTarget"));
		m_FogDurationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxFogDuration"));
		
		// OVERCAST
		m_OvercastValueSetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderOvercastValue"));
		m_OvercastValueGetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderOvercastActualValue"));	
		m_OvercastValueSetText = TextWidget.Cast(root.FindAnyWidget("TextOvercastValueActual"));	
		
		m_OvercastInterpolationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderOvercastTarget"));	
		m_OvercastDurationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderOvercastDuration"));
		
		m_OvercastInterpolationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxOvercastTarget"));
		m_OvercastDurationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxOvercastDuration"));
		
		m_OvercastValueEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxOvercastValue"));
		m_OvercastInterpolationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxOvercastTarget"));
		m_OvercastDurationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxOvercastDuration"));
		
		// Rain
		m_RainValueSetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderRainValue"));
		m_RainValueGetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderRainActualValue"));	
		m_RainValueSetText = TextWidget.Cast(root.FindAnyWidget("TextRainValueActual"));	
		
		m_RainInterpolationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderRainTarget"));	
		m_RainDurationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderRainDuration"));
		
		m_RainInterpolationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxRainTarget"));
		m_RainDurationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxRainDuration"));
		
		m_RainValueEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxRainValue"));
		m_RainInterpolationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxRainTarget"));
		m_RainDurationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxRainDuration"));
		
		// Snowfall
		m_SnowfallValueSetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderSnowfallValue"));
		m_SnowfallValueGetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderSnowfallActualValue"));
		m_SnowfallValueSetText = TextWidget.Cast(root.FindAnyWidget("TextSnowfallValueActual"));
		
		m_SnowfallInterpolationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderSnowfallTarget"));
		m_SnowfallDurationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderSnowfallDuration"));
		
		m_SnowfallInterpolationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxSnowfallTarget"));
		m_SnowfallDurationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxSnowfallDuration"));
		
		m_SnowfallValueEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxSnowfallValue"));
		m_SnowfallInterpolationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxSnowfallTarget"));
		m_SnowfallDurationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxSnowfallDuration"));
		
		//Other
		m_ResetButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonReset"));
		m_UpdateButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonUpdate"));
		m_ApplyButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonApply"));
		
		m_CopyButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonCopy"));
		m_PasteButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonPaste"));
		
		
		InitValues();
	}
	
	protected void ResetValues()
	{
		// FOG
		m_FogValueEditbox.SetText("0");
		m_FogDurationEditbox.SetText(DURATION_MAX.ToString());
		m_FogInterpolationEditbox.SetText("0");
		
		// OVERCAST
		m_OvercastValueEditbox.SetText("0");
		m_OvercastDurationEditbox.SetText(DURATION_MAX.ToString());
		m_OvercastInterpolationEditbox.SetText("0");
		
		// Rain
		m_RainValueEditbox.SetText("0");
		m_RainDurationEditbox.SetText(DURATION_MAX.ToString());
		m_RainInterpolationEditbox.SetText("0");
		
		// Snowfall
		m_SnowfallValueEditbox.SetText("0");
		m_SnowfallDurationEditbox.SetText(DURATION_MAX.ToString());
		m_SnowfallInterpolationEditbox.SetText("0");
		
		UpdateSliderValues();
		
		//FOG
		m_FogValue = 0;
		m_FogInterpolationTimeValue = 0;
		m_FogDurationTimeValue = DURATION_MAX;
		
		// OVERCAST
		m_OvercastValue = 0;
		m_OvercastInterpolationTimeValue = 0;
		m_OvercastDurationTimeValue = DURATION_MAX;
		
		//RAIN
		m_RainValue = 0;
		m_RainInterpolationTimeValue = 0;
		m_RainDurationTimeValue = DURATION_MAX;
		
		//SNOWFALL
		m_SnowfallValue = 0;
		m_SnowfallInterpolationTimeValue = 0;
		m_SnowfallDurationTimeValue = DURATION_MAX;
	}
	
	protected void InitValues(bool forceCurrent = false)
	{
		
		// FOG
		float fogVal = m_FogValue;
		if (forceCurrent || m_FogValue == -1)
		{
			fogVal = GetGame().GetWeather().GetFog().GetActual();
		}
		m_FogValueEditbox.SetText(fogVal.ToString());

		m_FogInterpolationEditbox.SetText(m_FogInterpolationTimeValue.ToString());
		
		if (m_FogDurationTimeValue == -1)
			m_FogDurationTimeValue = DURATION_MAX;
		m_FogDurationEditbox.SetText(m_FogDurationTimeValue.ToString());
		
		m_FogInterpolationMaxText.SetText(INTERPOLATION_MAX.ToString());
		m_FogDurationMaxText.SetText(DURATION_MAX.ToString());
		
		
		// OVERCAST
		float overcastVal = m_OvercastValue;
		if (forceCurrent || m_OvercastValue == -1)
		{
			overcastVal = GetGame().GetWeather().GetOvercast().GetActual();
		}
		m_OvercastValueEditbox.SetText(overcastVal.ToString());
		
		m_OvercastInterpolationEditbox.SetText(m_OvercastInterpolationTimeValue.ToString());
		
		
		if (m_OvercastDurationTimeValue == -1)
			m_OvercastDurationTimeValue = DURATION_MAX;
		m_OvercastDurationEditbox.SetText(m_OvercastDurationTimeValue.ToString());
		
		m_OvercastInterpolationMaxText.SetText(INTERPOLATION_MAX.ToString());
		m_OvercastDurationMaxText.SetText(DURATION_MAX.ToString());
		
		// RAIN
		float rainVal = m_RainValue;
		if (forceCurrent || m_RainValue == -1)
		{
			rainVal = GetGame().GetWeather().GetRain().GetActual();
		}

		m_RainValueEditbox.SetText(rainVal.ToString());
		m_RainInterpolationEditbox.SetText(m_RainInterpolationTimeValue.ToString());
		
		if (m_RainDurationTimeValue == -1)
			m_RainDurationTimeValue = DURATION_MAX;
		m_RainDurationEditbox.SetText(m_RainDurationTimeValue.ToString());
		
		m_RainInterpolationMaxText.SetText(INTERPOLATION_MAX.ToString());
		m_RainDurationMaxText.SetText(DURATION_MAX.ToString());
		
		// SNOWFALL
		float snowfallVal = m_SnowfallValue;
		if (forceCurrent || m_SnowfallValue == -1)
		{
			snowfallVal = GetGame().GetWeather().GetSnowfall().GetActual();
		}

		m_SnowfallValueEditbox.SetText(snowfallVal.ToString());
		m_SnowfallInterpolationEditbox.SetText(m_SnowfallInterpolationTimeValue.ToString());
		
		if (m_SnowfallDurationTimeValue == -1)
			m_SnowfallDurationTimeValue = DURATION_MAX;
		m_SnowfallDurationEditbox.SetText(m_SnowfallDurationTimeValue.ToString());
		
		m_SnowfallInterpolationMaxText.SetText(INTERPOLATION_MAX.ToString());
		m_SnowfallDurationMaxText.SetText(DURATION_MAX.ToString());
		
		UpdateSliderValues();
	}	
	
	protected void PasteValues(string clipboard)
	{
		TStringArray values = new TStringArray();
		clipboard.Split(" ", values);
		if (values.Count() >= 9)
		{
		// FOG
		m_FogValueEditbox.SetText(values.Get(0));
		m_FogInterpolationEditbox.SetText(values.Get(1));
		m_FogDurationEditbox.SetText(values.Get(2));
		// OVERCAST
		m_OvercastValueEditbox.SetText(values.Get(3));
		m_OvercastInterpolationEditbox.SetText(values.Get(4));
		m_OvercastDurationEditbox.SetText(values.Get(5));
		// RAIN
		m_RainValueEditbox.SetText(values.Get(6));
		m_RainInterpolationEditbox.SetText(values.Get(7));
		m_RainDurationEditbox.SetText(values.Get(8));
			
		if (values.Count() >= 12)
		{
			// SNOWFALL
			m_SnowfallValueEditbox.SetText(values.Get(9));
			m_SnowfallInterpolationEditbox.SetText(values.Get(10));
			m_SnowfallDurationEditbox.SetText(values.Get(11));
		}
			
		}		
	}	
	
	protected string CopyValues()
	{
		string output;
		
		// FOG
		output += m_FogValueEditbox.GetText();
		output += " ";
		output += m_FogInterpolationEditbox.GetText();
		output += " ";
		output += m_FogDurationEditbox.GetText();
		output += " ";
		// OVERCAST
		output += m_OvercastValueEditbox.GetText();
		output += " ";
		output += m_OvercastInterpolationEditbox.GetText();
		output += " ";
		output += m_OvercastDurationEditbox.GetText();
		output += " ";
		// RAIN
		output += m_RainValueEditbox.GetText();
		output += " ";
		output += m_RainInterpolationEditbox.GetText();
		output += " ";
		output += m_RainDurationEditbox.GetText();
		// SNOWFALL
		output += m_SnowfallValueEditbox.GetText();
		output += " ";
		output += m_SnowfallInterpolationEditbox.GetText();
		output += " ";
		output += m_SnowfallDurationEditbox.GetText();
		
		return output;
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		bool rmbDrag = (GetMouseState(MouseState.RIGHT) & MB_PRESSED_MASK);
		bool immDrag = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) && KeyState(KeyCode.KC_LSHIFT);
		
		if (rmbDrag)
		{
			// first row
			if (w == m_FogValueSetSlider)
			{
				m_OvercastValueSetSlider.SetCurrent(m_FogValueSetSlider.GetCurrent());
				m_RainValueSetSlider.SetCurrent(m_FogValueSetSlider.GetCurrent());
				m_SnowfallValueSetSlider.SetCurrent(m_FogValueSetSlider.GetCurrent());
			}
			else if (w == m_OvercastValueSetSlider)
			{
				m_FogValueSetSlider.SetCurrent(m_OvercastValueSetSlider.GetCurrent());
				m_RainValueSetSlider.SetCurrent(m_OvercastValueSetSlider.GetCurrent());
				m_SnowfallValueSetSlider.SetCurrent(m_OvercastValueSetSlider.GetCurrent());
			}			
			else if (w == m_RainValueSetSlider)
			{
				m_FogValueSetSlider.SetCurrent(m_RainValueSetSlider.GetCurrent());
				m_OvercastValueSetSlider.SetCurrent(m_RainValueSetSlider.GetCurrent());
				m_SnowfallValueSetSlider.SetCurrent(m_RainValueSetSlider.GetCurrent());
			}
			else if (w == m_SnowfallValueSetSlider)
			{
				m_FogValueSetSlider.SetCurrent(m_SnowfallValueSetSlider.GetCurrent());
				m_OvercastValueSetSlider.SetCurrent(m_SnowfallValueSetSlider.GetCurrent());
				m_RainValueSetSlider.SetCurrent(m_SnowfallValueSetSlider.GetCurrent());
			}
			// second row
			else if (w == m_FogInterpolationTimeSlider)
			{
				m_OvercastInterpolationTimeSlider.SetCurrent(m_FogInterpolationTimeSlider.GetCurrent());
				m_RainInterpolationTimeSlider.SetCurrent(m_FogInterpolationTimeSlider.GetCurrent());
				m_SnowfallInterpolationTimeSlider.SetCurrent(m_FogInterpolationTimeSlider.GetCurrent());
			}
			else if (w == m_OvercastInterpolationTimeSlider)
			{
				m_FogInterpolationTimeSlider.SetCurrent(m_OvercastInterpolationTimeSlider.GetCurrent());
				m_RainInterpolationTimeSlider.SetCurrent(m_OvercastInterpolationTimeSlider.GetCurrent());
				m_SnowfallInterpolationTimeSlider.SetCurrent(m_OvercastInterpolationTimeSlider.GetCurrent());
			}
			
			else if (w == m_RainInterpolationTimeSlider)
			{
				m_OvercastInterpolationTimeSlider.SetCurrent(m_RainInterpolationTimeSlider.GetCurrent());
				m_FogInterpolationTimeSlider.SetCurrent(m_RainInterpolationTimeSlider.GetCurrent());
				m_SnowfallInterpolationTimeSlider.SetCurrent(m_RainInterpolationTimeSlider.GetCurrent());
			}
			else if (w == m_SnowfallInterpolationTimeSlider)
			{
				m_FogInterpolationTimeSlider.SetCurrent(m_SnowfallInterpolationTimeSlider.GetCurrent());
				m_OvercastInterpolationTimeSlider.SetCurrent(m_SnowfallInterpolationTimeSlider.GetCurrent());
				m_RainInterpolationTimeSlider.SetCurrent(m_SnowfallInterpolationTimeSlider.GetCurrent());
			}
			// third row
			else if (w == m_FogDurationTimeSlider)
			{
				m_RainDurationTimeSlider.SetCurrent(m_FogDurationTimeSlider.GetCurrent());
				m_OvercastDurationTimeSlider.SetCurrent(m_FogDurationTimeSlider.GetCurrent());
				m_SnowfallDurationTimeSlider.SetCurrent(m_FogDurationTimeSlider.GetCurrent());
			}
			else if (w == m_RainDurationTimeSlider)
			{
				m_FogDurationTimeSlider.SetCurrent(m_RainDurationTimeSlider.GetCurrent());
				m_OvercastDurationTimeSlider.SetCurrent(m_RainDurationTimeSlider.GetCurrent());
				m_SnowfallDurationTimeSlider.SetCurrent(m_RainDurationTimeSlider.GetCurrent());
			}			
			else if (w == m_OvercastDurationTimeSlider)
			{
				m_RainDurationTimeSlider.SetCurrent(m_OvercastDurationTimeSlider.GetCurrent());
				m_FogDurationTimeSlider.SetCurrent(m_OvercastDurationTimeSlider.GetCurrent());
				m_SnowfallDurationTimeSlider.SetCurrent(m_OvercastDurationTimeSlider.GetCurrent());
			}
			else if (w == m_SnowfallDurationTimeSlider)
			{
				m_FogDurationTimeSlider.SetCurrent(m_SnowfallDurationTimeSlider.GetCurrent());
				m_OvercastDurationTimeSlider.SetCurrent(m_SnowfallDurationTimeSlider.GetCurrent());
				m_RainDurationTimeSlider.SetCurrent(m_SnowfallDurationTimeSlider.GetCurrent());
			}
		}

		
		SliderWidget slider = SliderWidget.Cast(w);
		if (slider)
		{
			if ( (rmbDrag||immDrag) && !m_RPCTimer.IsRunning())
			{
				if (w == m_OvercastValueSetSlider || w == m_RainValueSetSlider || w == m_FogValueSetSlider || w == m_SnowfallValueSetSlider)
					m_RPCTimer.Run(0.25, this, "InvokeSendRPC");
			}
			UpdateEditboxValues();
			return true;	
		}
		EditBoxWidget box = EditBoxWidget.Cast(w);
		if (box)
		{
			UpdateSliderValues();
			return true;	
		}
		return false;
	}
	
	void UpdateSliderValues()
	{
			//fog
			float editboxFogValue = m_FogValueEditbox.GetText().ToFloat();
			m_FogValueSetSlider.SetCurrent(editboxFogValue * 100);

			float editboxFogInterpolation = m_FogInterpolationEditbox.GetText().ToFloat();
			m_FogInterpolationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxFogInterpolation) * 100);

			float editboxFogDuration = m_FogDurationEditbox.GetText().ToFloat();
			m_FogDurationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxFogDuration) * 100);
		
			//overcast
			float editboxOvercastValue = m_OvercastValueEditbox.GetText().ToFloat();
			m_OvercastValueSetSlider.SetCurrent(editboxOvercastValue * 100);

			float editboxOvercastInterpolation = m_OvercastInterpolationEditbox.GetText().ToFloat();
			m_OvercastInterpolationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxOvercastInterpolation) * 100);

			float editboxOvercastDuration = m_OvercastDurationEditbox.GetText().ToFloat();
			m_OvercastDurationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxOvercastDuration) * 100);
		
			//rain
			float editboxRainValue = m_RainValueEditbox.GetText().ToFloat();
			m_RainValueSetSlider.SetCurrent(editboxRainValue * 100);

			float editboxRainInterpolation = m_RainInterpolationEditbox.GetText().ToFloat();
			m_RainInterpolationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxRainInterpolation) * 100);

			float editboxRainDuration = m_RainDurationEditbox.GetText().ToFloat();
			m_RainDurationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxRainDuration) * 100);
		
			//snowfall
			float editboxSnowfallValue = m_SnowfallValueEditbox.GetText().ToFloat();
			m_SnowfallValueSetSlider.SetCurrent(editboxSnowfallValue * 100);

			float editboxSnowfallInterpolation = m_SnowfallInterpolationEditbox.GetText().ToFloat();
			m_SnowfallInterpolationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxSnowfallInterpolation) * 100);

			float editboxSnowfallDuration = m_SnowfallDurationEditbox.GetText().ToFloat();
			m_SnowfallDurationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxSnowfallDuration) * 100);
	}
	
	void UpdateEditboxValues()
	{
			//fog
			m_FogInterpolationTimeValue = m_FogInterpolationTimeSlider.GetCurrent() / 100 * INTERPOLATION_MAX;
			m_FogInterpolationEditbox.SetText(m_FogInterpolationTimeValue.ToString());

			m_FogDurationTimeValue = m_FogDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
			m_FogDurationEditbox.SetText(m_FogDurationTimeValue.ToString());

			m_FogValue = m_FogValueSetSlider.GetCurrent() / 100;
			m_FogValueEditbox.SetText(m_FogValue.ToString());

			//overcast
			m_OvercastInterpolationTimeValue = m_OvercastInterpolationTimeSlider.GetCurrent() / 100 * INTERPOLATION_MAX;
			m_OvercastInterpolationEditbox.SetText(m_OvercastInterpolationTimeValue.ToString());

			m_OvercastDurationTimeValue = m_OvercastDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
			m_OvercastDurationEditbox.SetText(m_OvercastDurationTimeValue.ToString());

			m_OvercastValue = m_OvercastValueSetSlider.GetCurrent() / 100;
			m_OvercastValueEditbox.SetText(m_OvercastValue.ToString());
			
			//rain
			m_RainInterpolationTimeValue = m_RainInterpolationTimeSlider.GetCurrent() / 100 * INTERPOLATION_MAX;
			m_RainInterpolationEditbox.SetText(m_RainInterpolationTimeValue.ToString());

			m_RainDurationTimeValue = m_RainDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
			m_RainDurationEditbox.SetText(m_RainDurationTimeValue.ToString());

			m_RainValue = m_RainValueSetSlider.GetCurrent() / 100;
			m_RainValueEditbox.SetText(m_RainValue.ToString());
		
			//snowfall
			m_SnowfallInterpolationTimeValue = m_SnowfallInterpolationTimeSlider.GetCurrent() / 100 * INTERPOLATION_MAX;
			m_SnowfallInterpolationEditbox.SetText(m_SnowfallInterpolationTimeValue.ToString());

			m_SnowfallDurationTimeValue = m_SnowfallDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
			m_SnowfallDurationEditbox.SetText(m_SnowfallDurationTimeValue.ToString());

			m_SnowfallValue = m_SnowfallValueSetSlider.GetCurrent() / 100;
			m_SnowfallValueEditbox.SetText(m_SnowfallValue.ToString());
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_ApplyButton)
		{
			SendRPC(GetRPCDataNormal());
			return true;
		}
		else if (w == m_UpdateButton)
		{
			InitValues(true);
			return true;
		}
		else if (w == m_ResetButton)
		{
			ResetValues();
			InitValues(true);
			return true;
		}
		else if (w == m_CopyButton)
		{
			string output = CopyValues();
			GetGame().CopyToClipboard(output);
			return true;
		}
		
		else if (w == m_PasteButton)
		{
			string input;
			GetGame().CopyFromClipboard(input);
			PasteValues(input);
			UpdateSliderValues();
			return true;
		}
		
		return false;
	}
	
	
	
	//RPC data where each value is strickly as shown in the UI on the sliders
	protected DebugWeatherRPCData GetRPCDataNormal()
	{
		DebugWeatherRPCData data = new DebugWeatherRPCData();
		// FOG
		data.m_FogValue = m_FogValueSetSlider.GetCurrent() / 100;
		data.m_FogInterpolation = m_FogInterpolationTimeValue;
		data.m_FogDuration = m_FogDurationTimeSlider.GetCurrent() / 100 * 960;
		
		// Rain
		data.m_RainValue = m_RainValueSetSlider.GetCurrent() / 100;
		data.m_RainInterpolation = m_RainInterpolationTimeValue;
		data.m_RainDuration = m_RainDurationTimeSlider.GetCurrent() / 100 * 960;
		
		// OVERCAST
		data.m_OvercastValue = m_OvercastValueSetSlider.GetCurrent() / 100;
		data.m_OvercastInterpolation = m_OvercastInterpolationTimeValue;
		data.m_OvercastDuration = m_OvercastDurationTimeSlider.GetCurrent() / 100 * 960;
		
		// SNOWFALL
		data.m_SnowfallValue = m_SnowfallValueSetSlider.GetCurrent() / 100;
		data.m_SnowfallInterpolation = m_SnowfallInterpolationTimeValue;
		data.m_SnowfallDuration = m_SnowfallDurationTimeSlider.GetCurrent() / 100 * 960;
		
		return data;
	}
	
	//RPC data where the duration and interpolation times are forced(their slider values are ignored) to allow for immediate change and lasting effect
	protected DebugWeatherRPCData GetRPCDataImmediate()
	{
		DebugWeatherRPCData data = new DebugWeatherRPCData();
		// FOG
		data.m_FogValue = m_FogValueSetSlider.GetCurrent() / 100;
		data.m_FogInterpolation = 0;
		data.m_FogDuration = 960;
		
		// Rain
		data.m_RainValue = m_RainValueSetSlider.GetCurrent() / 100;
		data.m_RainInterpolation = 0;
		data.m_RainDuration = 960;
		
		// OVERCAST
		data.m_OvercastValue = m_OvercastValueSetSlider.GetCurrent() / 100;
		data.m_OvercastInterpolation = 0;
		data.m_OvercastDuration = 960;
		
		// SNOWFALL
		data.m_SnowfallValue = m_SnowfallValueSetSlider.GetCurrent() / 100;
		data.m_SnowfallInterpolation =0;
		data.m_SnowfallDuration = 960;
		
		return data;
	}
	
	protected void SendRPC(DebugWeatherRPCData data)
	{
		PlayerIdentity identity = null;
		if (GetGame().GetPlayer())
		{
			identity = GetGame().GetPlayer().GetIdentity();
		}
		
		GetGame().RPCSingleParam(null, ERPCs.DEV_SET_WEATHER, new Param1<DebugWeatherRPCData>(data), true, identity);
	}
	
	protected void InvokeSendRPC()
	{
		SendRPC(GetRPCDataImmediate());
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		float overcast = GetGame().GetWeather().GetOvercast().GetActual();
		m_OvercastValueGetSlider.SetCurrent(overcast * 100);
		m_OvercastValueSetText.SetText(overcast.ToString());
		
		float fog = GetGame().GetWeather().GetFog().GetActual();
		m_FogValueGetSlider.SetCurrent(fog * 100);
		m_FogValueSetText.SetText(fog.ToString());
		
		float rain = GetGame().GetWeather().GetRain().GetActual();
		m_RainValueGetSlider.SetCurrent(rain * 100);
		m_RainValueSetText.SetText(rain.ToString());
		
		float snowfall = GetGame().GetWeather().GetSnowfall().GetActual();
		m_SnowfallValueGetSlider.SetCurrent(snowfall * 100);
		m_SnowfallValueSetText.SetText(snowfall.ToString());
	}
}
