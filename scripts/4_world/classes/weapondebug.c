enum eWeaponDebugMode
{
    NORMAL,
	LOCAL,
	COUNT
};

class WeaponDebug
{
	const int BUFFER_SIZE = 1000;
	
	int m_BufferIndex;
	int m_PointCount = 0;
	bool m_IsDrawKeyHeldDownPrev;
	bool m_IsDrawKeyHeldDown;
	bool m_IsFireKeyPressed;
	float m_TargetDistance;
	eWeaponDebugMode m_CurrentMode;
	vector m_AimTrailCyclic[BUFFER_SIZE];	
	vector m_AimTrailOrdered[BUFFER_SIZE];
	
	ref map<int, string> m_DebugModesNames = new map<int, string>;
	
	//ref array<Selection> m_Selections = new array<Selection>();
	
	Shape m_HitShape;
	Shape m_ShapeTrailLines;
	Shape m_ShapeFireDirCamera;
	//Shape temp_shape;
	Shape m_PermanentLine1;
	Shape m_PermanentLine2;
	
	Weapon GetWeaponInHands()
	{
		Weapon weaponInHands;
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player)
		{
			Class.CastTo(weaponInHands,  player.GetItemInHands());
		}
		
		return weaponInHands;
	}
	
	void WeaponDebug()
	{
		m_DebugModesNames.Insert(eWeaponDebugMode.NORMAL, "Mode "+eWeaponDebugMode.NORMAL.ToString()+": normal");
		m_DebugModesNames.Insert(eWeaponDebugMode.LOCAL,"Mode "+eWeaponDebugMode.LOCAL.ToString()+": Ignore mouse move ");
	}
	
	void ~WeaponDebug()
	{
		RemoveAllShapes(true);
		
	}

	void RemoveAllShapes(bool is_exit = false)
	{
		Debug.RemoveShape(m_ShapeTrailLines);
		Debug.RemoveShape(m_ShapeFireDirCamera);
		Debug.RemoveShape(m_HitShape);
		
		if ( is_exit )
		{
			Debug.RemoveShape(m_PermanentLine1);
			Debug.RemoveShape(m_PermanentLine2);
		}
	}


	void OnCommandHandlerUpdate()
	{
		m_IsFireKeyPressed = false;
		
		if (KeyState(KeyCode.KC_P) == 1)
		{
			if (!m_IsDrawKeyHeldDown)
			{
				OnKeyDown(KeyCode.KC_P);
			}
			m_IsDrawKeyHeldDown = true;
		}
		else
		{
			if ( m_IsDrawKeyHeldDown )
			{
				//OnKeyUp();
			}
			m_IsDrawKeyHeldDown = false;
		}
		
		if (KeyState(KeyCode.KC_Z) == 1)
		{
			ClearKey(KeyCode.KC_Z);
			CycleWeaponDebugMode();
			m_PointCount = 0;
		}
		
		if (KeyState(KeyCode.KC_X) == 1)
		{
			m_IsFireKeyPressed = true;
			ClearKey(KeyCode.KC_X);
		}
	}

	void OnKeyDown(KeyCode key)
	{
		if (key == KeyCode.KC_X)
		{
		}
		
		if (key == KeyCode.KC_P)
		{
			m_PointCount = 0;
		}
	}
	
	void CycleWeaponDebugMode()
	{
		m_CurrentMode++;
		if ( m_CurrentMode == eWeaponDebugMode.COUNT )
		{
			m_CurrentMode = 0;
		}
	
	}
	
	void OnPostFrameUpdate()
	{
		Weapon weapon = GetWeaponInHands();
		if (weapon)
		{
			RemoveAllShapes();
			
			vector cameraDirection = g_Game.GetCurrentCameraDirection();
			vector cameraPosition = g_Game.GetCurrentCameraPosition();
			vector usti_hlavne_position = weapon.GetSelectionPositionLS( "usti hlavne" );//usti hlavne
			usti_hlavne_position = weapon.ModelToWorld(usti_hlavne_position);
			
			DrawLineOfFireMuzzleToHit(usti_hlavne_position, cameraDirection, cameraPosition);

			if (m_PointCount)
			{
				if (m_CurrentMode == eWeaponDebugMode.LOCAL)
				{
					m_ShapeTrailLines = Debug.DrawLines( m_AimTrailOrdered,m_PointCount, COLOR_YELLOW, ShapeFlags.NOZBUFFER );//TODO Local space -> world space (player camera)
				}
				else
				{
					m_ShapeTrailLines = Debug.DrawLines( m_AimTrailOrdered,m_PointCount, COLOR_YELLOW, ShapeFlags.NOZBUFFER );
				}
			}
		
			DisplayGeneralInfo();
			DisplayTargetInfo();
		}
	}

	void AddPosToCyclicBuffer(vector pos)
	{
		m_AimTrailCyclic[m_BufferIndex] = pos;
		m_BufferIndex++;
		if (m_BufferIndex == BUFFER_SIZE)
		{
			m_BufferIndex = 0;			
		}
		if (m_PointCount < BUFFER_SIZE)
		{
			m_PointCount++;
		}
	}	
	
	void OrderTrailArray()
	{
		int unordered_index;
		for (int i = 0; i < m_PointCount; i++)
		{
			unordered_index = m_BufferIndex - 1 - i;
			if ( unordered_index < 0 )
			{
				unordered_index = BUFFER_SIZE + unordered_index;
			}
			m_AimTrailOrdered[i] = m_AimTrailCyclic[unordered_index];
		}
	}	

	void DisplayGeneralInfo()
	{
		DbgUI.Begin("sway weight", 50, 50);
		DayZPlayerImplement player = DayZPlayerImplement.Cast(g_Game.GetPlayer());
		float sway_weight = player.GetAimingModel().GetSwayWeight();
		DbgUI.Text("value: " + sway_weight.ToString());
		DbgUI.Text("Hold P to draw debug line");
		DbgUI.Text("Press X to simulate fire");
		//DbgUI.Text("Press Z to cycle debug modes");
		//DbgUI.Text("Debug " +m_DebugModesNames.Get(m_CurrentMode) );
		//DbgUI.Text("");
		DbgUI.End();
	}

	void DisplayTargetInfo()
	{
		DbgUI.Begin("target distance", 50,200);
		DbgUI.Text("value: " + m_TargetDistance.ToString());
		DbgUI.End();
	}

	void DrawLineOfFireMuzzleToHit(vector begin_point, vector camera_dir, vector camera_pos)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player.IsRaised())
		{
			vector transform[4];
			player.GetTransformWS(transform);
			
			HumanCommandWeapons hcw = player.GetCommandModifier_Weapons();
			HumanInputController hic = player.GetInputController();
			
			float UpDownBase = hcw.GetBaseAimingAngleUD();
			float LeftRightBase = hcw.GetBaseAimingAngleLR();
			float UpDownOffset = hcw.GetAimingHandsOffsetUD();
			float LeftRightOffset = hcw.GetAimingHandsOffsetLR();
	
			
			vector aimAngles = Vector(LeftRightBase + LeftRightOffset, UpDownBase + UpDownOffset, 0.0);
			vector plrAngles = Math3D.MatrixToAngles(transform);	
	
			vector resAngle = aimAngles + plrAngles;
			
			vector start = camera_pos;
			//MiscGameplayFunctions.GetHeadBonePos(player, start);

			vector direction = resAngle.AnglesToVector();
			
			float range = 50.0;
			
			vector end = start + (direction * range);
			vector contact;
			vector contactDir;
			int muz;
			
			Debug.RemoveShape(m_HitShape);
			if ( DayZPhysics.RaycastRV(start, end, contact, contactDir, muz, null, null, player, false, false, ObjIntersectFire, 0.0) )
			{
				m_TargetDistance =  vector.Distance( player.GetPosition(), contact);
				m_HitShape = Debug.DrawSphere(contact, (m_TargetDistance / 150), COLOR_YELLOW);
				m_ShapeFireDirCamera = Debug.DrawLine(begin_point, contact, Colors.RED, ShapeFlags.NOZBUFFER );
				
	
				if (m_IsDrawKeyHeldDown)
				{
					if (m_CurrentMode == eWeaponDebugMode.LOCAL) 
						AddPosToCyclicBuffer(contact);//TODO change to local space
					else
						AddPosToCyclicBuffer(contact);
					OrderTrailArray();
				}
				
				if ( m_IsFireKeyPressed )
				{
					Debug.RemoveShape(m_PermanentLine1);
					Debug.RemoveShape(m_PermanentLine2);
					m_PermanentLine1 = Debug.DrawLine(begin_point, contact, Colors.RED, ShapeFlags.NOZBUFFER );
					m_PermanentLine2 = Debug.DrawLine(camera_pos, contact, Colors.GREEN, ShapeFlags.NOZBUFFER );
				}
				
				
				
				
			}
			else
			{
				m_HitShape = Debug.DrawSphere(end, 0.15, COLOR_RED);
				
				m_ShapeFireDirCamera = Debug.DrawLine(begin_point, end, Colors.GREEN, ShapeFlags.NOZBUFFER );
				m_TargetDistance = -1;
				
				if (m_IsDrawKeyHeldDown)
				{
					if (m_CurrentMode == eWeaponDebugMode.LOCAL) 
						AddPosToCyclicBuffer(end);//TODO change to local space
					else
						AddPosToCyclicBuffer(end);
					OrderTrailArray();
				}
				
				if ( m_IsFireKeyPressed )
				{
					Debug.RemoveShape(m_PermanentLine1);
					Debug.RemoveShape(m_PermanentLine2);
					m_PermanentLine1 = Debug.DrawLine(begin_point, end, Colors.RED, ShapeFlags.NOZBUFFER );
					m_PermanentLine2 = Debug.DrawLine(camera_pos, end, Colors.GREEN, ShapeFlags.NOZBUFFER );
				}
			}
		}
	}	
}