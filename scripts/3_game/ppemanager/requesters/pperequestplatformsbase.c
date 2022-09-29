
class PPERequesterBase
{
	protected bool m_IsRunning; //helps determine if the requester has been stopped or not
	protected bool m_ValuesSent;
	protected int m_IDX;
	
	protected bool m_Valid = false;
	
	protected ref map<int,ref map<int,ref PPERequestParamDataBase>> m_RequestDataStructure; // <material<param_id,data>> // parameter data objects are singletons
	
	void PPERequesterBase(int requester_IDX)
	{
		m_Valid = PPERequesterBank.VerifyRequester(this);
		
		m_IDX = requester_IDX;
		m_ValuesSent = true;
		m_IsRunning = false;
		m_RequestDataStructure = new map<int,ref map<int,ref PPERequestParamDataBase>>;
	}
	
	//! Has to be set for the requester to be handled
	void SetRequesterUpdating(bool state)
	{
		if (!m_Valid)
		{
			Debug.Log("" + this + " not valid!");
			return;
		}
		
		PPEManagerStatic.GetPPEManager().SetRequestUpdating(this,state);
		
		//TODO - separate into its own function?
		if (state)
			PPEManagerStatic.GetPPEManager().SetRequestActive(this,true);
	}
	
	void Start(Param par = null)
	{
		OnStart(par);
		m_IsRunning = true;
	}
	
	void Stop(Param par = null)
	{
		if (!m_RequestDataStructure || m_RequestDataStructure.Count() <= 0)
			return;
		
		OnStop(par);
		m_IsRunning = false;
	}
	
	bool IsRequesterRunning()
	{
		return m_IsRunning;
	}
	
	//! automatically assigned by PPERequesterBank
	void SetRequesterIDX(int idx)
	{
		m_IDX = idx;
	}
	
	//! Returns requester index
	int GetRequesterIDX()
	{
		return m_IDX;
	}
	
	//!mask for mass-handling methods like 'StopAllEffects'
	int GetCategoryMask()
	{
		return PPERequesterCategory.NONE;
	}
	
	//SETTERS//
	//vvvvvvv//
	protected void SetTargetValueBool(int mat_id, int param_idx, bool val, int priority_layer, int operator = PPOperators.SET)
	{
		if ( !m_RequestDataStructure )
			return;
		
		PPERequestParamDataBool data = PPERequestParamDataBool.Cast(GetRequestData(mat_id,param_idx,PPEConstants.VAR_TYPE_BOOL));
		data.SetPriorityLayer(priority_layer);
		data.SetInteractionMask(operator);
		
		data.m_BoolTarget = val;
		data.m_BoolLast = data.m_BoolCurrent;
		
		if (data.m_BoolTarget != data.m_BoolCurrent || data.IsSettingDefaultValues())
		{
			data.SetSettingDefaultValues(false);
			data.SetDataActive(true);
			
			QueueValuesSend();
		}
		//DbgPrnt("PPEDebug | PPERequesterBase - SetTargetValueBool | mat/par/req: " + mat_id + "/" + param_idx + "/" + m_IDX + " | data: " + data);
	}
	
	protected void SetTargetValueBoolDefault(int mat_id, int param_idx)
	{
		//TODO - running check and cleanup rework (elsewhere!)
		if ( !m_RequestDataStructure || !m_RequestDataStructure.Get(mat_id) || !m_RequestDataStructure.Get(mat_id).Get(param_idx))
			return;
		
		PPERequestParamDataBool data = PPERequestParamDataBool.Cast(GetRequestData(mat_id,param_idx,PPEConstants.VAR_TYPE_BOOL));
		data.m_BoolLast = data.m_BoolCurrent;
		if (!data.IsSettingDefaultValues())
		{
			data.SetSettingDefaultValues(true);
			
			QueueValuesSend();
		}
		//DbgPrnt("PPEDebug | PPERequesterBase - SetTargetValueBoolDefault | mat/par/req: " + mat_id + "/" + param_idx + "/" + m_IDX + " | data: " + data);
	}
	
	protected void SetTargetValueInt(int mat_id, int param_idx, bool relative, int val, int priority_layer, int operator = PPOperators.SET)
	{
		if ( !m_RequestDataStructure )
			return;
		
		PPERequestParamDataInt data = PPERequestParamDataInt.Cast(GetRequestData(mat_id,param_idx,PPEConstants.VAR_TYPE_INT));
		data.SetPriorityLayer(priority_layer);
		data.SetInteractionMask(operator);
		
		data.m_IntTarget = val;
		data.m_IntLast = data.m_IntCurrent;
		if (data.m_IntTarget != data.m_IntCurrent || data.IsSettingDefaultValues())
		{
			data.SetSettingDefaultValues(false);
			data.SetDataActive(true);
			
			QueueValuesSend();
		}
		//DbgPrnt("PPEDebug | PPERequesterBase - SetTargetValueInt | mat/par/req: " + mat_id + "/" + param_idx + "/" + m_IDX + " | data: " + data);
	}
	
	protected void SetTargetValueIntDefault(int mat_id, int param_idx)
	{
		//TODO - running check and cleanup rework (elsewhere!)
		if ( !m_RequestDataStructure || !m_RequestDataStructure.Get(mat_id) || !m_RequestDataStructure.Get(mat_id).Get(param_idx))
			return;
		
		PPERequestParamDataInt data = PPERequestParamDataInt.Cast(GetRequestData(mat_id,param_idx,PPEConstants.VAR_TYPE_INT));
		data.m_IntLast = data.m_IntCurrent;
		if (!data.IsSettingDefaultValues())
		{
			data.SetSettingDefaultValues(true);
			
			QueueValuesSend();
		}
		//DbgPrnt("PPEDebug | PPERequesterBase - SetTargetValueIntDefault | mat/par/req: " + mat_id + "/" + param_idx + "/" + m_IDX + " | data: " + data);
	}
	
	protected void SetTargetValueFloat(int mat_id, int param_idx, bool relative, float val, int priority_layer, int operator = PPOperators.ADD_RELATIVE)
	{
		if ( !m_RequestDataStructure )
			return;
		
		PPERequestParamDataFloat data = PPERequestParamDataFloat.Cast(GetRequestData(mat_id,param_idx,PPEConstants.VAR_TYPE_FLOAT));
		data.SetPriorityLayer(priority_layer);
		data.SetInteractionMask(operator);
		
		data.m_FloatFormerTarget = data.m_FloatTarget;
		data.m_FloatTarget = RelativizeValue(val,PPEConstants.VAR_TYPE_FLOAT,mat_id,param_idx,relative);
		data.m_FloatLast = data.m_FloatCurrent;
		data.m_FloatStart = data.m_FloatCurrent;
		if (data.m_FloatTarget != data.m_FloatCurrent || data.IsSettingDefaultValues())
		{
			data.SetSettingDefaultValues(false);
			data.SetDataActive(true);
			
			QueueValuesSend();
		}
		//DbgPrnt("PPEDebug | PPERequesterBase - SetTargetValueFloat | mat/par/req: " + mat_id + "/" + param_idx + "/" + m_IDX + " | data: " + data);
	}
	
	protected void SetTargetValueFloatDefault(int mat_id, int param_idx)
	{
		//TODO - running check and cleanup rework (elsewhere!)
		if ( !m_RequestDataStructure || !m_RequestDataStructure.Get(mat_id) || !m_RequestDataStructure.Get(mat_id).Get(param_idx))
			return;
		
		PPERequestParamDataFloat data = PPERequestParamDataFloat.Cast(GetRequestData(mat_id,param_idx,PPEConstants.VAR_TYPE_FLOAT));
		data.m_FloatFormerTarget = data.m_FloatTarget;
		data.m_FloatLast = data.m_FloatCurrent;
		data.m_FloatStart = data.m_FloatCurrent;
		if (!data.IsSettingDefaultValues())
		{
			data.SetSettingDefaultValues(true);
			
			QueueValuesSend();
		}
		//DbgPrnt("PPEDebug | PPERequesterBase - SetTargetValueFloatDefault | mat/par/req: " + mat_id + "/" + param_idx + "/" + m_IDX + " | data: " + data);
	}
	
	protected void SetTargetValueColor(int mat_id, int param_idx, ref array<float> val, int priority_layer, int operator = PPOperators.ADD_RELATIVE)
	{
		if ( !m_RequestDataStructure )
			return;
		
		PPERequestParamDataColor data = PPERequestParamDataColor.Cast(GetRequestData(mat_id,param_idx,PPEConstants.VAR_TYPE_COLOR));
		data.SetPriorityLayer(priority_layer);
		data.SetInteractionMask(operator);
		data.m_ColorFormerTarget = data.m_ColorTarget;
		
		data.m_ColorTarget.Copy(val);
		
		data.m_ColorLast = data.m_ColorCurrent;
		data.m_ColorStart = data.m_ColorCurrent;
		//if (data.m_ColorTarget != data.m_ColorCurrent)
		{
			data.SetSettingDefaultValues(false);
			data.SetDataActive(true);
					
			QueueValuesSend();
		}
		//DbgPrnt("PPEDebug | PPERequesterBase - SetTargetValueColor | mat/par/req: " + mat_id + "/" + param_idx + "/" + m_IDX + " | data: " + data);
	}
	
	protected void SetTargetValueColorDefault(int mat_id, int param_idx)
	{
		//TODO - running check and cleanup rework (elsewhere!)
		if ( !m_RequestDataStructure || !m_RequestDataStructure.Get(mat_id) || !m_RequestDataStructure.Get(mat_id).Get(param_idx))
			return;
		
		PPERequestParamDataColor data = PPERequestParamDataColor.Cast(GetRequestData(mat_id,param_idx,PPEConstants.VAR_TYPE_COLOR));
		data.m_ColorFormerTarget = data.m_ColorTarget;
		data.m_ColorLast = data.m_ColorCurrent;
		data.m_ColorStart = data.m_ColorCurrent;
		if (!data.IsSettingDefaultValues())
		{
			data.SetSettingDefaultValues(true);
			
			QueueValuesSend();
		}
		//DbgPrnt("PPEDebug | PPERequesterBase - SetTargetValueColorDefault | mat/par/req: " + mat_id + "/" + param_idx + "/" + m_IDX + " | data: " + data);
	}
	
	/*protected void SetTargetValueVector(int mat_id, int param_idx, bool relative, vector val, int priority_layer, int operator = PPOperators.ADD_RELATIVE)
	{
	}
	
	void SetTargetValueTexture(int mat_id, int param_idx, string val, int priority_layer, int mask, bool override_priority = false)
	{
	}*/
	
	/*void SetTargetValueResource(int mat_id, int param_idx, string val, int priority_layer, int mask, bool override_priority = false)
	{
	}*/
	
	//! Sets this requester to update in the manager
	protected void QueueValuesSend()
	{
		m_ValuesSent = false;
		//m_IsRunning = true;
		SetRequesterUpdating(true);
	}
	
	//! returns request data for specific parameter, or creates one if one does not exist in this request yet (singletons)
	PPERequestParamDataBase GetRequestData(int mat_id, int param_idx, int type)
	{
		PPERequestParamDataBase data;
		if ( m_RequestDataStructure.Get(mat_id) )
		{
			if (m_RequestDataStructure.Get(mat_id).Get(param_idx))
			{
				data = m_RequestDataStructure.Get(mat_id).Get(param_idx);
			}
			else
			{
				data = PrepareData(mat_id,param_idx,type);
				m_RequestDataStructure.Get(mat_id).Set(param_idx,data);
			}
		}
		else
		{
			data = PrepareData(mat_id,param_idx,type);
			map<int,ref PPERequestParamDataBase> temp_map = new map<int,ref PPERequestParamDataBase>;
			temp_map.Set(param_idx,data);
			m_RequestDataStructure.Set(mat_id, temp_map);
		}
		
		if (!data)
		{
			Error("GetRequestData | no data found in the structure");
		}
		
		data.m_Requester = this;
		
		return data;
	}
	
	protected PPERequestParamDataBase PrepareData(int mat_id, int param_idx, int type)
	{
		PPERequestParamDataBase data;
		switch (type)
		{
			case PPEConstants.VAR_TYPE_BOOL:
				data = new PPERequestParamDataBool(m_IDX, mat_id, param_idx);
			break;
			
			case PPEConstants.VAR_TYPE_INT:
				data = new PPERequestParamDataInt(m_IDX, mat_id, param_idx);
			break;
			
			case PPEConstants.VAR_TYPE_FLOAT:
				data = new PPERequestParamDataFloat(m_IDX, mat_id, param_idx);
			break;
			
			case PPEConstants.VAR_TYPE_COLOR:
				data = new PPERequestParamDataColor(m_IDX, mat_id, param_idx);
			break;
		}
		return data;
	}
	
	// unused?
	map<int,ref map<int,ref PPERequestParamDataBase>> GetActiveRequestStructure()
	{
		return m_RequestDataStructure;
	}
	
	protected void ClearRequesterData()
	{
		m_RequestDataStructure.Clear();
	}
	
	protected void SendCurrentValueData(PPERequestParamDataBase data)
	{
		PPEManagerStatic.GetPPEManager().SendMaterialValueData(data);
	}
	
	//EVENTS//
	//vvvvvv//
	void OnUpdate(float delta)
	{
		//DbgPrnt("PPEDebug | PPERequesterBase | OnUpdate");
		
		if ( !m_ValuesSent )
		{
			for (int i = 0; i < m_RequestDataStructure.Count(); i++)
			{
				for (int j = 0; j < m_RequestDataStructure.GetElement(i).Count(); j++)
				{
					SendCurrentValueData(m_RequestDataStructure.GetElement(i).GetElement(j)); //map<int,ref map<int,PPERequestParamDataBase>> //<material<param,data>>
				}
			}
			m_ValuesSent = true;
		}
		SetRequesterUpdating(false);
	}
	
	protected void OnStart(Param par = null)
	{
		SetRequesterUpdating(true);
	}
	
	//! automatically resets all material parameters of this requester to defaults
	protected void OnStop(Param par = null)
	{
		//DbgPrnt("PPEDebug | " + this + " | OnStop");
		SetRequesterUpdating(true);
		SetDefaultValuesAll();
	}
	
	//! converts all values used to relative values (if not relative already)
	protected float RelativizeValue(float value, int var_type, int mat_id, int param_id, bool relative)
	{
		float ret = value;
		if (!relative) //if not already relative...
		{
			switch (var_type)
			{
				case PPEConstants.VAR_TYPE_FLOAT:
					PPETemplateDefFloat p_float = PPETemplateDefFloat.Cast(PPEManagerStatic.GetPPEManager().GetPostProcessDefaultValues(mat_id,param_id));
					ret = Math.InverseLerp(p_float.param3, p_float.param4, ret);
					ret = Math.Clamp(ret,0.0,1.0);
				break;
			}
		}
		//Print("RelativizeValue: " + ret);
		
		return ret;
	}
	
	//! Sets all requested values to default, ignoring them in further calculations.
	protected void SetDefaultValuesAll()
	{
		//<material<param_id,data>>
		PPERequestParamDataBase req_data;
		
		for (int i = 0; i < m_RequestDataStructure.Count(); i++)
		{
			for (int j = 0; j < m_RequestDataStructure.GetElement(i).Count(); j++)
			{
				req_data = m_RequestDataStructure.GetElement(i).GetElement(j);
				
				if (!req_data.IsDataActive())
					continue;
				
				switch (req_data.GetVarType())
				{
					case PPEConstants.VAR_TYPE_BOOL:
						SetTargetValueBoolDefault(req_data.GetMaterialID(),req_data.GetParameterID());
					break;
					
					case PPEConstants.VAR_TYPE_INT:
						SetTargetValueIntDefault(req_data.GetMaterialID(),req_data.GetParameterID());
					break;
					
					case PPEConstants.VAR_TYPE_FLOAT:
						SetTargetValueFloatDefault(req_data.GetMaterialID(),req_data.GetParameterID());
					break;
					
					case PPEConstants.VAR_TYPE_COLOR:
						SetTargetValueColorDefault(req_data.GetMaterialID(),req_data.GetParameterID());
					break;
				}
			}
		}
	}
	
	void DbgPrnt(string text)
	{
		//Debug.Log(""+text);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//							Base requester platforms								//
//////////////////////////////////////////////////////////////////////////////////////

class PPERequester_GameplayBase extends PPERequesterBase
{
	override int GetCategoryMask()
	{
		return PPERequesterCategory.GAMEPLAY_EFFECTS;
	}
}

class PPERequester_MenuBase extends PPERequesterBase
{
	override int GetCategoryMask()
	{
		return PPERequesterCategory.MENU_EFFECTS;
	}
}
