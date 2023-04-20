class CrossHair
{
	protected string					m_Name;
	protected bool 						m_Shown;
	protected bool 						m_Current;

	protected Widget 					m_Widget;
	
	void CrossHair(Widget w)
	{
		m_Widget = w;
		m_Name = w.GetName();
		m_Shown = true;
		m_Current = false;
	}

	void ~CrossHair() {}

	string GetName()
	{
		return m_Name;
	}

	bool IsCurrent()
	{
		return m_Current;
	}

	bool IsShown()
	{
		return m_Shown;
	}

	void Show()
	{
		m_Shown = false;
		m_Current = true;
		m_Widget.Show(true);
	}
	
	void Hide()
	{
		m_Shown = true;
		m_Current = false;
		m_Widget.Show(false);
	}
	
	Widget GetWidget()
	{
		return m_Widget;
	}
}

class CrossHairSelector extends ScriptedWidgetEventHandler
{
	protected PlayerBase 					m_Player;
	protected ActionManagerBase 			m_AM;
	
	protected Widget					m_Root;

	protected ref set<ref CrossHair>	m_CrossHairs;

	//! Floating crosshair
	protected vector m_PreviousDirection;
	protected bool m_PreviousDirectionSet;
	
	void CrossHairSelector()
	{
		m_Player 		= null;
		m_AM 			= null;
		m_CrossHairs 	= new set<ref CrossHair>;

		GetGame().GetPostUpdateQueue(CALL_CATEGORY_GUI).Insert(Update);
	}
		
	void ~CrossHairSelector()
	{
		GetGame().GetPostUpdateQueue(CALL_CATEGORY_GUI).Remove(Update);
	}
	
	protected void Init()
	{
		m_CrossHairs.Clear();
				
		Widget child = m_Root.GetChildren();
		while(child)
		{
			m_CrossHairs.Insert(CrossHair(child));
			child = child.GetSibling();
		}
	}
	
	protected void OnWidgetScriptInit(Widget w)
	{
		m_Root = w;
		m_Root.SetHandler(this);
		Init();
		m_Root.Update();
	}
	
	protected void Update()
	{
		//! don't show crosshair if it's disabled in profile or from server
		if(!g_Game.GetProfileOption(EDayZProfilesOptions.CROSSHAIR) || g_Game.GetWorld().IsCrosshairDisabled())
		{
			if(GetCurrentCrossHair())
				ShowCrossHair(null);
			
			return;
		};

		if(m_Player && !m_Player.IsAlive()) // handle respawn
		{
			m_Player = null;
			m_AM = null;
		}
		if(!m_Player) GetPlayer();
		if(!m_AM) GetActionManager();
		
		if(m_Player && m_Player.IsPlayerSelected())
		{
			SelectCrossHair();
		}
	}
	
	// getters
 	protected void GetPlayer()
	{
		Class.CastTo(m_Player, GetGame().GetPlayer());
	}

	protected void GetActionManager()
	{
		if( m_Player && m_Player.IsPlayerSelected() )
		{
			Class.CastTo(m_AM, m_Player.GetActionManager());
		}
		else
			m_AM = null;
	}
	
	protected CrossHair GetCrossHairByName(string widgetName)
	{
		for(int i = 0; i < m_CrossHairs.Count(); i++)
		{
			if(m_CrossHairs.Get(i).GetName() == widgetName)
				return m_CrossHairs.Get(i);
		}

		return null;
	}

	protected CrossHair GetCurrentCrossHair()
	{
		for(int i = 0; i < m_CrossHairs.Count(); i++)
		{
			if(m_CrossHairs.Get(i).IsCurrent())
				return m_CrossHairs.Get(i);
		}

		return null;
	}
	
	protected void SelectCrossHair()
	{
		if(!m_AM) return;

		HumanInputController hic = m_Player.GetInputController();
		ActionBase action = m_AM.GetRunningAction();
		
		bool firearmInHands = m_Player.GetItemInHands() && m_Player.GetItemInHands().IsWeapon();

		//! firearms
		if ( firearmInHands && m_Player.IsRaised() && !m_Player.IsInIronsights() && !m_Player.IsInOptics() && !hic.CameraIsFreeLook() && !m_Player.GetCommand_Melee2() )
		{
			ShowCrossHair(GetCrossHairByName("crossT_128x128"));
		}
		//! On Continuous Actions
		else if (action && action.GetActionCategory() == AC_CONTINUOUS)
		{
			int actionState = m_AM.GetActionState(action);

			if ( actionState != UA_NONE )
				ShowCrossHair(null);
		}
		//! raised hands(bare + non-firearm) + melee cmd
		else if ( m_Player.IsRaised() || m_Player.GetCommand_Melee2() || GetGame().GetUIManager().GetMenu() != null )
		{
			ShowCrossHair(null);
		}
		//! handle unconscious state
		else if ( m_Player.GetCommand_Unconscious() )
		{
			ShowCrossHair(null);
		}
		//! default
		else
		{
			ShowCrossHair(GetCrossHairByName("dot"));
		}
	}
	
	protected void ShowCrossHair(CrossHair crossHair)
	{
		//! no crosshair + clean + hide the previous
		if (!crossHair)
		{
			if (GetCurrentCrossHair())
				GetCurrentCrossHair().Hide();
			
			return;
		}
		else //! hide prev crosshair
		{
			if (GetCurrentCrossHair() && GetCurrentCrossHair() != crossHair)
				GetCurrentCrossHair().Hide();
		}
		
		//! show the new one
		if (!crossHair.IsCurrent() && crossHair.IsShown())
			crossHair.Show();

#ifdef WIP_TRACKIR
		FloatingCrossHair(crossHair.GetWidget());
#endif
	}
	
	//! Highly WIP, do not use
	void FloatingCrossHair(Widget widget)
	{
		HumanInputController hic = m_Player.GetInputController();
		
		//! Only intended to be used with track IR
		if (!hic.CameraIsTracking())
		{
			widget.SetPos(0, 0);
			widget.Update();
			return;
		}
		
		ActionBase action = m_AM.GetRunningAction();
		
		float dt = GetDayZGame().GetDeltaT();
		
		HumanCommandWeapons hcw = m_Player.GetCommandModifier_Weapons();
		
		vector transform[4];
		m_Player.GetTransformWS(transform);
		
		vector aimAngles = Vector(hcw.GetBaseAimingAngleLR(), hcw.GetBaseAimingAngleUD(), 0.0);
		vector plrAngles = Math3D.MatrixToAngles(transform);
		
		aimAngles = Vector(0.0, hcw.GetBaseAimingAngleUD(), 0.0);
		plrAngles = Vector(hic.GetHeadingAngle() * -Math.RAD2DEG, 0, 0);
		
		vector resAngle = aimAngles + plrAngles;
		
		vector start;
		MiscGameplayFunctions.GetHeadBonePos(m_Player, start);
		
		vector direction = resAngle.AnglesToVector();
		
		int layer = ObjIntersectView;
		float range = 1.0;
		
		Weapon_Base weapon;
		if (Class.CastTo(weapon, m_Player.GetItemInHands()) && m_Player.IsRaised())
		{
			layer = ObjIntersectFire;
			range = 10.0;
			
			vector usti_hlavne_position = weapon.GetSelectionPositionMS( "usti hlavne" );
			vector konec_hlavne_position = weapon.GetSelectionPositionMS( "konec hlavne" );
			usti_hlavne_position = m_Player.ModelToWorld(usti_hlavne_position);
			konec_hlavne_position = m_Player.ModelToWorld(konec_hlavne_position);
			
			vector contact_dir;
			int contact_component;
			
			direction = konec_hlavne_position - usti_hlavne_position;
			direction.Normalize();
			
			start = konec_hlavne_position;
			
			m_PreviousDirectionSet = false;
		}
		else
		{
			if (!m_PreviousDirectionSet)
			{
				m_PreviousDirectionSet = true;
				m_PreviousDirection = direction;
			}
			
			float r0[4];
			float r1[4];
			
			vector t[4];
			
			Math3D.DirectionAndUpMatrix(m_PreviousDirection, vector.Up, t);
			Math3D.MatrixToQuat(t, r0);
			
			Math3D.DirectionAndUpMatrix(direction, vector.Up, t);
			Math3D.MatrixToQuat(t, r1);
			
			Math3D.QuatLerp(r0, r0, r1, 0.1);
			
			Math3D.QuatToMatrix(r0, t);
			
			direction = t[2];
		
			m_PreviousDirection = direction;
		}
		
		vector end = start + (direction * range);
		vector position = end;
		
		DayZPhysics.RaycastRV(start, end, position, contact_dir, contact_component, null, m_Player, m_Player, false, false, layer);
		
		/*
		vector lines[2];
		lines[0] = start;
		lines[1] = end;
		
		Shape.CreateSphere(0xFFFFFF00, ShapeFlags.ONCE, usti_hlavne_position, 0.1);
		Shape.CreateSphere(0xFFFFFF00, ShapeFlags.ONCE, konec_hlavne_position, 0.1);
		Shape.CreateLines(0xFF00FF00, ShapeFlags.ONCE, lines, 2);
		*/
		
		vector screenSpace = GetGame().GetScreenPos(position);

		float sx, sy;
		widget.GetScreenSize(sx, sy);
		screenSpace[0] = screenSpace[0] - (sx * 0.5);
		screenSpace[1] = screenSpace[1] - (sy * 0.5);
			
		widget.SetScreenPos(screenSpace[0], screenSpace[1]);
		widget.Update();
	}
}