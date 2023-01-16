class PluginConfigDebugProfileFixed extends PluginConfigDebugProfile
{
	const string POSITIONS_LIST			= "console_positions";
	const string POSITIONS_LIST_ENOCH	= "console_positions_enoch";
	const string SUB_PARAM_POS_NAME		= "name";
	const string SUB_PARAM_POS_VEC		= "pos";
	
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
}