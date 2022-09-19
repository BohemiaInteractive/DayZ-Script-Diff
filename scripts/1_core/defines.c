// All defines in this file are added from C++ side
// It simply exists for documentation purposes
#ifdef DOXYGEN

/**
 * \defgroup Defines
 * \desc static defines
 * @{
 */

/*!
	\brief Define filled in with the current Major and Minor version (e.g. DAYZ_1_16)
*/
#define DAYZ_X_XX

/*!
	\brief Enabled when game is in Buldozer mode
*/
#define BULDOZER

/*!
	\note Present for server builds
*/
#define NO_GUI

/*!
	\note Present for server builds
*/
#define NO_GUI_INGAME



/**
 * \defgroup BuildDefines Build defines
 * \desc Defines for different builds
 * @{
 */

	/*!
		\brief Define present in Diag builds
	*/
	#define DIAG
	
	/*!
		\note A build is always either DEVELOPER or RELEASE, one is always active
	*/
	#define DEVELOPER
	
	/*!
		\note includes Diag
	*/
	#define RELEASE
	
	/*!
		\brief DIAG || DEVELOPER
	*/
	#define DIAG_DEVELOPER
	
	/*!
		\brief Define present in Experimental builds
	*/
	#define BUILD_EXPERIMENTAL

/** @}*/



/**
 * \defgroup ServerDefines Server defines
 * \desc Defines for dedicated server code
 * \note Only defined when CGame.IsDedicatedServer equals true
 * \note The platforms mentioned are the client platform, for game platform use PLATFORM defines
 * @{
 */

	/*!
		\brief Define always present on dedicated servers
		\note Should be preferred over using CGame.IsDedicatedServer when possible
	*/
	#define SERVER
	
	/*!
		\warning This is defined for Linux servers as well, as it targets Windows clients
		\note A server is always either FOR_WINDOWS, FOR_X1 or FOR_PS4
	*/
	#define SERVER_FOR_WINDOWS
	
	/*!
		\note A server is always either FOR_WINDOWS, FOR_X1 or FOR_PS4
	*/
	#define SERVER_FOR_X1
	
	/*!
		\note A server is always either FOR_WINDOWS, FOR_X1 or FOR_PS4
	*/
	#define SERVER_FOR_PS4
	
	/*!
		\brief SERVER_FOR_X1 || SERVER_FOR_PS4
	*/
	#define SERVER_FOR_CONSOLE

/** @}*/



/**
 * \defgroup PlatformDefines Platform defines
 * \desc Defines for platform the build is built for
 * @{
 */

	#define PLATFORM_LINUX

	#define PLATFORM_WINDOWS
	
	#define PLATFORM_XBOX
	
	#define PLATFORM_PS4
	
	/*!
		\brief PLATFORM_XBOX || PLATFORM_PS4
	*/
	#define PLATFORM_CONSOLE

/** @}*/

/** @}*/
#endif