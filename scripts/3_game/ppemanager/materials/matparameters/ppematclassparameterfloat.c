class PPEMatClassParameterFloat extends PPEMatClassParameterCommandData
{
	protected ref map<int,ref array<float,int>> m_LayerInfo; //<priority,<value,operator>>
	
	protected PPETemplateDefFloat m_Float;
	protected float m_ValueDefault;
	protected float m_ValueMin;
	protected float m_ValueMax;
	
	void PPEMatClassParameterFloat(int mat_idx, int parameter_idx, PPEClassBase parent)
	{
		m_LayerInfo = new map<int,ref array<float,int>>;
	}
	
	override void InitDefaults()
	{
		Class.CastTo(m_Float,m_Defaults);
		//m_Float = PPETemplateDefFloat.Cast(m_Defaults);
		m_ValueDefault = m_Float.param2;
		m_ValueMin = m_Float.param3;
		m_ValueMax = m_Float.param4;
	}
	
	override void InitCuttent()
	{
		m_CurrentValues = new Param1<float>(m_ValueDefault);
	}
	
	override int GetParameterVarType()
	{
		return PPEConstants.VAR_TYPE_FLOAT;
	}
	
	override void Update(float timeslice, out Param p_total, out bool setting_defaults, int order)
	{
		super.Update(timeslice,p_total,setting_defaults,order);
		
		protected int active_request_count = 0;
		
		protected PPERequestParamDataFloat req_data;
		
		protected bool setting_value_zero = false;
		
		protected float float_value_temp = 0.0;
		protected float float_value_total = 0.0;
		
		if (p_total == null)
		{
			p_total = new Param1<float>(0.0);
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
			req_data = PPERequestParamDataFloat.Cast(m_RequestMap.GetElement(i));
			
			if (req_data == null)
			{
				Error("Debug | PPEMatClassParameterFloat | req_data not found! | " + this + " | mat/par: " + m_MaterialIndex + "/" + m_ParameterIndex);
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
				req_data.m_FloatTarget = 0;//min;
				//DbgPrnt("PPEDebug | PPEMatClassParameterFloat - UpdateParameterValues | !data.m_DefaultTargetSet | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | req_data.m_FloatTarget: " + req_data.m_FloatTarget);
			}
			else 
			{
				active_request_count++;
			}
			
			//evaluation
			//--------------------------------
			req_data.m_FloatLast = req_data.m_FloatCurrent;
			
			if (!req_data.GetUpdatingDataValues() && req_data.IsDataActive()) //set to exact value, not updating anymore
			{
				float_value_temp = req_data.m_FloatCurrent;
				PrepareLayerInfo(req_data.GetPriorityLayer(),float_value_temp,req_data.GetInteractionMask());
				//DbgPrnt("PPEDebug | PPEMatClassParameterFloat - UpdateParameterValues | !req_data.m_UpdatingData | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | not updating, addaing current value into mix: " + float_value_temp);
				continue;
			}
			
			float_value_temp = req_data.m_FloatTarget;
			
			//DbgPrnt("PPEDebug | PPEMatClassParameterFloat - UpdateParameterValues | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX());
			req_data.SetUpdatingDataValues(false);
			if (setting_value_zero)
			{
				req_data.SetDataActive(false);
				//RemovePriorityInfo(req_data.GetPriorityLayer()); //redundant?
			}
			else
			{
				float_value_temp = req_data.m_FloatTarget;
				PrepareLayerInfo(req_data.GetPriorityLayer(),float_value_temp,req_data.GetInteractionMask());
			}
			
			req_data.m_FloatCurrent = float_value_temp;
		}
		
		//---------------------------
		//MASK handling and calculation
		float value;
		int operator;
		bool override_active = false;
		
		if ( m_ValueMax == 0.0 )
			float_value_total = m_ValueDefault; //?
		else
			float_value_total = m_ValueDefault / m_ValueMax;
		
		for ( i = 0; i < m_LayerInfo.Count(); i++ )
		{
			if ( override_active )
				break;
			
			value = m_LayerInfo.Get(m_CommandLayersArray.Get(i)).Get(LAYER_INFO_VALUE);
			operator = m_LayerInfo.Get(m_CommandLayersArray.Get(i)).Get(LAYER_INFO_OPERATOR);
			
			switch (operator)
			{
				case PPOperators.LOWEST:
					float_value_total = Math.Min(float_value_total,value);
				break;
				
				case PPOperators.HIGHEST:
					float_value_total = Math.Max(float_value_total,value);
				break;
				
				case PPOperators.ADD:
					float_value_total = float_value_total + value;
				break;
				
				case PPOperators.ADD_RELATIVE:
					float_value_total = (1 - float_value_total) * value + float_value_total;
				break;
				
				case PPOperators.SUBSTRACT:
					float_value_total = float_value_total - value;
				break;
				
				case PPOperators.SUBSTRACT_RELATIVE:
					float_value_total = float_value_total - value * float_value_total;
				break;
				
				case PPOperators.SUBSTRACT_REVERSE:
					float_value_total = value - float_value_total;
				break;
				
				case PPOperators.SUBSTRACT_REVERSE_RELATIVE:
					float_value_total = value * float_value_total - float_value_total;
				break;
				
				case PPOperators.MULTIPLICATIVE:
					float_value_total = float_value_total * value;
				break;
				
				case PPOperators.OVERRIDE:
					float_value_total = value;
				break;
				
				case PPOperators.SET:
					float_value_total = value;
				break;
			}
			
			//DbgPrnt("m_LayerInfo | float_value_total pre-clamp: " + float_value_total + " | i: " + i);
			float_value_total = Math.Clamp(float_value_total,0,1);
			
			if ( operator == PPOperators.OVERRIDE )
			{
				//DbgPrnt("m_LayerInfo | PPOperators.OVERRIDE at: " + i);
				override_active = true;
			}
			
			//RemovePriorityInfo(m_CommandLayersArray.Get(i));
		}
		
		m_CommandLayersArray.Clear();
		
		//TODO - consider moving this up, if possible
		if (active_request_count == 0)
		{
			SetParameterValueDefault(p_total);
		}
		else
		{
			float res = Math.Lerp(m_ValueMin,m_ValueMax,float_value_total);
			Param1<float>.Cast(p_total).param1 = res; //converts back to absolute values
		}
		
		//DbgPrnt("PPEDebug | PPEMatClassParameterFloat - UpdateParameterValues | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | parameter update end, removing from queue");
		m_Parent.ParamUpdateRemove(m_ParameterIndex);
		
		m_CurrentValues = p_total;
	}
	
	void PrepareLayerInfo(int layer, float value, int operator)
	{
		m_LayerInfo.Set(layer,{value,operator});
		AddPriorityInfo(layer);
	}
	
	//! No active requests for the mat. parameter value change, sets the value to DEFAULT (zero?)
	override void SetParameterValueDefault(inout Param p_total)
	{
		p_total = new Param1<float>(PPETemplateDefFloat.Cast(m_Defaults).param2);
		m_CurrentValues = p_total;
		//DbgPrnt("PPEDebug | PPEMatClassParameterFloat - UpdateParameterValues | exit 3 - zero value");
	}
}

class PPEMatClassParameterFloatSaturation extends PPEMatClassParameterFloat
{
	void PPEMatClassParameterFloatSaturation(int mat_idx, int parameter_idx, PPEClassBase parent)
	{
		m_Dependencies = new map<int,ref array<int>>;
		m_Dependencies.Set(PostProcessEffectType.Glow,{PPEGlow.PARAM_COLORIZATIONCOLOR, PPEGlow.PARAM_OVERLAYCOLOR});
	}
}
