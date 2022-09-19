//!	EBiosPrivacyPermission represents possible privacy permissions
/*!
	See individual values for platform support and meaning.
*/
enum EBiosPrivacyPermission
{
	COMMUNICATE_VOICE,		//!< Xbox: represents CommunicateUsingVoice permissions, see Xbox Docs.
	COMMUNICATE_TEXT,		//!< Xbox: represents CommunicateUsingText permissions, see Xbox Docs.
	VIEW_PROFILE,			//!< Xbox: represents ViewTargetProfile permissions, see Xbox Docs.
	VIEW_PRESENCE,			//!< Xbox: represents ViewTargetPresence permissions, see Xbox Docs.
};

//!	EBiosPrivacyPrivilege represents possible privacy privileges
/*!
	See individual values for platform support and meaning.
*/
enum EBiosPrivacyPrivilege
{
	COMMUNICATE_VOICE,		//!< Xbox: represents XPRIVILEGE_COMMUNICATION_VOICE_INGAME privilege, see Xbox Docs.
	COMMUNICATE_TEXT,		//!< Xbox: represents XPRIVILEGE_COMMUNICATIONS privilege, see Xbox Docs.
	MULTIPLAYER_GAMEPLAY,	//!< Xbox: represents XPRIVILEGE_MULTIPLAYER_SESSIONS privilege, see Xbox Docs.
	MULTIPLAYER_LOBBY,		//!< Xbox: represents XPRIVILEGE_MULTIPLAYER_PARTIES privilege, see Xbox Docs.
};

//! BiosPrivacyPermissionResult represents the per permission result of the GetPermissionsAsync reqeust.

class BiosPrivacyPermissionResult
{
	EBiosPrivacyPermission	m_Permission;	//!< The reqeusted permission.
	bool					m_IsAllowed;	//!< Result.
	
	static bool Compare( BiosPrivacyPermissionResult a, BiosPrivacyPermissionResult b )
	{
		return ( a.m_Permission == b.m_Permission && a.m_IsAllowed == b.m_IsAllowed );
	}
};


typedef array<ref BiosPrivacyPermissionResult> BiosPrivacyPermissionResultArray;

//! BiosPrivacyUidResult represents the per user result of the GetPermissionsAsync request.

class BiosPrivacyUidResult
{
	string									m_Uid;			//!< Uid of the target user.
	ref BiosPrivacyPermissionResultArray	m_Results;		//!< Array of permission results for this target user.
};

typedef array<ref BiosPrivacyUidResult> BiosPrivacyUidResultArray;

//! BiosPrivacyService is used to query privacy permissions for a target user/s.

class BiosPrivacyService
{
	//! Query for privacy permissions
	/*!
		The async result is returned in the OnPermissions callback.
		Expected errors:
			BAD_PARAMETER -	if atleast one of the permissions is not supported on the current platform, 
							or atleast one of the input arrays is NULL.
			
		@param uid_list list of target user Uid's for which to query privacy permissions.
		@param permission_list list of requested permissions for each target user.
		@return EBiosError indicating if the async operation is pending.
		
	*/
	proto native EBiosError GetPermissionsAsync(array<string> uid_list, array<EBiosPrivacyPermission> permission_list);

	//! Query for privileges
	/*!
		The async result is returned in the OnPrivilege callback.
		Expected errors:
			BAD_PARAMETER -	if atleast one of the privileges is not supported on the current platform, 
			PURCHASE_REQUIRED,
			BANNED,
			NOT_ALLOWED - the privilege is granted,
			
		@param privilege the requested privilege.
		@param tryResolution Xbox: show system GUI with error msg and attempts to resolve it if possible.
		@return EBiosError indicating if the async operation is pending.
		
	*/
	proto native EBiosError GetPrivilegeAsync(EBiosPrivacyPrivilege privilege, bool try_resolution);
	
	//! Async callback for GetPermissionsAsync
	/*!
		@param result_list list of results for each requested user. NULL if failed.
		@param error error indicating success or fail of the async operation.
	
	*/
	void OnPermissions(BiosPrivacyUidResultArray result_list, EBiosError error)
	{
		if (result_list == null)
		{
			Print("biapi privacy error: " + error);
			return;
		}
				
		OnlineServices.OnPermissionsAsync( result_list, error );
	}
	
	//! Async callback for GetPrivilegeAsync
	/*!
		@param privilege the requested privilege.
		@param error error indicating success or fail of the async operation.
	*/
	void OnPrivilege(EBiosPrivacyPrivilege privilege, EBiosError error)
	{
		switch( privilege )
		{
			case EBiosPrivacyPrivilege.MULTIPLAYER_GAMEPLAY:
			{
				OnlineServices.OnLoadMPPrivilege( error );
				break;
			}
			case EBiosPrivacyPrivilege.COMMUNICATE_VOICE:
			{
				OnlineServices.OnLoadVoicePrivilege( error );
				break;
			}
		}
	}
};
