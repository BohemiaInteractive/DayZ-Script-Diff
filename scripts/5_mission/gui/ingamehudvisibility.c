// Corresponds to a widget within a HUD layout
enum EHudElement
{
	LHUD_PRESENCE,
	LHUD_STANCE,
	LHUD_PLAYER,
	LHUD_VEHICLE,
	RHUD_BADGES,
	RHUD_DIVIDER,
	RHUD_NOTIFIERS,
	QUICKBAR
}

// Flag representing a visibility rule
enum EHudContextFlags
{
	HUD_DISABLE = 1,		// left/right hud disabled in options
	HUD_HIDE = 2,			// left/right hud hidden through hotkey
	VEHICLE_DISABLE = 4,	// vehicle hud disabled in options
	DRIVER = 8,				// is driver -> show veh hud
	VEHICLE = 16, 			// in vehicle -> hide left stats
	MENU_OPEN = 32,			// another menu opened
	NO_BADGE = 64,			// no badges visible
	QUICKBAR_DISABLE = 128,	// quickbar hud disabled in options
	QUICKBAR_HIDE = 256,	// quickbar hud hidden through hotkey
	QUICKBAR_GLOBAL = 512,	// platform based visibility
	INVENTORY_OPEN = 1024,	// inventory is open -> HUD left/right should be always visible
	UNCONSCIOUS = 2048		// player is unconscious
}

// Hud visibility manager for hud elements with more complex/overlapping visibility rules
// This is meant for parents of widgets structures which are afected by multiple visibility rules, not small individual widgets 
class IngameHudVisibility : Managed
{	
	// TODO visibility getters to replace the remaining API in IngameHud
	
	protected EHudContextFlags m_CtxFlags;
	protected ref map<EHudElement, Widget> m_ElementMap;
	protected ref map< EHudContextFlags, ref array<EHudElement> > m_ElementLinkMap;
	
	// !these dont work when const
	EHudContextFlags QUICKBAR_HIDE_FLAGS = (EHudContextFlags.QUICKBAR_HIDE | EHudContextFlags.QUICKBAR_DISABLE | EHudContextFlags.QUICKBAR_GLOBAL);
	static EHudContextFlags HUD_HIDE_FLAGS = (EHudContextFlags.HUD_DISABLE | EHudContextFlags.HUD_HIDE | EHudContextFlags.MENU_OPEN | EHudContextFlags.UNCONSCIOUS);
	
	void Init(Widget panel)
	{
		m_ElementMap = new map<EHudElement, Widget>();
		m_ElementLinkMap = new map< EHudContextFlags, ref array<EHudElement> >();
		
		InitElementMap(panel);
		LinkElementsToFlags();
	}
	
	protected void InitElementMap(Widget panel)
	{
		Widget leftHUDPanel = panel.FindAnyWidget("LeftHUDPanel");		
		m_ElementMap.Insert(EHudElement.LHUD_PRESENCE, leftHUDPanel.FindAnyWidget("PresencePanel"));
		m_ElementMap.Insert(EHudElement.LHUD_STANCE, leftHUDPanel.FindAnyWidget("StancePanel"));
		m_ElementMap.Insert(EHudElement.LHUD_PLAYER, leftHUDPanel.FindAnyWidget("PlayerPanel"));
		m_ElementMap.Insert(EHudElement.LHUD_VEHICLE, leftHUDPanel.FindAnyWidget("VehicleHUDPanels"));
		m_ElementMap.Insert(EHudElement.RHUD_BADGES, panel.FindAnyWidget("BadgesPanel"));
		m_ElementMap.Insert(EHudElement.RHUD_DIVIDER, panel.FindAnyWidget("BadgeNotifierDivider"));
		m_ElementMap.Insert(EHudElement.RHUD_NOTIFIERS, panel.FindAnyWidget("NotifiersPanel"));
		m_ElementMap.Insert(EHudElement.QUICKBAR, panel.FindAnyWidget("QuickbarGrid"));
	}
	
	// Configures which elements are relevant to which flags 
	// When flag value changes, all relevant elements update visibility
	protected void LinkElementsToFlags()
	{
		array <int> lHud = {EHudElement.LHUD_STANCE, EHudElement.LHUD_PLAYER, EHudElement.LHUD_PRESENCE};
		array <int> hud = {EHudElement.LHUD_STANCE, EHudElement.LHUD_PLAYER, EHudElement.LHUD_PRESENCE, EHudElement.RHUD_BADGES, EHudElement.RHUD_DIVIDER, EHudElement.RHUD_NOTIFIERS};
		array <int> hudVeh = {EHudElement.LHUD_STANCE, EHudElement.LHUD_PLAYER, EHudElement.LHUD_PRESENCE, EHudElement.RHUD_BADGES, EHudElement.RHUD_DIVIDER, EHudElement.RHUD_NOTIFIERS, EHudElement.LHUD_VEHICLE};
		
		m_ElementLinkMap.Insert(EHudContextFlags.HUD_DISABLE, hud);
		m_ElementLinkMap.Insert(EHudContextFlags.HUD_HIDE, hud);
		m_ElementLinkMap.Insert(EHudContextFlags.VEHICLE_DISABLE, {EHudElement.LHUD_VEHICLE});
		m_ElementLinkMap.Insert(EHudContextFlags.DRIVER, {EHudElement.LHUD_VEHICLE});
		m_ElementLinkMap.Insert(EHudContextFlags.VEHICLE, lHud);
		m_ElementLinkMap.Insert(EHudContextFlags.MENU_OPEN, hudVeh);
		m_ElementLinkMap.Insert(EHudContextFlags.NO_BADGE, {EHudElement.RHUD_DIVIDER});
		m_ElementLinkMap.Insert(EHudContextFlags.QUICKBAR_DISABLE, {EHudElement.QUICKBAR});
		m_ElementLinkMap.Insert(EHudContextFlags.QUICKBAR_HIDE, {EHudElement.QUICKBAR});
		m_ElementLinkMap.Insert(EHudContextFlags.QUICKBAR_GLOBAL, {EHudElement.QUICKBAR});
		m_ElementLinkMap.Insert(EHudContextFlags.INVENTORY_OPEN, hud);
		m_ElementLinkMap.Insert(EHudContextFlags.UNCONSCIOUS, hud);
	}
		
	// Set flag which changes HUD visibility
	void SetContextFlag(EHudContextFlags flag, bool state)
	{
		if (state)
			m_CtxFlags |= flag;
		else 
			m_CtxFlags &= ~flag;
		
		array<EHudElement> affectedElements = m_ElementLinkMap.Get(flag);
		foreach (EHudElement element : affectedElements)
		{
			UpdateElement(element);
		}
	}
	
	// Check widget visibility
	bool IsElementVisible(EHudElement element)
	{
		Widget widget = m_ElementMap.Get(element);
		if (widget)
			return widget.IsVisible());
		
		return false;
	}
	
	// Check if specific visibility rules are active
	bool IsContextFlagActive(EHudContextFlags flag)
	{
		if (m_CtxFlags & flag)
			return true;	
		
		return false;
	}
	
	protected void UpdateElement(EHudElement element)
	{
		bool visible = true;

		switch (element)
		{
			// LHUD group
			case EHudElement.LHUD_PRESENCE:
			case EHudElement.LHUD_STANCE:
			case EHudElement.LHUD_PLAYER:
				if (m_CtxFlags & (HUD_HIDE_FLAGS | EHudContextFlags.VEHICLE) && (m_CtxFlags & EHudContextFlags.INVENTORY_OPEN) == 0)
					visible = false;
			
				break;
			
			case EHudElement.LHUD_VEHICLE:
				if ((m_CtxFlags & EHudContextFlags.DRIVER) == 0 || m_CtxFlags & (EHudContextFlags.VEHICLE_DISABLE | EHudContextFlags.MENU_OPEN | EHudContextFlags.UNCONSCIOUS))
					visible = false;
			
				break;
			
			// RHUD group
			case EHudElement.RHUD_DIVIDER:
				if (m_CtxFlags & EHudContextFlags.NO_BADGE)
					visible = false;
			
			case EHudElement.RHUD_BADGES:
			case EHudElement.RHUD_NOTIFIERS:
				if (m_CtxFlags & HUD_HIDE_FLAGS && (m_CtxFlags & EHudContextFlags.INVENTORY_OPEN) == 0)
					visible = false;
			
				break;
			
			case EHudElement.QUICKBAR:
				if (m_CtxFlags & QUICKBAR_HIDE_FLAGS)
					visible = false;
			
				break;
		}
		
		
		UpdateVisibility(element, visible);
	}
	
	protected void UpdateVisibility(EHudElement element, bool target)
	{
		Widget widget = m_ElementMap.Get(element);
		if (!widget)
			return;		
		
		if (target && !widget.IsVisible())
		{
			widget.Show(true);
			//Print(typename.EnumToString(EHudElement, element) + " SHOW");
		}
		
		if (!target && widget.IsVisible())
		{
			widget.Show(false);
			//Print(typename.EnumToString(EHudElement, element) + " HIDE");
		}
	}
}