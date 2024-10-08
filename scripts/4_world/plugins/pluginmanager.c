class PluginManager
{	
	private ref array<typename>					m_PluginRegister;	// list of modules for creation
	private ref map<typename, ref PluginBase>	m_PluginsPtrs;		// plugin, plugin pointer
	
	//=================================
	// Constructor
	//=================================
	void PluginManager()
	{
		m_PluginRegister	= new array<typename>;
		m_PluginsPtrs		= new map<typename, ref PluginBase>;
	}
	
	//=================================
	// Destructor
	//=================================
	void ~PluginManager()
	{
		int i;
		PluginBase plugin;
		
		for ( i = 0; i < m_PluginsPtrs.Count(); ++i)
		{
			plugin = m_PluginsPtrs.GetElement(i);
			if ( plugin != NULL )
			{
				plugin.OnDestroy();
				delete plugin;
			}
		}
		
		GetGame().GetUpdateQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.MainOnUpdate);
	}
	
	//=================================
	// Init
	//=================================
	void Init()
	{	
		//----------------------------------------------------------------------
		// Register modules
		//----------------------------------------------------------------------
		//				Module Class Name 								Client	Server
		//----------------------------------------------------------------------
		RegisterPlugin( "PluginHorticulture",							true, 	true );
		RegisterPlugin( "PluginRepairing",								true, 	true );
		RegisterPlugin( "PluginPlayerStatus",							true, 	true );
		RegisterPlugin( "PluginMessageManager",							true, 	true );
		RegisterPlugin( "PluginLifespan",								true, 	true );
		RegisterPlugin( "PluginVariables",								true, 	true );
		RegisterPlugin( "PluginObjectsInteractionManager",				false, 	true );
		RegisterPlugin( "PluginRecipesManager",							true, 	true );
		RegisterPlugin( "PluginTransmissionAgents",						true, 	true );
		RegisterPlugin( "PluginConfigEmotesProfile",					true, 	true );
		RegisterPlugin( "PluginPresenceNotifier",						true,	false );
		RegisterPlugin( "PluginAdminLog",								false, 	true );
		
		// Developer + Diag
		RegisterPluginDiag( "PluginKeyBinding",							true, 	false );
		RegisterPluginDiag( "PluginDeveloper",							true, 	true );
		RegisterPluginDiag( "PluginDeveloperSync",						true, 	true );
		RegisterPluginDiag( "PluginDiagMenuClient",						true, 	false );
		RegisterPluginDiag( "PluginDiagMenuServer",						false, 	true );
		RegisterPluginDiag( "PluginPermanentCrossHair",					true,	false );
		RegisterPluginDiag( "PluginRemotePlayerDebugClient",			true,	false );
		RegisterPluginDiag( "PluginRemotePlayerDebugServer",			false,	true );
		RegisterPluginDiag( "PluginUniversalTemperatureSourceClient",	true, 	false );
		RegisterPluginDiag( "PluginUniversalTemperatureSourceServer",	false, 	true );
		RegisterPluginDiag( "PluginTargetTemperature",					true, 	false );
		RegisterPluginDiag( "PluginDrawCheckerboard",					true,	false );
		RegisterPluginDiag( "PluginItemDiagnostic",						true, 	true );
		RegisterPluginDiag( "PluginDayZCreatureAIDebug",				true, 	true );
		
		// Only In Debug / Internal
		RegisterPluginDebug( "PluginConfigViewer",						true, 	true );
		RegisterPluginDebug( "PluginLocalEnscriptHistory",				true, 	true );
		RegisterPluginDebug( "PluginLocalEnscriptHistoryServer",		true, 	true );
		
		RegisterPluginDebug( "PluginSceneManager",						true, 	true );
		RegisterPluginDebug( "PluginConfigScene",						true, 	true );
		RegisterPluginDebug( "PluginMissionConfig",						true, 	true );
		RegisterPluginDebug( "PluginConfigEmotesProfile",				true, 	true );
		RegisterPluginDebug( "PluginConfigDebugProfile",				true, 	true );
		RegisterPluginDebug( "PluginConfigDebugProfileFixed",			true, 	true );
		
		RegisterPluginDebug( "PluginDayzPlayerDebug",					true, 	true );
		RegisterPluginDebug( "PluginDayZInfectedDebug",					true, 	true );
		RegisterPluginDebug( "PluginDoorRuler",							true, 	false );
		RegisterPluginDebug( "PluginCharPlacement",						true, 	false );
		//RegisterPluginDebug( "PluginSoundDebug",						false,	false );
		RegisterPluginDebug( "PluginCameraTools",						true, 	true );
		RegisterPluginDebug( "PluginNutritionDumper",					true, 	false );
		
		GetGame().GetUpdateQueue(CALL_CATEGORY_GAMEPLAY).Insert(MainOnUpdate);
	}
	
	//=================================
	// PluginsInit
	//=================================
	void PluginsInit()
	{
		int i = 0;
		int regCount = m_PluginRegister.Count();
		
		array<PluginBase> pluginPtrs = {};
		pluginPtrs.Reserve(regCount);
		
		foreach (typename pluginType : m_PluginRegister)
		{
			PluginBase moduleExist = GetPluginByType( pluginType );
			if ( moduleExist )
			{
				m_PluginsPtrs.Remove( pluginType );
			}
			
			PluginBase moduleNew = PluginBase.Cast(pluginType.Spawn());
			m_PluginsPtrs.Set(pluginType, moduleNew);
			pluginPtrs.Insert(moduleNew);
		}
		
		foreach (PluginBase plugin : pluginPtrs)
		{
			if ( plugin )
			{
				plugin.OnInit();
			}
		}
	}
	
	//=================================
	// MainOnUpdate
	//=================================
	void MainOnUpdate(float delta_time)
	{
		for ( int i = 0; i < m_PluginsPtrs.Count(); ++i)
		{
			PluginBase plugin = m_PluginsPtrs.GetElement(i);
			if ( plugin != NULL )
			{
				plugin.OnUpdate(delta_time);
			}
		}
	}
	
	/**
	\brief Returns registred plugin by class type, better is to use global funtion GetPlugin(typename plugin_type)
		\param module_tpye \p typename class type of plugin
		\return \p PluginBase
		@code
			PluginRepairing plugin = GetPluginManager().GetPluginByType(PluginRepairing);
		@endcode
	*/
	//=================================
	// GetPluginByType
	//=================================
	PluginBase GetPluginByType( typename plugin_type )
	{
		if ( m_PluginsPtrs.Contains( plugin_type ) )
		{
			return m_PluginsPtrs.Get( plugin_type );
		}
		
		return null;
	}
	
	/**
	\brief Register new PluginBase to PluginManager for storing and handling plugin.
		\param module_tpye \p typename class type of plugin
		\return \p void
		@code
			class PluginRepairing extends PluginBase
			{
			...
			}
			
			RegisterPlugin(PluginRepairing);
		@endcode
	*/
	//=================================
	// RegisterPlugin
	//=================================
	protected void RegisterPlugin( string plugin_class_name, bool reg_on_client, bool reg_on_server, bool reg_on_release = true )
	{
		if ( !reg_on_client )
		{
			if ( GetGame().IsMultiplayer() && GetGame().IsClient() )
			{
				return;
			}
		}
		
		if ( !reg_on_server )
		{
			if ( GetGame().IsMultiplayer() )
			{
				if ( GetGame().IsServer() )
				{
					return;
				}
			}
		}
		
		if ( !reg_on_release )
		{
			if ( !GetGame().IsDebug() )
			{
				return;
			}
		}
		
		m_PluginRegister.Insert( plugin_class_name.ToType() );
	}
	
	/**
	\brief Register new PluginBase to PluginManager for storing and handling plugin.
		\param module_tpye \p typename class type of plugin
		\return \p void
		@code
			class PluginRepairing extends PluginBase
			{
			...
			}
			
			RegisterPlugin(PluginRepairing);
		@endcode
	*/
	//=================================
	// RegisterPlugin
	//=================================
	protected void RegisterPluginDebug( string plugin_class_name, bool reg_on_client, bool reg_on_server )
	{
		RegisterPlugin(plugin_class_name, reg_on_client, reg_on_server, false);
	}
	//=================================
	// RegisterPlugin
	//=================================
	protected void RegisterPluginDiag( string plugin_class_name, bool reg_on_client, bool reg_on_server )
	{
		#ifdef DIAG_DEVELOPER
		RegisterPlugin(plugin_class_name, reg_on_client, reg_on_server, true);
		#else
		return;
		#endif		
	}
	
	//---------------------------------
	// UnregisterPlugin
	//---------------------------------
	protected bool UnregisterPlugin( string plugin_class_name )
	{
		typename plugin_type = plugin_class_name.ToType();
		
		if ( m_PluginRegister.Find( plugin_type ) != -1 )
		{
			m_PluginRegister.RemoveItem( plugin_type );
			return true;
		}
		
		return false;
	}
}

ref PluginManager g_Plugins;

/**
\brief Returns registred plugin by class type, better is to use global funtion GetPlugin(typename plugin_type)
	\param module_tpye \p typename class type of plugin
	\return \p PluginBase
	@code
		PluginRepairing plugin = GetPluginManager().GetPluginByType(PluginRepairing);
	@endcode
*/
PluginManager GetPluginManager()
{	
	/*
	if ( g_Plugins == NULL )
	{
		PluginManagerInit();
	}
	*/
	
	return g_Plugins;
}


void PluginManagerInit()
{	
	if (g_Plugins == NULL)
	{
		g_Plugins = new PluginManager;
		g_Plugins.Init();
		g_Plugins.PluginsInit();
	}
}

void PluginManagerDelete()
{	
	if ( g_Plugins )
	{
		delete g_Plugins;
		g_Plugins = NULL;
	}
}

bool IsPluginManagerExists()
{
	if ( g_Plugins != null )
	{
		return true;
	}	
	
	return false;
}

PluginBase GetPlugin(typename plugin_type)
{
	PluginBase plugin = null;
	
	if ( IsPluginManagerExists() )
	{
		plugin = GetPluginManager().GetPluginByType(plugin_type);
	
		if ( plugin == null )
		{
			#ifdef DIAG_DEVELOPER
			if ( IsPluginManagerExists() )
			{
				PrintString("Module " + plugin_type.ToString() + " is not Registred in PluginManager.c!");
				DumpStack();
			}
			#endif
		}
	}
	
	return plugin;
}

bool IsModuleExist(typename plugin_type)
{
	if ( IsPluginManagerExists() )
	{
		return ( GetPlugin(plugin_type) != NULL );
	}
	
	return false;
}
