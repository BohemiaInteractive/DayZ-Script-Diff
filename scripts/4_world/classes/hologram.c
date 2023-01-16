class Hologram
{
	#ifdef SERVER
	protected const int SPAWN_FLAGS 	= ECE_CREATEPHYSICS;
	#else
	protected const int SPAWN_FLAGS 	= ECE_LOCAL;
	#endif
	
	protected const string SUFFIX_MATERIAL_DEPLOYABLE 	= "_deployable.rvmat";
	protected const string SUFFIX_MATERIAL_UNDEPLOYABLE = "_undeployable.rvmat";
	protected const string SUFFIX_MATERIAL_POWERED 		= "_powered.rvmat";

	protected ItemBase 			m_Parent;
	protected EntityAI 			m_Projection;
	protected PlayerBase 		m_Player;
	protected ProjectionTrigger m_ProjectionTrigger;
	protected string			m_ProjectionTypename;

	protected bool 				m_IsColliding;
	protected bool 				m_IsCollidingGPlot;
	protected bool 				m_IsSlope;
	protected bool 				m_IsCollidingPlayer;
	protected bool 				m_IsFloating;
	protected bool 				m_UpdatePosition;
	protected bool 				m_IsHidden;

	protected vector 			m_DefaultOrientation;
	protected vector			m_Rotation;
	protected vector			m_y_p_r_previous;
	protected vector 			m_ContactDir;
	protected vector 			m_FromAdjusted;
	protected const string 		ANIMATION_PLACING 				= "Placing";
	protected const string 		ANIMATION_INVENTORY 			= "Inventory";
	protected const string 		SELECTION_PLACING 				= "placing";
	protected const string 		SELECTION_INVENTORY 			= "inventory";

	protected const float 		SMALL_PROJECTION_RADIUS 		= 1;
	protected const float 		SMALL_PROJECTION_GROUND 		= 2;
	protected const float		DISTANCE_SMALL_PROJECTION		= 1; //! Deprecated
	protected const float		LARGE_PROJECTION_DISTANCE_LIMIT	= 6;
	protected const float 		PROJECTION_TRANSITION_MIN		= 1;
	protected const float 		PROJECTION_TRANSITION_MAX		= 0.25;
	protected const float 		LOOKING_TO_SKY					= 0.75;
	static const float 			DEFAULT_MAX_PLACEMENT_HEIGHT_DIFF = 1.5;
	
	protected float 			m_SlopeTolerance;
	protected bool				m_AlignToTerrain;
	protected vector 			m_YawPitchRollLimit;
	protected int				m_ContactComponent;
	
	protected ref set<string> 	m_SelectionsToRefresh 			= new set<string>;
	
	// Watchtower correction variables	
	// These watchtower component names should be corrected when colliding with them as they are supposed to be "trigger boxes", not colliders
	static const protected ref array<string>	m_WatchtowerIgnoreComponentNames	= new array<string>;
	
	// These watchtower components are supposed to be trigger boxes, but should block placement on them (currently only the boxes above the stairs)
	static const protected ref array<string>	m_WatchtowerBlockedComponentNames	= new array<string>;

	void Hologram(PlayerBase player, vector pos, ItemBase item)
	{	
		m_Player = player;
		m_Parent = item;
		m_Projection = NULL;
		m_ProjectionTrigger = NULL;
		m_UpdatePosition = true;
		m_Rotation = "0 0 0";
		m_ContactComponent = -1;
		m_FromAdjusted = "0 0 0";
		
		// If the static names are empty, generate the needed names
		// Refer to their definitions to see why these are required
		if (m_WatchtowerIgnoreComponentNames.Count() == 0)
		{
			string baseStringBegin = Watchtower.BASE_VIEW_NAME;
			string baseIgnoreStringEnd = Watchtower.BASE_WALL_NAME;
			int floors = Watchtower.MAX_WATCHTOWER_FLOORS;
			int walls = Watchtower.MAX_WATCHTOWER_WALLS;
			string compName;
			
			for (int i = 1; i < floors + 1; ++i)
			{
				compName = baseStringBegin + i.ToString();
				for (int j = 1; j < walls + 1; ++j)
					m_WatchtowerIgnoreComponentNames.Insert(compName + baseIgnoreStringEnd + j.ToString());
				
				if (i != 1)
					m_WatchtowerBlockedComponentNames.Insert(compName);
				else
					m_WatchtowerIgnoreComponentNames.Insert(compName);

			}
		}

		string configPathProjectionTypename = string.Format("CfgVehicles %1 projectionTypename", m_Parent.GetType());
		if (GetGame().ConfigIsExisting(configPathProjectionTypename))
		{
			m_ProjectionTypename = GetGame().ConfigGetTextOut(configPathProjectionTypename);
		}

		EntityAI projectionEntity;
		if (GetGame().IsMultiplayer() && GetGame().IsServer())
		{	
			projectionEntity = EntityAI.Cast(GetGame().CreateObjectEx(ProjectionBasedOnParent(), pos, ECE_PLACE_ON_SURFACE));
			projectionEntity.SetAllowDamage(false);
			SetProjectionEntity(projectionEntity);
			SetAnimations();
		}
		else
		{
			projectionEntity = EntityAI.Cast(GetGame().CreateObjectEx(ProjectionBasedOnParent(), pos, ECE_TRACE|ECE_LOCAL));
			if (projectionEntity == null)
			{
				ErrorEx(string.Format("Cannot create hologram entity from config class %1", ProjectionBasedOnParent()), ErrorExSeverity.WARNING);
				return;
			}

			SetProjectionEntity(projectionEntity);
			SetAnimations();
			CreateTrigger();
			RefreshTrigger();
		}
		
		if (ItemBase.Cast(projectionEntity))
		{
			ItemBase.Cast(GetProjectionEntity()).SetIsHologram(true);
		}
		
		string configPathSlope = string.Format("CfgVehicles %1 slopeTolerance", GetProjectionEntity().GetType());
		if (GetGame().ConfigIsExisting(configPathSlope))
		{
			m_SlopeTolerance = GetGame().ConfigGetFloat(configPathSlope);
		}
		
		string configPathAlign = string.Format("CfgVehicles %1 alignHologramToTerain", GetProjectionEntity().GetType());
		if (GetGame().ConfigIsExisting(configPathAlign))
		{
			m_AlignToTerrain = GetGame().ConfigGetInt(configPathAlign);
		}
		
		string configPathOrientationLimit = string.Format("CfgVehicles %1 yawPitchRollLimit", GetProjectionEntity().GetType());
		if (GetGame().ConfigIsExisting(configPathOrientationLimit))
		{
			m_YawPitchRollLimit = GetGame().ConfigGetVector(configPathOrientationLimit);
		}
	}
	
	void ~Hologram()
	{
		if (GetGame())
		{
			if (m_Projection)
			{
				GetGame().ObjectDelete(m_Projection);
			}
	
			if (m_ProjectionTrigger)
			{
				GetGame().ObjectDelete(m_ProjectionTrigger);
			}
		}
		
		#ifdef DIAG_DEVELOPER
		DestroyDebugCollisionBox();
		#endif
	}
	
	void SetAnimations()
	{
		if ( m_Projection.HasAnimation( ANIMATION_PLACING ) )
		{
			m_Projection.SetAnimationPhase( ANIMATION_PLACING, 0 );
			SetSelectionToRefresh( SELECTION_PLACING );

			if ( m_Projection.HasAnimation( ANIMATION_INVENTORY ) )
			{
				m_Projection.SetAnimationPhase( ANIMATION_INVENTORY, 1 );
			}
		}
		else
		{
			UpdateSelections();
			SetSelectionToRefresh( SELECTION_INVENTORY );
		}
	}
	
	// Updates selections on hologram object so they reflect the state of the parent object's selections.
	void UpdateSelections()
	{
		string cfg_access = "CfgVehicles " + m_Projection.GetType() + " AnimationSources ";
		
		if ( GetGame().ConfigIsExisting(cfg_access) )
		{
			int cfg_access_count = g_Game.ConfigGetChildrenCount(cfg_access);

			for ( int i = 0; i < cfg_access_count; ++i )
			{
				string found_anim;
				GetGame().ConfigGetChildName(cfg_access, i, found_anim);
				
				float anim_phase = m_Parent.GetAnimationPhase(found_anim);
				m_Projection.SetAnimationPhase(found_anim, anim_phase);
			}
		}
	}

	string ProjectionBasedOnParent()
	{
		return GetProjectionName(m_Parent);
	}
	
	string GetProjectionName(ItemBase item)
	{
		//! configuration override
		if (m_ProjectionTypename != "")
		{
			return m_ProjectionTypename;
		}

		if (!item)
		{
			return "";
		}
		
		if (item.CanMakeGardenplot())
		{
			return "GardenPlotPlacing";
		}
		
		//Camping & Base building
		if (item.IsInherited( TentBase ) || item.IsBasebuildingKit())
		{
			return item.GetType() + "Placing";
		}
		
		return item.GetType();
	}
	
	//! DEPRECATED
	static bool DoesHaveProjection(ItemBase item)
	{
		return item && (item.IsDeployable() || item.CanMakeGardenplot() || item.IsInherited(DeployableContainer_Base));
	}
	
	// update loop for visuals and collisions of the hologram
	void UpdateHologram( float timeslice )
	{
		if ( !m_Parent )
		{
			m_Player.TogglePlacingLocal();
			
			return;
		}
		
		if ( m_Player.IsSwimming() || m_Player.IsClimbingLadder() || m_Player.IsRaised() || m_Player.IsClimbing() || m_Player.IsRestrained() || m_Player.IsUnconscious() )
		{
			m_Player.TogglePlacingLocal();
			
			return;
		}

		if ( !GetUpdatePosition() )
		{
			return;
		} 
		
		
		#ifdef DIAG_DEVELOPER
		DebugConfigValues();	
		DestroyDebugCollisionBox();
		#endif

		// update hologram position	
		SetProjectionPosition( GetProjectionEntityPosition( m_Player ) );
		SetProjectionOrientation( AlignProjectionOnTerrain( timeslice ) );

		EvaluateCollision();
		RefreshTrigger();
		CheckPowerSource();	
		RefreshVisual();

		m_Projection.OnHologramBeingPlaced( m_Player );
	}
	
	vector AlignProjectionOnTerrain( float timeslice )
	{
		vector y_p_r;
		
		if ( m_AlignToTerrain )
		{
			vector projection_orientation_angles = GetDefaultOrientation() + GetProjectionRotation();	
			vector mat0[3];
			vector mat1[3];
			vector mat2[3];
			vector projection_position = m_Projection.GetPosition();
			vector normal;
			
			if ( m_ContactDir.Length() > 0 )
			{
				normal = m_ContactDir;
			} 
			else
			{
				normal = GetGame().SurfaceGetNormal( projection_position[0], projection_position[2] );
			}
			
			vector angles = normal.VectorToAngles();
			angles[1] = angles[1] + 270;
					
			angles[0] = Math.Clamp( angles[0], 0, 360 ); 
			angles[1] = Math.Clamp( angles[1], 0, 360 );
			angles[2] = Math.Clamp( angles[2], 0, 360 );
				
			projection_orientation_angles[0] = projection_orientation_angles[0] + ( 360 - angles[0] );
			
			Math3D.YawPitchRollMatrix( projection_orientation_angles, mat0 );
			Math3D.YawPitchRollMatrix( angles, mat1 );
			Math3D.MatrixMultiply3( mat1, mat0, mat2 );	
			
			y_p_r = Math3D.MatrixToAngles( mat2 );
		}
		else
		{
			y_p_r = GetDefaultOrientation() + GetProjectionRotation();
			
			if ( y_p_r[0] > 180 )
			{
				y_p_r[0] = y_p_r[0] - 360;
			}
			
			if ( y_p_r[0] < -180 )
			{
				y_p_r[0] = y_p_r[0] + 360;
			}
		}	
		
		return SmoothProjectionMovement( y_p_r, timeslice );
	}
	
	vector SmoothProjectionMovement( vector y_p_r, float timeslice )
	{	
		if ( m_y_p_r_previous )
		{					
			if ( Math.AbsFloat( y_p_r[0] - m_y_p_r_previous[0] ) > 100 )
			{												
				if ( y_p_r[0] > 0 )
				{		
					m_y_p_r_previous[0] = m_y_p_r_previous[0] + 360; 
				}
				
				if ( y_p_r[0] < 0 )
				{		
					m_y_p_r_previous[0] = m_y_p_r_previous[0] - 360;
				}
			}
			
			y_p_r[0] = Math.Lerp( m_y_p_r_previous[0], y_p_r[0], 15 * timeslice );
			y_p_r[1] = Math.Lerp( m_y_p_r_previous[1], y_p_r[1], 15 * timeslice );
			y_p_r[2] = Math.Lerp( m_y_p_r_previous[2], y_p_r[2], 15 * timeslice );
		}
		
		m_y_p_r_previous = y_p_r;
		
		return y_p_r;
	}
	
	void CreateTrigger()
	{
		Class.CastTo(m_ProjectionTrigger, g_Game.CreateObjectEx("ProjectionTrigger", GetProjectionPosition(), SPAWN_FLAGS));

		m_ProjectionTrigger.SetOrientation(GetProjectionOrientation());
		m_ProjectionTrigger.SetParentObject(this);
		m_ProjectionTrigger.SetParentOwner(m_Player);
		
		RefreshVisual();
	}

	void RefreshTrigger()
	{
		vector min_max[2];
		GetProjectionCollisionBox( min_max );

		m_ProjectionTrigger.SetPosition(GetProjectionPosition());
		m_ProjectionTrigger.SetOrientation(GetProjectionOrientation());
		m_ProjectionTrigger.SetExtents(min_max[0], min_max[1]);
	}
	
	#ifdef DIAG_DEVELOPER
	void DebugText(string header, bool mustBeTrue = false, bool condition = true, string info = "")
	{		
		if ( DiagMenu.GetBool(DiagMenuIDs.MISC_HOLOGRAM) )
		{
			int color = 0xFFFFFFFF;
			
			if (mustBeTrue && !condition || !mustBeTrue && condition)
				color = COLOR_RED;
			
			string text = header + condition + info;	
			DbgUI.ColoredText(color, text);
		}		
	}
	
	protected float m_PitchOverride;
	protected float m_RollOverride;
	void DebugConfigValues()
	{
		if ( DiagMenu.GetBool(DiagMenuIDs.MISC_HOLOGRAM) )
		{
			m_PitchOverride = m_YawPitchRollLimit[1];
			m_RollOverride = m_YawPitchRollLimit[2];
			
			DbgUI.InputFloat("slopeTolerance override", m_SlopeTolerance);
			DbgUI.SameLine();
			DbgUI.InputFloat("pitch limit override", m_PitchOverride);
			DbgUI.SameLine();
			DbgUI.InputFloat("roll limit override", m_RollOverride);
			
			m_YawPitchRollLimit[1] = m_PitchOverride;
			m_YawPitchRollLimit[2] = m_RollOverride;
		}
	}
	#endif

	void EvaluateCollision(ItemBase action_item = null)
	{	
		if ( IsFloating() || IsHidden() || IsCollidingBBox(action_item) || IsCollidingPlayer() || IsClippingRoof() || !IsBaseViable() || IsCollidingGPlot() || IsCollidingZeroPos() || IsCollidingAngle() || !IsPlacementPermitted() || !HeightPlacementCheck() || IsUnderwater() || IsInTerrain() )
		{
			SetIsColliding( true );
		}
		else if ( m_Projection.IsInherited( TrapSpawnBase ))
		{
			#ifdef DIAG_DEVELOPER
			DebugText("Inherits from TrapSpawnBase, checking IsPlaceableAtposition", true);
			#endif
			TrapSpawnBase trap_spawn_base;
			Class.CastTo( trap_spawn_base,  m_Projection );
			SetIsColliding( !trap_spawn_base.IsPlaceableAtPosition( m_Projection.GetPosition() ) );
		}
		else if ( m_Projection.IsInherited( TrapBase ))
		{
			#ifdef DIAG_DEVELOPER
			DebugText("Inherits from TrapBase, checking IsPlaceableAtposition", true);
			#endif
			TrapBase trap_base;
			Class.CastTo(trap_base,  m_Projection);
			SetIsColliding( !trap_base.IsPlaceableAtPosition( m_Projection.GetPosition() ) );
		}
		else
		{
			SetIsColliding( false );
		}
	}
	
	bool IsClippingRoof()
	{
		if (CfgGameplayHandler.GetDisableIsClippingRoofCheck())
			return false;
		if (GetGame().IsServer() && GetGame().IsMultiplayer())
			return false;
		
		//Some locations allow you to look up and attempt placing the hologram on the bottom side of a floor - most notably the floors of a watchtower
		//This check also prevents some very unnatural placements

		bool b1 = m_Projection.GetPosition()[1] > GetGame().GetCurrentCameraPosition()[1];
		bool b2 = false;
		#ifdef DIAG_DEVELOPER
		vector from, to;
		#endif
		if (m_Projection.DoPlacingHeightCheck())
		{
			b2 = MiscGameplayFunctions.IsUnderRoofEx(m_Projection, GameConstants.ROOF_CHECK_RAYCAST_DIST, ObjIntersectFire);
			#ifdef DIAG_DEVELOPER			
			MiscGameplayFunctions.IsUnderRoofFromToCalculation(m_Projection, from, to);
			DrawArrow(from, to, !b2);
			#endif
		}
		
		#ifdef DIAG_DEVELOPER
		DebugText("IsClippingRoof: ", false, b1, " | (projection height) " + m_Projection.GetPosition()[1] + " > (camera height) " + GetGame().GetCurrentCameraPosition()[1]);
		DebugText("IsClippingRoof: ", false, b2, " | (DoPlacingHeightCheck) " + m_Projection.DoPlacingHeightCheck() + " && (IsUnderRoof) " + MiscGameplayFunctions.IsUnderRoof(m_Projection) + " | from: " + from[1] + " | to: " + to[1]);
		#endif
		
		return b1 || b2;
	}
	
	bool IsCollidingAngle()
	{
		if (CfgGameplayHandler.GetDisableIsCollidingAngleCheck())
			return false;
		vector projection_orientation = m_Projection.GetOrientation();		
		bool isTrue = Math.AbsFloat( projection_orientation[1] ) > m_YawPitchRollLimit[1] || Math.AbsFloat( projection_orientation[2] ) > m_YawPitchRollLimit[2];
		#ifdef DIAG_DEVELOPER
		DebugText("IsCollidingAngle: ", false, isTrue, " | (proj pitch) " + Math.AbsFloat( projection_orientation[1] ) + " > (pitch limit) " + m_YawPitchRollLimit[1] + " | (proj roll) " + Math.AbsFloat( projection_orientation[2] ) + " > (roll limit) " + m_YawPitchRollLimit[2]);
		#endif

		return isTrue;
	}
	
	#ifdef DIAG_DEVELOPER
	protected Shape m_CollisionBox;
	protected void DrawDebugCollisionBox( vector min_max[2], int color )
	{
		vector mat[4];
		m_Projection.GetTransform( mat );
		m_CollisionBox = Debug.DrawBox( min_max[0], min_max[1], color );
		m_CollisionBox.SetMatrix( mat );
	}
	
	protected void DestroyDebugCollisionBox()
	{
		if ( m_CollisionBox )
		{
			m_CollisionBox.Destroy();
			m_CollisionBox = NULL;
		}		
	}
	#endif

	bool IsCollidingBBox(ItemBase action_item = null)
	{
		if (CfgGameplayHandler.GetDisableIsCollidingBBoxCheck())
			return false;

		vector center;
		vector relativeOffset; //we need to lift BBox, because it is calculated from the bottom of projection, and not from the middle
		vector absoluteOffset = "0 0.01 0"; //we need to lift BBox even more, because it colliddes with house floors due to various reasons (probably geometry or float imperfections)
		vector orientation = GetProjectionOrientation();
		vector edgeLength;
		vector minMax[2];
		array<Object> excludedObjects = new array<Object>();
		array<Object> collidedObjects = new array<Object>();
		
		GetProjectionCollisionBox(minMax);
		relativeOffset[1] = (minMax[1][1] - minMax[0][1]) * 0.5;
		center = m_Projection.GetPosition() + relativeOffset + absoluteOffset;
		edgeLength = GetCollisionBoxSize(minMax);
		excludedObjects.Insert(m_Projection);
		excludedObjects.Insert(m_Player);

		if (action_item)
			excludedObjects.Insert(action_item);

		//add is construction check
		// Base building objects behave in a way that causes this test to generate false positives
		bool isTrue = GetGame().IsBoxCollidingGeometry(center, orientation, edgeLength, ObjIntersectFire, ObjIntersectGeom, excludedObjects, collidedObjects);		
		#ifdef DIAG_DEVELOPER	
		if ( DiagMenu.GetBool(DiagMenuIDs.MISC_HOLOGRAM) )
		{
			string text = "";
			foreach (Object object: collidedObjects)
				text += " | " + Object.GetDebugName(object);

			DebugText("IsCollidingBBox: ", false, isTrue, text);
		
			int color = 0x01FFFFFF;
			if (isTrue)
				color = 0x33F22613;
			
			DrawDebugCollisionBox(minMax, color);
		}
		#endif

		return isTrue;
	}

	bool IsBaseViable()
	{	
		//This function is not required to solve server-side fixes for clipping, saves calculations and potential false negatives
		if (GetGame().IsServer() && GetGame().IsMultiplayer())
			return true;

		if (CfgGameplayHandler.GetDisableIsBaseViableCheck())
			return true;
		
		vector from_left_close = m_Projection.CoordToParent( GetLeftCloseProjectionVector() );
		vector to_left_close_down = from_left_close + "0 -1 0";

		vector from_right_close = m_Projection.CoordToParent( GetRightCloseProjectionVector() );
		vector to_right_close_down = from_right_close + "0 -1 0";

		vector from_left_far = m_Projection.CoordToParent( GetLeftFarProjectionVector() );
		vector to_left_far_down = from_left_far + "0 -1 0";

		vector from_right_far = m_Projection.CoordToParent( GetRightFarProjectionVector() );
		vector to_right_far_down = from_right_far + "0 -1 0";
		
		vector contact_pos_left_close;
		vector contact_pos_right_close;
		vector contact_pos_left_far;
		vector contact_pos_right_far;
		vector contact_dir_left_close;
		vector contact_dir_right_close;
		vector contact_dir_left_far;
		vector contact_dir_right_far;
		int contact_component_left_close;
		int contact_component_right_close;
		int contact_component_left_far;
		int contact_component_right_far;
		set<Object> results_left_close = new set<Object>;
		set<Object> results_right_close = new set<Object>;
		set<Object> results_left_far = new set<Object>;
		set<Object> results_right_far = new set<Object>;
		Object obj_left_close;
		Object obj_right_close;
		Object obj_left_far;
		Object obj_right_far;

		//Not sure what the intention here was before, but it boiled down to a very bloated version of what you see here right now
		DayZPhysics.RaycastRV( from_left_close, to_left_close_down, contact_pos_left_close, contact_dir_left_close, contact_component_left_close, results_left_close, null, m_Projection, false, false, ObjIntersectFire );
		if (results_left_close.Count() > 0)
			obj_left_close = results_left_close[results_left_close.Count() - 1];

		DayZPhysics.RaycastRV( from_right_close, to_right_close_down, contact_pos_right_close, contact_dir_right_close, contact_component_right_close, results_right_close, null, m_Projection, false, false, ObjIntersectFire );
		if (results_right_close.Count() > 0)	
			obj_right_close = results_right_close[results_right_close.Count() - 1];

		DayZPhysics.RaycastRV( from_left_far, to_left_far_down, contact_pos_left_far, contact_dir_left_far, contact_component_left_far, results_left_far, null, m_Projection, false, false, ObjIntersectFire );
		if (results_left_far.Count() > 0) 
			obj_left_far = results_left_far[results_left_far.Count() - 1];

		DayZPhysics.RaycastRV( from_right_far, to_right_far_down, contact_pos_right_far, contact_dir_right_far, contact_component_right_far, results_right_far, null, m_Projection, false, false, ObjIntersectFire );
		if (results_right_far.Count() > 0)
			obj_right_far = results_right_far[results_right_far.Count() - 1];
		
		return IsBaseIntact( obj_left_close, obj_right_close, obj_left_far, obj_right_far ) && IsBaseStatic( obj_left_close ) && IsBaseFlat( contact_pos_left_close, contact_pos_right_close, contact_pos_left_far, contact_pos_right_far );
	}

	bool IsCollidingGPlot()
	{
		if (CfgGameplayHandler.GetDisableIsCollidingGPlotCheck())
			return false;

		#ifdef DIAG_DEVELOPER
		DebugText("IsCollidingGPlot: ", false, m_IsCollidingGPlot);
		#endif

		return m_IsCollidingGPlot;
	}

	bool IsCollidingZeroPos()
	{
		vector origin = Vector(0, 0, 0);
		bool isTrue = GetProjectionPosition() == origin;
		#ifdef DIAG_DEVELOPER
		DebugText("IsCollidingZeroPos: ", false, isTrue);
		#endif

		return isTrue;
	}
	
	//! DEPRECATED
	bool IsBehindObstacle()
	{			
		ErrorEx("Deprecated check - do not use", ErrorExSeverity.WARNING);
		return false;
	}
	
	//This function only takes one of the found objects since IsBaseIntact already verifies that all of them are either null or the same object
	bool IsBaseStatic( Object objectToCheck )
	{	
		//check if the object below hologram is dynamic object. Dynamic objects like barrels can be taken to hands 
		//and item which had been placed on top of them, would stay floating in the air			
		#ifdef DIAG_DEVELOPER
		if (objectToCheck == null)
			DebugText("IsBaseStatic(must be true): ", true, true, " | objectToCheck is null (this is good)");
		else
			DebugText("IsBaseStatic(must be true): ", true, IsObjectStatic(objectToCheck));
		#endif
		return objectToCheck == null || IsObjectStatic(objectToCheck);
	}
	
	bool IsObjectStatic( Object obj )
	{
		return obj.IsBuilding() || obj.IsPlainObject() || (!m_Parent.IsInherited(KitBase) && obj.IsInherited(BaseBuildingBase) && (m_WatchtowerBlockedComponentNames.Find(obj.GetActionComponentName(m_ContactComponent, LOD.NAME_VIEW)) == -1));
	}

	bool IsBaseIntact( Object under_left_close, Object under_right_close, Object under_left_far, Object under_right_far )
	{
		bool isTrue = (under_left_close == under_right_close && under_right_close == under_left_far && under_left_far == under_right_far);
		#ifdef DIAG_DEVELOPER
		DebugText("IsBaseIntact(must be true and all equal): ", true, isTrue, " | ulc: " + Object.GetDebugName(under_left_close) + " | urc: " + Object.GetDebugName(under_right_close) + " | ulf: " + Object.GetDebugName(under_left_far) + " | urf: " + Object.GetDebugName(under_right_far));
		if (!isTrue)
		{
			array<bool> conditions = new array<bool>();
			conditions.Insert(under_left_close == null);
			conditions.Insert(under_right_close == null);
			conditions.Insert(under_left_far == null);
			conditions.Insert(under_right_far == null);
			
			int amountOfNull = 0;
			if (!under_left_close)
				++amountOfNull;
			if (!under_right_close)
				++amountOfNull;
			if (!under_left_far)
				++amountOfNull;
			if (!under_right_far)
				++amountOfNull;
			
			if ( amountOfNull < 3 )
				for ( int i = 0; i < conditions.Count(); ++i)
					conditions[i] = !conditions[i];
			
			DrawBaseSpheres(conditions);
		}
		#endif

		return isTrue;
	}
	
	#ifdef DIAG_DEVELOPER
	void DrawArrow(vector from, vector to, bool condition)
	{
		if ( DiagMenu.GetBool(DiagMenuIDs.MISC_HOLOGRAM) )
		{
			int color = 0xFFFFFFFF;
			if (!condition)
				color = COLOR_RED;
			
			Debug.DrawArrow(from, to, 1, color, ShapeFlags.ONCE);
		}
	}
	
	void DrawSphere(vector pos, bool condition)
	{
		if ( DiagMenu.GetBool(DiagMenuIDs.MISC_HOLOGRAM) )
		{
			int color = 0x01FFFFFF;
			if (!condition)
				color = 0x99F22613;
			
			Debug.DrawSphere(pos, 1, color, ShapeFlags.ONCE|ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE);
		}
	}
	
	void DrawBaseSpheres(array<bool> conditions)
	{
		if ( DiagMenu.GetBool(DiagMenuIDs.MISC_HOLOGRAM) )
		{
			array<vector> positions = new array<vector>();
			positions.Insert(m_Projection.CoordToParent( GetLeftCloseProjectionVector() ));
			positions.Insert(m_Projection.CoordToParent( GetRightCloseProjectionVector() ));
			positions.Insert(m_Projection.CoordToParent( GetLeftFarProjectionVector() ));
			positions.Insert(m_Projection.CoordToParent( GetRightFarProjectionVector() ));
			
			for (int i = 0; i < positions.Count(); ++i)
				DrawSphere(positions[i], conditions[i]);
		}
	}
	
	void DrawDebugArrow(float start, float dist, int color = 0xFF1FFFFF)
	{
		if ( DiagMenu.GetBool(DiagMenuIDs.MISC_HOLOGRAM) )
		{
			vector from = m_Player.GetPosition() + start * MiscGameplayFunctions.GetHeadingVector(m_Player);
			vector to = m_Player.GetPosition() + dist * MiscGameplayFunctions.GetHeadingVector(m_Player);

			Debug.DrawArrow(from, to, 0.5, 0xFF1FFFFF, ShapeFlags.ONCE|ShapeFlags.TRANSP);
		}
	}
	#endif

	bool IsBaseFlat( vector contact_pos_left_close, vector contact_pos_right_close, vector contact_pos_left_far, vector contact_pos_right_far )
	{		
		vector projection_pos = GetProjectionPosition();
		float slope_pos_left_close = Math.AbsFloat(projection_pos[1] - contact_pos_left_close[1]);
		float slope_pos_right_close = Math.AbsFloat(projection_pos[1] - contact_pos_right_close[1]);
		float slope_pos_left_far = Math.AbsFloat(projection_pos[1] - contact_pos_left_far[1]);
		float slope_pos_right_far = Math.AbsFloat(projection_pos[1] - contact_pos_right_far[1]);
		
		bool isTrue = slope_pos_left_close < m_SlopeTolerance && slope_pos_right_close < m_SlopeTolerance && slope_pos_left_far < m_SlopeTolerance && slope_pos_right_far < m_SlopeTolerance;
		#ifdef DIAG_DEVELOPER
		DebugText("IsBaseFlat(must be true): ", true, isTrue, " (slope < slopeTolerance) | slopeTolerance:  " + m_SlopeTolerance + " | lc: " + slope_pos_left_close + " | rc: " + slope_pos_right_close + " | lf: " + slope_pos_left_far + " | rf: " + slope_pos_right_far);
		DrawArrow(projection_pos, contact_pos_left_close, slope_pos_left_close < m_SlopeTolerance);
		DrawArrow(projection_pos, contact_pos_right_close, slope_pos_right_close < m_SlopeTolerance);		
		DrawArrow(projection_pos, contact_pos_left_far, slope_pos_left_far < m_SlopeTolerance);
		DrawArrow(projection_pos, contact_pos_right_far, slope_pos_right_far < m_SlopeTolerance);
		#endif

		return isTrue;
		}
		
	//! Checks if the item can be legally placed (usually checked by action as well)
	bool IsPlacementPermitted()
	{
		if (CfgGameplayHandler.GetDisableIsPlacementPermittedCheck())
			return true;
		ItemBase item = m_Player.GetItemInHands();
		bool isTrue = ( item && item.Type() == GetProjectionEntity().Type() && !item.CanBePlaced(m_Player, GetProjectionPosition()) );
		isTrue = !isTrue; // ??????
		#ifdef DIAG_DEVELOPER
		DebugText("IsPlacementPermitted(must be true): ", true, isTrue, " (Note: CanBePlaced function on item and projection type must be same as item type)");
		#endif
		return isTrue;
	}
	
	//! Checks height relative to player's position
	bool HeightPlacementCheck()
	{
		if (CfgGameplayHandler.GetDisableHeightPlacementCheck())
			return true;
		if ( GetProjectionEntity() ) //simple height check
		{
			vector playerpos = m_Player.GetPosition();
			vector projectionpos = GetProjectionPosition();
			float delta1 = playerpos[1] - projectionpos[1];
			
			if ( delta1 > DEFAULT_MAX_PLACEMENT_HEIGHT_DIFF || delta1 < -DEFAULT_MAX_PLACEMENT_HEIGHT_DIFF )
			{
				#ifdef DIAG_DEVELOPER
				DebugText("HeightPlacementCheck(must be true): ", true, false, " | Height difference between item and player is larger than " + DEFAULT_MAX_PLACEMENT_HEIGHT_DIFF);
				#endif
				return false;
			}
		}
		#ifdef DIAG_DEVELOPER
		DebugText("HeightPlacementCheck(must be true): ", true, true);
		#endif
		return true;
	}
	
	bool IsUnderwater()
	{
		if (CfgGameplayHandler.GetDisableIsUnderwaterCheck())
			return false;
		// Fast check middle of object
		string type;
		int liquid;
		g_Game.SurfaceUnderObject(m_Projection, type, liquid);
		
		if (liquid == LIQUID_WATER)
		{
			#ifdef DIAG_DEVELOPER
			DebugText("IsUnderwater: ", false, true, " | Surface under object is water");
			#endif
			return true;
		}
		
		// Check every corner of the object
		vector left_close = m_Projection.CoordToParent( GetLeftCloseProjectionVector() );
		vector right_close = m_Projection.CoordToParent( GetRightCloseProjectionVector() );
		vector left_far = m_Projection.CoordToParent( GetLeftFarProjectionVector() );
		vector right_far = m_Projection.CoordToParent( GetRightFarProjectionVector() );
		bool surface_sea_water = IsSurfaceSea(left_close) || IsSurfaceSea(right_close) || IsSurfaceSea(left_far) || IsSurfaceSea(right_far);
		
		#ifdef DIAG_DEVELOPER
		// I'd rather duplicate this on internal than introduce (even) more raycasts than needed on retail..
		float lc = g_Game.GetWaterDepth(left_close);
		float rc = g_Game.GetWaterDepth(right_close);
		float lf = g_Game.GetWaterDepth(left_far);
		float rf = g_Game.GetWaterDepth(right_far);
		bool isTrue = (lc > 0 || rc > 0 || lf > 0 || rf > 0 || surface_sea_water);
		DebugText("IsUnderwater: ", false, isTrue, " surface_sea_water: " + surface_sea_water + " | (all must be less than zero) | lc: " + lc + " | rc: " + rc + " | lf: " + lf + " | rf: " + rf);
		//DebugText("Corner Height: ", false, true, " lc: " + left_close[1] + " | rc: " + right_close[1] + " | lf: " + left_far[1] + " | rf: " + right_far[1]);
		if (isTrue)
		{
			array<bool> conditions = {lc <= 0, rc <= 0, lf <= 0, rf <= 0};
			DrawBaseSpheres(conditions);
		}
		#endif
		
		return (surface_sea_water || g_Game.GetWaterDepth(left_close) > 0 || g_Game.GetWaterDepth(right_close) > 0 || g_Game.GetWaterDepth(left_far) > 0 || g_Game.GetWaterDepth(right_far) > 0);	
	}
	
	bool IsInTerrain()
	{
		if (CfgGameplayHandler.GetDisableIsInTerrainCheck())
			return false;
		vector fromHeightOffset = "0 0.3 0";
		vector toHeightOffset = "0 1 0";
		
		vector from_left_close = m_Projection.CoordToParent( GetLeftCloseProjectionVector() ) + fromHeightOffset;
		vector to_left_close_down = from_left_close + toHeightOffset;

		vector from_right_close = m_Projection.CoordToParent( GetRightCloseProjectionVector() ) + fromHeightOffset;
		vector to_right_close_down = from_right_close + toHeightOffset;

		vector from_left_far = m_Projection.CoordToParent( GetLeftFarProjectionVector() ) + fromHeightOffset;
		vector to_left_far_down = from_left_far + toHeightOffset;

		vector from_right_far = m_Projection.CoordToParent( GetRightFarProjectionVector() ) + fromHeightOffset;
		vector to_right_far_down = from_right_far + toHeightOffset;

		vector contact_pos_left_close;
		vector contact_pos_right_close;
		vector contact_pos_left_far;
		vector contact_pos_right_far;
		
		vector contact_dir_left_close;
		vector contact_dir_right_close;
		vector contact_dir_left_far;
		vector contact_dir_right_far;
		
		int contact_component_left_close;
		int contact_component_right_close;
		int contact_component_left_far;
		int contact_component_right_far;
		
		#ifdef DIAG_DEVELOPER
		// I'd rather duplicate this on internal than introduce (even) more raycasts than needed on retail..
		set<Object> lcO = new set<Object>();
		set<Object> rcO = new set<Object>();
		set<Object> lfO = new set<Object>();
		set<Object> rfO = new set<Object>();
		bool lc = DayZPhysics.RaycastRV( from_left_close, to_left_close_down, contact_pos_left_close, contact_dir_left_close, contact_component_left_close, lcO, m_Projection, m_Projection, false, true, ObjIntersectFire );
		bool rc = DayZPhysics.RaycastRV( from_right_close, to_right_close_down, contact_pos_right_close, contact_dir_right_close, contact_component_right_close, rcO, m_Projection, m_Projection, false, true, ObjIntersectFire );
		bool lf = DayZPhysics.RaycastRV( from_left_far, to_left_far_down, contact_pos_left_far, contact_dir_left_far, contact_component_left_far, lfO, m_Projection, m_Projection, false, true, ObjIntersectFire );
		bool rf = DayZPhysics.RaycastRV( from_right_far, to_right_far_down, contact_pos_right_far, contact_dir_right_far, contact_component_right_far, rfO, m_Projection, m_Projection, false, true, ObjIntersectFire );
		bool isTrue = ( lc || rc || lf || rf );
		string text = "";		
		if (isTrue)
		{
			if (lc)
				text += " | lc";
			if (rc)
				text += " | rc";
			if (lf)
				text += " | lf";
			if (rf)
				text += " | rf";
			
			if (lcO.Count() > 0)
				text += " | lcO: " + lcO[0];
			if (rcO.Count() > 0)
				text += " | rcO: " + rcO[0];
			if (lfO.Count() > 0)
				text += " | lfO: " + lfO[0];
			if (rfO.Count() > 0)
				text += " | rfO: " + rfO[0];
			
			array<bool> conditions = {!lc, !rc, !lf, !rf};
			DrawBaseSpheres(conditions);
		}
		DebugText("IsInTerrain: ", false, isTrue, text);
		#endif

		if (DayZPhysics.RaycastRV( from_left_close, to_left_close_down, contact_pos_left_close, contact_dir_left_close, contact_component_left_close, NULL, m_Projection, m_Projection, false, true, ObjIntersectFire ))
			return true;

		if (DayZPhysics.RaycastRV( from_right_close, to_right_close_down, contact_pos_right_close, contact_dir_right_close, contact_component_right_close, NULL,m_Projection, m_Projection, false, true, ObjIntersectFire ))
			return true;

		if (DayZPhysics.RaycastRV( from_left_far, to_left_far_down, contact_pos_left_far, contact_dir_left_far, contact_component_left_far, NULL, m_Projection, m_Projection, false, true, ObjIntersectFire ))
			return true;

		if (DayZPhysics.RaycastRV( from_right_far, to_right_far_down, contact_pos_right_far, contact_dir_right_far, contact_component_right_far, NULL, m_Projection, m_Projection, false, true, ObjIntersectFire ))
			return true;
		
		return false;
	}
	
	void CheckPowerSource()
	{
		//in range of its power source.
		if ( m_Player && m_Parent && m_Parent.HasEnergyManager() && m_Parent.GetCompEM().IsPlugged() )
		{
			// Unplug the device when the player is too far from the power source.
			m_Parent.GetCompEM().UpdatePlugState();
			
			// Delete local hologram when plug is rippled out and advanced placement is active
			if( GetGame().IsMultiplayer() && GetGame().IsClient() )
			{
				if (!m_Parent.GetCompEM().IsPlugged())
					m_Player.TogglePlacingLocal();
			}
		}				
	}
	
	EntityAI PlaceEntity( EntityAI entity_for_placing )
	{
		//string-based comparison
		if (entity_for_placing.IsInherited(TentBase) || entity_for_placing.IsBasebuildingKit() )
		{
			return entity_for_placing;
		}
		
		if (m_Projection.IsInherited(GardenPlotPlacing))
		{
			Class.CastTo(entity_for_placing, GetGame().CreateObjectEx( "GardenPlot", m_Projection.GetPosition(), ECE_OBJECT_SWAP ));
			return entity_for_placing;
		}
		
		//inheritance comparison
		if( !GetProjectionEntity().IsKindOf( m_Parent.GetType() ))
		{
			Class.CastTo(entity_for_placing, GetGame().CreateObjectEx( m_Projection.GetType(), m_Projection.GetPosition(), ECE_OBJECT_SWAP ));
		}
		
		return entity_for_placing;
	}
	
	protected void GetProjectionCollisionBox( out vector min_max[2] )
	{
		if (!m_Projection.GetCollisionBox( min_max ) && m_Projection.MemoryPointExists("box_placing_min"))
		{
			min_max[0] = m_Projection.GetMemoryPointPos( "box_placing_min" );
			min_max[1] = m_Projection.GetMemoryPointPos( "box_placing_max" );
			//Debug.DrawSphere(m_Projection.ModelToWorld(min_max[0]) , 0.8,Colors.RED, ShapeFlags.ONCE);
			//Debug.DrawSphere(m_Projection.ModelToWorld(min_max[1]), 0.8,Colors.RED, ShapeFlags.ONCE);
		}
	}
	
	protected vector GetCollisionBoxSize( vector min_max[2] )
	{
		vector box_size = Vector(1,1,1);
		
		box_size[0] = min_max[1][0] - min_max[0][0];
		box_size[2] = min_max[1][2] - min_max[0][2];
		box_size[1] = min_max[1][1] - min_max[0][1];
		
		return box_size;
	}

	vector GetLeftCloseProjectionVector()
	{
		vector min_max[2];
		GetProjectionCollisionBox( min_max );

		return min_max[0];
	}

	vector GetRightCloseProjectionVector()
	{
		vector min_max[2];
		GetProjectionCollisionBox( min_max );
		min_max[1][1] = min_max[0][1];
		min_max[1][2] = min_max[0][2];

		return min_max[1];
	}

	vector GetLeftFarProjectionVector()
	{
		vector min_max[2];
		GetProjectionCollisionBox( min_max );
		min_max[0][2] = min_max[1][2];

		return min_max[0];
	}

	vector GetRightFarProjectionVector()
	{
		vector min_max[2];
		GetProjectionCollisionBox( min_max );
		min_max[1][1] = min_max[0][1]; 

		return min_max[1];
	}

	// Replaced by IsUnderwater, currently unused
	bool IsSurfaceWater( vector position )
	{
		CGame game = GetGame();
		return game.SurfaceIsSea( position[0], position[2] ) || game.SurfaceIsPond( position[0], position[2] );
	}
	
	bool IsSurfaceSea( vector position )
	{
		CGame game = GetGame();
		return game.SurfaceIsSea( position[0], position[2] );
	}

	protected vector GetProjectionEntityPosition( PlayerBase player )
	{
		float minProjectionDistance;
		float maxProjectionDistance; 
		m_ContactDir = vector.Zero;
		vector minMax[2];
		float projectionRadius = GetProjectionRadius();
		float cameraToPlayerDistance = vector.Distance(GetGame().GetCurrentCameraPosition(), player.GetPosition());

		if (projectionRadius < SMALL_PROJECTION_RADIUS) // objects with radius smaller than 1m
		{
			minProjectionDistance = SMALL_PROJECTION_RADIUS;
			maxProjectionDistance = SMALL_PROJECTION_RADIUS * 2;
		}
		else
		{
			minProjectionDistance = projectionRadius;
			maxProjectionDistance = projectionRadius * 2;
			maxProjectionDistance = Math.Clamp(maxProjectionDistance, SMALL_PROJECTION_RADIUS, LARGE_PROJECTION_DISTANCE_LIMIT);
		}
		
		vector from = GetGame().GetCurrentCameraPosition();
		//adjusts raycast origin to player head approx. level (limits results behind the character)
		if ( DayZPlayerCamera3rdPerson.Cast(player.GetCurrentCamera()) )
		{
			vector head_pos;
			MiscGameplayFunctions.GetHeadBonePos(player,head_pos);
			float dist = vector.Distance(head_pos,from);
			from = from + GetGame().GetCurrentCameraDirection() * dist;
		}
		
		vector to = from + (GetGame().GetCurrentCameraDirection() * (maxProjectionDistance + cameraToPlayerDistance));
		vector contactPosition;
		set<Object> hitObjects = new set<Object>();

		DayZPhysics.RaycastRV(from, to, contactPosition, m_ContactDir, m_ContactComponent, hitObjects, player, m_Projection, false, false, ObjIntersectFire);
		
		bool contactHitProcessed = false;
		//! will not push hologram up when there is direct hit of an item
		if (!CfgGameplayHandler.GetDisableIsCollidingBBoxCheck())
		{
			if (hitObjects.Count() > 0)
			{
				if (hitObjects[0].IsInherited(Watchtower))
				{
					contactHitProcessed = true;
					contactPosition = CorrectForWatchtower(m_ContactComponent, contactPosition, player, hitObjects[0]);
				}
				
				if (!contactHitProcessed && hitObjects[0].IsInherited(InventoryItem))
					contactPosition = hitObjects[0].GetPosition();
			}
		}

		static const float raycastOriginOffsetOnFail = 0.25;
		static const float minDistFromStart = 0.01;
		// Camera isn't correctly positioned in some cases, leading to raycasts hitting the object directly behind the camera
		if ((hitObjects.Count() > 0) && (vector.DistanceSq(from, contactPosition) < minDistFromStart))
		{
			from = contactPosition + GetGame().GetCurrentCameraDirection() * raycastOriginOffsetOnFail;
			DayZPhysics.RaycastRV(from, to, contactPosition, m_ContactDir, m_ContactComponent, hitObjects, player, m_Projection, false, false, ObjIntersectFire);
		}

		bool isFloating = SetHologramPosition(player.GetPosition(), minProjectionDistance, maxProjectionDistance, contactPosition);
		SetIsFloating(isFloating);

		#ifdef DIAG_DEVELOPER
		DrawDebugArrow(minProjectionDistance, maxProjectionDistance);
		if (DiagMenu.GetBool(DiagMenuIDs.MISC_HOLOGRAM))
		{
			Debug.DrawSphere(GetProjectionPosition(), 0.1, 0x99FF0000, ShapeFlags.ONCE|ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE);
		}
		#endif
		
		m_FromAdjusted = from;

		return contactPosition;
	}
	
	/**  
	\brief Sets hologram position based on player and projection distance
	@param startPosition start position
	@param minProjectionDistance lower distance limit
	@param maxProjectionDistance higher distance limit
	@param inout contactPosition is position of the hologram contact with ground/object 	
	@return true if hologram is floating (is on the near or far edge)
	*/
	protected bool SetHologramPosition(vector startPosition, float minProjectionDistance, float maxProjectionDistance, inout vector contactPosition)
	{
		float playerToProjectionDistance = vector.Distance(startPosition, contactPosition);
		vector playerToProjection;
		
		#ifdef DIAG_DEVELOPER
		DebugText("SetHologramPosition::startPosition: ", false, m_IsHidden, string.Format(" | %1", startPosition));
		DebugText("SetHologramPosition::contactPosition [in]: ", false, m_IsHidden, string.Format(" | %1", contactPosition));
		DebugText("SetHologramPosition::minProjectionDistance: ", false, m_IsHidden, string.Format(" | %1", minProjectionDistance));
		DebugText("SetHologramPosition::maxProjectionDistance: ", false, m_IsHidden, string.Format(" | %1", maxProjectionDistance));
		DebugText("SetHologramPosition::playerToProjectionDistance: ", false, m_IsHidden, string.Format(" | %1", playerToProjectionDistance));
		#endif

		//hologram is at min distance from player
		if (playerToProjectionDistance <= minProjectionDistance)
		{
			playerToProjection = contactPosition - startPosition;
			playerToProjection.Normalize();
			//prevents the hologram to go underground/floor while hologram exceeds minProjectionDistance
			playerToProjection[1] = playerToProjection[1] + PROJECTION_TRANSITION_MIN;
			
			contactPosition = startPosition + (playerToProjection * minProjectionDistance);
			
			#ifdef DIAG_DEVELOPER
			DebugText("SetHologramPosition::contactPosition[out] (< minProjectDistance): ", false, m_IsHidden, string.Format(" | %1", contactPosition));
			#endif

			return true;
		}
		//hologram is at max distance from player
		else if (playerToProjectionDistance >= maxProjectionDistance)
		{
			playerToProjection = contactPosition - startPosition;
			playerToProjection.Normalize();
			//prevents the hologram to go underground/floor while hologram exceeds maxProjectionDistance
			playerToProjection[1] = playerToProjection[1] + PROJECTION_TRANSITION_MAX;
			
			contactPosition = startPosition + (playerToProjection * maxProjectionDistance);
			
			#ifdef DIAG_DEVELOPER
			DebugText("SetHologramPosition::contactPosition[out] (< maxProjectionDistance): ", false, m_IsHidden, string.Format(" | %1", contactPosition));
			#endif
		
			return true;
		}
		
		return false;
	}
	
	bool IsFenceOrWatchtowerKit()
	{
		return m_Parent.IsBasebuildingKit() || m_Parent.IsInherited(TentBase);
	}
	
	vector CorrectForWatchtower(int contactComponent, vector contactPos, PlayerBase player, Object hitObject)
	{
		// Raycast has hit one of the trigger boxes that show construction prompts, so projection would be floating in the air without this correction
		if (m_WatchtowerIgnoreComponentNames.Find(hitObject.GetActionComponentName(contactComponent, LOD.NAME_VIEW)) != -1 )
			contactPos[1] = hitObject.GetActionComponentPosition(contactComponent, LOD.NAME_VIEW)[1];
		
		return contactPos;
	}
	
	//This function is currently unused
	bool IsProjectionTrap()
	{
		return m_Projection.IsInherited( TrapBase ) || m_Projection.IsInherited( TrapSpawnBase );
	}

	float GetProjectionDiameter()
	{		
		float diameter;
		float radius;
		vector diagonal;
		vector min_max[2];

		GetProjectionCollisionBox( min_max );
		diagonal = GetCollisionBoxSize( min_max );
		diameter = diagonal.Length();

		return diameter;	
	}

	float GetProjectionRadius()
	{		
		float diameter;
		float radius;
		vector diagonal;
		vector min_max[2];

		GetProjectionCollisionBox( min_max );
		diagonal = GetCollisionBoxSize( min_max );
		diameter = diagonal.Length();
		radius = diameter / 2;

		return radius;	
	}
		
	void SetUpdatePosition( bool state )
	{
		m_UpdatePosition = state;
	}

	bool GetUpdatePosition()
	{
		return m_UpdatePosition;
	}

	EntityAI GetParentEntity()
	{
		return m_Parent;
	}
	
	void SetProjectionEntity( EntityAI projection )
	{
		m_Projection = projection;
	}

	EntityAI GetProjectionEntity()
	{
		return m_Projection;
	}
	
	void SetIsFloating( bool is_floating )
	{
		m_IsFloating = is_floating;
	}
		
	void SetIsColliding( bool is_colliding )
	{
		#ifdef DIAG_DEVELOPER
		DebugText("Is colliding: ", false, is_colliding);
		#endif
		m_IsColliding = is_colliding;
	}

	void SetIsHidden( bool is_hidden )
	{
		m_IsHidden = is_hidden;
	}
	
	void SetIsCollidingPlayer( bool is_colliding )
	{
		m_IsCollidingPlayer = is_colliding;
	}

	void SetIsCollidingGPlot( bool is_colliding_gplot )
	{
		m_IsCollidingGPlot = is_colliding_gplot;
	}

	bool IsFloating()
	{
		#ifdef DIAG_DEVELOPER
		DebugText("IsFloating: ", false, m_IsFloating);
		#endif
		return m_IsFloating;
	}

	bool IsColliding()
	{
		return m_IsColliding;
	}
	
	bool IsHidden()
	{
		#ifdef DIAG_DEVELOPER
		DebugText("IsHidden: ", false, m_IsHidden);
		#endif
		return m_IsHidden;
	}

	bool IsCollidingPlayer()
	{
		if (CfgGameplayHandler.GetDisableIsCollidingPlayerCheck())
			return false;
		#ifdef DIAG_DEVELOPER
		DebugText("IsCollidingPlayer: ", false, m_IsCollidingPlayer);
		#endif
		return m_IsCollidingPlayer;
	}

	void SetProjectionPosition(vector position)
	{
		m_Projection.SetPosition( position );
		
		if (IsFloating())
		{
			m_Projection.SetPosition(SetOnGround(position));
		}
	}

	void SetProjectionOrientation(vector orientation)
	{	
		m_Projection.SetOrientation(orientation);
	}
	
	vector GetProjectionRotation()
	{
		return m_Rotation;
	}
	
	void AddProjectionRotation( float addition )
	{	
		m_Rotation[0] = m_Rotation[0] + addition;
	}
	
	void SubtractProjectionRotation( float subtraction )
	{	
		m_Rotation[0] = m_Rotation[0] - subtraction;
	}
	
	vector SetOnGround( vector position )
	{
		vector from = position;
		vector ground;
		vector player_to_projection_vector;
		float projection_diameter = GetProjectionDiameter();
			
		ground = Vector(0, -Math.Max(projection_diameter, SMALL_PROJECTION_GROUND), 0);
		
		vector to = from + ground;
		vector contact_pos = to;
		
		RaycastRVParams rayInput = new RaycastRVParams(from, to, m_Projection);
		rayInput.flags = CollisionFlags.ALLOBJECTS;
		array<ref RaycastRVResult> results = new array<ref RaycastRVResult>;
		
		if (DayZPhysics.RaycastRVProxy(rayInput, results))
		{
			RaycastRVResult res;
			for (int i = 0; i < results.Count(); i++)
			{
				res = results.Get(i);
				if (res.entry || (!res.obj && !res.parent))
				{
					contact_pos = res.pos;
					break;
				}
			}
		}
		
		//LOS check
		if (contact_pos != "0 0 0")
		{
			vector check_pos;
			vector check_dir;
			int check_component = -1;
			set<Object> hit_object = new set<Object>;
			to = contact_pos;
			to[1] = to[1] + 0.1;
			from = m_FromAdjusted;
			
			if (DayZPhysics.RaycastRV(from, to, check_pos, check_dir, check_component, hit_object, null, m_Player, false, false, ObjIntersectFire))
			{
				if ((hit_object.Count() > 0)&& (!hit_object[0].IsInherited(Watchtower) || (hit_object[0].IsInherited(Watchtower) && (m_WatchtowerIgnoreComponentNames.Find(hit_object[0].GetActionComponentName(check_component, LOD.NAME_VIEW)) == -1))))
				{
					contact_pos = "0 0 0";
				}
			}
		}
		
		HideWhenClose(contact_pos);

		return contact_pos;
	}

	vector HideWhenClose( vector pos )
	{
		//if the hologram is too close to player when he looks to the sky, send the projection to away
		vector cam_dir = GetGame().GetCurrentCameraDirection();

		if( cam_dir[1] > LOOKING_TO_SKY )
		{
			pos = "0 0 0";
		}

		return pos;
	}

	vector GetProjectionPosition()
	{
		if (m_Projection)
			return m_Projection.GetPosition();
		
		return vector.Zero;
	}
	
	vector GetProjectionOrientation()
	{
		if (m_Projection)
			return m_Projection.GetOrientation();
		
		return vector.Zero;
	}
	
	vector GetDefaultOrientation()
	{
		m_DefaultOrientation = GetGame().GetCurrentCameraDirection().VectorToAngles();
		m_DefaultOrientation[1] = 0;
		
		if (!GetParentEntity().PlacementCanBeRotated())
		{
			m_DefaultOrientation = vector.Zero;
		}
		
		return m_DefaultOrientation;
	}

	int GetHiddenSelection( string selection )
	{	
		int idx = m_Projection.GetHiddenSelectionIndex(selection);

		if ( idx != -1 )
			return idx;
		else
			return 0;
	}

	// the function accepts string
	void SetSelectionToRefresh( string selection )
	{
		m_SelectionsToRefresh.Insert( selection );
	}

	//overloaded function to accept array of strings
	void SetSelectionToRefresh( array<string> selection )
	{
		for ( int i = 0; i < selection.Count(); i++ )
		{
			m_SelectionsToRefresh.Insert( selection.Get(i) );
		}
	}
	
	void RefreshVisual()
	{
		if (m_Projection)
		{
			static const string textureName = "#(argb,8,8,3)color(0.5,0.5,0.5,0.75,ca)";

			int hidden_selection 			= 0;
			string selection_to_refresh;
			string config_material 			= string.Format("CfgVehicles %1 hologramMaterial", m_Projection.GetType());
			string hologram_material 		= GetGame().ConfigGetTextOut(config_material);
			string config_model 			= string.Format("CfgVehicles %1 hologramMaterialPath", m_Projection.GetType());
			string hologram_material_path 	= string.Format("%1\\%2%3", GetGame().ConfigGetTextOut(config_model), hologram_material, CorrectMaterialPathName());
			
			for (int i = 0; i < m_SelectionsToRefresh.Count(); ++i)
			{
				selection_to_refresh = m_SelectionsToRefresh.Get(i);
				hidden_selection = GetHiddenSelection(selection_to_refresh);
				m_Projection.SetObjectTexture(hidden_selection, textureName);
				m_Projection.SetObjectMaterial(hidden_selection, hologram_material_path);
			}
		}
	}
	
	// Returns correct string to append to material path name
	string CorrectMaterialPathName()
	{
		if (IsColliding() || IsFloating())
		{
			return SUFFIX_MATERIAL_UNDEPLOYABLE;
		}
		else if (m_Parent.HasEnergyManager())
		{
			ComponentEnergyManager comp_em = m_Parent.GetCompEM();
			string SEL_CORD_PLUGGED = m_Parent.GetCompEM().SEL_CORD_PLUGGED;
			string SEL_CORD_FOLDED = m_Parent.GetCompEM().SEL_CORD_FOLDED;
			
			if (comp_em.IsPlugged() && comp_em.IsEnergySourceAtReach(GetProjectionPosition()))
			{
				m_Projection.SetAnimationPhase(SEL_CORD_PLUGGED, 0);
				m_Projection.SetAnimationPhase(SEL_CORD_FOLDED, 1);	
				return SUFFIX_MATERIAL_POWERED;
			}
			else
			{
				m_Projection.SetAnimationPhase(SEL_CORD_PLUGGED, 1);
				m_Projection.SetAnimationPhase(SEL_CORD_FOLDED, 0);
			}
		}

		return SUFFIX_MATERIAL_DEPLOYABLE;
	}
};

class ProjectionTrigger extends Trigger
{
	protected int m_TriggerUpdateMs;
	protected Hologram m_ParentObj;
	protected PlayerBase m_Player;

	override void OnEnter( Object obj )
	{
		//Print("OnEnter");
		if ( m_ParentObj )
		{
			m_ParentObj.SetIsCollidingPlayer( true );
			m_TriggerUpdateMs = 50;
		}
	}

	override void OnLeave( Object obj )
	{
		//Print("OnLeave");
		if ( m_ParentObj )
		{
			m_ParentObj.SetIsCollidingPlayer( false );
		}
	}

	override protected void UpdateInsiders(int timeout)
	{
		super.UpdateInsiders(m_TriggerUpdateMs);
	}
	
	void SetParentObject( Hologram projection )
	{
		m_ParentObj = projection;
	}

	void SetParentOwner( PlayerBase player )
	{
		m_Player = player;
	}
}
