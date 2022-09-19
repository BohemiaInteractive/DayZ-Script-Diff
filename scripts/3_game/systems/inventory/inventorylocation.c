//@NOTE: DO NOT EDIT! enum values are overwritten from c++
/// types of Inventory Location
enum InventoryLocationType
{
	UNKNOWN, ///< unknown, usually freshly created object
	GROUND, /// < ground
	ATTACHMENT, ///< attachment of another entity
	CARGO, ///< cargo of another entity
	HANDS, ///< hands of another entity
	PROXYCARGO, ///< cargo of a large object (building,...)
};

//@NOTE: DO NOT EDIT! enum values are overwritten from c++
/// flags for searching locations in inventory
enum FindInventoryLocationType
{
	ATTACHMENT, 	///< 	ATT
	CARGO,			///< 	CGO
	HANDS, 			///< 	HND
	PROXYCARGO, 	///< 	PXY
	ANY_CARGO, 		///< 	CGO | PXY
	ANY, 			///< 	ATT | CGO | PXY | HND
	NO_SLOT_AUTO_ASSIGN ///<	skips auto-assign test
};

//! InventoryLocation
class InventoryLocation
{
	/**
	 * @fn			IsValid
	 * @brief		verify current set inventory location
	 * @return	true if valid, false otherwise
	 **/
	proto native bool IsValid ();
	/**
	 * @fn		GetType
	 * @brief	returns type of InventoryLocation
	 *
	 * @see	InventoryLocationType for known types of inventory locations
	 **/
	proto native int GetType ();
	/**
	 * @fn		GetParent
	 * @brief	returns parent of current inventory location
	 *
	 * Parent entity can be null if item is on ground.
	 *
	 * @return parent entity, null otherwise
	 **/
	proto native EntityAI GetParent ();
	/**
	 * @fn		GetItem
	 * @brief	returns item of current inventory location
	 *
	 * Item can be null if and only if this is a query for location
	 * of item that is about to be created (new spawn).
	 *
	 * @return	item
	 **/
	proto native EntityAI GetItem ();
	/**
	 * @fn		GetSlot
	 * @brief	returns slot id if current type is Attachment
	 *
	 * @NOTE: slot id is related to order of entries in DZ/data/config.cpp secton CfgSlots
	 *
	 * @return	slot id if attachment, -1 otherwise
	 **/
	proto native int GetSlot ();
	/**
	 * @fn		GetIdx
	 * @brief	returns index of cargo if current type is Cargo / ProxyCargo
	 * @return	index or -1
	 **/
	proto native int GetIdx ();
	/**
	 * @fn		GetRow
	 * @brief	returns row of cargo if current type is Cargo / ProxyCargo
	 * @return	row or -1
	 **/
	proto native int GetRow ();
	/**
	 * @fn		GetCol
	 * @brief	returns column of cargo if current type is Cargo / ProxyCargo
	 * @return	index or -1
	 **/
	proto native int GetCol ();
	/**
	 * @fn		GetFlip
	 * @brief	returns flip status of cargo
	 * @return	true/false
	 **/
	proto native bool GetFlip ();
	/**
	 * @fn		GetPos
	 * @brief	returns position of item in world if type is Ground
	 * @return	position
	 **/
	proto native vector GetPos ();
	/**
	 * @fn		GetDir
	 * @brief	returns direction of item in world if type is Ground
	 * @return	position
	 **/	
	proto native void GetDir (out float dir[4]);

	/**
	 * @fn		SetGround
	 * @brief	sets current inventory location type to Ground with transformation mat
	 * @param[in]	e		entity with this inventory location (null for new spawns)
	 * @param[in]	mat	transformation matrix
	 **/
	proto native void SetGround (EntityAI e, vector mat[4]);
	/**
	 * @fn		SetGroundEx
	 * @brief	sets current inventory location type to Ground with transformation mat
	 * @param[in]	e		entity with this inventory location (null for new spawns)
	 * @param[in]	pos		position vector
	 * @param[in]	dir		direction quat
	 **/
	proto native void SetGroundEx (EntityAI e, vector pos, float dir[4]);
	/**
	 * @fn		SetAttachment
	 * @brief	sets current inventory location type to Attachment with slot id set to <slotId>
	 * @param[in]	parent		parent entity the item <e> will be attached to
	 * @param[in]	e		entity with this inventory location (null for new spawns)
	 * @param[in]	slotId		slot id where the item will be attached
	 **/
	proto native void SetAttachment (notnull EntityAI parent, EntityAI e, int slotId);
	
	/**
	 * @fn		SetCargoAuto
	 * @brief	based on Cargo.IsProxyCargo uses SetProxyCargo or SetCargo respectively
	 * @param[in]	cargo		cargo that will receive item <e> into cargo
	 * @param[in]	e		entity with this inventory location (null for new spawns)
	 * @param[in]	row		row of the cargo
	 * @param[in]	col		column of the cargo
	 **/
	proto native void SetCargoAuto (notnull CargoBase cargo, EntityAI e, int row, int col, bool flip);

	/**
	 * @fn		SetCargo
	 * @brief	sets current inventory location type to Cargo with coordinates (idx, row, col)
	 * @param[in]	parent		parent entity that will receive item <e> into cargo
	 * @param[in]	e		entity with this inventory location (null for new spawns)
	 * @param[in]	idx		index of cargo
	 * @param[in]	row		row of the cargo
	 * @param[in]	col		column of the cargo
	 **/
	proto native void SetCargo (notnull EntityAI parent, EntityAI e, int idx, int row, int col, bool flip);
	/**
	 * @fn		SetProxyCargo
	 * @brief	sets current inventory location type to ProxyCargo with coordinates (idx, row, col)
	 * @NOTE: typical usage is in building-like dummy structures, where furniture with cargo is part
	 * 		of the model, and is not a physical entity (like person or bag).
	 * @param[in]	parent		parent entity that will receive item <e> into proxy cargo
	 * @param[in]	e		entity with this inventory location (null for new spawns)
	 * @param[in]	idx		index of cargo
	 * @param[in]	row		row of the cargo
	 * @param[in]	col		column of the cargo
	 *
	 **/
	proto native void SetProxyCargo (notnull EntityAI parent, EntityAI e, int idx, int row, int col, bool flip);
	/**
	 * @fn		SetHands
	 * @brief	sets current inventory location type to Hands
	 * @param[in]	parent		parent entity the item <e> will be attached to. this'd better be inherited from Person
	 * @param[in]	e		entity with this inventory location (null for new spawns)
	 **/
	proto native void SetHands (notnull EntityAI parent, EntityAI e);
	
	/**
	 * @fn		SetParent
	 * brief	sets current inventory location's GetParent
	 **/
	proto native void SetParent (notnull EntityAI parent);
	/**
	 * @fn		SetItem
	 * brief	sets current inventory location's item
	 **/
	proto native void SetItem (notnull EntityAI item);

	// direct set methods
	proto native void SetSlot (int slotId);
	proto native void SetIndex (int idx);
	proto native void SetRow (int row);
	proto native void SetCol (int col);
	proto native void SetFlip (bool flip);

	/**
	 * @fn		Reset
	 **/
	proto native void Reset ();
	
	proto native bool CompareLocationOnly (notnull InventoryLocation other);

	/**
	 * @fn		CollidesWith
	 * @brief	checks if inventory locations collides each with other
	 **/
	proto native bool CollidesWith (notnull InventoryLocation rhs);

	/**
	 * @fn		Copy
	 * @brief	copies location data to another location
	 *
	 * @NOTE	it copies all data (except enforce internal data)
	 **/
	proto native InventoryLocation Copy (notnull InventoryLocation rhs);
	/**
	 * @fn		CopyLocationFrom
	 * @brief	copies location to another location without m_item member
	 *
	 * @NOTE	it does NOT do a copy of m_item member!
	 **/
	proto native InventoryLocation CopyLocationFrom (notnull InventoryLocation rhs, bool copyFlip);
	
	static string DumpToStringNullSafe (InventoryLocation loc)
	{
		if (loc)
			return loc.DumpToString();
		return "{ null }";
	}
	
	string DumpToString ()
	{
		string res = "{ type=" + typename.EnumToString(InventoryLocationType, GetType());
		switch (GetType())
		{
			case InventoryLocationType.UNKNOWN:
			{
				break;
			}
			case InventoryLocationType.GROUND:
			{
				res = res + " item=" + Object.GetDebugName(GetItem());
				vector pos = GetPos();
				float dir[4];
				GetDir(dir);
				res = res + " pos=(" + pos[0] + ", " + pos[1] + ", " + pos[2] + ")";
				res = res + " dir=(" + dir[0] + ", " + dir[1] + ", " + dir[2] + ", " + dir[3] + ")";
				break;
			}
			case InventoryLocationType.ATTACHMENT:
			{
				res = res + " item=" + Object.GetDebugName(GetItem());
				res = res + " parent=" + Object.GetDebugName(GetParent());
				res = res + " slot=" + GetSlot();
				break;
			}
			case InventoryLocationType.CARGO:
			{
				res = res + " item=" + Object.GetDebugName(GetItem());
				res = res + " parent=" + Object.GetDebugName(GetParent());
				res = res + " idx=" + GetIdx() + " row=" + GetRow() + " col=" + GetCol() + " f=" + GetFlip();
				break;
			}
			case InventoryLocationType.HANDS:
			{
				res = res + " item=" + Object.GetDebugName(GetItem());
				res = res + " parent=" + Object.GetDebugName(GetParent());
				break;
			}
			case InventoryLocationType.PROXYCARGO:
			{
				res = res + " item=" + Object.GetDebugName(GetItem());
				res = res + " parent=" + Object.GetDebugName(GetParent());
				res = res + " idx=" + GetIdx() + " row=" + GetRow() + " col=" + GetCol() + " f=" + GetFlip();
				break;
			}
			default:
			{
				res = res + "??";
				break;
			}
		}
		res = res + " }";
		return res;
	}

	bool ReadFromContext (ParamsReadContext ctx)
	{
		EntityAI parent;
		EntityAI item;
		int type = 0;
		int idx = -1;
		int row = -1;
		int col = -1;
		bool flp = false;
		if (!ctx.Read(type))
			return false;

		switch (type)
		{
			case InventoryLocationType.UNKNOWN:
			{
				break;
			}
			case InventoryLocationType.GROUND:
			{
				if (!ctx.Read(item))
					return false;
				vector pos;
				if (!ctx.Read(pos))
					return false;

				float dir[4];
				if (!ctx.Read(dir))
					return false;
				
				if (!item)
					break; // item is not in bubble

				SetGroundEx(item, pos, dir);
				break;
			}
			case InventoryLocationType.ATTACHMENT:
			{
				if (!ctx.Read(parent))
					return false;
				if (!ctx.Read(item))
					return false;
				int slot;
				if (!ctx.Read(slot))
					return false;
				
				if (!parent || !item)
					break; // parent or item is not in bubble

				SetAttachment(parent, item, slot);
				break;
			}
			case InventoryLocationType.CARGO:
			{
				if (!ctx.Read(parent))
					return false;
				if (!ctx.Read(item))
					return false;
				if (!ctx.Read(idx))
					return false;
				if (!ctx.Read(row))
					return false;
				if (!ctx.Read(col))
					return false;
				if (!ctx.Read(flp))
					return false;
				
				if (!parent || !item)
					break; // parent or item is not in bubble

				SetCargo(parent, item, idx, row, col, flp);
				break;
			}
			case InventoryLocationType.HANDS:
			{
				if (!ctx.Read(parent))
					return false;
				if (!ctx.Read(item))
					return false;
				
				if (!parent || !item)
					break; // parent or item is not in bubble

				SetHands(parent, item);
				break;
			}
			case InventoryLocationType.PROXYCARGO:
			{
				if (!ctx.Read(parent))
					return false;
				if (!ctx.Read(item))
					return false;
				if (!ctx.Read(idx))
					return false;
				if (!ctx.Read(row))
					return false;
				if (!ctx.Read(col))
					return false;
				if (!ctx.Read(flp))
					return false;
				
				if (!parent || !item)
					break; // parent or item is not in bubble

				SetProxyCargo(parent, item, idx, row, col, flp);
				break;
			}
			default:
			{
				Error("ReadFromContext - really unknown location type, this should not happen, type=" + type);
				return false;
			}
		}
		return true;
	}

	bool WriteToContext (ParamsWriteContext ctx)
	{
		if (!ctx.Write(GetType()))
		{
			Error("InventoryLocation::WriteToContext - cannot write to context! failed to write type");
			return false;
		}

		switch (GetType())
		{
			case InventoryLocationType.UNKNOWN:
			{
				break;
			}
			case InventoryLocationType.GROUND:
			{
				if (!ctx.Write(GetItem()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed GND, arg=item");
					return false;
				}

				vector pos = GetPos();
				if (!ctx.Write(pos))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed GND, arg=pos");
					return false;
				}

				float dir[4];
				GetDir(dir);
				if (!ctx.Write(dir))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed GND, arg=dir");
					return false;
				}

				break;
			}
			case InventoryLocationType.ATTACHMENT:
			{
				if (!ctx.Write(GetParent()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed ATT, arg=parent");
					return false;
				}
				if (!ctx.Write(GetItem()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed ATT, arg=item");
					return false;
				}
				if (!ctx.Write(GetSlot()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed ATT, arg=slot");
					return false;
				}
				break;
			}
			case InventoryLocationType.CARGO:
			{
				if (!ctx.Write(GetParent()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed CGO, arg=parent");
					return false;
				}
				if (!ctx.Write(GetItem()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed CGO, arg=item");
					return false;
				}
				if (!ctx.Write(GetIdx()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed CGO, arg=idx");
					return false;
				}
				if (!ctx.Write(GetRow()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed CGO, arg=row");
					return false;
				}
				if (!ctx.Write(GetCol()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed CGO, arg=col");
					return false;
				}
				if (!ctx.Write(GetFlip()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed CGO, arg=flp");
					return false;
				}
				break;
			}
			case InventoryLocationType.HANDS:
			{
				if (!ctx.Write(GetParent()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed HND, arg=parent");
					return false;
				}
				if (!ctx.Write(GetItem()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed HND, arg=item");
					return false;
				}
				break;
			}
			case InventoryLocationType.PROXYCARGO:
			{
				if (!ctx.Write(GetParent()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed PXY, arg=parent");
					return false;
				}
				if (!ctx.Write(GetItem()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed PXY, arg=item");
					return false;
				}
				if (!ctx.Write(GetIdx()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed PXY, arg=idx");
					return false;
				}
				if (!ctx.Write(GetRow()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed PXY, arg=row");
					return false;
				}
				if (!ctx.Write(GetCol()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed PXY, arg=col");
					return false;
				}
				if (!ctx.Write(GetFlip()))
				{
					Error("InventoryLocation::WriteToContext - cannot write to context! failed PXY, arg=flp");
					return false;
				}

				break;
			}
			default:
			{
				Error("WriteToContext - really unknown location type, this should not happen, type=" + GetType());
				return false;
			}
		}
		return true;
	}
};

bool OptionalLocationWriteToContext (InventoryLocation loc, notnull ParamsWriteContext ctx)
{
	if (loc)
	{
		if (!ctx.Write(true))
		{
			Error("OptionalLocationWriteToContext - cannot write 1 to context!");
			return false;
		}
		return loc.WriteToContext(ctx);
	}
	else
	{
		if (!ctx.Write(false))
		{
			Error("OptionalLocationWriteToContext - cannot write 0 to context!");
			return false;
		}
	}
	return true;
}

bool OptionalLocationReadFromContext (out InventoryLocation loc, notnull ParamsReadContext ctx)
{
	bool present = false;
	if (!ctx.Read(present))
	{
		Error("OptionalLocationReadFromContext - cannot read bool from context!");
		return false;
	}
	
	if (!present)
		return true;

	loc = new InventoryLocation;
	if (!loc.ReadFromContext(ctx))
	{
		Error("OptionalLocationReadFromContext - cannot read (present) inventorylocation from context!");
		return false;
	}
	return true;
}
