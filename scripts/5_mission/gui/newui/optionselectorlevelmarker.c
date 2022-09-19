/**
\brief This Option Selector handles a Slider Marker, which basically has 2 sliders
	   One slider is for selecting the value, and the other slider is for displaying some other value/ information
@param m_Slider2 other slider value, which can be used to convery some other info to the player
*/
class OptionSelectorLevelMarker extends OptionSelectorSliderSetup
{
	protected SliderWidget		m_Slider2;
	
	void OptionSelectorLevelMarker(Widget parent, float value, ScriptedWidgetEventHandler parentMenu, bool disabled, float min, float max)
	{
		m_Root					= GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/option_level_marker.layout", parent);
		#ifdef PLATFORM_CONSOLE
			m_Parent				= parent.GetParent().GetParent();
		#else
		#ifdef PLATFORM_WINDOWS
			m_Parent				= parent.GetParent();
		#endif
		#endif
		
		m_SelectorType			= 1;
		m_ParentClass			= parentMenu;
		m_Slider				= SliderWidget.Cast(m_Root.FindAnyWidget( "marker_value" ));
		m_Slider2				= SliderWidget.Cast(m_Root.FindAnyWidget( "other_slider_value" ));
		
		m_MinValue					= min;
		m_MaxValue					= max;
		
		SetValue(value, false);
		SetSlider2Value( 0 );
		Enable();

		m_Parent.SetHandler(this);
	}
	
	override void Enable()
	{
		super.Enable();
		m_Slider2.ClearFlags(WidgetFlags.IGNOREPOINTER);
	}
	
	override void Disable()
	{
		super.Disable();
		m_Slider2.SetFlags(WidgetFlags.IGNOREPOINTER);
	}
	
	void SetSlider2Value(float value)
	{
		m_Slider2.SetCurrent(NormalizeInput(value));
	}
	
	override bool OnUpdate(Widget w)
	{
		return false;
	}
}