class SyncPlayer
{
	PlayerIdentity m_Identity;

	//! Keeping for backwards compatability with mods. 

	[NonSerialized()]
	string m_UID;

	[NonSerialized()]
	string m_PlayerName;
}