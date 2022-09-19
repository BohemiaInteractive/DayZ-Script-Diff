class TutorialKeybinds extends ScriptedWidgetEventHandler
{
	protected Widget						m_Root;
	protected Widget						m_DetailsRoot;
	
	protected TutorialsMenu					m_Menu;
	
	void TutorialKeybinds( Widget parent, TutorialsMenu menu )
	{
		m_Root							= GetGame().GetWorkspace().CreateWidgets( GetLayoutName(), parent );
		
		m_Menu							= menu;
		
		int column_index;
		TIntArray actions = new TIntArray;
		GetUApi().GetActiveInputs(actions);
		int item_index;
		for( int i = 0; i < actions.Count(); i++ )
		{
			UAInput input		= GetUApi().GetInputByID( actions.Get( i ) );
			if( input )
			{
				if( item_index < 100 )
				{
					string output = "";
					string option_text = "";
					GetGame().GetInput().GetActionDesc( actions.Get( i ), option_text );
					if( SetElementTitle( input, EUAINPUT_DEVICE_KEYBOARDMOUSE, output ) )
					{
						column_index		= Math.Floor( item_index / 21 );
						Widget w			= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/tutorials/xbox/keybindings_panels/keybinding_panel.layout", m_Root.FindAnyWidget( "container" + column_index ) );
						Widget spacer		= w.FindWidget( "Spacer" );
						TextWidget name		= TextWidget.Cast( w.FindWidget( "KeybindName" ) );
						TextWidget mod		= TextWidget.Cast( spacer.FindWidget( "KeybindModifier" ) );
						TextWidget value	= TextWidget.Cast( spacer.FindWidget( "KeybindButton" ) );
						
						string modifier_text = "";
						name.SetText( option_text );
						value.SetText( output );
						
						if( SetElementModifier( input, modifier_text ) )
						{
							mod.SetText( modifier_text );
						}
						
						name.Update();
						mod.Update();
						value.Update();
						spacer.Update();
						
						item_index++;
					}
				}
			}
		}
		
		m_Root.SetHandler( this );
	}
	
	//! assemble all active bindings at widget element
	bool SetElementTitle( UAInput pInput, int iDeviceFlags, out string output )
	{
		int a, i, countbind = 0;
	
		for( a = 0; a < pInput.AlternativeCount(); a++ )
		{
			pInput.SelectAlternative(a);
			if( pInput.IsCombo() )
			{
				if( pInput.BindingCount() > 0 )
				{
					if( pInput.Binding(0) != 0 && pInput.CheckBindDevice(0,iDeviceFlags) )
					{
						if( countbind > 0 )
							output += ", ";
						
						output += GetUApi().GetButtonName( pInput.Binding(0) );
						countbind++;
						
						for( i = 1; i < pInput.BindingCount(); i++ )
						{
							if( pInput.Binding(i) != 0 )
							{
								output += " + " + GetUApi().GetButtonName( pInput.Binding(i) );
								countbind++;
							}
						}
						
					}
				}
			}
			else
			{
				if( pInput.BindingCount() > 0 )
				{
					if( pInput.Binding(0) != 0 && pInput.CheckBindDevice(0,iDeviceFlags) )
					{
						if( countbind > 0 )
							output += ", ";

						output += GetUApi().GetButtonName( pInput.Binding(0) );
						countbind++;
					}
				}
			}
			
		}
		
		return ( countbind > 0 );
	}
	
	//! Determine the active limiter of the bindings (currently unreliable, multiple limiters can be active on key combos!)
	bool SetElementModifier( UAInput pInput, out string output )
	{
		if( pInput.IsLimited() )
		{
			if( pInput.IsPressLimit() )
			{
				output = "#keybind_press";
			}
			if( pInput.IsReleaseLimit() )
			{
				output = "#keybind_release";
			}
			if( pInput.IsHoldLimit() )
			{
				output = "#keybind_hold";
			}
			if( pInput.IsHoldBeginLimit() )
			{
				output = "#keybind_holdbegin";
			}
			if( pInput.IsClickLimit() )
			{
				output = "#keybind_click";
			}
			if( pInput.IsDoubleClickLimit() )
			{
				output = "#keybind_doubletap";
			}
			
			return true;
		}
		else
		{
			return false;
		}
	}
	
	void ~TutorialKeybinds()
	{
		
	}
	
	string GetLayoutName()
	{
		return "gui/layouts/new_ui/tutorials/xbox/keybinds_tab.layout";
	}
}
