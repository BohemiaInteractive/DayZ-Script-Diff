class DeveloperFreeCamera
{
	static void OnInit()
	{
	}
	
	//! Enable / Disable Free camera (Fly mod) - disable of camera will teleport player at current free camera position.
	static void FreeCameraToggle( PlayerBase player, bool teleport_player = false )
	{
		if ( !IsFreeCameraEnabled() )
		{
			EnableFreeCamera( player );
		}
		else 
		{
			DisableFreeCamera( player, teleport_player );
		}
	}
	
	static void EnableFreeCameraSceneEditor( PlayerBase player )
	{
		EnableFreeCamera(player);	
	}
	
	//! Is free camera enabled
	static bool IsFreeCameraEnabled()
	{
		return FreeDebugCamera.GetInstance().IsActive();
	}
	
	//---------------------------------------------
	// EnableFreeCamera
	//---------------------------------------------
	protected static void EnableFreeCamera( PlayerBase player )
	{
		if ( player )
		{
			FreeDebugCamera.GetInstance().SetActive(true);

			#ifdef DEVELOPER
			GetGame().GetMission().RefreshCrosshairVisibility();
			//GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(GetGame().GetMission(), "RefreshCrosshairVisibility", NULL);
			#endif
		}
		else
		{
			Print("EnableFreeCamera( PlayerBase player ) -> player is NULL");
			Debug.LogError("EnableFreeCamera( PlayerBase player ) -> player is NULL", "DeveloperFreeCamera");
		}
	}

	//---------------------------------------------
	// DisableFreeCamera
	//---------------------------------------------
	static void DisableFreeCamera( PlayerBase player, bool teleport_player = false )
	{
		if ( player )
		{
			if ( FreeDebugCamera.GetInstance().IsActive() )
			{
				if (teleport_player)
					DeveloperTeleport.TeleportAtCursorEx();
				FreeDebugCamera.GetInstance().SetActive(false);

				#ifdef DEVELOPER
				if ( GetGame().GetMission() )
				{
					GetGame().GetMission().RefreshCrosshairVisibility();
				}
				#endif
				//GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(GetGame().GetMission(), "RefreshCrosshairVisibility", NULL);
			}
		}
		else
		{
			Debug.LogError("DisableFreeCamera( PlayerBase player, bool teleport_player ) -> player is NULL", "DeveloperFreeCamera");
		}
	}
	
	//---------------------------------------------
	// OnSetFreeCameraEvent
	//---------------------------------------------
	static void OnSetFreeCameraEvent( PlayerBase player, FreeDebugCamera camera )
	{
		if ( player )
		{
			#ifdef DEVELOPER
			if ( !GetGame().IsMultiplayer() )
			{
				PluginSceneManager plugin_scene_manager = PluginSceneManager.Cast( GetPlugin(PluginSceneManager) );
				
				if ( plugin_scene_manager.IsOpened() )
				{
					plugin_scene_manager.EditorToggle();
				}
			}
			
			
			GetGame().GetMission().RefreshCrosshairVisibility();
			//GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(GetGame().GetMission(), "RefreshCrosshairVisibility", NULL);
			#endif
		}
	}
}