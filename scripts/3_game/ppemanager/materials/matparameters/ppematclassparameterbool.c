class PPEMatClassParameterBool extends PPEMatClassParameterCommandData
{
	protected ref map<int,ref array<bool,int>> m_LayerInfo; //<priority,<value,operator>>
	
	protected PPETemplateDefBool m_Bool;
	protected bool m_ValueDefault;
	
	void PPEMatClassParameterBool(int mat_idx, int parameter_idx, PPEClassBase parent)
	{
		m_LayerInfo = new map<int,ref array<bool,int>>;
	}
	
	override void InitDefaults()
	{
		Class.CastTo(m_Bool,m_Defaults);
		m_ValueDefault = m_Bool.param2;
	}
	
	override void InitCuttent()
	{
		m_CurrentValues = new Param1<bool>(m_ValueDefault);
	}
	
	override int GetParameterVarType()
	{
		return PPEConstants.VAR_TYPE_BOOL;
	}
	
	override void Update(float timeslice, out Param p_total, out bool setting_defaults, int order)
	{
		super.Update(timeslice,p_total,setting_defaults,order);
		
		protected int active_request_count = 0;
		
		protected PPERequestParamDataBool req_data;
		
		protected bool setting_value_zero = false;
		
		protected bool bool_value_temp = false;
		protected bool bool_value_total = m_ValueDefault;
		
		if (p_total == null)
		{
			p_total = new Param1<bool>(false);
		}
		
		if (m_RequestMap.Count() > 0)
		{
			m_LayerInfo.Clear();
		}
		else
		{
			//DbgPrnt("m_RequestMap.Count() is zero! Using default values. | mat/par: " + m_MaterialIndex + "/" + m_ParameterIndex);
			SetParameterValueDefault(p_total);
			m_Parent.ParamUpdateRemove(m_ParameterIndex);
			return;
		}
		
		for ( int i = 0; i < m_RequestMap.Count(); i++ )
		{
			req_data = PPERequestParamDataBool.Cast(m_RequestMap.GetElement(i));
			
			if (req_data == null)
			{
				Error("PPEMatClassParameterBool | req_data not found! | " + this + " | mat/par: " + m_MaterialIndex + "/" + m_ParameterIndex);
				continue;
			}
			
			setting_value_zero = req_data.IsSettingDefaultValues();
			
			if (setting_value_zero && !req_data.GetUpdatingDataValues() && !req_data.IsDataActive())
			{
				//DbgPrnt("Is Default, not updating | idx: " + i + " | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX());
				continue;
			}
			
			if (setting_value_zero)
			{
				req_data.m_BoolTarget = false;
				//DbgPrnt("PPEDebug | PPEMatClassParameterBool - UpdateParameterValues | !data.m_DefaultTargetSet | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | req_data.m_BoolTarget: " + req_data.m_BoolTarget);
			}
			else 
			{
				active_request_count++;
			}
			
			//evaluation
			//--------------------------------
			req_data.m_BoolLast = req_data.m_BoolCurrent;
			
			if (!req_data.GetUpdatingDataValues() && req_data.IsDataActive()) //set to exact value, not updating anymore
			{
				bool_value_temp = req_data.m_BoolCurrent;
				PrepareLayerInfo(req_data.GetPriorityLayer(),bool_value_temp,req_data.GetInteractionMask());
				//DbgPrnt("PPEDebug | PPEMatClassParameterBool - UpdateParameterValues | !req_data.m_UpdatingData | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | not updating, addaing current value into mix: " + bool_value_temp);
				continue;
			}
			
			bool_value_temp = req_data.m_BoolTarget;
			
			//DbgPrnt("PPEDebug | PPEMatClassParameterBool - UpdateParameterValues | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX());
			req_data.SetUpdatingDataValues(false);
			if (setting_value_zero)
			{
				req_data.SetDataActive(false);
				//RemovePriorityInfo(req_data.GetPriorityLayer()); //redundant?
			}
			else
			{
				bool_value_temp = req_data.m_BoolTarget;
				PrepareLayerInfo(req_data.GetPriorityLayer(),bool_value_temp,req_data.GetInteractionMask());
			}
			
			req_data.m_BoolCurrent = bool_value_temp;
		}
		
		if (active_request_count == 0)
		{
			SetParameterValueDefault(p_total);
		}
		else
		{
			//---------------------------
			//MASK handling and calculation
			bool value;
			int operator;
			bool override_active = false;
			
			for ( i = 0; i < m_LayerInfo.Count(); i++ )
			{
				if ( override_active )
					break;
				
				value = m_LayerInfo.Get(m_CommandLayersArray.Get(i)).Get(LAYER_INFO_VALUE);
				operator = m_LayerInfo.Get(m_CommandLayersArray.Get(i)).Get(LAYER_INFO_OPERATOR);
				
				switch (operator)
				{
					case PPOperators.LOWEST:
						bool_value_total = Math.Min(bool_value_total,value);
					break;
					
					case PPOperators.HIGHEST:
						bool_value_total = Math.Max(bool_value_total,value);
					break;
					
					case PPOperators.OVERRIDE:
						bool_value_total = value;
					break;
					
					case PPOperators.SET:
						bool_value_total = value;
					break;
					
					default:
						Error("PPEMatClassParameterBool | Invalid operator " + operator + " in mat/par: " + m_MaterialIndex + "/" + m_ParameterIndex);
					break;
				}
				
				if ( operator == PPOperators.OVERRIDE )
				{
					//DbgPrnt("m_LayerInfo | PPOperators.OVERRIDE at: " + i);
					override_active = true;
				}
			}
			
			m_CommandLayersArray.Clear();
			
			Param1<bool>.Cast(p_total).param1 = bool_value_total;
		}
		
		//DbgPrnt("PPEDebug | PPEMatClassParameterBool - UpdateParameterValues | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | parameter update end, removing from queue");
		m_Parent.ParamUpdateRemove(m_ParameterIndex);
		
		m_CurrentValues = p_total;
	}
	
	void PrepareLayerInfo(int layer, bool value, int operator)
	{
		m_LayerInfo.Set(layer,{value,operator});
		AddPriorityInfo(layer);
	}
	
	//! No active requests for the mat. parameter value change, sets the value to DEFAULT
	override void SetParameterValueDefault(inout Param p_total)
	{
		p_total = new Param1<bool>(PPETemplateDefBool.Cast(m_Defaults).param2);
		m_CurrentValues = p_total;
		//DbgPrnt("PPEDebug | PPEMatClassParameterBool - UpdateParameterValues | exit 3 - zero value");
	}
}
