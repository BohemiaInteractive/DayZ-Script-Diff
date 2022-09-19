//!	BiosClientServices class provides individual online services.
/*!
	Bohemia interactive online services (Bios), Client services API.
 	Use the individual Get methods to access individual service providers.
	Do not store references to individual service providers, use the Get methods instead.
	Temporary storage in a function body is fine as long as the owning BiosClientServices object is referenced.
	Do not attempt to construct this object. It is currently provided by the engine.
 */
class BiosClientServices
{
	private void BiosClientServices() {}
	void ~BiosClientServices()
	{
	}
	
	proto native BiosPrivacyService GetPrivacyService();
	proto native BiosSocialService GetSocialService();
	proto native BiosLobbyService GetLobbyService();
	proto native BiosSessionService GetSessionService();
	proto native BiosPackageService GetPackageService();
};
