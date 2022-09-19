class PPEMatClassParameterInt extends PPEMatClassParameterCommandData
{
	protected ref map<int,ref array<int,int>> m_LayerInfo; //<priority,<value,operator>>
	
	protected PPETemplateDefInt m_Int;
	protected int m_ValueDefault;
	
	void PPEMatClassParameterInt(int mat_idx, int parameter_idx, PPEClassBase parent)
	{
		m_LayerInfo = new map<int,ref array<int,int>>;
	}
	
	override void InitDefaults()
	{
		Class.CastTo(m_Int,m_Defaults);
		m_ValueDefault = m_Int.param2;
	}
	
	override void InitCuttent()
	{
		m_CurrentValues = new Param1<int>(m_ValueDefault);
	}
	
	override int GetParameterVarType()
	{
		return PPEConstants.VAR_TYPE_INT;
	}
	
	override void Update(float timeslice, out Param p_total, out bool setting_defaults, int order)
	{
		super.Update(timeslice,p_total,setting_defaults,order);
		
		protected int active_request_count = 0;
		
		protected PPERequestParamDataInt req_data;
		
		protected bool setting_value_zero = false;
		
		protected bool int_value_temp = false;
		protected bool int_value_total = m_ValueDefault;
		
		if (p_total == null)
		{
			p_total = new Param1<int>(0);
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
			req_data = PPERequestParamDataInt.Cast(m_RequestMap.GetElement(i));
			
			if (req_data == null)
			{
				Error("Debug | PPEMatClassParameterInt | req_data not found! | " + this + " | mat/par: " + m_MaterialIndex + "/" + m_ParameterIndex);
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
				req_data.m_IntTarget = false;
				//DbgPrnt("PPEDebug | PPEMatClassParameterInt - UpdateParameterValues | !data.m_DefaultTargetSet | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | req_data.m_IntTarget: " + req_data.m_IntTarget);
			}
			else 
			{
				active_request_count++;
			}
			
			//evaluation
			//--------------------------------
			req_data.m_IntLast = req_data.m_IntCurrent;
			
			if (!req_data.GetUpdatingDataValues() && req_data.IsDataActive()) //set to exact value, not updating anymore
			{
				int_value_temp = req_data.m_IntCurrent;
				PrepareLayerInfo(req_data.GetPriorityLayer(),int_value_temp,req_data.GetInteractionMask());
				//DbgPrnt("PPEDebug | PPEMatClassParameterInt - UpdateParameterValues | !req_data.m_UpdatingData | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | not updating, addaing current value into mix: " + int_value_temp);
				continue;
			}
			
			int_value_temp = req_data.m_IntTarget;
			
			//DbgPrnt("PPEDebug | PPEMatClassParameterInt - UpdateParameterValues | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX());
			req_data.SetUpdatingDataValues(false);
			if (setting_value_zero)
			{
				req_data.SetDataActive(false);
				//RemovePriorityInfo(req_data.GetPriorityLayer()); //redundant?
			}
			else
			{
				int_value_temp = req_data.m_IntTarget;
				PrepareLayerInfo(req_data.GetPriorityLayer(),int_value_temp,req_data.GetInteractionMask());
			}
			
			req_data.m_IntCurrent = int_value_temp;
		}
		
		if (active_request_count == 0)
		{
			SetParameterValueDefault(p_total);
		}
		else
		{
			//---------------------------
			//MASK handling and calculation
			int value;
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
						int_value_total = Math.Min(int_value_total,value);
					break;
					
					case PPOperators.HIGHEST:
						int_value_total = Math.Max(int_value_total,value);
					break;
					
					case PPOperators.OVERRIDE:
						int_value_total = value;
					break;
					
					case PPOperators.SET:
						int_value_total = value;
					break;
					
					default:
						Error("PPEMatClassParameterInt | Invalid operator " + operator + " in mat/par: " + m_MaterialIndex + "/" + m_ParameterIndex);
					break;
				}
				
				if ( operator == PPOperators.OVERRIDE )
				{
					//DbgPrnt("m_LayerInfo | PPOperators.OVERRIDE at: " + i);
					override_active = true;
				}
			}
			
			m_CommandLayersArray.Clear();
			
			Param1<int>.Cast(p_total).param1 = int_value_total;
		}
		
		//DbgPrnt("PPEDebug | PPEMatClassParameterInt - UpdateParameterValues | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | parameter update end, removing from queue");
		m_Parent.ParamUpdateRemove(m_ParameterIndex);
		
		m_CurrentValues = p_total;
	}
	
	void PrepareLayerInfo(int layer, int value, int operator)
	{
		m_LayerInfo.Set(layer,{value,operator});
		AddPriorityInfo(layer);
	}
	
	//! No active requests for the mat. parameter value change, sets the value to DEFAULT
	override void SetParameterValueDefault(inout Param p_total)
	{
		p_total = new Param1<int>(PPETemplateDefInt.Cast(m_Defaults).param2);
		m_CurrentValues = p_total;
		//DbgPrnt("PPEDebug | PPEMatClassParameterInt - UpdateParameterValues | exit 3 - zero value");
	}
}
