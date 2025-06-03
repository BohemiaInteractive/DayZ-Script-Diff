#ifdef PLATFORM_CONSOLE
enum ConsoleActionToolbarMask
{
	EMPTY					= 0,
	TO_HANDS_SWAP_VICINITY	= 1,
	TO_HANDS_SWAP_INVENTORY	= 2,
	DROP					= 4,
	EQUIP					= 8,
	SPLIT					= 16,
	TO_INVENTORY			= 32,
	OPEN_CLOSE_CONTAINER	= 64,
	MICROMANAGMENT			= 128,
	QUICKSLOT				= 256,
	COMBINE					= 512,
}
#endif

enum Direction
{
	RIGHT,
	LEFT,
	UP,
	DOWN
}

class Inventory: LayoutHolder
{
	protected ref LeftArea					m_LeftArea;
	protected ref RightArea					m_RightArea;
	protected ref HandsArea					m_HandsArea;
	protected ref PlayerPreview				m_PlayerPreview;
	protected ref InventoryQuickbar			m_Quickbar;
	
	protected Widget						m_QuickbarWidget;
	protected Widget						m_TopConsoleToolbarVicinity;
	protected Widget						m_TopConsoleToolbarHands;
	protected Widget						m_TopConsoleToolbarEquipment;
	protected Widget						m_BottomConsoleToolbar;
	protected RichTextWidget				m_BottomConsoleToolbarRichText;

	protected ref ContextMenu				m_ContextMenu;
	protected static ref map<string, int>	m_PlayerAttachmentsIndexes;
	
	protected bool							m_HadFastTransferred;
	protected bool							m_HadInspected;
	protected bool							m_NeedUpdateConsoleToolbar;
	
	protected static Inventory				m_Instance;
	
	protected int							m_ControllerAngle;
	protected int							m_ControllerTilt;
	protected bool							m_ControllerRightStickTimerEnd = true;
	protected ref Timer						m_ControllerRightStickTimer;
	
	protected bool							m_HoldingQB;
	protected InventoryItem					m_QBHoveredItems;
	
	////////////////////
	const float BT_REPEAT_DELAY = 0.35;		//delay from first press to begin tick repeat state
	const float BT_REPEAT_TIME = 0.09;		//tick repeat frequency time
	const float BT_THRESHOLD_VALUE = 0.8;	//threshold values for analog inputs
	const int INV_MOV_LEFT = 0;
	const int INV_MOV_RIGHT = 1;
	const int INV_MOV_UP = 2;
	const int INV_MOV_DOWN = 3;
	protected ref array<string> m_InvInputNames = {"UAUILeftInventory","UAUIRightInventory","UAUIUpInventory","UAUIDownInventory"};
	protected ref array<UAIDWrapper> m_InvInputWrappers;
	protected ref array<float> m_InvInputTimes;
	protected int m_InvInputWrappersCount;
	protected float m_SensitivityThreshold = 0.0;
	UAInput m_InvUAInput;
	int m_InvInputActive = 0;
	
	void Inventory(LayoutHolder parent)
	{
		m_Instance = this;
		LoadPlayerAttachmentIndexes();
		
		m_ControllerRightStickTimer = new Timer();
		new ItemManager(GetMainWidget());
		new ColorManager();
		
		m_LeftArea			= new LeftArea(this);
		m_RightArea 		= new RightArea(this);
		m_HandsArea 		= new HandsArea(this);
		m_PlayerPreview		= new PlayerPreview(this);

		m_QuickbarWidget	= GetMainWidget().FindAnyWidget("QuickbarGrid");
		m_Quickbar 			= new InventoryQuickbar(m_QuickbarWidget);
		m_Quickbar.UpdateItems(m_QuickbarWidget);
		
		WidgetEventHandler.GetInstance().RegisterOnDropReceived(GetMainWidget().FindAnyWidget("LeftBackground"), this, "OnLeftPanelDropReceived");
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver(GetMainWidget().FindAnyWidget("LeftBackground"), this, "DraggingOverLeftPanel");
		WidgetEventHandler.GetInstance().RegisterOnDropReceived(GetMainWidget().FindAnyWidget("LeftPanel").FindAnyWidget("Scroller"), this, "OnLeftPanelDropReceived");
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver(GetMainWidget().FindAnyWidget("LeftPanel").FindAnyWidget("Scroller"), this, "DraggingOverLeftPanel");
		
		#ifndef PLATFORM_CONSOLE
		WidgetEventHandler.GetInstance().RegisterOnDropReceived(GetMainWidget().FindAnyWidget("LeftPanel").FindAnyWidget("SlotsContent"), this, "OnLeftPanelDropReceived");
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver(GetMainWidget().FindAnyWidget("LeftPanel").FindAnyWidget("SlotsContent"), this, "DraggingOverLeftPanel");
		WidgetEventHandler.GetInstance().RegisterOnDropReceived(GetMainWidget().FindAnyWidget("LeftPanel").FindAnyWidget("SlotsHeader"), this, "OnLeftPanelDropReceived");
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver(GetMainWidget().FindAnyWidget("LeftPanel").FindAnyWidget("SlotsHeader"), this, "DraggingOverLeftPanel");
		#endif
		
		WidgetEventHandler.GetInstance().RegisterOnDropReceived(GetMainWidget().FindAnyWidget("RightBackground"), this, "OnRightPanelDropReceived");
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver(GetMainWidget().FindAnyWidget("RightBackground"), this, "DraggingOverRightPanel");
		WidgetEventHandler.GetInstance().RegisterOnDropReceived(GetMainWidget().FindAnyWidget("RightPanel").FindAnyWidget("Scroller"), this, "OnRightPanelDropReceived");
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver(GetMainWidget().FindAnyWidget("RightPanel").FindAnyWidget("Scroller"), this, "DraggingOverRightPanel");
		
		#ifndef PLATFORM_CONSOLE
		WidgetEventHandler.GetInstance().RegisterOnDropReceived(GetMainWidget().FindAnyWidget("RightPanel").FindAnyWidget("SlotsContent"), this, "OnRightPanelDropReceived");
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver(GetMainWidget().FindAnyWidget("RightPanel").FindAnyWidget("SlotsContent"), this, "DraggingOverRightPanel");
		WidgetEventHandler.GetInstance().RegisterOnDropReceived(GetMainWidget().FindAnyWidget("RightPanel").FindAnyWidget("SlotsHeader"), this, "OnRightPanelDropReceived");
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver(GetMainWidget().FindAnyWidget("RightPanel").FindAnyWidget("SlotsHeader"), this, "DraggingOverRightPanel");
		#endif
		
		WidgetEventHandler.GetInstance().RegisterOnDropReceived(GetMainWidget().FindAnyWidget("CharacterPanel"), this, "OnCenterPanelDropReceived");
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver(GetMainWidget().FindAnyWidget("CharacterPanel"), this, "DraggingOverCenterPanel");
		
		WidgetEventHandler.GetInstance().RegisterOnDropReceived(GetMainWidget().FindAnyWidget("HandsPanel"), this, "OnHandsPanelDropReceived");
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver(GetMainWidget().FindAnyWidget("HandsPanel"), this, "DraggingOverHandsPanel");
		
		#ifdef PLATFORM_CONSOLE
		PluginDiagMenu plugin_diag_menu = PluginDiagMenu.Cast(GetPlugin(PluginDiagMenu));
		GetGame().GetUIManager().ShowUICursor(false);
		ResetFocusedContainers();
		GetMainWidget().FindAnyWidget("CursorCharacter").Show(false);

		//console inventory toolbar
		m_TopConsoleToolbarVicinity		= GetRootWidget().FindAnyWidget("LBRB_Vicinity");
		m_TopConsoleToolbarHands		= GetRootWidget().FindAnyWidget("LBRB_Hands");
		m_TopConsoleToolbarEquipment	= GetRootWidget().FindAnyWidget("LBRB_Equipment");

		RichTextWidget.Cast(m_TopConsoleToolbarVicinity.FindAnyWidget("LBRB_Vicinity_LBIcon")).SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIInventoryTabLeft", "", EUAINPUT_DEVICE_CONTROLLER));
		RichTextWidget.Cast(m_TopConsoleToolbarVicinity.FindAnyWidget("LBRB_Vicinity_RBIcon")).SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIInventoryTabRight", "", EUAINPUT_DEVICE_CONTROLLER));
		RichTextWidget.Cast(m_TopConsoleToolbarHands.FindAnyWidget("LBRB_Hands_LBIcon")).SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIInventoryTabLeft", "", EUAINPUT_DEVICE_CONTROLLER));
		RichTextWidget.Cast(m_TopConsoleToolbarHands.FindAnyWidget("LBRB_Hands_RBIcon")).SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIInventoryTabRight", "", EUAINPUT_DEVICE_CONTROLLER));
		RichTextWidget.Cast(m_TopConsoleToolbarEquipment.FindAnyWidget("LBRB_Equipment_LBIcon")).SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIInventoryTabLeft", "", EUAINPUT_DEVICE_CONTROLLER));
		RichTextWidget.Cast(m_TopConsoleToolbarEquipment.FindAnyWidget("LBRB_Equipment_RBIcon")).SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIInventoryTabRight", "", EUAINPUT_DEVICE_CONTROLLER));
		
		m_BottomConsoleToolbar			= GetRootWidget().FindAnyWidget("BottomConsoleToolbar");
		m_BottomConsoleToolbarRichText	= RichTextWidget.Cast(GetRootWidget().FindAnyWidget("ContextToolbarText"));
		
		m_NeedUpdateConsoleToolbar = false;
		
		GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		
		UpdateConsoleToolbar();
		InitInputWrapperData();
		#endif
	}
	
	void InitInputWrapperData()
	{
		int namesCount = m_InvInputNames.Count();
		m_InvInputWrappers = new array<UAIDWrapper>;
		m_InvInputTimes = new array<float>;
		
		for (int i = 0; i < namesCount; i++)
		{
			m_InvInputWrappers.Insert(GetUApi().GetInputByName(m_InvInputNames[i]).GetPersistentWrapper());
			m_InvInputTimes.Insert(0);
		}
		
		m_InvInputWrappersCount = m_InvInputWrappers.Count();
	}
	
	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		UpdateConsoleToolbar();
		#endif
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		switch (pInputDeviceType)
		{
			case EInputDeviceType.CONTROLLER:
				m_BottomConsoleToolbar.Show(true);
				UpdateConsoleToolbar();
				break;
	
			default:
				if (GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
				{
					m_BottomConsoleToolbar.Show(false);
					m_TopConsoleToolbarVicinity.Show(false);
					m_TopConsoleToolbarHands.Show(false);
					m_TopConsoleToolbarEquipment.Show(false);
				}
				break;
		}
	}
	
	static Inventory GetInstance()
	{
		return m_Instance;
	}
	
	void Serialize()
	{
		ItemManager.GetInstance().SerializeDefaultOpenStates();
		ItemManager.GetInstance().SerializeDefaultHeaderOpenStates();
		GetGame().SaveProfile();
	}

	void Deserialize()
	{
		ItemManager.GetInstance().DeserializeDefaultOpenStates();
		ItemManager.GetInstance().DeserializeDefaultHeaderOpenStates();
	}

	static int GetPlayerAttachmentIndex(string slot_name)
	{
		return m_PlayerAttachmentsIndexes[slot_name];
	}
	
	static int GetPlayerAttachmentIndex(int slot_id)
	{
		return GetPlayerAttachmentIndex(InventorySlots.GetSlotName(slot_id));
	}
	
	protected void LoadPlayerAttachmentIndexes()
	{
		int i;		
		string data, errorMessage;
		
		m_PlayerAttachmentsIndexes = new map<string, int>();

		if (GetGame().GetProfileString("INV_AttIndexes", data))
		{
			if (!JsonFileLoader<map<string, int>>.LoadData(data, m_PlayerAttachmentsIndexes, errorMessage))
				ErrorEx(errorMessage);
		}
		
		string configPathGhostSlots = "CfgVehicles SurvivorBase InventoryEquipment playerSlots";
		array<string> playerGhostSlots = new array<string>();
		GetGame().ConfigGetTextArray(configPathGhostSlots, playerGhostSlots);
		
		foreach (string slotName : playerGhostSlots)
		{
			slotName.Replace("Slot_", "");
			if (!m_PlayerAttachmentsIndexes.Contains(slotName) && InventorySlots.GetSlotIdFromString(slotName) != -1)
			{
				m_PlayerAttachmentsIndexes.Insert(slotName, m_PlayerAttachmentsIndexes.Count());
			}
		}
		
		if (!JsonFileLoader<map<string, int>>.MakeData(m_PlayerAttachmentsIndexes, data, errorMessage))
			ErrorEx(errorMessage);

		GetGame().SetProfileString("INV_AttIndexes", data);
	}
	
	static void MoveAttachmentUp(int slot_id)
	{
		int curr			= GetPlayerAttachmentIndex(slot_id);
		
		int next_offset		= 0;
		string next_item	= "init";
		int next_id;
		EntityAI next_ent;
		
		while(!next_ent && next_item != "")
		{
			next_item		= m_PlayerAttachmentsIndexes.GetKeyByValue(curr + --next_offset);
		 	next_id			= InventorySlots.GetSlotIdFromString(next_item);
			next_ent		= GetGame().GetPlayer().GetInventory().FindAttachment(next_id);
			if (next_ent && !m_Instance.m_RightArea.HasEntityContainerVisible(next_ent))
				next_ent	= null;
		}
		
		if (next_item != "" && next_ent)
		{
			int next = GetPlayerAttachmentIndex(next_item);
			m_PlayerAttachmentsIndexes.Set(InventorySlots.GetSlotName(slot_id), next);
			m_PlayerAttachmentsIndexes.Set(next_item, curr);
			if (m_Instance)
				m_Instance.m_RightArea.SwapItemsInOrder(next_id, slot_id);
		}
	}
	
	static void MoveAttachmentDown(int slot_id)
	{
		int curr			= GetPlayerAttachmentIndex(slot_id);
		
		int next_offset		= 0;
		string next_item	= "init";
		int next_id;
		EntityAI next_ent;
		
		while(!next_ent && next_item != "")
		{
			next_item		= m_PlayerAttachmentsIndexes.GetKeyByValue(curr + ++next_offset);
		 	next_id			= InventorySlots.GetSlotIdFromString(next_item);
			next_ent		= GetGame().GetPlayer().GetInventory().FindAttachment(next_id);
			if (next_ent && !m_Instance.m_RightArea.HasEntityContainerVisible(next_ent))
				next_ent	= null;
		}
		
		if (next_item != "" && next_ent)
		{
			int next = GetPlayerAttachmentIndex(next_item);
			m_PlayerAttachmentsIndexes.Set(InventorySlots.GetSlotName(slot_id), next);
			m_PlayerAttachmentsIndexes.Set(next_item, curr);
			if (m_Instance)
				m_Instance.m_RightArea.SwapItemsInOrder(next_id, slot_id);
		}
	}

	protected int GetProperControllerStickAngle(int angle)
	{
		int proper_angle = (360 - angle) % 360;
		return proper_angle;
	}

	protected int AngleToDirection(int angle)
	{
		if (angle < 45 || angle > 315)
		{
			return Direction.RIGHT;
		}
		else if (angle < 135 && angle > 45)
		{
			return Direction.DOWN;
		}
		else if (angle < 225 && angle > 135)
		{
			return Direction.LEFT;
		}
		else if (angle < 315 && angle > 225)
		{
			return Direction.UP;
		}
		return -1;
	}

	void TimerEnd()
	{
		m_ControllerRightStickTimerEnd = true;
		m_ControllerRightStickTimer.Stop();
	}

	bool Controller(Widget w, int control, int value)
	{
		#ifdef PLATFORM_CONSOLE
		
		//Right stick
		if (control == ControlID.CID_RADIALMENU)
		{
			m_ControllerAngle 	= value >> 4; 	// <0,360>
			m_ControllerTilt	= value & 0xF; 	// <0,10>

			m_ControllerAngle = GetProperControllerStickAngle(m_ControllerAngle);

			m_ControllerAngle = AngleToDirection(m_ControllerAngle);

			if (m_ControllerTilt>5)
			{
				if (m_ControllerRightStickTimerEnd)
				{
					m_ControllerRightStickTimerEnd = false;
					m_ControllerRightStickTimer.Run(0.1, this, "TimerEnd");
				}
			}
			return true;
		}
		
		UpdateConsoleToolbar();
		#endif
		return false;
	}

	void DraggingOverHandsPanel(Widget w, int x, int y, Widget receiver)
	{
		ItemManager.GetInstance().HideDropzones();
		m_HandsArea.DraggingOverHandsPanel(w, x, y, receiver);
	}
	
	void OnHandsPanelDropReceived(Widget w, int x, int y, Widget receiver)
	{
		m_HandsArea.OnHandsPanelDropReceived(w, x, y, receiver);
	}

	void OnLeftPanelDropReceived(Widget w, int x, int y, Widget receiver)
	{
		m_LeftArea.OnLeftPanelDropReceived(w, x, y, receiver);
	}

	void OnRightPanelDropReceived(Widget w, int x, int y, Widget receiver)
	{
		if (w)
		{
			ItemPreviewWidget ipw = ItemPreviewWidget.Cast(w.FindAnyWidget("Render"));
			if (!ipw)
			{
				string name = w.GetName();
				name.Replace("PanelWidget", "Render");
				ipw = ItemPreviewWidget.Cast(w.FindAnyWidget(name));
				if (!ipw)
				{
					ipw = ItemPreviewWidget.Cast(w);
					if (!ipw)
					{
						return;
					}
				}
			}
	
			EntityAI item = ipw.GetItem();
			if (item)
			{
				PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
				InventoryLocation il = new InventoryLocation;
				if (player && player.GetInventory().FindFreeLocationFor(item, FindInventoryLocationType.CARGO | FindInventoryLocationType.ATTACHMENT, il))
				{
					SplitItemUtils.TakeOrSplitToInventory(player, player, item);
				}
			}
		}
	}
	
	void OnCenterPanelDropReceived(Widget w, int x, int y, Widget receiver)
	{
		if (w)
		{
			ItemPreviewWidget ipw = ItemPreviewWidget.Cast(w.FindAnyWidget("Render"));
			if (!ipw)
			{
				string name = w.GetName();
				name.Replace("PanelWidget", "Render");
				ipw = ItemPreviewWidget.Cast(w.FindAnyWidget(name));
				if (!ipw)
				{
					ipw = ItemPreviewWidget.Cast(w);
					if (!ipw)
					{
						return;
					}
				}
			}
	
			EntityAI item = ipw.GetItem();
			if (item)
			{
				if (!item.GetInventory().CanRemoveEntity())
					return;
				PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
				
				bool found = false;
				
				InventoryLocation inv_loc = new InventoryLocation;
				
				if (player)
				{
					int index = player.GetHumanInventory().FindUserReservedLocationIndex(item);
					if (index>=0)
					{
						player.GetHumanInventory().GetUserReservedLocation(index, inv_loc);
						if (player.GetInventory().LocationCanAddEntity(inv_loc))
						{
							SplitItemUtils.TakeOrSplitToInventoryLocation(player, inv_loc);
						}
						return;
					}
					
				}	
						
				if (player && (player.GetInventory().CanAddAttachment(item)))
				{
					float stackable = item.GetTargetQuantityMax(-1);
		
					if (stackable == 0 || stackable >= item.GetQuantity())
					{
						player.PredictiveTakeEntityAsAttachment(item);
					}
					else
					{
						InventoryLocation il = new InventoryLocation;
						player.GetInventory().FindFreeLocationFor(item, FindInventoryLocationType.ATTACHMENT, il);
						ItemBase.Cast(item).SplitIntoStackMaxToInventoryLocationClient(il);
					}
				}
				else
				{
					int slot_id = item.GetInventory().GetSlotId(0);
					EntityAI slot_item = player.GetInventory().FindAttachment(slot_id);
					if (slot_item && player.GetInventory().CanSwapEntitiesEx(item, slot_item))
					{
						player.PredictiveSwapEntities(item, slot_item);
					}
					else if (player.CanReceiveItemIntoCargo(item))
					{
						InventoryLocation dst = new InventoryLocation;
						player.GetInventory().FindFreeLocationFor(item, FindInventoryLocationType.ANY, dst);
						if (dst.IsValid())
						{						
							if (dst.GetType() == InventoryLocationType.HANDS && item.IsHeavyBehaviour())
							{											
								ActionManagerClient mngr_client;
								CastTo(mngr_client, player.GetActionManager());
			
								ActionTarget atrg = new ActionTarget(item, null, -1, vector.Zero, -1.0);
								if (mngr_client.GetAction(ActionTakeItemToHands).Can(player, atrg, null))
									mngr_client.PerformActionStart(mngr_client.GetAction(ActionTakeItemToHands), atrg, null);
								
								return;
							}
							
							SplitItemUtils.TakeOrSplitToInventoryLocation(player, dst);
						}
					}
				}
			}
		}
	}

	void DraggingOverLeftPanel(Widget w, int x, int y, Widget receiver)
	{
		m_LeftArea.DraggingOverHeader(w, x, y, receiver);
	}

	void DraggingOverRightPanel(Widget w, int x, int y, Widget receiver)
	{
		m_RightArea.DraggingOverHeader(w, x, y, receiver);
	}
	
	void DraggingOverCenterPanel(Widget w, int x, int y, Widget receiver)
	{
		if (w)
		{
			ItemPreviewWidget ipw = ItemPreviewWidget.Cast(w.FindAnyWidget("Render"));
			if (!ipw)
			{
				string name = w.GetName();
				name.Replace("PanelWidget", "Render");
				ipw = ItemPreviewWidget.Cast(w.FindAnyWidget(name));
				if (!ipw)
				{
					ipw = ItemPreviewWidget.Cast(w);
					if (!ipw)
					{
						return;
					}
				}
			}
			
			EntityAI item = ipw.GetItem();
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			
			if (player && item)
			{
				int slot_id = item.GetInventory().GetSlotId(0);
				EntityAI slot_item = player.GetInventory().FindAttachment(slot_id);
				bool found = false;
				
				InventoryLocation inv_loc = new InventoryLocation;
				
				int index = player.GetHumanInventory().FindUserReservedLocationIndex(item);
				if (index>=0)
				{
					player.GetHumanInventory().GetUserReservedLocation(index, inv_loc);
					if (player.GetInventory().LocationCanAddEntity(inv_loc))
					found = true;
				}
				
				if (!found)
				{
					player.GetInventory().FindFreeLocationFor(item, FindInventoryLocationType.ANY, inv_loc);
				}
				
				
				if (inv_loc.IsValid())
				{
					ItemManager.GetInstance().HideDropzones();
					GetMainWidget().FindAnyWidget("RightPanel").FindAnyWidget("DropzoneX").SetAlpha(1);
					if (inv_loc.GetType() == 4)
					{
						ItemManager.GetInstance().HideDropzones();
						GetMainWidget().FindAnyWidget("HandsPanel").FindAnyWidget("DropzoneX").SetAlpha(1);
					}
		
					ColorManager.GetInstance().SetColor(w, ColorManager.GREEN_COLOR);
				}
				else
				{
					ItemManager.GetInstance().ShowSourceDropzone(item);
					ColorManager.GetInstance().SetColor(w, ColorManager.RED_COLOR);
				}
			}
			else
			{
				ColorManager.GetInstance().SetColor(w, ColorManager.RED_COLOR);
			}
		}
	}

	void Update(float timeslice)
	{
		#ifdef PLATFORM_CONSOLE
		//inventory grid movement
		InventoryMovementButtonTickHandler(timeslice);
		
		if (m_InvInputActive & (1 << INV_MOV_RIGHT))
		{
			if (GetUApi().GetInputByID(UAUIDragNDrop).LocalValue())
			{
				EnableMicromanagement();
			}
			
			if (!GetUApi().GetInputByID(UAUIInventoryTabRight).LocalValue())
			{
				if (m_RightArea.IsActive())
					m_RightArea.MoveGridCursor(Direction.RIGHT);
				if (m_LeftArea.IsActive())
					m_LeftArea.MoveGridCursor(Direction.RIGHT);
				if (m_HandsArea.IsActive())
					m_HandsArea.MoveGridCursor(Direction.RIGHT);
			}
		}
		else if (m_InvInputActive & (1 << INV_MOV_LEFT))
		{
			if (GetUApi().GetInputByID(UAUIDragNDrop).LocalValue())
			{
				EnableMicromanagement();
			}
			
			if (!GetUApi().GetInputByID(UAUIInventoryTabLeft).LocalValue())
			{
				if (m_RightArea.IsActive())
					m_RightArea.MoveGridCursor(Direction.LEFT);
				if (m_LeftArea.IsActive())
					m_LeftArea.MoveGridCursor(Direction.LEFT);
				if (m_HandsArea.IsActive())
					m_HandsArea.MoveGridCursor(Direction.LEFT);
			}
		}
		else if (m_InvInputActive & (1 << INV_MOV_UP))
		{
			if (GetUApi().GetInputByID(UAUIDragNDrop).LocalValue())
			{
				EnableMicromanagement();
			}
			
			if (!GetUApi().GetInputByID(UAUIInventoryContainerUp).LocalValue())
			{
				if (m_RightArea.IsActive())
					m_RightArea.MoveGridCursor(Direction.UP);
				if (m_LeftArea.IsActive())
					m_LeftArea.MoveGridCursor(Direction.UP);
				if (m_HandsArea.IsActive())
					m_HandsArea.MoveGridCursor(Direction.UP);
			}
		}
		else if (m_InvInputActive & (1 << INV_MOV_DOWN))
		{
			if (GetUApi().GetInputByID(UAUIDragNDrop).LocalValue())
			{
				EnableMicromanagement();
			}
			
			if (!GetUApi().GetInputByID(UAUIInventoryContainerDown).LocalValue())
			{
				if (m_RightArea.IsActive())
					m_RightArea.MoveGridCursor(Direction.DOWN);
				if (m_LeftArea.IsActive())
					m_LeftArea.MoveGridCursor(Direction.DOWN);
				if (m_HandsArea.IsActive())
					m_HandsArea.MoveGridCursor(Direction.DOWN);
			}
		}
		
		m_InvInputActive = 0;
		#endif
		
		UpdateInterval();
	}
	
	UAInput m_InpInp = null;
	override void UpdateInterval()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		InventoryItem item;
		InventoryItem handsItem
		if (GetUApi().GetInputByID(UAUIRotateInventory).LocalPress())
		{
			item = InventoryItem.Cast(ItemManager.GetInstance().GetDraggedItem());
			if (item)
			{
				int size_x, size_y;
				GetGame().GetInventoryItemSize(item, size_x, size_y);
				if (size_x != size_y)
				{
					item.GetInventory().FlipCargo();
					item.GetOnItemFlipped().Invoke(item.GetInventory().GetFlipCargo());
				}
			}
		}
		
		#ifdef PLATFORM_CONSOLE
		DayZPlayerInventory dpi;
		if (player)
			dpi = player.GetDayZPlayerInventory();
		
		// There's a second one a bit below
		if (dpi && !dpi.IsProcessing())
		{
			if (GetUApi().GetInputByID(UAUIExpandCollapseContainer).LocalPress())
			{
				if (m_RightArea.IsActive())
				{
					m_RightArea.ExpandCollapseContainer();
				}
				else if (m_LeftArea.IsActive())
				{
					m_LeftArea.ExpandCollapseContainer();
				}
			}
			
			if (GetUApi().GetInputByID(UAUIDragNDrop).LocalHoldBegin())
			{
				EnableMicromanagement();
			}
			
			if (GetUApi().GetInputByID(UAUISplit).LocalPress())
			{
				if (m_HandsArea.IsActive())
				{
					if (m_HandsArea.SplitItem())
					{
						m_HandsArea.SetActive(false);
						m_HandsArea.UnfocusGrid();
						m_RightArea.SetActive(true);
					}
				}
				else if (m_RightArea.IsActive())
				{
					m_RightArea.SplitItem();
				}
				else if (m_LeftArea.IsActive())
				{
					m_LeftArea.SplitItem();
				}
				
				HideOwnedTooltip();
			}
			
			if (GetUApi().GetInputByID(UAUIFastEquip).LocalPress())
			{
				if (m_HandsArea.IsActive())
				{
					if (m_HandsArea.EquipItem())
					{
						m_HandsArea.SetActive(false);
						m_HandsArea.UnfocusGrid();
						m_RightArea.SetActive(true);
					}
				}
				else if (m_RightArea.IsActive())
				{
					m_RightArea.EquipItem();
				}
				else if (m_LeftArea.IsActive())
				{
					m_LeftArea.EquipItem();
				}
				
				UpdateConsoleToolbar();
				HideOwnedTooltip();
			}
			
			if (ItemManager.GetInstance().IsMicromanagmentMode() && GetUApi().GetInputByID(UAUIDragNDrop).LocalRelease())
			{
				if (m_RightArea.IsActive())
				{
					m_RightArea.Select();
				}
				else if (m_LeftArea.IsActive())
				{
					m_LeftArea.Select();
				}
				else if (m_HandsArea.IsActive())
				{
					m_HandsArea.Select();
				}
				
				DisableMicromanagement();
				UpdateConsoleToolbar();
			}
			if (GetUApi().GetInputByID(UAUIPutInHandsFromVicinity).LocalPress())
			{
				if (m_LeftArea.IsActive())
				{
					if (m_LeftArea.Select())
					{
						m_LeftArea.SetActive(false);
						m_LeftArea.UnfocusGrid();
						m_HandsArea.SetActive(true);
						
						UpdateConsoleToolbar();
					}
				}
			}
			if (GetUApi().GetInputByID(UAUIPutInHandsFromInventory).LocalPress())
			{
				if (m_RightArea.IsActive())
				{
					if (m_RightArea.Select())
					{
						m_RightArea.SetActive(false);
						m_RightArea.UnfocusGrid();
						m_HandsArea.SetActive(true);
						
						UpdateConsoleToolbar();
					}
				}
			}
			
			if (GetUApi().GetInputByID(UAUIFastTransferToVicinity).LocalPress()) //item drop
			{
				if (m_HandsArea.IsActive())
				{
					item = InventoryItem.Cast(m_HandsArea.GetFocusedItem());
					if (item && item.GetInventory().CanRemoveEntity())
					{
						if (m_HandsArea.TransferItemToVicinity())
						{
							handsItem = InventoryItem.Cast(player.GetHumanInventory().GetEntityInHands());
							if (handsItem && handsItem == item)
							{
								m_HandsArea.SetActive(false);
								m_HandsArea.UnfocusGrid();
								m_LeftArea.SetActive(true);
							}
							m_HadFastTransferred = true;
						}
					}
				}
				else if (m_RightArea.IsActive())
				{
					item = InventoryItem.Cast(m_RightArea.GetFocusedItem());
					if (item && item.GetInventory().CanRemoveEntity())
					{
						if (m_RightArea.TransferItemToVicinity())
							m_HadFastTransferred = true;
					}
				}
				
				UpdateConsoleToolbar();
				HideOwnedTooltip();
			}
		}
		
		if (GetUApi().GetInputByID(UAUIInspectItem).LocalPress())
		{
			if (m_HandsArea.IsActive())
			{
				if (m_HandsArea.InspectItem())
				{
					m_HadInspected = true;
				}
			}
			else if (m_RightArea.IsActive())
			{
				if (m_RightArea.InspectItem())
				{
					m_HadInspected = true;
				}
			}
			else if (m_LeftArea.IsActive())
			{
				if (m_LeftArea.InspectItem())
				{
					m_HadInspected = true;
				}
			}
			
			UpdateConsoleToolbar();
		}
		
		if (GetUApi().GetInputByID(UAUIFastTransferItem).LocalPress())
			m_HadFastTransferred = false;
		
		if (GetUApi().GetInputByID(UAUIInspectItem).LocalPress())
			m_HadInspected = false;
		
		if (dpi && !dpi.IsProcessing())
		{
			if (!m_HadFastTransferred && GetUApi().GetInputByID(UAUIFastTransferItem).LocalPress()) //transfers item to inventory (not hands, or hands last?)
			{
				if (m_LeftArea.IsActive())
				{
					item = InventoryItem.Cast(m_LeftArea.GetFocusedItem());
					if (item && item.GetInventory().CanRemoveEntity())
					{
						m_LeftArea.TransferItem();
					}
				}
				else if (m_HandsArea.IsActive())
				{
					item = InventoryItem.Cast(m_HandsArea.GetFocusedItem());
					if (item && item.GetInventory().CanRemoveEntity())
					{
						handsItem = InventoryItem.Cast(player.GetHumanInventory().GetEntityInHands());
						if (m_HandsArea.TransferItem() && handsItem && handsItem == item)
						{
							m_HandsArea.SetActive(false);
							m_HandsArea.UnfocusGrid();
							m_RightArea.SetActive(true);
						}
						
						HideOwnedTooltip();
					}
				}
				
				UpdateConsoleToolbar();
			}
		}
		
		if (GetUApi().GetInputByID(UAUIInventoryContainerUp).LocalPress())
		{
			if (GetUApi().GetInputByID(UAUIDragNDrop).LocalValue())
			{
				EnableMicromanagement();
			}
			
			MoveFocusByContainer(Direction.UP);
		}

		if (GetUApi().GetInputByID(UAUIInventoryContainerDown).LocalPress())
		{
			if (GetUApi().GetInputByID(UAUIDragNDrop).LocalValue())
			{
				EnableMicromanagement();
			}
			
			MoveFocusByContainer(Direction.DOWN);
		}

		if (GetUApi().GetInputByID(UAUIInventoryTabLeft).LocalPress())
		{
			if (GetUApi().GetInputByID(UAUIDragNDrop).LocalValue())
			{
				EnableMicromanagement();
			}
			
			MoveFocusByArea(Direction.LEFT);
		}

		if (GetUApi().GetInputByID(UAUIInventoryTabRight).LocalPress())
		{
			if (GetUApi().GetInputByID(UAUIDragNDrop).LocalValue())
			{
				EnableMicromanagement();
			}
			
			MoveFocusByArea(Direction.RIGHT);
		}
		
		//Open Quickbar radial menu
		if (GetUApi().GetInputByID(UAUIQuickbarRadialInventoryOpen).LocalPress())
		{
			//assign item
			EntityAI item_to_assign;
						
			if (m_HandsArea.IsActive())
			{
				player = PlayerBase.Cast(GetGame().GetPlayer());
				item_to_assign = m_HandsArea.GetFocusedItem();
				m_HandsArea.AddItemToQuickbarRadial(item_to_assign);
			}
			else if (m_RightArea.IsActive())
			{
				item_to_assign = m_RightArea.GetFocusedItem();
				m_RightArea.AddItemToQuickbarRadial(item_to_assign);
			}
		}
		#endif
		
		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (!m_HadInspected && GetUApi().GetInputByID(UAUICombine).LocalPress())
		{
			if (GetMainWidget().IsVisible())
			{
			#ifdef PLATFORM_CONSOLE
				//DisableMicromanagement();
				if (m_RightArea.IsActive())
				{
					if (m_RightArea.CanCombine())
					{
						if (m_RightArea.Combine())
						{
							mission.HideInventory();
						}
					}
				}
				else if (m_LeftArea.IsActive())
				{
					if (m_LeftArea.CanCombine())
					{
						if (m_LeftArea.Combine())
						{
							mission.HideInventory();
						}
					}
				}
			#endif
			}
		}
		
		// controller close inventory using back action
		if (!m_HadInspected && GetUApi().GetInputByID(UAUIBack).LocalPress() && GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER)
		{
			mission.HideInventory();
		}
		
		for (int i = 0; i < 10; i++)
		{
			if (!m_HoldingQB && GetUApi().GetInputByName("UAItem" + i).LocalPress())
			{
				m_QBHoveredItems = InventoryItem.Cast(ItemManager.GetInstance().GetHoveredItem());
				m_HoldingQB = true;
			}
			
			if (m_HoldingQB && GetUApi().GetInputByName("UAItem" + i).LocalHold())
			{
				
				AddQuickbarItem(m_QBHoveredItems, i);
				m_QBHoveredItems = null;
				m_HoldingQB = false;
			}
		}

		m_LeftArea.UpdateInterval();
		m_RightArea.UpdateInterval();
		m_HandsArea.UpdateInterval();
		m_PlayerPreview.UpdateInterval();
		
		#ifdef PLATFORM_CONSOLE
			UpdateConsoleToolbarCheck();
		#endif
	}
	
	void AddQuickbarItem(InventoryItem item, int index)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			
		if (item && item.GetInventory().CanRemoveEntity())
		{
			player.SetQuickBarEntityShortcut(item, index) ;
		}
		
		InventoryMenu menu = InventoryMenu.Cast(GetGame().GetUIManager().FindMenu(MENU_INVENTORY));
		if (menu)
		{
			menu.RefreshQuickbar();
		}
	}
	
	void EnableMicromanagement()
	{
		if (!ItemManager.GetInstance().IsMicromanagmentMode())
		{
			ItemManager.GetInstance().SetItemMicromanagmentMode(true);
			if (m_RightArea.IsActive())
			{
				m_RightArea.SelectItem();
			}
			else if (m_LeftArea.IsActive())
			{
				m_LeftArea.SelectItem();
			}
			else if (m_HandsArea.IsActive())
			{
				m_HandsArea.SelectItem();
			}
			
			UpdateConsoleToolbar();
			HideOwnedTooltip();
		}
	}
	
	void DisableMicromanagement()
	{
		if (ItemManager.GetInstance().IsMicromanagmentMode())
		{
			ItemManager.GetInstance().SetItemMicromanagmentMode(false);
			ItemManager.GetInstance().SetSelectedItemEx(null, null, null);
			UpdateConsoleToolbar();
			HideOwnedTooltip();
		}
	}

	override void SetLayoutName()
	{
		#ifdef PLATFORM_CONSOLE
			m_LayoutName = WidgetLayoutName.InventoryXbox;
		#else
			switch (InventoryMenu.GetWidthType())
			{
				case ScreenWidthType.NARROW:
				{
					m_LayoutName = WidgetLayoutName.InventoryNarrow;
					break;
				}
				case ScreenWidthType.MEDIUM:
				{
					m_LayoutName = WidgetLayoutName.InventoryMedium;
					break;
				}
				case ScreenWidthType.WIDE:
				{
					m_LayoutName = WidgetLayoutName.InventoryWide;
					break;
				}
			}
		#endif
	}

	void Init() {}

	void Reset()
	{
		m_LeftArea.Refresh();
		m_RightArea.Refresh();
		m_HandsArea.Refresh();
	}
	
	void ResetFocusedContainers()
	{
		#ifdef PLATFORM_CONSOLE
		m_RightArea.UnfocusGrid();
		m_LeftArea.UnfocusGrid();
		m_HandsArea.UnfocusGrid();
		
		m_LeftArea.SetActive(false);
		m_HandsArea.SetActive(false);
		m_RightArea.SetActive(false);
		
		m_RightArea.ResetFocusedContainer();
		m_LeftArea.ResetFocusedContainer();
		
		m_RightArea.SetActive(true);
		#endif
	}

	override void OnShow()
	{
		SetFocus(GetMainWidget());
		Deserialize();
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.IsPlacingLocal())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(player.TogglePlacingLocal);
		}

		Mission mission = GetGame().GetMission();
		if (mission)
		{
			IngameHud hud = IngameHud.Cast(mission.GetHud());
			if (hud)
			{
				hud.ShowQuickbarUI(false);
				hud.ShowHudInventory(true);
			}
		}
		
		RefreshQuickbar();
		UpdateInterval();
		UpdateConsoleToolbar();
		
		m_HadFastTransferred = false;
		m_HadInspected = false;
		
		m_PlayerPreview.RefreshPlayerPreview();
	}

	override void OnHide()
	{
		Serialize();
		HideOwnedTooltip();
		Mission mission = GetGame().GetMission();
		if (mission)
		{
			IngameHud hud = IngameHud.Cast(mission.GetHud());
			if (hud)
			{
				hud.ShowQuickbarUI(true);
				hud.ShowHudInventory(false);
			}
		}
		ItemManager.GetInstance().SetSelectedItemEx(null, null, null);
		ResetFocusedContainers();
	}
	
	override void Refresh()
	{
		m_LeftArea.Refresh();
		m_HandsArea.Refresh();
		m_RightArea.Refresh();
		
		RefreshQuickbar();
		UpdateConsoleToolbar();
	}
	
	void RefreshQuickbar()
	{
		#ifndef PLATFORM_CONSOLE
		m_QuickbarWidget.Show(true);
		#else
		m_QuickbarWidget.Show(GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer());
		#endif
		
		#ifndef PLATFORM_CONSOLE
		if (m_Quickbar)
		#else
		if (m_Quickbar && GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
		#endif
		{
			m_Quickbar.UpdateItems(m_QuickbarWidget);
		}
	}
	

	#ifdef PLATFORM_CONSOLE
	protected string GetConsoleToolbarText(int mask)
	{
		string result = "";
		
		string toHandsSwapVicinity	= string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUIPutInHandsFromVicinity", GetStringVariant("UAUIPutInHandsFromVicinity",{"#STR_Controls_TakeInHandsSwap","#STR_USRACT_HoldToHandSwap",""}), EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string toHandsSwapInv		= string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUIPutInHandsFromInventory", GetStringVariant("UAUIPutInHandsFromInventory",{"#STR_Controls_TakeInHandsSwap","#STR_USRACT_HoldToHandSwap",""}), EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string drop					= string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUIFastTransferToVicinity", "#dayz_context_menu_drop", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string equip 				= string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUIFastEquip", "#dayz_context_menu_equip", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string split 				= string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUISplit", GetStringVariant("UAUISplit",{"#dayz_context_menu_split","#STR_Controls_HoldSplit",""}), EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string toInventory 			= string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUIFastTransferItem", "#dayz_context_menu_to_inventory", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string openCloseContainer 	= string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUIExpandCollapseContainer", "#dayz_context_menu_open_close", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string micromanagment 		= string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUIDragNDrop", "#dayz_context_menu_micro", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string quickslot 			= string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUIQuickbarRadialInventoryOpen", "#ps4_dayz_context_menu_quickslot", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string combine 				= string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUICombine", GetStringVariant("UAUICombine",{"#dayz_context_menu_combine","#dayz_context_menu_combine_hold",""}), EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		
		if (mask & ConsoleActionToolbarMask.OPEN_CLOSE_CONTAINER)
		{
			result += openCloseContainer;
		}		
		if (mask & ConsoleActionToolbarMask.TO_HANDS_SWAP_VICINITY)
		{
			result += toHandsSwapVicinity;
		}
		if (mask & ConsoleActionToolbarMask.TO_HANDS_SWAP_INVENTORY)
		{
			result += toHandsSwapInv;
		}
		if (mask & ConsoleActionToolbarMask.TO_INVENTORY)
		{
			result += toInventory;
		}
		if (mask & ConsoleActionToolbarMask.DROP)
		{
			result += drop;
		}
		if (mask & ConsoleActionToolbarMask.SPLIT)
		{
			result += split;
		}
		if (mask & ConsoleActionToolbarMask.EQUIP)
		{
			result += equip;
		}
		if (mask & ConsoleActionToolbarMask.COMBINE)
		{
			result += combine;
		}
		if (mask & ConsoleActionToolbarMask.MICROMANAGMENT)
		{
			result += micromanagment;
		}
		if (mask & ConsoleActionToolbarMask.QUICKSLOT)
		{
			result += quickslot;
		}

		return result;
	}
	#endif

	Container GetFocusedArea()
	{
		if (m_LeftArea && m_LeftArea.IsActive())
		{
			return m_LeftArea;
		}
		else if (m_RightArea && m_RightArea.IsActive())
		{
			return m_RightArea;
		}
		else if (m_HandsArea && m_HandsArea.IsActive())
		{
			return m_HandsArea;
		}
		return null;
	}
	
	#ifdef PLATFORM_CONSOLE
	void UpdateConsoleToolbarCheck()
	{
		if ( m_NeedUpdateConsoleToolbar )
		{
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if (player.GetInventory().GetAnyInventoryReservationCount() == 0)
			{
				m_NeedUpdateConsoleToolbar = false;
				UpdateConsoleToolbar();
			}
		}
	}
	#endif
	
	//Console toolbar
	void UpdateConsoleToolbar()
	{
		#ifdef PLATFORM_CONSOLE
		int combinationFlag = 0;
		string contextualText;
		InventoryLocation il;
		
		Container focusedArea = GetFocusedArea();
		if (focusedArea)
		{
			Container focusedContainer = GetFocusedArea().GetFocusedContainer();
			EntityAI focusedItem = GetFocusedArea().GetFocusedItem();
			
			if (!(focusedItem && focusedItem.IsSetForDeletion()))
			{
				if (focusedContainer)
				{
					if (focusedItem)
					{
						il = new InventoryLocation;
						focusedItem.GetInventory().GetCurrentInventoryLocation( il );
					}
	
					bool canBeManipulated = false;
					PlayerBase player;
					PlayerBase itemPlayerOwner;
					
					player = PlayerBase.Cast(GetGame().GetPlayer());
					
					m_NeedUpdateConsoleToolbar = player.GetInventory().GetAnyInventoryReservationCount() > 0;
					
					if (focusedItem)
					{
						itemPlayerOwner = PlayerBase.Cast(focusedItem.GetHierarchyRootPlayer());
						il = new InventoryLocation;
						focusedItem.GetInventory().GetCurrentInventoryLocation( il );
						
						canBeManipulated = !player.GetInventory().HasInventoryReservation( focusedItem, null ) && !player.GetInventory().IsInventoryLocked() && !player.IsItemsToDelete();
						m_NeedUpdateConsoleToolbar |= !canBeManipulated;
						canBeManipulated = canBeManipulated && focusedItem.CanPutIntoHands(null);
						canBeManipulated = canBeManipulated && focusedItem.GetInventory().CanRemoveEntity();
						
						EntityAI parent = il.GetParent();
						if ( parent && il.GetType() == InventoryLocationType.ATTACHMENT )
						{
							canBeManipulated = canBeManipulated && AttachmentsOutOfReach.IsAttachmentReachable( parent, "", il.GetSlot() );
							canBeManipulated = canBeManipulated && !parent.GetInventory().GetSlotLock(  il.GetSlot() );
						}
					
						if (canBeManipulated)
						{
							if (!ItemManager.GetInstance().IsMicromanagmentMode())
							{
								combinationFlag |= ConsoleActionToolbarMask.MICROMANAGMENT;
							}
							
							if (focusedContainer.CanEquipEx(focusedItem))
							{
								combinationFlag |= ConsoleActionToolbarMask.EQUIP;
							}
							
							if (player!= null && player == itemPlayerOwner)
							{
								if (focusedContainer.CanSwapOrTakeToHandsEx(focusedItem))
								{
									combinationFlag |= ConsoleActionToolbarMask.TO_HANDS_SWAP_INVENTORY;
								}
								
								if (focusedContainer.CanDropEx(focusedItem))
								{
									combinationFlag |= ConsoleActionToolbarMask.DROP;
								}
							}
							else
							{
								if (focusedContainer.CanSwapOrTakeToHandsEx(focusedItem))
								{
									combinationFlag |= ConsoleActionToolbarMask.TO_HANDS_SWAP_VICINITY;
								}
							}
							
							if (focusedContainer.CanCombineEx(focusedItem))
							{
								combinationFlag |= ConsoleActionToolbarMask.COMBINE;
							}
							
							if (focusedContainer.CanSplitEx(focusedItem))
							{
								combinationFlag |= ConsoleActionToolbarMask.SPLIT;
							}
							
							if (focusedArea.CanTakeToInventoryEx(focusedItem))
							{
								combinationFlag |= ConsoleActionToolbarMask.TO_INVENTORY;
							}
						}
					}
					
					if (focusedContainer.CanOpenCloseContainerEx(focusedItem))
					{
						combinationFlag |= ConsoleActionToolbarMask.OPEN_CLOSE_CONTAINER;
					}
					
					if (player!= null && focusedContainer.CanAddToQuickbarEx(focusedItem))
					{
						combinationFlag |= ConsoleActionToolbarMask.QUICKSLOT;
					}
				}
			}
			contextualText = GetConsoleToolbarText(combinationFlag);
		if (m_TopConsoleToolbarVicinity)
			m_TopConsoleToolbarVicinity.Show(m_LeftArea.IsActive());
		if (m_TopConsoleToolbarHands)
			m_TopConsoleToolbarHands.Show(m_HandsArea.IsActive());
		if (m_TopConsoleToolbarEquipment)
			m_TopConsoleToolbarEquipment.Show(m_RightArea.IsActive());
		if (m_BottomConsoleToolbarRichText)
			m_BottomConsoleToolbarRichText.SetText(contextualText + " ");
		}
		#endif
	}
	
	//! Picks from the strings by active input limiter variant in order: {click,hold,doubleclick}. Intended for inputs that check 'LocalPress' exclusively!!
	string GetStringVariant(string pInputAction, notnull array<string> variants)
	{
		if (variants.Count() != 3)
		{
			ErrorEx("wrong array count!");
			return "";
		}
		
		UAInput inp = GetUApi().GetInputByName(pInputAction);
		if (!inp.IsLimited() || inp.IsClickLimit()) //returns 'click' (no extension) variant as default
		{
			return variants[0];
		}
		if (inp.IsHoldLimit() || inp.IsHoldBeginLimit())
		{
			return variants[1];
		}
		if (inp.IsDoubleClickLimit())
		{
			return variants[2];
		}
		ErrorEx("Unhandled limiter exception!");
		return "";
	}
	
	//! Shifts between containers vertically
	void MoveFocusByContainer(int direction)
	{
		HideOwnedTooltip();
		
		if (direction == Direction.UP)
		{
			if ( m_LeftArea.IsActive() )
			{
				m_LeftArea.SetSameLevelPreviousActive();
			}
			else if ( m_RightArea.IsActive() )
			{
				m_RightArea.SetSameLevelPreviousActive();
			}
			else if ( m_HandsArea.IsActive() )
			{
				m_HandsArea.SetSameLevelPreviousActive();
			}
			
			UpdateConsoleToolbar();
		}
		else if (direction == Direction.DOWN)
		{
			if ( m_LeftArea.IsActive() )
			{
				m_LeftArea.SetSameLevelNextActive();
			}
			else if ( m_RightArea.IsActive() )
			{
				m_RightArea.SetSameLevelNextActive();
			}
			else if ( m_HandsArea.IsActive() )
			{
				m_HandsArea.SetSameLevelNextActive();
			}
			
			UpdateConsoleToolbar();
		}
	}
	
	//! Shifts between vicinity-hands-player
	void MoveFocusByArea(int direction)
	{
		HideOwnedTooltip();
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		
		if (direction == Direction.LEFT)
		{
			if (m_LeftArea.IsActive())
			{
				if (!ItemManager.GetInstance().IsMicromanagmentMode())
				{
					m_LeftArea.UnfocusGrid();
				}
				m_LeftArea.SetActive(false);
				m_RightArea.SetActive(true);
				
				UpdateConsoleToolbar();
			}
			else if (m_RightArea.IsActive())
			{
				if (!ItemManager.GetInstance().IsMicromanagmentMode())
				{
					m_RightArea.UnfocusGrid();
				}
				m_RightArea.SetActive(false);
				player = PlayerBase.Cast(GetGame().GetPlayer());
				EntityAI item_in_hands = player.GetItemInHands();
				m_HandsArea.SetActive(true);

				UpdateConsoleToolbar();
			}
			else if (m_HandsArea.IsActive())
			{
				m_HandsArea.UnfocusGrid();
				m_HandsArea.SetActive(false);
				m_LeftArea.SetActive(true);
				
				UpdateConsoleToolbar();
			}
		}
		else if (direction == Direction.RIGHT)
		{
			if (m_LeftArea.IsActive())
			{
				if (!ItemManager.GetInstance().IsMicromanagmentMode())
				{
					m_LeftArea.UnfocusGrid();
				}
				m_LeftArea.SetActive(false);
				player = PlayerBase.Cast(GetGame().GetPlayer());
				item_in_hands = player.GetItemInHands();
				m_HandsArea.SetActive(true);
				
				UpdateConsoleToolbar();
			}
			else if (m_RightArea.IsActive())
			{
				if (!ItemManager.GetInstance().IsMicromanagmentMode())
				{
					m_RightArea.UnfocusGrid();
				}
				m_RightArea.SetActive(false);
				m_LeftArea.SetActive(true);
				
				UpdateConsoleToolbar();
			}
			else if (m_HandsArea.IsActive())
			{
				m_HandsArea.UnfocusGrid();
				m_HandsArea.SetActive(false);
				m_RightArea.SetActive(true);
				
				UpdateConsoleToolbar();
			}
		}
	}
	
	void InventoryMovementButtonTickHandler(float timeslice)
	{
		float tickvalue = 0;
		
		if (m_SensitivityThreshold > 0.0)
		{
			if (m_SensitivityThreshold < BT_REPEAT_TIME) //sensitivity solution...
			{
				m_SensitivityThreshold += timeslice;
				return;
			}
			else
			{
				m_SensitivityThreshold = 0.0;
			}
		}
		
		for (int i = 0; i < m_InvInputWrappersCount; i++)
		{
			m_InvUAInput = m_InvInputWrappers[i].InputP();
			
			if (m_InvUAInput.LocalValue() > BT_THRESHOLD_VALUE)
			{
				tickvalue = m_InvInputTimes[i];
				tickvalue += timeslice;
			}
			else
			{
				tickvalue = 0.0;
			}
			
			if (tickvalue < BT_REPEAT_DELAY && m_InvUAInput.LocalPress())
			{
				m_InvInputActive |= (1 << i);
				m_SensitivityThreshold += timeslice;
			}
			else if (tickvalue > (BT_REPEAT_DELAY + BT_REPEAT_TIME))
			{
				while (tickvalue > (BT_REPEAT_DELAY + BT_REPEAT_TIME))
				{
					tickvalue -= BT_REPEAT_TIME;
				}
				
				m_InvInputActive |= (1 << i);
				m_SensitivityThreshold += timeslice;
			}
			else
			{
				m_InvInputActive &= ~(1 << i);
			}
			
			m_InvInputTimes[i] = tickvalue;
		}
	}
	
	// DEPRECATED below
	protected Widget m_SpecializationPanel;
	protected Widget m_SpecializationIcon;
	
	void UpdateSpecialtyMeter();
}
