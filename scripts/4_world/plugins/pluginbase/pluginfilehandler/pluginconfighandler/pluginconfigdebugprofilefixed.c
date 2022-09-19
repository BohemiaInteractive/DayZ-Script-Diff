class PluginConfigDebugProfileFixed extends PluginConfigDebugProfile
{
	private const string POSITIONS_LIST			= "console_positions";
	private const string POSITIONS_LIST_ENOCH	= "console_positions_enoch";
	private const string SUB_PARAM_POS_NAME		= "name";
	private const string SUB_PARAM_POS_VEC		= "pos";
	
	//========================================
	// ModuleLocalProfileUIFixed
	//========================================
	void ModuleLocalProfileUIFixed()
	{
		m_ReadOnly = true;
	}
	
	//========================================
	// GetFileName
	//========================================
	override string GetFileName()
	{
		return CFG_FILE_FIXED_PROFILE;
	}
	
	//========================================
	// GetAllPositionsNames
	//========================================
	void GetAllPositionsNames( out TStringArray arr )
	{
		string world = g_Game.GetWorldName();
		world.ToLower();
		if ( world == "enoch")
			GetSubParametersInStringArray( POSITIONS_LIST_ENOCH, SUB_PARAM_POS_NAME, arr );
		else
			GetSubParametersInStringArray( POSITIONS_LIST, SUB_PARAM_POS_NAME, arr );
	}
	
	//========================================
	// GetPositionByName
	//========================================
	vector GetPositionByName( string pos_name )
	{
		vector pos;
		string world = g_Game.GetWorldName();
		world.ToLower();
		
		array<ref CfgParam> params;
		if ( world == "enoch")
			params = GetArray( POSITIONS_LIST_ENOCH );
		else
			params = GetArray( POSITIONS_LIST );		
		
		for ( int i = 0; i < params.Count(); i++ )
		{
			CfgParamArray params_array = CfgParamArray.Cast( params.Get( i ) );
			array<ref CfgParam> elements = params_array.GetValues();
			
			CfgParamString param_string;
			bool element_ok = false;
			int j;
			
			for ( j = 0; j < elements.Count(); j++ )
			{
				param_string = CfgParamString.Cast( elements.Get( j ) );
				if ( param_string.GetName() == SUB_PARAM_POS_NAME && param_string.GetValue() == pos_name )
				{
					element_ok = true;
				}
			}
			
			if ( element_ok )
			{
				for ( j = 0; j < elements.Count(); j++ )
				{
					param_string = CfgParamString.Cast( elements.Get( j ) );
					if ( param_string.GetName() == SUB_PARAM_POS_VEC )
					{
						pos = param_string.GetValue().ToVector();
					}
				}
			}
		}
		
		return pos;
	}
}