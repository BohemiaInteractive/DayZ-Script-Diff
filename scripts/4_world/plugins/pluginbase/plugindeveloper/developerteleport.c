class DeveloperTeleport
{
	protected static const float TELEPORT_DISTANCE_MAX = 1000;
	
	static void TeleportAtCursor()
	{
		if ( !DeveloperFreeCamera.IsFreeCameraEnabled() )
		{
			PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
			vector pos_player = player.GetPosition();
			
			Object ignore;
			if (!Class.CastTo(ignore, player.GetParent()))
			{
				ignore = player;
			}

			vector rayStart = g_Game.GetCurrentCameraPosition();
			vector rayEnd = rayStart + g_Game.GetCurrentCameraDirection() * 1000;		
			vector hitPos;
			vector hitNormal;
			int hitComponentIndex;		
			DayZPhysics.RaycastRV(rayStart, rayEnd, hitPos, hitNormal, hitComponentIndex, NULL, NULL, ignore);
				
			float distance = vector.Distance( pos_player, hitPos );
			
			if ( distance < TELEPORT_DISTANCE_MAX )
			{
				bool breakSync = false;
				
				#ifdef DIAG_DEVELOPER
				breakSync = DiagMenu.GetBool(DiagMenuIDs.MISC_TELEPORT_BREAKS_SYNC);
				#endif
				
				SetPlayerPosition(player, hitPos, breakSync);
			}
			else
			{
				Debug.LogWarning("Distance for teleportation is too far!");
			}				
		}
	}
	
	protected static const float TELEPORT_DISTANCE_MAX_EX = 500;
	static void TeleportAtCursorEx()
	{
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		vector rayStart = g_Game.GetCurrentCameraPosition();
		vector rayEnd = rayStart + g_Game.GetCurrentCameraDirection() * TELEPORT_DISTANCE_MAX_EX;		
		vector hitPos;
		vector hitNormal;
		float hitFraction;
		Object hitObj;
			
		Object ignore;
		if (!Class.CastTo(ignore, player.GetParent()))
		{
			ignore = player;
		}

		int layers = 0;
		layers |= PhxInteractionLayers.TERRAIN;
		layers |= PhxInteractionLayers.ROADWAY;
		layers |= PhxInteractionLayers.ITEM_LARGE;
		layers |= PhxInteractionLayers.BUILDING;
		layers |= PhxInteractionLayers.VEHICLE;
		layers |= PhxInteractionLayers.RAGDOLL;
		if (DayZPhysics.SphereCastBullet(rayStart, rayEnd, 0.01, layers, ignore, hitObj, hitPos, hitNormal, hitFraction))
		{
			bool breakSync = false;
				
			#ifdef DIAG_DEVELOPER
			breakSync = DiagMenu.GetBool(DiagMenuIDs.MISC_TELEPORT_BREAKS_SYNC);
			#endif
				
			SetPlayerPosition(player, hitPos, breakSync);
			
			if (DeveloperFreeCamera.IsFreeCameraEnabled())
			{
				DeveloperTeleport.SetPlayerDirection( player, FreeDebugCamera.GetInstance().GetDirection() );
			}
		}
	}

	static Object GetPlayerRootForTeleporting(PlayerBase player)
	{
		Object playerRoot = player;
		
		HumanCommandVehicle hcv = player.GetCommand_Vehicle();
		if (hcv)
		{
			playerRoot = hcv.GetTransport();
		}

		HumanCommandUnconscious hcu = player.GetCommand_Unconscious();
		if (hcu)
		{
			Class.CastTo(playerRoot, player.GetParent());

			if (playerRoot != player.GetTransportCache())
			{
				playerRoot = null;
			}
		}

		if (playerRoot == null)
		{
			playerRoot = player;
		}

		return playerRoot;
	}
	
	// Set Player Position (MP support)
	static void SetPlayerPosition(PlayerBase player, vector position, bool breakSync = false)
	{
		Object playerRoot = GetPlayerRootForTeleporting(player);
		
#ifdef DIAG_DEVELOPER
		if (g_Game.IsMultiplayer() && breakSync)
		{
			vector v;
			v[0] = Math.RandomFloat(-Math.PI, Math.PI);
			v[1] = Math.RandomFloat(-Math.PI, Math.PI);
			v[2] = Math.RandomFloat(-Math.PI, Math.PI);
			dBodySetAngularVelocity(playerRoot, v);
			SetVelocity(playerRoot, vector.Zero);
			
			v[0] = Math.RandomFloat(-Math.PI, Math.PI);
			v[1] = Math.RandomFloat(-Math.PI, Math.PI);
			v[2] = Math.RandomFloat(-Math.PI, Math.PI);
			playerRoot.SetOrientation(v  * Math.RAD2DEG);
		}
#endif

		if (position[1] < g_Game.SurfaceGetSeaLevel())
		{
			position[1] = g_Game.SurfaceGetSeaLevel();
		}
		
		if (g_Game.IsServer())
		{
			playerRoot.SetPosition(position);
		}
		else
		{
			Param4<float, float, float, bool> params = new Param4<float, float, float, bool>(position[0], position[1], position[2], breakSync);
			player.RPCSingleParam(ERPCs.DEV_RPC_TELEPORT, params, true);
		}
	}
	
	// Set Player Direction (MP support)
	static void SetPlayerDirection(PlayerBase player, vector direction)
	{
		Object playerRoot = GetPlayerRootForTeleporting(player);

		if (g_Game.IsServer())
		{
			playerRoot.SetDirection(direction);
		}
		else
		{
			Param3<float, float, float> params = new Param3<float, float, float>(direction[0], direction[1], direction[2]);
			player.RPCSingleParam(ERPCs.DEV_RPC_SET_PLAYER_DIRECTION, params, true);
		}
	}
	
	static void OnRPC(PlayerBase player, int rpc_type, ParamsReadContext ctx)
	{
		#ifdef DIAG_DEVELOPER
		if ( rpc_type == ERPCs.DEV_RPC_TELEPORT )
		{
			OnRPCSetPlayerPosition(player, ctx);
		}
		else if ( rpc_type == ERPCs.DEV_RPC_SET_PLAYER_DIRECTION )
		{
			OnRPCSetPlayerDirection(player, ctx);
		}
		#endif
	}
	
	static protected void OnRPCSetPlayerPosition(PlayerBase player, ParamsReadContext ctx)
	{
		Param4<float, float, float, bool> p = new Param4<float, float, float, bool>(0, 0, 0, false);
		if (ctx.Read(p))
		{
			vector v = "0 0 0";
			v[0] = p.param1;
			v[1] = p.param2;
			v[2] = p.param3;
			SetPlayerPosition(player, v, p.param4);
		}
	}
	
	static protected void OnRPCSetPlayerDirection(PlayerBase player, ParamsReadContext ctx)
	{
		Param3<float, float, float> p = new Param3<float, float, float>(0, 0, 0);
		if (ctx.Read(p))
		{
			vector v = "0 0 0";
			v[0] = p.param1;
			v[1] = p.param2;
			v[2] = p.param3;
			SetPlayerDirection(player, v);
		}
	}
}