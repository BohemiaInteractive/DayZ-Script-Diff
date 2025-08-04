class UniversalTemperatureSourceSettings
{
	float m_UpdateInterval		= 1.0;			//! how often the Update is ticking
	float m_TemperatureItemCap 	= GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE; //! max temperature 'non-IsSelfAdjustingTemperature' entity in vicinity will get per update (cap);
	float m_TemperatureItemCoef = GameConstants.TEMP_COEF_UTS; //! used to determine speed of temperature change, and some temperature subsystems
	float m_TemperatureCap		= float.MAX;	//! temperature cap that will limit the return value from GetTemperature method
	float m_RangeFull			= 1;			//! range where the full temperature is given to receiver
	float m_RangeMax			= 2;			//! maximum range where the receiver can get some temperature
	
	bool m_EnableOnTemperatureControl		= false;	//! enable or disable activation/deactivation on set temperature
	float m_ActiveTemperatureThreshold 		= 0.0; 	 	//! UTS will be active on temperature >= to this value
	float m_InactiveTemperatureThreshold 	= 0.0;	 	//! UTS will be inactive on temperature < to this value
	
	bool m_Updateable 			= false;		//! if the Update is running periodically
	bool m_ManualUpdate  		= false;		//! update is called manually (ex. own tick of parent entity)
	bool m_IsWorldOverriden		= true;			//! if the stats can be overriden by coefficient/variables from WorldData (currently TemperatureCap only)

	vector m_Position			= vector.Zero;
	EntityAI m_Parent			= null;			//! parent Entity of the UTS

	//! DEPRECATED
	bool m_AffectStat			= false;
	float m_TemperatureMin		= 0;
	float m_TemperatureMax		= 100;
}

class UniversalTemperatureSourceResult
{
	float m_TemperatureItem = 0; //! Item target temperature
	float m_TemperatureHeatcomfort = 0; //! Player HC target(?) value
	
	//! DEPRECATED
	float m_Temperature = 0;
}

//! original Timer deletes m_params which is unwanted
class UniversalTemperatureSourceTimer : Timer
{
	override void OnTimer()
	{
		if (m_params)
		{
			GetGame().GameScript.CallFunctionParams(m_target, m_function, null, m_params);
		}
		else
		{
			GetGame().GameScript.CallFunction(m_target, m_function, null, 0);
		}
	}
	
	override void Stop()
	{
		SetRunning(false);
		m_time = 0;
	}
	
	void SetParams(Param params)
	{
		m_params = params;
	}
}

typedef UniversalTemperatureSource UTemperatureSource;

class UniversalTemperatureSource
{
	private float m_ParentTemperaturePrevious;

	protected bool 										m_Active
	protected ref UniversalTemperatureSourceTimer 		m_Timer;
	protected UniversalTemperatureSourceSettings 		m_Settings;
	protected ref UniversalTemperatureSourceResult 		m_ResultValues;
	protected ref UniversalTemperatureSourceLambdaBase 	m_Lambda;

	void UniversalTemperatureSource(EntityAI pParent, UniversalTemperatureSourceSettings pSettings, UniversalTemperatureSourceLambdaBase pLambda)
	{
		m_Active			= false;
		m_Settings			= pSettings;
		m_Lambda			= pLambda;
		m_ResultValues 		= new UniversalTemperatureSourceResult();
		m_Timer				= new UniversalTemperatureSourceTimer();
		
		Init(pParent);
	}
	
	void ~UniversalTemperatureSource() 
	{
		if (m_Timer)
			m_Timer.Stop();
	};
	
	void Init(EntityAI pParent)
	{
		if (pParent)
		{
			pParent.SetUniversalTemperatureSource(this);
	
			m_Settings.m_Parent 	= pParent;
			m_Settings.m_Position	= pParent.GetPosition();
		}
		
		if (m_Settings && !m_Settings.m_ManualUpdate)
		{
			auto params = new Param2<UniversalTemperatureSourceSettings, UniversalTemperatureSourceLambdaBase>(m_Settings, m_Lambda);
	
			m_Timer.Run(m_Settings.m_UpdateInterval, this, "Update", params, m_Settings.m_Updateable);
			SetActive(false);
		}
		
		if (m_Settings && m_Settings.m_IsWorldOverriden)
			m_Settings.m_TemperatureCap += g_Game.GetMission().GetWorldData().GetUniversalTemperatureSourceCapModifier();
	}
	
	vector GetPosition()
	{
		return m_Settings.m_Position;
	}
	
	float GetFullRange()
	{
		return m_Settings.m_RangeFull;
	}

	float GetMaxRange()
	{
		return m_Settings.m_RangeMax;
	}
	
	float GetTemperatureCap()
	{
		return m_Settings.m_TemperatureCap;
	}
	
	float GetTemperatureItemCap()
	{
		return m_Settings.m_TemperatureItemCap;
	}
	
	float GetTemperatureItemCoef()
	{
		return m_Settings.m_TemperatureItemCoef;
	}
	
	EntityAI GetParent()
	{
		return m_Settings.m_Parent;
	}
	
	UniversalTemperatureSourceLambdaBase GetLambda()
	{
		return m_Lambda;
	}
	
	bool IsActive()
	{
		if (m_Settings && m_Settings.m_ManualUpdate)
		{
			return m_Active;
		}

		return m_Timer && m_Timer.IsRunning();
	}
	
	void SetActive(bool pActive)
	{
		if (pActive)
			m_Lambda.OnUTSActivate();
		else
			m_Lambda.OnUTSDeactivate();
		
		if (m_Settings && m_Settings.m_ManualUpdate)
		{
			m_Active = pActive;
			return;
		}

		if (pActive && !m_Timer.IsRunning())
		{
			m_Timer.Continue();
		}
		else
		{
			m_Timer.Stop();
		}
	}
	
	void SetDefferedActive(bool pActive, float pSeconds)
	{
		if (m_Settings)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLaterByName(this, "SetActive", pSeconds * 1000, false, new Param1<bool>(pActive));
	}
	
	void ChangeSettings(UniversalTemperatureSourceSettings pSettings)
	{
		if (m_Settings)
			m_Timer.SetParams(new Param2<UniversalTemperatureSourceSettings, UniversalTemperatureSourceLambdaBase>(m_Settings, m_Lambda));
	}

	void Update(UniversalTemperatureSourceSettings settings, UniversalTemperatureSourceLambdaBase lambda)
	{
		if (!settings)
			return;
		
		if (settings.m_EnableOnTemperatureControl)
		{
			float parentTemperature = GetParent().GetTemperature();
			float temperatureDifference = parentTemperature - m_ParentTemperaturePrevious;

			if (parentTemperature >= settings.m_ActiveTemperatureThreshold && temperatureDifference > 0)
				SetActive(true);

			if (parentTemperature < settings.m_InactiveTemperatureThreshold && temperatureDifference < 0)
				SetActive(false);
			
			m_ParentTemperaturePrevious = parentTemperature;
		}

		if (!IsActive())
			return;
		
		if (lambda)
		{
			settings.m_Position = settings.m_Parent.GetUniversalTemperatureSourcePosition();
			lambda.OnBeforeExecute();
			lambda.Execute(settings, m_ResultValues);
			lambda.OnAfterExecute();
		}

	}
	
	//!
	//!DEPRECATED
	//!
	float GetTemperatureMin()
	{
		return m_Settings.m_TemperatureMin;
	}
	
	float GetTemperatureMax()
	{
		return m_Settings.m_TemperatureMax;
	}
	
	float GetTemperature()
	{
		return GetTemperatureRaw();
	}
	
	float GetTemperatureRaw()
	{
		if (m_ResultValues)
			return m_ResultValues.m_Temperature;

		return 0;
	}
}

typedef UniversalTemperatureSourceDebug UTemperatureSourceDebug

class UniversalTemperatureSourceDebug
{
	const string DELIMITER_DATA		= "|";
	const string DELIMITER_KEYPAIR	= ":";

	string m_Header;
	string m_Data;
	ref array<string> m_Names;		//! names parsed from m_Pairs
	ref array<string> m_Values;		//! values parsed from m_Pairs
	ref array<string> m_Pairs;		//! keeps first iteration of parsed data from the m_Data
	
	void UniversalTemperatureSourceDebug()
	{
		m_Header 		= "";
		m_Data 			= "";
		m_Pairs			= new array<string>();
		m_Names			= new array<string>();
		m_Values		= new array<string>();
	}
	
	void AddHeader(string header)
	{
		m_Header = header;
	}
	
	void Add(string name, string value)
	{
		m_Data = string.Format("%1%2:%3%4", m_Data, name, value, DELIMITER_DATA);	
	}
	
	void Commit()
	{
		m_Pairs = ParseData();
		ParseKeyPairs();
	}

	int PairsCount()
	{
		return m_Pairs.Count();
	}
	
	string GetHeader()
	{
		return m_Header;
	}
	
	string GetName(int pIndex)
	{
		if (m_Names.Count() - 1 < pIndex)
		{
			Debug.Log(string.Format("GetName index: %1 from data of length: %2", pIndex, m_Names.Count()), "UniversalTemperatureSourceDebug");
			return "";
		}
		
		return m_Names.Get(pIndex);
	}
	
	string GetValue(int pIndex)
	{
		if (m_Values.Count() - 1 < pIndex)
		{
			Debug.Log(string.Format("GetValue index: %1 from data of length: %2", pIndex, m_Values.Count()), "UniversalTemperatureSourceDebug");
			return "";
		}
		
		return m_Values.Get(pIndex);
	}
	
	protected array<string> ParseData()
	{
		
		array<string> parsed = new array<string>();
		if (m_Data)
		{
			m_Data.Split(DELIMITER_DATA, parsed);
		}
		
		return parsed;
	}
	
	protected void ParseKeyPairs()
	{
		m_Names.Clear();
		m_Values.Clear();

		if (m_Pairs)
		{
			for (int i = 0; i < m_Pairs.Count(); i++)
			{
				array<string> keypair = new array<string>();

				m_Pairs.Get(i).Split(DELIMITER_KEYPAIR, keypair);
				m_Names.Insert(keypair[0]);
				m_Values.Insert(keypair[1]);
			}
		}
	}
	
	void Debug()
	{
		for (int i = 0; i < m_Names.Count(); i++)
		{
			Debug.Log(string.Format("%1: %2", m_Names.Get(i), m_Values.Get(i)), "UniversalTemperatureSourceDebug");
		}
	}
}
