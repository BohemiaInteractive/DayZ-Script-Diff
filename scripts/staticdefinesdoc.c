#ifdef DOXYGEN
/**
| %Defines                                                             | Short description                                                                                 | DayZ builds                                                                                                        | %Workbench builds |
| -------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------ | ----------------- |
| DEVELOPER                                                            | Internal developer version                                                                        | yes (binarize, debug, release, releaseAsserts, releaseNoOpt)\n no(Profile server, Retail server, Retail client)    | yes               |
| RELEASE                                                              | Retail build of the game                                                                          | yes (retail client, retail server)                                                                                 | no                |
| PREVIEW_BUILD                                                        | Preview build. Currently used only in console builds                                              | yes (preview build)                                                                                                | no                |
| PLATFORM_WINDOWS \|\| PLATFORM_MACOSX \|\| ENF_LINUX                 | Platform specific defines                                                                         | yes                                                                                                                | yes               |
| PLATFORM_CONSOLE && (PLATFORM_XBOX \|\| PLATFORM_PS4)                | Platform specific defines                                                                         | yes                                                                                                                | no                |
| X1_TODO_TEMP_GUI                                                     | Temporary XBOX GUI hacks                                                                          | yes (xbox builds)                                                                                                  | no                |
| BULDOZER                                                             | Game is launched in land editor mode (-buldozer)                                                  | yes (binarize, debug, release, releaseAsserts, releaseNoOpt, retail client)\n no(server, retail server, xbox, ps4) | yes               |
| SERVER                                                               | Game runs as server                                                                               | yes (all pc builds, client with -server)                                                                           | no                |
| NO_GUI, NO_GUI_INGAME                                                | Game runs without gui - Server, client simulator                                                  | yes                                                                                                                | no                |
| WORKBENCH                                                            | Game runs from workbench                                                                          | no                                                                                                                 | yes               |
| COMPONENT_SYSTEM                                                     | Enfusion entity component system                                                                  | no                                                                                                                 | yes               |
| GAME_TEMPLATE                                                        | Game template implementation see [link](https://confluence.bistudio.com/display/EN/Game+Template) | no                                                                                                                 | yes               |
| ENF_DONE                                                             | Looks like commented out wip or dead code                                                         | no                                                                                                                 | no                |
| DOXYGEN                                                              | Doxygen documentation, never compiled                                                             | no                                                                                                                 | no                |
| _DAYZ_CREATURE_DEBUG_SHADOW                                          | Game is built with network debug for DayZCreature                                                 | no                                                                                                                 | no                |
| BOT                                                                  | ? Script player debug bot ?                                                                       | no                                                                                                                 | no                |
| PS3                                                                  | ? obsolete ?                                                                                      | no                                                                                                                 | no                |

| PC builds                     | %Defines used                                                                                                                                                                                                                                                                                                    |
| ----------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| release, releaseAsserts       | DEVELOPER, (PLATFORM_WINDOWS \|\| PLATFORM_MACOSX \|\| ENF_LINUX), (-server => SERVER, NO_GUI, NO_GUI_INGAME), (-buldozer => BULDOZER) + any number of static defines using command line parameter -scrDef=... see [link](https://confluence.bistudio.com/display/DAYZ/DayZ+Development+Command-line+Parameters) |
| retail client                 | RELEASE, (PLATFORM_WINDOWS \|\| PLATFORM_MACOSX \|\| ENF_LINUX), (-buldozer => BULDOZER)                                                                                                                                                                                                                         |
| retail server, profile server | RELEASE, (PLATFORM_WINDOWS \|\| PLATFORM_MACOSX \|\| ENF_LINUX), SERVER, NO_GUI, NO_GUI_INGAME                                                                                                                                                                                                                   |

| Console builds | %Defines used                                                               |
| -------------- | --------------------------------------------------------------------------- |
| preview        | RELEASE, PLATFORM_CONSOLE, (PLATFORM_XBOX \|\| PLATFORM_PS4), PREVIEW_BUILD |
| retail         | RELEASE, PLATFORM_CONSOLE, (PLATFORM_XBOX \|\| PLATFORM_PS4)                |
| release        | DEVELOPER, PLATFORM_CONSOLE, (PLATFORM_XBOX \|\| PLATFORM_PS4)              |
*/

enum StaticDefines
{
	DEVELOPER,
	RELEASE,
	PREVIEW_BUILD,
	PLATFORM_WINDOWS,
	PLATFORM_MACOSX,
	ENF_LINUX,
	PLATFORM_CONSOLE,
	PLATFORM_XBOX,
	PLATFORM_PS4,
	X1_TODO_TEMP_GUI,
	BULDOZER,
	SERVER,
	NO_GUI, 
	NO_GUI_INGAME,
	WORKBENCH,
	GAME_TEMPLATE,
	ENF_DONE,
	COMPONENT_SYSTEM,
	DOXYGEN,
	_DAYZ_CREATURE_DEBUG_SHADOW,
	BOT,
	PS3
};
#endif