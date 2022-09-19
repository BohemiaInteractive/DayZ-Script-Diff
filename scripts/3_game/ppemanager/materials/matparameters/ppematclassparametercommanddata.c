typedef map<int,ref PPERequestParamDataBase> ActiveParameterRequestsMap; //<request_ID, data>

class PPEMatClassParameterCommandData
{
	//default layer constants, complex data like colors PPEMatClassParameterColor are handled separately
	const int LAYER_INFO_VALUE = 0;
	const int LAYER_INFO_OPERATOR = 1;
	
	ref array<int> 									m_CommandLayersArray; //for tracking active priorities and sorting them //TODO - could have been 'set'..
	protected int 									m_UpdatedCount;
	protected int 									m_MaterialIndex; //just a helper
	protected int 									m_ParameterIndex;
	protected ref ActiveParameterRequestsMap 		m_RequestMap;//<request_ID, parameter data>
	protected PPEClassBase 							m_Parent;
	protected ref Param 							m_Defaults; // Careful, formating is such, that param1 is ALWAYS string, containing parameter name. Actual values follow.
	protected ref Param 							m_CurrentValues; // Careful, only actual values, WITHOUT string
	
	protected ref map<int,ref array<int>> 			m_Dependencies;
	
	void PPEMatClassParameterCommandData(int mat_idx, int parameter_idx, PPEClassBase parent)
	{
		m_MaterialIndex = mat_idx;
		m_ParameterIndex = parameter_idx;
		m_Parent = parent;
		
		m_CommandLayersArray = new array<int>;
		m_UpdatedCount = 0;
		m_RequestMap = new ActiveParameterRequestsMap;
	}
	
	int GetParameterVarType()
	{
		return -1;
	}
	
	void SetMaterialIndex(int value)
	{
		m_MaterialIndex = value;
	}
	
	void SetParameterIndex(int value)
	{
		m_ParameterIndex = value;
	}
	
	void SetParent(PPEClassBase parent)
	{
		m_Parent = parent;
	}
	
	void InsertRequestData(PPERequestParamDataBase request_data)
	{
		m_RequestMap.Set(request_data.GetRequesterIDX(),request_data);//<request_ID, data>
	}
	
	void Update(float timeslice, out Param p_total, out bool setting_defaults, int order)
	{
		//insert dependencies to another update round
		if ( m_Dependencies && m_Dependencies.Count() > 0 && order < PPEConstants.DEPENDENCY_ORDER_HIGHEST )
		{
			int key_mat = -1;
			int element_par = -1;
			for (int i = 0; i < m_Dependencies.Count(); i++)
			{
				key_mat = m_Dependencies.GetKey(i);
				for (int j = 0; j < m_Dependencies.GetElement(i).Count(); j++)
				{
					element_par = m_Dependencies.GetElement(i).Get(j);
					PPEManagerStatic.GetPPEManager().SetMaterialParamUpdating(key_mat,element_par,order + 1); // TODO - revise, currently only does one more update
				}
			}
		}
	}
	
	//! Modifies values to be used for setter methods later in the manager update. Currently used only on PPEMatClassParameterColor, TODO!!
	void ModifyResultValues(inout Param result_values)
	{
	}
	
	//! Adds 'layers' to be iterated throug
	void AddPriorityInfo(int priority)
	{
		if ( m_CommandLayersArray.Find(priority) == -1 )
		{
			m_CommandLayersArray.Insert(priority);
			m_CommandLayersArray.Sort();
		}
		else
		{
			//DbgPrnt("PPEDebug | PPEMatClassParameterCommandData - AddPriorityInfo | Already exists, values have been overwritten!");
		}
	}
	
	//! Currently unused; layer info gets cleared every update
	void RemovePriorityInfo(int priority)
	{
	}
	
	//! Adds name and default values from material registration, override on children to properly add for each type
	void RegisterDefaults(Param p)
	{
		m_Defaults = p;
		InitDefaults();
		InitCuttent();
	}
	
	protected void InitDefaults() {};
	
	protected void InitCuttent() {};
	
	protected void SetParameterValueDefault(inout Param p_total)
	{
	}
	
	//! Careful, formating is such, that param1 is ALWAYS string, containing parameter name, should it be needed. Actual values follow.
	Param GetDefaultValues()
	{
		return m_Defaults;
	}
	
	//! Careful, only actual values, WITHOUT string
	Param GetCurrentValues()
	{
		return m_CurrentValues;
	}
	
	void DbgPrnt(string text)
	{
		//Debug.Log(""+text);
	}
}
