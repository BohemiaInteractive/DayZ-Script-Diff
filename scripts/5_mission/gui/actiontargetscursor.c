class ATCCachedObject
{
	protected Object 	m_CachedObject;
	protected vector 	m_CursorWPos;
	protected vector	m_ScreenPos;
	protected int		m_CompIdx;
	
	void ATCCachedTarget()
	{
		m_CachedObject = null;
		m_ScreenPos = vector.Zero;
		m_CompIdx = -1;
	}

	//! cache object and its world pos
	void Store(Object obj, vector pos, int compIdx)
	{
		if (!m_CachedObject)
		{
			m_CachedObject = obj;
			m_CursorWPos = pos;
			m_CompIdx = compIdx;
		}
	}

	//! invalidate cached objec
	void Invalidate()
	{
		if (m_CachedObject)
		{
			m_CachedObject = null;
			m_CursorWPos = vector.Zero;
			m_CompIdx = -1;
		}
	}

	Object Get()
	{
		return m_CachedObject; 	
	}
	
	vector GetCursorWorldPos()
	{
		return m_CursorWPos;
	}
	
	int GetCursorCompIdx()
	{
		return m_CompIdx;
	}
};

class ActionTargetsCursor : ScriptedWidgetEventHandler
{
	private const ref array<typename> VISION_OBSTRUCTION_PPEFFECTS_TYPES = {
		PPERequester_BurlapSackEffects,
		PPERequester_FlashbangEffects
	};

	protected PlayerBase 					m_Player;
	protected ActionTarget 					m_Target;
	protected ref ATCCachedObject			m_CachedObject;
	protected Object						m_DisplayInteractTarget;

	protected ActionBase					m_Interact;
	protected ActionBase					m_ContinuousInteract;
	protected ActionBase					m_Single;
	protected ActionBase					m_Continuous;
	protected ActionManagerClient 			m_AM;
	protected IngameHud						m_Hud;
	
	protected UAIDWrapper 					m_UseActionWrapper;

	protected int							m_InteractActionsNum;
	protected int							m_ContinuousInteractActionsNum;
	protected int							m_ItemActionsNum;
	protected int							m_ContinuousItemActionsNum;
	protected typename						m_SelectedActionCategory;
	protected bool 							m_HealthEnabled;
	protected bool							m_QuantityEnabled;	
	protected bool							m_FixedOnPosition;
	protected bool 							m_Hidden;
	protected bool 							m_TargetItemFrozen;

	protected Widget						m_Root;
	protected Widget						m_Container;
	protected Widget 						m_ItemLeft;

	//! widget width
	protected float m_MaxWidthChild;
	protected float m_RootWidth;
	protected float m_RootHeight;

	void ActionTargetsCursor()
	{
		m_Interact 				= null;
		m_ContinuousInteract	= null;
		m_Single				= null;
		m_Continuous			= null;
		m_AM					= null;
		
		m_HealthEnabled			= true;
		m_QuantityEnabled		= true;
		
		m_CachedObject 			= new ATCCachedObject;
		m_Hidden 				= false;
		m_TargetItemFrozen 		= false;
		m_DisplayInteractTarget	= null;
		
		m_Hud					= GetHud();
		
		m_UseActionWrapper 		= GetUApi().GetInputByID(UAAction).GetPersistentWrapper();
		
		GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
	}
	
	void ~ActionTargetsCursor() {}
	
	// Controls appearance of the builded cursor
	void SetHealthVisibility(bool state)
	{
		m_HealthEnabled = state;
	}

	void SetQuantityVisibility(bool state)
	{
		m_QuantityEnabled = state;
	}

	//! DEPRECATED
	void SetInteractXboxIcon(string imageset_name, string image_name);
	void SetContinuousInteractXboxIcon(string imageset_name, string image_name);
	void SetSingleXboxIcon(string imageset_name, string image_name);
	void SetContinuousXboxIcon(string imageset_name, string image_name);
	protected void SetXboxIcon(string name, string imageset_name, string image_name);
	//! ---------
	
	protected void SetControllerIcon(string pWidgetName, string pInputName)
	{
		RichTextWidget w = RichTextWidget.Cast(m_Root.FindAnyWidget(pWidgetName + "_btn_icon_xbox"));	
		w.SetText(InputUtils.GetRichtextButtonIconFromInputAction(pInputName, "", EUAINPUT_DEVICE_CONTROLLER));
	}
	
	protected void OnWidgetScriptInit(Widget w)
	{
		m_Root = w;
		m_Root.Show(false);
		m_Root.SetHandler(this);
		
		//! UA -> widget icon
		UpdateControllerInputIcons();
		UpdatePCIconsVisibility();

		m_Container = w.FindAnyWidget("container");
		m_ItemLeft = w.FindAnyWidget("item_left");

		m_Root.Update();
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
		SetControllerIcon("interact", "UAAction");
		SetControllerIcon("continuous_interact", "UAAction");
		SetControllerIcon("single", "UADefaultAction");
		SetControllerIcon("continuous", "UADefaultAction");
	}
	
	//! Contains logic for icon set switching (Gamepad/M&K)
	private void UpdatePCIconsVisibility()
	{
		bool showConsoleIcons = false;
		#ifdef PLATFORM_CONSOLE
		showConsoleIcons = GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER || !GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer();
		#endif
		
		ShowXboxHidePCIcons("interact", showConsoleIcons);
		ShowXboxHidePCIcons("continuous_interact", showConsoleIcons);
		ShowXboxHidePCIcons("continuous", showConsoleIcons);
		ShowXboxHidePCIcons("single", showConsoleIcons);
	}

	protected void PrepareCursorContent()
	{
		m_TargetItemFrozen = false;
		int health = -1;
		int cargoCount = 0;
		int q_type = 0;
		int q_min, q_max = -1;
		float q_cur = -1.0;
		
		//! item health
		m_TargetItemFrozen = GetItemFrozen();
		health = GetItemHealth();
		SetItemHealth(health, "item", "item_health_mark", m_HealthEnabled);
		//! quantity
		GetItemQuantity(q_type, q_cur, q_min, q_max);
		//! cargo in item
		GetItemCargoCount(cargoCount);
		//! fill the widget with data
		SetItemQuantity(q_type, q_cur, q_min, q_max, "item", "item_quantity_pb", "item_quantity_text", m_QuantityEnabled);
		SetInteractActionIcon("interact", "interact_icon_frame", "interact_btn_inner_icon", "interact_btn_text");
		SetItemDesc(GetItemDesc(m_Interact), cargoCount, "item", "item_desc");
		SetActionWidget(m_Interact, GetActionDesc(m_Interact), "interact", "interact_action_name");

		SetInteractActionIcon("continuous_interact", "continuous_interact_icon_frame", "continuous_interact_btn_inner_icon", "continuous_interact_btn_text");
		SetActionWidget(m_ContinuousInteract, GetActionDesc(m_ContinuousInteract), "continuous_interact", "continuous_interact_action_name");

		SetActionWidget(m_Single, GetActionDesc(m_Single), "single", "single_action_name");
		SetActionWidget(m_Continuous, GetActionDesc(m_Continuous), "continuous", "continuous_action_name");
		SetMultipleItemAction("single_mlt_wrapper", "single_mlt_wrapper_not_select");
		SetMultipleContinuousItemAction("continuous_mlt_wrapper", "continuous_mlt_wrapper_not_select");
		SetMultipleInteractAction("interact_mlt_wrapper", "interact_mlt_wrapper_not_select");
		SetMultipleContinuousInteractAction("continuous_interact_mlt_wrapper", "continuous_interact_mlt_wrapper_not_select");
	}
	
	protected void BuildFixedCursor()
	{
		int w, h, x, y;

		PrepareCursorContent();
		GetScreenSize(w, h);
		x = w/2 + 32;
		y = h/2 + 32;

		m_Root.SetPos(x, y);
	}
	
	protected void BuildFloatingCursor(bool forceRebuild)
	{
		float pos_x, pos_y = 0.0;
		
		PrepareCursorContent();
		
		//! Get OnScreenPos when forced or targeted component differs
		if (forceRebuild || m_Target.GetComponentIndex() != m_CachedObject.GetCursorCompIdx())
		{
			GetOnScreenPosition(pos_x, pos_y);
		}
		//! in case of cached item, all above is reused except the position
		else
		{
			vector screen_pos = TransformToScreenPos(m_CachedObject.GetCursorWorldPos());
	
			pos_x = screen_pos[0];
			pos_y = screen_pos[1];
		}
		
		pos_x = Math.Ceil(pos_x);
		pos_y = Math.Ceil(pos_y);

		Widget parentWdg = m_Root.GetParent();
		
		float screen_w = 0;
		float screen_h = 0;
		
		float wdg_w = 0;
		float wdg_h = 0; 
		
		parentWdg.GetScreenSize(screen_w, screen_h);
		m_Root.GetSize(wdg_w, wdg_h);

		if (pos_x + wdg_w > screen_w)
			pos_x = screen_w - wdg_w;

		if (pos_y + wdg_h > screen_h)
			pos_y = screen_h - wdg_h;
		
		m_Root.SetPos(pos_x, pos_y);
	}
	
	override bool OnUpdate(Widget w)
	{
		if (m_Root == w)
		{
			Update();
			return true;
		}
		
		return false;
	}

	protected void HideWidget()
	{
		if (m_Root.IsVisible())
		{
			m_Root.Show(false);
			m_CachedObject.Invalidate();
		}
	}
	
	void Update()
	{
		if (m_Player && !m_Player.IsAlive()) // handle respawn
		{
			m_Player = null;
			m_AM = null;
		}

		if (!m_Player)
			GetPlayer();

		if (!m_AM)
			GetActionManager();
		
		if (m_Player.IsInVehicle() || m_AM.GetRunningAction())
			m_Hidden = true;
		
		
		/*
		#ifdef DIAG_DEVELOPER
		if (DeveloperFreeCamera.IsFreeCameraEnabled())
			HideWidget();
			return;
		#endif
		*/

		bool isVisionObstructionActive = PPEManagerStatic.GetPPEManager().IsAnyRequesterRunning(VISION_OBSTRUCTION_PPEFFECTS_TYPES);
				
		//! don't show floating widget if it's disabled in profile or the player is unconscious
		if (isVisionObstructionActive || m_Hud.GetHudVisibility().IsContextFlagActive(IngameHudVisibility.HUD_HIDE_FLAGS))
		{
			HideWidget();
			return;
		}

		GetTarget();
		GetActions();

		//! check if action has target, otherwise don't show the widget		
		bool showTarget = (m_Target && !m_Hidden) || m_Interact || m_ContinuousInteract;
		if (!showTarget)
		{
			if (m_Single)
				showTarget = m_Single.HasTarget();

			if (m_Continuous)
				showTarget = showTarget || m_Continuous.HasTarget();
		}

		if (showTarget)
		{
			//! cursor with fixed position (environment interaction mainly)
			if (m_Target.GetObject() == null && (m_Interact || m_ContinuousInteract || m_Single || m_Continuous))
			{
				//Print(">> fixed widget");
				m_CachedObject.Invalidate();
				BuildFixedCursor();
				m_Root.Show(true);
				m_FixedOnPosition = false;
				m_Hidden = false;
				return;
			}
			else if (m_Target.GetObject() != null && !m_Target.GetObject().IsHologram() && (!m_Target.GetParent() || m_Target.GetParent() && !m_Target.GetParent().IsHologram()))
			{
				CheckRefresherFlagVisibilityEx(m_Target);
				//! build cursor for new target
				if (m_Target.GetObject() != m_CachedObject.Get())
				{
					if (!m_FixedOnPosition)
					{
						//Print(">> non-cached widget");
						m_CachedObject.Invalidate();
						BuildFloatingCursor(true);
						m_Root.Show(true);
						m_Hidden = false;
						return;
					}
					else
					{
						//Print(">> non-cached widget (fixed)");
						m_CachedObject.Invalidate();
						BuildFixedCursor();
						m_Root.Show(true);
						m_FixedOnPosition = false;
						m_Hidden = false;
						return;
					}
				}
				//! use cached version for known target - recalculate onscreen pos only
				else if (m_Target.GetObject() == m_CachedObject.Get())
				{
					if (!m_FixedOnPosition)
					{
						//Print(">> cached widget");
						BuildFloatingCursor(false);
						m_Root.Show(true);
						m_Hidden = false;
						return;
					}
					else
					{
						//Print(">> cached widget (fixed)");
						m_CachedObject.Invalidate();
						BuildFixedCursor();
						m_Root.Show(true);
						m_FixedOnPosition = false;
						m_Hidden = false;
						return;
					}
				}
			}
			else
			{
				if (m_Root.IsVisible())
				{
					m_CachedObject.Invalidate();
					m_Root.Show(false);
					m_Hidden = false;

					// remove previous backlit
					GetDayZGame().GetBacklit().HintClear();
				}
			}
		}
		else
		{
			if (m_Root.IsVisible())
			{
				m_CachedObject.Invalidate();
				m_Root.Show(false);
				m_FixedOnPosition = false;

				// remove previous backlit
				GetDayZGame().GetBacklit().HintClear();
			}
			
			m_Hidden = false;
		}
		
		m_MaxWidthChild = 350;
	}
	
	protected void ShowXboxHidePCIcons(string widget, bool show_xbox_icon)
	{
		m_Root.FindAnyWidget(widget + "_btn_icon_xbox").Show(show_xbox_icon);
		m_Root.FindAnyWidget(widget + "_btn_icon").Show(!show_xbox_icon);
	}


	//! transform world pos to screen pos (related to parent widget size)
	protected vector TransformToScreenPos(vector pWorldPos)
	{
		float parent_width, parent_height;
		vector transformed_pos, screen_pos;
		
		//! get relative pos for screen from world pos vector
		screen_pos = GetGame().GetScreenPosRelative(pWorldPos);
		//! get size of parent widget
		m_Root.GetParent().GetScreenSize(parent_width, parent_height);
		
		//! calculate corrent position from relative pos and parent widget size
		transformed_pos[0] = screen_pos[0] * parent_width;
		transformed_pos[1] = screen_pos[1] * parent_height;
		
		return transformed_pos;
	}

	protected void GetOnScreenPosition(out float x, out float y)
	{
		const float 	DEFAULT_HANDLE_OFFSET 	= 0.2;
		const string 	CE_CENTER_COMP_NAME 	= "ce_center";
		const string 	MEM_LOD_NAME 			= LOD.NAME_MEMORY; //! kept for backward compatibility
		
		int compIdx;
		float pivotOffset = 0.0;
		float memOffset   = 0.0;
		string compName;

		bool isTargetForced = false;
		
		vector worldPos;
		vector modelPos;

		LOD lod;

		array<Selection> memSelections	= new array<Selection>();
		array<string> components = new array<string>; // for components with multiple selection
		
		Object object;

		if (m_Target)
		{
			object = m_Target.GetObject();
			compIdx  = m_Target.GetComponentIndex();

			if (m_Target.GetCursorHitPos() == vector.Zero)
				isTargetForced = true;
		}
		else
		{
			return;
		}

		if (object)
		{
			if (!isTargetForced)
			{
				compName = object.GetActionComponentName(compIdx);
				int namedComponentType = object.GetActionComponentNameList(compIdx, components);

				if (!object.IsInventoryItem() && (object.HasFixedActionTargetCursorPosition() || namedComponentType == 0))
				{
					m_FixedOnPosition = true;
					return;
				}
				
				pivotOffset	= object.ConfigGetFloat("actionTargetPivotOffsetY");
				memOffset	= object.ConfigGetFloat("actionTargetMemOffsetY");
	
				//! Get memory LOD from p3d
				lod = object.GetLODByName(MEM_LOD_NAME);
				if (lod != null)
				{
					//! save selection from memory lod
					lod.GetSelections(memSelections);
					
					// items with CE_Center mem point
					if (MiscGameplayFunctions.IsComponentInSelection(memSelections, CE_CENTER_COMP_NAME))
					{
						for (int i2 = 0; i2 < memSelections.Count(); ++i2)
						{
							if (memSelections[i2].GetName() == CE_CENTER_COMP_NAME && memSelections[i2].GetVertexCount() == 1)
							{
								m_FixedOnPosition = false;
								modelPos = object.GetSelectionPositionMS(CE_CENTER_COMP_NAME);
								worldPos = object.ModelToWorld(modelPos);
								if (memOffset != 0.0)
								{
									worldPos[1] = worldPos[1] + memOffset;
								}
								else
								{
									worldPos[1] = worldPos[1] + DEFAULT_HANDLE_OFFSET;
								}
							}
						}

						//! cache current object and the widget world pos
						m_CachedObject.Store(object, worldPos, compIdx);
					} 
					//! doors/handles
					else if (!compName.Contains("ladder") && IsComponentInSelection(memSelections, compName))
					{
						for (int i1 = 0; i1 < memSelections.Count(); ++i1)
						{
							//! single vertex in selection
							if (memSelections[i1].GetName() == compName && memSelections[i1].GetVertexCount() == 1)
							{
								modelPos = object.GetSelectionPositionMS(compName);
								worldPos = object.ModelToWorld(modelPos);

								m_FixedOnPosition = false;
								if (object.GetType() == "Fence" || object.GetType() == "Watchttower" || object.GetType() == "GardenPlot")
									m_FixedOnPosition = true;
								
								if (memOffset != 0.0)
								{
									worldPos[1] = worldPos[1] + memOffset;
								}
								else
								{
									worldPos[1] = worldPos[1] + DEFAULT_HANDLE_OFFSET;
								}
							}

							//! multiple vertices in selection
							if (memSelections[i1].GetName() == compName && memSelections[i1].GetVertexCount() > 1)
							{
								for (int j = 0; j < components.Count(); ++j)
								{
									if (IsComponentInSelection(memSelections, components[j]))
									{
										modelPos = object.GetSelectionPositionMS(components[j]);
										worldPos = object.ModelToWorld(modelPos);

										m_FixedOnPosition = false;
										if (memOffset != 0.0)
										{
											worldPos[1] = worldPos[1] + memOffset;
										}
										else
										{
											worldPos[1] = worldPos[1] + DEFAULT_HANDLE_OFFSET;
										}
									}
								}
							}
						}
						
						//! cache current object and the widget world pos
						m_CachedObject.Store(object, worldPos, -1); //! do not store component index for doors/handles
					}
					//! ladders handling
					else if (compName.Contains("ladder") && IsComponentInSelection(memSelections, compName))
					{
						vector ladderHandlePointLS, ladderHandlePointWS;
						vector closestHandlePos;
						float lastDistance = 0;
	
						for (int i3 = 0; i3 < memSelections.Count(); ++i3)
						{
							if (memSelections[i3].GetName() == compName && memSelections[i3].GetVertexCount() > 1)
							{
								ladderHandlePointLS = memSelections[i3].GetVertexPosition(lod, 0);
								ladderHandlePointWS = object.ModelToWorld(ladderHandlePointLS);
								closestHandlePos = ladderHandlePointWS;
								lastDistance = Math.AbsFloat(vector.DistanceSq(ladderHandlePointWS, m_Player.GetPosition()));
	
								for (int k = 1; k < memSelections[i3].GetVertexCount(); ++k)
								{
									ladderHandlePointLS = memSelections[i3].GetVertexPosition(lod, k);
									ladderHandlePointWS = object.ModelToWorld(ladderHandlePointLS);
									
									if (lastDistance > Math.AbsFloat(vector.DistanceSq(ladderHandlePointWS, m_Player.GetPosition())))
									{
										lastDistance = Math.AbsFloat(vector.DistanceSq(ladderHandlePointWS, m_Player.GetPosition()));
										closestHandlePos = ladderHandlePointWS;
									}
								}

								m_FixedOnPosition = false;
								worldPos = closestHandlePos;						
								if (memOffset != 0.0)
								{
									worldPos[1] = worldPos[1] + memOffset;
								}
								else
								{
									worldPos[1] = worldPos[1] + DEFAULT_HANDLE_OFFSET;
								}
							}
						}

						//! cache current object and the widget world pos
						m_CachedObject.Store(object, worldPos, -1); //! do not store component index for ladders
					}
					else
					{
						m_FixedOnPosition = true;
					}
				}
				else
				{
					m_FixedOnPosition = true;
				}
			}
			else
			{
				m_FixedOnPosition = true;
			}

			vector pos = TransformToScreenPos(worldPos);

			x = pos[0];
			y = pos[1];
		}

		worldPos = vector.Zero;
		isTargetForced = false;
	}

	// kept for backward compatibility
	protected bool IsComponentInSelection(array<Selection> selection, string compName)
	{
		return MiscGameplayFunctions.IsComponentInSelection(selection, compName);
	}

	// getters

 	protected void GetPlayer()
	{
		Class.CastTo(m_Player, GetGame().GetPlayer());
	}

	protected void GetActionManager()
	{
		if (m_Player && m_Player.IsPlayerSelected())
		{
			Class.CastTo(m_AM, m_Player.GetActionManager());
		}
		else
		{
			m_AM = null;
		}
	}

	//! get actions from Action Manager
  	protected void GetActions()
	{
		m_Interact = null;
		m_ContinuousInteract = null;
		m_Single = null;
		m_Continuous = null;

		if (!m_AM) return;
		if (!m_Target) return;
		if (m_Player.IsSprinting()) return;
		if (m_Player.IsInVehicle()) return; // TODO: TMP: Car AM rework needed

		m_Interact = m_AM.GetPossibleAction(InteractActionInput);
		m_ContinuousInteract = m_AM.GetPossibleAction(ContinuousInteractActionInput);
		m_Single = m_AM.GetPossibleAction(DefaultActionInput);
		m_Continuous = m_AM.GetPossibleAction(ContinuousDefaultActionInput);
		
		m_InteractActionsNum = m_AM.GetPossibleActionCount(InteractActionInput);
		m_ContinuousInteractActionsNum = m_AM.GetPossibleActionCount(ContinuousInteractActionInput);
		m_ItemActionsNum = m_AM.GetPossibleActionCount(DefaultActionInput);
		m_ContinuousItemActionsNum = m_AM.GetPossibleActionCount(ContinuousDefaultActionInput);
		
		m_SelectedActionCategory = m_AM.GetSelectedActionCategory();
		
		if (m_Interact)
		{
			m_DisplayInteractTarget = m_Interact.GetDisplayInteractObject(m_Player, m_Target);
		}
		else if (m_Single)
		{
			m_DisplayInteractTarget = m_Single.GetDisplayInteractObject(m_Player, m_Target);
		}
		else if (m_ContinuousInteract)
		{
			m_DisplayInteractTarget = m_ContinuousInteract.GetDisplayInteractObject(m_Player, m_Target);
		}
		else
		{
			m_DisplayInteractTarget = null;
		}
	}

	protected void GetTarget()
	{
		if (!m_AM)
			return;

		m_Target = m_AM.FindActionTarget();
		if (m_Target && m_Target.GetObject() && m_Target.GetObject().IsItemBase())
		{
			ItemBase item = ItemBase.Cast(m_Target.GetObject());
			InventoryLocation invLocation = new InventoryLocation();
			item.GetInventory().GetCurrentInventoryLocation(invLocation);

			if ((!item.IsTakeable() && !item.IsActionTargetVisible()) || (m_Player && m_Player.IsInVehicle()) || invLocation.GetType() != InventoryLocationType.GROUND)
				m_Hidden = true;
		}
	}
	
	protected string GetActionDesc(ActionBase action)
	{
		string desc = "";
		if (action && action.GetText())
		{
			desc = action.GetText();
		
			if (desc && action.DisplayTargetInActionText())
				string extraDescription = action.GetTargetName(m_Player, m_Target);
				if (extraDescription)
					desc = string.Format("%1 (%2)", desc, extraDescription);
		}

		return desc;
	}
	
	//Getting NAME of the entity
	protected string GetItemDesc(ActionBase action)
	{
		string desc = "";
		Object tgObject = m_DisplayInteractTarget;
		if (!tgObject && m_Target)
		{
			tgObject = m_Target.GetObject();
		}
		
		if (tgObject)
		{
			//BreakOut if item is ruined
			Object tgParent = m_Target.GetParent();
			if (!tgObject.IsAlive())
			{
				//Fetch parent item name if one is present
				if (!tgParent || tgObject.DisplayNameRuinAttach())
					desc = tgObject.GetDisplayName();
				else
					desc = tgParent.GetDisplayName();
				return desc;
			}
			
			EntityAI targetEntity;
			
			if (tgParent && (tgParent.IsItemBase() || tgParent.IsTransport()))
			{
				targetEntity = EntityAI.Cast(tgParent);
			}
			
			if (tgObject.IsItemBase() || tgObject.IsTransport())
			{
				targetEntity = EntityAI.Cast(tgObject);
			}
			
			if (targetEntity && targetEntity.ShowZonesHealth())
			{
				string zone = "";
				
				array<string> selections = new array<string>();
				
				//NOTE: relevant fire geometry and view geometry selection names MUST match in order to get a valid damage zone
				if (targetEntity.IsInherited(TentBase) && TentBase.Cast(targetEntity).GetState() != TentBase.PACKED)
				{
					//This is really specific to tents, as they use proxies. Hence object must be used
					if (DamageSystem.GetDamageZoneFromComponentName(targetEntity, tgObject.GetActionComponentName(m_Target.GetComponentIndex(), LOD.NAME_FIRE), zone))
					{
						desc = DamageSystem.GetDamageDisplayName(targetEntity, zone);
					}
				}
				else
				{
					targetEntity.GetActionComponentNameList(m_Target.GetComponentIndex(), selections, LOD.NAME_VIEW);
					
					//Important to get display name from component tied to multiple selections
					for (int s = 0; s < selections.Count(); s++)
					{
						if (DamageSystem.GetDamageZoneFromComponentName(targetEntity, selections[s], zone))
						{
							desc = DamageSystem.GetDamageDisplayName(targetEntity, zone);
						}
					}
				}
			}
			
			//Safety check to output something to widget
			if (targetEntity && desc == "")
				desc = targetEntity.GetDisplayName();
		}

		return desc;
	}
	
	protected int GetItemHealth()
	{
		int health = -1;

		if (m_Interact && !m_Interact.HasTarget())
		{
			return health;
		}
		
		Object tgObject = m_DisplayInteractTarget;
		if (!tgObject && m_Target)
		{
			tgObject = m_Target.GetObject();
		}
		
		if (tgObject)
		{
			Object tgParent = m_Target.GetParent();
			EntityAI targetEntity;
			
			//Return specific part health, even if display name is from parent
			if (!tgObject.IsAlive())
			{
				EntityAI ent;
				if (m_TargetItemFrozen && Class.CastTo(ent,tgObject) && (ent.IsAnimal() || ent.IsMan() || ent.IsZombie()))
					health = -2; //frozen interaction later
				else
					health = tgObject.GetHealthLevel();
				
				return health;
			}
			
			if (tgParent && (tgParent.IsItemBase() || tgParent.IsTransport()))
			{
				targetEntity = EntityAI.Cast(tgParent);
			}
			
			if (tgObject.IsItemBase() || tgObject.IsTransport())
			{
				targetEntity = EntityAI.Cast(tgObject);
			}
			
			if (targetEntity)
			{
				if (!targetEntity.IsDamageDestroyed())
				{
					string zone = "";
					array<string> selections = new array<string>();
					
					if (targetEntity.IsInherited(TentBase) && TentBase.Cast(targetEntity).GetState() != TentBase.PACKED)
					{
						//This is really specific to tents, as they use proxies. Hence object must be used
						if (DamageSystem.GetDamageZoneFromComponentName(targetEntity, tgObject.GetActionComponentName(m_Target.GetComponentIndex(), LOD.NAME_FIRE), zone))
						{
							health = targetEntity.GetHealthLevel(zone);
						}
					}
					else
					{
						//NOTE: relevant view geometry and view geometry selection names MUST match in order to get a valid damage zone
						targetEntity.GetActionComponentNameList(m_Target.GetComponentIndex(), selections, LOD.NAME_VIEW);
						
						for (int s = 0; s < selections.Count(); s++)
						{
							if (DamageSystem.GetDamageZoneFromComponentName(targetEntity , selections[s], zone))
							{
								health = targetEntity.GetHealthLevel(zone);
								break;
							}
						}
					}
	
					if (zone == "")
						health = targetEntity.GetHealthLevel();
				}
			}
			else
			{
				health = tgObject.GetHealthLevel();
			}
		}
		
		return health;
	}
	
	protected bool GetItemFrozen()
	{
		Object tgObject = m_DisplayInteractTarget;
		if (!tgObject && m_Target)
			tgObject = m_Target.GetObject();
		
		if (tgObject)
		{
			Object tgParent = m_Target.GetParent();
			EntityAI targetEntity;
			
			if (tgParent)
			{
				targetEntity = EntityAI.Cast(tgParent);
			}
			else
			{
				targetEntity = EntityAI.Cast(tgObject);
			}
			
			return (targetEntity && targetEntity.GetIsFrozen());
		}
		
		return false; 
	}

	protected void GetItemQuantity(out int q_type, out float q_cur, out int q_min, out int q_max)
	{
		InventoryItem item = null;

		if (m_Interact && !m_Interact.HasTarget())
		{
			return;
		}
		
		Object tgObject = m_DisplayInteractTarget;
		if (!tgObject && m_Target)
		{
			tgObject = m_Target.GetObject();
		}
		
		if (Class.CastTo(item, tgObject))
		{
			q_type = QuantityConversions.HasItemQuantity(item);
			if (q_type > 0)
				QuantityConversions.GetItemQuantity(item, q_cur, q_min, q_max);
		}
	}

	//! returns number of items in cargo for targeted entity
	protected void GetItemCargoCount(out int cargoCount)
	{
		EntityAI entity = null;
		
		Object tgObject = m_DisplayInteractTarget;
		if (!tgObject && m_Target)
		{
			tgObject = m_Target.GetObject();
		}

		if (Class.CastTo(entity, tgObject))
		{

			GameInventory inventory = entity.GetInventory();
			if (inventory)
			{
				cargoCount = AttachmentsWithInventoryOrCargoCount(inventory);
				return;
			}

			//! default cargo count
			cargoCount = 0;
		}
	}

	// setters
	protected void SetItemDesc(string descText, int cargoCount, string itemWidget, string descWidget)
	{
		Widget widget;
		widget = m_Root.FindAnyWidget(itemWidget);
		
		//! Last message from finished User Action on target (Thermometer, Blood Test Kit, etc.)
		PlayerBase playerT = PlayerBase.Cast(m_Target.GetObject());
		if (playerT)
			string msg = playerT.GetLastUAMessage();
				
		if (descText.Length() == 0 && msg.Length() == 0)
		{
			widget.Show(false);
			return;
		}
		
		descText.ToUpper();
		TextWidget itemName;
		Class.CastTo(itemName, widget.FindAnyWidget(descWidget));
	
		//! when cargo in container
		if (cargoCount > 0)
			descText = string.Format("[+] %1  %2", descText, msg);
		else
			descText = string.Format("%1  %2", descText, msg);

		itemName.SetText(descText);
		widget.Show(true);
	}
	
	protected void SetItemHealth(int health, string itemWidget, string healthWidget, bool enabled)
	{
		Widget widget = m_Root.FindAnyWidget(itemWidget);
		
		if (enabled)
		{
			ImageWidget healthMark;
			Class.CastTo(healthMark, widget.FindAnyWidget(healthWidget));
			
			if (health == -1)
			{
				healthMark.GetParent().Show(false);
				widget.Show(enabled);
				return;
			}
			
			int color = 0x00FFFFFF;
			if (m_TargetItemFrozen && (health != GameConstants.STATE_RUINED || health == -2))
				color = Colors.COLOR_FROZEN;
			else
				color = ItemManager.GetItemHealthColor(health);
			
			healthMark.SetColor(color);
			healthMark.SetAlpha(0.5);
			healthMark.GetParent().Show(true);
		}
		
		widget.Show(enabled);
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
					}
					else
					{
						progressBar.Show(false);
						textWidget.Show(false);
						textWidget.GetParent().Show(false);
					}
					break;
				case QUANTITY_PROGRESS :
					float qty_num = Math.Round((current / max) * 100);
	
					textWidget.Show(false);
					progressBar.SetCurrent(qty_num);
					progressBar.Show(true);
					textWidget.GetParent().Show(true);
					break;
			}
			widget.Show(true);
		}
		else
			widget.Show(false);	
	}

	protected void SetActionWidget(ActionBase action, string descText, string actionWidget, string descWidget)
	{
		Widget widget = m_Root.FindAnyWidget(actionWidget);
		
		if (action)
		{
			if (action.HasTarget() && m_AM.GetActionState() < 1) // targeted & action not performing
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
				
				int x, y;
				actionName.GetTextSize(x, y);
				if (x > m_MaxWidthChild)
					m_MaxWidthChild = x;
			}
			else
			{
				widget.Show(false);
			}
		}
		else
		{
			widget.Show(false);
		}
	}

	//! shows arrows near the interact action if there are more than one available
	protected void SetMultipleInteractAction(string multiActionsWidget, string multiActionsWidget_NotSelect)
	{
		Widget widget, widget_not_select;

		widget = m_Root.FindAnyWidget(multiActionsWidget);
		widget_not_select = m_Root.FindAnyWidget(multiActionsWidget_NotSelect);

		if (m_InteractActionsNum > 1)
		{
			if (m_SelectedActionCategory == InteractActionInput)
			{
				widget.Show(true);
				widget_not_select.Show(false);
			}
			else
			{
				widget.Show(false);
				widget_not_select.Show(true);
			}	
		}
		else
		{
			widget.Show(false);
			widget_not_select.Show(false);
		}
	}
	
	protected void SetMultipleContinuousInteractAction(string multiActionsWidget, string multiActionsWidget_NotSelect)
	{
		Widget widget, widget_not_select;

		widget = m_Root.FindAnyWidget(multiActionsWidget);
		widget_not_select = m_Root.FindAnyWidget(multiActionsWidget_NotSelect);

		if (m_ContinuousInteractActionsNum > 1)
		{
			if (m_SelectedActionCategory == ContinuousInteractActionInput)
			{
				widget.Show(true);
				widget_not_select.Show(false);
			}
			else
			{
				widget.Show(false);
				widget_not_select.Show(true);
			}
		}
		else
		{
			widget.Show(false);
			widget_not_select.Show(false);
		}
				
	}
	
	protected void SetMultipleItemAction(string multiActionsWidget, string multiActionsWidget_NotSelect)
	{
		Widget widget, widget_not_select;

		widget = m_Root.FindAnyWidget(multiActionsWidget);
		widget_not_select = m_Root.FindAnyWidget(multiActionsWidget_NotSelect);

		if (m_ItemActionsNum > 1)
		{ 
			if (m_SelectedActionCategory == DefaultActionInput)
			{
				widget.Show(true);
				widget_not_select.Show(false);
			}
			else
			{
				widget.Show(false);
				widget_not_select.Show(true);
			}
		}
		else
		{
			widget.Show(false);
			widget_not_select.Show(false);
		}	
	}
	
	protected void SetMultipleContinuousItemAction(string multiActionsWidget, string multiActionsWidget_NotSelect)
	{
		Widget widget, widget_not_select;

		widget = m_Root.FindAnyWidget(multiActionsWidget);
		widget_not_select = m_Root.FindAnyWidget(multiActionsWidget_NotSelect);

		if (m_ContinuousItemActionsNum > 1)
		{
			if (m_SelectedActionCategory == ContinuousDefaultActionInput)
			{
				widget.Show(true);
				widget_not_select.Show(false);
			}
			else
			{
				widget.Show(false);
				widget_not_select.Show(true);
			}
		}
		else
		{
			widget.Show(false);
			widget_not_select.Show(false);
		}		
	}
	
	protected void SetInteractActionIcon(string actionWidget, string actionIconFrameWidget, string actionIconWidget, string actionIconTextWidget)
	{
		Widget widget;
		ImageWidget iconWidget;
		TextWidget textWidget;
		
		widget = m_Root.FindAnyWidget(actionWidget);
		Class.CastTo(iconWidget, widget.FindAnyWidget(actionIconWidget));
		Class.CastTo(textWidget, widget.FindAnyWidget(actionIconTextWidget));
		
		GetDayZGame().GetBacklit().HintShow(m_UseActionWrapper.InputP());
		
		// uses text in floating widget
		iconWidget.Show(false);
		textWidget.SetText(InputUtils.GetButtonNameFromInput("UAAction", EInputDeviceType.MOUSE_AND_KEYBOARD));
		textWidget.Show(true);
	}
	
	protected void CheckRefresherFlagVisibilityEx(ActionTarget target)
	{
		EntityAI entity = EntityAI.Cast(target.GetObject());
		if (!entity)
			entity = EntityAI.Cast(target.GetParent());
		
		if (!entity)
			return;
		
		Widget w = m_Root.FindAnyWidget("item_flag_icon");
		if (w)
			w.Show(entity.IsRefresherSignalingViable() && m_Player.IsTargetInActiveRefresherRange(entity));
	}
	
	protected int AttachmentsWithInventoryOrCargoCount(notnull GameInventory inventory)
	{
		int attachmentsWithInventory = 0;
		
		CargoBase cargo = inventory.GetCargo();
		if (cargo && cargo.GetItemCount() > 0)
		{
			return 1;
		}
		
		for (int i = 0; i < inventory.AttachmentCount(); i++)	
		{
			EntityAI attachment = inventory.GetAttachmentFromIndex(i);
			int attachmentSlotId = attachment.GetInventory().GetSlotId(0);
			
			if (attachment.GetInventory())
			{
				attachmentsWithInventory += 1;
			}
		}
		
		return attachmentsWithInventory;
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
	
	// deprecated 
	protected void CheckRefresherFlagVisibility(Object object);
}