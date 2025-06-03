class ScriptConsoleCameraDOFPreset
{
	string Name = "New Preset";
	float FocusDistance = 1.0;
	float FocusLength = 0.0;
	float FocusLengthNear = -1.0;
	float Blur = 0.0;
	float FocusDepthOffset = 1.0;
}

class ScriptConsoleWeatherPreset
{
	string Name = "New Preset";
	float WOvercast;
	float WRain;
	float WSnow;
	float WFog;
	float WWindMagnitude;
	float WWindDir;
	float WVolFogDD;
	float WVolFogHD;
	float WVolFogHB;
}

typedef Param3<string, bool, vector> LocationParams;//  param1 - name, param2 - isCustom?, param3 - position
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
	protected const string HEALTH_VIS						= "console_health_visible";
	protected const string HORTICULTURE_VIS					= "console_horticulture_visible";
	protected const string SUB_PARAM_ITEM					= "item";
	protected const string SUB_PARAM_ITEM_NAME				= "name";
	protected const string SUB_PARAM_ITEM_HEALTH			= "health";
	protected const string SUB_PARAM_ITEM_QUANTITY			= "quantity";
	protected const string SUB_PARAM_ITEM_WITH_PHYSICS		= "with_physics";
	protected const string LOGS_ENABLED						= "logs_enabled";
	protected const string CONFIG_CLASSES_FLAG				= "toggled_config_classes_flag";
	protected const string ITEM_CATEGORY_FLAG				= "toggled_item_categories_flag";
	protected const string ITEM_PREVIEW						= "show_item_preview";
	protected const string BATCH_RECT						= "batch_spawn_rect";
	protected const string BATCH_QUANT						= "batch_spawn_quantity";
	protected const string SOUNDFILTER						= "soundset_editbox";
	protected const string ITEMDEBUG						= "item_debug";
	protected const string SPAWN_LOC_INDEX					= "spawn_loc_index";
	protected const string FILTER_REVERSED					= "filter_order_reversed";
	
	protected const string CAMERA_PRESETS_LIST				= "camera_presets";
	protected const string WEATHER_PRESETS_LIST				= "weather_presets";

	protected ref map<string, ref CfgParam>					m_DefaultValues;
	protected ref TStringArray 								m_PresetList;
	protected const string POSITION_NAME_ROOT				= "console_positions_";
	
	protected ref array<ref ScriptConsoleCameraDOFPreset> 	m_CameraPresets;
	protected ref array<ref ScriptConsoleWeatherPreset> 	m_WeatherPresets;
	
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

	protected array<ref CfgParam> GetArray(string key)
	{
		CfgParamArray paramsOut;
		if (ParamExist(key))
		{
			paramsOut = CfgParamArray.Cast( GetParamByName(key, CFG_TYPE_ARRAY));
			return paramsOut.GetValues();
		}
		else
		{
			if (m_DefaultValues.Contains(key))
			{
				CfgParamArray default_param = CfgParamArray.Cast(m_DefaultValues.Get(key));
				paramsOut = SetArray(key, default_param.GetValues());
			}
			else
			{
				array<ref CfgParam> param = new array<ref CfgParam>();
				paramsOut = SetArray(key, param);
			}			
		}

		return paramsOut.GetValues();
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
		m_DefaultValues.Insert(HEALTH_VIS,				GetNewCfgParamBool(false) );
		m_DefaultValues.Insert(HORTICULTURE_VIS,		GetNewCfgParamBool(false) );
		m_DefaultValues.Insert(MERGE_TYPE,				GetNewCfgParamBool(false) );
		m_DefaultValues.Insert(ITEM_PREVIEW,			GetNewCfgParamBool(true) );
		m_DefaultValues.Insert(BATCH_RECT,				GetNewCfgParamArray(GetDefaultBatchRectParams()) );
		m_DefaultValues.Insert(BATCH_QUANT,				GetNewCfgParamInt(10) );
		m_DefaultValues.Insert(SOUNDFILTER,				GetNewCfgParamString("") );
		m_DefaultValues.Insert(ITEMDEBUG,				GetNewCfgParamString("0 0 0") );
		m_DefaultValues.Insert(SPAWN_LOC_INDEX,			GetNewCfgParamInt(0));
		m_DefaultValues.Insert(FILTER_REVERSED,			GetNewCfgParamBool(false));
		m_DefaultValues.Insert(CAMERA_PRESETS_LIST,		GetNewCfgParamArray(GetDefaultCameraPresetsParams()));
		m_DefaultValues.Insert(WEATHER_PRESETS_LIST,	GetNewCfgParamArray(GetDefaultWeatherPresetsParams()));
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

	array<ref CfgParam> GetDefaultCameraPresetsParams()
	{
		array<ref CfgParam> params = new array<ref CfgParam>;
		CfgParamArray presetParam;
		CfgParamString paramName;
		CfgParamFloat paramFocusDistance, paramFocusLength, paramFocusLengthNear, paramBlur, paramFocusDepthOffset;
		
		// 50m distance DOF (Buildings / action)
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("50m distance DOF (Buildings / action)");
		
		paramFocusDistance = new CfgParamFloat("FocusDistance");
		paramFocusDistance.SetValue(50.0);
		
		paramFocusLength = new CfgParamFloat("FocusLength");
		paramFocusLength.SetValue(2450.0);
		
		paramFocusLengthNear = new CfgParamFloat("FocusLengthNear");
		paramFocusLengthNear.SetValue(750.0);
		
		paramBlur = new CfgParamFloat("Blur");
		paramBlur.SetValue(4.0);
		
		paramFocusDepthOffset = new CfgParamFloat("FocusDepthOffset");
		paramFocusDepthOffset.SetValue(8.0);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramFocusDistance);
		presetParam.InsertValue(paramFocusLength);
		presetParam.InsertValue(paramFocusLengthNear);
		presetParam.InsertValue(paramBlur);
		presetParam.InsertValue(paramFocusDepthOffset);
		params.Insert(presetParam);
		
		// 15m distance soft blur (close objects are blurred)
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("15m distance soft blur (close objects are blurred)");
		
		paramFocusDistance = new CfgParamFloat("FocusDistance");
		paramFocusDistance.SetValue(15.0);
		
		paramFocusLength = new CfgParamFloat("FocusLength");
		paramFocusLength.SetValue(1050.0);
		
		paramFocusLengthNear = new CfgParamFloat("FocusLengthNear");
		paramFocusLengthNear.SetValue(500.0);
		
		paramBlur = new CfgParamFloat("Blur");
		paramBlur.SetValue(4.0);
		
		paramFocusDepthOffset = new CfgParamFloat("FocusDepthOffset");
		paramFocusDepthOffset.SetValue(10.0);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramFocusDistance);
		presetParam.InsertValue(paramFocusLength);
		presetParam.InsertValue(paramFocusLengthNear);
		presetParam.InsertValue(paramBlur);
		presetParam.InsertValue(paramFocusDepthOffset);
		params.Insert(presetParam);
		
		// Full Body Character
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Full Body Character");
		
		paramFocusDistance = new CfgParamFloat("FocusDistance");
		paramFocusDistance.SetValue(5.0);
		
		paramFocusLength = new CfgParamFloat("FocusLength");
		paramFocusLength.SetValue(1050.0);
		
		paramFocusLengthNear = new CfgParamFloat("FocusLengthNear");
		paramFocusLengthNear.SetValue(100.0);
		
		paramBlur = new CfgParamFloat("Blur");
		paramBlur.SetValue(4.0);
		
		paramFocusDepthOffset = new CfgParamFloat("FocusDepthOffset");
		paramFocusDepthOffset.SetValue(10.0);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramFocusDistance);
		presetParam.InsertValue(paramFocusLength);
		presetParam.InsertValue(paramFocusLengthNear);
		presetParam.InsertValue(paramBlur);
		presetParam.InsertValue(paramFocusDepthOffset);
		params.Insert(presetParam);
		
		// Close up
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Close up");
		
		paramFocusDistance = new CfgParamFloat("FocusDistance");
		paramFocusDistance.SetValue(1.0);
		
		paramFocusLength = new CfgParamFloat("FocusLength");
		paramFocusLength.SetValue(450.0);
		
		paramFocusLengthNear = new CfgParamFloat("FocusLengthNear");
		paramFocusLengthNear.SetValue(100.0);
		
		paramBlur = new CfgParamFloat("Blur");
		paramBlur.SetValue(4.0);
		
		paramFocusDepthOffset = new CfgParamFloat("FocusDepthOffset");
		paramFocusDepthOffset.SetValue(10.0);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramFocusDistance);
		presetParam.InsertValue(paramFocusLength);
		presetParam.InsertValue(paramFocusLengthNear);
		presetParam.InsertValue(paramBlur);
		presetParam.InsertValue(paramFocusDepthOffset);
		params.Insert(presetParam);

		// NEW Close up
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("NEW Close up");
		
		paramFocusDistance = new CfgParamFloat("FocusDistance");
		paramFocusDistance.SetValue(2.0);
		
		paramFocusLength = new CfgParamFloat("FocusLength");
		paramFocusLength.SetValue(750.0);
		
		paramFocusLengthNear = new CfgParamFloat("FocusLengthNear");
		paramFocusLengthNear.SetValue(500.0);
		
		paramBlur = new CfgParamFloat("Blur");
		paramBlur.SetValue(4.0);
		
		paramFocusDepthOffset = new CfgParamFloat("FocusDepthOffset");
		paramFocusDepthOffset.SetValue(10.0);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramFocusDistance);
		presetParam.InsertValue(paramFocusLength);
		presetParam.InsertValue(paramFocusLengthNear);
		presetParam.InsertValue(paramBlur);
		presetParam.InsertValue(paramFocusDepthOffset);
		params.Insert(presetParam);

		return params;
	}
	
	array<ref CfgParam> GetDefaultWeatherPresetsParams()
	{
		array<ref CfgParam> params = new array<ref CfgParam>;
		CfgParamArray presetParam;
		CfgParamString paramName;
		CfgParamFloat paramOvercast, paramRain, paramSnow, paramFog, paramWindMagnitude, paramWindDirection, paramVolFogDD, paramVolFogHD, paramVolFogHB;
		
		// Chernarus - Clear
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Chernarus - Clear");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0.04);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(10);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(-1);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.005);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(0.9);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(50);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Chernarus - Cloudy
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Chernarus - Cloudy");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0.4);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0.04);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(10);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(-2.2);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.013);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(0.94);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(50);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Chernarus - Bad
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Chernarus - Bad");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0.7);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0.3);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0.04);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(12);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(1.5);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.015);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(0.97);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(50);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Chernarus - Storm
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Chernarus - Storm");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0.9);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0.55);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0.04);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(15);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(1.2);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.011);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(0.99);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(50);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Chernarus - Heavy Storm
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Chernarus - Heavy Storm");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(1);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0.9);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0.04);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(17);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(1.2);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.007);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(1);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(50);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		//------------------------------------------------------------------------
		
		// Livonia - Clear
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Livonia - Clear");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0.1);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(3);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(-2);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.012);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(0.91);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(170);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Livonia - Cloudy
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Livonia - Cloudy");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0.4);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0.1);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(9);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(-1.2);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.14);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(0.94);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(170);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Livonia - Bad
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Livonia - Bad");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0.7);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0.3);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0.1);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(10);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(-0.3);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.019);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(0.97);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(170);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Livonia - Storm
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Livonia - Storm");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0.85);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0.55);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0.1);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(12);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(0);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.018);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(0.985);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(170);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Livonia - Heavy Storm
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Livonia - Heavy Storm");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(1);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0.9);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0.1);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(18);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(0.6);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.005);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(1);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(170);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		//------------------------------------------------------------------------

		// Sakhal - Clear
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Sakhal - Clear");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0.07);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(2);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(0);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.002);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(1);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(0);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Sakhal - Cloudy
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Sakhal - Cloudy");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0.4);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0.35);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(9);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(1.1);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.058);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(1);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(0);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Sakhal - Bad
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Sakhal - Bad");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0.7);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0.65);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(12);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(-1.6);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.139);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(1);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(0);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Sakhal - Storm
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Sakhal - Storm");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(0.9);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(0.85);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(19);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(2.5);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.283);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(1);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(0);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);
		
		// Sakhal - Heavy Storm
		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue("Sakhal - Heavy Storm");
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(1);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(0);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(1);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(0);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(20);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(3);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(0.35);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(1);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(0);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		params.Insert(presetParam);		
		
		return params;
	}
	
	//========================================
	// OnInit
	//========================================
	override void OnInit()
	{
		super.OnInit();
		
		int i;
		
		array<ref CfgParam> params = GetArray(PRESET_LIST);
		m_PresetList = new TStringArray();
		for (i = 0; i < params.Count(); ++i)
		{
			CfgParamString param = CfgParamString.Cast(params.Get(i));
			m_PresetList.Insert(param.GetValue());
		}
		
		LoadCameraPresets();
		LoadWeatherPresets();
	}
	
	protected void LoadCameraPresets()
	{
		if (m_CameraPresets)
		{
			m_CameraPresets.Clear();
		}
		else
		{
			m_CameraPresets = new array<ref ScriptConsoleCameraDOFPreset>;
		}

		CfgParamArray presetParam;
		array<ref CfgParam> camera_params = GetArray(CAMERA_PRESETS_LIST);
		for (int i = 0; i < camera_params.Count(); i++)
		{
			presetParam = CfgParamArray.Cast(camera_params.Get(i));
			if (!presetParam)
				continue;

			CfgParamString paramCName = CfgParamString.Cast(presetParam.GetValueByName("Name", CFG_TYPE_STRING));
			CfgParamFloat paramFocusDistance = CfgParamFloat.Cast(presetParam.GetValueByName("FocusDistance", CFG_TYPE_FLOAT));
			CfgParamFloat paramFocusLength = CfgParamFloat.Cast(presetParam.GetValueByName("FocusLength", CFG_TYPE_FLOAT));
			CfgParamFloat paramFocusLengthNear = CfgParamFloat.Cast(presetParam.GetValueByName("FocusLengthNear", CFG_TYPE_FLOAT));
			CfgParamFloat paramBlur = CfgParamFloat.Cast(presetParam.GetValueByName("Blur", CFG_TYPE_FLOAT));
			CfgParamFloat paramFocusDepthOffset = CfgParamFloat.Cast(presetParam.GetValueByName("FocusDepthOffset", CFG_TYPE_FLOAT));
			
			ScriptConsoleCameraDOFPreset preset = new ScriptConsoleCameraDOFPreset();
			preset.Name = paramCName.GetValue();
			preset.FocusDistance = paramFocusDistance.GetValue();
			preset.FocusLength = paramFocusLength.GetValue();
			preset.FocusLengthNear = paramFocusLengthNear.GetValue();
			preset.Blur = paramBlur.GetValue();
			preset.FocusDepthOffset = paramFocusDepthOffset.GetValue();
			m_CameraPresets.Insert(preset);
		}
	}
	
	protected void LoadWeatherPresets()
	{
		if (m_WeatherPresets)
		{
			m_WeatherPresets.Clear();
		}
		else
		{
			m_WeatherPresets = new array<ref ScriptConsoleWeatherPreset>;
		}

		CfgParamArray presetParam;
		array<ref CfgParam> weather_params = GetArray(WEATHER_PRESETS_LIST);
		for (int i = 0; i < weather_params.Count(); i++)
		{
			presetParam = CfgParamArray.Cast(weather_params.Get(i));
			if (!presetParam)
				continue;

			CfgParamString paramWName = CfgParamString.Cast(presetParam.GetValueByName("Name", CFG_TYPE_STRING));
			CfgParamFloat paramOvercast = CfgParamFloat.Cast(presetParam.GetValueByName("Overcast", CFG_TYPE_FLOAT));
			CfgParamFloat paramRain = CfgParamFloat.Cast(presetParam.GetValueByName("Rain", CFG_TYPE_FLOAT));
			CfgParamFloat paramSnow = CfgParamFloat.Cast(presetParam.GetValueByName("Snow", CFG_TYPE_FLOAT));
			CfgParamFloat paramFog = CfgParamFloat.Cast(presetParam.GetValueByName("Fog", CFG_TYPE_FLOAT));
			CfgParamFloat paramWindMagnitude = CfgParamFloat.Cast(presetParam.GetValueByName("WindMagnitude", CFG_TYPE_FLOAT));
			CfgParamFloat paramWindDirection = CfgParamFloat.Cast(presetParam.GetValueByName("WindDirection", CFG_TYPE_FLOAT));
			CfgParamFloat paramVolFogDD = CfgParamFloat.Cast(presetParam.GetValueByName("VolFogDD", CFG_TYPE_FLOAT));
			CfgParamFloat paramVolFogHD = CfgParamFloat.Cast(presetParam.GetValueByName("VolFogHD", CFG_TYPE_FLOAT));
			CfgParamFloat paramVolFogHB = CfgParamFloat.Cast(presetParam.GetValueByName("VolFogHB", CFG_TYPE_FLOAT));
			
			ScriptConsoleWeatherPreset preset = new ScriptConsoleWeatherPreset();
			preset.Name = paramWName.GetValue();
			preset.WOvercast = paramOvercast.GetValue();
			preset.WRain = paramRain.GetValue();
			preset.WSnow = paramSnow.GetValue();
			preset.WFog = paramFog.GetValue();
			preset.WWindMagnitude =  paramWindMagnitude.GetValue();
			preset.WWindDir = paramWindDirection.GetValue();
			preset.WVolFogDD =  paramVolFogDD.GetValue();
			preset.WVolFogHD = paramVolFogHD.GetValue();
			preset.WVolFogHB = paramVolFogHB.GetValue();
			m_WeatherPresets.Insert(preset);
		}
	}

	void AddCameraPreset(string name, float focusDistance, float focusLength, float focusLengthNear, float blur, float focusDepthOffset)
	{
		CfgParamArray presetParam;
		CfgParamString paramName;
		array<ref CfgParam> camera_params = GetArray(CAMERA_PRESETS_LIST);

		for (int i = 0; i < camera_params.Count(); i++)
		{
			presetParam = CfgParamArray.Cast(camera_params.Get(i));
			if (!presetParam)
				return;

			paramName = CfgParamString.Cast(presetParam.GetValueByName("Name", CFG_TYPE_STRING));
			if (paramName.GetName() == name)
			{
				return;
			}
		}

		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue(name);
		
		CfgParamFloat paramFocusDistance = new CfgParamFloat("FocusDistance");
		paramFocusDistance.SetValue(focusDistance);
		
		CfgParamFloat paramFocusLength = new CfgParamFloat("FocusLength");
		paramFocusLength.SetValue(focusLength);
		
		CfgParamFloat paramFocusLengthNear = new CfgParamFloat("FocusLengthNear");
		paramFocusLengthNear.SetValue(focusLengthNear);
		
		CfgParamFloat paramBlur = new CfgParamFloat("Blur"); 
		paramBlur.SetValue(blur);
		
		CfgParamFloat paramFocusDepthOffset = new CfgParamFloat("FocusDepthOffset");
		paramFocusDepthOffset.SetValue(focusDepthOffset);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramFocusDistance);
		presetParam.InsertValue(paramFocusLength);
		presetParam.InsertValue(paramFocusLengthNear);
		presetParam.InsertValue(paramBlur);
		presetParam.InsertValue(paramFocusDepthOffset);
		camera_params.Insert(presetParam);
		
		SaveConfigToFile();
		
		ScriptConsoleCameraDOFPreset preset = new ScriptConsoleCameraDOFPreset();
		preset.Name = name;
		preset.FocusDistance = focusDistance;
		preset.FocusLength = focusLength;
		preset.FocusLengthNear = focusLengthNear;
		preset.Blur = blur;
		preset.FocusDepthOffset = focusDepthOffset;
		m_CameraPresets.Insert(preset);
	}
	
	void RemoveCameraPreset(string name)
	{
		int i;
		array<ref CfgParam> camera_params = GetArray(CAMERA_PRESETS_LIST);
		for (i = 0; i < camera_params.Count(); i++)
		{
			CfgParamArray presetParam = CfgParamArray.Cast(camera_params.Get(i));
			if (!presetParam)
				continue;

			CfgParamString paramName = CfgParamString.Cast(presetParam.GetValueByName("Name", CFG_TYPE_STRING));
			if (paramName.GetValue() == name)
			{
				camera_params.Remove(i);
				break;
			}
		}
		
		SaveConfigToFile();
		
		for (i = 0; i < m_CameraPresets.Count(); i++)
		{
			ScriptConsoleCameraDOFPreset cameraPreset = m_CameraPresets.Get(i);
			if (cameraPreset.Name == name)
			{
				m_CameraPresets.Remove(i);
				break;
			}
		}
	}
	
	void AddWeatherPreset(string name, float overcast, float rain, float snow, float fog, float windM, float windD, float volFogDD, float volFogHD, float volFogHB)
	{
		CfgParamArray presetParam;
		CfgParamString paramName;
		CfgParamFloat paramOvercast, paramRain, paramSnow, paramFog, paramWindMagnitude, paramWindDirection, paramVolFogDD, paramVolFogHD, paramVolFogHB;

		array<ref CfgParam> weather_params = GetArray(WEATHER_PRESETS_LIST);
		int weatherPresetIndex = -1;
		for (int i = 0; i < weather_params.Count(); i++)
		{
			presetParam = CfgParamArray.Cast(weather_params.Get(i));
			if (!presetParam)
				return;

			paramName = CfgParamString.Cast(presetParam.GetValueByName("Name", CFG_TYPE_STRING));
			if (paramName.GetValue() == name)
			{
				weatherPresetIndex = i;
				break;
			}
		}
		
		if (weatherPresetIndex > -1)
		{
			weather_params.Remove(weatherPresetIndex);
		}

		presetParam = new CfgParamArray("");
		paramName = new CfgParamString("Name");
		paramName.SetValue(name);
		paramOvercast = new CfgParamFloat("Overcast");
		paramOvercast.SetValue(overcast);
		paramRain = new CfgParamFloat("Rain");
		paramRain.SetValue(rain);
		paramSnow = new CfgParamFloat("Snow");
		paramSnow.SetValue(snow);
		paramFog = new CfgParamFloat("Fog");
		paramFog.SetValue(fog);
		paramWindMagnitude = new CfgParamFloat("WindMagnitude");
		paramWindMagnitude.SetValue(windM);
		paramWindDirection = new CfgParamFloat("WindDirection");
		paramWindDirection.SetValue(windD);
		paramVolFogDD = new CfgParamFloat("VolFogDD");
		paramVolFogDD.SetValue(volFogDD);
		paramVolFogHD = new CfgParamFloat("VolFogHD");
		paramVolFogHD.SetValue(volFogHD);
		paramVolFogHB = new CfgParamFloat("VolFogHB");
		paramVolFogHB.SetValue(volFogHB);
		
		presetParam.InsertValue(paramName);
		presetParam.InsertValue(paramOvercast);
		presetParam.InsertValue(paramRain);
		presetParam.InsertValue(paramSnow);
		presetParam.InsertValue(paramFog);
		presetParam.InsertValue(paramWindMagnitude);
		presetParam.InsertValue(paramWindDirection);
		presetParam.InsertValue(paramVolFogDD);
		presetParam.InsertValue(paramVolFogHD);
		presetParam.InsertValue(paramVolFogHB);
		
		if (weatherPresetIndex == -1)
		{
			weather_params.Insert(presetParam);
			
			ScriptConsoleWeatherPreset preset = new ScriptConsoleWeatherPreset();
			preset.Name = name;
			preset.WOvercast = overcast;
			preset.WRain = rain;
			preset.WSnow = snow;
			preset.WFog = fog;
			preset.WWindMagnitude = windM;
			preset.WWindDir = windD;
			preset.WVolFogDD = volFogDD;
			preset.WVolFogHD = volFogHD;
			preset.WVolFogHB = volFogHB;
			m_WeatherPresets.Insert(preset);
		}
		else
		{
			weather_params.InsertAt(presetParam, weatherPresetIndex);
			LoadWeatherPresets();
		}
		
		SaveConfigToFile();
	}
	
	void RemoveWeatherPreset(string name)
	{
		int i;
		array<ref CfgParam> weather_params = GetArray(WEATHER_PRESETS_LIST);
		for (i = 0; i < weather_params.Count(); i++)
		{
			CfgParamArray presetParam = CfgParamArray.Cast(weather_params.Get(i));
			if (!presetParam)
				continue;

			CfgParamString paramName = CfgParamString.Cast(presetParam.GetValueByName("Name", CFG_TYPE_STRING));
			if (paramName.GetValue() == name)
			{
				weather_params.Remove(i);
				break;
			}
		}
		
		SaveConfigToFile();
		
		for (i = 0; i < m_WeatherPresets.Count(); i++)
		{
			ScriptConsoleWeatherPreset weatherPreset = m_WeatherPresets.Get(i);
			if (weatherPreset.Name == name)
			{
				m_WeatherPresets.Remove(i);
				break;
			}
		}
	}
	
	//========================================
	// GetFileName
	//========================================
	override string GetFileName()
	{
		if (!FileExist(GetSubFolderName()))//---ALL LINES CONTAINING THIS COMMENT CAN BE REMOVED IN 1.23
			return "$profile:debugProfile.cfg";//---ALL LINES CONTAINING THIS COMMENT CAN BE REMOVED IN 1.23
		
		return GetSubFolderName() + CFG_FILE_DEBUG_PROFILE;
	}
	
	override string GetSubFolderName()
	{
		return CFG_FILE_DEBUG_DIR;
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
	// SpawnLocIndex
	//========================================
	int GetSpawnLocIndex()
	{
		return GetInt( SPAWN_LOC_INDEX );
	}
	
	void SetSpawnLocIndex( int value )
	{
		SetInt( SPAWN_LOC_INDEX, value );
	}
	
	//========================================
	// FilterOrder
	//========================================
	int GetFilterOrderReversed()
	{
		return GetBool( FILTER_REVERSED );
	}
	
	void SetFilterOrderReversed( bool reversed )
	{
		SetBool( FILTER_REVERSED, reversed );
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
	// ItemDebugPos
	//========================================	
	void SetItemDebugPos(vector pos)
	{
		 SetString( ITEMDEBUG ,pos.ToString(false));
	}
	

	vector GetItemDebugPos()
	{
		return GetString( ITEMDEBUG ).ToVector();
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
	// HealthVisible
	//========================================	
	bool GetHealthVisible()
	{
		return GetBool( HEALTH_VIS );
	}

	void SetHealthVisible( bool is_visible )
	{
		SetBool( HEALTH_VIS, is_visible );
	}
	
	//========================================
	// HorticultureVisible
	//========================================	
	bool GetHorticultureVisible()
	{
		return GetBool( HORTICULTURE_VIS );
	}

	void SetHorticultureVisible( bool is_visible )
	{
		SetBool( HORTICULTURE_VIS, is_visible );
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
	// User Location
	//========================================
	bool CustomLocationsAdd( string name, vector position )
	{
		string world = g_Game.GetWorldName();
		world.ToLower();
		
		CfgParamString locationName = new CfgParamString(PluginConfigDebugProfileFixed.SUB_PARAM_POS_NAME);
		locationName.SetValue(name);
		CfgParamString locationPos = new CfgParamString(PluginConfigDebugProfileFixed.SUB_PARAM_POS_VEC);
		locationPos.SetValue(position.ToString(false));
		
		CfgParamArray locationEntry = new CfgParamArray( locationName.GetValue() );
		locationEntry.InsertValue( locationName );
		locationEntry.InsertValue( locationPos );
		
		string paramName = POSITION_NAME_ROOT + world;
		array<ref CfgParam> params = GetArray(paramName);
		params.Insert(locationEntry);
		SetArray(paramName, params);
		SaveConfigToFile();
		
		return true;
	}
	
	void CustomLocationsRemove( string name )
	{
		if (!name)
			return;
		string world = g_Game.GetWorldName();
		world.ToLower();
		string paramName = POSITION_NAME_ROOT + world;
		array<ref CfgParam> params = GetArray(paramName);
		
		for (int i = 0; i < params.Count(); i++)
		{
			CfgParamArray param = CfgParamArray.Cast(params.Get(i));
			array<ref CfgParam> prms = param.GetValues();
			CfgParamString par = CfgParamString.Cast(prms.Get(0));
			if (par.GetValue() == name)
			{
				params.RemoveOrdered(i);
				break;
			}
		}
		
		//SetArray(paramName, params);
		SaveConfigToFile();
	}
	
	//========================================
	// Camera Presets
	//========================================
	array<ref ScriptConsoleCameraDOFPreset> GetCameraPresets()
	{
		return m_CameraPresets;
	}
	
	//========================================
	// Weather Presets
	//========================================
	array<ref ScriptConsoleWeatherPreset> GetWeatherPresets()
	{
		return m_WeatherPresets;
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
		
		if ( !params || item_index >= params.Count() )
		{
			return NULL;
		}
		if (params.IsValidIndex(item_index))
			CfgParamArray params_array = CfgParamArray.Cast( params.Get( item_index ) );
		if (params_array)
			return params_array.GetValues();
		
		return null;
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
			return -1;
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
			return 1;
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
	
	bool GetItemWithPhysics( string preset_name, int item_index )
	{
		CfgParam param = GetItemParam( preset_name, item_index, SUB_PARAM_ITEM_WITH_PHYSICS );
		
		if ( param == NULL )
		{
			return false;
		}
		else
		{
			CfgParamBool param_bool = CfgParamBool.Cast( param );
			return param_bool.GetValue();
		}
	}
	
	bool SetItemWithPhysics( string preset_name, int item_index, bool with_physics )
	{
		CfgParam param = GetItemParam( preset_name, item_index, SUB_PARAM_ITEM_WITH_PHYSICS );
		
		if ( param != NULL )
		{
			CfgParamBool param_bool = CfgParamBool.Cast( param );
			param_bool.SetValue( with_physics );
		}
		else
		{
			array<ref CfgParam> item_params = GetItemParams( preset_name, item_index );
			
			if ( item_params != NULL )
			{
				CfgParamBool new_param = new CfgParamBool( SUB_PARAM_ITEM_WITH_PHYSICS );
				new_param.SetValue( with_physics );
				item_params.Insert( new_param );
			}
		}

		SaveConfigToFile();
		
		return true;
	}
	
	//========================================
	// Locations
	//========================================

	void GetLocationsData( out array<ref LocationParams> arr , bool isCustom)
	{
		string world = g_Game.GetWorldName();
		world.ToLower();
		string paramName = POSITION_NAME_ROOT + world;
		
		TStringArray names = new TStringArray();
		GetSubParametersInStringArray( paramName, PluginConfigDebugProfileFixed.SUB_PARAM_POS_NAME, names );
		TStringArray positions = new TStringArray();
		GetSubParametersInStringArray( paramName, PluginConfigDebugProfileFixed.SUB_PARAM_POS_VEC, positions );
		int i;
		if (isCustom)
		{
			for ( i = names.Count() - 1; i >= 0 ; i--)
			{
				arr.Insert(new LocationParams(names.Get(i), isCustom, positions.Get(i).ToVector()));
			}
		}
		else
		{
			for (i = 0; i < names.Count(); i++)
			{
				arr.Insert(new LocationParams(names.Get(i), isCustom, positions.Get(i).ToVector()));
			}
		}
	}
}