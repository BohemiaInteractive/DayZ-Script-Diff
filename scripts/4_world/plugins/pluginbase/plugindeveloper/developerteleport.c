class DeveloperTeleport
{
	protected static const float TELEPORT_DISTANCE_MAX = 1000;
	
	static void TeleportAtCursor()
	{
		if ( !DeveloperFreeCamera.IsFreeCameraEnabled() )
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			vector pos_player = player.GetPosition();
			
			Object ignore;
			if (!Class.CastTo(ignore, player.GetParent()))
			{
				ignore = player;
			}

			vector rayStart = GetGame().GetCurrentCameraPosition();
			vector rayEnd = rayStart + GetGame().GetCurrentCameraDirection() * 1000;		
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
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		vector rayStart = GetGame().GetCurrentCameraPosition();
		vector rayEnd = rayStart + GetGame().GetCurrentCameraDirection() * TELEPORT_DISTANCE_MAX_EX;		
		vector hitPos;
		vector hitNormal;
		float hitFraction;
		Object hitObj;
			
		Object ignore;
		if (!Class.CastTo(ignore, player.GetParent()))
		{
			ignore = player;
		}

		if (DayZPhysics.SphereCastBullet(rayStart, rayEnd, 0.01, PhxInteractionLayers.TERRAIN | PhxInteractionLayers.ROADWAY | PhxInteractionLayers.ITEM_LARGE|PhxInteractionLayers.BUILDING|PhxInteractionLayers.VEHICLE, ignore, hitObj, hitPos, hitNormal, hitFraction))
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
	
	// Set Player Position (MP support)
	static void SetPlayerPosition(PlayerBase player, vector position, bool breakSync = false)
	{
		Object object = player;
		
		HumanCommandVehicle vehCmd = player.GetCommand_Vehicle();
		if (vehCmd)
		{
			object = vehCmd.GetTransport();
		}
		
#ifdef DIAG_DEVELOPER
		if (GetGame().IsMultiplayer() && breakSync)
		{
			vector v;
			v[0] = Math.RandomFloat(-Math.PI, Math.PI);
			v[1] = Math.RandomFloat(-Math.PI, Math.PI);
			v[2] = Math.RandomFloat(-Math.PI, Math.PI);
			dBodySetAngularVelocity(object, v);
			SetVelocity(object, vector.Zero);
			
			v[0] = Math.RandomFloat(-Math.PI, Math.PI);
			v[1] = Math.RandomFloat(-Math.PI, Math.PI);
			v[2] = Math.RandomFloat(-Math.PI, Math.PI);
			object.SetOrientation(v  * Math.RAD2DEG);
		}
#endif
		
		if (GetGame().IsServer())
		{
			object.SetPosition(position);
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
		if (GetGame().IsServer())
		{
			HumanCommandVehicle vehCmd = player.GetCommand_Vehicle();
			if (vehCmd)
			{
				Transport transport = vehCmd.GetTransport();
				if (transport)
					transport.SetDirection(direction);
			}
			else
				player.SetDirection(direction);
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