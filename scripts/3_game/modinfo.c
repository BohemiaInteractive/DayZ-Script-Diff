class ModInfo
{
	proto owned string GetName();
	proto owned string GetPicture();
	proto owned string GetLogo();
	proto owned string GetLogoSmall();
	proto owned string GetLogoOver(); //hover-over logo variant
	proto owned string GetTooltip();
	proto owned string GetOverview(); //description
	proto owned string GetAction();
	proto owned string GetAuthor();
	proto owned string GetVersion();
	proto bool GetDefault();
	proto bool GetIsDLC();
	proto bool GetIsOwned();
	proto void GoToStore();
	
	static const string DEFAULT_PICTURE = "Gui/textures/modlogo_default_co.edds";
	static const string DEFAULT_LOGO = "Gui/textures/modlogo_default_co.edds";
	static const string DEFAULT_LOGO_SMALL = "Gui/textures/modlogo_default_co.edds";
	static const string DEFAULT_LOGO_OVER = "Gui/textures/modlogo_default_hover_co.edds";
	static const string DEFAULT_OVERVIEW = "";
}