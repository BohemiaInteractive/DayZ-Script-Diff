class InputUtils
{
	//useful for console preset differentiation
	static const string PRESET_OLD 			= "#STR_UAPRESET_0";
	static const string PRESET_NEW 			= "#STR_UAPRESET_1";
	static const int VARIANT_OLD 			= 0;
	static const int VARIANT_NEW 			= 1;
	static int m_CurrentPresetIDConsole 	= -1;
	//
	
	static protected ref map<int, ref array<int>> m_InputActionSortingMap; //<sorting_idx,<input_ID>>
	static protected ref array<int> m_UnsortedInputActions;
	
	static const float ICON_SCALE_NORMAL	= 1.21;
	static const float ICON_SCALE_TOOLBAR	= 1.81;
	
	static string GetButtonNameFromInput(string pInputName, int pInputDeviceType)
	{
		UAInput inp = GetUApi().GetInputByName(pInputName);
		for (int i = 0; i < inp.AlternativeCount(); i++)
		{
			inp.SelectAlternative(i);
			if (inp.CheckBindDevice(0, pInputDeviceType))
			{
				return GetUApi().GetButtonName(inp.GetBindKey(0));
			}
		}
		
		return "";
	}
	
	//! returns a map of button names, combo or not
	static map<int,ref TStringArray> GetComboButtonNamesFromInput(string pInputName, int pInputDeviceType) //TODO: revisit to include same input device alternatives too?
	{
		UAInput inp = GetUApi().GetInputByName(pInputName);
		TStringArray buttons;
		map<int,ref TStringArray> output;//<alternativeIDX<button_array>>
		for (int i = 0; i < inp.AlternativeCount(); i++)
		{
			inp.SelectAlternative(i);
			if (inp.BindingCount() > 0 && inp.Binding(0) != 0 && inp.CheckBindDevice(0,pInputDeviceType))
			{
				buttons = new TStringArray;
				if (inp.IsCombo())
				{
					buttons.Insert(GetUApi().GetButtonName( inp.Binding(0) ));
					
					for (int j = 1; j < inp.BindingCount(); j++)
					{
						if (inp.Binding(j) != 0 && inp.CheckBindDevice(j,pInputDeviceType))
						{
							buttons.Insert(GetUApi().GetButtonName( inp.Binding(j) ));
						}
					}
					//return buttons;
				}
				else
				{
					buttons.Insert(GetUApi().GetButtonName(inp.GetBindKey(0)));
					//return buttons;
				}
				
				if (buttons.Count() > 0)
				{
					if (!output)
					{
						output = new map<int,ref TStringArray>;
					}
					output.Insert(i,buttons);
				}
			}
		}
		
		return output;
	}

	static array<string> GetButtonIconPathFromInput(notnull UAInput pInput, int pInputDeviceType = EUAINPUT_DEVICE_CONTROLLER)
	{
		array<string> buttonIcons = new array<string>();
		
		for (int i = 0; i < pInput.AlternativeCount(); i++)
		{
			pInput.SelectAlternative(i);
			bool done = false;
			for (int bk = 0; bk < pInput.BindKeyCount(); bk++)
			{
				if (pInput.CheckBindDevice(0, pInputDeviceType))
				{
					buttonIcons.Insert(GetUApi().GetButtonIcon(pInput.Binding(bk)));
				
					if (bk == pInput.BindKeyCount() - 1)
					{
						done = true;
					}
				}
			}
				
			if (done)
			{
				buttonIcons.Invert();
				return buttonIcons;
			}
		}
		
		return buttonIcons;
	}
	
	static array<string> GetButtonIconPathFromInput(string pInputName, int pInputDeviceType)
	{
		UAInput inp = GetUApi().GetInputByName(pInputName);

		array<string> buttonIcons = new array<string>();
		buttonIcons = InputUtils.GetButtonIconPathFromInput(inp, pInputDeviceType);
		
		return buttonIcons;
	}
	
	static void GetImagesetAndIconFromInputAction(notnull UAInput pInput, int pInputDeviceType, out array<string> pImageSet, out array<string> pIconName)
	{
		array<string> buttons = InputUtils.GetButtonIconPathFromInput(pInput, pInputDeviceType);
		if (buttons.Count() == 0 )
		{
			return;
		}
		
		for (int i = 0; i < buttons.Count(); i++)
		{
			array<string> parts = new array<string>;
			buttons.Get(i).Split(":", parts);
			
			if (parts.Count() < 2)
			{
				return;
			}
			
			pImageSet.Insert(parts[1].SubstringInverted(parts[1], parts[1].Length() - 6, parts[1].Length()));
	
			//! Asia vs Europe/N.America controller layout
			#ifdef PLATFORM_PS4		
			if (GetGame().GetInput().GetEnterButton() == GamepadButton.B)
			{
				//! switch confirm button
				if (parts[2] == "cross")
				{
					parts[2] = "circle";
				}
				
				//! switch back button
				if (parts[2] == "circle")
				{
					parts[2] = "cross";
				}
			}
			#endif
		
			pIconName.Insert(parts[2]);
		}
	}
	
	static void GetImagesetAndIconFromInputAction(string pInputAction, int pInputDeviceType, out array<string> pImageSet, out array<string> pIconName)
	{
		UAInput inp = GetUApi().GetInputByName(pInputAction);
		
		InputUtils.GetImagesetAndIconFromInputAction(inp, pInputDeviceType, pImageSet, pIconName);
	}
	
	static string GetRichtextButtonIconFromInputAction(notnull UAInput pInput, string pLocalizedDescription, int pInputDeviceType = EUAINPUT_DEVICE_CONTROLLER, float pScale = ICON_SCALE_NORMAL, bool pVertical = false)
	{
		array<string> imageSets = new array<string>();
		array<string> iconNames = new array<string>();
		InputUtils.GetImagesetAndIconFromInputAction(pInput, pInputDeviceType, imageSets, iconNames);
		
		if (imageSets.Count() == 0)
		{
			return "";
		}
		
		string result = string.Format("<image set=\"%1\" name=\"%2\" scale=\"%3\" />", imageSets.Get(0), iconNames.Get(0), pScale);
		
		string divider = " ";
		if (pVertical)
		{
			divider = string.Format("\n%1\n", divider);
		}
		
		if (imageSets.Count() > 1)
		{
			//! first element is already taken
			for (int i = 1; i < imageSets.Count(); i++)
			{
				result = string.Format("%1%2<image set=\"%3\" name=\"%4\" scale=\"%5\" />", result, divider, imageSets.Get(i), iconNames.Get(i), pScale);
			}
		}
		
		return string.Format("%1 %2", result, pLocalizedDescription);
	}
	
	static string GetRichtextButtonIconFromInputAction(string pInputAction, string pLocalizedDescription, int pInputDeviceType = EUAINPUT_DEVICE_CONTROLLER, float pScale = ICON_SCALE_NORMAL, bool pVertical = false)
	{
		UAInput inp = GetUApi().GetInputByName(pInputAction);

		array<string> imageSets = new array<string>();
		array<string> iconNames = new array<string>();
		InputUtils.GetImagesetAndIconFromInputAction(inp, pInputDeviceType, imageSets, iconNames);
		
		return InputUtils.GetRichtextButtonIconFromInputAction(inp, pLocalizedDescription, pInputDeviceType, pScale, pVertical);
	}
	
	static void UpdateConsolePresetID()
	{
		string profile_name;
		GetGame().GetInput().GetProfileName(GetUApi().PresetCurrent(), profile_name);

		if (profile_name == PRESET_OLD)
		{
			m_CurrentPresetIDConsole = VARIANT_OLD;
		}
		else
		{
			m_CurrentPresetIDConsole = VARIANT_NEW;
		}
	}
	
	static int GetConsolePresetID()
	{
		return m_CurrentPresetIDConsole;
	}
	
	static map<int, ref array<int>> GetInputActionSortingMap()
	{
		return m_InputActionSortingMap;
	}
	
	static array<int> GetUnsortedInputActions()
	{
		return m_UnsortedInputActions;
	}
	
	static bool InitInputMetadata()
	{
		if (!m_InputActionSortingMap)
		{
			m_InputActionSortingMap = new map<int, ref array<int>>;
			
			TIntArray sorted_actions = new TIntArray;
			if (!m_UnsortedInputActions)
			{
				m_UnsortedInputActions = new array<int>;
			}
			GetUApi().GetActiveInputs(m_UnsortedInputActions);
			
			UAInput inp;
			
			for (int i = 0; i < GetUApi().SortingCount(); i++)
			{
				int input_id;
				array<int> sorting_content = new array<int>;
				for (int j = 0; j < m_UnsortedInputActions.Count(); j++)
				{
					input_id = m_UnsortedInputActions[j];
					inp = GetUApi().GetInputByID(input_id);
					if (inp.HasSorting(i))
					{
						sorting_content.Insert(input_id);
						sorted_actions.Insert(input_id);
					}
				}
				
				if (sorting_content.Count() > 0)
				{
					sorting_content.Sort();
					m_InputActionSortingMap.Insert(i,sorting_content);
				}
			}
			
			//remove sorted used inputs
			int count = sorted_actions.Count();
			for (i = 0; i < count; i++)
			{
				m_UnsortedInputActions.RemoveItem(sorted_actions[i]);
			}
			return true;
		}
		return false;
	}
}