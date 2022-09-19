enum ScreenWidthType
{
	NARROW,
	MEDIUM,
	WIDE
}

class InventoryMenu extends UIScriptedMenu
{
	ref Inventory						m_Inventory;
	private ref ContextMenu				m_context_menu;
	protected bool						m_IsOpened;
	protected bool						m_OnlyFirstTime;
	protected int  						m_LastDisplayLanguage;
	
	protected static ScreenWidthType	m_WidthType;
	protected static int				m_Width;
	protected static int				m_Height;
	
	void InventoryMenu()
	{
		CheckWidth();
		m_Inventory = new Inventory(null);
		m_Inventory.Reset();
		m_Inventory.UpdateInterval();
		m_context_menu = new ContextMenu;
		m_LastDisplayLanguage = g_Game.GetCurrentDisplayLanguageIdx();
	}
	
	override Widget Init()
	{
		m_Inventory.Init();
		m_context_menu.Init(layoutRoot);
		layoutRoot = m_Inventory.GetMainWidget();
		return layoutRoot;
	}
	
	void CheckWidth()
	{
		GetScreenSize( m_Width, m_Height );
		
		if( m_Height > 0 )
		{
			float ratio = m_Width / m_Height;
			if( ratio > 1.75 )
				m_WidthType = ScreenWidthType.WIDE;
			else if( ratio > 1.5 )
				m_WidthType = ScreenWidthType.MEDIUM;
			else
				m_WidthType = ScreenWidthType.NARROW;
		}
	}
	
	static ScreenWidthType GetWidthType()
	{
		return m_WidthType;
	}
	
	static int GetWidth()
	{
		return m_Width;
	}
	
	static int GetHeight()
	{
		return m_Height;
	}
	
	static float GetHeightMultiplied( float value )
	{
		float height = m_Height;
		return height / 1080 * value;
	}
	
	void RefreshQuickbar()
	{
		m_Inventory.RefreshQuickbar();
	}
	
	override ContextMenu GetContextMenu()
	{
		return m_context_menu;
	}

	void InitContainers(EntityAI target)
	{
		
	}
	
	override void Update( float timeslice )
	{
		if( m_Inventory )
		{
			m_Inventory.Update(timeslice);
		}
	}
	
	override void Refresh()
	{
		super.Refresh();
		
		m_Inventory.UpdateConsoleToolbar();
	}

	override void OnShow()
	{
		super.OnShow();
		m_IsOpened = true;
		PPERequesterBank.GetRequester(PPERequesterBank.REQ_INVENTORYBLUR).Start();
		VicinityItemManager.GetInstance().RefreshVicinityItems();
		
		if(m_Inventory)
			m_Inventory.OnShow();
		
		SetFocus( layoutRoot );
		
		MissionGameplay mission = MissionGameplay.Cast( GetGame().GetMission() );
		if( mission )
		{
			mission.MoveHudForInventory( true );
		}
		
		ItemManager.GetInstance().SetItemMicromanagmentMode( false );
		ItemManager.GetInstance().SetSelectedItem( null, null, null, null );

		m_Inventory.Refresh();
	}
	
	override bool OnController( Widget w, int control, int value )
	{
		if( m_IsOpened )
			return m_Inventory.Controller( w, control, value );
		return false;
	}
	
	bool IsOpened()
	{
		return m_IsOpened;
	}

	override void OnHide()
	{
		super.OnHide();
		m_context_menu.Hide();
		m_IsOpened = false;
		PPERequesterBank.GetRequester(PPERequesterBank.REQ_INVENTORYBLUR).Stop();
		if(m_Inventory)
			m_Inventory.OnHide();
		MissionGameplay mission = MissionGameplay.Cast( GetGame().GetMission() );
		if( mission )
		{
			mission.MoveHudForInventory( false );
		}
		
		ItemManager.GetInstance().SetItemMicromanagmentMode( false );
		ItemManager.GetInstance().SetSelectedItem( null, null, null, null );
		ItemManager.GetInstance().HideTooltip();
	}
	
	int GetLastDisplayLanguage()
	{
		return m_LastDisplayLanguage;
	}
	
	bool LanguageChanged()
	{
		return g_Game.GetCurrentDisplayLanguageIdx() != m_LastDisplayLanguage;
	}
}
