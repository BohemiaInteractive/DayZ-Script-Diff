class Head_Default extends Head
{
	bool m_handling_exception = false;
	int m_beard_idx;
	int m_hair_idx;
	
	ref map<int,ref SelectionTranslation> 			m_HeadHairHidingStateMap;
	ref array<string> 								m_HeadHairSelectionArray;
	
	void Head_Default()
	{
		Init();
	}
	
	void Init()
	{
		m_HeadHairHidingStateMap = new map<int,ref SelectionTranslation>;
		m_HeadHairSelectionArray = new array<string>;
		
		ConfigGetTextArray("simpleHiddenSelections",m_HeadHairSelectionArray);
		
		m_beard_idx = m_HeadHairSelectionArray.Find("beard");
		m_hair_idx = m_HeadHairSelectionArray.Find("hair");
		if (!ConfigIsExisting("HairHiding"))
			m_handling_exception = true;
		
		InitSelectionTranslation();
	}
	
	void InitSelectionTranslation()
	{
		for (int i = 0; i < m_HeadHairSelectionArray.Count(); i++)
		{
			SelectionTranslation selTran = new SelectionTranslation(this,i);
			selTran.SetSelectionState(true);
			m_HeadHairHidingStateMap.Insert(i,selTran); //all considered "shown" on init
		}
	}
	
	bool GetHeadHideableSelections(out array<string> selections)
	{
		selections = m_HeadHairSelectionArray;
		return true;
	}
	
	int GetBeardIndex()
	{
		return m_beard_idx;
	}
	
	int GetHairIndex()
	{
		return m_hair_idx;
	}
	
	int GetSelectionIndex(string str)
	{
		return m_HeadHairSelectionArray.Find(str);
	}
	
	bool IsHandlingException()
	{
		return m_handling_exception;
	}
};

class SelectionTranslation
{
	bool 			m_SelectionState;
	
	bool 			m_TranslationFinished = false;
	string 			m_BasePath;// = "cfgVehicles " + GetType() + " HairHiding";
	ref array<int> 	m_TranslatedSelections;
	Head_Default 	m_Head;
	
	void SelectionTranslation(Head_Default head, int idx)
	{
		m_Head = head;
		m_BasePath = "cfgVehicles " + m_Head.GetType() + " HairHiding";
		InitTranslatedSelections(idx);
	}
	
	void InitTranslatedSelections(int idx)
	{
		if ( !m_Head.ConfigIsExisting("HairHiding") )
			return;
		
		string path;
		int selectionIdx = -1;
		m_TranslatedSelections = new ref array<int>;
		
		//Hair
		path = m_BasePath + " Group_Hair";
		SearchAndTranslate(path,idx);
		//Beard
		path = m_BasePath + " Group_Beard";
		SearchAndTranslate(path,idx);
		//Hair + Beard
		path = m_BasePath + " Group_HairBeard";
		SearchAndTranslate(path,idx);
		
		//other
		for (int i = 0; !m_TranslationFinished; i++)
		{
			path = m_BasePath + " Group_" + i;
			if ( g_Game.ConfigIsExisting(path) )
			{
				SearchAndTranslate(path,idx);
			}
			else
			{
				m_TranslationFinished = true;
			}
		}
	}
	
	bool SearchAndTranslate(string path, int idx)
	{
		if (!g_Game.ConfigIsExisting(path))
			return false;
		
		array<string> tempArrayStr = new array<string>;
		g_Game.ConfigGetTextArray(path + " memberSelections", tempArrayStr);
		int indexInOriginalArray = -2;
		
		for (int i = 0; i < tempArrayStr.Count(); i++)
		{
			indexInOriginalArray = m_Head.m_HeadHairSelectionArray.Find(tempArrayStr.Get(i));
			if ( idx == indexInOriginalArray ) //found it
			{
				g_Game.ConfigGetTextArray(path + " simpleSelectionName", tempArrayStr);
				for (i = 0; i < tempArrayStr.Count(); i++)
				{
					m_TranslatedSelections.Insert(m_Head.m_HeadHairSelectionArray.Find(tempArrayStr.Get(i)));
				}
				m_TranslationFinished = true;
				return true;
			}
		}
		return false;
	}
	
	void SetSelectionState(bool state)
	{
		m_SelectionState = state;
	}
	
	bool GetSelectionState()
	{
		return m_SelectionState;
	}
	
	ref array<int> GetTranslatedSelections()
	{
		return m_TranslatedSelections;
	}
};