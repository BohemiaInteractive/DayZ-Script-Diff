enum ConstructionMaterialType
{
	MATERIAL_NONE	= 0,
	MATERIAL_LOG	= 1,
	MATERIAL_WOOD	= 2,
	MATERIAL_STAIRS	= 3,
	MATERIAL_METAL	= 4,
	MATERIAL_WIRE	= 5
}

class Construction
{
	static const float REPAIR_MATERIAL_PERCENTAGE = 0.15;
	static const float DECONSTURCT_MATERIAL_LOSS = 0.2;
	protected ref map<string, ref ConstructionPart> m_ConstructionParts;	//string - part name; int - 0-not constructed, 1-constructed
	protected BaseBuildingBase 	m_Parent;
	
	//Debug
	protected Shape m_CollisionBox;
	//Collision detectection
	protected ConstructionBoxTrigger m_ConstructionBoxTrigger;
	
	//============================================
	// Construction
	//============================================	
	void Construction( BaseBuildingBase parent )
	{
		m_ConstructionParts = new ref map<string, ref ConstructionPart>;
		
		//set parent object
		SetParent( parent );
	}
	
	void Init()
	{
		UpdateConstructionParts();
	}
		
	//parent
	protected BaseBuildingBase GetParent()
	{
		return m_Parent;
	}
	protected void SetParent( BaseBuildingBase parent )
	{
		m_Parent = parent;
	}
	//============================================
	// Construction process
	//============================================
	//constructed parts
	void AddToConstructedParts( string part_name )
	{
		ConstructionPart constrution_part = GetConstructionPart( part_name );
		
		if ( constrution_part )
		{
			bsbDebugPrint("[bsb] Construction " + Object.GetDebugName(m_Parent) + " AddToConstructedParts part=" + constrution_part.GetPartName());
			constrution_part.SetBuiltState( true );
		}
	}
	
	void RemoveFromConstructedParts( string part_name )
	{
		ConstructionPart constrution_part = GetConstructionPart( part_name );
		
		if ( constrution_part ) 
		{
			bsbDebugPrint("[bsb] Construction " + Object.GetDebugName(m_Parent) + " RemoveFromConstructedParts part=" + constrution_part.GetPartName());
			constrution_part.SetBuiltState( false );
		}
	}
	
	//BuildPart
	void BuildPartServer( notnull Man player, string part_name, int action_id )
	{
		bsbDebugPrint("[bsb] Construction BuildPartServer | " + part_name);
		//reset DamageZone health
		string damage_zone;
		if (DamageSystem.GetDamageZoneFromComponentName(GetParent(),part_name,damage_zone))
		{
			GetParent().SetAllowDamage(true);
			GetParent().SetHealthMax(damage_zone);
			GetParent().ProcessInvulnerabilityCheck(GetParent().GetInvulnerabilityTypeString());
		}
		
		//on action
		TakeMaterialsServer( part_name );

		//destroy build collision check trigger
		DestroyCollisionTrigger();

		//call event
		GetParent().OnPartBuiltServer( player, part_name, action_id );
	}
	
	//DismantlePart
	void DismantlePartServer( notnull Man player, string part_name, int action_id )
	{
		string damage_zone;
		DamageSystem.GetDamageZoneFromComponentName( GetParent(),part_name,damage_zone );
		
		bsbDebugPrint("[bsb] Construction DismantlePartServer | " + part_name);
		//receive materials
		ReceiveMaterialsServer( player, part_name, damage_zone );
			
		//drop non-usable materials
		DropNonUsableMaterialsServer( player, part_name );
		
		//call event
		GetParent().OnPartDismantledServer( player, part_name, action_id );
		
		//set DamageZone health to zero (redundant?)
		/*if ( GetParent().GetHealth(damage_zone,"Health") > 0 )
		{
			GetParent().SetHealth(damage_zone,"Health",0);
		}*/
	}
	
	//DestroyPart
	void DestroyPartServer( Man player, string part_name, int action_id, bool destroyed_by_connected_part = false )
	{
		bsbDebugPrint("[bsb] Construction DestroyPartServer | " + part_name);
		//destroy attached materials (if locked)
		DestroyMaterialsServer( player, part_name );
		
		//drop non-usable materials
		DropNonUsableMaterialsServer( player, part_name );			
		
		//call event
		GetParent().OnPartDestroyedServer( player, part_name, action_id, destroyed_by_connected_part );
		
		//set DamageZone health to zero (redundant?)
		string damage_zone;
		if ( DamageSystem.GetDamageZoneFromComponentName(GetParent(),part_name,damage_zone) && GetParent().GetHealth(damage_zone,"Health") > 0 )
		{
			GetParent().SetHealth(damage_zone,"Health",0);
		}
	}
	
	void DestroyConnectedParts(string part_name)
	{
		array<string> parts;// = new array<string>;
		parts = GetValidDepenentPartsArray(part_name);
		if (parts)
		{
			for (int i = 0; i < parts.Count(); i++)
			{
				bsbDebugPrint("[bsb] Construction DestroyConnectedParts | " + parts.Get(i));
				if (!ExceptionCheck(parts.Get(i)))
					DestroyPartServer(null,parts.Get(i),AT_DESTROY_PART,true);
			}
		}
	}
	
	//!Exceptions from 'dependent parts' hierarchy are handled here
	bool ExceptionCheck(string part_name)
	{
		//gate hack
		ConstructionPart part = GetConstructionPart(part_name);
		if( /*Fence.Cast(m_Parent) && */part.IsGate() )
		{
			if( GetConstructionPart("wall_base_down").IsBuilt() || GetConstructionPart("wall_base_up").IsBuilt() )
				return true;
		}
		return false;
	}
	
	//============================================
	// Update construction
	//============================================
	//update visual
	void InitVisuals()
	{
		bsbDebugPrint("[bsb] Construction " + Object.GetDebugName(m_Parent) + " InitVisuals");
		for ( int i = 0; i < m_ConstructionParts.Count(); ++i )
		{
			string key = m_ConstructionParts.GetKey( i );
			ConstructionPart value = m_ConstructionParts.Get( key );
		
			if ( value.IsBuilt() )
			{
				ShowConstructionPart( value.GetPartName() );		
			}
		}
	}

	void UpdateVisuals()
	{
		bsbDebugPrint("[bsb] Construction " + Object.GetDebugName(m_Parent) + " UpdateVisuals");
		for ( int i = 0; i < m_ConstructionParts.Count(); ++i )
		{
			string key = m_ConstructionParts.GetKey( i );
			ConstructionPart value = m_ConstructionParts.Get( key );
			if ( value.IsBuilt() )
			{
				ShowConstructionPart( value.GetPartName() );
			}
			else
			{
				HideConstructionPart( value.GetPartName() );
			}
		}
	}
	
	//update physics (only)
	void UpdatePhysics()
	{
		bsbDebugPrint("[bsb] Construction " + Object.GetDebugName(m_Parent) + " UpdatePhysics m_ConstructionParts=" + m_ConstructionParts.Count());
		for ( int i = 0; i < m_ConstructionParts.Count(); ++i )
		{
			string key = m_ConstructionParts.GetKey( i );
			ConstructionPart value = m_ConstructionParts.Get( key );
		
			if ( value.IsBuilt() )
			{
				bsbDebugPrint("[bsb] GetType=" + m_Parent.GetType() + " i=" + i + " ADD");
				ShowConstructionPartPhysics( value.GetPartName() );
			}
			else
			{
				bsbDebugPrint("[bsb] GetType=" + m_Parent.GetType() + " i=" + i + " RM");
				HideConstructionPartPhysics( value.GetPartName() );
			}
		}
	}
	
	void InitBaseState ()
	{
		bsbDebugPrint("[bsb] Construction " + Object.GetDebugName(m_Parent) + " InitBaseState");
		InitVisuals();
	}
	
	//update construction parts
	protected void UpdateConstructionParts()
	{
		string construction_path = "cfgVehicles" + " " + GetParent().GetType() + " " + "Construction";
		
		if ( GetGame().ConfigIsExisting( construction_path ) )
		{
			//main parts
			for ( int i = 0; i < GetGame().ConfigGetChildrenCount( construction_path ); ++i )
			{
				string main_part_name;
				GetGame().ConfigGetChildName( construction_path, i, main_part_name );
				string part_path = construction_path + " " + main_part_name;
				
				//parts
				for ( int j = 0; j < GetGame().ConfigGetChildrenCount( part_path ); ++j )
				{
					string part_name;
					GetGame().ConfigGetChildName( part_path, j, part_name );
					
					string name;
					GetGame().ConfigGetTextRaw( part_path + " " + part_name + " " + "name", name );							//name
					GetGame().FormatRawConfigStringKeys(name);
					bool show_on_init = GetGame().ConfigGetInt( part_path + " " + part_name + " " + "show_on_init" );		//show on init
					int id = GetGame().ConfigGetInt( part_path + " " + part_name + " " + "id" );							//part id
					bool is_base = GetGame().ConfigGetInt( part_path + " " + part_name + " " + "is_base" );					//is base (part)
					bool is_gate = GetGame().ConfigGetInt( part_path + " " + part_name + " " + "is_gate" );					//is gate (part)
					
					m_ConstructionParts.Insert( part_name, new ConstructionPart( name, part_name, main_part_name, id, show_on_init, is_base, is_gate, GetRequiredParts(part_name,main_part_name) ) );
					
					bsbDebugPrint("[bsb] Construction name=" + name + " part_name=" + part_name + " show=" + show_on_init + " base=" + is_base + " gate=" + is_gate);
				}
			}
		}
	}
	
	//============================================
	// Parts
	//============================================	
	map<string, ref ConstructionPart> GetConstructionParts()
	{
		return m_ConstructionParts;
	}
	
	ConstructionPart GetConstructionPart( string part_name )
	{
		return m_ConstructionParts.Get( part_name );
	}

	//CONSTRUCTION
	/*ConstructionPart GetConstructionPartToBuild( string part_name, ItemBase tool )
	{
		if ( CanBuildPart( part_name, tool ) )
		{
			return GetConstructionPart( part_name );
		}
		
		return NULL;
	}*/
	
	bool CanBuildPart( string part_name, ItemBase tool, bool use_tool )
	{
		if ( !IsPartConstructed( part_name ) && HasRequiredPart( part_name ) && !HasConflictPart( part_name ) && HasMaterials( part_name ) && (!use_tool || CanUseToolToBuildPart( part_name, tool )) && !MaterialIsRuined(part_name) )
		{
			return true;
		}
		
		return false;
	}
	
	bool MaterialIsRuined(string part_name)
	{
		string main_part_name = GetConstructionPart( part_name ).GetMainPartName();
		string cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + main_part_name + " " + part_name + " " + "Materials";
		
		if ( GetGame().ConfigIsExisting( cfg_path ) )
		{
			int	child_count = GetGame().ConfigGetChildrenCount( cfg_path );
			
			for ( int i = 0; i < child_count; i++ )
			{
				string child_name;
				GetGame().ConfigGetChildName( cfg_path, i, child_name );
				
				//get type, quantity from material
				string config_path;
				string slot_name;
				config_path = cfg_path + " " + child_name + " " + "slot_name";
				GetGame().ConfigGetText( config_path, slot_name );
				config_path = cfg_path + " " + child_name + " " + "quantity";
				float quantity = GetGame().ConfigGetFloat( config_path );
				config_path = cfg_path + " " + child_name + " " + "lockable";
				bool lockable = GetGame().ConfigGetInt( config_path );
				
				ItemBase attachment = ItemBase.Cast( GetParent().FindAttachmentBySlotName( slot_name ) );
				if (attachment.IsRuined())
					return true;
			}
		}
		return false;
	}
	
	//Get all construction parts that can be build (at that current time)
	void GetConstructionPartsToBuild( string main_part_name, out array<ConstructionPart> construction_parts, ItemBase tool, out string real_constructionTarget, bool use_tool )
	{
		construction_parts.Clear();
		string part_name;
		ConstructionPart value;
		string key;
		
		for ( int i = 0; i < m_ConstructionParts.Count(); ++i )
		{
			key = m_ConstructionParts.GetKey( i );
			value = m_ConstructionParts.Get( key );
		
			if ( main_part_name == value.GetMainPartName() && CanBuildPart( value.GetPartName(), tool, use_tool ) )
			{
				construction_parts.Insert( value );
			}
			
			if ( main_part_name == value.GetPartName() )
			{
				part_name = value.GetMainPartName();
			}
		}
		
		if( construction_parts.Count() == 0 && part_name )
		{
			for ( i = 0; i < m_ConstructionParts.Count(); ++i )
			{
				key = m_ConstructionParts.GetKey( i );
				value = m_ConstructionParts.Get( key );
		
				if ( part_name == value.GetMainPartName() && CanBuildPart( value.GetPartName(), tool, use_tool ) )
				{
					construction_parts.Insert( value );
				}
			}
		}
	}
	
	//Returns (first found) base construction part
	ConstructionPart GetBaseConstructionPart()
	{
		for ( int i = 0; i < m_ConstructionParts.Count(); ++i )
		{
			string key = m_ConstructionParts.GetKey( i );
			ConstructionPart value = m_ConstructionParts.Get( key );
		
			if ( value.IsBase() )
			{
				return value;
			}
		}
		
		return NULL;
	}
	
	//Returns (first found) gate construction part
	ConstructionPart GetGateConstructionPart()
	{
		for ( int i = 0; i < m_ConstructionParts.Count(); ++i )
		{
			string key = m_ConstructionParts.GetKey( i );
			ConstructionPart value = m_ConstructionParts.Get( key );
		
			if ( value.IsGate() )
			{
				return value;
			}
		}
		
		return NULL;
	}
	
	//checks if construction part has required part already built
	protected bool HasRequiredPart( string part_name )
	{
		string main_part_name = GetConstructionPart( part_name ).GetMainPartName();
		string cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + main_part_name + " " + part_name + " " + "required_parts";
		
		ref array<string> required_parts = new array<string>;
		GetGame().ConfigGetTextArray( cfg_path, required_parts );
		
		//check if parts are already built
		for ( int i = 0; i < required_parts.Count(); ++i )
		{
			if ( !IsPartConstructed( required_parts.Get( i ) ) )
			{
				return false;
			}
			//hack - gate
			/*else if (part_name == "wall_gate" && (IsPartConstructed("wall_base_down") || IsPartConstructed("wall_base_up")))
			{
				return true;
			}*/
		}
		
		return true;
	}
	
	//checks if there are conflict parts already built
	protected bool HasConflictPart( string part_name )
	{
		string main_part_name = GetConstructionPart( part_name ).GetMainPartName();
		string cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + main_part_name + " " + part_name + " " + "conflicted_parts";
		ref array<string> conflict_parts = new array<string>;
		GetGame().ConfigGetTextArray( cfg_path, conflict_parts );

		//check if parts are already built
		for ( int i = 0; i < conflict_parts.Count(); i++ )
		{
			if ( IsPartConstructed( conflict_parts.Get( i ) ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	//DECONSTRUCTION
	ConstructionPart GetConstructionPartToDismantle( string part_name, ItemBase tool )
	{
		if ( CanDismantlePart( part_name, tool ) )
		{
			return GetConstructionPart( part_name );
		}
		
		return NULL;
	}

	bool CanDismantlePart( string part_name, ItemBase tool )
	{
		if ( IsPartConstructed( part_name ) && !HasDependentPart( part_name ) && CanUseToolToDismantlePart( part_name, tool ) )
		{
			return true;
		}
		
		return false;
	}
	
	//checks if construction part has dependent part (that is already built) because of which it cannot be deconstruct
	//TODO return whole array of dependent parts/dependencies (one or the other), should be used to eventually destroy all dependent parts instead
	bool HasDependentPart( string part_name )
	{
		for ( int i = 0; i < m_ConstructionParts.Count(); ++i )
		{
			string key = m_ConstructionParts.GetKey( i );
			ConstructionPart construction_part = m_ConstructionParts.Get( key );
			
			if ( construction_part.IsBuilt() )
			{
				if ( construction_part.GetRequiredParts().Find( part_name ) > -1 )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	
	//returns array of BUILT parts that directly depend on 'part_name'
	protected array<string> GetValidDepenentPartsArray( string part_name, array<string> recurs = null )
	{
		string name;
		string cfg_path;
		ref array<string> dependent_parts;
		
		for ( int i = 0; i < m_ConstructionParts.Count(); ++i )
		{
			name = m_ConstructionParts.GetKey( i );
			ConstructionPart construction_part = m_ConstructionParts.Get( name );
			
			if ( construction_part.IsBuilt() && construction_part.GetRequiredParts() && construction_part.GetRequiredParts().Find( part_name ) > -1 ) //does the construction part need 'part_name' to exist?
			{
				if ( !dependent_parts )
				{
					dependent_parts = new array<string>;
				}
				
				if ( !recurs || (recurs.Find(name) == -1 ) )
				{
					dependent_parts.Insert(name);
				}
//				Print("part #" + i + ": " + name);
			}
		}
		
		//fully recursive search, disconnected (unnescessary)
		/*if (dependent_parts)
		{
			if ( dependent_parts.Count() > 0 )
			{
				ref array<string> temp = new array<string>;
				for ( i = 0; i < dependent_parts.Count(); i++ )
				{
					temp = GetValidDepenentPartsArray(dependent_parts.Get(i),dependent_parts);
					if (temp.Count() > 0)
					{
						dependent_parts.InsertAll(temp);
					}
				}
			}
			Print("dependent_parts.Count(): " + dependent_parts.Count());
		}*/
		return dependent_parts;
	}
	
	//gets all required parts of a construction part; fills into ConstructionPart on init
	array<string> GetRequiredParts( string part_name, string main_part_name )
	{
		string cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " " + "Construction" + " " + main_part_name + " " + part_name + " " + "required_parts";
		ref array<string> required_parts = new array<string>;
		GetGame().ConfigGetTextArray( cfg_path, required_parts );
		
		return required_parts;
	}
	
	//DESTROY
	ConstructionPart GetConstructionPartToDestroy( string part_name )
	{
		if ( CanDestroyPart( part_name ) )
		{
			return GetConstructionPart( part_name );
		}
		
		return NULL;
	}	

	bool CanDestroyPart( string part_name )
	{
		if ( IsPartConstructed( part_name ) && !HasDependentPart( part_name ) )
		{
			return true;
		}
		
		return false;
	}	
	
	//CONSTRUCTION PART STATE
	//show/hide construction part
	protected void ShowConstructionPart( string part_name )
	{
		bsbDebugPrint("[bsb] Construction ShowConstructionPart - " + part_name);
		GetParent().SetAnimationPhase( part_name, 0 );
	}
	
	protected void HideConstructionPart( string part_name )
	{
		bsbDebugPrint("[bsb] Construction HideConstructionPart - " + part_name);
		GetParent().SetAnimationPhase( part_name, 1 );
	}
	
	//show/hide physics
	void ShowConstructionPartPhysics( string part_name )
	{
		GetParent().AddProxyPhysics( part_name );
	}
	
	void HideConstructionPartPhysics( string part_name )
	{
		GetParent().RemoveProxyPhysics( part_name );
	}	
	
	//is part constructed
	bool IsPartConstructed( string part_name )
	{
		ConstructionPart construction_part = GetConstructionPart( part_name );
		if ( construction_part && construction_part.IsBuilt() )
		{
			return true;
		}
		
		return false;
	}
	
	//============================================
	// Materials for construction
	//============================================
	//has materials
	bool HasMaterials( string part_name, bool repairing = false )
	{
		string main_part_name = GetConstructionPart( part_name ).GetMainPartName();
		string cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + main_part_name + " " + part_name + " " + "Materials";
		
		if ( GetGame().ConfigIsExisting( cfg_path ) )
		{
			int	child_count = GetGame().ConfigGetChildrenCount( cfg_path );
			
			for ( int i = 0; i < child_count; i++ )
			{
				string child_name;
				GetGame().ConfigGetChildName( cfg_path, i, child_name );
				
				//get type, quantity from material
				string material_path;
				string slot_name;
				float quantity;
				material_path = cfg_path + " " + child_name + " " + "slot_name";
				GetGame().ConfigGetText( material_path, slot_name );
				material_path = cfg_path + " " + child_name + " " + "quantity";
				quantity = GetGame().ConfigGetFloat( material_path );
				
				if (repairing)
				{
					quantity *= REPAIR_MATERIAL_PERCENTAGE;
					quantity = Math.Max(Math.Floor(quantity),1);
				}
				
				//if the selected material (or its quantity) is not available
				if ( !HasMaterialWithQuantityAttached( slot_name, quantity ) )
				{
					return false;
				}
			}
		}
		
		return true;	//return true even if no material required
	}
	
	//check if parent object has attachment of required quantity attached to it
	protected bool HasMaterialWithQuantityAttached( string slot_name, float quantity )
	{
		ItemBase attachment = ItemBase.Cast( GetParent().FindAttachmentBySlotName( slot_name ) );
		
		if ( attachment && attachment.GetQuantity() >= quantity )
		{
			return true;
		}
		
		return false;
	}
	
	//take materials when building
	void TakeMaterialsServer( string part_name, bool repairing = false )
	{
		string main_part_name = GetConstructionPart( part_name ).GetMainPartName();
		string cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + main_part_name + " " + part_name + " " + "Materials";
		
		if ( GetGame().ConfigIsExisting( cfg_path ) )
		{
			int	child_count = GetGame().ConfigGetChildrenCount( cfg_path );
			
			for ( int i = 0; i < child_count; i++ )
			{
				string child_name;
				GetGame().ConfigGetChildName( cfg_path, i, child_name );
				
				//get type, quantity from material
				string config_path;
				string slot_name;
				config_path = cfg_path + " " + child_name + " " + "slot_name";
				GetGame().ConfigGetText( config_path, slot_name );
				config_path = cfg_path + " " + child_name + " " + "quantity";
				float quantity = GetGame().ConfigGetFloat( config_path );
				config_path = cfg_path + " " + child_name + " " + "lockable";
				bool lockable = GetGame().ConfigGetInt( config_path );
				
				ItemBase attachment = ItemBase.Cast( GetParent().FindAttachmentBySlotName( slot_name ) );
				if ( lockable )
				{
					//lock attachment
					InventoryLocation inventory_location = new InventoryLocation;
					attachment.GetInventory().GetCurrentInventoryLocation( inventory_location );
			
					GetParent().GetInventory().SetSlotLock( inventory_location.GetSlot(), true );
				}
				else
				{
					if ( quantity > -1 )						//0 - ignores quantity
					{
						if (repairing)
						{
							quantity *= REPAIR_MATERIAL_PERCENTAGE;
							quantity = Math.Max(Math.Floor(quantity),1);
						}
						//subtract quantity
						attachment.AddQuantity( -quantity );
					}
					else										//-1 - deletes the object
					{
						GetGame().ObjectDelete( attachment );
					}
				}
			}
		}
	}
	
	//receive materials when dismantling
	protected void ReceiveMaterialsServer( notnull Man player, string part_name, string damagezone_name )
	{
		ConstructionPart construction_part = GetConstructionPart( part_name );
		bool is_base = construction_part.IsBase();
		string main_part_name = construction_part.GetMainPartName();
		string cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + main_part_name + " " + part_name + " " + "Materials";
		
		if ( GetGame().ConfigIsExisting( cfg_path ) )
		{
			StaticConstructionMethods.SpawnConstructionMaterialPiles(GetParent(),player,cfg_path,part_name,damagezone_name,is_base);
		}
	}
	
	//destroy lockable materials when destroying
	protected void DestroyMaterialsServer( Man player, string part_name )
	{
		ConstructionPart cPart = GetConstructionPart( part_name );
		string main_part_name = cPart.GetMainPartName();
		string cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + main_part_name + " " + part_name + " " + "Materials";
		
		if ( GetGame().ConfigIsExisting( cfg_path ) )
		{
			int	child_count = GetGame().ConfigGetChildrenCount( cfg_path );
			
			for ( int i = 0; i < child_count; i++ )
			{
				string child_name;
				GetGame().ConfigGetChildName( cfg_path, i, child_name );
				
				//get type, quantity from material
				string config_path;
				string type;
				string slot_name;
				config_path = cfg_path + " " + child_name + " " + "type";
				GetGame().ConfigGetText( config_path, type );
				config_path = cfg_path + " " + child_name + " " + "slot_name";
				GetGame().ConfigGetText( config_path, slot_name );
				config_path = cfg_path + " " + child_name + " " + "quantity";
				float quantity = GetGame().ConfigGetFloat( config_path );
				config_path = cfg_path + " " + child_name + " " + "lockable";
				bool lockable = GetGame().ConfigGetInt( config_path );
				
				//get material
				ItemBase attachment = ItemBase.Cast( GetParent().FindAttachmentBySlotName( slot_name ) );
				
				//material still attached
				if ( lockable )			//if lockable 
				{
					if ( attachment )
					{
						InventoryLocation inventory_location = new InventoryLocation;
						attachment.GetInventory().GetCurrentInventoryLocation( inventory_location );
						bsbDebugPrint("[bsb] " + Object.GetDebugName(GetParent()) + " DestroyMaterialsServer unlock slot=" + inventory_location.GetSlot());
						
						GetParent().GetInventory().SetSlotLock( inventory_location.GetSlot() , false );
						GetGame().ObjectDelete( attachment );		//delete object
					}
				}
			}
		}
	}
		
	void DropNonUsableMaterialsServer( Man player, string part_name )
	{
		ConstructionPart construction_part = GetConstructionPart( part_name );
		
		string cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " " + "Construction" + " " + construction_part.GetMainPartName() + " " + construction_part.GetPartName() + " " + "platform_support";
		string platform_support;
		
		if ( GetGame().ConfigIsExisting( cfg_path ) )
		{		
			GetGame().ConfigGetText( cfg_path, platform_support );
		}
		
		if ( platform_support.Length() > 0 || construction_part.IsBase() )
		{
			string at_cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "GUIInventoryAttachmentsProps";
			
			if ( GetGame().ConfigIsExisting( at_cfg_path ) )
			{
				int	child_count = GetGame().ConfigGetChildrenCount( at_cfg_path );
				
				for ( int i = 0; i < child_count; i++ )
				{
					string child_name;
					GetGame().ConfigGetChildName( at_cfg_path, i, child_name );
					child_name.ToLower();
					
					if ( child_name.Contains( platform_support ) )
					{
						ref array<string> attachment_slots = new array<string>;
						GetGame().ConfigGetTextArray( at_cfg_path + " " + child_name + " " + "attachmentSlots", attachment_slots );
						
						for ( int j = 0; j < attachment_slots.Count(); ++j )
						{
							//get material
							ItemBase attachment = ItemBase.Cast( GetParent().FindAttachmentBySlotName( attachment_slots.Get( j ) ) );
							
							//material still attached
							if ( attachment )
							{
								InventoryLocation inventory_location = new InventoryLocation;
								attachment.GetInventory().GetCurrentInventoryLocation( inventory_location );
								bsbDebugPrint("[bsb] " + Object.GetDebugName(GetParent()) + " DropNonUsableMaterials UNlocking slot=" + inventory_location.GetSlot());
								
								//unlock slot
								GetParent().GetInventory().SetSlotLock( inventory_location.GetSlot() , false );
								
								EntityAI parent = GetParent();
								if (!parent)
									parent = player;
								
								int quantity_max = attachment.GetTargetQuantityMax(-1);
								InventoryLocation dst = new InventoryLocation;
								vector mat[4];
								attachment.GetTransform(mat);
								
								if ( parent.MemoryPointExists("" + part_name + "_materials") )
								{
									vector destination = parent.GetMemoryPointPos("" + part_name + "_materials");
									destination = GetGame().ObjectModelToWorld(parent,destination);
									float health = attachment.GetHealth("","Health");
									float quantity = attachment.GetQuantity() - 1;
									if (quantity < 1.0)
										quantity = 1.0;
									float dir[4];
									inventory_location.GetDir(dir);
									dst.SetGroundEx(attachment,destination,dir);
									//Print(dst.DumpToString());
									MiscGameplayFunctions.CreateItemBasePiles(attachment.GetType(),destination,quantity,health,true);
									attachment.AddQuantity( -quantity );
								}
								else
								{
									dst.SetGround(attachment,mat);
									
									for ( int k = attachment.GetQuantity(); k > quantity_max;  )
									{
										Object o = parent.GetInventory().LocationCreateEntity( dst, attachment.GetType(), ECE_PLACE_ON_SURFACE, RF_DEFAULT );
										ItemBase new_item = ItemBase.Cast( o );
										
										if( new_item )
										{
											MiscGameplayFunctions.TransferItemProperties( attachment, new_item );
											attachment.AddQuantity( -quantity_max );
											new_item.SetQuantity( quantity_max );
										}
										k -= quantity_max;
									}
								}
								
								//drop
								if (attachment.GetQuantity() > 0)
								{
									if ( GetGame().IsMultiplayer() )
									{
										parent.ServerTakeToDst( inventory_location, dst );
									}
									else
									{
										parent.LocalTakeToDst( inventory_location, dst );
									}
								}
								else
								{
									attachment.Delete();
								}
							}
						}
					}
				}
			}
		}
	}
	
	//set lock on materials that are attached and cannot be locked/unlocked
	void SetLockOnAttachedMaterials( string part_name, bool lock_slot )
	{
		string main_part_name = GetConstructionPart( part_name ).GetMainPartName();
		string cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + main_part_name + " " + part_name + " " + "Materials";
		
		if ( GetGame().ConfigIsExisting( cfg_path ) )
		{
			int	child_count = GetGame().ConfigGetChildrenCount( cfg_path );
			
			for ( int i = 0; i < child_count; i++ )
			{
				string child_name;
				GetGame().ConfigGetChildName( cfg_path, i, child_name );
				
				//get type, quantity from material
				string config_path;
				string type;
				string slot_name;
				config_path = cfg_path + " " + child_name + " " + "type";
				GetGame().ConfigGetText( config_path, type );
				config_path = cfg_path + " " + child_name + " " + "slot_name";
				GetGame().ConfigGetText( config_path, slot_name );
				config_path = cfg_path + " " + child_name + " " + "quantity";
				float quantity = GetGame().ConfigGetFloat( config_path );
				config_path = cfg_path + " " + child_name + " " + "lockable";
				bool lockable = GetGame().ConfigGetInt( config_path );
				
				//get material
				ItemBase attachment = ItemBase.Cast( GetParent().FindAttachmentBySlotName( slot_name ) );
				
				//material still attached
				if ( lockable )			//if lockable 
				{
					if ( attachment )
					{
						InventoryLocation inventory_location = new InventoryLocation;
						attachment.GetInventory().GetCurrentInventoryLocation( inventory_location );
						bsbDebugPrint("[bsb] " + Object.GetDebugName(GetParent()) + " SetLockOnAttachedMaterials lock=" + lock_slot +" slot=" + inventory_location.GetSlot());
						GetParent().GetInventory().SetSlotLock( inventory_location.GetSlot(), lock_slot );
					}
				}
			}
		}
	}
	
	//============================================
	// Construction tools
	//============================================	
	bool CanUseToolToBuildPart( string part_name, ItemBase tool )
	{
		ConstructionPart construction_part = GetConstructionPart( part_name );
		string part_cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + construction_part.GetMainPartName() + " " + construction_part.GetPartName() + " " + "build_action_type";
		if ( GetGame().ConfigIsExisting( part_cfg_path ) )
		{
			int part_build_action_type = GetGame().ConfigGetInt( part_cfg_path );
			string tool_cfg_path = "cfgVehicles" + " " + tool.GetType() + " " + "build_action_type";
			
			if ( GetGame().ConfigIsExisting( tool_cfg_path ) )
			{
				int tool_build_action_type = GetGame().ConfigGetInt( tool_cfg_path );
  
				if ( ( part_build_action_type & tool_build_action_type ) > 0 )
				{
					return true;
				}				
			}
		}
		
		return false;
	}
	
	bool CanUseToolToDismantlePart( string part_name, ItemBase tool )
	{
		ConstructionPart construction_part = GetConstructionPart( part_name );
		string part_cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + construction_part.GetMainPartName() + " " + construction_part.GetPartName() + " " + "dismantle_action_type";
		if ( GetGame().ConfigIsExisting( part_cfg_path ) )
		{
			int part_dismantle_action_type = GetGame().ConfigGetInt( part_cfg_path );
			string tool_cfg_path = "cfgVehicles" + " " + tool.GetType() + " " + "dismantle_action_type";
			
			if ( GetGame().ConfigIsExisting( tool_cfg_path ) )
			{
				int tool_dismantle_action_type = GetGame().ConfigGetInt( tool_cfg_path );
  
				if ( ( part_dismantle_action_type & tool_dismantle_action_type ) > 0 )
				{
					return true;
				}				
			}
		}
		
		return false;
	}
	
	ConstructionMaterialType GetMaterialType( string part_name )
	{
		ConstructionPart construction_part = GetConstructionPart( part_name );
		string part_cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + construction_part.GetMainPartName() + " " + construction_part.GetPartName() + " " + "material_type";
		if ( GetGame().ConfigIsExisting( part_cfg_path ) )
		{
			return GetGame().ConfigGetInt( part_cfg_path );
		}
		
		return ConstructionMaterialType.MATERIAL_NONE;
	}
	
	//============================================
	// Collision check
	//============================================
	//Collisions (BBox and Trigger); deprecated
	bool IsColliding( string part_name )
	{
		if (CfgGameplayHandler.GetDisableIsCollidingCheck())
			return false;
		ConstructionPart construction_part = GetConstructionPart( part_name );
		
		if ( construction_part )
		{
			vector center;
			float absolute_ofset = 0.05; 	//we need to lift BBox even more, because it colliddes with house floors due to various reasons (probably geometry or float imperfections)
			vector edge_length;
			vector min_max[2];				//data used for creating trigger
			ref array<Object> excluded_objects = new array<Object>;
			ref array<Object> collided_objects = new array<Object>;
			
			excluded_objects.Insert( GetParent() );
			
			//get min_max and center from config and memory points
			GetCollisionBoxData( part_name, min_max );
			
			center = GetBoxCenter( min_max );
			center = GetParent().ModelToWorld( center );		//convert to world coordinates
			edge_length = GetCollisionBoxSize( min_max );
			
			//Create trigger
			//CreateCollisionTrigger( part_name, min_max, center );
			
			//check collision on box trigger and collision box
			//IsTrigger colliding was turned off (for now) for easier way to build something with other players around
			if ( /* IsTriggerColliding() || */ GetGame().IsBoxCollidingGeometry( Vector( center[0], center[1] + absolute_ofset, center[2] ), GetParent().GetOrientation(), edge_length, ObjIntersectView, ObjIntersectGeom, excluded_objects, collided_objects ) )
			{
				//Debug
//				DrawDebugCollisionBox( min_max, ARGB( 150, 255, 0, 0 ) );
				//
				for (int i = 0; i < collided_objects.Count(); i++)
				{
					//Print(collided_objects.Get(i).GetType());
					EntityAI entity = EntityAI.Cast(collided_objects.Get(i));
					if ( entity && !entity.IsIgnoredByConstruction() )
						return true;
				}
			}
			//Debug
//			DrawDebugCollisionBox( min_max, ARGB( 150, 255, 255, 255 ) );
		}
		return false;
	}
	
	//! Collision check for building part
	bool IsCollidingEx( CollisionCheckData check_data )
	{
		if (CfgGameplayHandler.GetDisableIsCollidingCheck())
			return false;
		ConstructionPart construction_part = GetConstructionPart( check_data.m_PartName );
		
		if ( construction_part )
		{
			vector center;
			float absolute_ofset = 0.05; 	//we need to lift BBox even more, because it colliddes with house floors due to various reasons (probably geometry or float imperfections)
			vector edge_length;
			vector min_max[2];				//data used for creating trigger
			ref array<Object> excluded_objects = new array<Object>;
			ref array<Object> collided_objects = new array<Object>;
			
			excluded_objects.Insert( GetParent() );
			if (check_data.m_AdditionalExcludes.Count() > 0)
			{
				excluded_objects.InsertAll(check_data.m_AdditionalExcludes);
			}
			
			GetCollisionBoxData( check_data.m_PartName, min_max );
			center = GetBoxCenter( min_max );
			center = GetParent().ModelToWorld( center );		//convert to world coordinates
			edge_length = GetCollisionBoxSize( min_max );
			
			if ( GetGame().IsBoxCollidingGeometry( Vector( center[0], center[1] + absolute_ofset, center[2] ), GetParent().GetOrientation(), edge_length, check_data.m_PrimaryGeometry, check_data.m_SecondaryGeometry, excluded_objects, collided_objects ) )
			{
				//Debug
				//DrawDebugCollisionBox( min_max, ARGB( 150, 255, 0, 0 ) );
				for (int i = 0; i < collided_objects.Count(); i++)
				{
					EntityAI entity = EntityAI.Cast(collided_objects.Get(i));
					if ( entity && !entity.IsIgnoredByConstruction() )
						return true;
				}
			}
			//Debug
			//DrawDebugCollisionBox( min_max, ARGB( 150, 255, 255, 255 ) );
		}
		return false;
	}

	vector GetCollisionBoxSize( vector min_max[2] )
	{
		vector box_size = Vector( 0, 0, 0 );
		
		box_size[0] = Math.AbsFloat( min_max[1][0] - min_max[0][0] );
		box_size[1] = Math.AbsFloat( min_max[1][1] - min_max[0][1] );
		box_size[2] = Math.AbsFloat( min_max[1][2] - min_max[0][2] );
		
		return box_size;
	}
	
	//returns collision box data from construction config and model p3d
	protected void GetCollisionBoxData( string part_name, out vector min_max[2] )
	{
		string main_part_name = GetConstructionPart( part_name ).GetMainPartName();
		string cfg_path = "cfgVehicles" + " " + GetParent().GetType() + " "+ "Construction" + " " + main_part_name + " " + part_name + " " + "collision_data";
		ref array<string> collision_data = new array<string>;
		GetGame().ConfigGetTextArray( cfg_path, collision_data );
		
		if ( collision_data.Count() > 0 )
		{
			if ( GetParent().MemoryPointExists( collision_data[0] ) )
			{
				min_max[0] = GetParent().GetMemoryPointPos( collision_data[0] );
			}
			if ( GetParent().MemoryPointExists( collision_data[1] ) )
			{
				min_max[1] = GetParent().GetMemoryPointPos( collision_data[1] );
			}
		}
	}
	
	//returns center point of box defined by min/max values
	vector GetBoxCenter( vector min_max[2] )
	{
		vector center;
		
		center[0] = ( min_max[1][0] - min_max[0][0] ) / 2;
		center[1] = ( min_max[1][1] - min_max[0][1] ) / 2;
		center[2] = ( min_max[1][2] - min_max[0][2] ) / 2;
		center = Vector( min_max[1][0] - center[0], min_max[1][1] - center[1], min_max[1][2] - center[2] ); //offset to box center
		
		return center;
	}
	
	void GetTriggerExtents( vector min_max[2], out vector extents[2] )
	{
		vector egde_length = GetCollisionBoxSize( min_max );
		extents[0][0] = -egde_length[0] / 2;		//min
		extents[0][1] = -egde_length[1] / 2;
		extents[0][2] = -egde_length[2] / 2;
		extents[1][0] = egde_length[0] / 2;			//max
		extents[1][1] = egde_length[1] / 2;
		extents[1][2] = egde_length[2] / 2;
	}
	
	//Debug
	protected void DrawDebugCollisionBox( vector min_max[2], int color )
	{
		DestroyDebugCollisionBox();
		
		vector mat[4];
		GetParent().GetTransform( mat );
		
		m_CollisionBox = Debug.DrawBox( min_max[0], min_max[1], color );
		m_CollisionBox.SetMatrix( mat );
	}
	
	protected void DestroyDebugCollisionBox()
	{
		if ( m_CollisionBox )
		{
			m_CollisionBox.Destroy();
			m_CollisionBox = NULL;
		}		
	}
	
	void CreateCollisionTrigger( string part_name, vector min_max[2], vector center )
	{
		if ( m_ConstructionBoxTrigger )
		{
			if ( m_ConstructionBoxTrigger.GetPartName() == part_name )		//already created
			{
				return;
			}
			else
			{
				DestroyCollisionTrigger();
			}
		}	
		
		//get proper trigger extents (min<max)
		vector extents[2];
		GetTriggerExtents( min_max, extents );
		
		//create trigger
		m_ConstructionBoxTrigger = ConstructionBoxTrigger.Cast( GetGame().CreateObject( "ConstructionBoxTrigger", center, false, false, false ) );
		m_ConstructionBoxTrigger.SetPosition( center );
		m_ConstructionBoxTrigger.SetOrientation( GetParent().GetOrientation() );	
		m_ConstructionBoxTrigger.SetExtents( extents[0], extents[1] );
		
		m_ConstructionBoxTrigger.SetPartName( part_name );
	}
	//
	
	void DestroyCollisionTrigger()
	{
		GetGame().ObjectDelete( m_ConstructionBoxTrigger );
		m_ConstructionBoxTrigger = NULL;
	}
	
	bool IsTriggerColliding()
	{
		return m_ConstructionBoxTrigger.IsColliding();
	}
}

class StaticConstructionMethods
{
	//! spawns material from any construction; 'player' parameter optional
	static void SpawnConstructionMaterialPiles(notnull EntityAI entity, Man player, string cfg_path, string main_part_name, string damagezone_name = "", bool is_base = false )
	{
		int	child_count = GetGame().ConfigGetChildrenCount( cfg_path );
		
		for ( int i = 0; i < child_count; i++ )
		{
			string child_name;
			GetGame().ConfigGetChildName( cfg_path, i, child_name );
			
			//get type, quantity from material
			string config_path;
			string type;
			string slot_name;
			config_path = cfg_path + " " + child_name + " " + "type";
			GetGame().ConfigGetText( config_path, type );
			config_path = cfg_path + " " + child_name + " " + "slot_name";
			GetGame().ConfigGetText( config_path, slot_name );
			config_path = cfg_path + " " + child_name + " " + "quantity";
			float quantity = GetGame().ConfigGetFloat( config_path );
			config_path = cfg_path + " " + child_name + " " + "lockable";
			bool lockable = GetGame().ConfigGetInt( config_path );
			
			//receive material quantity
			ItemBase attachment = ItemBase.Cast( entity.FindAttachmentBySlotName( slot_name ) );
			int slot_id;
			
			//material still attached
			if ( lockable )			//if lockable 
			{
				if ( attachment )
				{
					InventoryLocation src = new InventoryLocation;
					attachment.GetInventory().GetCurrentInventoryLocation( src );
					bsbDebugPrint("[bsb] " + Object.GetDebugName( entity) + " DropNonUsableMaterials UNlocking slot=" + src.GetSlot() );
					entity.GetInventory().SetSlotLock( src.GetSlot() , false );
					
					//detach if base
					if ( is_base )
					{
						if ( GetGame().IsMultiplayer() && player )
						{
							InventoryLocation dst = new InventoryLocation;
							GameInventory.SetGroundPosByOwner( player, src.GetItem(), dst );
							player.ServerTakeToDst( src, dst );
						}
						else
						{
							entity.GetInventory().DropEntity( InventoryMode.PREDICTIVE, entity, attachment );
						}
					}
				}
			}
			else
			{
				float pile_health;
				float qty_coef;
				vector destination = entity.GetPosition();
				//placed on helper memory point, if available
				if ( entity.MemoryPointExists("" + main_part_name + "_materials") )
				{
					destination = entity.GetMemoryPointPos("" + main_part_name + "_materials");
					destination = GetGame().ObjectModelToWorld(entity,destination);
				}
				else if ( entity.MemoryPointExists(main_part_name) )
				{
					destination = entity.GetMemoryPointPos(main_part_name);
					destination = GetGame().ObjectModelToWorld(entity,destination);
				}
				//pile_health = GameConstants.DAMAGE_WORN_VALUE * MiscGameplayFunctions.GetTypeMaxGlobalHealth(type);
				pile_health = entity.GetHealth01(damagezone_name,"Health") * MiscGameplayFunctions.GetTypeMaxGlobalHealth(type);
				qty_coef =  1 - (entity.GetHealthLevel(damagezone_name) * Construction.DECONSTURCT_MATERIAL_LOSS) - Construction.DECONSTURCT_MATERIAL_LOSS;
				quantity *= qty_coef;
				quantity = Math.Max(Math.Floor(quantity),1);
				MiscGameplayFunctions.CreateItemBasePiles(type,destination,quantity,pile_health,true);
			}
		}
	}
}

//! Data structure for passing parameters (extendable, modable)
class CollisionCheckData
{
	ref array<Object> m_AdditionalExcludes;
	string m_PartName;
	int m_PrimaryGeometry;
	int m_SecondaryGeometry;
	
	void CollisionCheckData()
	{
		m_AdditionalExcludes = new array<Object>;
		m_PartName = "";
		m_PrimaryGeometry = ObjIntersectGeom;
		m_SecondaryGeometry = ObjIntersectView;
	}
}

class ConstructionBoxTrigger : ManTrigger
{
	string m_PartName;
	
	void SetPartName( string part_name )
	{
		m_PartName = part_name;
	}
	
	string GetPartName()
	{
		return m_PartName;
	}
	
	override protected void UpdateInsiders( int timeout )
	{
		super.UpdateInsiders( 20 );
	}
	
	bool IsColliding()
	{
		if ( GetInsiders().Count() > 0 )
		{
			return true;
		}
		
		return false;
	}
}
