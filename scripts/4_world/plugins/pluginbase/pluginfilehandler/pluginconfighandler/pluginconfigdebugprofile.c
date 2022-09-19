class PluginConfigDebugProfile extends PluginConfigHandler
{	
	protected const string SCENE_DRAW_SELECTION				= "scene_editor_draw_selection";
	protected const string SCENE_LOAD_PLAYER_POS			= "scene_editor_load_player_pos";
	protected const string SCENE_ROTATION_ANGLE				= "scene_editor_rotation_angle";
	protected const string ITEM_TAB_SELECTED				= "console_TabSelected";
	protected const string PRESET_LIST						= "console_presets";
	protected const string PRESET_DEFAULT					= "console_preset_default";
	protected const string ITEM_SEARCH						= "console_ItemSearch";
	protected const string SPAWN_DISTANCE					= "console_SpawnDistance";
	protected const string CHAR_STATS_VIS					= "console_character_stats_visible";
	protected const string CHAR_LEVELS_VIS					= "console_character_levels_visible";
	protected const string CHAR_MODIFIERS_VIS				= "console_character_modifiers_visible";
	protected const string CHAR_AGENTS_VIS					= "console_character_agents_visible";
	protected const string CHAR_DEBUG_VIS					= "console_character_debug_visible";
	protected const string CHAR_STOMACH_VIS					= "console_character_stomach_visible";
	protected const string FREE_CAMERA_CROSSHAIR			= "console_free_camera_crosshair_visible";
	protected const string VERSION_VIS						= "console_version_visible";
	protected const string MERGE_TYPE						= "category_merge_type";
	protected const string TEMP_VIS							= "console_temperature_visible";
	protected const string SUB_PARAM_ITEM					= "item";
	protected const string SUB_PARAM_ITEM_NAME				= "name";
	protected const string SUB_PARAM_ITEM_HEALTH			= "health";
	protected const string SUB_PARAM_ITEM_QUANTITY			= "quantity";
	protected const string LOGS_ENABLED						= "logs_enabled";
	protected const string CONFIG_CLASSES_FLAG				= "toggled_config_classes_flag";
	protected const string ITEM_CATEGORY_FLAG				= "toggled_item_categories_flag";
	protected const string ITEM_PREVIEW						= "show_item_preview";
	protected const string BATCH_RECT						= "batch_spawn_rect";
	protected const string BATCH_QUANT						= "batch_spawn_quantity";
	protected const string SOUNDFILTER						= "soundset_editbox";
	
	protected ref map<string, ref CfgParam>				m_DefaultValues;
	protected ref TStringArray 							m_PresetList;	
	
	//========================================
	// GetInstance
	//========================================
	static PluginConfigDebugProfile GetInstance()
	{
		return PluginConfigDebugProfile.Cast( GetPlugin(PluginConfigDebugProfile) );
	}	
	
	//========================================
	// GetCfgParamArray
	//========================================
	protected CfgParamArray GetNewCfgParamArray( array<ref CfgParam> params )
	{
		CfgParamArray param = new CfgParamArray( "" );
		param.SetParams( params );
		return param;
	}
	
	//========================================
	// GetCfgParamBool
	//========================================
	protected CfgParamBool GetNewCfgParamBool( bool value )
	{
		CfgParamBool param = new CfgParamBool( "" );
		param.SetValue( value );
		return param;
	}
	
	//========================================
	// GetCfgParamFloat
	//========================================
	protected CfgParamFloat GetNewCfgParamFloat( float value )
	{
		CfgParamFloat param = new CfgParamFloat( "" );
		param.SetValue( value );
		return param;
	}
	
	//========================================
	// GetCfgParamInt
	//========================================
	protected CfgParamInt GetNewCfgParamInt( int value )
	{
		CfgParamInt param = new CfgParamInt( "" );
		param.SetValue( value );
		return param;
	}
	
	//========================================
	// GetCfgParamString
	//========================================
	protected CfgParamString GetNewCfgParamString( string value )
	{
		CfgParamString param = new CfgParamString( "" );
		param.SetValue( value );
		return param;
	}
	
	//========================================
	// Array
	//========================================
	protected CfgParamArray SetArray( string key, array<ref CfgParam> params )
	{
		CfgParamArray param = CfgParamArray.Cast( GetParamByName( key, CFG_TYPE_ARRAY ) );
		param.SetParams( params );
		SaveConfigToFile();
		return param;
	}

	protected array<ref CfgParam> GetArray( string key )
	{
		if ( ParamExist(key) )
		{
			CfgParamArray param = CfgParamArray.Cast( GetParamByName( key, CFG_TYPE_ARRAY ) );
			return param.GetValues();
		}
		else
		{
			if ( m_DefaultValues.Contains(key) )
			{
				CfgParamArray default_param = CfgParamArray.Cast( m_DefaultValues.Get( key ) );
				return SetArray( key, default_param.GetValues() ).GetValues();
			}
			else
			{
				return SetArray( key, new array<ref CfgParam> ).GetValues();
			}
		}
	}
	
	//========================================
	// Bool
	//========================================
	protected CfgParamBool SetBool( string key, bool value )
	{
		CfgParamBool param = CfgParamBool.Cast( GetParamByName( key, CFG_TYPE_BOOL ) );
		param.SetValue( value );
		SaveConfigToFile();
		return param;
	}

	protected bool GetBool( string key )
	{
		if ( ParamExist(key) )
		{
			CfgParamBool param = CfgParamBool.Cast( GetParamByName( key, CFG_TYPE_BOOL ) );
			return param.GetValue();
		}
		else
		{
			if ( m_DefaultValues.Contains(key) )
			{
				CfgParamBool default_param = CfgParamBool.Cast( m_DefaultValues.Get( key ) );
				return SetBool( key, default_param.GetValue() ).GetValue();
			}
			else
			{
				return SetBool( key, false ).GetValue();
			}
		}
	}
	
	//========================================
	// Float
	//========================================
	protected CfgParamFloat SetFloat( string key, float value )
	{
		CfgParamFloat param = CfgParamFloat.Cast( GetParamByName( key, CFG_TYPE_FLOAT ) );
		param.SetValue( value );
		SaveConfigToFile();
		return param;
	}

	protected float GetFloat( string key )
	{
		if ( ParamExist(key) )
		{
			CfgParamFloat param = CfgParamFloat.Cast( GetParamByName( key, CFG_TYPE_FLOAT ) );
			return param.GetValue();
		}
		else
		{
			if ( m_DefaultValues.Contains(key) )
			{
				CfgParamFloat default_param = CfgParamFloat.Cast( m_DefaultValues.Get( key ) );
				return SetFloat( key, default_param.GetValue() ).GetValue();
			}
			else
			{
				return SetFloat( key, 0.0 ).GetValue();
			}
		}
	}
	
	//========================================
	// Int
	//========================================
	protected CfgParamInt SetInt( string key, int value )
	{
		CfgParamInt param = CfgParamInt.Cast( GetParamByName( key, CFG_TYPE_INT ) );
		param.SetValue( value );
		SaveConfigToFile();
		return param;
	}

	protected int GetInt( string key )
	{
		if ( ParamExist(key) )
		{
			CfgParamInt param = CfgParamInt.Cast( GetParamByName( key, CFG_TYPE_INT ) );
			return param.GetValue();
		}
		else
		{
			if ( m_DefaultValues.Contains(key) )
			{
				CfgParamInt default_param = CfgParamInt.Cast( m_DefaultValues.Get( key ) );
				return SetInt( key, default_param.GetValue() ).GetValue();
			}
			else
			{
				return SetInt( key, 0 ).GetValue();
			}
		}
	}
	
	//========================================
	// String
	//========================================
	protected CfgParamString SetString( string key, string value )
	{
		CfgParamString param = CfgParamString.Cast( GetParamByName( key, CFG_TYPE_STRING ) );
		if(param)
		{
			param.SetValue( value );
			SaveConfigToFile();
		}
		return param;
	}

	protected string GetString( string key )
	{
		CfgParamString param = CfgParamString.Cast( GetParamByName( key, CFG_TYPE_STRING ) );
		
		if ( param && param.GetValue() != STRING_EMPTY )
		{
			return param.GetValue();
		}
		else
		{
			if ( m_DefaultValues.Contains(key) )
			{
				CfgParamString default_param = CfgParamString.Cast( m_DefaultValues.Get( key ) );
				CfgParamString param_string = SetString( key, default_param.GetValue() );
				if(param_string)
					return param_string.GetValue();
			}
			else
			{
				return SetString( key, "" ).GetValue();
			}
		}
		return "";
	}
	
	protected void GetSubParametersInStringArray( string setting_name, string sub_param_name, out TStringArray arr )
	{
		array<ref CfgParam> items = GetArray( setting_name );
		for ( int i = 0; i < items.Count(); i++ )
		{
			CfgParamArray items_array = CfgParamArray.Cast( items.Get(i) );
			array<ref CfgParam> item_params = items_array.GetValues();
			 
			for ( int j = 0; j < item_params.Count(); j++ )
			{
				if ( item_params.Get(j).GetName() == sub_param_name )
				{
					CfgParamString param_string = CfgParamString.Cast( item_params.Get( j ) );
					arr.Insert( param_string.GetValue() );
				}
			}
		}
	}
	
	//========================================
	// PluginConfigDebugProfile
	//========================================
	void PluginConfigDebugProfile()
	{
		m_DefaultValues = new map<string, ref CfgParam>;
		m_DefaultValues.Insert(SCENE_DRAW_SELECTION, 	GetNewCfgParamBool(true) );
		m_DefaultValues.Insert(SCENE_LOAD_PLAYER_POS, 	GetNewCfgParamBool(true) );
		m_DefaultValues.Insert(SCENE_ROTATION_ANGLE, 	GetNewCfgParamInt(15) );
		m_DefaultValues.Insert(ITEM_TAB_SELECTED, 		GetNewCfgParamInt(0) );
		m_DefaultValues.Insert(PRESET_DEFAULT, 			GetNewCfgParamString("") );
		m_DefaultValues.Insert(ITEM_SEARCH, 			GetNewCfgParamString("") );
		m_DefaultValues.Insert(SPAWN_DISTANCE, 			GetNewCfgParamFloat(0.0) );
		m_DefaultValues.Insert(LOGS_ENABLED, 			GetNewCfgParamBool(true) );
		m_DefaultValues.Insert(CHAR_STATS_VIS, 			GetNewCfgParamBool(false) );
		m_DefaultValues.Insert(CHAR_LEVELS_VIS, 		GetNewCfgParamBool(false) );
		m_DefaultValues.Insert(CHAR_MODIFIERS_VIS, 		GetNewCfgParamBool(false) );
		m_DefaultValues.Insert(CHAR_AGENTS_VIS, 		GetNewCfgParamBool(false) );
		m_DefaultValues.Insert(CHAR_DEBUG_VIS, 			GetNewCfgParamBool(false) );
		m_DefaultValues.Insert(FREE_CAMERA_CROSSHAIR, 	GetNewCfgParamBool(true) );
		m_DefaultValues.Insert(VERSION_VIS,				GetNewCfgParamBool(true) );
		m_DefaultValues.Insert(CONFIG_CLASSES_FLAG,		GetNewCfgParamInt(15) );
		m_DefaultValues.Insert(TEMP_VIS,				GetNewCfgParamBool(false) );
		m_DefaultValues.Insert(MERGE_TYPE,				GetNewCfgParamBool(false) );
		m_DefaultValues.Insert(ITEM_PREVIEW,			GetNewCfgParamBool(true) );
		m_DefaultValues.Insert(BATCH_RECT,				GetNewCfgParamArray(GetDefaultBatchRectParams()) );
		m_DefaultValues.Insert(BATCH_QUANT,				GetNewCfgParamInt(10) );
		m_DefaultValues.Insert(SOUNDFILTER,				GetNewCfgParamString("") );
	}
	
	array<ref CfgParam> GetDefaultBatchRectParams()
	{
		array<ref CfgParam> params = new array<ref CfgParam>;
		CfgParamInt param1 = new CfgParamInt("");
		param1.SetValue(5);
		CfgParamInt param2 = new CfgParamInt("");
		param2.SetValue(5);
		CfgParamFloat param3 = new CfgParamFloat("");
		param3.SetValue(1.0);
		CfgParamFloat param4 = new CfgParamFloat("");
		param4.SetValue(1.0);
		
		params.Insert(param1);
		params.Insert(param2);
		params.Insert(param3);
		params.Insert(param4);
		
		return params;
	}
	
	//========================================
	// OnInit
	//========================================
	override void OnInit()
	{
		super.OnInit();
		
		array<ref CfgParam> params = GetArray( PRESET_LIST );
		
		m_PresetList = new TStringArray;
		for ( int i = 0; i < params.Count(); i++ )
		{
			CfgParamString param = CfgParamString.Cast( params.Get( i ) );
			m_PresetList.Insert( param.GetValue() );
		}
	}
	
	//========================================
	// GetFileName
	//========================================
	override string GetFileName()
	{
		return CFG_FILE_DEBUG_PROFILE;
	}

	//========================================
	// SceneEditorDrawSelection
	//========================================
	bool GetSceneEditorDrawSelection()
	{
		return GetBool( SCENE_DRAW_SELECTION );
	}

	void SetSceneEditorDrawSelection( bool value )
	{
		SetBool( SCENE_DRAW_SELECTION, value );
	}
	
	//========================================
	// SceneEditorSavePlayerPos
	//========================================
	bool GetSceneEditorLoadPlayerPos()
	{
		return GetBool( SCENE_LOAD_PLAYER_POS );
	}

	void SetSceneEditorLoadPlayerPos( bool value )
	{
		SetBool( SCENE_LOAD_PLAYER_POS, value );
	}
	
	//========================================
	// SceneEditorRotationAngle
	//========================================
	int GetSceneEditorRotationAngle()
	{
		return GetInt( SCENE_ROTATION_ANGLE );
	}

	void SetSceneEditorRotationAngle( int value )
	{
		SetInt( SCENE_ROTATION_ANGLE, value );
	}
	
	//========================================
	// TabSelected
	//========================================
	int GetTabSelected()
	{
		return GetInt( ITEM_TAB_SELECTED );
	}
	
	void SetTabSelected( int value )
	{
		SetInt( ITEM_TAB_SELECTED, value );
	}
	
	//========================================
	// DefaultPreset
	//========================================
	string GetDefaultPreset()
	{
		return GetString( PRESET_DEFAULT );
	}
	
	void SetDefaultPreset( string value )
	{
		SetString( PRESET_DEFAULT, value );
	}
	
	//========================================
	// Batch Spawn Params
	//========================================
	array<ref CfgParam> GetBatchSpawnRectangle()
	{
		return GetArray( BATCH_RECT );
	}
	
	void SetBatchSpawnRectangle( int row, int column, float rowStep, float columnStep)
	{
		array<ref CfgParam> params = GetArray( BATCH_RECT );
		params.Clear();
		CfgParamString param;
		
		CfgParamInt param1 = new CfgParamInt("");
		param1.SetValue(row);
		CfgParamInt param2 = new CfgParamInt("");
		param2.SetValue(column);
		CfgParamFloat param3 = new CfgParamFloat("");
		param3.SetValue(rowStep);
		CfgParamFloat param4 = new CfgParamFloat("");
		param4.SetValue(columnStep);
		
		params.Insert(param1);
		params.Insert(param2);
		params.Insert(param3);
		params.Insert(param4);
		SaveConfigToFile();
	}
	
	//========================================
	// Batch Spawn Quantity
	//========================================
	int GetBatchSpawnQuantity()
	{
		return GetInt( BATCH_QUANT );
	}
	
	void SetBatchSpawnQuantity(int value)
	{
		SetInt( BATCH_QUANT, value );
	}
	//========================================
	// ItemSearch
	//========================================
	string GetItemSearch()
	{
		return GetString( ITEM_SEARCH );
	}
	
	void SetItemSearch( string value )
	{
		SetString( ITEM_SEARCH, value );
	}

	//========================================
	// SpawnDistance
	//========================================
	float GetSpawnDistance()
	{
		return GetFloat( SPAWN_DISTANCE );
	}

	void SetSpawnDistance( float value )
	{
		SetFloat( SPAWN_DISTANCE, value );
	}

	//========================================
	// LogsEnabled
	//========================================
	bool GetLogsEnabled()
	{
		return GetBool( LOGS_ENABLED );
	}

	void SetLogsEnabled( bool value )
	{
		SetBool( LOGS_ENABLED, value );
	}
	
	//========================================
	// CharacterLevelsVisible
	//========================================	
	bool GetCharacterLevelsVisible()
	{
		return GetBool( CHAR_LEVELS_VIS );
	}
	

	void SetCharacterLevelsVisible( bool is_visible )
	{
		SetBool( CHAR_LEVELS_VIS, is_visible );
	}
	
	
	//========================================
	// CharacterStatsVisible
	//========================================	
	bool GetCharacterStatsVisible()
	{
		return GetBool( CHAR_STATS_VIS );
	}
	

	void SetCharacterStatsVisible( bool is_visible )
	{
		SetBool( CHAR_STATS_VIS, is_visible );
	}
	
	//========================================
	// CharacterModifiersVisible
	//========================================	
	bool GetCharacterModifiersVisible()
	{
		return GetBool( CHAR_MODIFIERS_VIS );
	}

	void SetCharacterModifiersVisible( bool is_visible )
	{
		SetBool( CHAR_MODIFIERS_VIS, is_visible );
	}	
	
	//========================================
	// CharacterAgentsVisible
	//========================================	
	bool GetCharacterAgentsVisible()
	{
		return GetBool( CHAR_AGENTS_VIS );
	}

	void SetCharacterAgentsVisible( bool is_visible )
	{
		SetBool( CHAR_AGENTS_VIS, is_visible );
	}
	
	//========================================
	// CharacterDebugVisible
	//========================================	
	bool GetCharacterDebugVisible()
	{
		return GetBool( CHAR_DEBUG_VIS );
	}

	void SetCharacterDebugVisible( bool is_visible )
	{
		SetBool( CHAR_DEBUG_VIS, is_visible );
	}	
		
	//========================================
	// CharacterStomachVisible
	//========================================	
	bool GetCharacterStomachVisible()
	{
		return GetBool( CHAR_STOMACH_VIS );
	}

	void SetCharacterStomachVisible( bool is_visible )
	{
		SetBool( CHAR_STOMACH_VIS, is_visible );
	}	
	
	//========================================
	// FreeCameraCrosshairVisible
	//========================================	
	bool GetFreeCameraCrosshairVisible()
	{
		return GetBool( FREE_CAMERA_CROSSHAIR );
	}

	void SetFreeCameraCrosshairVisible( bool is_visible )
	{
		SetBool( FREE_CAMERA_CROSSHAIR, is_visible );
	}
	
	//========================================
	// VersionVisible
	//========================================	
	bool GetVersionVisible()
	{
		return GetBool( VERSION_VIS );
	}

	void SetVersionVisible( bool is_visible )
	{
		SetBool( VERSION_VIS, is_visible );
	}
	
	//========================================
	// Merge Type
	//========================================	
	bool GetMergeType()
	{
		return GetBool( MERGE_TYPE );
	}

	void SetMergeType( bool is_visible )
	{
		SetBool( MERGE_TYPE, is_visible );
	}	
	
	//========================================
	// Soundset Filter
	//========================================	
	string GetSoundsetFilter()
	{
		return GetString( SOUNDFILTER );
	}

	void SetSoundsetFilter( string content )
	{
		SetString( SOUNDFILTER, content );
	}
	
	//========================================
	// TemperatureVisible
	//========================================	
	bool GetTempVisible()
	{
		return GetBool( TEMP_VIS );
	}

	void SetTempVisible( bool is_visible )
	{
		SetBool( TEMP_VIS, is_visible );
	}
	
	//========================================
	// Show Item Preview
	//========================================	
	bool GetShowItemPreview()
	{
		return GetBool( ITEM_PREVIEW );
	}

	void SetShowItemPreview( bool show )
	{
		SetBool( ITEM_PREVIEW, show );
	}
	
	
	//========================================
	// ConfigClassesFlag
	//========================================	
	int GetConfigClassesFlag()
	{
		return GetInt( CONFIG_CLASSES_FLAG );
	}

	void SetConfigClassesFlag( int flag )
	{
		SetInt( CONFIG_CLASSES_FLAG, flag );
	}
	
	
	//========================================
	// Item Category Flag
	//========================================	
	int GetItemCategoryFlag()
	{
		return GetInt( ITEM_CATEGORY_FLAG );
	}

	void SetItemCategoryFlag( int flag )
	{
		SetInt( ITEM_CATEGORY_FLAG, flag );
	}
	
	//========================================
	// Presets
	//========================================
	TStringArray GetPresets()
	{
		return m_PresetList;
	}
	
	protected void SetPreset( string preset_name, int index = -1 )
	{
		array<ref CfgParam> params = GetArray( PRESET_LIST );
		CfgParamString param;
		
		if ( index == -1 )
		{
			param = new CfgParamString( "" );
			param.SetValue( preset_name );
			params.Insert( param );
			m_PresetList.Insert( preset_name );
		}
		else
		{
			param = CfgParamString.Cast( params.Get( index ) );
			param.SetValue( preset_name );
			params.Set( index, param );
			m_PresetList.Set( index, preset_name );
		}
		
		SaveConfigToFile();
	}
	
	protected void RemovePreset( string preset_name )
	{
		array<ref CfgParam> params = GetArray( PRESET_LIST );
		for ( int i = 0; i < params.Count(); i++ )
		{
			CfgParamString param = CfgParamString.Cast( params.Get(i) );
			if ( param.GetValue() == preset_name )
			{
				params.RemoveItem( param );
				m_PresetList.RemoveItem( preset_name );
			}
		}
	}
	
	bool GetPresetItems( string preset_name, out TStringArray arr, string param_name = "" )
	{
		if ( m_PresetList.Find(preset_name) == -1 )
		{
			return false;
		}
		if(param_name == "")
			param_name = SUB_PARAM_ITEM_NAME;
		GetSubParametersInStringArray( preset_name, param_name, arr );
		
		return true;
	}
	
	bool PresetAdd( string preset_name )
	{
		if ( m_PresetList.Find(preset_name) > -1 )
		{
			return false;
		}
		
		SetPreset( preset_name );
		SetArray( preset_name, new array<ref CfgParam> );
		SaveConfigToFile();
		
		return true;
	}
	
	bool PresetRemove( string preset_name )
	{
		if ( m_PresetList.Find(preset_name) == -1 )
		{
			return false;
		}
		
		RemovePreset( preset_name );
		RemoveParamByName( preset_name );
		SaveConfigToFile();
		
		return true;
	}
	
	bool PresetRename( string old_preset_name, string new_preset_name )
	{
		int index = m_PresetList.Find( old_preset_name );
		
		if ( index == -1 )
		{
			return false;
		}
		
		SetPreset( new_preset_name, index );
		RenameParam( old_preset_name, new_preset_name );
		SaveConfigToFile();
		
		return true;
	}
	
	bool ItemAddToPreset( string preset_name, string item_name )
	{		
		if ( m_PresetList.Find( preset_name ) == -1 )
		{
			return false;
		}
		
		CfgParamString param = new CfgParamString( SUB_PARAM_ITEM_NAME );
		param.SetValue( item_name );
		
		CfgParamArray item_params = new CfgParamArray( SUB_PARAM_ITEM );
		item_params.InsertValue( param );
		
		array<ref CfgParam> params = GetArray( preset_name );
		params.Insert( item_params );
		
		SaveConfigToFile();
		
		return true;
	}
	
	bool ItemRemoveFromPreset( string preset_name, int item_index )
	{
		if ( m_PresetList.Find( preset_name ) == -1 )
		{
			return false;
		}
		
		array<ref CfgParam> params = GetArray( preset_name );
		
		if ( item_index >= params.Count() )
		{
			return false;
		}
		
		params.Remove( item_index );

		SaveConfigToFile();
		
		return true;
	}

	bool SwapItem( string preset_name, int item1_index, int item2_index )
	{
		array<ref CfgParam> params = GetArray( preset_name );
		
		if ( item1_index >= params.Count() || item2_index >= params.Count() )
		{
			return false;
		}
		
		params.SwapItems( item1_index, item2_index );
		SaveConfigToFile();
		
		return true;
	}
	
	protected array<ref CfgParam> GetItemParams( string preset_name, int item_index )
	{
		if ( m_PresetList.Find( preset_name ) == -1 )
		{
			return NULL;
		}
		
		array<ref CfgParam> params = GetArray( preset_name );
		
		if ( item_index >= params.Count() )
		{
			return NULL;
		}
		
		CfgParamArray params_array = CfgParamArray.Cast( params.Get( item_index ) );
		array<ref CfgParam> item_params = params_array.GetValues();
		
		return item_params;
	}
	
	protected CfgParam GetItemParam( string preset_name, int item_index, string param_name )
	{
		array<ref CfgParam> item_params = GetItemParams( preset_name, item_index );
		
		if ( item_params == NULL )
		{
			return NULL;
		}
		
		CfgParam param = NULL;
		for ( int i = 0; i < item_params.Count(); i++ )
		{
			if ( item_params.Get(i).GetName() == param_name )
			{
				param = item_params.Get(i);
			}
		}
		
		return param;
	}
	
	float GetItemHealth( string preset_name, int item_index )
	{
		CfgParam param = GetItemParam( preset_name, item_index, SUB_PARAM_ITEM_HEALTH );
		
		if ( param == NULL )
		{
			return 100;
		}
		else
		{
			CfgParamFloat param_float = CfgParamFloat.Cast( param );
			return param_float.GetValue();
		}
	}	
	
	bool SetItemHealth( string preset_name, int item_index, float health )
	{
		CfgParam param = GetItemParam( preset_name, item_index, SUB_PARAM_ITEM_HEALTH );
		
		if ( param != NULL )
		{
			CfgParamFloat param_float = CfgParamFloat.Cast( param );
			param_float.SetValue( health );
		}
		else
		{
			array<ref CfgParam> item_params = GetItemParams( preset_name, item_index );
			
			if ( item_params != NULL )
			{
				CfgParamFloat new_param = new CfgParamFloat( SUB_PARAM_ITEM_HEALTH );
				new_param.SetValue( health );
				item_params.Insert( new_param );
			}
		}

		SaveConfigToFile();
		
		return true;
	}
	
	int GetItemQuantity( string preset_name, int item_index )
	{
		CfgParam param = GetItemParam( preset_name, item_index, SUB_PARAM_ITEM_QUANTITY );
		
		if ( param == NULL )
		{
			return -1;
		}
		else
		{
			CfgParamInt param_int = CfgParamInt.Cast( param );
			return param_int.GetValue();
		}
	}
	
	bool SetItemQuantity( string preset_name, int item_index, int quantity )
	{
		CfgParam param = GetItemParam( preset_name, item_index, SUB_PARAM_ITEM_QUANTITY );
		
		if ( param != NULL )
		{
			CfgParamInt param_int = CfgParamInt.Cast( param );
			param_int.SetValue( quantity );
		}
		else
		{
			array<ref CfgParam> item_params = GetItemParams( preset_name, item_index );
			
			if ( item_params != NULL )
			{
				CfgParamInt new_param = new CfgParamInt( SUB_PARAM_ITEM_QUANTITY );
				new_param.SetValue( quantity );
				item_params.Insert( new_param );
			}
		}

		SaveConfigToFile();
		
		return true;
	}
}