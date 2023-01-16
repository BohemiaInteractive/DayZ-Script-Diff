class DeveloperFreeCamera
{
	static void OnInit();
	
	//! Enable / Disable Free camera (Fly mod) - disable of camera will teleport player at current free camera position.
	static void FreeCameraToggle(PlayerBase player, bool teleport_player = false)
	{
		if (!IsFreeCameraEnabled())
		{
			EnableFreeCamera(player);
		}
		else 
		{
			DisableFreeCamera(player, teleport_player);
		}
	}
	
	static void EnableFreeCameraSceneEditor(PlayerBase player)
	{
		EnableFreeCamera(player);	
	}
	
	static bool IsFreeCameraEnabled()
	{
		return FreeDebugCamera.GetInstance().IsActive();
	}
	
	protected static void EnableFreeCamera(PlayerBase player)
	{
		if (player)
		{
			FreeDebugCamera.GetInstance().SetActive(true);

			#ifdef DEVELOPER
			GetGame().GetMission().RefreshCrosshairVisibility();
			#endif
		}
		else
		{
			Debug.LogError("EnableFreeCamera( PlayerBase player ) -> player is NULL", "DeveloperFreeCamera");
		}
	}

	static void DisableFreeCamera(PlayerBase player, bool teleport_player = false)
	{
		if (player)
		{
			if (FreeDebugCamera.GetInstance().IsActive())
			{
				if (teleport_player)
					DeveloperTeleport.TeleportAtCursorEx();

				FreeDebugCamera.GetInstance().SetActive(false);

				#ifdef DEVELOPER
				if (GetGame().GetMission())
					GetGame().GetMission().RefreshCrosshairVisibility();
				#endif
			}
		}
		else
		{
			Debug.LogError("DisableFreeCamera( PlayerBase player, bool teleport_player ) -> player is NULL", "DeveloperFreeCamera");
		}
	}
	
	static void OnSetFreeCameraEvent(PlayerBase player, FreeDebugCamera camera)
	{
		if (player)
		{
			#ifdef DEVELOPER
			if (!GetGame().IsMultiplayer())
			{
				PluginSceneManager sceneManager = PluginSceneManager.Cast(GetPlugin(PluginSceneManager));
				
				if (sceneManager.IsOpened())
					sceneManager.EditorToggle();
			}
			
			GetGame().GetMission().RefreshCrosshairVisibility();
			#endif
		}
	}
}
