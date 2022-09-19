
//! Manages all bleeding indicators and their updates
class GameplayEffectsDataBleeding extends GameplayEffectsData
{
	protected bool m_Initialized; //tied to initialization of 'BleedingSourcesManagerBase' on player object, skips updates until ready, even when formally active
	protected bool m_Visible; //overall visibility
	protected ref map<int,ref BleedingIndicator> m_RegisteredInstances;
	protected ref array<int> m_CleanupQueue;
	protected ref array<int> m_RunningIndicators;
	protected int m_LastDropIdx;
	protected int m_ImageWidgetCount; //number of available blood drop image widgets
	protected ref map<int,ref array<float>> m_ProbabilityMap;
	protected ref array<Widget> m_PositioningFramesArray;
	protected Widget m_BloodDropsFrame;
	
	protected int m_LastPositionFrameUsed;
	
	void GameplayEffectsDataBleeding(array<ref Widget> input, int type, int user_override = -1)
	{
		m_RegisteredInstances = new map<int,ref BleedingIndicator>;
		m_CleanupQueue = new array<int>;
		m_RunningIndicators = new array<int>;
		m_Initialized = false;
	}
	
	override void Init(array<ref Widget> input, int type, Widget layout_root, int user_override = -1)
	{
		super.Init(input, type, layout_root, user_override);
		
		m_WidgetArray.ShuffleArray(); //shuffles order of the widgets on every init
		m_ImageWidgetCount = m_WidgetArray.Count();
		m_BloodDropsFrame = m_LayoutRoot.FindAnyWidgetById(EffectWidgetsTypes.BLEEDING_LAYER);
		m_Visible = g_Game.GetProfileOption(EDayZProfilesOptions.BLEEDINGINDICATION);
		m_BloodDropsFrame.Show(m_Visible);
		m_LastDropIdx = -1;
		m_LastPositionFrameUsed = -1;
		
		BuildProbabilityData(BleedingIndicationConstants.INDICATOR_SEVERITY_LOW,BleedingIndicationConstants.SEQUENCE_DROP_AVERAGE_LOW);
		BuildProbabilityData(BleedingIndicationConstants.INDICATOR_SEVERITY_MEDIUM,BleedingIndicationConstants.SEQUENCE_DROP_AVERAGE_MEDIUM);
		BuildProbabilityData(BleedingIndicationConstants.INDICATOR_SEVERITY_HIGH,BleedingIndicationConstants.SEQUENCE_DROP_AVERAGE_HIGH);
		BuildPositioningData();
	}
	
	override bool HasDefinedHandle()
	{
		return true;
	}
	
	override bool DataInitialized()
	{
		return m_Initialized;
	}
	
	override void RegisterData(Param p)
	{
		if (m_Initialized)
		{
			#ifdef DEVELOPER
				ErrorEx("" + this + " is already initialized, further registration not possible!");
			#else
				Debug.Log("ERROR: " + this + " is already initialized, further registration not possible!");
			#endif
			return;
		}
		
		//<finish registration,bit,flow_modifier>
		Param3<bool,int,float> par;
		if (Class.CastTo(par,p))
		{
			if (par.param1 == true) //finish registration
			{
				m_Initialized = true;
				return;
			}
			
			RegisterBleedingIndicatorInstance(par.param2,par.param3);
		}
	}
	
	void RegisterBleedingIndicatorInstance(int bit, float flow_modifier)
	{
		int severity = CalculateSeverity(flow_modifier);
		BleedingIndicator indicator = new BleedingIndicator(bit,severity,this); //source_ID == bit for the purpose of pairing
		m_RegisteredInstances.Set(bit,indicator);
	}
	
	void SpawnBleedingIndicator(int source_ID, vector position)
	{
		if (m_RunningIndicators.Find(source_ID) != -1)
		{
			return;
		}
		BleedingIndicator indicator = m_RegisteredInstances.Get(source_ID);
		//indicator.InitIndicator(position); //would be nice if we could pair the indicator position to the hit position that caused the bleeding, wouldn't it
		indicator.InitIndicator(GenerateSequenceRandomPosition());
		m_RunningIndicators.Insert(source_ID);
	}
	
	int CalculateSeverity(float flow_modifier)
	{
		switch (flow_modifier)
		{
			case PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_LOW:
				return BleedingIndicationConstants.INDICATOR_SEVERITY_LOW;
			
			case PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM:
				return BleedingIndicationConstants.INDICATOR_SEVERITY_MEDIUM;
			
			case PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_HIGH:
				return BleedingIndicationConstants.INDICATOR_SEVERITY_HIGH;
		}
		return BleedingIndicationConstants.INDICATOR_SEVERITY_LOW;
	}
	
	ImageWidget GetNextDropImage()
	{
		m_LastDropIdx++;
		if (m_LastDropIdx > (m_ImageWidgetCount - 1))
		{
			m_LastDropIdx = 0;
		}
		
		return ImageWidget.Cast(m_WidgetArray[m_LastDropIdx]);
	}
	
	void StopBleedingIndicator(int source_ID, bool instant = false)
	{
		m_RegisteredInstances.Get(source_ID).StopIndicator(instant);
	}
	
	void UpdateBleedingIndicators(float timeSlice)
	{
		BleedingIndicator bib;
		for (int i = 0; i < m_RunningIndicators.Count(); i++)
		{
			bib = m_RegisteredInstances.Get(m_RunningIndicators[i]);
			bib.Update(timeSlice);
			if ( bib.GetEndNow() )
			{
				m_CleanupQueue.Insert(m_RunningIndicators[i]);
			}
		}
	}
	
	void CleanupBleedingIndicators()
	{
		for (int i = 0; i < m_CleanupQueue.Count(); i++)
		{
			m_RunningIndicators.RemoveItem(m_CleanupQueue[i]);
		}
		m_CleanupQueue.Clear();
	}
	
	override void Update(float timeSlice = 0, Param p = null, int handle = -1)
	{
		//starts or ends a bleeding indicator
		if (p)
		{
			//start/stop of the indicator
			//<state,bit,position,immediate>
			Param4<bool,int,vector,bool> par;
			
			//hide/show of bleeding effect visuals
			//<state>
			Param1<bool> parShow;
			
			if (Class.CastTo(par,p))
			{
				bool state = par.param1;
				if (state) //add indicator
				{
					SpawnBleedingIndicator(par.param2,par.param3);
				}
				else //stop indicator
				{
					StopBleedingIndicator(par.param2,par.param4);
				}
			}
			else if (Class.CastTo(parShow,p) && m_Visible != parShow.param1)
			{
				m_Visible = parShow.param1;
				m_BloodDropsFrame.Show(m_Visible);
			}
		}
		
		//updates bleeding indicators
		UpdateBleedingIndicators(timeSlice);
		CleanupBleedingIndicators();
	
		if (m_RunningIndicators.Count() <= 0)
		{
			GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.BLEEDING_LAYER});
			m_WidgetArray.ShuffleArray();
		}
	}
	
	override void UpdateVisibility(bool state)
	{
		m_Visible = g_Game.GetProfileOption(EDayZProfilesOptions.BLEEDINGINDICATION); //ugh
		//manage layout visibility
		Widget w = m_BloodDropsFrame;
		while (w && w != m_LayoutRoot && m_Visible == state && w.IsVisibleHierarchy() != state)
		{
			w.Show(state);
			w = w.GetParent();
		}
	}
	
	void BuildProbabilityData(int severity, float frequency)
	{
		if (!m_ProbabilityMap)
		{
			m_ProbabilityMap = new map<int,ref array<float>>;
		}
		
		array<float> probabilities = new array<float>;
		
		for (int i = 0; i < BleedingIndicationConstants.SEQUENCE_STEPS; i++)
		{
			probabilities.Insert(Math.Poisson(frequency,i));
		}
		
		m_ProbabilityMap.Insert(severity,probabilities);
	}
	
	array<float> GetProbabilities(int severity)
	{
		array<float> ret = m_ProbabilityMap.Get(severity);
		if (!ret)
		{
			ErrorEx("No defined data for the selected severity!");
		}
		return ret;
	}
	
	void BuildPositioningData()
	{
		Widget frameParent = m_LayoutRoot.FindAnyWidget("PoisitioningFrames");
		if (frameParent)
		{
			if (!m_PositioningFramesArray)
			{
				m_PositioningFramesArray = new array<Widget>;
			}
			
			Widget frame = frameParent.GetChildren();
			while (frame)
			{
				m_PositioningFramesArray.Insert(frame);
				frame = frame.GetSibling();
			}
		}
	}
	
	vector GenerateSequenceRandomPosition()
	{
		if (m_PositioningFramesArray)
		{
			int arrayCount = m_PositioningFramesArray.Count();
			int index = Math.RandomInt(0,arrayCount);
			if (index == m_LastPositionFrameUsed)
			{
				index++;
				if (index >= arrayCount)
				{
					index = 0;
				}
			}
			
			Widget frame = m_PositioningFramesArray[index];
			m_LastPositionFrameUsed = index;
			
			if (frame)
			{
				int screenX,screenY;
				float x,y,x_size,y_size;
				frame.GetScreenPos(x,y);
				frame.GetScreenSize(x_size,y_size);
				
				x = Math.RandomFloatInclusive(x,x+x_size);
				y = Math.RandomFloatInclusive(y,y+y_size);
				
				return Vector(x,y,0);
			}
		}
		return vector.Zero;
	}
}