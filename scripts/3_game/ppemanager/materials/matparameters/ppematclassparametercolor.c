
class ColorValuesData : Managed
{
	protected ref array<float> m_Values;
	protected int m_Operator;
	
	void ColorValuesData(ref array<float> values, int operator)
	{
		m_Values = new array<float>;
		m_Values.Copy(values);
		m_Operator = operator;
	}
	
	void SetValues(ref array<float> values)
	{
		m_Values.Copy(values);
	}
	
	array<float> GetValues()
	{
		return m_Values;
	}
	
	void SetOperator(int operator)
	{
		m_Operator = operator;
	}
	
	int GetOperator()
	{
		return m_Operator;
	}
}

class PPEMatClassParameterColor extends PPEMatClassParameterCommandData
{
	const int VALUE_RED = 0;
	const int VALUE_GREEN = 1;
	const int VALUE_BLUE = 2;
	const int VALUE_ALPHA = 3;
	const int VALUES_COUNT = 4; //rgba
	
	protected PPETemplateDefColor m_Color;
	protected ref map<int,ref ColorValuesData> m_LayerInfo; //<priority,<<values>,operator>>
	
	float m_ColorDefaults[4];
	
	void PPEMatClassParameterColor(int mat_idx, int parameter_idx, PPEClassBase parent)
	{
		m_LayerInfo = new map<int,ref ColorValuesData>;
	}
	
	override void InitDefaults()
	{
		m_Color = PPETemplateDefColor.Cast(m_Defaults);
		m_ColorDefaults = {m_Color.param2,m_Color.param3,m_Color.param4,m_Color.param5};
	}
	
	override void InitCuttent()
	{
		m_CurrentValues = new Param4<float,float,float,float>(m_ColorDefaults[0],m_ColorDefaults[1],m_ColorDefaults[2],m_ColorDefaults[3]);
	}
	
	override int GetParameterVarType()
	{
		return PPEConstants.VAR_TYPE_COLOR;
	}
	
	override void Update(float timeslice, out Param p_total, out bool setting_defaults, int order)
	{
		super.Update(timeslice,p_total,setting_defaults,order);
		 
		int active_request_count = 0;
		
		PPERequestParamDataColor req_data;
		
		bool setting_value_zero = false;
		
		array<float> color_values_total = {0.0,0.0,0.0,0.0};
		array<float> color_values_temp = {0.0,0.0,0.0,0.0};
		bool color_total_override_set = false;
		
		// param creation
		if (p_total == null)
		{
			p_total = new Param4<float,float,float,float>(0.0,0.0,0.0,0.0);
		}
		
		if (m_RequestMap.Count() > 0)
		{
			m_LayerInfo.Clear();
		}
		else
		{
			//DbgPrnt("m_RequestMap.Count() is zero! Using default values. | mat/par: " + m_MaterialIndex + "/" + m_ParameterIndex);
			SetParameterValueDefault(p_total); //TODO - redundant?
			m_Parent.ParamUpdateRemove(m_ParameterIndex);
			return;
		}
		
		//get target values, evaluate active and priority conditions, and insert into arrays to later process
		for ( int i = 0; i < m_RequestMap.Count(); i++ )
		{
			req_data = PPERequestParamDataColor.Cast(m_RequestMap.GetElement(i));
			
			if (req_data == null)
			{
				Error("PPEMatClassParameterColor | req_data not found! | " + this + " | mat/par: " + m_MaterialIndex + "/" + m_ParameterIndex);
				continue;
			}
			
			setting_value_zero = req_data.IsSettingDefaultValues();
			
			//skips "zero value" defaults
			if (setting_value_zero && !req_data.GetUpdatingDataValues() && !req_data.IsDataActive()) 
			{
				//DbgPrnt("Is Default, not updating | idx: " + i + " | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX());
				continue;
			}
			
			active_request_count++; //something is active, though not necessarily setting new values
			
			//evaluation
			//--------------------------------
			req_data.m_ColorLast = req_data.m_ColorCurrent;
			
			if (!req_data.GetUpdatingDataValues() && req_data.IsDataActive()) //set to exact value, not updating anymore
			{
				color_values_temp.Copy(req_data.m_ColorCurrent); //already converted from before?
				PrepareLayerInfo(req_data.GetPriorityLayer(),{color_values_temp[0],color_values_temp[1],color_values_temp[2],color_values_temp[3]},req_data.GetInteractionMask());
				//DbgPrnt("PPEDebug | PPEMatClassParameterColor - UpdateParameterValues | !req_data.m_UpdatingData | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | not updating, addaing current values into mix: " + req_data.m_ColorCurrent[0] + "," + req_data.m_ColorCurrent[1] + "," + req_data.m_ColorCurrent[2] + "," + req_data.m_ColorCurrent[3]);
				continue;
			}
			
			if (setting_value_zero)
			{
				req_data.m_ColorTarget = {0.0,0.0,0.0,0.0}; //color_defaults;
				//DbgPrnt("PPEDebug | PPEMatClassParameterColor - UpdateParameterValues | !data.m_DefaultTargetSet | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | req_data.m_FloatTarget: " + req_data.m_FloatTarget);
			}
			
			//DbgPrnt("PPEDebug | PPEMatClassParameterColor - UpdateParameterValues | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX());
			req_data.SetUpdatingDataValues(false);
			if (setting_value_zero)
			{
				req_data.SetDataActive(false);
				//RemovePriorityInfo(req_data.GetPriorityLayer()); //redundant?
			}
			else
			{
				color_values_temp.Copy(req_data.m_ColorTarget);
				PrepareLayerInfo(req_data.GetPriorityLayer(),{color_values_temp[0],color_values_temp[1],color_values_temp[2],color_values_temp[3]},req_data.GetInteractionMask());
			}
			
			req_data.m_ColorCurrent.Copy(color_values_temp);
			//DbgPrnt("PPEDebug | PPEMatClassParameterColor - UpdateParameterValues | current: " + req_data.m_ColorCurrent);
		}
		
		//---------------------------
		//MASK handling and calculation
		array<float> values;
		int operator = -1;
		bool override_active = false;
		
		color_values_total = {m_ColorDefaults[0],m_ColorDefaults[1],m_ColorDefaults[2],m_ColorDefaults[3]};
		
		for ( i = 0; i < m_LayerInfo.Count(); i++ )
		{
			if ( override_active )
				break;
			
			values = m_LayerInfo.Get(m_CommandLayersArray.Get(i)).GetValues();
			operator = m_LayerInfo.Get(m_CommandLayersArray.Get(i)).GetOperator();
			
			switch (operator)
			{
				case PPOperators.LOWEST:
					color_values_total[VALUE_RED] = Math.Min(color_values_total[VALUE_RED], values[VALUE_RED]);
					color_values_total[VALUE_GREEN] = Math.Min(color_values_total[VALUE_GREEN], values[VALUE_GREEN]);
					color_values_total[VALUE_BLUE] = Math.Min(color_values_total[VALUE_BLUE], values[VALUE_BLUE]);
					color_values_total[VALUE_ALPHA] = Math.Min(color_values_total[VALUE_ALPHA], values[VALUE_ALPHA]);
				break;
				
				case PPOperators.HIGHEST:
					color_values_total[VALUE_RED] = Math.Max(color_values_total[VALUE_RED], values[VALUE_RED]);
					color_values_total[VALUE_GREEN] = Math.Max(color_values_total[VALUE_GREEN], values[VALUE_GREEN]);
					color_values_total[VALUE_BLUE] = Math.Max(color_values_total[VALUE_BLUE], values[VALUE_BLUE]);
					color_values_total[VALUE_ALPHA] = Math.Max(color_values_total[VALUE_ALPHA], values[VALUE_ALPHA]);
				break;
				
				case PPOperators.ADD_RELATIVE:
				case PPOperators.ADD:
					color_values_total[VALUE_RED] = color_values_total[VALUE_RED] + values[VALUE_RED];
					color_values_total[VALUE_GREEN] = color_values_total[VALUE_GREEN] + values[VALUE_GREEN];
					color_values_total[VALUE_BLUE] = color_values_total[VALUE_BLUE] + values[VALUE_BLUE];
					color_values_total[VALUE_ALPHA] = color_values_total[VALUE_ALPHA] + values[VALUE_ALPHA];
				break;
				
				/*case PPOperators.ADD_RELATIVE: //no known max values, unusable!
					float_value_total = (1 - float_value_total) * value + float_value_total;
				break;*/
				
				case PPOperators.SUBSTRACT_RELATIVE:
				case PPOperators.SUBSTRACT:
					color_values_total[VALUE_RED] = color_values_total[VALUE_RED] - values[VALUE_RED];
					color_values_total[VALUE_GREEN] = color_values_total[VALUE_GREEN] - values[VALUE_GREEN];
					color_values_total[VALUE_BLUE] = color_values_total[VALUE_BLUE] - values[VALUE_BLUE];
					color_values_total[VALUE_ALPHA] = color_values_total[VALUE_ALPHA] - values[VALUE_ALPHA];
				break;
				
				/*case PPOperators.SUBSTRACT_RELATIVE: //no known max values, unusable!
					float_value_total = float_value_total - value * float_value_total;
				break;*/
				
				case PPOperators.SUBSTRACT_REVERSE_RELATIVE:
				case PPOperators.SUBSTRACT_REVERSE:
					color_values_total[VALUE_RED] = values[VALUE_RED] - color_values_total[VALUE_RED];
					color_values_total[VALUE_GREEN] = values[VALUE_GREEN] - color_values_total[VALUE_GREEN];
					color_values_total[VALUE_BLUE] = values[VALUE_BLUE] - color_values_total[VALUE_BLUE];
					color_values_total[VALUE_ALPHA] = values[VALUE_ALPHA] - color_values_total[VALUE_ALPHA];
				break;
				
				/*case PPOperators.SUBSTRACT_REVERSE_RELATIVE: //no known max values, unusable!
					float_value_total = value * float_value_total - float_value_total;
				break;*/
				
				case PPOperators.MULTIPLICATIVE:
					color_values_total[VALUE_RED] = color_values_total[VALUE_RED] * values[VALUE_RED];
					color_values_total[VALUE_GREEN] = color_values_total[VALUE_GREEN] * values[VALUE_GREEN];
					color_values_total[VALUE_BLUE] = color_values_total[VALUE_BLUE] * values[VALUE_BLUE];
					color_values_total[VALUE_ALPHA] = color_values_total[VALUE_ALPHA] * values[VALUE_ALPHA];
				break;
				
				case PPOperators.OVERRIDE:
					color_values_total.Copy(values);
				break;
				
				case PPOperators.SET:
					color_values_total.Copy(values);
				break;
			}
			
			//DbgPrnt("m_LayerInfo | color_values_total pre-clamp: " + color_values_total[VALUE_RED] + "/" + color_values_total[VALUE_GREEN] + "/" + color_values_total[VALUE_BLUE] + "/" + color_values_total[VALUE_ALPHA] + " | i: " + i);
			if ( operator == PPOperators.OVERRIDE )
			{
				//DbgPrnt("m_LayerInfo | PPOperators.OVERRIDE at: " + i);
				override_active = true;
			}
			
			//RemovePriorityInfo(m_CommandLayersArray.Get(i));
		}
		
		m_CommandLayersArray.Clear();
		
		if ( m_MaterialIndex == 15 && m_ParameterIndex == 21 )
		{
			//Print("m_MaterialIndex: " + m_MaterialIndex + "|m_ParameterIndex: " + m_ParameterIndex);
			//Print("PPEMatClassParameterColor values: " + color_values_total[0] + " " + color_values_total[1] + " " + color_values_total[2] + " " + color_values_total[3]);
		}
		
		//TODO - consider moving this up, if possible
		if ( active_request_count <= 0 )
		{
			SetParameterValueDefault(p_total);
		}
		else
		{
			Param4<float,float,float,float>.Cast(p_total).param1 = color_values_total[0];
			Param4<float,float,float,float>.Cast(p_total).param2 = color_values_total[1];
			Param4<float,float,float,float>.Cast(p_total).param3 = color_values_total[2];
			Param4<float,float,float,float>.Cast(p_total).param4 = color_values_total[3];
		}
		
		//DbgPrnt("PPEDebug | PPEMatClassParameterColor - UpdateParameterValues | mat/par/req: " + m_MaterialIndex + "/" + m_ParameterIndex + "/" + req_data.GetRequesterIDX() + " | parameter update end, removing from queue");
		m_Parent.ParamUpdateRemove(m_ParameterIndex);
		
		ModifyResultValues(p_total);
		
		m_CurrentValues = p_total;
	}
	
	//! Fills relative values into arrays/member variables
	void PrepareLayerInfo(int layer, ref array<float> values, int operator)
	{
		ColorValuesData tmp;
		if (m_LayerInfo.Contains(layer))
		{
			tmp = m_LayerInfo.Get(layer);
			tmp.SetValues(values);
			tmp.SetOperator(operator);
		}
		else
		{
			tmp = new ColorValuesData(values,operator);
		}
		
		m_LayerInfo.Set(layer,tmp);
		AddPriorityInfo(layer);
	}
	
	override void SetParameterValueDefault(inout Param p_total)
	{
		if (!p_total)
		{
			p_total = new Param4<float,float,float,float>(m_ColorDefaults[0],m_ColorDefaults[1],m_ColorDefaults[2],m_ColorDefaults[3]);
		}
		else
		{
			Param4<float,float,float,float>.Cast(p_total).param1 = m_ColorDefaults[0];
			Param4<float,float,float,float>.Cast(p_total).param2 = m_ColorDefaults[1];
			Param4<float,float,float,float>.Cast(p_total).param3 = m_ColorDefaults[2];
			Param4<float,float,float,float>.Cast(p_total).param4 = m_ColorDefaults[3];
		}
		m_CurrentValues = p_total;
	}
}

class PPEMatClassParameterColorColorization extends PPEMatClassParameterColor
{
	override void ModifyResultValues(inout Param result_values)
	{
		super.ModifyResultValues(result_values);
		
		//saturation-dependency handling
		Param par = PPEManagerStatic.GetPPEManager().GetPostProcessCurrentValues(PostProcessEffectType.Glow,PPEGlow.PARAM_SATURATION);
		float saturation_coef = Param1<float>.Cast(par).param1;
		
		float r = Param4<float,float,float,float>.Cast(result_values).param1;
		float g = Param4<float,float,float,float>.Cast(result_values).param2;
		float b = Param4<float,float,float,float>.Cast(result_values).param3;
		float a = Param4<float,float,float,float>.Cast(result_values).param4;
		
		float highest_channel = Math.Max(Math.Max(r,g),b);
		saturation_coef = Easing.EaseOutSine(saturation_coef);
		
		r = Math.Lerp(highest_channel,r,saturation_coef); 
		g = Math.Lerp(highest_channel,g,saturation_coef);
		b = Math.Lerp(highest_channel,b,saturation_coef);
		a = Math.Lerp(m_ColorDefaults[3],a,saturation_coef);
		//
		
		Param4<float,float,float,float>.Cast(result_values).param1 = r;
		Param4<float,float,float,float>.Cast(result_values).param2 = g;
		Param4<float,float,float,float>.Cast(result_values).param3 = b;
		Param4<float,float,float,float>.Cast(result_values).param4 = a;
	}
}
