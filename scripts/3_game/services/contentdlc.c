
// Binded values from engine
enum EDLCId
{
	DLC_UNKNOWN,
	DLC_BLISS,
};

//! ContentDLC is for query installed DLC (only as entitlement keys, not content)
class ContentDLC
{
	/*!
		For getting current entitlements on device (console). Could be called 
		after any entitlement change or before accessing locked content.
		\param dlcIdList through this list is returned all installed DLC with valid license for current user
		\return count of installed and valid DLCs.
	*/
	proto native int GetEntitlements(out TIntArray dlcIdList);
	
	/*!
		\param dlcId ID of DLC
		\return current state of availability of unlocking' dlc. Only DLC with valid license return true.
	*/
	proto native bool IsDLCInstalled(EDLCId dlcId);
	
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
	
	/*!
		Called from engine for notifing script after any change in additional 
		content (installed, lost license). There should be check for current state of entitlements.
		On Xbox should be problem with event lost license! (system bug)
		\param dlcId ID of DLC that was changed. Could be empty string if console does not report concrete DLC ID.
	*/
	void OnChange(EDLCId dlcId)
	{

	}
};
