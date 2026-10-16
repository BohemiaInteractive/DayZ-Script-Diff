/*
	This class implements scripted functionality over the Sandstorm controller.
	Note that certain functionality may be limited to server-only or client-only!
*/
class ScriptedSandstormController : SandstormController
{
	private static const float MAX_SURFACE_DISTANCE = 2.0;
	private static const float MAX_SURFACE_DISTANCE_INVERSE = 1.0 / MAX_SURFACE_DISTANCE;
	private const float SURFACE_TRANSITION_DISTANCE = 10.0;
	private static const float SURFACE_TRANSITION_FRACTION = 1.0 / MAX_SURFACE_DISTANCE;

	private const float SURFACE_INTENSITY_MIN = 0;
	private const float SURFACE_INTENSITY_MAX = 1.0;
	
	private static const float INTERIOR_TRANSITION_DURATION = 1.5; // 1.5 seconds transition
	private const float INTERIOR_TRANSITION_MIN = 0;
	private const float INTERIOR_TRANSITION_MAX = 1.0;
	private static const float INTERIOR_TRANSITION_DURATON_FRACTION = 1.0 / INTERIOR_TRANSITION_DURATION;
	
	private const ref array<typename> EXCLUDED_SHELTER_TYPES = {CrashBase, Plant};
	
	private ref map<string, ref PlayerSandstormData> m_PlayerData = new map<string, ref PlayerSandstormData>();
	
	private ref PPERequester_SandstormEffect m_PPESandstormRequester;
	private float m_StormEffectTimer;
	
	protected ref ScriptInvoker m_OnSandstormStartSI = new ScriptInvoker();
	protected ref ScriptInvoker m_OnSandstormStopSI = new ScriptInvoker();
	
	private void ScriptedSandstormController()  { /*Disallowed in scripts, leave to native.*/ }
	private void ~ScriptedSandstormController() { /*Disallowed in scripts, leave to native.*/ }
	
	private PlayerSandstormData GetPlayerData(string playerId)
	{
		if (!m_PlayerData.Contains(playerId))
		{
			m_PlayerData.Set(playerId, new PlayerSandstormData());
		}
		return m_PlayerData.Get(playerId);
	}
	
	private PlayerSandstormData GetPlayerSandstormData(PlayerBase player)
	{
		if (!player)
			return NULL;
		
		PlayerSandstormData playerData;
		PlayerIdentity identity = player.GetIdentity();
		if (g_Game.IsMultiplayer() || g_Game.IsDedicatedServer())
		{
			if (!identity)
				return NULL;
			
			playerData = GetPlayerData(identity.GetId());
		}
		else // singleplayer override
		{
			playerData = GetPlayerData("-1");
		}
		
		if (!playerData)
		{
			if (g_Game.IsMultiplayer() || g_Game.IsDedicatedServer())
			{
				ErrorEx("Could not get players sandstorm data! Player UID= [" + identity.GetId() + "]", ErrorExSeverity.ERROR);
			}
			else
			{
				ErrorEx("Could not get players sandstorm data! Player UID=[-1]", ErrorExSeverity.ERROR);
			}
		}
		
		return playerData;
	}
	
	void CleanupDisconnectedPlayer(string playerId)
	{
		if (m_PlayerData.Contains(playerId))
		{
			m_PlayerData.Remove(playerId);
		}
	}
	
	void CheckConnectedPlayers()
	{
		array<Man> players = new array<Man>();
		array<string> playerUIDs = new array<string>();
		
		if (g_Game.IsMultiplayer() || g_Game.IsDedicatedServer())
		{
			g_Game.GetPlayers(players);
			foreach(Man player: players)
			{
				if (!player)
					continue;
				
				PlayerIdentity identity = player.GetIdentity();
				if (!identity)
					continue;
				
				string playerUIDToAdd = identity.GetId();
				if (playerUIDs.Find(playerUIDToAdd) == -1)
					playerUIDs.Insert(playerUIDToAdd);
			}
		}
		else // singleplayer override
		{
			playerUIDs.Insert("-1");
		}
		
		//! Check if we have any PlayerSandstormData elements catched for players that are no longer connected.
		for (int i = m_PlayerData.Count() - 1; i >=0; --i)
		{
			string playerUIDToRemove = m_PlayerData.GetKey(i);
			if (playerUIDs.Find(playerUIDToRemove) == -1)
			{
				m_PlayerData.RemoveElement(i);
			}
		}
		
		playerUIDs.Clear();
	}
	
	// Lerps 'intensity' between its outside value and 0 based on whether the player is
	// currently in a shielding state and how far through the transition they are.
	//   progress = 0  -> no change (still fully outside / just entered)
	//   progress = 1  -> fully sheltered (intensity = 0) when entering,
	//                    or fully exposed (intensity = outside) when leaving.
	protected void ApplyShelterFade(inout float intensity, bool isCurrentlySheltered, float transitionProgress)
	{
		if (isCurrentlySheltered)
		{
			intensity = Math.Lerp(intensity, 0.0, transitionProgress);
		}
		else
		{
			intensity = Math.Lerp(0.0, intensity, transitionProgress);
		}
	}
	
	// Get local sandstorm intensity value for player
	override float GetIntensityForPlayer(Man player)
	{
		super.GetIntensityForPlayer(player);
		
		if (!player)
			return 0.0;
		
		PlayerBase playerPB = PlayerBase.Cast(player);
		if (!playerPB)
			return 0.0;
				
		vector headPosition = MiscGameplayFunctions.GetPlayerHeadPosition(playerPB);
		float baseIntensity = GetIntensity(headPosition);
		if (baseIntensity <= 0.0)
			return 0.0;
		
		// Get player-specific data
		PlayerSandstormData playerData = GetPlayerSandstormData(playerPB);
		if (!playerData)
			return 0.0;
		
		// Apply intensity modifiers in sequence - each can modify the result
		float modifiedIntensity = baseIntensity;

		// Apply underground modifier first (highest priority)
		ApplyUndergroundModifier(modifiedIntensity, playerPB, playerData);
		
		// If not underground, apply building modifier
		if (modifiedIntensity > 0.0)
			ApplyBuildingCheckModifier(modifiedIntensity, playerPB, playerData);
		
		// If not underground or in building, apply vehicle modifier
		if (modifiedIntensity > 0.0)
			ApplyVehicleModifier(modifiedIntensity, playerPB, playerData);
		
		return modifiedIntensity;
	}
	
	private void ApplyUndergroundModifier(inout float intensity, PlayerBase player, PlayerSandstormData playerData)
	{
		HumanCommandVehicle vehicleCommand = player.GetCommand_Vehicle();
		if (vehicleCommand)
			return;

		bool isUnderground = false;
		EUndergroundPresence presence = player.GetUndergroundPresence();
		if (presence == EUndergroundPresence.TRANSITIONING || presence == EUndergroundPresence.FULL)
			isUnderground = true;
		
		float currentTime = g_Game.GetTime();
		float elapsedTime = currentTime - playerData.m_UndergroundTransitionTimer;
		float transitionProgress = Math.Clamp(elapsedTime * INTERIOR_TRANSITION_DURATON_FRACTION, INTERIOR_TRANSITION_MIN, INTERIOR_TRANSITION_MAX);
		
		ApplyShelterFade(intensity, isUnderground, transitionProgress);
		ApplySurfaceDistanceModifier(intensity, player);
	}
	
	private void ApplySurfaceDistanceModifier(inout float intensity, PlayerBase player)
	{
		vector playerPos = player.GetPosition();
		//float surfaceHeight = GetInterpolatedSurfaceY(playerPos[0], playerPos[2]);
		float surfaceHeight = g_Game.SurfaceY(playerPos[0], playerPos[2]);
		
		if (playerPos[1] < surfaceHeight)
		{
			float depthBelowSurface = surfaceHeight - playerPos[1];
			float fade = Math.Clamp(depthBelowSurface * MAX_SURFACE_DISTANCE_INVERSE, 0, 1.0);
			intensity *= (1.0 - fade);
		}
	}
		
	private void ApplyBuildingCheckModifier(inout float intensity, PlayerBase player, PlayerSandstormData playerData)
	{
		if (!player)
			return;
		
		if (intensity <= 0)
			return;
		
		Object roofHitObj;
		bool isUnderRoof = MiscGameplayFunctions.IsUnder(player, EXCLUDED_SHELTER_TYPES, roofHitObj, GameConstants.ROOF_CHECK_RAYCAST_DIST, ObjIntersectIFire);
		bool isInInterior = player.IsSoundInsideBuilding();
		
		//ErrorEx(string.Format("Under roof=%1 | In Interior=%2", isUnderRoof, isInInterior), ErrorExSeverity.INFO);
		
		// Determine if player is currently "inside"
		bool isCurrentlyInside = (isUnderRoof && isInInterior);
		bool wasInside = (playerData.GetWasUnderRoof() && playerData.GetWasInInterior());
		float currentTime = g_Game.GetTime();
		
		// Initialize transition ONLY if building state actually changed
		if (isCurrentlyInside != wasInside)
		{
			playerData.m_InteriorTransitionTimer = currentTime; // Only reset timer on state change
			playerData.SetWasUnderRoof(isUnderRoof);
			playerData.SetWasInInterior(isInInterior);
		}
		
		// If not inside and never was inside, no modification needed
		if (!isCurrentlyInside && !wasInside)
			return;
		
		float elapsedTime = currentTime - playerData.m_InteriorTransitionTimer;
		float transitionProgress = Math.Clamp(elapsedTime * INTERIOR_TRANSITION_DURATON_FRACTION, INTERIOR_TRANSITION_MIN, INTERIOR_TRANSITION_MAX);
		
		ApplyShelterFade(intensity, isCurrentlyInside, transitionProgress);
	}
	
	private void ApplyVehicleModifier(inout float intensity, PlayerBase player, PlayerSandstormData playerData)
	{
		if (!player)
			return;
		
		if (intensity <= 0)
			return;
		
		HumanCommandVehicle vehicleCommand = player.GetCommand_Vehicle();
		if (!vehicleCommand)
			return;

		Transport transport = vehicleCommand.GetTransport();

		// no transport or not in Car transport
		if (!transport || transport && !transport.IsAnyInherited({Car}))
			return;
		
		bool isInVehicle = transport != null;
		float currentTime = g_Game.GetTime();
		
		// Initialize transition if vehicle state changed
		if (isInVehicle != playerData.GetWasInVehicle())
		{
			playerData.m_VehicleTransitionTimer = currentTime;
			playerData.SetWasInVehicle(isInVehicle);
		}
		
		float elapsedTime = currentTime - playerData.m_VehicleTransitionTimer;
		float transitionProgress = Math.Clamp(elapsedTime * INTERIOR_TRANSITION_DURATON_FRACTION, INTERIOR_TRANSITION_MIN, INTERIOR_TRANSITION_MAX);
		
		ApplyShelterFade(intensity, isInVehicle, transitionProgress);
	}
	
	// Helper function to interpolate SurfaceY values
	/*private float GetInterpolatedSurfaceY(float x, float z, float sampleRadius = 2.0)
	{
		float totalHeight = 0;
		int validSamples = 0;
		
		// Sample in a 3x3 grid around the point
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				float sampleX = x + (i * sampleRadius);
				float sampleZ = z + (j * sampleRadius);
				float height = g_Game.SurfaceY(sampleX, sampleZ);
				
				totalHeight += height;
				validSamples++;
			}
		}
		
		return totalHeight / validSamples;
	}*/
	
	/*!
		\see SandstormController::OnStart
	*/
	override void OnStart(bool continuedSession, bool calledByWeather = false)
	{
		super.OnStart(calledByWeather, continuedSession);
		
		// Server is not interested in local effects like post-processing.
		if (!g_Game || g_Game.IsDedicatedServer())
			return;
		
		// Clients may start their post processing effects as soon as the sandstorm starts - this may also be during join in progress so set the intensity accordingly as well.
		m_PPESandstormRequester = PPERequester_SandstormEffect.Cast(PPERequesterBank.GetRequester(PPERequesterBank.REQ_SANDSTORMEFFECT));
		
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player && m_PPESandstormRequester)
		{
			float localIntensity = GetIntensityForPlayer(player);
			m_PPESandstormRequester.SetTargetIntensity(localIntensity);
			m_PPESandstormRequester.Start();
		}
		
		if (m_OnSandstormStartSI)
			m_OnSandstormStartSI.Invoke();
	}
	
	/*!
		\see SandstormController::OnUpdate
	*/
	override void OnUpdate(float deltaTime)
	{
		super.OnUpdate(deltaTime);
		
		// Server is not interested in local effects like post-processing.
		if (!g_Game || g_Game.IsDedicatedServer())
			return;
		
		// Clients may update their post-processing effect and its properties
		// on a per-tick basis, to represent the actual state closely
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player && m_PPESandstormRequester)
		{
			float localIntensity = GetIntensityForPlayer(player);
			m_PPESandstormRequester.SetTargetIntensity(localIntensity);
		}
	}
	
	/*!
		\see SandstormController::OnStop
	*/
	override void OnStop(bool calledByWeather = false)
	{
		super.OnStop(calledByWeather);
		
		// Server is not interested in local effects like post-processing.
		if (!g_Game || g_Game.IsDedicatedServer())
			return;
		
		// Clients may reset their post processing to default state
		if (m_PPESandstormRequester)
		{
			m_PPESandstormRequester.SetTargetIntensity(0);
			m_PPESandstormRequester.Stop();
		}
		
		if (m_OnSandstormStopSI)
			m_OnSandstormStopSI.Invoke();
	}
	
	ScriptInvoker GetSandstormStartSI()
	{
		return m_OnSandstormStartSI;
	}
	
	ScriptInvoker GetSandstormStopSI()
	{
		return m_OnSandstormStopSI;
	}
}