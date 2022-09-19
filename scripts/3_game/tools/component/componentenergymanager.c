//-----------------------------
// ENERGY MANAGER
//-----------------------------
/*
Author: Boris Vacula

Documentation can be found at DayZ Confluence >> Camping & Squatting >> Electricity >> Energy Manager functionalities

This system controls storage, spending and sharing of energy between instances.

Every EntityAI object which uses this API gains these functions:
	-It can store some amout of energy
	-It can use this amount of energy for any kind of functionality
	-It can share this energy with other devices plugged into it
	-It will have an ON/OFF switch
*/

class ComponentEnergyManager : Component
{
	protected const  float DEFAULT_UPDATE_INTERVAL = 15;
	protected static bool 			m_DebugPlugs = false; //true; // Use this to toggle visualisation of plug connections
	protected 		Shape			m_DebugPlugArrow;
	
	protected 		bool			m_IsSwichedOn;
	protected 		bool			m_IsSwichedOnPreviousState; // Necesarry due to synchronization of m_IsSwichedOn
	protected 		bool			m_IsPassiveDevice;
	protected 		bool			m_IsWorking;
	protected 		bool			m_CanWork;
	protected 		bool			m_CanStopWork;
	protected 		bool			m_RestorePlugState; // After server restart, this value reports if this device was plugged into something or not at the end of last session.
	protected 		bool			m_AutoSwitchOff;
	protected 		bool			m_ShowSocketsInInventory;
	protected 		bool			m_HasElectricityIcon; // Electricity icon over the item in inventory
	protected 		bool			m_AutoSwitchOffWhenInCargo;
	protected 		bool			m_IsPlugged; // Synchronized variable
	protected 		bool			m_ConvertEnergyToQuantity;
	
	protected 		int				m_MySocketID = -1;
	protected 		int				m_PlugType;
	protected 		int				m_EnergySourceStorageIDb1; // Storage persistence ID
	protected 		int				m_EnergySourceStorageIDb2; // Storage persistence ID
	protected 		int				m_EnergySourceStorageIDb3; // Storage persistence ID
	protected 		int				m_EnergySourceStorageIDb4; // Storage persistence ID
	protected 		int				m_AttachmentActionType;
	protected 		int				m_EnergySourceNetworkIDLow = -1;  // Network ID
	protected 		int				m_EnergySourceNetworkIDHigh = -1; // Network ID
	
	protected 		float			m_EnergyUsage;
	protected 		float			m_Energy;
	protected 		float			m_EnergyStorageMax;
	protected 		float			m_ReduceMaxEnergyByDamageCoef;
	protected 		float			m_SocketsCount;
	protected 		float			m_CordLength;
	protected 		float			m_LastUpdateTime;
	protected 		float			m_WetnessExposure;
	protected 		float			m_UpdateInterval; // Interval of OnWork(...) calls and device updates.
	
	protected 		string			m_CordTextureFile;
	
	// Concatenated strings for p3d selections
	protected static const string	SOCKET_ 			= "socket_";
	protected static const string	_PLUGGED 			= "_plugged";
	protected static const string	_AVAILABLE 			= "_available";
	static const string				SEL_CORD_PLUGGED 	= "cord_plugged";
	static const string				SEL_CORD_FOLDED		= "cord_folded";
	
	protected ref TIntArray			m_CompatiblePlugTypes;
	EntityAI						m_EnergySource; // Energy source can be any EntityAI object
	ref array<EntityAI> 			m_PluggedDevices;
	ref map<string,EntityAI>		m_DeviceByPlugSelection;
	
	ref Timer 						m_UpdateTimer;
	ref Timer 						m_UpdateQuantityTimer;
	ref Timer 						m_DebugUpdate;
	
	const int MAX_SOCKETS_COUNT 	= 4;
	EntityAI m_Sockets[MAX_SOCKETS_COUNT];
	
	

	// Constructor
	void ComponentEnergyManager()
	{
		// Disable debug arrows on public release, so that they don't use their timers.
		#ifndef DEVELOPER
		m_DebugPlugs = false;
		#endif
	}
	
	void ~ComponentEnergyManager()
	{
		if (m_DebugPlugArrow)
		{
			m_DebugPlugArrow.Destroy();
			m_DebugPlugArrow = NULL; 
		}
	}

	// Initialization. Energy Manager is ready.
	override void Event_OnInit()
	{
		m_ThisEntityAI.m_EM = this;
		GetGame().GameScript.CallFunction(m_ThisEntityAI, "OnInitEnergy", NULL, 0);
	}
	
	// Update debug arrows
	void DebugUpdate()
	{
		if ( GetDebugPlugs() )
		{
			if ( GetGame().IsMultiplayer()  &&  GetGame().IsServer() )
			{
				if (m_DebugUpdate)
					m_DebugUpdate.Stop();
				
				return;
			}
		
			if (m_DebugPlugArrow)
			{
				m_DebugPlugArrow.Destroy();
				m_DebugPlugArrow = NULL; 
			}
			
			if ( GetEnergySource() )
			{
				vector from = GetEnergySource().GetPosition() + "0 0.1 0";
				vector to = m_ThisEntityAI.GetPosition() + "0 0.1 0";
				
				//No need to draw an arrow in this situation as it would not be visible
				if ( vector.DistanceSq(from, to) == 0 )
					return;
				
				if ( m_ThisEntityAI.GetType() == "BarbedWire" ) // Special case for debugging of electric fences. Temporal code until offsets in fences are fixed.
				{
					EntityAI BBB = m_ThisEntityAI.GetHierarchyParent();
					
					if ( BBB  &&  BBB.GetType() == "Fence" )
					{
						to = to + "0 -1.3 0";
					}
				}
				
				m_DebugPlugArrow = DrawArrow( from, to );
			}
		}
	}
	
	Shape DrawArrow(vector from, vector to, float size = 0.5, int color = 0xFFFFFFFF, float flags = 0)
	{
		vector dir = to - from;
		dir.Normalize();
		vector dir1 = dir * size;
		size = size * 0.5;
	
		vector dir2 = dir.Perpend() * size;
	
		vector pts[5];
		pts[0] = from;
		pts[1] = to;
		pts[2] = to - dir1 - dir2;
		pts[3] = to - dir1 + dir2;
		pts[4] = to;
		
		return Shape.CreateLines(color, flags, pts, 5);
	}

	EntityAI GetThisEntityAI()
	{
		return m_ThisEntityAI;
	}
	
	// Prepare everything
	override void Event_OnAwake()
	{
		string cfg_item = "CfgVehicles " + m_ThisEntityAI.GetType();
		string cfg_energy_manager = cfg_item + " EnergyManager ";
		
		// Read all config parameters
		m_EnergyUsage					= GetGame().ConfigGetFloat(cfg_energy_manager + "energyUsagePerSecond");
		bool switch_on					= GetGame().ConfigGetFloat(cfg_energy_manager + "switchOnAtSpawn");
		m_AutoSwitchOff					= GetGame().ConfigGetFloat(cfg_energy_manager + "autoSwitchOff");
		m_HasElectricityIcon			= GetGame().ConfigGetFloat(cfg_energy_manager + "hasIcon");
		m_AutoSwitchOffWhenInCargo 		= GetGame().ConfigGetFloat(cfg_energy_manager + "autoSwitchOffWhenInCargo");
		
		m_Energy						= GetGame().ConfigGetFloat(cfg_energy_manager + "energyAtSpawn");
		m_EnergyStorageMax				= GetGame().ConfigGetFloat(cfg_energy_manager + "energyStorageMax");
		m_ReduceMaxEnergyByDamageCoef	= GetGame().ConfigGetFloat(cfg_energy_manager + "reduceMaxEnergyByDamageCoef");
		m_SocketsCount					= GetGame().ConfigGetFloat(cfg_energy_manager + "powerSocketsCount");
		
		m_IsPassiveDevice				= GetGame().ConfigGetFloat(cfg_energy_manager + "isPassiveDevice");
		m_CordLength 					= GetGame().ConfigGetFloat(cfg_energy_manager + "cordLength");
		m_PlugType 						= GetGame().ConfigGetFloat(cfg_energy_manager + "plugType");
		
		m_AttachmentActionType			= GetGame().ConfigGetFloat(cfg_energy_manager + "attachmentAction");
		m_WetnessExposure				= GetGame().ConfigGetFloat(cfg_energy_manager + "wetnessExposure");
		
		float update_interval			= GetGame().ConfigGetFloat(cfg_energy_manager + "updateInterval");
		
		m_ConvertEnergyToQuantity		= GetGame().ConfigGetFloat(cfg_energy_manager + "convertEnergyToQuantity");
		
		
		// Check if energy->quantity converion is configured properly
		float cfg_max_quantity = GetGame().ConfigGetFloat (cfg_item + " varQuantityMax");
		
		if (m_ConvertEnergyToQuantity  &&  cfg_max_quantity <= 0)
		{
			string error = "Error! Item " + m_ThisEntityAI.GetType() + " has invalid configuration of the energy->quantity conversion feature. To fix this, add 'varQuantityMax' parameter with value higher than 0 to the item's config. Then make sure to re-build the PBO containing this item!";
			Error(error);
			m_ConvertEnergyToQuantity = false;
		}
		else
		{
			if (m_ConvertEnergyToQuantity)
			{
				if (!m_UpdateQuantityTimer)
					m_UpdateQuantityTimer = new Timer( CALL_CATEGORY_SYSTEM );
				
				m_UpdateQuantityTimer.Run( 0.3 , this, "OnEnergyAdded", NULL, false);
			}
		}
		
		// Set update interval
		if ( update_interval <= 0 )
			update_interval = DEFAULT_UPDATE_INTERVAL;
		
		SetUpdateInterval( update_interval );
		
		// If energyAtSpawn is present, then use its value for energyStorageMax if that cfg param is not present (for convenience's sake)
		string cfg_check_energy_limit = cfg_energy_manager + "energyStorageMax";
		
		if ( !GetGame().ConfigIsExisting (cfg_check_energy_limit)  &&  m_Energy > 0 )
		{
			m_EnergyStorageMax = m_Energy;
		}
		
		// Fill m_CompatiblePlugTypes
		string cfg_check_plug_types = cfg_energy_manager + "compatiblePlugTypes";
		
		if ( GetGame().ConfigIsExisting (cfg_check_plug_types) )
		{
			m_CompatiblePlugTypes = new TIntArray;
			GetGame().ConfigGetIntArray(cfg_check_plug_types, m_CompatiblePlugTypes);	
		}
		
		if (GetSocketsCount() > 0)
			m_PluggedDevices = new array<EntityAI>;
		
		if ( m_CordLength < 0 )
		{
			m_CordLength = 0;
			string error_message_cord = "Warning! " + m_ThisEntityAI.GetType() + ": config parameter 'cordLength' is less than 0! Cord length should not be negative!";
			DPrint(error_message_cord);
		}
		
		if (GetSocketsCount() > 0)
		{
			m_DeviceByPlugSelection = new map<string,EntityAI>;
			// Prepare the m_DeviceByPlugSelection
			string cfg_animation_sources = "cfgVehicles " + m_ThisEntityAI.GetType() + " " + "AnimationSources ";
			int animation_sources_count = GetGame().ConfigGetChildrenCount(cfg_animation_sources);
			
			for (int i_selection = 0; i_selection < animation_sources_count; i_selection++)
			{
				// TO DO: This could be optimized so not all selections on item are considered as plug/socket selections.
				string selection;
				GetGame().ConfigGetChildName(cfg_animation_sources, i_selection, selection);
				m_DeviceByPlugSelection.Set(selection, NULL);
			}
		}
		
		
		
		// Prepare sockets
		if ( m_SocketsCount > MAX_SOCKETS_COUNT )
		{
			m_SocketsCount = MAX_SOCKETS_COUNT;
			string error_message_sockets = "Error! " + m_ThisEntityAI.GetType() + ": config parameter 'powerSocketsCount' is higher than the current limit (" + MAX_SOCKETS_COUNT.ToString() + ")! Raise the limit (constant MAX_SOCKETS_COUNT) or decrease the powerSocketsCount parameter for this device!";
			DPrint(error_message_sockets);
		}
		
		m_Sockets[MAX_SOCKETS_COUNT]; // Handles selections for plugs in the sockets. Feel free to change the limit if needed.
		
		GetGame().ConfigGetText(cfg_energy_manager + "cordTextureFile", m_CordTextureFile); 
		
		if ( switch_on )
		{
			SwitchOn();
		}
		
		for ( int i = 0; i <= GetSocketsCount(); ++i )
		{
			m_ThisEntityAI.HideSelection ( SOCKET_ + i.ToString() + _PLUGGED );
		}
		
		// Show/hide inventory sockets
		m_ShowSocketsInInventory = false;
		if ( GetSocketsCount() > 0  &&  IsPlugCompatible(PLUG_COMMON_APPLIANCE)  &&  m_ThisEntityAI.GetType() != "MetalWire" ) // metal wire filter is hopefully temporal.
		{
			m_ShowSocketsInInventory = true;
		}
		
		m_CanWork = HasEnoughStoredEnergy();
		
		m_ThisEntityAI.HideSelection( SEL_CORD_PLUGGED );
		
		
		#ifdef DEVELOPER
		GetGame().m_EnergyManagerArray.Insert( this );
		#endif
	}

	// Returns the type of this component
	override int GetCompType()
	{
		return COMP_TYPE_ENERGY_MANAGER;
	}
	
	// When the object is deleted
	void OnDeviceDestroyed()
	{
		bool was_working = m_ThisEntityAI.GetCompEM().IsWorking();
		
		SwitchOff();
		UnplugAllDevices();
		UnplugThis();
		SetPowered( false );
		
		if ( was_working )
			m_ThisEntityAI.OnWorkStop();
		;
	}
	
	//Restart the debug timer when relogging
	void RefreshDebug()
	{
		if ( m_DebugPlugs )
		{
			if ( !m_DebugUpdate )
				m_DebugUpdate = new Timer( CALL_CATEGORY_SYSTEM );
			
			if ( !m_DebugUpdate.IsRunning() )
				m_DebugUpdate.Run(0.01, this, "DebugUpdate", NULL, true);
		}
		else
		{
			if ( m_DebugPlugArrow )
			{
				m_DebugPlugArrow.Destroy();
				m_DebugPlugArrow = NULL; 
			}
		}
	}
	
	bool GetDebugPlugs()
	{
		return m_DebugPlugs;
	}
	
	void SetDebugPlugs( bool newVal )
	{
		m_DebugPlugs = newVal;
		RefreshDebug();
	}

	//======================================================================================
	// 									 PUBLIC FUNCTIONS
	// 						Use these to control the Energy Manager
	// Functions are in order of their return value: void, bool, int, float, string, array.
	//======================================================================================

	//! Energy manager: Switches ON the device so it starts doing its work if it has enough energy.
	void SwitchOn()
	{
		m_IsSwichedOnPreviousState = m_IsSwichedOn;
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			if ( CanSwitchOn() )
			{
				m_IsSwichedOn = true;
				Synch();
				
				DeviceUpdate(); // 'Wake up' this device now
				StartUpdates();
				
				// 'Wakes up' all connected devices
				WakeUpWholeBranch( m_ThisEntityAI );
				
				UpdateCanWork();
				
				// Call event
				GetGame().GameScript.CallFunction(m_ThisEntityAI, "OnSwitchOn", NULL, 0);
			}
		}
		
		if ( !GetGame().IsServer()  &&  GetGame().IsMultiplayer()/*  &&  CanSwitchOn() */) // I want the CanSwitchOn() check, but when it's here, the OnSwitchOn() event is never called on client-side due to engine's synchronization system changing the m_IsSwichedOn to true without any specific event beign called. (Yes there is OnVariablesSynchronized() but that is called also when m_CanWork is synchronized, so I need to write a method of knowing when was this specific value changed.)
		{
			GetGame().GameScript.CallFunction(m_ThisEntityAI, "OnSwitchOn", NULL, 0);
		}
	}

	//! Energy manager: Switches OFF the device.
	void SwitchOff()
	{
		m_IsSwichedOnPreviousState = m_IsSwichedOn;
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			if ( CanSwitchOff() )
			{
				m_IsSwichedOn = false;
				Synch();
				
				if ( IsWorking() )
				{
					StopUpdates();
					DeviceUpdate();
				}
				
				// 'Wakes up' all connected devices
				WakeUpWholeBranch( m_ThisEntityAI );

				UpdateCanWork();
								
				// Call event
				GetGame().GameScript.CallFunction(m_ThisEntityAI, "OnSwitchOff", NULL, 0);
			}
		}
		
		if ( !GetGame().IsServer()  &&  GetGame().IsMultiplayer() )
		{
			m_IsSwichedOn = false;
			GetGame().GameScript.CallFunction(m_ThisEntityAI, "OnSwitchOff", NULL, 0);
		}
	}

	//! Energy manager: Changes the status of this device. When it's passive (true), the main timer and OnWork events are not used.
	void SetPassiveState(bool state = true)
	{
		m_IsPassiveDevice = state;
		if ( !m_IsPassiveDevice )
		{
			DeviceUpdate();
		}
	}

	//! Energy manager: Unplugs the given device from this one.
	void UnplugDevice(EntityAI device_to_unplug)
	{
		if ( GetGame() )
		{
			int index_start	= GetPluggedDevicesCount() - 1;
			bool device_found = false;
			
			for ( int i = index_start; i >= 0; --i )
			{
				EntityAI plugged_device = GetPluggedDevices().Get(i);
				
				if (plugged_device == device_to_unplug)
				{
					GetPluggedDevices().Remove(i);
					device_found = true;
					break;
				}
			}
			
			if (device_found)
			{
				int socket_ID = device_to_unplug.GetCompEM().GetMySocketID();
				UnplugCordFromSocket( socket_ID );
				device_to_unplug.GetCompEM().SetEnergySource(NULL);
				device_to_unplug.GetCompEM().DeviceUpdate();
				device_to_unplug.GetCompEM().StartUpdates();
				device_to_unplug.GetCompEM().WakeUpWholeBranch( m_ThisEntityAI );
				
				if (m_DebugPlugs  &&  m_DebugPlugArrow)
				{
					m_DebugPlugArrow.Destroy();
					m_DebugPlugArrow = NULL;
				}
				
				OnOwnSocketReleased(device_to_unplug);
				device_to_unplug.GetCompEM().OnIsUnplugged( m_ThisEntityAI );
				device_to_unplug.ShowSelection ( SEL_CORD_FOLDED );
				device_to_unplug.HideSelection ( SEL_CORD_PLUGGED );
			}
		}
	}

	//! Energy manager: Unplugs this device from its power source
	void UnplugThis()
	{
		if ( GetGame() )
		{
			if ( GetEnergySource() )
			{
				GetEnergySource().GetCompEM().UnplugDevice(m_ThisEntityAI);
			}
		}
	}
	
	//! Energy manager: Unplugs everything directly connected to this device
	void UnplugAllDevices()
	{
		if ( GetPluggedDevices() ) // This check is necesarry in case this function is called before initialization
		{
			int index_start	= GetPluggedDevicesCount() - 1;
			for ( int i = index_start; i >= 0; --i )
			{
				UnplugDevice( GetPluggedDevices().Get(i) );
			}
		}
	}
	
	// Used only for storing of the plug's state through server restart
	void RestorePlugState(bool state)
	{
		m_RestorePlugState = state;
	}

	//! Energy manager: Sets stored energy for this device. It ignores the min/max limit!
	void SetEnergy(float new_energy)
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer()) // Client can't change energy value.
		{
			float old_energy = m_Energy;
			m_Energy = new_energy;
			
			if ( old_energy - GetEnergyUsage() <= 0 || (old_energy != new_energy && Math.Min(old_energy,new_energy) <= 0) )
			{
				UpdateCanWork();
			}
		}
	}
	
	//! Energy manager: Sets stored energy for this device between 0 and MAX based on relative input value between 0 and 1
	void SetEnergy0To1(float energy01)
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer()) // Client can't change energy value.
		{
			SetEnergy( Math.Lerp(0, GetEnergyMax(),energy01));
		}
	}

	//! Energy manager: Shows/Hides all selections this system works with. Call this if something is wrong with selections (like during Init and Restore event in config)
	void UpdateSelections()
	{
		// Lets update sockets, if there are any
		int slots_c = GetSocketsCount();
		
		for ( int i = 0; i < slots_c; ++i )
		{
			EntityAI plug_owner = GetDeviceBySocketID(i);
			
			if ( plug_owner )
			{
				string plugged_selection = SOCKET_ + (i+1).ToString() + _PLUGGED;
				string available_selection = SOCKET_ + (i+1).ToString() + _AVAILABLE;
				m_ThisEntityAI.ShowSelection ( plugged_selection );
				m_ThisEntityAI.HideSelection ( available_selection );
				string texture_path = plug_owner.GetCompEM().GetCordTextureFile();
				int selection_index = m_ThisEntityAI.GetHiddenSelectionIndex( plugged_selection );
				m_ThisEntityAI.SetObjectTexture(selection_index, texture_path );
			}
			else
			{
				m_ThisEntityAI.ShowSelection ( SOCKET_ + (i+1).ToString() + _AVAILABLE );
				m_ThisEntityAI.HideSelection ( SOCKET_ + (i+1).ToString() + _PLUGGED );
			}
		}
		
		// Now lets update the cord/plug state
		if ( GetEnergySource() )
		{
			m_ThisEntityAI.ShowSelection ( SEL_CORD_PLUGGED );
			m_ThisEntityAI.HideSelection ( SEL_CORD_FOLDED );
		}
		else
		{
			m_ThisEntityAI.ShowSelection ( SEL_CORD_FOLDED );
			m_ThisEntityAI.HideSelection ( SEL_CORD_PLUGGED );
		}
	}

	//! Energy manager: Unplugs this device when it's necesarry
	void UpdatePlugState()
	{
		// Check if the item is held in hands during advanced placement
		if ( m_ThisEntityAI.GetHierarchyRootPlayer()  &&  m_ThisEntityAI.GetHierarchyRootPlayer().IsKindOf("SurvivorBase") )
		{
			// Yes it is in player's hands
			EntityAI player = m_ThisEntityAI.GetHierarchyRootPlayer();
			
			// Measure distance from the player
			vector pos_player = player.GetPosition();
			if ( ! IsEnergySourceAtReach( pos_player , 5 )  )
			{
				UnplugThis();
			}
		}
		else
		{
			// No it is not in player's hands.
			// Measure distance from the device
			vector pos_item = m_ThisEntityAI.GetPosition();
			
			if ( m_ThisEntityAI.GetHierarchyParent() )
			{
				pos_item = m_ThisEntityAI.GetHierarchyParent().GetPosition();
			}
			
			if ( ! IsEnergySourceAtReach( pos_item )  )
			{
				UnplugThis();
			}
		}
	}

	// Returns an array of plug types this device can accept
	void GetCompatiblePlugTypes(out TIntArray IDs)
	{
		IDs = m_CompatiblePlugTypes;
	}

	// Stores IDs of the energy source.
	void StoreEnergySourceIDs(int b1, int b2, int b3, int b4)
	{
		m_EnergySourceStorageIDb1 = b1;
		m_EnergySourceStorageIDb2 = b2;
		m_EnergySourceStorageIDb3 = b3;
		m_EnergySourceStorageIDb4 = b4;
	}
	
	//! Energy manager: Changes the maximum amount of energy this device can store (when pristine).
	void SetEnergyMaxPristine(float new_limit)
	{
		m_EnergyStorageMax = new_limit;
	}
	
	//! Energy manager: Changes the length of the virtual power cord.
	void SetCordLength( float new_length )
	{
		m_CordLength = new_length;
	}
	
	// Sets the plug type (for plug -> socket compatibility checks).
	void SetPlugType( int new_type )
	{
		m_PlugType = new_type;
	}
	
	// Sets the new attachment action type.
	void SetAttachmentAction( int new_action_type )
	{
		m_AttachmentActionType = new_action_type;
	}
	
	//! Energy manager: Changes the energy usage per second.
	void SetEnergyUsage( float new_usage )
	{
		m_EnergyUsage = new_usage;
	}
	
	//! Energy manager: Resets energy usage to default (config) value.
	void ResetEnergyUsage()
	{
		string cfg_energy_usage = "CfgVehicles " + m_ThisEntityAI.GetType() + " EnergyManager ";
		m_EnergyUsage = GetGame().ConfigGetFloat (cfg_energy_usage + "energyUsagePerSecond");
	}
	
	// Sets path to the cord texture file.
	void SetCordTextureFile( string new_path )
	{
		m_CordTextureFile = new_path;
	}
	
	// Sets energy source. Intended to be called only on client through RPC.
	void SetEnergySourceClient( EntityAI source )
	{	
		SetEnergySource(source);
	}
	
	//! Energy manager: Stores the device which is plugged into the given socket ID.
	void SetDeviceBySocketID(int id, EntityAI plugged_device)
	{
		m_Sockets[id] = plugged_device;
	}
	
		
	//! Energy manager: Sets visibility of the electricity icon (bolt).
	void SetElectricityIconVisibility( bool make_visible )
	{
		m_HasElectricityIcon = make_visible;
	}
	
	// Checks whenever this device can work or not and updates this information on all clients. Can be called many times per frame because synchronization happens only once if a change has occured.
	void UpdateCanWork()
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			bool current_state = CanWork();
			
			if (current_state != m_CanWork)
			{
				m_CanWork = current_state;
				Synch();
				
				if ( m_ThisEntityAI && m_ThisEntityAI.GetHierarchyParent() && m_ThisEntityAI.GetHierarchyParent().GetCompEM() )
				{
					m_ThisEntityAI.GetHierarchyParent().GetCompEM().UpdateCanWork();
				}
			}
		}
	}
	
	void HandleMoveInsideCargo(EntityAI container)
	{
		if ( m_AutoSwitchOffWhenInCargo )
		{
			if (IsSwitchedOn())
			{
				SwitchOff();
			}
		}
	}
	
	//! Energy manager: Sets the interval of the OnWork(...) calls. Changing this value does not change the rate of energy consumption.
	void SetUpdateInterval( float value )
	{
		m_UpdateInterval = value;
	}
	
	// Returns true if this device was plugged into something at the end of previous session
	bool GetRestorePlugState()
	{
		return m_RestorePlugState;
	}
	
	//! Energy manager: Attempts to plug this device into the energy_source. Returns true if the action was successfull, or false if not (due to plug incompatibility or no free socket on the receiver). The ID of the power socket is chosen automatically unless optional parameter socket_id is used (starting from 0). If the given ID is not free then a free socket is found.
	bool PlugThisInto(EntityAI energy_source, int socket_id = -1)
	{
		return energy_source.GetCompEM().PlugInDevice(m_ThisEntityAI, socket_id);
	}
	
	//! Energy manager: Checks if the device can be switched ON
	bool CanSwitchOn()
	{
		if ( !IsSwitchedOn() )
		{
			return true;
		}
		
		return false;
	}

	
	/**
	\brief Energy manager: Checks whenever this device can do work or not.
		\param test_energy \p float optional parameter will overwite the default energy consumption value of this device.
		\return \p bool Returns true if this device will work when it's switched on. Otherwise it returns false.
	*/
	bool CanWork( float test_energy = -1)
	{
		if ( GetGame().IsMultiplayer()  &&  GetGame().IsClient() )
		{
			return m_CanWork;
		}
		
		if (m_ThisEntityAI && m_ThisEntityAI.IsRuined())
		{
			return false;
		}
		
		// Check if the power source(s) (which can be serially connected) can provide needed energy.
		float 		energy_usage 	= test_energy;
		float 		gathered_energy = GetEnergy();
		EntityAI	energy_source 	= GetEnergySource();
		
		if (energy_usage == -1)
		{
			energy_usage = GetEnergyUsage();
		}
		
		if ( !CheckWetness() )
		{
			return false;
		}
		
		if (gathered_energy <= 0 && energy_usage <= 0) //empty power source
		{
			return false;
		}
		
		int cycle_limit = 500; // Sanity check to definitely avoid infinite cycles
		
		while ( gathered_energy < energy_usage ) // Look for energy source if we don't have enough stored energy
		{
			// Safetycheck!
			if (cycle_limit > 0)
			{
				cycle_limit--;
			}
			else
			{
				DPrint("Energy Manager ERROR: The 'cycle_limit' safety break had to be activated to prevent possible game freeze. Dumping debug information...");
				//Print(m_ThisEntityAI);
				//Print(this);
				//Print(energy_source);
				
				if (energy_source.GetCompEM())
				{
					//Print(energy_source.GetCompEM());
				}
				
				//Print(gathered_energy);
				//Print(energy_usage);
				
				//Print(m_ThisEntityAI.GetPosition());
				
				if (energy_source)
				{
					//Print(energy_source.GetPosition());
				}
				
				//Print("End of the 'cycle_limit' safety break ^ ");
				
				return false;
			}
			// ^ Safetycheck!
			
			if ( energy_source  &&  energy_source != m_ThisEntityAI  &&  !energy_source.IsRuined()  &&  energy_source.GetCompEM()  &&  energy_source.GetCompEM().IsSwitchedOn()  &&  energy_source.GetCompEM().CheckWetness() )
			{
				gathered_energy = gathered_energy + energy_source.GetCompEM().GetEnergy();
				energy_source = energy_source.GetCompEM().GetEnergySource();
			}
			else
			{
				// No power source, no energy.
				return false;
			}
		}
		
		// Enough energy was found
		return true;
	}

	//! Energy manager: Checks if this device is being stopped from working by its wetness level. Returns true when its wetness is not blocking it, false when its to owet to work.
	bool CheckWetness()
	{
		return (m_ThisEntityAI.GetWet() <= 1-m_WetnessExposure);
	}
	
	//! Energy manager: Checks if the device can be switched OFF
	bool CanSwitchOff()
	{
		if ( IsPassive() )
		{
			return false;
		}
		
		return IsSwitchedOn();
	}

	// Returns previous state of the switch.
	bool GetPreviousSwitchState()
	{
		return m_IsSwichedOnPreviousState;
	}
	
	//! Energy manager: Returns state of the switch. Whenever the device is working or not does not matter. Use IsWorking() to account for that as well.
	bool IsSwitchedOn()
	{
		return m_IsSwichedOn;
	}

	//! Energy manager: Returns true if the cord of this device is folded. Returns false if it's plugged.
	bool IsCordFolded()
	{
		if ( IsPlugged() )
			return false;
		
		return true;
	}
	
	//! Energy manager: Returns true if this device is set to be passive. False if otherwise.
	bool IsPassive()
	{
		return m_IsPassiveDevice;
	}

	//! Energy manager: Returns true if this device is plugged into some other device (even if they are OFF or ruined). Otherwise it returns false.
	bool IsPlugged()
	{
		return m_IsPlugged;
	}
	
	
	//! Energy manager: Consumes the given amount of energy. If there is not enough of stored energy in this device, then it tries to take it from its power source, if any exists. Returns true if the requested amount of energy was consumed. Otherwise it returns false.
	bool ConsumeEnergy(float amount)
	{
		return FindAndConsumeEnergy(m_ThisEntityAI, amount, true);
	}

	//! Energy manager: Returns true if this device is working right now.
	bool IsWorking()
	{
		return m_IsWorking;
	}

	//! Energy manager: Returns true if this device has enough of stored energy for its own use.
	bool HasEnoughStoredEnergy()
	{
		if ( GetEnergy() > GetEnergyUsage() )
		{
			return true;
		}
		
		return false;
	}

	//! Energy manager: Returns true if this device has any free socket to receive a plug. If optional parameter socket_id is provided then only that socket ID is checked.
	bool HasFreeSocket( int socket_id = -1 )
	{
		if (socket_id == -1)
		{
			int plugged_devices = GetPluggedDevicesCount();
			int plugged_devices_limit = GetSocketsCount();
			
			if ( plugged_devices < plugged_devices_limit )
			{
				return true;
			}
			
			return false;
		}
		else
		{
			EntityAI device = GetDeviceBySocketID(socket_id);
			
			if (device)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}

	//! Energy manager: Checks if the given plug is compatible with this device's socket. Used by CanReceivePlugFrom() method.
	bool IsPlugCompatible(int plug_ID)
	{
		if ( plug_ID == PLUG_UNDEFINED )
		{
			return true; // When plugType is undefined in config then make it compatible.
		}
		
		if ( m_CompatiblePlugTypes )
		{
			for ( int i = 0; i < m_CompatiblePlugTypes.Count(); i++ )
			{
				int plug_ID_to_Check = m_CompatiblePlugTypes.Get(i);
				
				if ( plug_ID_to_Check == plug_ID )
				{
					return true;
				}
			}
		}
		else
		{
			// Since the config parameter compatiblePlugTypes is not present, then accept all plugs for simplicity's sake
			return true;
		}
		
		return false;
	}
	
	//! Energy manager: Returns true if this device can receive power plug of the other device.
	bool CanReceivePlugFrom( EntityAI device_to_plug )
	{
		// The following conditions are broken down for the sake of easier reading/debugging.
		
		if ( HasFreeSocket()  &&  device_to_plug != m_ThisEntityAI)
		{
			if ( device_to_plug.GetCompEM().GetEnergySource() != m_ThisEntityAI)
			{	
				if ( IsPlugCompatible(device_to_plug.GetCompEM().GetPlugType()) )
				{
					if ( device_to_plug.GetCompEM().IsEnergySourceAtReach( device_to_plug.GetPosition(), 0, m_ThisEntityAI.GetPosition() ) )
					{
						return true;
					}
				}
			}
		}
		
		return false;
	}
	
	//! Energy manager: Returns true if this device can be plugged into the given energy source. Otherwise returns false.
	bool CanBePluggedInto( EntityAI potential_energy_provider )
	{
		return potential_energy_provider.GetCompEM().CanReceivePlugFrom( m_ThisEntityAI );
	}
	
	//! Energy manager: Returns true if the electricity icon (bolt) is supposed to be visible for this device. False if not.
	bool HasElectricityIcon()
	{
		return m_HasElectricityIcon;
	}
	
	//! Energy manager: Returns true if this item automatically converts its energy to quantity
	bool HasConversionOfEnergyToQuantity()
	{
		return m_ConvertEnergyToQuantity;
	}

	/**
	\brief Energy manager: Returns true if this device's virtual power cord can reach its energy source at the given position, depending on its cordLength config parameter. Otherwise returns false.
		\param from_position \p vector position from where the measurement will be taken
		\param add_tolerance \p float parameter will add to the cord's length in meters (optional)
		\return \p bool True if the power source will be at reach for the plug, or if config param cordLength is absent or 0. False if not, or is not plugged, or this device lacks Energy Manager component.
		@code
			vector position_player = GetGame().GetPlayer().GetPosition();
			
			if ( my_device.GetCompEM().IsEnergySourceAtReach( position_player )  )
			{
				Print("Power source is at reach!");
			}else{
				Print("Power source is NOT at reach!");
			}
		@endcode
	*/
	bool IsEnergySourceAtReach( vector from_position, float add_tolerance = 0, vector override_source_position = "-1 -1 -1" )
	{
		if ( !IsPlugged()  &&  override_source_position == "-1 -1 -1" )
		{
			return false;
		}
		
		if ( GetCordLength() == 0 ) // 0 is an exception, which means infinitely long cable.
		{
			return true;
		}
		
		vector source_pos;
		float distance;
		
		if ( override_source_position == "-1 -1 -1" )
		{
			EntityAI energy_source = GetEnergySource();
			
			if (!energy_source)
				return false;
			
			source_pos = energy_source.GetPosition();
			distance = vector.Distance( from_position, source_pos );
		}
		else
		{
			source_pos = override_source_position;
			distance = vector.Distance( from_position, source_pos );
		}
		
		if ( distance > GetCordLength() + add_tolerance )
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	
	bool HasVisibleSocketsInInventory()
	{
		return m_ShowSocketsInInventory;
	}
	
	//! Energy manager: Returns true if this selection is a plug that's plugged into this device. Otherwise returns false.
	bool IsSelectionAPlug(string selection_to_test )
	{
		if ( GetPluggedDevices() )
		{
			int socket_count = GetSocketsCount();
			
			for ( int i = socket_count; i >= 0; --i )
			{
				string real_selection = SOCKET_ + i.ToString() +_PLUGGED;
				
				if ( selection_to_test == real_selection)
				{
					return true;
				}
			}
		}
		
		return false;
	}
	
	
	
	
	//! Energy manager: Returns the count of power sockets (whenever used or not)
	int GetSocketsCount()
	{
		return m_SocketsCount;
	}

	//! Energy manager: Returns plug type. Check \DZ\data\basicDefines.hpp  OR  \Scripts\Classes\Component\_constants.h files for types of plugs
	int GetPlugType()
	{
		return m_PlugType;
	}

	// Returns the action ID which is supposed to be done upon receiving an attachment
	int GetAttachmentAction()
	{
		return m_AttachmentActionType;
	}

	// Returns persistent ID (block 1) of the energy source
	int GetEnergySourceStorageIDb1()
	{
		return m_EnergySourceStorageIDb1;
	}

	// Returns persistent ID (block 2) of the energy source
	int GetEnergySourceStorageIDb2()
	{
		return m_EnergySourceStorageIDb2;
	}
	
	// Returns persistent ID (block 3) of the energy source
	int GetEnergySourceStorageIDb3()
	{
		return m_EnergySourceStorageIDb3;
	}
	
	// Returns persistent ID (block 4) of the energy source
	int GetEnergySourceStorageIDb4()
	{
		return m_EnergySourceStorageIDb4;
	}
	
	// Returns network ID (low) of the energy source
	int GetEnergySourceNetworkIDLow()
	{
		return m_EnergySourceNetworkIDLow;
	}

	// Returns network ID (high) of the energy source
	int GetEnergySourceNetworkIDHigh()
	{
		return m_EnergySourceNetworkIDHigh;
	}
	
	//! Energy manager: Returns the number of devices plugged into this one.
	int GetPluggedDevicesCount()
	{
		if ( GetPluggedDevices() )
		{
			return GetPluggedDevices().Count();
		}
		
		return 0;
	}
	
	//! Energy manager: Returns % of stored energy this device has as integer (from 0 to 100)
	int GetEnergy0To100()
	{
		if ( m_EnergyStorageMax > 0 )
		{
			int coef = Math.Round( m_Energy / m_EnergyStorageMax * 100 );
			return coef;
		}
		
		return 0;
	}
	
	//! Energy manager: Returns % of stored energy this device has as float (from 0.0 to 1.0)
	float GetEnergy0To1()
	{
		if ( m_EnergyStorageMax > 0 )
		{
			return m_Energy / m_EnergyStorageMax;
		}
		
		return 0;
	}
	
	//! Energy manager: Returns the update interval of this device.
	float GetUpdateInterval()
	{
		#ifdef DIAG_DEVELOPER
		if (FeatureTimeAccel.GetFeatureTimeAccelEnabled(ETimeAccelCategories.ENERGY_CONSUMPTION) || (FeatureTimeAccel.GetFeatureTimeAccelEnabled(ETimeAccelCategories.ENERGY_RECHARGE)))
		{
			return 1;//when modifying time accel, we might want to see things happen when they should, instead of waiting for the next tick
		}
		#endif
		return m_UpdateInterval;
	}
	
	//! Returns wetness exposure value defined in config
	float GetWetnessExposure()
	{
		return m_WetnessExposure;
	}
	
	//! Energy manager: Returns the number of energy this device needs to run itself (See its config >> energyUsagePerSecond)
	float GetEnergyUsage()
	{
		#ifdef DIAG_DEVELOPER
		if (FeatureTimeAccel.GetFeatureTimeAccelEnabled(ETimeAccelCategories.ENERGY_CONSUMPTION))
		{
			float timeAccel = FeatureTimeAccel.GetFeatureTimeAccelValue();
			return m_EnergyUsage * timeAccel;
		}
		#endif
		return m_EnergyUsage;
	}

	//! Energy manager: Returns the amount of stored energy this device has
	float GetEnergy()
	{
		return m_Energy;
	}

	//! Energy manager: Adds energy to this device and clamps it within its min/max storage limits. Returns the amount of energy that was clamped. Negative value is supported, but you should still use ConsumeEnergy(...) for propper substraction of energy.
	float AddEnergy(float added_energy)
	{
		if (added_energy != 0)
		{
			#ifdef DIAG_DEVELOPER
			if (FeatureTimeAccel.GetFeatureTimeAccelEnabled(ETimeAccelCategories.ENERGY_RECHARGE))
			{
				float timeAccel = FeatureTimeAccel.GetFeatureTimeAccelValue();
				added_energy *= timeAccel;
			}
			#endif

			bool energy_was_added = (added_energy > 0);
			
			float energy_to_clamp = GetEnergy() + added_energy;
			float clamped_energy = Math.Clamp( energy_to_clamp, 0, GetEnergyMax() );
			SetEnergy(clamped_energy);
			StartUpdates();
			
			if (energy_was_added)
				OnEnergyAdded();
			else
				OnEnergyConsumed();
			
			return energy_to_clamp - clamped_energy;
		}
		
		return 0;
	}
	
	//! Energy manager: Returns the maximum amount of energy this device can curently store. If parameter 'reduceMaxEnergyByDamageCoef' is used in the config of this device then the returned value will be reduced by damage.
	float GetEnergyMax()
	{
		float max_health = 0;
		
		if ( m_ThisEntityAI.HasDamageSystem() )
			max_health = m_ThisEntityAI.GetMaxHealth("","");
		//else if ( m_ReduceMaxEnergyByDamageCoef != 0 )
		//	Error("[ERROR] ReduceMaxEnergyByDamageCoef is setup but " + m_ThisEntityAI.GetType() + " does not have a Damage System");
		
		if ( max_health == 0 || m_ReduceMaxEnergyByDamageCoef == 0 )
			return GetEnergyMaxPristine();
		
		float health = 100;
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer()) // TO DO: Remove this IF when method GetHealth can be called on client!
			health = m_ThisEntityAI.GetHealth("","");
		
		float damage_coef = 1 - (health / max_health);
		
		return GetEnergyMaxPristine() * (1 - ( damage_coef * m_ReduceMaxEnergyByDamageCoef ) );
	}
	
	//! Energy manager: Returns the maximum amount of energy this device can store. It's damage is NOT taken into account.
	float GetEnergyMaxPristine()
	{
		return m_EnergyStorageMax;
	}
	
	//! Energy manager: Returns the length of the virtual power cord.
	float GetCordLength()
	{
		return m_CordLength;
	}

	//! Energy manager: Returns the energy source this device is plugged into
	EntityAI GetEnergySource()
	{
		return m_EnergySource;
	}
	
	//! Energy manager: Returns the device which is plugged into the given socket ID.
	EntityAI GetDeviceBySocketID(int id)
	{
		return m_Sockets[id];
	}

	//! Energy manager: Returns the device to which the given plug selection belongs to
	EntityAI GetPlugOwner(string plug_selection_name)
	{
		if ( m_DeviceByPlugSelection.Contains(plug_selection_name) )
		{
			return m_DeviceByPlugSelection.Get(plug_selection_name);
		}
		
		return NULL;
	}
	
	//! Energy manager: Returns a device which is plugged into this one. If there are more devices to choose from then it returns the first one that is found.
	EntityAI GetPluggedDevice()
	{
		if ( GetPluggedDevicesCount() > 0 )
		{
			return GetPluggedDevices().Get(0);
		}
		
		return NULL;
	}
	
	//! Energy manager: Returns path to the cord texture file.
	string GetCordTextureFile()
	{
		return m_CordTextureFile;
	}

	//! Energy manager: Returns an array of devices which are plugged into this one
	array<EntityAI> GetPluggedDevices()
	{
		return m_PluggedDevices;
	}

	//! Energy manager: Returns an array of devices which are plugged into this one and are turned on
	array<EntityAI> GetPoweredDevices()
	{
		array<EntityAI> return_array = new array<EntityAI>;
		int plugged_devices_c = GetPluggedDevicesCount();
		for ( int i = 0; i < plugged_devices_c; ++i )
		{
			EntityAI device = GetPluggedDevices().Get(i);
			if ( IsSwitchedOn() )
			{
				return_array.Insert(device);
			}
		}
		
		return return_array;
	}
	
	
	/*===================================
				PUBLIC EVENTS
	===================================*/

	// Called every device update if its supposed to do some work. The update can be every second or at random, depending on its manipulation.
	void OnWork( float consumed_energy ) 
	{
		m_ThisEntityAI.OnWork(consumed_energy);
	}

	// Called when this device is plugged into some energy source
	void OnIsPlugged(EntityAI source_device) 
	{
		if (m_DebugPlugs)
		{
			if (!m_DebugUpdate)
				m_DebugUpdate = new Timer( CALL_CATEGORY_SYSTEM );
			
			if (!m_DebugUpdate.IsRunning())
				m_DebugUpdate.Run(0.01, this, "DebugUpdate", NULL, true);
		}
		
		UpdateCanWork();
		
		m_ThisEntityAI.OnIsPlugged(source_device);
	}

	// Called when this device is UNPLUGGED from the energy source
	void OnIsUnplugged( EntityAI last_energy_source ) 
	{
		UpdateCanWork();
		
		m_ThisEntityAI.OnIsUnplugged( last_energy_source );
	}

	// When something is plugged into this device
	void OnOwnSocketTaken( EntityAI device ) 
	{
		//play sound
		if ( device.GetCompEM().GetPlugType() == PLUG_COMMON_APPLIANCE && m_ThisEntityAI.IsInitialized() )
		{
			EffectSound sound_plug;
			m_ThisEntityAI.PlaySoundSet( sound_plug, "cablereel_plugin_SoundSet", 0, 0 );
		}
		
		m_ThisEntityAI.OnOwnSocketTaken(device);
	}

	// When something is UNPLUGGED from this device
	void OnOwnSocketReleased( EntityAI device ) 
	{
		//play sound
		if ( device.GetCompEM().GetPlugType() == PLUG_COMMON_APPLIANCE && m_ThisEntityAI.IsInitialized() )
		{
			EffectSound sound_unplug;
			m_ThisEntityAI.PlaySoundSet( sound_unplug, "cablereel_unplug_SoundSet", 0, 0 );
		}
				
		m_ThisEntityAI.OnOwnSocketReleased( device );
	}
	
	
	// Handles automatic attachment action
	void OnAttachmentAdded(EntityAI elec_device)
	{
		int attachment_action_type = GetAttachmentAction();
		
		if ( attachment_action_type == PLUG_THIS_INTO_ATTACHMENT )
		{
			if ( elec_device.GetCompEM().CanReceivePlugFrom( m_ThisEntityAI ) )
			{
				PlugThisInto(elec_device);
			}
		}
		else if ( attachment_action_type == PLUG_ATTACHMENTS_INTO_THIS )
		{
			elec_device.GetCompEM().PlugThisInto(m_ThisEntityAI);
		}
	}

	// Handles automatic detachment action
	void OnAttachmentRemoved(EntityAI elec_device)
	{
		int attachment_action_type = GetAttachmentAction();
		
		if ( attachment_action_type == PLUG_THIS_INTO_ATTACHMENT )
		{
			if ( elec_device == GetEnergySource() )
			{
				UnplugThis();
			}
		}
		else if ( attachment_action_type == PLUG_ATTACHMENTS_INTO_THIS )
		{
			elec_device.GetCompEM().UnplugThis();
		}
	}

	// Starts the device's main cycle
	void StartUpdates()
	{
		if ( !m_IsPassiveDevice )
		{
			if (!m_UpdateTimer)
				m_UpdateTimer = new Timer( CALL_CATEGORY_SYSTEM );
			
			if ( !m_UpdateTimer.IsRunning() ) // Makes sure the timer is NOT running already
			{
				m_UpdateTimer.Run( GetUpdateInterval() , this, "DeviceUpdate", NULL, true);
			}
		}
	}
	
	//! Energy manager: Called when energy was consumed on this device
	void OnEnergyConsumed()
	{
		m_ThisEntityAI.OnEnergyConsumed();
	}
	
	//! Energy manager: Called when energy was added on this device
	void OnEnergyAdded()
	{
		if (m_UpdateQuantityTimer)
		{
			m_UpdateQuantityTimer.Stop();
			m_UpdateQuantityTimer = NULL;
		}
		
		m_ThisEntityAI.OnEnergyAdded();
	}


	/*===================================
			 PROTECTED FUNCTIONS
	===================================*/

	// Stops the device's main cycle
	protected void StopUpdates()
	{
		if (m_UpdateTimer)
		{
			m_UpdateTimer.Stop();
			m_UpdateTimer = NULL; // Delete timer from memory
		}
	}

	// 'Wakes up' all devices down the network so they start working, if they have enough power, and are switched ON
	protected void WakeUpWholeBranch( EntityAI original_caller )
	{
		if ( GetSocketsCount() > 0 )
		{
			array<EntityAI> plugged_devices = GetPluggedDevices();
			int plugged_devices_c = plugged_devices.Count();
			
			for ( int i = 0; i < plugged_devices_c; ++i )
			{
				EntityAI device = plugged_devices.Get(i);
				if ( device != original_caller ) // original_caller check here prevents infinite loops
				{
					device.GetCompEM().UpdateCanWork();
					device.GetCompEM().DeviceUpdate();
					device.GetCompEM().StartUpdates();
					device.GetCompEM().WakeUpWholeBranch( original_caller );
				}
			}
		}
	}

	// Finds an available socket and plugs the given device into it.
	// This is mainly about visualisation.
	protected void PlugCordIntoSocket( EntityAI device_to_plug, int socket_id = -1 )
	{
		if (socket_id >= 0)
		{
			EntityAI plug_owner_by_socket = GetDeviceBySocketID(socket_id);
			
			if (!plug_owner_by_socket)
			{
				UpdateSocketSelections(socket_id, device_to_plug);
				return;
			}
		}

		int slots_c = GetSocketsCount();
		
		for ( int i = 0; i < slots_c; ++i )
		{
			EntityAI plug_owner = GetDeviceBySocketID(i);
			
			if ( !plug_owner ) // Check if this socket is available
			{
				UpdateSocketSelections(i, device_to_plug);
				break;
			}
		}
	}
	
	// Updates socket selections (plugged/unplugged) of the given ID and sets color texture of the plug.
	protected void UpdateSocketSelections(int socket_id, EntityAI device_to_plug)
	{
		SetDeviceBySocketID(socket_id, device_to_plug);
		
		string plugged_selection = SOCKET_ + (socket_id+1).ToString() + _PLUGGED;
		SetPlugOwner( plugged_selection, device_to_plug );
		m_ThisEntityAI.ShowSelection ( plugged_selection );
		
		string unplugged_selection = SOCKET_ + (socket_id+1).ToString() + _AVAILABLE;
		m_ThisEntityAI.HideSelection ( unplugged_selection );
		string texture_path = device_to_plug.GetCompEM().GetCordTextureFile();
		int selection_index = m_ThisEntityAI.GetHiddenSelectionIndex( plugged_selection );
		m_ThisEntityAI.SetObjectTexture( selection_index, texture_path );
		device_to_plug.GetCompEM().SetMySocketID(socket_id);
	}
	
	
	// Sets energy source for this device
	protected void SetEnergySource( EntityAI source )
	{	
		m_EnergySource = source;
		
		if (source)
		{
			m_IsPlugged = true;
			StartUpdates();
		}
		else
		{
			m_IsPlugged = false;
			m_EnergySourceNetworkIDLow = -1;
			m_EnergySourceNetworkIDHigh = -1;
		}
		
		if (m_EnergySource)
		{
			m_EnergySource.GetNetworkID( m_EnergySourceNetworkIDLow, m_EnergySourceNetworkIDHigh );
			
			//Print(m_EnergySource);
			//Print(m_EnergySourceNetworkIDLow);
			//Print(m_EnergySourceNetworkIDHigh);
		}
		
		Synch();
	}

	// Plugs the given device into this one
	protected bool PlugInDevice(EntityAI device_to_plug, int socket_id = -1)
	{
		if ( CanReceivePlugFrom(device_to_plug) )
		{
			if ( device_to_plug.GetCompEM().IsPlugged() )
				device_to_plug.GetCompEM().UnplugThis();
			
			GetPluggedDevices().Insert( device_to_plug );
			device_to_plug.GetCompEM().SetEnergySource(m_ThisEntityAI);
			
			PlugCordIntoSocket( device_to_plug, socket_id ); // Visualisation
			OnOwnSocketTaken( device_to_plug );
			
			device_to_plug.GetCompEM().OnIsPlugged(m_ThisEntityAI);
			WakeUpWholeBranch( m_ThisEntityAI );
			
			if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			{
				device_to_plug.HideSelection( SEL_CORD_FOLDED );
				device_to_plug.ShowSelection( SEL_CORD_PLUGGED );
			}
			
			return true;
		}
		
		return false;
	}

	// Sets the device to which the given plug selection belongs to
	protected void SetPlugOwner(string selection_name, EntityAI device)
	{
		if ( m_DeviceByPlugSelection.Contains(selection_name) )
		{
			m_DeviceByPlugSelection.Set(selection_name, device);
		}
	}
	
	// Frees the given socket.
	// This is only about visualisation.
	protected void UnplugCordFromSocket( int socket_to_unplug_ID )
	{
		EntityAI plug_owner = GetDeviceBySocketID(socket_to_unplug_ID);
		
		if ( plug_owner )
		{
			SetDeviceBySocketID(socket_to_unplug_ID, NULL);
			string unplugged_selection = SOCKET_ + (socket_to_unplug_ID+1).ToString() + _AVAILABLE;
			m_ThisEntityAI.ShowSelection ( unplugged_selection );
			
			string plugged_selection = SOCKET_ + (socket_to_unplug_ID+1).ToString() + _PLUGGED;
			m_ThisEntityAI.HideSelection ( plugged_selection );
			SetPlugOwner( plugged_selection, NULL );
			plug_owner.GetCompEM().SetMySocketID(-1);
		}
	}

	// Sets the state of the device
	protected void SetPowered( bool state )
	{
		m_IsWorking = state;
	}
	
	// Tries to consume the given amount of energy. If there is none in this device, then it tries to take it from some power source.
	protected bool FindAndConsumeEnergy(EntityAI original_caller, float amount, bool ignore_switch_state = false)
	{
		if ( (ignore_switch_state  ||  IsSwitchedOn())  &&  !m_ThisEntityAI.IsRuined() )
		{
			float available_energy = AddEnergy(-amount);
			
			if ( available_energy < 0  &&  IsPlugged() )
			{
				// This devices does not has enough of stored energy, therefore it will take it from its power source (which can be a chain of cable reels)
				EntityAI next_power_source = GetEnergySource();
				
				if (next_power_source  &&  next_power_source != original_caller) // Prevents infinite loop if the power source is the original caller itself
				{
					return next_power_source.GetCompEM().FindAndConsumeEnergy( original_caller, -available_energy );
				}
			}
			
			if ( available_energy >= 0)
			{
				return true;
			}
			
			return false;
		}
		else
		{
			return false;
		}
	}
	
	// Gets the socket ID this device is powered from.
	protected int GetMySocketID()
	{
		return m_MySocketID;
	}
	
	// Sets the socket ID this device is plugged into.
	protected void SetMySocketID( int slot_ID )
	{
		m_MySocketID = slot_ID;
	}

	void Synch()
	{
		m_ThisEntityAI.SetSynchDirty();
	}
	
	void ClearLastUpdateTime()
	{
		m_LastUpdateTime = 0;
	}
	
	void RememberLastUpdateTime()
	{
		m_LastUpdateTime = GetCurrentUpdateTime();
	}
	
	float GetCurrentUpdateTime()
	{
		return GetGame().GetTime();
	}
	
	// Updates the device's state of power. This function is visualized in the diagram at DayZ Confluence >> Camping & Squatting >> Electricity >> Energy Manager functionalities
	void DeviceUpdate()
	{
		//vector pos = m_ThisEntityAI.GetPosition();
		//string debug_message = "Object " + m_ThisEntityAI.GetType() + "  |  Energy: " + GetEnergy() + "  |  IsAtReach: " + (IsEnergySourceAtReach(pos)).ToString();
		//Print(debug_message);
		
		if ( !m_IsPassiveDevice )
		{
			// 'm_ThisEntityAI' and 'this' must be checked because this method is caled from a timer
			if ( m_ThisEntityAI  &&  this  &&  IsSwitchedOn()  &&  !m_ThisEntityAI.IsRuined()  &&  CheckWetness()  &&  m_CanWork  &&  !GetGame().IsMissionMainMenu() )
			{
				bool was_powered = IsWorking();
				float consumed_energy_coef;
				// Make sure to use only as much % of energy as needed since this function can be called at random.
				
				if ( m_LastUpdateTime == 0 )
				{
					RememberLastUpdateTime();
					consumed_energy_coef = 1.0;
				}
				else
				{
					float updatetime = GetCurrentUpdateTime();
					float time = updatetime - m_LastUpdateTime;
					consumed_energy_coef = time / 1000;
				}
				
				if (consumed_energy_coef > 0) // Prevents calling of OnWork events when no energy is consumed
				{
					m_LastUpdateTime = GetCurrentUpdateTime();
					float consume_energy = GetEnergyUsage() * consumed_energy_coef;
					bool has_consumed_enough = true;
					
					if (GetGame().IsServer() || !GetGame().IsMultiplayer()) // single player or server side multiplayer
						has_consumed_enough = ConsumeEnergy( consume_energy );
					
					SetPowered( has_consumed_enough );
					
					if ( has_consumed_enough )
					{
						if ( !was_powered )
						{
							m_CanStopWork = true;
							WakeUpWholeBranch(m_ThisEntityAI);
							GetGame().GameScript.CallFunction(m_ThisEntityAI, "OnWorkStart", NULL, 0);
							UpdateCanWork();
						}
						
						OnWork( consume_energy );
					}
					else
					{
						if ( was_powered )
						{
							if (m_CanStopWork)
							{
								m_CanStopWork = false;
								ClearLastUpdateTime();
								GetGame().GameScript.CallFunction(m_ThisEntityAI, "OnWorkStop", NULL, 0); // This event is called only once when the device STOPS being powered
								UpdateCanWork();
								
								if (m_AutoSwitchOff)
								{
									SwitchOff(); 
								}
							}
						}
						
						StopUpdates();
					}
				}
				else
				{
					ClearLastUpdateTime();
				}
			}
			else if (this  &&  m_ThisEntityAI)
			{
				SetPowered( false );
				StopUpdates();
				
				if (m_CanStopWork)
				{
					m_CanStopWork = false;
					ClearLastUpdateTime();
					GetGame().GameScript.CallFunction(m_ThisEntityAI, "OnWorkStop", NULL, 0); // This event is called only once when the device STOPS being powered
					UpdateCanWork();
					
					if (m_AutoSwitchOff)
					{
						SwitchOff(); 
					}
				}
			}
		}
	}
}