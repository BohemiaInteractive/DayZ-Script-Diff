
// Binded values from engine
enum EDLCId
{
	UNKNOWN,
	FROSTLINE,
	BADLANDS,
};

enum EDLCChange
{
	UNKNOWN,
	AVAILABLE,
	UNAVAILABLE,
};

//! ContentDLC is for query installed DLC (only as entitlement keys, not content)
class ContentDLC
{
	private void ContentDLC();
	private void ~ContentDLC();
	
	//! Subscribe to changes: void() OR void(EDLCId) OR void(EDLCId,EDLCChange)
	proto void Subscribe(func fnc);
	proto void Unsubscribe(func fnc);
	
	//! Check if a DLC ID is valid
	static proto native bool IsValidDLCID(EDLCId dlcId);
	
	/*!
		For getting current entitlements on device (console). Could be called 
		after any entitlement change or before accessing locked content.
		\param dlcIdList through this list is returned all installed DLC with valid license for current user
		\return count of installed and valid DLCs.
	*/
	proto native int GetEntitlements(out TIntArray dlcIdList);
	
	/*!
		\param dlcId ID of DLC
		\return current state of availability of dlc. Only DLC with valid license which has been installed returns true.
	*/
	proto bool IsDLCInstalled(EDLCId dlcId);
	
	/*!
		\param dlcId ID of DLC
		\return state of mount of DLC on console/device for current user (a DLC must be in the mounted state to use it)
	*/
	proto bool IsDLCMounted(EDLCId dlcId);
	
	/*!
		\param dlcId ID of DLC
		\return attempt to use a DLC, DLC needs to be installed and mounted, tries to mount if possible
	*/
	proto bool UseDLC(EDLCId dlcId);

	/*!
		Opens the platform store page for the DLC. The player is expected to be
		informed about the missing DLC before this is called.
		\param dlcId ID of DLC
		\return true when the store page was opened
	*/
	proto bool BuyDLC(EDLCId dlcId);

	/*!
		\param world World name
		\return ID of DLC which this world belongs to, otherwise EDLCId.DLC_UNKNOWN
	*/
	proto EDLCId GetDLCFromWorld(string world);

	/*!
		\param dlcId ID of DLC
		\return localized name of the DLC, placeholder text for an unknown DLC
	*/
	proto string GetLocalizedName(EDLCId dlcId);
	
	bool OwnsAllDLC()
	{
		const int length = EnumTools.GetEnumSize(EDLCId);
		
		// Start at 1, because 0 is DLC_UNKNOWN
		for (int i = 1; i < length; ++i)
		{
			if (!IsDLCInstalled(EnumTools.GetEnumValue(EDLCId, i)))
				return false;
		}
		
		return true;
	}
};
