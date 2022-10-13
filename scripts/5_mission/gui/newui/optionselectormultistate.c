class OptionSelectorMultistate extends OptionSelector
{
	protected bool 	m_CanSwitch;
	
	void OptionSelectorMultistate( Widget parent, int current_index, ScriptedWidgetEventHandler parent_c, bool disabled, notnull array<string> options )
	{
		m_CanSwitch = true;
		
		m_SelectorType				= 2;
		m_Options					= options;
		if( options.Count() == 0 )
		{
			Error( "Invalid OptionSelectorMultistate options" );
		}
		
		if( current_index < 0 || current_index >= m_Options.Count() )
		{
			m_SelectedOptionIndex = 0;
		}
		else
		{
			m_SelectedOptionIndex = current_index;
		}
		
		/*
		//handled in the 'OptionSelector'
		m_Enabled = !disabled;
		if( m_Enabled )
		{
			Enable();
		}
		else
		{
			Disable();
		}*/
	
		m_SelectedOption.SetText( m_Options.Get( m_SelectedOptionIndex ) );
	}
	
	void LoadNewValues( notnull array<string> options, int current_index )
	{
		m_Options = options;
		SetValue( current_index );
	}
	
	override void SetNextOption()
	{
		int idx = m_SelectedOptionIndex;
		idx++;
		if( idx >= m_Options.Count() )
		{
			idx = 0;
		}
		
		m_AttemptOptionChange.Invoke(idx);
		if (m_CanSwitch)
		{
			//m_SelectedOptionIndex = idx;
			PerformSetOption(idx);
		}
	}
	
	override void SetPrevOption()
	{
		int idx = m_SelectedOptionIndex;
		idx--;
		if( idx < 0 )
		{
			idx = m_Options.Count() - 1;
		}
		
		m_AttemptOptionChange.Invoke(idx);
		if (m_CanSwitch)
		{
			//m_SelectedOptionIndex = idx;
			PerformSetOption(idx);
		}
	}
	
	//actual set
	void PerformSetOption(int index)
	{
		m_SelectedOptionIndex = index;
		m_SelectedOption.SetText( m_Options.Get( index ) );
		m_OptionChanged.Invoke( index );
	}
	
	void SetCanSwitch(bool value)
	{
		m_CanSwitch = value;
	}
	
	bool CanSwitch()
	{
		return m_CanSwitch;
	}
	
	void SetValue( int value, bool fire_event = true )
	{
		if( value < m_Options.Count() && value >= 0 )
		{
			m_SelectedOptionIndex = value;
			m_SelectedOption.SetText( m_Options.Get( m_SelectedOptionIndex ) );
			if( fire_event )
				m_OptionChanged.Invoke( m_SelectedOptionIndex );
		}
	}
	
	void SetValue( string value, bool fire_event = true )
	{
		int index = m_Options.Find( value );
		if( index >= 0 )
		{
			m_SelectedOptionIndex = index;
			m_SelectedOption.SetText( m_Options.Get( m_SelectedOptionIndex ) );
			if( fire_event )
				m_OptionChanged.Invoke( m_SelectedOptionIndex );
		}
	}
	
	void SetRandomValue()
	{
		m_SelectedOptionIndex = Math.RandomInt( 0, m_Options.Count() );
		m_SelectedOption.SetText( m_Options.Get( m_SelectedOptionIndex ) );
		m_OptionChanged.Invoke( m_SelectedOptionIndex );
	}
	
	int GetValue()
	{
		return m_SelectedOptionIndex;
	}
	
	override void SetStringOption( string option, bool fire_event = true )
	{
		int index = m_Options.Find( option );
		if( index > -1 )
		{
			m_SelectedOptionIndex = index;
			m_SelectedOption.SetText( m_Options.Get( m_SelectedOptionIndex ) );
			
			if( fire_event )
				m_OptionChanged.Invoke( m_SelectedOptionIndex );
		}
	}
}

class OptionSelectorMultistateCharacterMenu extends OptionSelectorMultistate
{
	//m_SelectedOption.SetDisplayNameText( m_Options.Get( m_SelectedOptionIndex ) );
	string class_name;
	string path = "cfgVehicles " + class_name + " displayName";
	string displayname;
	
	void OptionSelectorMultistateCharacterMenu( Widget parent, int current_index, ScriptedWidgetEventHandler parent_c, bool disabled, notnull array<string> options )
	{
		SetTextSmart( m_Options.Get( m_SelectedOptionIndex ) );
	}
	
	void SetDisplayNameText(string value, string appendix)
	{
		//class_name = value;
		path = "cfgVehicles " + class_name + " displayName";
		GetGame().ConfigGetText(path, displayname);
		if (value == "Male")
		{
			m_SelectedOption.SetText("#str_cfgvehicles_survivorM0");
		}
		else if (value == "Female")
		{
			m_SelectedOption.SetText("#str_cfgvehicles_survivorF0");
		}
		else if (appendix != "")
		{
			if (value == "")
				m_SelectedOption.SetText("NO ITEM - localization needed!");
			else
				m_SelectedOption.SetText("" + displayname + " " + appendix); //characters
		}
		else
		{
			m_SelectedOption.SetText(displayname);
		}
	}
	
	void SetTextSmart(string value)
	{
		if (value == "Male" || value == "Female")
		{
			SetDisplayNameText(value,"");
			return;
		}
		
		class_name = value;
		int index = m_Options.Find( value ); //TODO - design better way to differentiate between items
		if ( InheritsFrom(class_name, "Clothing_Base") )
		{
			//todo
			SetDisplayNameText(class_name,index.ToString());
		}
		else if (InheritsFrom(class_name,"SurvivorBase"))
		{
			if (InheritsFrom(class_name,"SurvivorMale_Base"))
			{
				SetDisplayNameText("#str_cfgvehicles_survivorM0",(m_SelectedOptionIndex + 1).ToString());
				//#str_cfgvehicles_survivorM0
			}
			else if (InheritsFrom(class_name,"SurvivorFemale_Base"))
			{
				SetDisplayNameText("#str_cfgvehicles_survivorF0",(m_SelectedOptionIndex + 1).ToString());
			}
			//m_SelectedOption.SetText(class_name);
			//SetDisplayNameText(class_name,m_SelectedOptionIndex.ToString());
		}
		else
		{
			//todo
			SetDisplayNameText(class_name,index.ToString());
		}
	}
	
	bool InheritsFrom(string value, string baseclass)
	{
		string child = value;
		string parent;
		while ( GetGame().ConfigGetBaseName("cfgVehicles " + child, parent) )
		{
			if (parent == baseclass)
			{
				return true;
			}
			child = parent;
		}
		return false;
	}
	
	override void SetNextOption()
	{
		super.SetNextOption();
		SetTextSmart( m_Options.Get( m_SelectedOptionIndex ) );
	}
	
	override void SetPrevOption()
	{
		super.SetPrevOption();
		SetTextSmart( m_Options.Get( m_SelectedOptionIndex ) );
	}
	
	override void SetValue( int value, bool fire_event = true )
	{
		super.SetValue( value,fire_event );
		SetTextSmart( m_Options.Get( m_SelectedOptionIndex ) );
	}
	
	override void SetValue( string value, bool fire_event = true )
	{
		super.SetValue( value,fire_event );
		SetTextSmart( m_Options.Get( m_SelectedOptionIndex ) );
	}
	
	override void SetRandomValue()
	{
		super.SetRandomValue();
		SetTextSmart( m_Options.Get( m_SelectedOptionIndex ) );
	}
	
	override void SetStringOption( string option, bool fire_event = true )
	{
		super.SetStringOption( option, fire_event );
		SetTextSmart( m_Options.Get( m_SelectedOptionIndex ) );
	}
}