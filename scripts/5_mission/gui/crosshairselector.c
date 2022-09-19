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
}

class CrossHairSelector extends ScriptedWidgetEventHandler
{
	protected PlayerBase 					m_Player;
	protected ActionManagerBase 			m_AM;
	
	protected Widget					m_Root;

	protected ref set<ref CrossHair>	m_CrossHairs;

	void CrossHairSelector()
	{
		m_Player 		= null;
		m_AM 			= null;
		m_CrossHairs 	= new set<ref CrossHair>;

		GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Insert(Update);
	}
		
	void ~CrossHairSelector()
	{
		GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Remove(Update);
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
		if(!crossHair)
		{
			if(GetCurrentCrossHair())
				GetCurrentCrossHair().Hide();
			
			return;
		}
		else //! hide prev crosshair
		{
			if(GetCurrentCrossHair() && GetCurrentCrossHair() != crossHair)
				GetCurrentCrossHair().Hide();
		}
		
		//! show the new one
		if(!crossHair.IsCurrent() && crossHair.IsShown())
			crossHair.Show();
	}
}