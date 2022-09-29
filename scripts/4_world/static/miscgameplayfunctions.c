class TurnItemIntoItemLambda extends ReplaceItemWithNewLambda
{
	bool m_TransferAgents;
	bool m_TransferVariables;
	bool m_TransferHealth;
	bool m_ExcludeQuantity;
	float m_quantity_override;
	
	void TurnItemIntoItemLambda (EntityAI old_item, string new_item_type, PlayerBase player)
	{
		SetTransferParams();
	}
	
	void SetTransferParams (bool transfer_agents = true, bool transfer_variables = true, bool transfer_health = true, bool exclude_quantity = false, float quantity_override = -1)
	{
		m_TransferAgents = transfer_agents;
		m_TransferVariables = transfer_variables; 
		m_TransferHealth = transfer_health;
		m_ExcludeQuantity = exclude_quantity;
		m_quantity_override = quantity_override;
	}

	override void CopyOldPropertiesToNew (notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);

		if (new_item) 
		{
			MiscGameplayFunctions.TransferItemProperties(old_item, new_item, m_TransferAgents, m_TransferVariables, m_TransferHealth, m_ExcludeQuantity);
			MiscGameplayFunctions.TransferInventory(old_item, new_item, m_Player);
			
			//quantity override
			if (ItemBase.Cast(new_item) && m_quantity_override != -1)
			{
				m_quantity_override = Math.Max(m_quantity_override,0);
				ItemBase.Cast(new_item).SetQuantity(m_quantity_override);
			}
		}
		else
		{
			Debug.LogError("TurnItemIntoItemLambda: failed to create new item","static");
		}
	}
	
	//! if attaching from att.slot to att.slot, skips the change to "ground" version. Allows for correct property transfers.
	override void VerifyItemTypeBySlotType ()
	{
		if (m_NewLocation.GetType() == InventoryLocationType.ATTACHMENT && m_OldItem.ConfigIsExisting("ChangeIntoOnAttach"))
		{
			string str;
			int idx = -1;
			TStringArray inventory_slots = new TStringArray;
			TIntArray inventory_slots_idx = new TIntArray;
			TStringArray attach_types = new TStringArray;
			
			m_OldItem.ConfigGetTextArray("ChangeInventorySlot",inventory_slots);
			if (inventory_slots.Count() < 1) //is string
			{
				inventory_slots_idx.Insert(InventorySlots.GetSlotIdFromString(m_OldItem.ConfigGetString("ChangeInventorySlot")));
				attach_types.Insert(m_OldItem.ConfigGetString("ChangeIntoOnAttach"));
			}
			else //is array
			{
				inventory_slots_idx.Clear();
				for (int i = 0; i < inventory_slots.Count(); i++)
				{
					inventory_slots_idx.Insert(InventorySlots.GetSlotIdFromString(inventory_slots.Get(i)));
				}
				m_OldItem.ConfigGetTextArray("ChangeIntoOnAttach",attach_types);
			}
		
			idx = m_NewLocation.GetSlot();
			str = attach_types.Get(inventory_slots_idx.Find(idx));
			if (str != "")
			{
				m_NewItemType = str;
			}
		}
	}
	
	override void OnSuccess (EntityAI new_item)
	{
		super.OnSuccess(new_item);
		if( m_Player )
		{
			m_Player.GetItemAccessor().OnItemInHandsChanged();
		}
	}
};

class TurnItemIntoItemLambdaAnimSysNotifyLambda extends TurnItemIntoItemLambda
{
	override void OnSuccess (EntityAI new_item)
	{
		super.OnSuccess(new_item);
		if( m_Player )
		{
			m_Player.GetItemAccessor().OnItemInHandsChanged();
		}
	}
}

class TurnItemIntoItemLambdaRestrainLambda extends TurnItemIntoItemLambdaAnimSysNotifyLambda
{
	override void OnSuccess (EntityAI new_item)
	{
		super.OnSuccess(new_item);
		m_Player.SetRestrained(true);
	}
}

/**@class		DropEquipAndDestroyRootLambda
 * @brief		this one is a bit special: it drops all items and destroys the ex-root of the hierarchy
 **/
class DropEquipAndDestroyRootLambda : ReplaceItemWithNewLambdaBase
{
	PlayerBase m_Player;

	void DropEquipAndDestroyRootLambda (EntityAI old_item, string new_item_type, PlayerBase player)
	{
		m_Player = player;
	}
	
	override void CopyOldPropertiesToNew (notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);
		
		InventoryLocation understash_src = m_NewLocation; // m_NewLocation is a backup of original old_item's src before the operation started
		
		array<EntityAI> children = new array<EntityAI>;
		old_item.GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, children);
		int count = children.Count();
		for (int i = 0; i < count; i++)
		{
			EntityAI child = children.Get(i);
			if (child)
			{
				InventoryLocation child_src = new InventoryLocation;
				child.GetInventory().GetCurrentInventoryLocation(child_src);
				
				InventoryLocation child_dst = new InventoryLocation;
				//@TODO: modify _dst with place on gnd?
				
				vector m4[4];
				Math3D.MatrixIdentity4(m4);
				
				GameInventory.PrepareDropEntityPos(old_item, child, m4, false);
				
				child_dst.SetGround(child,m4);
				
				m_Player.LocalTakeToDst(child_src, child_dst);
												
				GetGame().RemoteObjectTreeCreate(child); // this forces server to send CreateVehicle Message to client. This is needed for preserving the appearance of network operations on client (so that DeleteObject(old) arrives before CreateVehicle(new)). @NOTE: this does not delete the object on server, only it's network representation.
			}
		}
	}
}

/**@class		MoveEquipToExistingItemAndDestroyOldRootLambda
 * @brief		this one is a also bit special: it moves all items to already existing item and destroys the ex-root of the hierarchy
 **/
class MoveEquipToExistingItemAndDestroyOldRootLambda : ReplaceItemWithNewLambdaBase
{
	PlayerBase m_Player;
	EntityAI m_NewItem;

	void MoveEquipToExistingItemAndDestroyOldRootLambda (EntityAI old_item, string new_item_type, PlayerBase player, EntityAI new_item)
	{
		m_Player = player;
		m_NewItem = new_item;
		if (new_item_type != string.Empty)
			Error("MoveEquipAndDestroyRootLambda expects new_item_type to be empty");
	}
	
	override protected void RemoveNetworkObjectInfo ()
	{
		super.RemoveNetworkObjectInfo();
		GetGame().RemoteObjectTreeDelete(m_NewItem);
	}
	override protected void UndoRemoveNetworkObjectInfo ()
	{
		super.UndoRemoveNetworkObjectInfo();
		GetGame().RemoteObjectTreeCreate(m_NewItem);
	}
	
	override void CopyOldPropertiesToNew (notnull EntityAI old_item, EntityAI new_item)
	{
		// @NOTE: new_item is NULL, this lambda does not create new entity
		super.CopyOldPropertiesToNew(old_item, new_item);
		
		MiscGameplayFunctions.TransferInventory(old_item, m_NewItem, m_Player);		
	}
	
	override protected void CreateNetworkObjectInfo (EntityAI new_item)
	{
		super.CreateNetworkObjectInfo(new_item);
		GetGame().RemoteObjectTreeCreate(m_NewItem);
	}
};

enum TransferInventoryResult
{
	Ok, DroppedSome
};

enum ThrowEntityFlags
{
	NONE = 0,
	SPLIT = 1, //< Splits the item when it has quantity, recommended to use when called on an attachment
}

class MiscGameplayFunctions
{
	//! truncate float to specified precision
	static float Truncate(float value, int decimals = 2)
	{
		int multiplier = Math.Pow(10,decimals);
		return Math.Clamp(Math.Floor(value * multiplier),float.LOWEST, float.MAX) / multiplier;
	}
	//! truncate float to specified precision, output as string
	static string TruncateToS(float value, int decimals = 2)
	{
		return Truncate(value, decimals).ToString();
	}
	//! truncate float to specified precision
	static vector TruncateVec(vector value, int decimals = 2)
	{
		int multiplier = Math.Pow(10,decimals);
		float v1 = Math.Clamp(Math.Floor(value[0] * multiplier),float.LOWEST, float.MAX) / multiplier;
		float v2 = Math.Clamp(Math.Floor(value[1] * multiplier),float.LOWEST, float.MAX) / multiplier;
		float v3 = Math.Clamp(Math.Floor(value[2] * multiplier),float.LOWEST, float.MAX) / multiplier;
		return Vector(v1,v2,v3);
	}
	
	static string TruncateVecToS(vector value,int decimals = 2, string delimiter = " ")
	{
		return MiscGameplayFunctions.TruncateToS(value[0],decimals) + delimiter + MiscGameplayFunctions.TruncateToS(value[1],decimals) +delimiter + MiscGameplayFunctions.TruncateToS(value[2],decimals));
	}
	
	static string GetColorString(float r, float g, float b, float a)
	{
		return string.Format("#(argb,8,8,3)color(%1,CO)", string.Format("%1,%2,%3,%4", r, g, b, a));
	}
	
	//! Produces ACII "progress bar" based on an 0..1 'value' input
	static string ValueToBar(float value, string bar = "[----------]", string mark = "x")
	{
		int length = bar.Length() - 2;
		float index = Math.Lerp(0,length, value);
		index = Math.Round(index);
		index = Math.Clamp(index,0,length);
		
		return InsertAtPos(bar,mark,index);
	}
	
	//! Insert 'insert' behind index 'pos' of the 'base' string
	static string InsertAtPos(string base, string insert, int pos)
	{
		int length_first = pos+1;
		int length_base = base.Length();
		int length_second = length_base - length_first;
		string first = base.Substring(0,length_first);
		string second = base.Substring(pos+1,length_second);
		return first + insert + second;
	}
	
	//! will transform item' variables, agents and other local scripted properties as well as any relevant non-scripted properties like health
	static void TransferItemProperties(EntityAI source, notnull EntityAI target, bool transfer_agents = true, bool transfer_variables = true, bool transfer_health = true, bool exclude_quantity = false)
	{
		ItemBase target_ib = ItemBase.Cast(target);
		ItemBase source_ib = ItemBase.Cast(source);
		
		if( transfer_agents ) target_ib.TransferAgents( source.GetAgents() );
		
		if( transfer_variables )
		{
			MiscGameplayFunctions.TransferItemVariables(source_ib, target_ib, exclude_quantity);
		}

		if( !GetGame().IsClient())
		{
			//if( transfer_health ) target_ib.SetHealth("", "", source.GetHealth("",""));
			target_ib.SetHealth("", "", source.GetHealth01("","") * target_ib.GetMaxHealth("",""));
		}
	}

	static void TransferItemVariables(ItemBase source, ItemBase target, bool exclude_quantity = false)
	{
		target.TransferVariablesFloat( source.GetVariablesFloat() );
		//target.TransferVariablesString( source.GetVariablesString() );
		if ( exclude_quantity )
		{
			target.RemoveItemVariable(VARIABLE_QUANTITY);
		}
		else if ( source.IsMagazine() && target.IsMagazine() )
		{
			Magazine source_mag = Magazine.Cast(source);
			Magazine target_mag = Magazine.Cast(target);
			
			target_mag.ServerSetAmmoCount( source_mag.GetAmmoCount() );
		}
		
		if ( source.IsWeapon() && target.IsWeapon() )
		{
			Weapon_Base source_wpn = Weapon_Base.Cast(source);
			Weapon_Base target_wpn = Weapon_Base.Cast(target);
			
			target_wpn.CopyWeaponStateFrom(source_wpn);
		}
		else if ( source.HasEnergyManager() && target.HasEnergyManager() )
		{
			ComponentEnergyManager ems = source.GetCompEM();
			ComponentEnergyManager emt = target.GetCompEM();
			
			emt.SetEnergy(ems.GetEnergy());

			if (ems.IsSwitchedOn())
				emt.SwitchOn();
		}
		else if ( source.CanDecay() && target.CanDecay() )
		{
			Edible_Base source_edb = Edible_Base.Cast(source);
			Edible_Base target_edb = Edible_Base.Cast(target);
			
			target_edb.TransferFoodStage(source_edb);
		}
	}

	static TransferInventoryResult TransferInventory( EntityAI sourceItem, EntityAI targetItem, PlayerBase player)
	{
		TransferInventoryResult result = TransferInventoryResult.Ok;

		array<EntityAI> children = new array<EntityAI>;
		sourceItem.GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, children);
		int count = children.Count();
		for (int i = 0; i < count; i++)
		{
			EntityAI child = children.Get(i);
			if (child)
			{
				InventoryLocation child_src = new InventoryLocation;
				child.GetInventory().GetCurrentInventoryLocation( child_src );
				
				InventoryLocation child_dst = new InventoryLocation;
				child_dst.Copy( child_src );
				child_dst.SetParent( targetItem );

				bool drop = false;

				if (GameInventory.LocationCanAddEntity(child_dst))
				{
					// try to move it to the same exact place in dst
					targetItem.GetInventory().TakeToDst(InventoryMode.LOCAL, child_src, child_dst);
				}
				else
				{
					drop = true; // or drop otherwise
				}

				if (drop)
				{
					player.LocalDropEntity(child);
					GetGame().RemoteObjectTreeCreate(child);
					result = TransferInventoryResult.DroppedSome;
				}
			}
		}
		return result;
	}
	
	static void UnlimitedAmmoDebugCheck(Weapon_Base weapon)
	{
		if ( ItemBase.GetDebugActionsMask() & DebugActionType.UNLIMITED_AMMO )
		{
			Magazine magazine;
			if ( GetGame().IsServer() )
			{
				magazine = weapon.GetMagazine(weapon.GetCurrentMuzzle());
			
				if (magazine)
				{
					if (magazine.GetAmmoCount() <= 5)
					{
						magazine.ServerSetAmmoMax();
					}
				}
			}
			else
			{
				magazine = weapon.GetMagazine(weapon.GetCurrentMuzzle());
			
				if (magazine)
				{
					if (magazine.GetAmmoCount() <= 5)
					{
						magazine.LocalSetAmmoMax();
					}
				}
			
			}
		}
	}
	

		
	static void TurnItemIntoItem (notnull ItemBase old_item, string new_item_type, notnull PlayerBase player)
	{
		TurnItemIntoItemEx(player, new TurnItemIntoItemLambda(old_item, new_item_type, player));
	}
		
	static void TurnItemIntoItemEx (notnull PlayerBase player, ReplaceItemWithNewLambdaBase lambda)
	{
		player.ServerReplaceItemWithNew(lambda);
	}

	static void TurnItemInHandsIntoItem (notnull ItemBase old_item, string new_item_type, notnull PlayerBase player)
	{
		TurnItemInHandsIntoItemEx(player, new TurnItemIntoItemLambda(old_item, new_item_type, player));
	}
	
	static void TurnItemInHandsIntoItemEx (notnull PlayerBase player, ReplaceItemWithNewLambdaBase lambda)
	{
		player.ServerReplaceItemInHandsWithNew(lambda);
	}

	//!Spawns multiple piles of stackable ItemBase objects on ground (intended for generic use)
	static array<ItemBase> CreateItemBasePiles(string item_name, vector ground_position, float quantity, float health, bool floaty_spawn = false)
	{
		array<ItemBase>	item_piles;
		float max_stack_size;
		ItemBase pile;
		
		item_piles = new array<ItemBase>;
		max_stack_size = g_Game.ConfigGetInt("cfgVehicles " + item_name + " varStackMax");
		if( max_stack_size < 1)
			max_stack_size = g_Game.ConfigGetInt("cfgVehicles " + item_name + " varQuantityMax");
		if( max_stack_size < 1)
			max_stack_size = 1;
		
		int full_piles_count = Math.Floor(quantity/max_stack_size);
		int rest = quantity - (full_piles_count*max_stack_size);
		
		for ( int i = 0; i < full_piles_count; i++ )
		{
			if (floaty_spawn)
				pile = ItemBase.Cast(GetGame().CreateObjectEx(item_name, ground_position, ECE_CREATEPHYSICS|ECE_UPDATEPATHGRAPH));
			else
				pile = ItemBase.Cast(GetGame().CreateObjectEx(item_name, ground_position, ECE_PLACE_ON_SURFACE));
			pile.SetQuantity(max_stack_size);
			pile.SetHealth(health);
			item_piles.Insert(pile);
		}
		if ( rest > 0 )
		{
			if (floaty_spawn)
				pile = ItemBase.Cast(GetGame().CreateObjectEx(item_name, ground_position, ECE_CREATEPHYSICS|ECE_UPDATEPATHGRAPH));
			else
				pile = ItemBase.Cast(GetGame().CreateObjectEx(item_name, ground_position, ECE_PLACE_ON_SURFACE));
			pile.SetQuantity(rest);
			pile.SetHealth(health);
			item_piles.Insert(pile);
		}
		return item_piles;
	}
	
	static array<Magazine> CreateMagazinePiles(string item_name, vector ground_position, float quantity,  float health )
	{
		array<Magazine>	items;
		float stack_size;
		Magazine pile;
		
		items = new array<Magazine>;
		stack_size = g_Game.ConfigGetInt("cfgMagazines " + item_name + " count");
		
		int piles_count = Math.Floor(quantity/stack_size);
		int rest = quantity - (piles_count*stack_size);
		
		for ( int i = 0; i < piles_count; i++ )
		{
			pile = Magazine.Cast(GetGame().CreateObjectEx(item_name, ground_position, ECE_PLACE_ON_SURFACE));
			pile.ServerSetAmmoCount(stack_size);
			items.Insert(pile);
		}
		if ( rest > 0)
		{
			pile = Magazine.Cast(GetGame().CreateObjectEx(item_name, ground_position, ECE_PLACE_ON_SURFACE));
			pile.ServerSetAmmoCount(rest);
			items.Insert(pile);
		}
		return items;
	}
	
	static array<Magazine> CreateMagazinePilesFromBullet(string bullet_type, vector ground_position, float quantity,  float health )
	{
		array<Magazine>	items;
		items = new array<Magazine>;
		float stack_size;
		Magazine pile;
		string item_name;
		if( !g_Game.ConfigGetText("cfgAmmo " + bullet_type + " spawnPileType", item_name) )
			return items;
		
		stack_size = g_Game.ConfigGetInt("cfgMagazines " + item_name + " count");
		
		if(stack_size > 0)
		{
			int piles_count = Math.Floor(quantity/stack_size);
			int rest = quantity - (piles_count*stack_size);
		
			for ( int i = 0; i < piles_count; i++ )
			{
				pile = Magazine.Cast(GetGame().CreateObjectEx(item_name, ground_position, ECE_PLACE_ON_SURFACE));
				pile.ServerSetAmmoCount(stack_size);
				items.Insert(pile);
			}
			if ( rest > 0)
			{
				pile = Magazine.Cast(GetGame().CreateObjectEx(item_name, ground_position, ECE_PLACE_ON_SURFACE));
				pile.ServerSetAmmoCount(rest);
				items.Insert(pile);
			}
		}
		return items;
	}
	
	static int GetHealthLevelForAmmo(string class_name, float health)
	{
		float health_normalized = health / 100;
		string config_path = CFG_WEAPONSPATH + " " + class_name + " DamageSystem" + " GlobalHealth" + " healthLabels";
		CachedObjectsArrays.ARRAY_FLOAT.Clear();
		GetGame().ConfigGetFloatArray(config_path, CachedObjectsArrays.ARRAY_FLOAT);
		for(int i = 0; i < CachedObjectsArrays.ARRAY_FLOAT.Count(); i++)
		{
			if( health_normalized >= CachedObjectsArrays.ARRAY_FLOAT.Get(i) )
			{
				return i;
			}
		}
		return -1;
	}
	
	static float GetTypeMaxGlobalHealth(string class_name, string health_type = "Health")
	{
		float max_health;
		string cfg_path;
		
		if ( GetGame().ConfigIsExisting(CFG_VEHICLESPATH+" "+class_name) )
		{
			cfg_path = CFG_VEHICLESPATH;
		}
		else if ( GetGame().ConfigIsExisting(CFG_WEAPONSPATH+" "+class_name) )
		{
			cfg_path = CFG_WEAPONSPATH;
		}
		else if ( GetGame().ConfigIsExisting(CFG_MAGAZINESPATH+" "+class_name) )
		{
			cfg_path = CFG_MAGAZINESPATH;
		}
		cfg_path = cfg_path + " "+class_name+" DamageSystem GlobalHealth " + health_type + " hitpoints";
		max_health = GetGame().ConfigGetFloat(cfg_path);
		
		return max_health;
	}
	
	static bool GetProjectedCursorPos3d (out vector position, Weapon_Base weapon)
	{
		vector usti_hlavne_position = weapon.GetSelectionPositionMS( "usti hlavne" );
		vector konec_hlavne_position = weapon.GetSelectionPositionMS( "konec hlavne" );
		vector end_point = weapon.ModelToWorld(usti_hlavne_position);
		vector begin_point = weapon.ModelToWorld(konec_hlavne_position);
		vector contact_dir;
		int contact_component;
		
		vector aim_point = end_point - begin_point;
		aim_point = aim_point.Normalized() * PROJECTED_CURSOR_DISTANCE;
		aim_point = aim_point + end_point;
		
		if (DayZPhysics.RaycastRV(begin_point, aim_point, position, contact_dir, contact_component, null, null, null, false, false, ObjIntersectFire))
			return true;
		return false;
	}
	
	static void GetHeadBonePos(notnull PlayerBase player, out vector pos)
	{
		// Get position of head for starting trace pos, otherwise use sane default
		int headBoneIdx = player.GetBoneIndexByName("Head");
		if ( headBoneIdx == -1 )
			{ pos = player.GetPosition()[1] + 1.6; }
		else
			{ pos = player.GetBonePositionWS(headBoneIdx); }	
	}

	static vector GetHeadingVector(notnull PlayerBase player)
	{
		vector dir = vector.Zero;
		float headingAngle = GetHeadingAngle(player);
		dir[0] = Math.Cos(headingAngle + Math.PI_HALF);
		dir[2] = Math.Sin(headingAngle + Math.PI_HALF);
		
		return dir.Normalized();
	}

	static float GetHeadingAngle(notnull DayZPlayerImplement player)
	{
		HumanInputController hic = player.GetInputController();
		float headingAngle = hic.GetHeadingAngle();
		
		return headingAngle;
	}
	
	static float GetEnergyMetabolicSpeed(int movement_speed)
	{
		float speed;
		switch (movement_speed)
		{
		case DayZPlayerConstants.MOVEMENTIDX_WALK:
			speed = PlayerConstants.METABOLIC_SPEED_ENERGY_WALK;
		break;
		case DayZPlayerConstants.MOVEMENTIDX_RUN:
			speed = PlayerConstants.METABOLIC_SPEED_ENERGY_JOG;
		break;
		case DayZPlayerConstants.MOVEMENTIDX_SPRINT:
			speed = PlayerConstants.METABOLIC_SPEED_ENERGY_SPRINT;
		break;
		default:
			speed = 0;
		break;
		}

		speed += PlayerConstants.METABOLIC_SPEED_ENERGY_BASAL;
		return speed;
	}
	
	static float GetWaterMetabolicSpeed(int movement_speed)
	{
		float speed;
		switch (movement_speed)
		{
		case DayZPlayerConstants.MOVEMENTIDX_WALK:
			speed = PlayerConstants.METABOLIC_SPEED_WATER_WALK;
		break;
		case DayZPlayerConstants.MOVEMENTIDX_RUN:
			speed = PlayerConstants.METABOLIC_SPEED_WATER_JOG;
		break;
		case DayZPlayerConstants.MOVEMENTIDX_SPRINT:
			speed = PlayerConstants.METABOLIC_SPEED_WATER_SPRINT;
		break;
		default:
			speed = 0;
		break;
		}

		speed += PlayerConstants.METABOLIC_SPEED_WATER_BASAL;
		return speed;
	}
	
	static string ObtainRestrainItemTargetClassname(notnull EntityAI entity)
	{
		return entity.ConfigGetString( "OnRestrainChange");
	}
	
	static void TransformRestrainItem(EntityAI current_item, EntityAI tool, PlayerBase player_source, PlayerBase player_target, bool destroy = false)
	{
		bool type;
		
		if ( tool )
		{
			//is unrestrain and not struggle
			type = tool.ConfigGetBool("RestrainUnlockType");
		}
		string new_item_name = current_item.ConfigGetString( "OnRestrainChange");
		
		if ( new_item_name != "" )
		{
			if ( player_target )
			{
				if (player_target.IsAlive())
					MiscGameplayFunctions.TurnItemIntoItemEx(player_target, new ReplaceAndDestroyLambdaEx(current_item, new_item_name, player_target, type));
				else
					MiscGameplayFunctions.TurnItemIntoItemEx(player_source, new DestroyItemInCorpsesHandsAndCreateNewOnGndLambda(current_item, new_item_name, player_target, type));
			}
			else
			{
				MiscGameplayFunctions.TurnItemIntoItemEx(player_target, new ReplaceAndDestroyLambdaEx(current_item, new_item_name, player_target, type));
			}
		}
		else
		{
			Error("current_item:" +current_item+ ", tool:" +tool +". No value for 'OnRestrainChange' config parameter");
		}
	}
	
	static bool IsValueInRange(float value, float from, float to)
	{
		return (value >= from) && (value <= to);
	}

	//! Check if player direction(based on cone of defined angle) is oriented to target position
	static bool IsPlayerOrientedTowardPos(notnull DayZPlayerImplement player, vector target_pos, float cone_angle)
	{
		if (player)
		{
			vector player_dir = player.GetDirection();
			vector to_target_dir = target_pos - player.GetPosition();
	
			player_dir[1] = 0;
			to_target_dir[1] = 0;
		
			player_dir.Normalize();
			to_target_dir.Normalize();
		
			float cos_fi = vector.Dot(player_dir, to_target_dir);
			vector cross = player_dir * to_target_dir;
		
			int dir = Math.Acos(cos_fi) * Math.RAD2DEG;
				
			if( cross[1] < 0 )
				dir = -dir;

			//! dir in cone or in a tip of cone 
			if( (dir <= cone_angle && dir >= -cone_angle) || Math.AbsFloat(dir) == 90 )
			{
				return true;
			}
		}
		
		return false;
	}
	
	//TODO
	static string SanitizeString(string input)
	{
		int max_length = 512;
		string output = input;
		
		output = output.Substring(0,Math.Clamp(max_length,0,output.Length()));
		return output;
	}
	
	// deprecated - dont use
	static bool ComplexBuildCollideCheckClient( PlayerBase player, ActionTarget target, ItemBase item, string partName = "" )
	{
		return true;
	}
	
	static bool ComplexBuildCollideCheckClient( PlayerBase player, ActionTarget target, ItemBase item, int constraction_index )
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast( target.GetObject() );
		if (base_building)
		{
			Construction construction = base_building.GetConstruction();
			if (construction && BuildCondition( player, target, item, false, constraction_index ))
			{
				ConstructionActionData construction_action_data = player.GetConstructionActionData();
				string partName;
				if ( item )
				{
					partName = construction_action_data.GetBuildPartAtIndex(constraction_index).GetPartName();
				}
				else
				{
					partName = construction_action_data.GetBuildPartNoToolAtIndex(constraction_index).GetPartName();
				}
				bool boo;
				if (base_building.PerformRoofCheckForBase(partName,player,boo) && boo)
					return false;
				if ( player.IsPlacingLocal() || player.IsPlacingServer() )
					return false;
				
				/*float distance_root = vector.DistanceSq(target.GetCursorHitPos(), player.GetPosition());

				if (!CfgGameplayHandler.GetDisableDistanceCheck() && distance_root < UAMaxDistances.BASEBUILDING_SHORT)
				{
					return false;
				} */
				return !construction.IsColliding( partName );
			}
		}
		return false;
	}
	
	// deprecated - dont use
	static bool BuildCondition( PlayerBase player, ActionTarget target, ItemBase item, bool camera_check )
	{
		return true;
	}
	
	static bool BuildCondition( PlayerBase player, ActionTarget target, ItemBase item, bool camera_check, int constraction_index )
	{	
		Object targetObject = target.GetObject();
		if ( targetObject && targetObject.CanUseConstruction() )
		{
			BaseBuildingBase base_building = BaseBuildingBase.Cast( targetObject );
			ConstructionActionData construction_action_data = player.GetConstructionActionData();
			construction_action_data.SetTarget( targetObject );
			
			ConstructionPart constrution_part;
			if ( item )
			{
				constrution_part = construction_action_data.GetBuildPartAtIndex(constraction_index);
			}
			else
			{
				constrution_part = construction_action_data.GetBuildPartNoToolAtIndex(constraction_index);
			}

			//Debug
			/*
			if ( constrution_part )
			{
				Construction construction = base_building.GetConstruction();	
				construction.IsColliding( constrution_part.GetPartName() );
			}
			*/
			
			if ( constrution_part )
			{
				//camera and position checks
				bool position_check = ( base_building.MustBeBuiltFromOutside() && !base_building.IsPlayerInside(player, constrution_part.GetMainPartName()) ) || ( !base_building.MustBeBuiltFromOutside() && base_building.IsPlayerInside(player, constrution_part.GetMainPartName()) );
				if ( position_check && !player.GetInputController().CameraIsFreeLook() )
				{
					//Camera check (client-only)
					if ( camera_check )
					{
						if ( GetGame() && ( !GetGame().IsDedicatedServer() ) )
						{
							return !base_building.IsFacingCamera( constrution_part.GetMainPartName() );
						}
					}
					
					return true;
				}
			}
		}
		
		return false;
	}
	
	static void IsUnderRoofFromToCalculation(EntityAI entity, out vector from, out vector to, float height = GameConstants.ROOF_CHECK_RAYCAST_DIST)
	{
		vector minMax[2];
		entity.GetCollisionBox(minMax);

		vector size = Vector(0,0,0);
		//size[1] = minMax[1][1] - minMax[0][1];
		float from_override = entity.HeightStartCheckOverride();
		if (from_override > 0.0)
		{
			size[1] = from_override;
		}
		else
		{
			size[1] = minMax[1][1] - minMax[0][1];
		}
		
		from = entity.GetPosition() + size;  
		if ( entity.HeightCheckOverride() > 0 )
		{
			to = entity.GetPosition() + Vector(0, entity.HeightCheckOverride(), 0);
		}
		else
		{
			vector ceiling = "0 0 0";
			ceiling[1] = height;
			to = from + ceiling; //+size ??? offset to cast same distance
		}
	}
	
	static bool IsUnderRoof(EntityAI entity, float height = GameConstants.ROOF_CHECK_RAYCAST_DIST) 
	{
		return IsUnderRoofEx(entity, height, ObjIntersectView);
	}
	
	static bool IsUnderRoofEx(EntityAI entity, float height = GameConstants.ROOF_CHECK_RAYCAST_DIST, int geometry = ObjIntersectView) 
	{
		vector from;
		vector to;
		
		IsUnderRoofFromToCalculation(entity, from, to, height);

		vector contact_pos;
		vector contact_dir;

		int contact_component;
		//set<Object> hit_object = new set<Object>;
		bool boo = DayZPhysics.RaycastRV( from, to, contact_pos, contact_dir, contact_component, /*hit_object*/NULL, NULL, entity, false, false, geometry,0.25 );
		
		/*if (boo)
		{
			Debug.DrawSphere(from , 0.8,Colors.YELLOW, ShapeFlags.ONCE);
			Debug.DrawSphere(to , 0.8,Colors.RED, ShapeFlags.ONCE);
		}
		else
		{
			Debug.DrawSphere(from , 0.8,Colors.GREEN, ShapeFlags.ONCE);
			Debug.DrawSphere(to , 0.8,Colors.RED, ShapeFlags.ONCE);
		}*/
		
		return boo;
	}

	// cooking equipment effects (get position for steam particle)
	static vector GetSteamPosition( EntityAI parent )
	{
		vector particle_pos;
		float steam_offset = 0;
		
		if ( parent )
		{
			particle_pos = parent.GetPosition();
			
			if ( parent.IsInherited( PortableGasStove ) )
			{
				steam_offset = 0.2;
			}
			else if ( parent.IsInherited( FireplaceBase ) )
			{
				FireplaceBase fireplace = FireplaceBase.Cast( parent );
				
				if ( fireplace.IsBaseFireplace() )
				{
					steam_offset = 0.8;
				}
				else if ( fireplace.IsBarrelWithHoles() )
				{
					steam_offset = 1.1;
				}
				else if ( fireplace.IsFireplaceIndoor() )
				{
					steam_offset = 0.45;
				}
				else if ( fireplace.IsIndoorOven() )
				{
					steam_offset = 0.9;
				}
			}
		}
		
		particle_pos[1] = particle_pos[1] + steam_offset;
		
		return particle_pos;
	}
	
	static void DropAllItemsInInventoryInBounds(ItemBase ib, vector halfExtents)
	{
		array<EntityAI> items = new array<EntityAI>;
		ib.GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, items);
		
		vector direction = ib.GetDirection();
		float dot = vector.Dot(direction, vector.Forward);
		
		float angle = Math.Acos(dot);	
		if (direction[0] < 0)
			angle = -angle;	

		float cos = Math.Cos(angle);
		float sin = Math.Sin(angle);
		
		EntityAI item;
		int count = items.Count();
		for ( int i = 0; i < count; ++i )
		{
			item = items.Get(i);
			if ( item )
				ib.GetInventory().DropEntityInBounds(InventoryMode.SERVER, ib, item, halfExtents, angle, cos, sin);					
		}
	}
	
	static void ThrowAllItemsInInventory(notnull EntityAI parent, int flags)
	{
		vector position = parent.GetPosition();
		vector orientation = parent.GetOrientation();
		vector rotation_matrix[3];
		float direction[4];
		
		vector minmax[2];
		parent.GetCollisionBox(minmax);

		Math3D.YawPitchRollMatrix( orientation, rotation_matrix );
		Math3D.MatrixToQuat( rotation_matrix, direction );

		vector randomPos;
		for ( int i = 0; i < parent.GetInventory().AttachmentCount(); ++i )
		{
			randomPos = Vector(position[0] + Math.RandomFloat(minmax[0][0], minmax[1][0]),
								position[1] + Math.RandomFloat(minmax[0][1], minmax[1][1]),
								position[2] + Math.RandomFloat(minmax[0][2], minmax[1][2]));
			
			ThrowEntityFromInventory(parent.GetInventory().GetAttachmentFromIndex( i ), randomPos, direction, -GetVelocity(parent), ThrowEntityFlags.NONE);
		}

		for ( int j = 0; j < parent.GetInventory().GetCargo().GetItemCount(); ++j )
		{
			randomPos = Vector(position[0] + Math.RandomFloat(minmax[0][0], minmax[1][0]),
								position[1] + Math.RandomFloat(minmax[0][1], minmax[1][1]),
								position[2] + Math.RandomFloat(minmax[0][2], minmax[1][2]));
			
			ThrowEntityFromInventory(parent.GetInventory().GetCargo().GetItem( j ), randomPos, direction, -GetVelocity(parent), ThrowEntityFlags.NONE);
		}
	}
	
	static void ThrowEntityFromInventory(notnull EntityAI entity, vector position, float direction[4], vector force, int flags)
	{	
		InventoryMode invMode = InventoryMode.SERVER;
		if ( !GetGame().IsMultiplayer() )
			invMode = InventoryMode.LOCAL;
		
		ItemBase entityIB;
		if (CastTo(entityIB, entity))
		{
			InventoryLocation dst = new InventoryLocation;	
			dst.SetGroundEx(entity, position, direction);
			
			if ( (flags & ThrowEntityFlags.SPLIT) && entityIB.CanBeSplit() )
			{
				for (int l = 0; l < entityIB.GetQuantity(); ++l)
				{
					ItemBase new_item = ItemBase.Cast( GameInventory.LocationCreateEntity( dst, entityIB.GetType(), ECE_NONE, RF_DEFAULT ) );
		
					if ( new_item )
					{
						MiscGameplayFunctions.TransferItemProperties(entityIB, new_item);
						entityIB.AddQuantity( -1 );
						new_item.SetQuantity( 1 );
						new_item.ThrowPhysically(null, force);
					}
				}
			}
			else
			{
				float stackable = entityIB.GetTargetQuantityMax();
				if ( !(stackable == 0 || stackable >= entityIB.GetQuantity()) )
				{					
					while (entityIB.GetQuantity() > stackable)
					{
						InventoryLocation spltDst = new InventoryLocation;
						position[1] = position[1] + 0.1;
						spltDst.SetGroundEx(entity, position, direction);
						
						ItemBase splitItem = entityIB.SplitIntoStackMaxToInventoryLocationEx( spltDst );
						splitItem.ThrowPhysically(null, force);
					}
				}
				
				InventoryLocation src = new InventoryLocation;
				entity.GetInventory().GetCurrentInventoryLocation(src);
				
				entity.GetInventory().TakeToDst(invMode, src, dst);
				entityIB.ThrowPhysically(null, force);
			}
		}
		else
		{
			entity.GetInventory().DropEntity(invMode, entity.GetHierarchyRoot(), entity);
			dBodyApplyImpulse(entity, force);
		}
	}

	static float GetCurrentItemHeatIsolation( ItemBase pItem )
	{
		float wetFactor;
		float healthFactor;

		float heatIsolation = pItem.GetHeatIsolation(); 	//! item heat isolation (from cfg)
		float itemHealthLabel = pItem.GetHealthLevel();		//! item health (state)
		float itemWetness = pItem.GetWet();					//! item wetness
		
		//! wet factor selection
		if ( itemWetness >= GameConstants.STATE_DRY && itemWetness < GameConstants.STATE_DAMP )
		{
			wetFactor = GameConstants.ENVIRO_ISOLATION_WETFACTOR_DRY;
		}
		else if ( itemWetness >= GameConstants.STATE_DAMP && itemWetness < GameConstants.STATE_WET )
		{
			wetFactor = GameConstants.ENVIRO_ISOLATION_WETFACTOR_DAMP;
		}
		else if ( itemWetness >= GameConstants.STATE_WET && itemWetness < GameConstants.STATE_SOAKING_WET )
		{
			wetFactor = GameConstants.ENVIRO_ISOLATION_WETFACTOR_WET;
		}
		else if ( itemWetness >= GameConstants.STATE_SOAKING_WET && itemWetness < GameConstants.STATE_DRENCHED )
		{
			wetFactor = GameConstants.ENVIRO_ISOLATION_WETFACTOR_SOAKED;
		}
		else if ( itemWetness >= GameConstants.STATE_DRENCHED )
		{
			wetFactor = GameConstants.ENVIRO_ISOLATION_WETFACTOR_DRENCHED;
		}
		
		//! health factor selection
		switch (itemHealthLabel)
		{
			case GameConstants.STATE_PRISTINE:
				healthFactor = GameConstants.ENVIRO_ISOLATION_HEALTHFACTOR_PRISTINE;
			break;
			
			case GameConstants.STATE_WORN:
				healthFactor = GameConstants.ENVIRO_ISOLATION_HEALTHFACTOR_WORN;
			break;
			
			case GameConstants.STATE_DAMAGED:
				healthFactor = GameConstants.ENVIRO_ISOLATION_HEALTHFACTOR_DAMAGED;
			break;
			
			case GameConstants.STATE_BADLY_DAMAGED:
				healthFactor = GameConstants.ENVIRO_ISOLATION_HEALTHFACTOR_B_DAMAGED;
			break;
			
			case GameConstants.STATE_RUINED:
				healthFactor = GameConstants.ENVIRO_ISOLATION_HEALTHFACTOR_RUINED;
			break;
		}
		
		//! apply factors
		heatIsolation *= healthFactor;
		heatIsolation *= wetFactor;

		return heatIsolation;
	}
	
	static void FilterObstructingObjects(array<Object> potentiallyObstructingObjects, out array<Object> obstructingObjects)
	{
		if (!obstructingObjects)
			obstructingObjects = new array<Object>;
		
		for ( int i = 0; i < potentiallyObstructingObjects.Count(); ++i )
		{
			Object obj = potentiallyObstructingObjects[i];
			if ( obj && ( obj.CanObstruct() || obj.CanProxyObstruct() ) )
				obstructingObjects.Insert(obj);
		}
	}
	
	static bool CanIgnoreDistanceCheck(Object obj)
	{
	    return obj.IsTransport() || obj.CanUseConstruction();
	}
	
	//! group objects that are close to each other together	
	static void FilterObstructedObjectsByGrouping(vector origin, float range, float distanceDelta, array<Object> objects, array<Object> obstructingObjects, out array<Object> filteredObjects, bool doDistanceCheck = false, bool checkIfDistanceCanBeIgnored = false, float maxDist = 0)
	{
		array<Object> vicinityObjects= new array<Object>;
		vicinityObjects.Copy(objects);
		
		int i = 0;
		int j = 0;
		int k = 0;			
		int mCount = vicinityObjects.Count();
		
		if (!filteredObjects)
			filteredObjects = new array<Object>;
		
		// Remove objects that are too far from the player anyways
		if ( doDistanceCheck )
		{
			for ( i = vicinityObjects.Count() - 1; i >= 0; --i )
			{
				Object obj = vicinityObjects[i];
				if ( obj && !CanIgnoreDistanceCheck( obj ) && vector.DistanceSq(origin, obj.GetPosition()) > maxDist * maxDist )
					vicinityObjects.Remove(i);
			}
		}

		// Sort obstructingObjects to have the furthest one first
		array<Object> sortedObstructingObjects = new array<Object>;
		array<float> distanceHelper = new array<float>;
		array<float> distanceHelperUnsorted = new array<float>;
		float distance, dist1, dist2;

		for ( i = 0; i < obstructingObjects.Count(); ++i )
		{
			distance = vector.DistanceSq(obstructingObjects[i].GetWorldPosition(), origin);
			distanceHelper.Insert(distance);
		}

		distanceHelperUnsorted.Copy(distanceHelper);
		distanceHelper.Sort();

		for ( i = distanceHelper.Count() - 1; i >= 0; --i )
			sortedObstructingObjects.Insert(obstructingObjects[distanceHelperUnsorted.Find(distanceHelper[i])]);

		array<ref array<Object>> tempGroups = new array<ref array<Object>>;
		array<ref array<Object>> objectGroups = new array<ref array<Object>>;
		array<Object> group;
		
		float cos = Math.Cos(90);
		float sin = Math.Sin(90);

		// Iterate through sorted obstructingObjects
		for ( i = 0; i < sortedObstructingObjects.Count(); ++i )
		{
			Object obstrObj = sortedObstructingObjects[i];
			vector worldPos = obstrObj.GetWorldPosition();
			vector min, max;
			vector minMax[2];
			if ( obstrObj.GetCollisionBox(minMax) )
			{
				min = minMax[0];
				max = minMax[1];
				max = max * (obstrObj.GetOrientation() * range);

				vector center, dx, dy, dz, half;
				center = (min + max) * 0.5;
				dz = obstrObj.GetOrientation();
				dx = vector.RotateAroundZero(dz, vector.Up, cos, sin);
				dy = vector.RotateAroundZero(dz, vector.Aside, cos, sin);
				half = (max - min) * 0.5;
				half = Vector(Math.AbsFloat(half[0]), Math.AbsFloat(half[1]), Math.AbsFloat(half[2]));

				group = new array<Object>;

				// Group objects within the above box
				for ( j = vicinityObjects.Count() - 1; j >= 0; --j )
				{
					Object vicObj = vicinityObjects[j];
					if ( vicObj )
					{
						vector d = vicObj.GetWorldPosition() - worldPos + center;
						if ( Math.AbsFloat(vector.Dot(d, dx)) <= half[0] && Math.AbsFloat(vector.Dot(d, dy)) <= half[1] && Math.AbsFloat(vector.Dot(d, dz)) <= half[2] )
						{
							group.Insert(vicObj);
							vicinityObjects.Remove(j);
						}
					}
				}

				if ( group.Count() > 0 )
					tempGroups.Insert(group);
			}
		}

		// Go through the objects grouped by obstruction to split them by distance too
		for ( i = 0; i < tempGroups.Count(); ++i )
			SplitArrayIntoGroupsByDistance(tempGroups[i], objectGroups, distanceDelta);

		// Split initial groups by distance
		SplitArrayIntoGroupsByDistance(vicinityObjects, objectGroups, distanceDelta);

		// Raycast accordingly to groups		
		IsObjectObstructedCache cache = new IsObjectObstructedCache(origin, mCount);
		for ( i = 0; i < objectGroups.Count(); ++i )
		{
			array<Object> objectGroup = objectGroups[i];
			Object sampleObject = objectGroup[0];

			if ( !IsObjectObstructedEx(sampleObject, cache) )
				filteredObjects.InsertAll(objectGroup);

			cache.ClearCache();
		}
	}

	static void SplitArrayIntoGroupsByDistance(array<Object> objects, array<ref array<Object>> objectGroups, float squaredDistanceDelta)
	{
		array<Object> group;
		for ( int i = 0; i < objects.Count(); )
		{
			Object obj1 = objects[i];
			if ( obj1 )
			{
				group = new array<Object>;
				group.Insert(obj1);
				for ( int j = objects.Count() - 1; j > i; --j )
				{
					Object obj2 = objects[j];
					if ( obj1 && obj2 )
					{
						vector start = obj1.GetWorldPosition();
						vector end = obj2.GetWorldPosition();

						float distance = vector.DistanceSq(start, end);
						if ( distance < squaredDistanceDelta )
						{								
							group.Insert(obj2);
							objects.Remove(j);
						}
					}
				}
				objectGroups.Insert(group);
				objects.Remove(i);
				continue;
			}
			++i;
		}
	}
	
	static bool IsObjectObstructed(Object object, bool doDistanceCheck = false, vector distanceCheckPos = "0 0 0", float maxDist = 0)
	{
		vector rayStart;
		MiscGameplayFunctions.GetHeadBonePos( PlayerBase.Cast( GetGame().GetPlayer() ), rayStart);
		IsObjectObstructedCache cache = new IsObjectObstructedCache(rayStart, 1);
		return IsObjectObstructedEx(object, cache, doDistanceCheck, distanceCheckPos, maxDist);
	}

	static bool IsObjectObstructedEx(Object object, IsObjectObstructedCache cache, bool doDistanceCheck = false, vector distanceCheckPos = "0 0 0", float maxDist = 0)
	{
		if (!object)
			return true;
		
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (doDistanceCheck && vector.DistanceSq(player.GetPosition(), distanceCheckPos) > maxDist * maxDist)
			return true;

		cache.ObjectCenterPos = object.GetCenter();
			
		if (IsObjectObstructedProxy(object, cache, player))
			return true;
			
		//Print(" ===>>> pouzij standardny raycast s fire geometriou koli domom a basebuildingu <<<=== ");
		DayZPhysics.RaycastRV( cache.RaycastStart, cache.ObjectCenterPos, cache.ObjectContactPos, cache.ObjectContactDir, cache.ContactComponent, cache.HitObjects, object, GetGame().GetPlayer(), false, false, ObjIntersectFire, 0.0, CollisionFlags.ALLOBJECTS );
			
		return IsObjectObstructedFilter(object, cache, player);
	}
 	
	static bool IsObjectObstructedProxy(Object object, IsObjectObstructedCache cache, PlayerBase player)
	{
		if ( object.CanProxyObstruct() )
		{
			//Print(" :) (: pouzij proxy raycast koli proxy itemom :) (: ");
			RaycastRVParams ray_input = new RaycastRVParams( cache.RaycastStart, cache.ObjectCenterPos, player );
			DayZPhysics.RaycastRVProxy( ray_input, cache.HitProxyObjects );				
			if ( cache.HitProxyObjects )
			{
				//Print(" - cache.HitProxyObjects - ");
				if ( cache.HitProxyObjects.Count() > 0 )
				{
					if ( cache.HitProxyObjects[0].hierLevel > 0 )
					{
						// ignores attachments on player
						if ( !cache.HitProxyObjects[0].parent.IsMan() )
						{
							//Print( "cache.HitProxyObjects[0].obj " + cache.HitProxyObjects[0].obj );
							//Print( "cache.HitProxyObjects[0].parent" + cache.HitProxyObjects[0].parent );
								
							if ( cache.HitProxyObjects[0].parent )
							{
								EntityAI proxy_parent = EntityAI.Cast( cache.HitProxyObjects[0].parent );
								if ( proxy_parent.GetInventory() && proxy_parent.GetInventory().GetCargo() )
									return true;
							}
						}	
					}
				}
			}
		}
		return false;
	}
	
	static bool IsObjectObstructedFilter(Object object, IsObjectObstructedCache cache, PlayerBase player)
	{
		for ( int m = 0; m < cache.HitObjects.Count(); m++ )
		{
			Object hit_object = cache.HitObjects.Get(m);
			
			if ( hit_object.CanObstruct() )
				return true;

			//4.3. ignore item if items are big and heavy >= OBJECT_OBSTRUCTION_WEIGHT 
			//EntityAI eai;
			//if ( Class.CastTo( eai, hit_object ) )
			//{					
			//	if ( eai.GetWeight() >= OBJECT_OBSTRUCTION_WEIGHT )
			//	{
			//		if ( eai != filtered_object && eai.GetHierarchyRoot() != filtered_object )
			//		{
			//			//Print("!!!!!obstacle vaha: " + hit_object);
			//			is_obstructed = true;
			//		}
			//	}
			//}
		}
		
		return false;
	}

	//Inflict absolute damage to item (used on tools when completing actions)
	static void DealAbsoluteDmg(ItemBase item, float dmg)
	{
		float totalDmg = dmg;
		//Print("The damage dealt is : " + totalDmg);
		item.DecreaseHealth(totalDmg, false);
		//Print("Current health of item is : " + item.GetHealth());
	}
	
	//Function used to normailze values, enter the used value and the max value (max will become 1)
	static float Normalize(int val, int maxVal)
	{
		float normVal;
		if (maxVal == 0)
		{
			Debug.LogError("Division by 0 is not allowed");
			return 0;
		}
		
		normVal = val / maxVal;
		return normVal;
	}
	
	static float Bobbing(float period, float amplitude, float elapsedTime)
	{
		//Prevent division by 0
		if ( period == 0 )
			period = 1;
		
		elapsedTime /= period;
		
		float cycle;
		cycle += elapsedTime;
		cycle = FModulus(cycle, 360);
		cycle = Math.Sin(cycle) * amplitude;
		
		return cycle;
	}
	
	// DEPRECATED, use Math.ModFloat directly instead
	static float FModulus(float x, float y)
	{
		// Keeping this for reference in case someone wants it later
		/*float res;
		//Prevent division by 0
		if (y == 0)
			y = 1;
		
		int n = Math.Floor(x/y);
		res = x - n * y;
		return res;*/
		return Math.ModFloat( x, y );
	}
	
	static void RemoveSplint( PlayerBase player )
	{
		EntityAI entity = player.GetInventory().CreateInInventory("Splint");
		if (!entity)
			entity = player.SpawnEntityOnGroundOnCursorDir("Splint", 0.5);
		
		EntityAI attachment;
		ItemBase new_item = ItemBase.Cast(entity);
		Class.CastTo(attachment, player.GetItemOnSlot("Splint_Right"));
		if ( attachment && attachment.GetType() == "Splint_Applied" )
		{
			if (new_item)
			{	
				MiscGameplayFunctions.TransferItemProperties(attachment,new_item);
				
				if (GetGame().IsServer())
				{
					//Lower health level of splint after use
					if (new_item.GetHealthLevel() < 4)
					{
						int newDmgLevel = new_item.GetHealthLevel() + 1;
						
						float max = new_item.GetMaxHealth("","");
						
						switch ( newDmgLevel )
						{
							case GameConstants.STATE_BADLY_DAMAGED:
								new_item.SetHealth( "", "", max * GameConstants.DAMAGE_BADLY_DAMAGED_VALUE );
								break;
		
							case GameConstants.STATE_DAMAGED:
								new_item.SetHealth( "", "", max * GameConstants.DAMAGE_DAMAGED_VALUE );
								break;
		
							case GameConstants.STATE_WORN:
								new_item.SetHealth( "", "", max * GameConstants.DAMAGE_WORN_VALUE );
								break;
							
							case GameConstants.STATE_RUINED:
								new_item.SetHealth( "", "", max * GameConstants.DAMAGE_RUINED_VALUE );
								break;
							
							default:
								break;
						}
					}
				}
			}
			
			attachment.Delete();
		}
	}
	
	//! checks if we should teleport the player to a safe location and if so, performs the teleportation
	static void TeleportCheck(notnull PlayerBase player, notnull array<ref array<float>> safe_positions)
	{
		if( player.GetSimulationTimeStamp() < 20 && !player.IsPersistentFlag(PersistentFlag.AREA_PRESENCE) )
		{
			//simulation time is bellow a threshold, which means the player has recently connected,
			//the player does not have the AREA_PRESENCE flag set, which means they were not inside the area when they disconnected,
			//that means they just spawned into a contaminated area, lets move them somewhere safe
			vector player_pos = player.GetPosition();
			vector closest_safe_pos = MiscGameplayFunctions.GetClosestSafePos(player_pos, safe_positions);
			
			if (player_pos!=closest_safe_pos)
			{
				closest_safe_pos[1] = GetGame().SurfaceY(closest_safe_pos[0], closest_safe_pos[2]);
				
				player.SetPosition( closest_safe_pos );//...so lets teleport them somewhere safe
				//DeveloperTeleport.SetPlayerPosition(player, closest_safe_pos);
				GetGame().RPCSingleParam(player, ERPCs.RPC_WARNING_TELEPORT, null, true, player.GetIdentity());
			}
			
			player.SetPersistentFlag(PersistentFlag.AREA_PRESENCE, false);
		}
	}
	
	
	static vector GetClosestSafePos(vector to_pos, notnull array<ref array<float>> positions)
	{
		vector closest_pos = to_pos;
		float smallest_dist = float.MAX;
		foreach( array<float> pos:positions)
		{
			vector vpos = "0 0 0";
			vpos[0] = pos[0];
			vpos[2] = pos[1];

			to_pos[1] = 0;
			float dist = vector.DistanceSq(to_pos, vpos);//2d dist sq
			if ( dist < smallest_dist)
			{
				smallest_dist = dist;
				closest_pos = vpos;
			}
		}
		return closest_pos;
	}
	
	static void GenerateAINoiseAtPosition(vector position, float lifeTime, NoiseParams noiseParams)
	{
		if (GetGame().IsServer())
		{
			NoiseSystem noise = GetGame().GetNoiseSystem();
			if (noise)
			{
				noise.AddNoiseTarget(position, lifeTime, noiseParams);
			}
		}
	}

	static float GetMinValue(array<float> pArray)
	{
		float minValue = 0.0;
		for (int i = 0; i < pArray.Count(); i++)
		{
			if (minValue == 0 || pArray.Get(i) < minValue)
			{
				minValue = pArray.Get(i);
			}
		}
		
		return minValue;
	}
	
	static float GetMaxValue(array<float> pArray)
	{
		float maxValue = 0.0;
		for (int i = 0; i < pArray.Count(); i++)
		{
			if (maxValue == 0 || pArray.Get(i) > maxValue)
			{
				maxValue = pArray.Get(i);
			}
		}
		
		return maxValue;
	}
		
	static string GetItemDisplayName(string type)
	{
		return GetGame().ConfigGetTextOut("CfgVehicles " + type + " displayName");
	}

	static bool IsComponentInSelection(array<Selection> pSelection, string pCompName)
	{
		if (pSelection.Count() == 0 || pCompName.Length() == 0)
		{
			return false;
		}

		for (int i = 0; i < pSelection.Count(); ++i)
		{
			pCompName.ToLower();
			if (pSelection[i] && pSelection[i].GetName() == pCompName)
			{
				return true;
			}
		}

		return false;
	}
	
	static int GetComponentIndex(array<Selection> pSelection, string pCompName)
	{
		if (!MiscGameplayFunctions.IsComponentInSelection(pSelection, pCompName))
		{
			return INDEX_NOT_FOUND;
		}

		for (int i = 0; i < pSelection.Count(); ++i)
		{
			pCompName.ToLower();
			if (pSelection[i] && pSelection[i].GetName() == pCompName)
			{
				return i;
			}
		}

		return INDEX_NOT_FOUND;
	}
};

class DestroyItemInCorpsesHandsAndCreateNewOnGndLambda : ReplaceAndDestroyLambda
{
	// @NOTE m_Player == target player - i.e. restrained one
	void DestroyItemInCorpsesHandsAndCreateNewOnGndLambda(EntityAI old_item, string new_item_type, PlayerBase player, bool destroy = false)
	{
		InventoryLocation gnd = new InventoryLocation;
		vector mtx[4];
		if (old_item)
			old_item.GetTransform(mtx);
		else
			player.GetTransform(mtx);
		gnd.SetGround(NULL, mtx);
		OverrideNewLocation(gnd);
	}
	
	protected override void RemoveOldItemFromLocation()
	{
		super.RemoveOldItemFromLocation();
		m_Player.GetHumanInventory().OnEntityInHandsDestroyed(m_OldLocation);
	}
}

// This was created since IsObjectObstructed is often called multiple times in one frame
// And apparently it seems that keeping this data in one struct seems to be more efficient than creating the variables dozens of times per frame
class IsObjectObstructedCache // Pretending this is a struct
{
	// Outside data
	vector RaycastStart = "0 0 0";
	int TotalObjects = 0;
	
	// Inside data
	vector ObjectCenterPos = "0 0 0";
	vector ObjectContactPos = "0 0 0";
	vector ObjectContactDir = "0 0 0";
	int ContactComponent = -1;
	ref array<ref RaycastRVResult> HitProxyObjects = new array<ref RaycastRVResult>;	
	ref set<Object> HitObjects = new set<Object>;
	
	void IsObjectObstructedCache(vector rayCastStart, int totalObjects)
	{
		RaycastStart = rayCastStart;
		TotalObjects = totalObjects;
	}
	
	// Only inside data
	void ClearCache()
	{		
		// TODO: What is fastest to assign to vector, "0 0 0" or Vector(0, 0, 0) ?
		ObjectCenterPos = "0 0 0";
		ObjectContactPos = "0 0 0";
		ObjectContactDir = "0 0 0";
		ContactComponent = -1;
		HitProxyObjects.Clear();
		HitObjects.Clear();
	}
	
	// Clear everything
	/*void Clear()
	{
		
	}*/
}