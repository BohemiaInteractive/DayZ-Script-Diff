//! Created once, on manager init. Script-side representation of C++ material class, separate handling.
class PPEClassBase
{
	protected PPEManager 											m_Manager;
	protected string 												m_MaterialPath = "";
	protected Material 												m_Material;
	
	protected ref map<int, ref array<int>>							m_ParameterUpdateQueueMap;
	protected ref array<int> 										m_ParameterRemovalQueue;
	protected ref array<int> 										m_UpdatedParameters;
	
	protected ref map<int,ref PPEMatClassParameterCommandData> 		m_MaterialParamMapStructure; //<param_idx, <ParamData>>
	
	void PPEClassBase(string mat_path_override = "")
	{
		Init(mat_path_override);
		CreateMaterial();
		CreateDataStructure();
		RegisterMaterialParameters();
	}
	
	protected void Init(string mat_path_override = "")
	{
		if (mat_path_override != "")
		{
			m_MaterialPath = mat_path_override;
		}
		else
		{
			m_MaterialPath = GetDefaultMaterialPath();
		}
		m_Manager = PPEManagerStatic.GetPPEManager();
	}
	
	protected void CreateMaterial()
	{
		if (m_MaterialPath != "")
			m_Material = GetGame().GetWorld().GetMaterial(m_MaterialPath);
	}
	
	Material GetMaterial()
	{
		return m_Material;
	}
	
	//do not override!
	protected void CreateDataStructure()
	{
		m_MaterialParamMapStructure = new map<int,ref PPEMatClassParameterCommandData>;
		//m_ParameterUpdateQueue = new array<int>;
		m_ParameterUpdateQueueMap = new map<int, ref array<int>>;
		m_ParameterRemovalQueue = new array<int>;
		m_UpdatedParameters = new array<int>;
	}
	
	//! inserted into associative array by parameter int value, parameter registration order does not matter (still ordered, though)
	protected void RegisterMaterialParameters();
	
	protected void RegisterParameterScalarBool(int idx, string parameter_name, bool default_value)
	{
		PPETemplateDefBool p = new PPETemplateDefBool(parameter_name,default_value);
		PPEMatClassParameterBool parameter_data = new PPEMatClassParameterBool(GetPostProcessEffectID(),idx,this);
		parameter_data.RegisterDefaults(p);
		m_MaterialParamMapStructure.Set(idx, parameter_data);
	}
	
	protected void RegisterParameterScalarInt(int idx, string parameter_name, int default_value, int min, int max)
	{
		PPETemplateDefInt p = new PPETemplateDefInt(parameter_name,default_value,min,max);
		
		PPEMatClassParameterInt parameter_data = new PPEMatClassParameterInt(GetPostProcessEffectID(),idx,this);
		parameter_data.RegisterDefaults(p);
		m_MaterialParamMapStructure.Set(idx, parameter_data);
	}
	
	//!WARNING - min/max values are usually taken from Workbench defaults, may not be actual min/max values the renderer can handle! When in doubt, try some higher/lower values...
	protected void RegisterParameterScalarFloat(int idx, string parameter_name, float default_value, float min, float max)
	{
		PPETemplateDefFloat p = new PPETemplateDefFloat(parameter_name,default_value,min,max);
		
		PPEMatClassParameterFloat parameter_data = new PPEMatClassParameterFloat(GetPostProcessEffectID(),idx,this);
		parameter_data.RegisterDefaults(p);
		m_MaterialParamMapStructure.Set(idx, parameter_data);
	}
	
	//!WARNING - min/max values are usually taken from Workbench defaults, may not be actual min/max values the renderer can handle! When in doubt, try some higher/lower values...
	protected void RegisterParameterScalarFloatEx(int idx, string parameter_name, float default_value, float min, float max, typename type)
	{
		PPETemplateDefFloat p = new PPETemplateDefFloat(parameter_name,default_value,min,max);
		
		PPEMatClassParameterFloat parameter_data;
		bool boo = Class.CastTo(parameter_data,type.Spawn());
		//Print("RegisterParameterColorEx: " + boo );
		parameter_data.RegisterDefaults(p);
		parameter_data.SetMaterialIndex(GetPostProcessEffectID());
		parameter_data.SetParameterIndex(idx);
		parameter_data.SetParent(this);
		m_MaterialParamMapStructure.Set(idx, parameter_data);
	}
	
	//!WARNING - min/max values are usually taken from Workbench defaults, may not be actual min/max values the renderer can handle! When in doubt, try some higher/lower values...
	protected void RegisterParameterColor(int idx, string parameter_name, float r, float g, float b, float a)
	{
		PPETemplateDefColor p = new PPETemplateDefColor(parameter_name,r,g,b,a);
		PPEMatClassParameterColor parameter_data = new PPEMatClassParameterColor(GetPostProcessEffectID(),idx,this);
		parameter_data.RegisterDefaults(p);
		m_MaterialParamMapStructure.Set(idx, parameter_data);
	}
	
	//!WARNING - min/max values are usually taken from Workbench defaults, may not be actual min/max values the renderer can handle! When in doubt, try some higher/lower values...
	protected void RegisterParameterColorEx(int idx, string parameter_name, float r, float g, float b, float a, typename type)
	{
		PPETemplateDefColor p = new PPETemplateDefColor(parameter_name,r,g,b,a);
		PPEMatClassParameterColor parameter_data;
		bool boo = Class.CastTo(parameter_data,type.Spawn());
		//Print("RegisterParameterColorEx: " + boo );
		parameter_data.RegisterDefaults(p);
		parameter_data.SetMaterialIndex(GetPostProcessEffectID());
		parameter_data.SetParameterIndex(idx);
		parameter_data.SetParent(this);
		m_MaterialParamMapStructure.Set(idx, parameter_data);
	}
	
	protected void RegisterParameterVector(int idx, string parameter_name, array<float> default_values) //needs to be compatible with every type of vector (vector2 to vector4), hence array<float>...
	{
		PPETemplateDefVector p = new PPETemplateDefVector(parameter_name,default_values);
		PPEMatClassParameterVector parameter_data = new PPEMatClassParameterVector(GetPostProcessEffectID(),idx,this);
		parameter_data.RegisterDefaults(p);
		m_MaterialParamMapStructure.Set(idx, parameter_data);
	}
	
	//TEXTURE and RESOURCE types are not overridable during runtime..currently unused and unhandled
	protected void RegisterParameterTexture(int idx, string parameter_name, string default_path)
	{
		PPETemplateDefTexture p = new PPETemplateDefTexture(parameter_name,default_path);
		PPEMatClassParameterTexture parameter_data = new PPEMatClassParameterTexture(GetPostProcessEffectID(),idx,this);
		parameter_data.RegisterDefaults(p);
		m_MaterialParamMapStructure.Set(idx, parameter_data);
	}
	
	protected void RegisterParameterResource(int idx, string parameter_name, string default_path)
	{
		PPETemplateDefResource p = new PPETemplateDefResource(parameter_name,default_path);
		PPEMatClassParameterResource parameter_data = new PPEMatClassParameterResource(GetPostProcessEffectID(),idx,this);
		parameter_data.RegisterDefaults(p);
		m_MaterialParamMapStructure.Set(idx, parameter_data);
	}
	
	//------------------------------------------------------------------------------------
	//! Distributes requester data to the material class structure and links them to appropriate parameter
	void InsertParamValueData(PPERequestParamDataBase request_data)
	{
		PPEMatClassParameterCommandData param_data;
		bool exists = m_MaterialParamMapStructure.Find(request_data.GetParameterID(),param_data);
		if ( !exists )
		{
			Error("PPEClassBase | InsertParamValueData | mat/par/req: " + GetPostProcessEffectID() + "/" + request_data.GetParameterID() + "/" + request_data.GetRequesterIDX() + " not registered in m_MaterialParamMapStructure!");
			return;
		}
		
		request_data.SetDataActive(true);
		request_data.SetUpdatingDataValues(true); //marks request data as updating
		param_data.InsertRequestData(request_data);//<request_ID, data>
		
		//DbgPrnt("PPEDebug | InsertParamValueData | mat/par/req: " + GetPostProcessEffectID() + "/" + request_data.GetParameterID() + "/" + request_data.GetRequesterIDX() + " | requester: " + request_data.m_Requester);
	}
	
	//TODO - rework
	//!unused, see 'RemoveActiveRequestFromMaterials' for more info
	void RemoveRequest(int req_idx)
	{
		/*for (int i = 0; i < m_ActiveMaterialRequestMap.Count(); i++)
		{
			ActiveParameterRequestsMap dbg = m_ActiveMaterialRequestMap.Get(i);
			//DbgPrnt("PPEDebug | dbg size: " + dbg.Count());
			
			if ( m_ActiveMaterialRequestMap.Get(i).Contains(req_idx) )
			{
				m_ActiveMaterialRequestMap.Get(i).Remove(req_idx);
				
				dbg = m_ActiveMaterialRequestMap.Get(i);
				
				if ( !m_ActiveMaterialRequestMap.Get(i) || m_ActiveMaterialRequestMap.Get(i).Count() < 1 )
				{
					if (m_ParameterUpdateQueue.IsValidIndex(i))
					{
						DbgPrnt("PPEDebug | PPEClassBase - RemoveRequest | Removing: " + i);
						DbgPrnt("PPEDebug | PPEClassBase - RemoveRequest | exit 4 - last request removal");
						m_ParameterRemovalQueue.Insert(m_ParameterUpdateQueue.Get(i));
					}
					else
						DbgPrnt("PPEDebug | PPEClassBase - RemoveRequest | Update queue no. " + i + " already removed!");
				}
			}
			//Adds to update one more time
			m_Manager.SetMaterialParamUpdating(GetPostProcessEffectID(),i);
		}*/
	}
	
	//! generic update method, take care when overriding!
	void OnUpdate(float timeslice, int order)
	{
		int parameter_idx = -1;
		//DbgPrnt("PPEDebug | PPEClassBase - OnUpdate | mat_id: " + GetPostProcessEffectID());
		
		if ( m_ParameterUpdateQueueMap.Contains(order) )
		{
			//Print(m_ParameterUpdateQueueMap.Get(order));
			for ( int i = 0; i < m_ParameterUpdateQueueMap.Get(order).Count(); i++ )
			{
				//DbgPrnt("PPEDebug | PPEClassBase - OnUpdate | parameter_idx: " + m_ParameterUpdateQueue.Get(i));
				Param p_values; //TODO - move to material classes?
				bool setting_defaults = false;
				parameter_idx = m_ParameterUpdateQueueMap.Get(order).Get(i);
				m_MaterialParamMapStructure.Get(parameter_idx).Update(timeslice,p_values,setting_defaults,order);
				
				InsertUpdatedParameter(parameter_idx);
			}
			m_Manager.InsertUpdatedMaterial(GetPostProcessEffectID());
			
			ParamUpdateQueueCleanup(order);
		}
		/*if ( !m_ParameterUpdateQueueMap.Contains(order) || m_ParameterUpdateQueueMap.Get(order).Count() < 1 )
			m_Manager.RemoveMaterialUpdating(GetPostProcessEffectID(),order); //stops material from updating when no parameters are.*/
	}
	
	//! Clamps the values being set to defaults, if there is no request setting non-zero values on the parameter
	void SetFinalParameterValue(int parameter_idx)
	{
		int var_type = GetParameterCommandData(parameter_idx).GetParameterVarType();
		Param values = GetParameterCommandData(parameter_idx).GetCurrentValues();
		
		switch (var_type)
		{
			case PPEConstants.VAR_TYPE_BOOL:
				bool value_var_bool = Param1<bool>.Cast(values).param1;
				
				m_Material.SetParamByIndex(parameter_idx,value_var_bool);
				//Print("DebugValues | PPEConstants.VAR_TYPE_BOOL | bool val: " + value_var_bool);
			break;
			
			case PPEConstants.VAR_TYPE_INT:
				int value_var_int = Param1<int>.Cast(values).param1;
				
				m_Material.SetParamByIndex(parameter_idx,value_var_int);
				//Print("DebugValues | PPEConstants.VAR_TYPE_BOOL | bool val: " + value_var_bool);
			break;
			
			case PPEConstants.VAR_TYPE_FLOAT:
				float value_var_float = Param1<float>.Cast(values).param1;
				
				m_Material.SetParamByIndex(parameter_idx,value_var_float);
				//Print("DebugValues | PPEConstants.VAR_TYPE_FLOAT | float val: " + value_var_float);
			break;
			
			case PPEConstants.VAR_TYPE_COLOR:
				float color[4] = {0,0,0,0};
				color[0] = Param4<float,float,float,float>.Cast(values).param1;
				color[1] = Param4<float,float,float,float>.Cast(values).param2;
				color[2] = Param4<float,float,float,float>.Cast(values).param3;
				color[3] = Param4<float,float,float,float>.Cast(values).param4;
				
				m_Material.SetParamByIndex(parameter_idx,color);
				//Print("DebugValues | PPEConstants.VAR_TYPE_COLOR | color val:: " + color[0] + " " + color[1] + " " + color[2] + " " + color[3]);
			break;
		}
	}
	
	void ApplyValueChanges()
	{
		int parameter_id;
		for (int i = 0; i < m_UpdatedParameters.Count(); i++)
		{
			parameter_id = m_UpdatedParameters.Get(i);
			SetFinalParameterValue(parameter_id);
		}
		
		m_UpdatedParameters.Clear();
	}
	
	protected void InsertUpdatedParameter(int mat_id)
	{
		if ( m_UpdatedParameters.Find(mat_id) == -1 )
			m_UpdatedParameters.Insert(mat_id);
	}
	
	//! Queue selected parameter for removal from the update queue
	void ParamUpdateRemove(int parameter_idx)
	{
		if ( m_ParameterRemovalQueue.Find(parameter_idx) == -1 )
			m_ParameterRemovalQueue.Insert(parameter_idx);
	}
	
	//! Queue specific parameter of this material to update
	void SetParameterUpdating(int order, int parameter_id)
	{
		if ( !m_ParameterUpdateQueueMap.Contains(order) )
		{
			m_ParameterUpdateQueueMap.Set(order,new array<int>);
		}
		
		if ( m_ParameterUpdateQueueMap.Get(order).Find(parameter_id) == -1 )
		{
			m_ParameterUpdateQueueMap.Get(order).Insert(parameter_id);
		}
	}
	
	protected void ParamUpdateQueueCleanup(int order)
	{
		//DbgPrnt("PPEDebug | PPEClassBase - ParamUpdateQueueCleanup | mat_id: " + GetPostProcessEffectID() + " | UpdateQueue count: " + m_ParameterUpdateQueue.Count());
		//DbgPrnt("PPEDebug | PPEClassBase - ParamUpdateQueueCleanup | mat_id: " + GetPostProcessEffectID() + " | RemovalQueue count: " + m_ParameterRemovalQueue.Count());
		for ( int i = 0; i < m_ParameterUpdateQueueMap.Get(order).Count(); i++ )
		{
			if ( m_ParameterRemovalQueue.Find(m_ParameterUpdateQueueMap.Get(order).Get(i)) != -1 )
			{
				//DbgPrnt("PPEDebug | PPEClassBase - ParamUpdateQueueCleanup | removing update of: " + m_ParameterUpdateQueue.Get(i));
				//m_ParameterUpdateQueue.RemoveItem(m_ParameterUpdateQueue.Get(i));
			}
		}
		m_ParameterUpdateQueueMap.Get(order).Clear();
	}
	
	//! override this if you want to use different path by default; '.emat' is appended automatically
	string GetDefaultMaterialPath();
	
	void ChangeMaterialPathUsed(string path)
	{
		m_MaterialPath = path;
		m_Material = null;
		CreateMaterial();
	}
	
	string GetCurrentMaterialPath()
	{
		return m_MaterialPath;
	}
	
	//! Overriden in all material classes!
	int GetPostProcessEffectID()
	{
		return PostProcessEffectType.None;
	}
	
	//! Some PP effects are handled as hard-coded exceptions, outside of material system. Default == PPEExceptions.NONE (systemic behaviour)
	/*int GetExceptionID()
	{
		return PPEExceptions.NONE;
	}*/
	
	PPEMatClassParameterCommandData GetParameterCommandData(int parameter_idx)
	{
		return m_MaterialParamMapStructure.Get(parameter_idx);
	}
	
#ifdef DEVELOPER
	//Debug//
	//-----//
	/*void DumpMap()
	{
		DbgPrnt("PPEClassDebug | m_ActiveMaterialRequestMap COUNT: " + m_ActiveMaterialRequestMap.Count());
		for (int i = 0; i < m_ActiveMaterialRequestMap.Count(); i++)
		{
			DbgPrnt("PPEClassDebug | m_ActiveRequest#: " + i);
			//ActiveMaterialRequests request = m_ActiveMaterialRequestMap.Get(i);
			DbgPrnt("PPEClassDebug | request: " + request);
			for (int j = 0; j < request.Count(); j++)
			{
				DbgPrnt("PPEClassDebug | request#: " + j);
				array<bool,float,int,int> values = request.Get(j);
				foreach (auto val : values)
				{
					DbgPrnt("PPEClassDebug | relative: " + val);
				}
			}
		}
		DbgPrnt("------------");
	}*/
#endif
	
	void DbgPrnt(string text)
	{
		//Debug.Log(""+text);
	}
};
