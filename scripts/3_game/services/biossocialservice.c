
//! BiosFriendInfo represents friend information

class BiosFriendInfo
{
	string	m_Uid;			//!< The Uid of the friend.
	string	m_DisplayName;	//!< The Displayable nick name of the friend.
	bool	m_IsFavorite;	//!< Xbox: Is a favorite friend?
	bool	m_IsFollowed;	//!< Xbox: Is this a two-way friendship?
	
	static bool Compare( BiosFriendInfo a, BiosFriendInfo b )
	{
		return ( a.m_Uid == b.m_Uid && a.m_DisplayName == b.m_DisplayName && a.m_IsFavorite == b.m_IsFavorite && a.m_IsFollowed == b.m_IsFollowed );
	}
};

typedef array<ref BiosFriendInfo> BiosFriendInfoArray;

//! BiosSocialService is used to query friend list and other social features for the current user.
/*!
	Todo: report friend changes
*/
class BiosSocialService
{
	//! Display small system UI profile for the target.
	/*!
		Xbox: The async result is returned in the OnPermissionsAsync callback.
			
		@param uid_target target user Uid's for which to display profile.
		@return EBiosError indicating if the async operation is pending.		
	*/
	proto native EBiosError ShowUserProfileAsync(string uid_target);
	
	//! Query for friends list
	/*!
		The async result is returned in the OnFriendsAsync callback.
		
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError GetFriendsAsync();

	//! Async callback for ShowUserProfileAsync
	/*!
		@param error error indicating success or fail of the async operation.
			Xbox: OK - the user displayed the fullscreen profile
			Xbox: CANCEL - the user closed the small profile.
	*/
	void OnUserProfileAsync(EBiosError error)
	{
		OnlineServices.OnUserProfileAsync( error );
	}
	
	//! Async callback for GetFriendsAsync
	/*!
		@param friend_list list of BiosFriendInfo for each friend. NULL if failed.
		@param error error indicating success or fail of the async operation.
	*/
	void OnFriendsAsync(BiosFriendInfoArray friend_list, EBiosError error)
	{
		OnlineServices.OnFriendsAsync( friend_list, error );
	}
};
