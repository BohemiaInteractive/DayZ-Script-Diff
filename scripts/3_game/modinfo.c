class ModInfo
{
	proto owned string GetName();
	proto owned string GetPicture();
	proto owned string GetLogo();
	proto owned string GetLogoSmall();
	proto owned string GetLogoOver();
	proto owned string GetTooltip();
	proto owned string GetOverview();
	proto owned string GetAction();
	proto owned string GetAuthor();
	proto owned string GetVersion();
	proto bool GetDefault();
	proto bool GetIsDLC();
	proto bool GetIsOwned();
	proto void GoToStore();
}