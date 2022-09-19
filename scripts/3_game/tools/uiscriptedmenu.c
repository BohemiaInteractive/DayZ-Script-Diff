//-----------------------------------------------------------------------------
class UIMenuPanel: Managed
{
	proto native UIMenuPanel GetSubMenu();
	proto native UIMenuPanel GetParentMenu();
	proto native UIMenuPanel GetVisibleMenu();
	proto native void SetSubMenu(UIMenuPanel submenu);
	proto native void SetParentMenu(UIMenuPanel parent);
	proto native bool CanClose();
	proto native bool CanCloseOnEscape();
	//! Create & open menu with specific id (see \ref MenuID) and set this menu as its parent
	proto native UIScriptedMenu EnterScriptedMenu(int id);
	
	proto native void DestroySubmenu();
	proto native bool IsAnyMenuVisible();
	proto native bool IsVisible();
	
	//! If visibility of application is changed. On console it is called when application is suspended or constrained.
	//! @param isVisible indicate if application is visible in foreground
	void OnVisibilityChanged(bool isVisible)
	{
	}
	
	//! Safe way to close window, using this function can even window safely close itself
	proto native void Close();

	bool UseMouse()	{
		#ifdef PLATFORM_CONSOLE
		return GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer();
		#else
		return true;
		#endif
	}

	bool UseKeyboard()	{
		#ifdef PLATFORM_CONSOLE
		return GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer();
		#else
		return true;
		#endif
	}
	
	bool UseGamepad()	{
		return true;
	}

	//! Returns \ref MenuID
	int GetID()	{
		return MENU_UNKNOWN;
	}
	
	//! Refresh request, called from anywhere
	void Refresh()
	{
	}
};

//-----------------------------------------------------------------------------
//! Part of main menu hierarchy to create custom menus from script
class UIScriptedMenu extends UIMenuPanel
{
	int m_id;
	Widget layoutRoot;
	private Widget	m_AnimAlphaWidget;
	private bool	m_AnimAlphaIsIncreasing;
	private float	m_AnimAlphaValue;

	Widget GetLayoutRoot()
	{
		return layoutRoot;
	}
	
	void LockControls()
	{
		if (UseMouse())
		{
			GetGame().GetInput().ChangeGameFocus(1, INPUT_DEVICE_MOUSE);
			GetGame().GetUIManager().ShowUICursor(true);
		}

		if(UseKeyboard())
		{
			GetGame().GetInput().ChangeGameFocus(1, INPUT_DEVICE_KEYBOARD);
		}
		
		if(UseGamepad())
		{
			GetGame().GetInput().ChangeGameFocus(1, INPUT_DEVICE_GAMEPAD);
		}
	}
	
	void UnlockControls()
	{
		if (UseMouse())
		{
			GetGame().GetInput().ChangeGameFocus(-1, INPUT_DEVICE_MOUSE);
		}

		if (GetParentMenu() && GetParentMenu().UseMouse())
		{
			GetGame().GetUIManager().ShowUICursor(true);
		}
		else
		{
			GetGame().GetUIManager().ShowUICursor(false);
		}

		if(UseKeyboard())
		{
			GetGame().GetInput().ChangeGameFocus(-1, INPUT_DEVICE_KEYBOARD);
		}
		
		if(UseGamepad())
		{
			GetGame().GetInput().ChangeGameFocus(-1, INPUT_DEVICE_GAMEPAD);
		}
	}
	
	void UIScriptedMenu() 
	{
		m_id = MENU_UNKNOWN;
	}
	
	void ~UIScriptedMenu()
	{
	}

	//! Sets \ref MenuID
	void SetID(int id) {
		m_id = id;
	}

	//! Returns \ref MenuID
	override int GetID()	{
		return m_id;
	}

	void SetWidgetAnimAlpha( Widget widget )
	{
		m_AnimAlphaValue = 0.3;
		m_AnimAlphaWidget = widget;
	}
	
	//create widgets here and return layout root Widget
	//widgets will be destroyed automatically by c++ side
	Widget Init()
	{
		return NULL;
	}

	void Cleanup()
	{
	}

	//after show
	void OnShow()
	{
		LockControls();	
	}

	//after hide
	void OnHide()
	{
		UnlockControls();
	}

	//! Per frame update, called from engine
	void Update(float timeslice)
	{
		#ifdef PLATFORM_CONSOLE
		if ( m_AnimAlphaWidget )
		{
			float anim_speed = 1.2;
			float anim_value_max = 1.0;
			float anim_value_min = 0.3;
			if ( m_AnimAlphaIsIncreasing )
			{
				m_AnimAlphaValue += anim_speed * timeslice;
				
				if ( m_AnimAlphaValue >= anim_value_max )
				{
					m_AnimAlphaValue = anim_value_max;
					m_AnimAlphaIsIncreasing = false;
				}
			}
			else
			{
				m_AnimAlphaValue -= anim_speed * timeslice;
				
				if ( m_AnimAlphaValue <= anim_value_min )
				{
					m_AnimAlphaValue = anim_value_min;
					m_AnimAlphaIsIncreasing = true;
				}
			}
			
			
			m_AnimAlphaWidget.SetAlpha( m_AnimAlphaValue );
		}
		#endif
	}

	// Moved to parent
	//! Refresh request, called from anywhere
	//void Refresh()
	//{
	//}

	proto native void SetFadingPanels(Widget panel0, Widget panel1, Widget panel2, Widget panel3, Widget panel4);
	
	bool OnClick(Widget w, int x, int y, int button)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnClick( w, x, y, button ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnModalResult(Widget w, int x, int y, int code, int result)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnModalResult( w, x, y, code, result ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnDoubleClick( w, x, y, button ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnSelect(Widget w, int x, int y)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnSelect( w, x, y ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnItemSelected(Widget w, int x, int y, int row, int  column,	int  oldRow, int  oldColumn)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnItemSelected( w, x, y, row, column, oldRow, oldColumn ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnFocus(Widget w, int x, int y)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnFocus( w, x, y ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnFocusLost(Widget w, int x, int y)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnFocusLost( w, x, y ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnMouseEnter(Widget w, int x, int y)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnMouseEnter( w, x, y ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnMouseLeave( w, enterW, x, y ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnMouseButtonDown( w, x, y, button ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnMouseButtonUp( w, x, y, button ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnMouseWheel( w, x, y, wheel ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnController(Widget w, int control, int value)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnController( w, control, value ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnKeyDown( w, x, y, key ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnKeyUp(Widget w, int x, int y, int key)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnKeyUp( w, x, y, key ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnKeyPress( w, x, y, key ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnChange(Widget w, int x, int y, bool finished)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnChange( w, x, y, finished ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnDrag(Widget w, int x, int y)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnDrag( w, x, y ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnDragging(Widget w, int x, int y, Widget reciever)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnDragging( w, x, y, reciever ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnDraggingOver(Widget w, int x, int y, Widget reciever)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnDraggingOver( w, x, y, reciever ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnDrop(Widget w, int x, int y, Widget reciever)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnDrop( w, x, y, reciever ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	bool OnDropReceived(Widget w, int x, int y, Widget reciever)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnDropReceived( w, x, y, reciever ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}

	bool OnEvent(EventType eventType, Widget target, int parameter0, int parameter1)
	{
		if ( UIScriptedWindow.GetActiveWindows() )
		{
			for ( int i = 0; i < UIScriptedWindow.GetActiveWindows().Count(); i++ )
			{
				if ( UIScriptedWindow.GetActiveWindows().GetElement( i ).OnEvent( eventType, target, parameter0, parameter1 ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}

	ContextMenu GetContextMenu()
	{
		return NULL;
	}
	
	bool OnXboxEvent(int xboxEvent)
	{
		return true;
	}
	
	void OnRPC(ParamsReadContext ctx){}
	void OnRPCEx(int rpc_type, ParamsReadContext ctx){}
	
	void InitNoteWrite(EntityAI paper, EntityAI pen, string text = "") {}
	void InitNoteRead(string text = "") {}
	void InitMapItem(EntityAI item) {}
	void LoadMapMarkers() {}
};
