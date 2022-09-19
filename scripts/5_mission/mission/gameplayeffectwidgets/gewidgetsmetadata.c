/*
TODO - doxygen formating
*/

//! generic metadata class
class GameplayEffectsData extends Managed
{
	protected ref array<ref Widget> 	m_WidgetArray;
	int 								m_Type;
	int 								m_WidgetSetIdentifier;
	Widget 								m_LayoutRoot;
	
	void GameplayEffectsData(array<ref Widget> input, int type, int user_override = -1)
	{
		m_WidgetArray = input;
		m_Type = type;
		m_WidgetSetIdentifier = type;
		
		if (user_override != -1)
		{
			m_WidgetSetIdentifier = user_override;
		}
	}
	
	void Init(array<ref Widget> input, int type, Widget layout_root, int user_override = -1)
	{
		m_WidgetArray = input;
		m_Type = type;
		m_WidgetSetIdentifier = type;
		m_LayoutRoot = layout_root;
		
		if (user_override != -1)
		{
			m_WidgetSetIdentifier = user_override;
		}
	}
	
	array<ref Widget> GetWidgetSet()
	{
		return m_WidgetArray;
	}
	
	int GetWidgetSetType()
	{
		return m_Type;
	}
	
	int GetWidgetSetID()
	{
		return m_WidgetSetIdentifier;
	}
	
	//! Returns 'true' if this class contains update info
	bool HasDefinedHandle()
	{
		return false;
	}
	
	bool DataInitialized()
	{
		return true;
	}
	
	void UpdateVisibility(bool state){}
	void RegisterData(Param p){}
	void Update(float timeSlice = 0, Param p = null, int handle = -1){}
}

class GameplayEffectsDataImage extends GameplayEffectsData
{
	ref array<int> 	m_OriginalColors;
	ref array<int> 	m_CurrentColors;
	
	protected float 			m_SaturationMultiplier;//TODO ?
	
	void GameplayEffectsDataImage(array<ref Widget> input, int type, int user_override = -1)
	{
		ImageWidget w;
		m_OriginalColors = new array<int>;
		m_CurrentColors = new array<int>;
		
		for (int i = 0; i < input.Count(); i++)
		{
			if ( Class.CastTo(w,input.Get(i)) )
			{
				m_OriginalColors.Insert(w.GetColor());
			}
		}
		
		m_CurrentColors.Copy(m_OriginalColors);
	}
	
	void SetSaturationMultiplier(float value)
	{
		m_SaturationMultiplier = value;
	}
	
	float GetSaturationMultiplier()
	{
		return m_SaturationMultiplier;
	}
}

typedef map<int,ref GameplayEffectsData> GameplayEffectDataMap;