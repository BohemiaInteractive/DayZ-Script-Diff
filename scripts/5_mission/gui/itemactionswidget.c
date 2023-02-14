class ItemActionsWidget extends ScriptedWidgetEventHandler
{
	protected PlayerBase 				m_Player;
	protected EntityAI 					m_EntityInHands;
	protected ActionBase				m_Interact;
	protected ActionBase				m_ContinuousInteract;
	protected ActionBase				m_Single;
	protected ActionBase				m_Continuous;
	protected ActionManagerClient	 	m_AM;
	protected IngameHud					m_Hud;
	
	protected UAIDWrapper 				m_UseActionWrapper;

	protected int						m_InteractActionsNum;
	protected int						m_ContinuousInteractActionsNum;
	protected int						m_ItemActionsNum;
	protected int						m_ContinuousItemActionsNum;
	protected bool 						m_HealthEnabled;
	protected bool						m_QuantityEnabled;
	
	protected ref WidgetFadeTimer		m_FadeTimer;
	protected bool						m_Faded;
	protected bool 						m_Hidden;

	protected Widget					m_Root;
	protected Widget 					m_ItemLeft;

	//! widget width
	protected float m_MaxWidthChild;
	protected float m_RootWidth;
	protected float m_RootHeight;
	
	void ItemActionsWidget()
	{
		m_Interact				= null;
		m_ContinuousInteract 	= null;
		m_Single 				= null;
		m_Continuous 			= null;
		m_AM 					= null;

		m_FadeTimer 			= new WidgetFadeTimer;
		m_Faded 				= true;
		
		m_HealthEnabled 		= true;
		m_QuantityEnabled 		= true;
		
		m_Hud					= GetHud();
		m_Hidden 				= true;
		
		m_UseActionWrapper 		= GetUApi().GetInputByID(UAAction).GetPersistentWrapper();
		
		GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Insert(Update);
		GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
	}
	
	void ~ItemActionsWidget()
	{
		GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Remove(Update);
	}
	
	//! DEPRECATED
	void SetInteractXboxIcon(string imageset_name, string image_name) {};
	void SetContinuousInteractXboxIcon(string imageset_name, string image_name) {};
	void SetSingleXboxIcon(string imageset_name, string image_name) {};
	void SetContinuousXboxIcon(string imageset_name, string image_name) {};
	protected void SetXboxIcon(string name, string imageset_name, string image_name) {};
	//! ----------
	
	protected void SetControllerIcon(string pWidgetName, string pInputName)
	{
		RichTextWidget w = RichTextWidget.Cast(m_Root.FindAnyWidget(pWidgetName + "_btn_icon_xbox"));
		w.SetText(InputUtils.GetRichtextButtonIconFromInputAction(pInputName, "", EUAINPUT_DEVICE_CONTROLLER));
	}

	protected void ShowXboxHidePCIcons(string widget, bool show_xbox_icon)
	{
		m_Root.FindAnyWidget(widget + "_btn_icon_xbox").Show(show_xbox_icon);
		m_Root.FindAnyWidget(widget + "_btn_icon").Show(!show_xbox_icon);
	}

	protected void OnWidgetScriptInit(Widget w)
	{
		m_Root = w;
		m_Root.SetHandler(this);
		m_Root.Show(false);		

		m_ItemLeft = w.FindAnyWidget("ia_item_left");
		
		//! UA -> widget icon
		UpdateControllerInputIcons();
		UpdatePCIconsVisibility();
	}
	
	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		UpdateControllerInputIcons();
		#endif
	}

	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		UpdatePCIconsVisibility();
	}
	
	//! Loads icons from the latest keybinds
	private void UpdateControllerInputIcons()
	{
		SetControllerIcon("ia_interact", "UAAction");
		SetControllerIcon("ia_continuous_interact", "UAAction");
		SetControllerIcon("ia_single", "UADefaultAction");
		SetControllerIcon("ia_continuous", "UADefaultAction");
	}
	
	//! Contains logic for icon set switching (Gamepad/M&K)
	private void UpdatePCIconsVisibility()
	{
		bool showConsoleIcons = false;
		#ifdef PLATFORM_CONSOLE
		showConsoleIcons = GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER || !GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer();
		#endif
		
		ShowXboxHidePCIcons("ia_interact", showConsoleIcons);
		ShowXboxHidePCIcons("ia_continuous_interact", showConsoleIcons);
		ShowXboxHidePCIcons("ia_single", showConsoleIcons);
		ShowXboxHidePCIcons("ia_continuous", showConsoleIcons);
	}

	protected void BuildCursor()
	{
		int health 				= -1;
		int quantityType 		= 0;
		int quantityMin			= -1;
		int quantityMax 		= -1;
		float quantityCurrent 	= -1.0;

		// item health
		health = GetItemHealth();
		SetItemHealth(health, "ia_item", "ia_item_health_mark", m_HealthEnabled);
		
		// quantity
		//! weapon specific
		if (m_EntityInHands && m_EntityInHands.IsWeapon())
		{
			SetWeaponQuantity(0, 0, "ia_item", "ia_item_quantity_pb", "ia_item_quantity_text", m_QuantityEnabled);
			SetWeaponModeAndZeroing("ia_item_subdesc", "ia_item_subdesc_up", "ia_item_subdesc_down", true);
		}
		//! transmitter/PAS specific
		else if (m_EntityInHands && m_EntityInHands.IsTransmitter())
		{
			SetRadioFrequency(GetRadioFrequency(), "ia_item_subdesc", "ia_item_subdesc_up", "ia_item_subdesc_down", m_QuantityEnabled);
			GetItemQuantity(quantityType, quantityCurrent, quantityMin, quantityMax);
			SetItemQuantity(quantityType, quantityCurrent, quantityMin, quantityMax, "ia_item", "ia_item_quantity_pb", "ia_item_quantity_text", true);
		}
		else
		{
			GetItemQuantity(quantityType, quantityCurrent, quantityMin, quantityMax);
			SetItemQuantity(quantityType, quantityCurrent, quantityMin, quantityMax, "ia_item", "ia_item_quantity_pb", "ia_item_quantity_text", m_QuantityEnabled);
			SetWeaponModeAndZeroing("ia_item_subdesc", "", "", false);
		}
	
		SetItemDesc(m_EntityInHands, GetItemDesc(m_EntityInHands), "ia_item", "ia_item_desc");
		SetInteractActionIcon("ia_interact", "ia_interact_icon_frame", "ia_interact_btn_inner_icon", "ia_interact_btn_text");
		SetActionWidget(m_Interact, GetActionDesc(m_Interact), "ia_interact", "ia_interact_action_name");

		SetInteractActionIcon("ia_continuous_interact", "ia_continuous_interact_icon_frame", "ia_continuous_interact_btn_inner_icon", "ia_continuous_interact_btn_text");
		SetActionWidget(m_ContinuousInteract, GetActionDesc(m_ContinuousInteract), "ia_continuous_interact", "ia_continuous_interact_action_name");

		SetActionWidget(m_Single, GetActionDesc(m_Single), "ia_single", "ia_single_action_name");
		SetActionWidget(m_Continuous, GetActionDesc(m_Continuous), "ia_continuous", "ia_continuous_action_name");
		SetMultipleInteractAction("ia_interact_mlt_wrapper");
		SetMultipleContinuousInteractAction("ia_continuous_interact_mlt_wrapper");
		SetMultipleItemAction("ia_single_mlt_wrapper");
		SetMultipleContinuousItemAction("ia_continuous_mlt_wrapper");
	}
	
	protected void Update()
	{
		//! don't show when disabled in profile
		if (!g_Game.GetProfileOption(EDayZProfilesOptions.HUD) || !m_Hud.IsHudVisible())
		{
			m_Root.Show(false);
			return;
		}

		if (m_Player && !m_Player.IsAlive()) // handle respawn
		{
			m_Player = null;
			m_AM = null;
		}

		if (!m_Player)
			GetPlayer();

		if (!m_AM)
			GetActionManager();

		GetActions();
		GetEntityInHands();

		if ((m_EntityInHands || !m_Hidden) && GetGame().GetUIManager().GetMenu() == null)
		{
			CheckForActionWidgetOverrides();
			BuildCursor();
			CheckRefresherFlagVisibility();
			m_Root.Show(true);
		}
		else
			m_Root.Show(false);

		m_MaxWidthChild = 200;
	}

	// getters
    protected void GetPlayer()
	{
		Class.CastTo(m_Player, GetGame().GetPlayer());
	}

	protected void GetActionManager()
	{
		if (m_Player && m_Player.IsPlayerSelected())
			ActionManagerClient.CastTo(m_AM, m_Player.GetActionManager());
		else
			m_AM = null;
	}

    protected void GetActions()
	{
		m_Interact = null;
		m_ContinuousInteract = null;
		m_Single = null;
		m_Continuous = null;

		if (!m_AM)
			return;

		if (m_Player.IsSprinting())
			return;
		
		m_Interact = m_AM.GetPossibleAction(InteractActionInput);
		m_ContinuousInteract = m_AM.GetPossibleAction(ContinuousInteractActionInput);
		m_Single = m_AM.GetPossibleAction(DefaultActionInput);
		m_Continuous = m_AM.GetPossibleAction(ContinuousDefaultActionInput);
		
		if (!m_Interact && !m_ContinuousInteract && !m_Continuous && !m_Single)
			m_Hidden = false;
		
		m_InteractActionsNum = m_AM.GetPossibleActionCount(InteractActionInput);
		m_ContinuousInteractActionsNum = m_AM.GetPossibleActionCount(ContinuousInteractActionInput);
		m_ItemActionsNum = m_AM.GetPossibleActionCount(DefaultActionInput);
		m_ContinuousItemActionsNum = m_AM.GetPossibleActionCount(ContinuousDefaultActionInput);
	}

	protected void GetEntityInHands()
	{
		if (!m_Player) return;
		
		EntityAI eai = m_Player.GetHumanInventory().GetEntityInHands();
		
		if (eai && !eai.IsInherited(DummyItem))
		{
			m_EntityInHands = eai;
		}
		else
		{
			m_EntityInHands = null;
		}
	}
	
	protected string GetActionDesc(ActionBase action)
	{
		if (action && action.GetText())
			return action.GetText();

		return string.Empty;
	}
	
	protected string GetItemDesc(EntityAI entity)
	{
		if (m_EntityInHands)
			return m_EntityInHands.GetDisplayName();

		return string.Empty;
	}
	
	protected int GetItemHealth()
	{
		if (m_EntityInHands)
			return m_EntityInHands.GetHealthLevel();
		
		return -1;
	}

	protected void GetItemQuantity(out int q_type, out float q_cur, out int q_min, out int q_max)
	{
		if (m_EntityInHands)
		{
			InventoryItem item = InventoryItem.Cast(m_EntityInHands);
			q_type = QuantityConversions.HasItemQuantity(m_EntityInHands);
			if (q_type > 0)
			{
				if (q_type == QUANTITY_PROGRESS)
				{
					QuantityConversions.GetItemQuantity(item, q_cur, q_min, q_max);

					q_cur = (q_cur / q_max);
					// to percents
					q_min = 0;
					q_cur = q_cur * 100;
					q_max = 100;
				}
				else
				{
					QuantityConversions.GetItemQuantity(item, q_cur, q_min, q_max);
				}
			}
		}
	}
	
	protected string GetRadioFrequency()
	{
		TransmitterBase trans;
		if (Class.CastTo(trans, m_EntityInHands))
		{
			return trans.GetTunedFrequency().ToString();
		}

		return string.Empty;
	}
	
	// setters
	protected void SetItemDesc(EntityAI entity, string descText, string itemWidget, string descWidget)
	{
		Widget widget;
		
		widget = m_Root.FindAnyWidget(itemWidget);
		
		if (entity && !m_Player.GetItemAccessor().IsItemInHandsHidden())
		{
			descText.ToUpper();
			TextWidget itemName;
			Class.CastTo(itemName, widget.FindAnyWidget(descWidget));
			itemName.SetText(descText);
			widget.Show(true);
		}
		else
			widget.Show(false);
	}
	
	protected void SetItemHealth(int health, string itemWidget, string healthWidget, bool enabled)
	{
		Widget widget;
		
		widget = m_Root.FindAnyWidget(itemWidget);
		
		if (enabled)
		{
			ImageWidget healthMark;
			Class.CastTo(healthMark, widget.FindAnyWidget(healthWidget));

			switch (health)
			{
				case -1 :
					healthMark.GetParent().Show(false);
					break;
				case GameConstants.STATE_PRISTINE :
					healthMark.SetColor(Colors.COLOR_PRISTINE);
					healthMark.SetAlpha(0.5);
					healthMark.GetParent().Show(true);
					break;
				case GameConstants.STATE_WORN :
					healthMark.SetColor(Colors.COLOR_WORN);
					healthMark.SetAlpha(0.5);
					healthMark.GetParent().Show(true);
					break;
				case GameConstants.STATE_DAMAGED :
					healthMark.SetColor(Colors.COLOR_DAMAGED);
					healthMark.SetAlpha(0.5);
					healthMark.GetParent().Show(true);
					break;
				case GameConstants.STATE_BADLY_DAMAGED:
					healthMark.SetColor(Colors.COLOR_BADLY_DAMAGED);
					healthMark.SetAlpha(0.5);
					healthMark.GetParent().Show(true);
					break;
				case GameConstants.STATE_RUINED :
					healthMark.SetColor(Colors.COLOR_RUINED);
					healthMark.SetAlpha(0.5);
					healthMark.GetParent().Show(true);
					break;
				default :
					healthMark.SetColor(0x00FFFFFF);
					healthMark.SetAlpha(0.5);
					healthMark.GetParent().Show(true);
					break;			
			}
			
			widget.Show(true);
		}
		else
			widget.Show(false);
	}
	
	protected void SetItemQuantity(int type, float current, int min, int max, string itemWidget, string quantityPBWidget, string quantityTextWidget, bool enabled)
	{
		Widget widget;
		
		widget = m_Root.FindAnyWidget(itemWidget);
		
		if (enabled)
		{
			ProgressBarWidget progressBar;
			TextWidget textWidget;
			Class.CastTo(progressBar, widget.FindAnyWidget(quantityPBWidget));
			Class.CastTo(textWidget, widget.FindAnyWidget(quantityTextWidget));

			//check for volume vs. count and display progressbar or "bubble" with exact count/max text
			switch (type)
			{
				case QUANTITY_HIDDEN :
					progressBar.Show(false);
					textWidget.Show(false);
					textWidget.GetParent().Show(false);
					break;
				case QUANTITY_COUNT :
					if (max > 1 || current > 1)
					{
						string qty_text = string.Format("%1/%2", Math.Round(current).ToString(), max.ToString());
						progressBar.Show(false);
						textWidget.SetText(qty_text);
						textWidget.Show(true);
						textWidget.GetParent().Show(true);
						break;
					}
					else
					{
						progressBar.Show(false);
						textWidget.Show(false);
						textWidget.GetParent().Show(false);
						break;
					}
				case QUANTITY_PROGRESS :	
					textWidget.Show(false);
					progressBar.SetCurrent(current);
					progressBar.Show(true);
					textWidget.GetParent().Show(true);
					break;
			}
			widget.Show(true);
		}
		else
			widget.Show(false);	
	}
	
	protected void SetWeaponQuantity(int chamber, int mag, string itemWidget, string quantityPBWidget, string quantityTextWidget, bool enabled)
	{
		Widget widget;
		
		widget = m_Root.FindAnyWidget(itemWidget);
		
		if (enabled)
		{
			string wpn_qty = "";

			ProgressBarWidget progressBar;
			TextWidget textWidget;
			Class.CastTo(progressBar, widget.FindAnyWidget(quantityPBWidget));
			Class.CastTo(textWidget, widget.FindAnyWidget(quantityTextWidget));
			
			Weapon_Base wpn;
			Magazine maga;
			int mag_quantity = -1;

			if (Class.CastTo(wpn, m_EntityInHands))
			{
				if (Magnum_Base.Cast(wpn))
				{
					mag_quantity = 0;
					for (int j = 0; j < wpn.GetMuzzleCount(); j++)
					{
						if (wpn.IsChamberFull(j)&& !wpn.IsChamberFiredOut(j))
							mag_quantity++; 
					}
					wpn_qty = mag_quantity.ToString();
				}
				else
				{
					for (int i = 0; i < wpn.GetMuzzleCount(); i++)
					{
						if (i > 0 && (wpn.GetMuzzleCount() < 3 ||  i%2 == 0))
						{
							wpn_qty = wpn_qty + " ";
						}
						if (wpn.IsChamberEmpty(i))
						{
							wpn_qty = wpn_qty + "0";
						}
						else if (wpn.IsChamberFiredOut(i))
						{
							wpn_qty = wpn_qty + "F";
						}
						else
						{
							wpn_qty = wpn_qty + "1";
						}
						
						maga = wpn.GetMagazine(i);
						if (maga)
						{
							mag_quantity = maga.GetAmmoCount();
						}
						else if (wpn.GetInternalMagazineMaxCartridgeCount(i) > 0)
						{
							mag_quantity = wpn.GetInternalMagazineCartridgeCount(i);
						}
					
					}
				
					if (wpn.IsJammed())
					{
						if (mag_quantity != -1)
							wpn_qty = string.Format("X (+%1)", mag_quantity);
						else
							wpn_qty = "X";
					}
					else
					{
						if (mag_quantity != -1)
						{
							wpn_qty = wpn_qty + " (" + mag_quantity.ToString() + ")";
						}
					}
				}
			}
	
			progressBar.Show(false);
			textWidget.SetText(wpn_qty);
			textWidget.Show(true);
			textWidget.GetParent().Show(true);
		}
		else
			widget.Show(false);
	}
	
	protected void SetWeaponModeAndZeroing(string itemWidget, string upWidget, string downWidget, bool enabled)
	{
		Widget widget;
		
		widget = m_Root.FindAnyWidget(itemWidget);

		if (enabled)
		{
			TextWidget txtModeWidget;
			TextWidget txtZeroingWidget;

			Class.CastTo(txtModeWidget, widget.FindAnyWidget(upWidget));
			Class.CastTo(txtZeroingWidget, widget.FindAnyWidget(downWidget));
			
			Weapon w = Weapon.Cast(m_Player.GetHumanInventory().GetEntityInHands());
			string zeroing	= string.Empty;
			if (w)
				zeroing	= string.Format("%1 m", w.GetCurrentZeroing(w.GetCurrentMuzzle()));

			txtModeWidget.SetText(m_Player.GetWeaponManager().GetCurrentModeName());
			txtZeroingWidget.SetText(zeroing);
			widget.Show(true);
		}
		else
			widget.Show(false);
	}
	
	// not used instead of this is used confing parameter for fire mode
	protected string WeaponModeTextTemp()
	{
		return "unused";
	}
	
	protected void SetRadioFrequency(string freq, string itemWidget, string upWidget, string downWidget, bool enabled)
	{
		Widget widget;
		
		widget = m_Root.FindAnyWidget(itemWidget);
		
		if (enabled)
		{
			TextWidget txtUpWidget;
			TextWidget txtDownWidget;
			Class.CastTo(txtUpWidget, widget.FindAnyWidget(upWidget));
			Class.CastTo(txtDownWidget, widget.FindAnyWidget(downWidget));

			txtUpWidget.SetText(freq);
			txtDownWidget.SetText("MHz");
			widget.Show(true);
		}
		else
			widget.Show(false);	
	}
	
	protected void SetActionWidget(ActionBase action, string descText, string actionWidget, string descWidget)
	{
		Widget widget = m_Root.FindAnyWidget(actionWidget);

		if (action && (!action.HasTarget() || m_Player.GetCommand_Vehicle()))
		{
			TextWidget actionName;
			Class.CastTo(actionName, widget.FindAnyWidget(descWidget));
			

			if (action.GetInput().GetInputType() == ActionInputType.AIT_CONTINUOUS)
			{
				descText = descText + " " + "#action_target_cursor_hold";
				actionName.SetText(descText);
			}
			else
			{
				actionName.SetText(descText);
			}

			widget.Show(true);
		}
		else
		{
			widget.Show(false);
		}
	}

	//! shows arrows near the interact action if there are more than one available
	protected void SetMultipleInteractAction(string multiActionsWidget)
	{
		Widget widget;

		widget = m_Root.FindAnyWidget(multiActionsWidget);
		widget.Show(m_InteractActionsNum > 1);
	}
	
	//! shows arrows near the interact action if there are more than one available
	protected void SetMultipleContinuousInteractAction(string multiActionsWidget)
	{
		Widget widget;

		widget = m_Root.FindAnyWidget(multiActionsWidget);
		widget.Show(m_ContinuousInteractActionsNum > 1);
	}
	
	protected void SetMultipleItemAction(string multiActionsWidget)
	{
		Widget widget;

		widget = m_Root.FindAnyWidget(multiActionsWidget);
		widget.Show(m_ItemActionsNum > 1);
	}
	
	//! shows arrows near the interact action if there are more than one available
	protected void SetMultipleContinuousItemAction(string multiActionsWidget)
	{
		Widget widget;

		widget = m_Root.FindAnyWidget(multiActionsWidget);
		widget.Show(m_ContinuousItemActionsNum > 1);
	}
	
	protected void SetInteractActionIcon(string actionWidget, string actionIconFrameWidget, string actionIconWidget, string actionIconTextWidget)
	{
		string keyName = string.Empty;
		Widget widget, frameWidget;
		ImageWidget iconWidget;
		TextWidget textWidget;
		
		widget = m_Root.FindAnyWidget(actionWidget);
		Class.CastTo(frameWidget, widget.FindAnyWidget(actionIconFrameWidget));
		Class.CastTo(iconWidget, widget.FindAnyWidget(actionIconWidget));
		Class.CastTo(textWidget, widget.FindAnyWidget(actionIconTextWidget));
		
		m_UseActionWrapper.InputP().SelectAlternative(0); //! select first alternative (which is the primary bind)
		for (int c = 0; c < m_UseActionWrapper.InputP().BindKeyCount(); c++)
		{
		  	int _hc = m_UseActionWrapper.InputP().GetBindKey(0);
		  	keyName = GetUApi().GetButtonName(_hc);
		}

		// uses text in floating widget
		iconWidget.Show(false);
		textWidget.SetText(keyName);
		textWidget.Show(true);	
	}
	
	protected void CheckRefresherFlagVisibility()
	{
		Widget w = m_Root.FindAnyWidget("ia_item_flag_icon");
		if (m_Player && m_Player.GetHologramLocal())
		{
			EntityAI entity = m_Player.GetHologramLocal().GetProjectionEntity();
			w.Show(entity.IsRefresherSignalingViable() && m_Player.IsTargetInActiveRefresherRange(entity));
		}
		else if (w.IsVisible())
		{
			w.Show(false);
		}
	}
	
	protected void CheckForActionWidgetOverrides()
	{
		ItemBase item;
		typename actionName;
		if (Class.CastTo(item, m_EntityInHands) && item.GetActionWidgetOverride(actionName))
		{
			ActionBase action = ActionManagerBase.GetAction(actionName);
			if (action)
			{
				m_Single = action;
			}
		}
	}
	
	protected IngameHud GetHud()
	{
		Mission mission = GetGame().GetMission();
		if (mission)
		{
			IngameHud hud = IngameHud.Cast(mission.GetHud());
			return hud;
		}

		return null;
	}
}