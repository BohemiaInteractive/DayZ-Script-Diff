class ScriptConsoleWeatherValues
{
	static void SetDefaultValues()
	{
		
	}
}

class ScriptConsoleWeatherTab : ScriptConsoleTabBase
{
	protected ref Timer m_RPCTimer = new Timer();
	protected const int INTERPOLATION_MAX = 920;
	protected const int DURATION_MAX = 960;
	
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
	
	// Vol Fog
	protected EditBoxWidget m_VolFogDistanceDensityEditbox;
	protected SliderWidget m_VolFogDistanceDensitySlider;
	protected TextWidget m_VolFogDistanceDensityMinText;
	protected TextWidget m_VolFogDistanceDensityMaxText;
	protected TextWidget m_VolFogDistanceDensitySetText;
	
	protected EditBoxWidget m_VolFogDistanceDensityTimeEditbox;
	protected SliderWidget m_VolFogDistanceDensityTimeSlider;
	protected TextWidget m_VolFogDistanceDensityTimeMinText;
	protected TextWidget m_VolFogDistanceDensityTimeMaxText;
	protected TextWidget m_VolFogDistanceDensityTimeSetText;
	
	protected EditBoxWidget m_VolFogHeightDensityEditbox;
	protected SliderWidget m_VolFogHeightDensitySlider;
	protected TextWidget m_VolFogHeightDensityMinText;
	protected TextWidget m_VolFogHeightDensityMaxText;
	protected TextWidget m_VolFogHeightDensitySetText;
	
	protected EditBoxWidget m_VolFogHeightDensityTimeEditbox;
	protected SliderWidget m_VolFogHeightDensityTimeSlider;
	protected TextWidget m_VolFogHeightDensityTimeMinText;
	protected TextWidget m_VolFogHeightDensityTimeMaxText;
	protected TextWidget m_VolFogHeightDensityTimeSetText;
	
	protected EditBoxWidget m_VolFogHeightBiasEditbox;
	protected SliderWidget m_VolFogHeightBiasSlider;
	protected TextWidget m_VolFogHeightBiasMinText;
	protected TextWidget m_VolFogHeightBiasMaxText;
	protected TextWidget m_VolFogHeightBiasSetText;
	
	protected EditBoxWidget m_VolFogHeightBiasTimeEditbox;
	protected SliderWidget m_VolFogHeightBiasTimeSlider;
	protected TextWidget m_VolFogHeightBiasTimeMinText;
	protected TextWidget m_VolFogHeightBiasTimeMaxText;
	protected TextWidget m_VolFogHeightBiasTimeSetText;
	
	protected static float m_VolFogDistanceDensityValue = -1;
	protected static float m_VolFogDistanceDensityTimeValue;
	protected static float m_VolFogHeightDensityValue = -1;
	protected static float m_VolFogHeightDensityTimeValue;
	protected static float m_VolFogHeightBiasValue = -1;
	protected static float m_VolFogHeightBiasTimeValue;
	
	// Wind Magnitude
	protected SliderWidget m_WindMValueSetSlider;
	protected SliderWidget m_WindMValueGetSlider;
	
	protected TextWidget m_WindMValueSetText;

	protected SliderWidget m_WindMInterpolationTimeSlider;
	protected SliderWidget m_WindMDurationTimeSlider;

	protected TextWidget m_WindMInterpolationMaxText;
	protected TextWidget m_WindMDurationMaxText;
	
	protected EditBoxWidget m_WindMValueEditbox;
	protected EditBoxWidget m_WindMInterpolationEditbox;
	protected EditBoxWidget m_WindMDurationEditbox;
	
	protected static float m_WindMagnitudeValue = -1;
	protected static float m_WindMInterpolationTimeValue;
	protected static float m_WindMDurationTimeValue = -1;
	
	// Wind Direction
	protected SliderWidget m_WindDValueSetSlider;
	protected SliderWidget m_WindDValueGetSlider;
	
	protected TextWidget m_WindDValueSetText;

	protected SliderWidget m_WindDInterpolationTimeSlider;
	protected SliderWidget m_WindDDurationTimeSlider;

	protected TextWidget m_WindDInterpolationMaxText;
	protected TextWidget m_WindDDurationMaxText;
	
	protected EditBoxWidget m_WindDValueEditbox;
	protected EditBoxWidget m_WindDInterpolationEditbox;
	protected EditBoxWidget m_WindDDurationEditbox;
	
	protected static float m_WindDirectionValue = -1;
	protected static float m_WindDInterpolationTimeValue;
	protected static float m_WindDDurationTimeValue = -1;
	
	// Other
	protected ButtonWidget m_HideUIButton;
	protected ImageWidget m_HideUIIcon;
	
	protected ButtonWidget m_LoadPresetButton;
	protected ButtonWidget m_DeletePresetButton;
	protected ButtonWidget m_SavePresetButton;

	protected ButtonWidget m_CopyButton;
	protected ButtonWidget m_PasteButton;
	protected ButtonWidget m_ResetButton;
	protected ButtonWidget m_UpdateButton;
	protected ButtonWidget m_ApplyButton;
	
	protected Widget m_WeatherPresetsPanel;
	protected EditBoxWidget m_WeatherPresetEditbox;
	
	protected bool m_UIVisibilityState = true;
	protected ScriptConsoleWeatherPreset m_SelectedWeatherPreset;
	protected ref ScriptConsolePresetsList m_WeatherPresetsList;
	protected bool m_MouseMovementEnabled;
	protected bool m_IsCAPSPressed;
	protected bool m_CanUseMovement;
	protected bool m_IsSavingPreset;

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
		
		// Vol Fog
		m_VolFogDistanceDensityValue = m_VolFogDistanceDensitySlider.GetCurrent() / 100;
		m_VolFogDistanceDensityTimeValue = m_VolFogDistanceDensityTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		m_VolFogHeightDensityValue = m_VolFogHeightDensitySlider.GetCurrent() / 100;
		m_VolFogHeightDensityTimeValue = m_VolFogHeightDensityTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		m_VolFogHeightBiasValue = m_VolFogHeightDensitySlider.GetCurrent();
		m_VolFogHeightBiasTimeValue = m_VolFogHeightDensityTimeSlider.GetCurrent() / 100 * DURATION_MAX;
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
		
		// Vol Fog
		m_VolFogDistanceDensityEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxVolfogDDValue"));
		m_VolFogDistanceDensitySlider = SliderWidget.Cast(root.FindAnyWidget("SliderVolfogDD"));
		m_VolFogDistanceDensitySlider.SetMinMax(0, 1);
		m_VolFogDistanceDensitySlider.SetStep(0.01);
		m_VolFogDistanceDensityMinText = TextWidget.Cast(root.FindAnyWidget("TextMinVolfogDDValue"));
		m_VolFogDistanceDensityMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxVolfogDDValue"));
		m_VolFogDistanceDensitySetText = TextWidget.Cast(root.FindAnyWidget("TextVolfogDDValueActual"));
		
		m_VolFogDistanceDensityTimeEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxVolfogDDTValue"));
		m_VolFogDistanceDensityTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderVolfogDDTime"));
		m_VolFogDistanceDensityTimeMinText = TextWidget.Cast(root.FindAnyWidget("TextMinVolfogDDTValue"));
		m_VolFogDistanceDensityTimeMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxVolfogDDTValue"));
		m_VolFogDistanceDensityTimeSetText = TextWidget.Cast(root.FindAnyWidget("TextVolfogDDTValueActual"));
		
		m_VolFogHeightDensityEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxVolfogHDValue"));
		m_VolFogHeightDensitySlider = SliderWidget.Cast(root.FindAnyWidget("SliderVolfogHD"));
		m_VolFogHeightDensitySlider.SetMinMax(0, 1);
		m_VolFogHeightDensitySlider.SetStep(0.01);
		m_VolFogHeightDensityMinText = TextWidget.Cast(root.FindAnyWidget("TextMinVolfogHDValue"));
		m_VolFogHeightDensityMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxVolfogHDValue"));
		m_VolFogHeightDensitySetText = TextWidget.Cast(root.FindAnyWidget("TextVolfogHDValueActual"));
		
		m_VolFogHeightDensityTimeEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxVolfogHDTValue"));
		m_VolFogHeightDensityTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderVolfogHDTime"));
		m_VolFogHeightDensityTimeMinText = TextWidget.Cast(root.FindAnyWidget("TextMinVolfogHDTValue"));
		m_VolFogHeightDensityTimeMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxVolfogHDTValue"));
		m_VolFogHeightDensityTimeSetText = TextWidget.Cast(root.FindAnyWidget("TextVolfogHDTValueActual"));
		
		m_VolFogHeightBiasEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxVolfogHBValue"));
		m_VolFogHeightBiasSlider = SliderWidget.Cast(root.FindAnyWidget("SliderVolfogHB"));
		m_VolFogHeightBiasSlider.SetMinMax(-500, 500);
		m_VolFogHeightBiasMinText = TextWidget.Cast(root.FindAnyWidget("TextMinVolfogHBValue"));
		m_VolFogHeightBiasMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxVolfogHBValue"));
		m_VolFogHeightBiasSetText = TextWidget.Cast(root.FindAnyWidget("TextVolfogHBValueActual"));
		
		m_VolFogHeightBiasTimeEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxVolfogHBTValue"));
		m_VolFogHeightBiasTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderVolfogHBTime"));
		m_VolFogHeightBiasTimeMinText = TextWidget.Cast(root.FindAnyWidget("TextMinVolfogHBTValue"));
		m_VolFogHeightBiasTimeMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxVolfogHbTValue"));
		m_VolFogHeightBiasTimeSetText = TextWidget.Cast(root.FindAnyWidget("TextVolfogHBTValueActual"));
		
		// Wind Magnitude
		m_WindMValueSetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderWindMValue"));
		m_WindMValueSetSlider.SetMinMax(0, 20);
		m_WindMValueSetSlider.SetStep(0.1);
		m_WindMValueGetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderWindMActualValue"));
		m_WindMValueGetSlider.SetMinMax(0, 20);
		m_WindMValueGetSlider.SetStep(0.1);

		m_WindMValueSetText = TextWidget.Cast(root.FindAnyWidget("TextWindMValueActual"));
	
		m_WindMInterpolationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderWindMTarget"));
		m_WindMDurationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderWindMDuration"));
	
		m_WindMInterpolationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxWindMTarget"));
		m_WindMDurationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxWindMDuration"));
		
		m_WindMValueEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxWindMValue"));
		m_WindMInterpolationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxWindMTarget"));
		m_WindMDurationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxWindMDuration"));
		
		// Wind Direction
		m_WindDValueSetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderWindDValue"));
		m_WindDValueSetSlider.SetMinMax(-3.14, 3.14);
		m_WindDValueSetSlider.SetStep(0.01);

		m_WindDValueGetSlider = SliderWidget.Cast(root.FindAnyWidget("SliderWindDActualValue"));
		m_WindDValueGetSlider.SetMinMax(-3.14, 3.14);
		m_WindDValueGetSlider.SetStep(0.01);

		m_WindDValueSetText = TextWidget.Cast(root.FindAnyWidget("TextWindDValueActual"));
	
		m_WindDInterpolationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderWindDTarget"));
		m_WindDDurationTimeSlider = SliderWidget.Cast(root.FindAnyWidget("SliderWindDDuration"));
	
		m_WindDInterpolationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxWindDTarget"));
		m_WindDDurationMaxText = TextWidget.Cast(root.FindAnyWidget("TextMaxWindDDuration"));
		
		m_WindDValueEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxWindDValue"));
		m_WindDInterpolationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxWindDTarget"));
		m_WindDDurationEditbox = EditBoxWidget.Cast(root.FindAnyWidget("EditBoxWindDDuration"));
		
		//Other
		m_HideUIButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonHide"));
		m_HideUIIcon = ImageWidget.Cast(root.FindAnyWidget("ButtonHideIcon"));
		m_HideUIIcon.LoadImageFile(0, "set:dayz_gui image:PasswordEyeOpen");
		m_HideUIIcon.LoadImageFile(1, "set:dayz_gui image:PasswordEyeClose");
		m_HideUIIcon.SetImage(1);
	
		m_CopyButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonCopy"));
		m_PasteButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonPaste"));
		m_ResetButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonReset"));
		m_UpdateButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonUpdate"));
		m_ApplyButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonApply"));

		m_LoadPresetButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonLoadPreset"));
		m_DeletePresetButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonDeletePreset"));
		m_SavePresetButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonSavePreset")); 
		m_WeatherPresetEditbox = EditBoxWidget.Cast(root.FindAnyWidget("WeatherPresetName"));
		
		m_WeatherPresetsPanel = root.FindAnyWidget("WeatherPresetsPanel");
		m_WeatherPresetsList = new ScriptConsolePresetsList(m_WeatherPresetsPanel, this);
		m_WeatherPresetsList.m_ItemSelected.Insert(OnListItemSelected);
		
		InitValues();
		ReloadWeatherPresetsList();
	}
	
	protected void ReloadWeatherPresetsList()
	{
		m_WeatherPresetsList.ClearList();
		
		for (int i = 0; i < m_ConfigDebugProfile.GetWeatherPresets().Count(); i++) 
		{
			ScriptConsoleWeatherPreset preset = m_ConfigDebugProfile.GetWeatherPresets()[i];
			if (!preset)
			{
				ErrorEx("Could not load DOF preset entry!");
				continue;
			}

			m_WeatherPresetsList.AddListItem(preset.Name, 0, i);
		}
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
		
		// Vol Fog
		m_VolFogDistanceDensityEditbox.SetText("0");
		m_VolFogDistanceDensityTimeEditbox.SetText(DURATION_MAX.ToString());
		
		m_VolFogHeightDensityEditbox.SetText("0");
		m_VolFogHeightDensityTimeEditbox.SetText(DURATION_MAX.ToString());
		
		m_VolFogHeightBiasEditbox.SetText("0");
		m_VolFogHeightBiasTimeEditbox.SetText(DURATION_MAX.ToString());
		
		// Wind Magnitude
		m_WindMValueEditbox.SetText("0");
		m_WindMDurationEditbox.SetText(DURATION_MAX.ToString());
		m_WindMInterpolationEditbox.SetText("0");
		
		// Wind Direction
		m_WindDValueEditbox.SetText("0");
		m_WindDDurationEditbox.SetText(DURATION_MAX.ToString());
		m_WindDInterpolationEditbox.SetText("0");
		
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
		
		// Vol Fog
		m_VolFogDistanceDensityValue = 0;
		m_VolFogDistanceDensityTimeValue = DURATION_MAX;
		
		m_VolFogHeightDensityValue = 0;
		m_VolFogHeightDensityTimeValue = DURATION_MAX;
		
		m_VolFogHeightBiasValue = 0;
		m_VolFogHeightBiasTimeValue = DURATION_MAX;
		
		// Wind Magnitude
		m_WindMagnitudeValue = 0;
		m_WindMInterpolationTimeValue = 0;
		m_WindMDurationTimeValue = DURATION_MAX;
		
		// Wind Direction
		m_WindDirectionValue = 0;
		m_WindDInterpolationTimeValue = 0;
		m_WindDDurationTimeValue = DURATION_MAX;
	}
	
	protected void InitValues(bool forceCurrent = false)
	{
		// FOG
		float fogVal = m_FogValue;
		if (forceCurrent || m_FogValue == -1)
		{
			fogVal = g_Game.GetWeather().GetFog().GetActual();
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
			overcastVal = g_Game.GetWeather().GetOvercast().GetActual();
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
			rainVal = g_Game.GetWeather().GetRain().GetActual();
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
			snowfallVal = g_Game.GetWeather().GetSnowfall().GetActual();
		}

		m_SnowfallValueEditbox.SetText(snowfallVal.ToString());
		m_SnowfallInterpolationEditbox.SetText(m_SnowfallInterpolationTimeValue.ToString());
		
		if (m_SnowfallDurationTimeValue == -1)
			m_SnowfallDurationTimeValue = DURATION_MAX;
		m_SnowfallDurationEditbox.SetText(m_SnowfallDurationTimeValue.ToString());
		
		m_SnowfallInterpolationMaxText.SetText(INTERPOLATION_MAX.ToString());
		m_SnowfallDurationMaxText.SetText(DURATION_MAX.ToString());
		
		// VOLFOG
		float volFogDistanceDensityValue = m_VolFogDistanceDensityValue;
		if (forceCurrent || m_VolFogDistanceDensityValue == -1)
		{
			volFogDistanceDensityValue = g_Game.GetWeather().GetDynVolFogDistanceDensity();
		}

		m_VolFogDistanceDensityEditbox.SetText(volFogDistanceDensityValue.ToString());
				
		m_VolFogDistanceDensityTimeValue = 0;
		m_VolFogDistanceDensityTimeEditbox.SetText(m_VolFogDistanceDensityTimeValue.ToString());
		
		float volFogHeightDensityValue = m_VolFogHeightDensityValue;
		if (forceCurrent || m_VolFogHeightDensityValue == -1)
		{
			volFogHeightDensityValue = g_Game.GetWeather().GetDynVolFogHeightDensity();
		}
		
		m_VolFogHeightDensityEditbox.SetText(volFogHeightDensityValue.ToString());
		
		m_VolFogHeightDensityTimeValue = 0;
		m_VolFogHeightDensityTimeEditbox.SetText(m_VolFogHeightDensityTimeValue.ToString());
		
		float volFogHeightBiasValue = m_VolFogHeightBiasValue;
		if (forceCurrent /*|| m_VolFogHeightBiasValue == -500*/)
		{
			volFogHeightBiasValue = g_Game.GetWeather().GetDynVolFogHeightBias();
		}

		m_VolFogHeightBiasEditbox.SetText(volFogHeightBiasValue.ToString());
		
		m_VolFogHeightBiasTimeValue = 0;
		m_VolFogHeightBiasTimeEditbox.SetText(m_VolFogHeightBiasTimeValue.ToString());
		
		// Wind Magnitude
		float windMagnitudeVal = m_WindMagnitudeValue;
		if (forceCurrent || m_WindMagnitudeValue == -1)
		{
			windMagnitudeVal = g_Game.GetWeather().GetWindMagnitude().GetActual();
		}

		m_WindMValueEditbox.SetText(windMagnitudeVal.ToString());
		m_WindMInterpolationEditbox.SetText(m_WindMInterpolationTimeValue.ToString());
		
		if (m_WindMDurationTimeValue == -1)
			m_WindMDurationTimeValue = DURATION_MAX;
		m_WindMDurationEditbox.SetText(m_WindMDurationTimeValue.ToString());
		
		m_WindMInterpolationMaxText.SetText(INTERPOLATION_MAX.ToString());
		m_WindMDurationMaxText.SetText(DURATION_MAX.ToString());
		
		// Wind Direction
		float windDirectionVal = m_WindDirectionValue;
		if (forceCurrent || m_WindDirectionValue == -1)
		{
			windDirectionVal = g_Game.GetWeather().GetWindDirection().GetActual();
		}

		m_WindDValueEditbox.SetText(windDirectionVal.ToString());
		m_WindDInterpolationEditbox.SetText(m_WindDInterpolationTimeValue.ToString());
		
		if (m_WindDDurationTimeValue == -1)
			m_WindDDurationTimeValue = DURATION_MAX;
		m_WindDDurationEditbox.SetText(m_WindDDurationTimeValue.ToString());
		
		m_WindDInterpolationMaxText.SetText(INTERPOLATION_MAX.ToString());
		m_WindDDurationMaxText.SetText(DURATION_MAX.ToString());

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
			
			if (values.Count() >= 18)
			{
				// VOL FOG
				m_VolFogDistanceDensityEditbox.SetText(values.Get(12));
				m_VolFogDistanceDensityTimeEditbox.SetText(values.Get(13));
				m_VolFogHeightDensityEditbox.SetText(values.Get(14));
				m_VolFogHeightDensityTimeEditbox.SetText(values.Get(15));
				m_VolFogHeightBiasEditbox.SetText(values.Get(16));
				m_VolFogHeightBiasTimeEditbox.SetText(values.Get(17));
			}
			
			if (values.Count() >= 24)
			{
				// WIND MAGNITUDE
				m_WindMValueEditbox.SetText(values.Get(18));
				m_WindMInterpolationEditbox.SetText(values.Get(19));
				m_WindMDurationEditbox.SetText(values.Get(20));
				// WIND DIRECTION
				m_WindDValueEditbox.SetText(values.Get(21));
				m_WindDInterpolationEditbox.SetText(values.Get(22));
				m_WindDDurationEditbox.SetText(values.Get(23));
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
		output += " ";
		// SNOWFALL
		output += m_SnowfallValueEditbox.GetText();
		output += " ";
		output += m_SnowfallInterpolationEditbox.GetText();
		output += " ";
		output += m_SnowfallDurationEditbox.GetText();
		output += " ";
		// VOLFOG
		output += m_VolFogDistanceDensityEditbox.GetText();
		output += " ";
		output += m_VolFogDistanceDensityTimeEditbox.GetText();
		output += " ";
		output += m_VolFogHeightDensityEditbox.GetText();
		output += " ";
		output += m_VolFogHeightDensityTimeEditbox.GetText();
		output += " ";
		output += m_VolFogHeightBiasEditbox.GetText();
		output += " ";
		output += m_VolFogHeightBiasTimeEditbox.GetText();
		output += " ";
		// WIND MAGNITUDE
		output += m_WindMValueEditbox.GetText();
		output += " ";
		output += m_WindMInterpolationEditbox.GetText();
		output += " ";
		output += m_WindMDurationEditbox.GetText();
		output += " ";
		// WIND DIRECTION
		output += m_WindDValueEditbox.GetText();
		output += " ";
		output += m_WindDInterpolationEditbox.GetText();
		output += " ";
		output += m_WindDDurationEditbox.GetText();

		return output;
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		bool rmbDrag = (GetMouseState(MouseState.RIGHT) & MB_PRESSED_MASK);
		bool immDrag = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) && KeyState(KeyCode.KC_LSHIFT);
		
		if (rmbDrag)
		{
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
			if ((rmbDrag||immDrag) && !m_RPCTimer.IsRunning())
			{
				if (w == m_OvercastValueSetSlider || w == m_RainValueSetSlider || w == m_FogValueSetSlider || w == m_SnowfallValueSetSlider || w == m_VolFogDistanceDensitySlider || w == m_VolFogHeightDensitySlider || w == m_VolFogHeightBiasSlider || w == m_WindMValueSetSlider || w == m_WindDValueSetSlider)
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
		
		// vol fog
		float editboxVolFogDistanceDensity = m_VolFogDistanceDensityEditbox.GetText().ToFloat();
		m_VolFogDistanceDensitySlider.SetCurrent(editboxVolFogDistanceDensity);
		
		float editboxVolFogDistanceDensityTime = m_VolFogDistanceDensityTimeEditbox.GetText().ToFloat();
		m_VolFogDistanceDensityTimeSlider.SetCurrent(Math.InverseLerp(0,DURATION_MAX,editboxVolFogDistanceDensityTime) * 100);
		
		float editboxVolFogHeightDensity = m_VolFogHeightDensityEditbox.GetText().ToFloat();
		m_VolFogHeightDensitySlider.SetCurrent(editboxVolFogHeightDensity);
		
		float editboxVolFogHeightDensityTime = m_VolFogHeightDensityTimeEditbox.GetText().ToFloat();
		m_VolFogHeightDensityTimeSlider.SetCurrent(Math.InverseLerp(0,DURATION_MAX,editboxVolFogHeightDensityTime) * 100);
		
		float editboxVolFogHeightBias = m_VolFogHeightBiasEditbox.GetText().ToFloat();
		m_VolFogHeightBiasSlider.SetCurrent(editboxVolFogHeightBias);
		
		float editboxVolFogHeightBiasTime = m_VolFogHeightBiasTimeEditbox.GetText().ToFloat();
		m_VolFogHeightBiasTimeSlider.SetCurrent(Math.InverseLerp(0,DURATION_MAX,editboxVolFogHeightBiasTime) * 100);
		
		// wind magnitude
		float editboxWindMValue = m_WindMValueEditbox.GetText().ToFloat();
		m_WindMValueSetSlider.SetCurrent(editboxWindMValue);

		float editboxWindMInterpolation = m_WindMInterpolationEditbox.GetText().ToFloat();
		m_WindMInterpolationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxWindMInterpolation) * 100);

		float editboxWindMDuration = m_WindMDurationEditbox.GetText().ToFloat();
		m_WindMDurationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxWindMDuration) * 100);
		
		// wind direction
		float editboxWindDValue = m_WindDValueEditbox.GetText().ToFloat();
		m_WindDValueSetSlider.SetCurrent(editboxWindDValue);

		float editboxWindDInterpolation = m_WindDInterpolationEditbox.GetText().ToFloat();
		m_WindDInterpolationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxWindDInterpolation) * 100);

		float editboxWindDDuration = m_WindDDurationEditbox.GetText().ToFloat();
		m_WindDDurationTimeSlider.SetCurrent(Math.InverseLerp(0,INTERPOLATION_MAX,editboxWindDDuration) * 100);
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
		
		// vol fog
		m_VolFogDistanceDensityValue = m_VolFogDistanceDensitySlider.GetCurrent();
		m_VolFogDistanceDensityEditbox.SetText(m_VolFogDistanceDensityValue.ToString());
		
		m_VolFogDistanceDensityTimeValue = m_VolFogDistanceDensityTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		m_VolFogDistanceDensityTimeEditbox.SetText(m_VolFogDistanceDensityTimeValue.ToString());
		
		m_VolFogHeightDensityValue = m_VolFogHeightDensitySlider.GetCurrent();
		m_VolFogHeightDensityEditbox.SetText(m_VolFogHeightDensityValue.ToString());
		
		m_VolFogHeightDensityTimeValue = m_VolFogHeightDensityTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		m_VolFogHeightDensityTimeEditbox.SetText(m_VolFogHeightDensityTimeValue.ToString());
		
		m_VolFogHeightBiasValue = m_VolFogHeightBiasSlider.GetCurrent();
		m_VolFogHeightBiasEditbox.SetText(m_VolFogHeightBiasValue.ToString());
		
		m_VolFogHeightBiasTimeValue = m_VolFogHeightBiasTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		m_VolFogHeightBiasTimeEditbox.SetText(m_VolFogHeightBiasTimeValue.ToString());
		
		//wind magnitude
		m_WindMInterpolationTimeValue = m_WindMInterpolationTimeSlider.GetCurrent() / 100 * INTERPOLATION_MAX;
		m_WindMInterpolationEditbox.SetText(m_WindMInterpolationTimeValue.ToString());

		m_WindMDurationTimeValue = m_WindMDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		m_WindMDurationEditbox.SetText(m_WindMDurationTimeValue.ToString());

		m_WindMagnitudeValue = m_WindMValueSetSlider.GetCurrent();
		m_WindMValueEditbox.SetText(m_WindMagnitudeValue.ToString());
		
		//wind direction
		m_WindDInterpolationTimeValue = m_WindDInterpolationTimeSlider.GetCurrent() / 100 * INTERPOLATION_MAX;
		m_WindDInterpolationEditbox.SetText(m_WindDInterpolationTimeValue.ToString());

		m_WindDDurationTimeValue = m_WindDDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		m_WindDDurationEditbox.SetText(m_WindDDurationTimeValue.ToString());

		m_WindDirectionValue = m_WindDValueSetSlider.GetCurrent();
		m_WindDValueEditbox.SetText(m_WindDirectionValue.ToString());
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_HideUIButton)
		{
			ToogleUIVisibility();
		}
		else if (w == m_ApplyButton)
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
			g_Game.CopyToClipboard(output);
			return true;
		}
		
		else if (w == m_PasteButton)
		{
			string input;
			g_Game.CopyFromClipboard(input);
			PasteValues(input);
			UpdateSliderValues();
			return true;
		}
		else if (w == m_LoadPresetButton)
		{
			LoadWeatherPreset();
			return true;
		}
		else if (w == m_SavePresetButton)
		{
			SaveWeatherPreset();
			return true;
		}
		else if (w == m_DeletePresetButton)
		{
			DeleteWeatherPreset();
			return true;
		}
		
		return false;
	}
	
	protected void SaveWeatherPreset()
	{
		if (m_IsSavingPreset || m_WeatherPresetEditbox.GetText() == "")
			return;
		
		m_IsSavingPreset = true;
		m_ConfigDebugProfile.AddWeatherPreset(m_WeatherPresetEditbox.GetText(), m_OvercastValueEditbox.GetText().ToFloat(), m_RainValueEditbox.GetText().ToFloat(), m_SnowfallValueEditbox.GetText().ToFloat(), m_FogValueEditbox.GetText().ToFloat(), m_WindMValueEditbox.GetText().ToFloat(), m_WindDValueEditbox.GetText().ToFloat(), m_VolFogDistanceDensityEditbox.GetText().ToFloat(), m_VolFogHeightDensityEditbox.GetText().ToFloat(), m_VolFogHeightBiasEditbox.GetText().ToFloat());
		ReloadWeatherPresetsList();
		m_IsSavingPreset = false;
	}
	
	protected void DeleteWeatherPreset()
	{
		if (!m_SelectedWeatherPreset)
			return;
		
		m_ConfigDebugProfile.RemoveWeatherPreset(m_SelectedWeatherPreset.Name);
		ReloadWeatherPresetsList();
	}
	
	protected void LoadWeatherPreset()
	{
		if (!m_SelectedWeatherPreset)
			return;

		m_OvercastValueSetSlider.SetCurrent(m_SelectedWeatherPreset.WOvercast * 100);
		m_RainValueSetSlider.SetCurrent(m_SelectedWeatherPreset.WRain * 100);
		m_SnowfallValueSetSlider.SetCurrent(m_SelectedWeatherPreset.WSnow * 100);
		m_FogValueSetSlider.SetCurrent(m_SelectedWeatherPreset.WFog * 100);
		m_WindMValueSetSlider.SetCurrent(m_SelectedWeatherPreset.WWindMagnitude);
		m_WindDValueSetSlider.SetCurrent(m_SelectedWeatherPreset.WWindDir);
		m_VolFogDistanceDensitySlider.SetCurrent(m_SelectedWeatherPreset.WVolFogDD);
		m_VolFogHeightDensitySlider.SetCurrent(m_SelectedWeatherPreset.WVolFogHD);
		m_VolFogHeightBiasSlider.SetCurrent(m_SelectedWeatherPreset.WVolFogHB);
		
		UpdateEditboxValues();
		UpdateSliderValues();
		InvokeSendRPC();
	}
	
	protected void OnListItemSelected(int row, int column)
	{
		m_SelectedWeatherPreset = m_ConfigDebugProfile.GetWeatherPresets().Get(row);
		m_WeatherPresetEditbox.SetText(m_SelectedWeatherPreset.Name);
	}

	protected void ToogleUIVisibility()
	{
		m_UIVisibilityState = !m_UIVisibilityState;
		m_ScriptConsole.ShowMenuBackground(m_UIVisibilityState);
		m_Root.FindAnyWidget("WeatherRoot").Show(m_UIVisibilityState);
		m_HideUIIcon.SetImage(m_UIVisibilityState);
	}
	
	//RPC data where each value is strickly as shown in the UI on the sliders
	protected DebugWeatherRPCData GetRPCDataNormal()
	{
		DebugWeatherRPCData data = new DebugWeatherRPCData();
		// FOG
		data.m_FogValue = m_FogValueSetSlider.GetCurrent() / 100;
		data.m_FogInterpolation = m_FogInterpolationTimeValue;
		data.m_FogDuration = m_FogDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		// Rain
		data.m_RainValue = m_RainValueSetSlider.GetCurrent() / 100;
		data.m_RainInterpolation = m_RainInterpolationTimeValue;
		data.m_RainDuration = m_RainDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		// OVERCAST
		data.m_OvercastValue = m_OvercastValueSetSlider.GetCurrent() / 100;
		data.m_OvercastInterpolation = m_OvercastInterpolationTimeValue;
		data.m_OvercastDuration = m_OvercastDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		// SNOWFALL
		data.m_SnowfallValue = m_SnowfallValueSetSlider.GetCurrent() / 100;
		data.m_SnowfallInterpolation = m_SnowfallInterpolationTimeValue;
		data.m_SnowfallDuration = m_SnowfallDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		// VOLFOG
		data.m_VolFogDistanceDensity = m_VolFogDistanceDensitySlider.GetCurrent();
		data.m_VolFogDistanceDensityTime = m_VolFogDistanceDensityTimeSlider.GetCurrent() / 100 * DURATION_MAX;

		data.m_VolFogHeightDensity = m_VolFogHeightDensitySlider.GetCurrent();
		data.m_VolFogHeightDensityTime = m_VolFogHeightDensityTimeSlider.GetCurrent() / 100 * DURATION_MAX;

		data.m_VolFogHeightBias = m_VolFogHeightBiasSlider.GetCurrent();
		data.m_VolFogHeightBiasTime = m_VolFogHeightBiasTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		// Wind Magnitude
		data.m_WindMagnitudeValue = m_WindMValueSetSlider.GetCurrent();
		data.m_WindMInterpolation = m_WindMInterpolationTimeValue;
		data.m_WindMDuration = m_WindMDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		// Wind Direction
		data.m_WindDirectionValue = m_WindDValueSetSlider.GetCurrent();
		data.m_WindDInterpolation = m_WindDInterpolationTimeValue;
		data.m_WindDDuration = m_WindDDurationTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
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
		data.m_SnowfallInterpolation = 0;
		data.m_SnowfallDuration = 960;
		
		// VOLFOG
		data.m_VolFogDistanceDensity = m_VolFogDistanceDensitySlider.GetCurrent();
		data.m_VolFogDistanceDensityTime = m_VolFogDistanceDensityTimeSlider.GetCurrent() / 100 * DURATION_MAX;

		data.m_VolFogHeightDensity = m_VolFogHeightDensitySlider.GetCurrent();
		data.m_VolFogHeightDensityTime = m_VolFogHeightDensityTimeSlider.GetCurrent() / 100 * DURATION_MAX;

		data.m_VolFogHeightBias = m_VolFogHeightBiasSlider.GetCurrent();
		data.m_VolFogHeightBiasTime = m_VolFogHeightBiasTimeSlider.GetCurrent() / 100 * DURATION_MAX;
		
		// Wind Magnitude
		data.m_WindMagnitudeValue = m_WindMValueSetSlider.GetCurrent();
		data.m_WindMInterpolation = 0;
		data.m_WindMDuration = 960;
		
		// Wind Direction
		data.m_WindDirectionValue = m_WindDValueSetSlider.GetCurrent();
		data.m_WindDInterpolation = 0;
		data.m_WindDDuration = 960;
		
		return data;
	}
	
	protected void SendRPC(DebugWeatherRPCData data)
	{
		PlayerIdentity identity = null;
		if (g_Game.GetPlayer())
		{
			identity = g_Game.GetPlayer().GetIdentity();
		}
		
		if (g_Game.IsDedicatedServer())
		{
			// have to define earlier due to 'ScriptReadWriteContext' asserting on attempting to 
			// decrement ref count of the serializers. This is due to the Serializers being inline allocated
			ParamsWriteContext wCtx;
			ParamsReadContext rCtx;

			ScriptReadWriteContext ctx = new ScriptReadWriteContext();
			
			// see above comment
			wCtx = ctx.GetWriteContext();
			rCtx = ctx.GetReadContext();

			// aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa it was null for some reason
			Param1<DebugWeatherRPCData> d = new Param1<DebugWeatherRPCData>(data);
			
			wCtx.Write(d);

			g_Game.OnRPC(null, null, ERPCs.DEV_SET_WEATHER, rCtx);
		}
		else
		{
			g_Game.RPCSingleParam(null, ERPCs.DEV_SET_WEATHER, new Param1<DebugWeatherRPCData>(data), true, identity);
		}
	}
	
	protected void InvokeSendRPC()
	{
		SendRPC(GetRPCDataImmediate());
		//m_RPCTimer.Stop();
	}
	
	protected void UpdateWeatherValues()
	{
		float overcast = g_Game.GetWeather().GetOvercast().GetActual();
		m_OvercastValueGetSlider.SetCurrent(overcast * 100);
		m_OvercastValueSetText.SetText(overcast.ToString());
		
		float fog = g_Game.GetWeather().GetFog().GetActual();
		m_FogValueGetSlider.SetCurrent(fog * 100);
		m_FogValueSetText.SetText(fog.ToString());
		
		float rain = g_Game.GetWeather().GetRain().GetActual();
		m_RainValueGetSlider.SetCurrent(rain * 100);
		m_RainValueSetText.SetText(rain.ToString());
		
		float snowfall = g_Game.GetWeather().GetSnowfall().GetActual();
		m_SnowfallValueGetSlider.SetCurrent(snowfall * 100);
		m_SnowfallValueSetText.SetText(snowfall.ToString());
		
		float volFogDistanceDensity = g_Game.GetWeather().GetDynVolFogDistanceDensity();
		//m_VolFogDistanceDensitySlider.SetCurrent(volFogDistanceDensity * 100);
		m_VolFogDistanceDensitySetText.SetText(volFogDistanceDensity.ToString());
		
		float volFogHeightDensity = g_Game.GetWeather().GetDynVolFogHeightDensity();
		//m_VolFogHeightDensitySlider.SetCurrent(volFogHeightDensity * 100);
		m_VolFogHeightDensitySetText.SetText(volFogHeightDensity.ToString());
		
		float volFogHeightBias = g_Game.GetWeather().GetDynVolFogHeightBias();
		//m_VolFogHeightBiasSlider.SetCurrent(volFogHeightBias * 100);
		m_VolFogHeightBiasSetText.SetText(volFogHeightBias.ToString());
		
		float windMagnitude = g_Game.GetWeather().GetWindMagnitude().GetActual();
		m_WindMValueGetSlider.SetCurrent(windMagnitude);
		m_WindMValueSetText.SetText(windMagnitude.ToString());
		
		float windDirection = g_Game.GetWeather().GetWindDirection().GetActual();
		m_WindDValueGetSlider.SetCurrent(windDirection);
		m_WindDValueSetText.SetText(windDirection.ToString());
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		UpdateWeatherValues();

		m_IsCAPSPressed = KeyState(KeyCode.KC_CAPITAL);
		if (m_IsCAPSPressed && !m_MouseMovementEnabled && (!GetFocus() || GetFocus() != m_WeatherPresetEditbox) && GetWidgetUnderCursor() != m_WeatherPresetEditbox)
		{
			if (g_Game && g_Game.GetMission() && g_Game.GetUIManager())
			{
				m_MouseMovementEnabled = true;
				g_Game.GetMission().RemoveActiveInputExcludes({"aiming"});
				g_Game.GetUIManager().ShowUICursor(false);
			}
		}
		else if (!m_IsCAPSPressed && m_MouseMovementEnabled)
		{
			if (g_Game && g_Game.GetMission() && g_Game.GetUIManager())
			{
				m_MouseMovementEnabled = false;
				g_Game.GetMission().AddActiveInputExcludes({"aiming"});
				g_Game.GetUIManager().ShowUICursor(true);
			}
		}
		
		if (GetFocus() == m_WeatherPresetEditbox && !m_CanUseMovement)
		{
			m_CanUseMovement = true;
			g_Game.GetMission().RemoveActiveInputExcludes({"UAMoveForward", "UAMoveBack", "UAMoveLeft", "UAMoveRight", "UAWalkRunTemp"});
		}
		else if ((GetFocus() != m_WeatherPresetEditbox || !GetFocus()) && GetWidgetUnderCursor() != m_WeatherPresetEditbox && m_CanUseMovement)
		{
			m_CanUseMovement = false;
			g_Game.GetMission().AddActiveInputExcludes({"UAMoveForward", "UAMoveBack", "UAMoveLeft", "UAMoveRight", "UAWalkRunTemp"});
		}
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		ButtonHighlight(w);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		ButtonNormal(w);
		return false;
	}
	
	protected void ButtonHighlight(Widget w)
	{
		ButtonWidget wb;
		if (Class.CastTo(wb, w))
		{
			if (wb.GetName() == "ButtonHide")
			{
				ImageWidget iw = ImageWidget.Cast(wb.FindAnyWidget(wb.GetName() + "Icon"));
				iw.SetColor(ARGB(255, 0, 0, 0));
			}
			else
			{
				wb.SetTextColor(ARGB(255, 0, 0, 0));
			}
		}
	}
	
	protected void ButtonNormal(Widget w)
	{
		ButtonWidget wb;
		if (Class.CastTo(wb, w))
		{
			if (wb.GetName() == "ButtonHide")
			{
				ImageWidget iw = ImageWidget.Cast(wb.FindAnyWidget(wb.GetName() + "Icon"));
				iw.SetColor(ARGB(255, 255, 255, 255));
			}
			else
			{
				wb.SetTextColor(ARGB(255, 255, 255, 255));
			}
		}
	}
}

class ScriptConsoleWeatherWindTab : ScriptConsoleTabBase
{
	protected ScriptConsoleWeatherTab m_WeatherTab;
	
	void ScriptConsoleWeatherWindTab(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
	{
		m_Parent = parent;
		m_WeatherTab = ScriptConsoleWeatherTab.Cast(parent);
	}
}
