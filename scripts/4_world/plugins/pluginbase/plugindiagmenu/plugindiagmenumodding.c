/**
 * \defgroup Modding Modding DiagMenu
 *	\warning Only available on developer and diag builds
 *	\note This file is extensive documentation about modding DiagMenu, please read EVERYTHING before starting
 * @{
 */

#ifdef DOXYGEN
#ifdef MODDING_TEST
#ifdef DIAG_DEVELOPER

/**
\brief Example of adding DiagMenu entries by modders
	\note PluginDiagMenu is the base of ..Client and ...Server, which is why we are registering the IDs on this level, so they exist on both Client and Server
	\note Keep in mind that in SinglePlayer missions, both Client and Server PluginDiagMenu are created
	\note Keep in mind that PluginDiagMenu... is destroyed and recreated every time the world changes
*/
#ifdef DOXYGEN
class PluginDiagMenuModded // Just to not have it show up for the regular entry... Doxygen doesn't know about modding
#else
modded class PluginDiagMenu
#endif
{	
	/**
	\brief The name of the root menu where all debugs of this mod will be placed in
		\note Try to think of a name which will make it easy to identify as originating from your mod
		\note The 'maximum' length of the name of an entry is 24 characters (excess will be cut off when rendering)
		\note The hardcap length of the name of an entry is 64 characters
	*/
	protected string m_ModdedDiagsExampleRootMenu = "BI - DiagsModdingExample";
	
	/**
	\brief The name of an example sub menu
	*/
	protected string m_ModdedDiagsExampleSubMenu = "Example Sub Menu";
	
	/** \name Modded Diag IDs
	*	To prevent mod conflicts, a system has been set up to create unique IDs for the modded Diags
	*	These are then best saved to a variable so that they can be used by the other DiagMenu functions
	*	!!! Remember to give them as unique as possible name, as something with a generic name could cause conflicts	
	*/
	//@{
	protected int m_ModdedDiagsExampleRootMenuID;
		protected int m_ModdedDiagsExampleBoolID;
		protected int m_ModdedDiagsExampleSubMenuID;
			protected int m_ModdedDiagsExampleRangeID;
	//@}
	
	//---------------------------------------------
	/**
	\brief Obtain unique IDs and store them in variables
		\warning Please only call GetModdedDiagID when necessary, as every time it is called it will increment and Script has a limit of 512 Diag IDs
		\note Don't forget to call super!
	*/
	override protected void RegisterModdedDiagsIDs()
	{
		super.RegisterModdedDiagsIDs();
		
		m_ModdedDiagsExampleRootMenuID = GetModdedDiagID();
			m_ModdedDiagsExampleBoolID = GetModdedDiagID();
			m_ModdedDiagsExampleSubMenuID = GetModdedDiagID();
				m_ModdedDiagsExampleRangeID = GetModdedDiagID();
	}
	
	//---------------------------------------------
	/**
	\brief Register entries to the DiagMenu
		\note Please create a root menu for your mod which is then the single parent of all your diags, which is then a child of ModdedRootMenu
		\warning Read the in-file comments as well
		\warning Don't forget to call super! Mandatory at the beginning of the function so it can register the modded root menu!
	*/
	override protected void RegisterModdedDiags()
	{
		super.RegisterModdedDiags();
		
		// Register the root menu of your mod under the ModdedRootMenu
		// Then register all following menus and items under this menu
		// This can not be enforced, but it will help keep things clean
		// Including being able to easily identify where a debug is coming from
		// So that reports of a broken debug can be sent to the correct developer
		//
		// If you have multiple mods, you might even want to consider to create a root menu with your developer name
		// And then put the mod menus as a submenu
		// To prevent someone running a lot of mods from having an overflooded menu		
		// DiagMenu.MenuExists(...) could serve to help to identify if the root menu already exists when using this format
		// So that the multiple mods can know if they still have to register your root menu or not
		DiagMenu.RegisterMenu(m_ModdedDiagsExampleRootMenuID, m_ModdedDiagsExampleRootMenu, GetModdedRootMenu());
		{
			DiagMenu.RegisterBool(m_ModdedDiagsExampleBoolID, "", "Modded Example Bool", m_ModdedDiagsExampleRootMenuID);
			
			// A sub menu inside the root of the mod menu
			// The curly braces are simply for readability
			DiagMenu.RegisterMenu(m_ModdedDiagsExampleSubMenuID, m_ModdedDiagsExampleSubMenu, m_ModdedDiagsExampleRootMenuID);
			{
				DiagMenu.RegisterRange(m_ModdedDiagsExampleRangeID, "", "Modded Example Range", m_ModdedDiagsExampleSubMenuID, "3 9 6 3");
			}
		}
	}
}

#ifdef DOXYGEN
class DummyDoxygenClass
{
	int m_IgnoreThisVariable;
}
#endif

/**
\brief Example of adding DiagMenu functionality by modders
	\note DiagMenu is mostly designed to run on Client, which is why the decision was made to keep most of the functionality for only the Client
	\note This way the callbacks are only registered on Client and not on Server, to prevent mishaps
*/
#ifdef DOXYGEN
class PluginDiagMenuClientModded // Just to not have it show up for the regular entry... Doxygen doesn't know about modding
#else
modded class PluginDiagMenuClient
#endif
{
	/**
	\brief Bind callbacks to the entries to immediately perform an action when the value is changed
		\note Don't forget to call super!
	*/
	override protected void BindCallbacks()
	{
		super.BindCallbacks();

		DiagMenu.BindCallback(m_ModdedDiagsExampleBoolID, CBModdedDiadIDsExampleBool);
		DiagMenu.BindCallback(m_ModdedDiagsExampleRangeID, CBModdedDiadIDsExampleRange);
	}
	
	/**
	\brief Example callback for RegisterBool
		\note Give it a unique name to prevent conflicts
		\note These MUST be static and return void
	@code
		// Valid RegisterBool callback signatures (Technically 'bool' can be 'int' as well, so be mindful of that)
		//	o static void Callback();
		//	o static void Callback(bool value);
		//	o static void Callback(bool value, int id);
	@endcode
	*/
	static void CBModdedDiadIDsExampleBool(bool enabled)
	{
		Print("CBModdedDiadIDsExampleBool: " + enabled);
	}
	
	/**
	\brief Example callback for RegisterRange
		\note Give it a unique name to prevent conflicts
		\note These MUST be static and return void
	@code
		// Valid RegisterRange callback signatures (Technically 'int' can be 'bool' as well, so be mindful of that)
		//	o static void Callback();
		//	o static void Callback(float value);
		//	o static void Callback(float value, int id);
		//	o static void Callback(int value);
		//	o static void Callback(int value, int id);
	@endcode
	*/
	static void CBModdedDiadIDsExampleRange(float value)
	{
		Print("CBModdedDiadIDsExampleRange: " + value);
	}
}

#endif
#endif
#endif

//@}