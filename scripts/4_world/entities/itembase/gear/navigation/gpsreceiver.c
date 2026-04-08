class GPSReceiver : ItemGPS
{

	protected const string DISPLAY_TEXTURE_PATH_FORMAT 	= "dz\\gear\\navigation\\data\\GPS_%1_ca.paa";
	protected const string ANIM_PHASE_DISPLAY_HIDE 		= "DisplayState";
	
	protected const int PHASE_OFF = 1;
	protected const int PHASE_ON = 0;

	protected ref set<string> 		m_DisplayGridSelections;
	protected ref set<string> 		m_DisplayAltSelections;
	protected ref map<int, string> 	m_DisplayNumericSignTextureMap;
	
	//! cache
	protected ref array<int> 		m_OrderedPositionNumbersLast;
	protected ref array<int> 		m_AltitudeNumbersLast;
	
	protected bool 					m_InitGPS;
	
	void GPSReceiver()
	{
		m_OrderedPositionNumbersLast 	= new array<int>;
		m_AltitudeNumbersLast 			= new array<int>;
		
		int i;
		for (i = 0; i < MapNavigationBehaviour.DISPLAY_GRID_POS_MAX_CHARS_COUNT * 2; ++i)
		{
			m_OrderedPositionNumbersLast.Insert(0);
		}
		
		for (i = 0; i < MapNavigationBehaviour.DISPLAY_ALT_MAX_CHARS_COUNT; ++i)
		{
			m_AltitudeNumbersLast.Insert(0);
		}	
		
		m_DisplayGridSelections = new set<string>();
		m_DisplayGridSelections.Insert("grid_1_0");
		m_DisplayGridSelections.Insert("grid_1_1");
		m_DisplayGridSelections.Insert("grid_1_2");
		m_DisplayGridSelections.Insert("grid_2_0");
		m_DisplayGridSelections.Insert("grid_2_1");
		m_DisplayGridSelections.Insert("grid_2_2");
		
		m_DisplayAltSelections = new set<string>();
		m_DisplayAltSelections.Insert("alt_0");
		m_DisplayAltSelections.Insert("alt_1");
		m_DisplayAltSelections.Insert("alt_2");
		m_DisplayAltSelections.Insert("alt_3");
		
		m_DisplayNumericSignTextureMap = new map<int, string>();
		for (i = -1; i < 11; i++)
		{
			
			string texturePath = string.Format(DISPLAY_TEXTURE_PATH_FORMAT, i);
			
			//! translate numerical -1 to `-` texture
			if (i == -1)
			{
				texturePath = string.Format(DISPLAY_TEXTURE_PATH_FORMAT, "dash");
			}

			m_DisplayNumericSignTextureMap.Insert(i, texturePath);
		}
	}
	
	protected void PositionalUpdate()
	{
		UpdateDisplayPosition();
		UpdateDisplayElevation();
		
		if (!m_InitGPS)
			m_InitGPS = true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
	}
	
	override bool IsTurnedOn()
	{
		return GetCompEM() && GetCompEM().IsWorking();
	}
	
	override void OnWorkStart()
	{
		UpdateDisplayState(PHASE_ON);
		
		if (GetHierarchyRoot())
		{
			PlayerBase pb = PlayerBase.Cast(GetHierarchyRoot());
			if (pb)
			{
				pb.MapNavigationItemInPossession(this);
			}
		}
	}
	
	override void OnWorkStop()
	{
		UpdateDisplayState(PHASE_OFF);

		if (GetHierarchyRoot())
		{
			PlayerBase pb = PlayerBase.Cast(GetHierarchyRoot());
			if (pb)
			{
				pb.MapNavigationItemNotInPossession(this);
			}
		}
	}
	
	override void OnWork(float consumed_energy)
	{
		PositionalUpdate();
	}
	
	protected void UpdateDisplayState(int animPhase)
	{
		ResetAnimationPhase(ANIM_PHASE_DISPLAY_HIDE, PHASE_OFF);
		SetAnimationPhaseNow(ANIM_PHASE_DISPLAY_HIDE, animPhase);
	}
		
	protected void UpdateDisplayPosition()
	{
		int selectionIndex;
		string selectionName;
		string textureName;
		array<int> orderedPositionNumbers = MapNavigationBehaviour.OrderedPositionNumbersFromGridCoords(this);
		
		bool isArrayDifferent = orderedPositionNumbers.DifferentAtPosition(m_OrderedPositionNumbersLast) != INDEX_NOT_FOUND;
		if (isArrayDifferent || !m_InitGPS)
		{
			for (int i = 0; i < m_DisplayGridSelections.Count(); ++i)
			{
				selectionName 	= m_DisplayGridSelections.Get(i);
				selectionIndex 	= GetHiddenSelection(selectionName);
				textureName 	= m_DisplayNumericSignTextureMap.Get(orderedPositionNumbers.Get(i));
				SetObjectTexture(selectionIndex, textureName);
			}
			
			m_OrderedPositionNumbersLast = orderedPositionNumbers;
		}
	}
	
	protected void UpdateDisplayElevation()
	{
		int selectionIndex;
		string selectionName;
		array<int> altitudeNumbers = MapNavigationBehaviour.OrderedAltitudeNumbersPosition(this);
		
		bool isArrayDifferent = altitudeNumbers.DifferentAtPosition(m_AltitudeNumbersLast) != INDEX_NOT_FOUND;
		if (isArrayDifferent || !m_InitGPS)
		{
			for (int i = 0; i < m_DisplayAltSelections.Count(); ++i)
			{
				selectionName 	= m_DisplayAltSelections.Get(i);
				selectionIndex 	= GetHiddenSelection(selectionName);
				SetObjectTexture(selectionIndex, m_DisplayNumericSignTextureMap.Get(altitudeNumbers.Get(i)));
			}

			m_AltitudeNumbersLast = altitudeNumbers;
		}
	}
	
	protected int GetHiddenSelection(string selection)
	{	
		int idx = GetHiddenSelectionIndex(selection);

		if (idx != INDEX_NOT_FOUND)
		{
			return idx;
		}
		else
		{
			return 0;
		}
	}

	//================================================================
	// DEBUG
	//================================================================
			
	override void OnDebugSpawn()
	{
		Battery9V.Cast(GetInventory().CreateInInventory("Battery9V"));
	}
	
	//================================================================
	// DEPRECATED BELOW
	//================================================================
	[Obsolete("1.29: Use the overloaded function GPSReceiver::UpdateDisplayState instead")]
	protected void UpdateDisplayState(bool pIsTurnedOn)
	{
		if (pIsTurnedOn)
		{
			UpdateDisplayState(PHASE_ON);
		}
		else
		{
			UpdateDisplayState(PHASE_OFF);
		}
	}
}