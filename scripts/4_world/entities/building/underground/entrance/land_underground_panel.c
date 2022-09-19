enum ELEDState
{
	OFF,
	BLINKING,
	ON,
}

enum ELEDColors
{
	RED,
	GREEN,
}

class Land_Underground_Panel: House
{
	static ref set<Land_Underground_EntranceBase> 		m_Entrances;
	static ref set<Land_Underground_Panel>				m_Panels;
	
	Land_Underground_EntranceBase m_LinkedDoor;
	
	const string COLOR_LED_OFF 		= "#(argb,8,8,3)color(0,0,0,1.0,co)";
	const string COLOR_LED_GREEN	= "#(argb,8,8,3)color(0,1,0,1.0,co)";
	const string COLOR_LED_RED 		= "#(argb,8,8,3)color(1,0,0,1.0,co)";
	
	const string SELECTION_NAME_LED_RED		= "LED_Red";
	const string SELECTION_NAME_LED_GREEN	= "LED_Green";
	
	bool m_PanelWasUsed
	bool m_PanelWasUsedPrev;
	bool m_BlinkingFlipFlop;
	ref Timer m_FlipFlopTimer;
	
	EffectSound 		m_ActivationSound;
	
	ELEDState m_LedStateRed;
	ELEDState m_LedStateGreen;
	
	void Land_Underground_Panel()
	{
		RegisterNetSyncVariableBool("m_PanelWasUsed");
		RegisterPanel(this);
		SetLEDState(ELEDColors.RED, ELEDState.ON);
		SetLEDState(ELEDColors.GREEN, ELEDState.OFF);
	}
	
	void ~Land_Underground_Panel()
	{
		UnregisterPanel(this);
	}
	
	bool CanInteract()
	{
		Land_Underground_EntranceBase door = GetLinkedDoor();
		if (door)
		{
			return Land_Underground_EntranceBase.Cast(door).CanManipulate();
		}
		return false;
	}
	
	void Interact()
	{
		Land_Underground_EntranceBase door = GetLinkedDoor();
		if (door)
		{
			GetGame().RegisterNetworkStaticObject(this);
			Land_Underground_EntranceBase.Cast(door).Manipulate();
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( ResetPanelUsed, 500);
			m_PanelWasUsed = true;
			SetSynchDirty();
		}
		else
		{
			ErrorEx("Can't find the instance of entrance the panel is linkined to");
		}
	}
	
	static void RegisterEntrance(Land_Underground_EntranceBase entrance)
	{
		if (!m_Entrances)
		{
			m_Entrances = new set<Land_Underground_EntranceBase>();
		}
		m_Entrances.Insert(entrance);
	}
	
	static void UnregisterEntrance(Land_Underground_EntranceBase entrance)
	{
		if (m_Entrances)
		{
			int index = m_Entrances.Find(entrance);
			if (index != -1)
			{
				m_Entrances.Remove(index);
			}
			else
			{
				ErrorEx("Attempted to unregistered non-registered entrance");
			}
		}
	}
	
	static void RegisterPanel(Land_Underground_Panel panel)
	{
		if (!m_Panels)
		{
			m_Panels = new set<Land_Underground_Panel>;
		}
		m_Panels.Insert(panel);
	}
	
	static void UnregisterPanel(Land_Underground_Panel panel)
	{
		if (m_Panels)
		{
			int index = m_Panels.Find(panel);
			if (index != -1)
			{
				m_Panels.Remove(index);
			}
			else
			{
				ErrorEx("Attempted to unregistered non-registered panel");
			}
		}
	}

	EUndegroundDoorType GetLinkedDoorType()
	{
		return GetLinkedDoor().m_DoorType;
	}
	
	Land_Underground_EntranceBase GetLinkedDoor()
	{
		if (!m_LinkedDoor)
			m_LinkedDoor = GetClosestDoor();
		return m_LinkedDoor;
	}
	
	Land_Underground_EntranceBase GetClosestDoor()
	{
		if (!m_Entrances)
		{
			return null;
		}
		
		float closestDst = float.MAX;
		Land_Underground_EntranceBase closestObj;
		vector thisPos = GetPosition();
		
		foreach (Land_Underground_EntranceBase obj: m_Entrances)
		{
			float dist = vector.DistanceSq(thisPos, obj.GetPosition());
			if (dist < closestDst)
			{
				closestDst = dist;
				closestObj = obj;
			}
		}
		
		return closestObj;
	}
	
	void SetLEDState(ELEDColors color, ELEDState state)
	{
		#ifndef SERVER
		if (color == ELEDColors.RED)
		{
			if (m_LedStateRed != state)
			{
				m_LedStateRed = state;
				OnLEDStateChanged();
			}
		}
		else
		{
			if (m_LedStateGreen != state)
			{
				m_LedStateGreen = state;
				OnLEDStateChanged();
			}
		}
		#endif
	}
	
	void SetBlinkingTimer(bool enable)
	{
		if (enable)
		{
			if (!m_FlipFlopTimer)
			{
				m_FlipFlopTimer = new Timer();
				m_FlipFlopTimer.Run(0.1, this, "ToggleFlipFlop", NULL, true);
			}
		}
		else
		{
			if (m_FlipFlopTimer)
			{
				m_FlipFlopTimer = null;
			}
		}
	}
	
	void OnLEDStateChanged()
	{
		/*
		Print("OnLEDStateChanged()");
		Print("m_LedStateRed: " + m_LedStateRed);
		Print("m_LedStateGreen: " + m_LedStateGreen);
		*/
		if (m_LedStateRed == ELEDState.BLINKING || m_LedStateGreen == ELEDState.BLINKING)
		{
			SetBlinkingTimer(true);
		}
		else
		{
			SetBlinkingTimer(false);
		}
		//red
		if (m_LedStateRed == ELEDState.ON)
		{
			UpdateLED(SELECTION_NAME_LED_RED, COLOR_LED_RED);
		}
		else if (m_LedStateRed == ELEDState.OFF)
		{
			UpdateLED(SELECTION_NAME_LED_RED, COLOR_LED_OFF);
		}
		else if (m_LedStateRed == ELEDState.BLINKING)
		{
			if (m_BlinkingFlipFlop)
			{
				UpdateLED(SELECTION_NAME_LED_RED, COLOR_LED_RED);
			}
			else
			{
				UpdateLED(SELECTION_NAME_LED_RED, COLOR_LED_OFF);
			}
			
		//green
		}
		if (m_LedStateGreen == ELEDState.ON)
		{
			UpdateLED(SELECTION_NAME_LED_GREEN, COLOR_LED_GREEN);
		}
		else if (m_LedStateGreen == ELEDState.OFF)
		{
			UpdateLED(SELECTION_NAME_LED_GREEN, COLOR_LED_OFF);
		}
		else if (m_LedStateGreen == ELEDState.BLINKING)
		{
			if(m_BlinkingFlipFlop)
			{
				UpdateLED(SELECTION_NAME_LED_GREEN, COLOR_LED_GREEN);
			}
			else
			{
				UpdateLED(SELECTION_NAME_LED_GREEN, COLOR_LED_OFF);
			}
			
		}
	}
	
	void ResetPanelUsed()
	{
		m_PanelWasUsed = false;
		SetSynchDirty();
	}
	
	void OnDoorStateChangedClient(EUndegroundEntranceState newState, EUndegroundEntranceState prevState)
	{
		switch (GetLinkedDoorType())
		{
			case EUndegroundDoorType.MAIN:
			{
				if (newState >= EUndegroundEntranceState.CLOSED && newState < EUndegroundEntranceState.OPENING_G)
				{
					SetLEDState(ELEDColors.RED, ELEDState.ON);
				}
				else if (newState == EUndegroundEntranceState.OPENING_G)
				{
					SetLEDState(ELEDColors.RED, ELEDState.OFF);
				}
				else if (newState >= EUndegroundEntranceState.CLOSING_A)
				{
					SetLEDState(ELEDColors.RED, ELEDState.BLINKING);
				}
				
				if (newState == EUndegroundEntranceState.CLOSED)
				{
					SetLEDState(ELEDColors.GREEN, ELEDState.OFF);
				}
				else if (newState >= EUndegroundEntranceState.OPENING_A && newState < EUndegroundEntranceState.OPENING_G)
				{
					SetLEDState(ELEDColors.GREEN, ELEDState.BLINKING);
				}
				else if (newState >= EUndegroundEntranceState.OPENING_G)
				{
					SetLEDState(ELEDColors.GREEN, ELEDState.ON);
				}
			}
			break;
			case EUndegroundDoorType.SMALL:
			{
				if (newState == EUndegroundEntranceState.OPENING_A)
				{
					SetLEDState(ELEDColors.RED, ELEDState.ON);
					SetLEDState(ELEDColors.GREEN, ELEDState.BLINKING);
				}
				else if (newState == EUndegroundEntranceState.OPENING_B)
				{
					SetLEDState(ELEDColors.RED, ELEDState.OFF);
					SetLEDState(ELEDColors.GREEN, ELEDState.ON);
				}
				else if (newState == EUndegroundEntranceState.CLOSING_A)
				{
					SetLEDState(ELEDColors.RED, ELEDState.BLINKING);
					SetLEDState(ELEDColors.GREEN, ELEDState.ON);
				}
				if (newState == EUndegroundEntranceState.CLOSING_B || newState == EUndegroundEntranceState.CLOSED)
				{
					SetLEDState(ELEDColors.RED, ELEDState.ON);
					SetLEDState(ELEDColors.GREEN, ELEDState.OFF);
				}
			}
			break;
		}
	}
	
	
	protected void UpdateLED(string selection, string color)
	{
		int selectionIdx = GetHiddenSelectionIndex(selection);
		SetObjectTexture(selectionIdx, color);
		//Print("selection: " +selection + ", color: " + color +", selectionIdx: "+ selectionIdx);
	}
	
	void ToggleFlipFlop()
	{
		m_BlinkingFlipFlop = !m_BlinkingFlipFlop;
		OnLEDStateChanged();
	}
	
	void OnPanelUsedSynchronized()
	{
		PlaySoundSet( m_ActivationSound, "UndergroundDoor_PanelActivation_SoundSet", 0, 0 );
	}
	
	override void OnVariablesSynchronized()
	{
		if (m_PanelWasUsed != m_PanelWasUsedPrev)
		{
			if (m_PanelWasUsed)
			{
				OnPanelUsedSynchronized();
			}
			m_PanelWasUsedPrev = m_PanelWasUsed;
		}
	}
	
}