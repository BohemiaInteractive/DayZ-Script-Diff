/**@class	CargoBase
 * @brief	represents base for cargo storage for entities
 *
 * @NOTE: rows == y axis
 **/
class CargoBase : Managed
{
	/**@fn			GetCargoOwner
	 * @brief		get the entity that owns the cargo
	 * @return		cargo owner
	 **/
	proto native EntityAI GetCargoOwner ();

	/**@fn			GetCargoOwnerIndex
	 * @brief		get index of this cargo in the entity that owns the cargo
	 * @return		owner's index of this cargo
	 **/
	proto native int GetOwnerCargoIndex ();

	/**@fn			IsProxyCargo
	 * @return		true if cargo is in proxy object
	 **/	
	proto native bool IsProxyCargo ();
	
	/**@fn			GetItemCount
	 * @return		number of items in cargo
	 **/
	proto native int GetItemCount ();
	/**@fn			GetItem
	 * @return		get item at specific index
	 **/
	proto native EntityAI GetItem (int index);

	/**@fn			GetWidth
	 * @return		width of the cargo
	 **/
	proto native int GetWidth ();
	/**@fn			GetHeight
	 * @return		height of the cargo
	 **/
	proto native int GetHeight ();
	/**@fn			GetItemRowCol
	 * @param[out]	row			returned row of the item at internal index
	 * @param[out]	col			returned col. 0 on xbox
	 **/
	proto bool GetItemRowCol (int index, out int row, out int col);
	/**@fn			GetItemSize
	 * @param[out]	w			returned width of the item at internal index
	 * @param[out]	h			returned height of the item at internal index
	 **/
	proto bool GetItemSize (int index, out int w, out int h);

	/**@fn			FindEntityInCargo
	 * @return		find internal index of the entity in cargo or -1 if not found
	 **/
	proto native int FindEntityInCargo (notnull EntityAI e);

	private void CargoBase ();
	private void ~CargoBase ();
	
	/**@fn			CanReceiveItemIntoCargo
	 * @brief		condition EntityAI::CanReceiveItemIntoCargo for Cargo.
	 * @return		true if cargo can be added, false otherwise
	 **/
	bool CanReceiveItemIntoCargo (EntityAI item) { return true; }

	/**@fn			CanSwapItemInCargo
	 * @brief		condition EntityAI::CanSwapItemInCargo for Cargo.
	 * @return		true if cargo can be added, false otherwise
	 **/
	bool CanSwapItemInCargo (EntityAI child_entity, EntityAI new_entity) { return true; }
	
	proto native int GetUserReservedLocationCount ();
	/**@fn			GetUserReservedLocation
	 * @param[in]	idx			index of the user reserved location
	 * @param[out]	eai			returned entity of the user reservation at internal index
	 * @param[out]	row			returned row of the user reservation at internal index
	 * @param[out]	col			returned col
	 * @param[out]	w			returned width of the user reservation at internal index
	 * @param[out]	h			returned height of the user reservation at internal index
	 * @param[out]	flp			returned flip
	 * @return		true if record found, false otherwise
	 **/
	proto bool GetUserReservedLocation (int index, out EntityAI eai, out int row, out int col, out int w, out int h, out int flp);
	proto native void SetUserReservedLocation (notnull EntityAI eai);
	proto native void ClearUserReservedLocation (notnull EntityAI eai);
};


#ifdef PLATFORM_CONSOLE
	class CargoList : CargoBase
	{
		/**@fn			GetMaxWeight
		 * @return		maximum weight that the cargo can hold
		 **/
		proto native int GetMaxWeight ();
	
		/**@fn			GetTotalWeight
		 * @brief		sums weight of all items in cargo and adds weight of item if item != null
		 * @return		sum of weights plus weight of item (if !null)
		 **/
		proto native int GetTotalWeight (EntityAI item);

		/**@fn			CanFitItemIntoCargo
		 * @return		true if adding item does not exceed GetMaxWeight, false otherwise
		 **/		
		proto native bool CanFitItemIntoCargo (EntityAI cargo);
	
		/**@fn			CanReceiveItemIntoCargo
		 * @return		true if adding item does not exceed GetMaxWeight, false otherwise
		 **/
		override bool CanReceiveItemIntoCargo (EntityAI item)
		{
			return true;
			//return CanFitItemIntoCargo(item);
		}
	
		/**@fn			CanFitSwappedItemInCargo
		 * @return		true if swapping item does not exceed GetMaxWeight, false otherwise
		 **/		
		proto native bool CanFitSwappedItemInCargo (EntityAI child_entity, EntityAI new_entity);
	
		override bool CanSwapItemInCargo (EntityAI child_entity, EntityAI new_entity)
		{
			return CanFitSwappedItemInCargo(child_entity, new_entity);
		}
	};
#else
#ifdef SERVER_FOR_CONSOLE
	class CargoList : CargoBase
	{
		/**@fn			GetMaxWeight
		 * @return		maximum weight that the cargo can hold
		 **/
		proto native int GetMaxWeight ();
	
		/**@fn			GetTotalWeight
		 * @brief		sums weight of all items in cargo and adds weight of item if item != null
		 * @return		sum of weights plus weight of item (if !null)
		 **/
		proto native int GetTotalWeight (EntityAI item);

		/**@fn			CanFitItemIntoCargo
		 * @return		true if adding item does not exceed GetMaxWeight, false otherwise
		 **/		
		proto native bool CanFitItemIntoCargo (EntityAI cargo);
	
		/**@fn			CanReceiveItemIntoCargo
		 * @return		true if adding item does not exceed GetMaxWeight, false otherwise
		 **/
		override bool CanReceiveItemIntoCargo (EntityAI item)
		{
			return true; 
			//return CanFitItemIntoCargo(item);
		}
	
		/**@fn			CanFitSwappedItemInCargo
		 * @return		true if swapping item does not exceed GetMaxWeight, false otherwise
		 **/		
		proto native bool CanFitSwappedItemInCargo (EntityAI child_entity, EntityAI new_entity);
	
		override bool CanSwapItemInCargo (EntityAI child_entity, EntityAI new_entity)
		{
			return CanFitSwappedItemInCargo(child_entity, new_entity);
		}
	};
#else
	class CargoGrid : CargoBase
	{
		/**@fn			FindEntityInCargoOn
		 * @return		get item at coordinates (row, col). col is 0 on xbox.
		 **/
		proto native EntityAI FindEntityInCargoOn (int row, int col);
	};
#endif
#endif
