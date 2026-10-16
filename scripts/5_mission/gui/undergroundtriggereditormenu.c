#ifdef DIAG_DEVELOPER
class UndergroundTriggerEditorMenu : UIScriptedMenu
{
	private static const int POPUP_ID_NONE				= -1;
	private static const int POPUP_ID_DIALOG_RELOAD		= 0;
	private static const int POPUP_ID_DIALOG_RENAME		= 1;

	private static const int POPUP_ID_DIALOG_DISCARD_BREADCRUMB	= 2;
	private static const int POPUP_ID_DIALOG_RENAME_BREADCRUMB	= 3;

	private int m_ActivePopup;
	private ref map<int, ref UndergroundTriggerEditorPopupBase>	m_Popups;

	private Widget m_ScenePanel;
	private WrapSpacerWidget m_TriggersListWidget;
	private ScrollWidget m_TriggersListScrollWidget;
	private ButtonWidget m_TriggerRenameButton;
	private ButtonWidget m_TriggerExportButton;
	private ButtonWidget m_TriggerDeleteButton;
	private ButtonWidget m_TriggerReloadButton;
	private ButtonWidget m_TriggerEditBreadcrumbsButton;

	private Widget m_TriggersSection;
	private Widget m_BreadcrumbEditPanel;
	private TextWidget m_BreadcrumbEditHeader;
	private WrapSpacerWidget m_BreadcrumbsListWidget;
	private ScrollWidget m_BreadcrumbsListScrollWidget;
	private ButtonWidget m_BreadcrumbRenameButton;
	private ButtonWidget m_BreadcrumbDeleteButton;
	private ButtonWidget m_BreadcrumbSaveButton;
	private ButtonWidget m_BreadcrumbBackButton;
	private ButtonWidget m_BreadcrumbApplyButton;

	private Widget m_BreadcrumbParamsSection;
	private ButtonWidget m_BreadcrumbParamsHeader;
	private Widget m_BreadcrumbParamsContent;
	private EditBoxWidget m_BreadcrumbParamsEyeAcc;
	private CheckBoxWidget m_BreadcrumbParamsUseRaycast;
	private EditBoxWidget m_BreadcrumbParamsRadius;
	private CheckBoxWidget m_BreadcrumbParamsLightLerp;


	private PluginUndergroundTriggerManager m_UndergroundTriggerManager;

	private ref map<Widget, ref UndergroundTriggerEditorCustomListElement> m_TriggersListElements;
	private ref map<Widget, UndergroundTriggerHandle> m_TriggersList;
	private ref map<Widget, UndergroundBreadcrumbHandle> m_BreadcrumbsList;
	private ref map<Widget, ref UndergroundTriggerEditorCustomListElement> m_BreadcrumbsListElements;

	private bool m_IsInputActive;
	
	// --------------------------------------------------------------------------------

	void UndergroundTriggerEditorMenu()
	{
		m_TriggersListElements 		= new map<Widget, ref UndergroundTriggerEditorCustomListElement>();
		m_TriggersList 				= new map<Widget, UndergroundTriggerHandle>();
		m_BreadcrumbsList 			= new map<Widget, UndergroundBreadcrumbHandle>();
		m_BreadcrumbsListElements 	= new map<Widget, ref UndergroundTriggerEditorCustomListElement>();

		m_UndergroundTriggerManager = PluginUndergroundTriggerManager.Cast(GetPlugin(PluginUndergroundTriggerManager));
		m_UndergroundTriggerManager.OnUIEditorOpened();

		m_ActivePopup = POPUP_ID_NONE;
		m_Popups = new map<int, ref UndergroundTriggerEditorPopupBase>();
		m_Popups.Insert(POPUP_ID_DIALOG_RELOAD, new UndergroundTriggerEditorReloadDialog(layoutRoot));
		m_Popups.Insert(POPUP_ID_DIALOG_RENAME, new UndergroundTriggerEditorRenameDialog(layoutRoot));
		m_Popups.Insert(POPUP_ID_DIALOG_DISCARD_BREADCRUMB, new UndergroundTriggerEditorDiscardBreadcrumbsDialog(layoutRoot));
		m_Popups.Insert(POPUP_ID_DIALOG_RENAME_BREADCRUMB, new UndergroundTriggerEditorRenameBreadcrumbDialog(layoutRoot));

		m_UndergroundTriggerManager.Event_OnUpdateData().Insert(OnUpdateData);
		m_UndergroundTriggerManager.Event_OnSelectTrigger().Insert(OnSelectTrigger);
		m_UndergroundTriggerManager.Event_OnDeleteTrigger().Insert(OnDeleteTrigger);
		m_UndergroundTriggerManager.Event_OnSelectBreadcrumb().Insert(OnSelectBreadcrumb);
		m_UndergroundTriggerManager.Event_OnDeleteBreadcrumb().Insert(OnDeleteBreadcrumb);
		m_UndergroundTriggerManager.Event_OnBreadcrumbEditModeChanged().Insert(OnBreadcrumbEditModeChanged);
	}
	
	// --------------------------------------------------------------------------------

	void ~UndergroundTriggerEditorMenu()
	{
		RemovePopup();

		if (IsModuleExist(PluginUndergroundTriggerManager))
		{
			m_UndergroundTriggerManager.ExitBreadcrumbEditMode(false);

			m_UndergroundTriggerManager.SelectTrigger(null);

			m_UndergroundTriggerManager.Event_OnUpdateData().Remove(OnUpdateData);
			m_UndergroundTriggerManager.Event_OnSelectTrigger().Remove(OnSelectTrigger);
			m_UndergroundTriggerManager.Event_OnDeleteTrigger().Remove(OnDeleteTrigger);
			m_UndergroundTriggerManager.Event_OnSelectBreadcrumb().Remove(OnSelectBreadcrumb);
			m_UndergroundTriggerManager.Event_OnDeleteBreadcrumb().Remove(OnDeleteBreadcrumb);
			m_UndergroundTriggerManager.Event_OnBreadcrumbEditModeChanged().Remove(OnBreadcrumbEditModeChanged);
			m_UndergroundTriggerManager.OnUIEditorClosed();

			m_UndergroundTriggerManager = null;
		}
	}

	// --------------------------------------------------------------------------------

	override bool UseMouse()
	{
		return true;
	}

	// --------------------------------------------------------------------------------

	override bool UseKeyboard()
	{
		return true;
	}

	// --------------------------------------------------------------------------------

	override Widget Init()
	{
		layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/underground_trigger_editor/day_z_underground_trigger_editor.layout");

		m_TriggersListWidget			= WrapSpacerWidget.Cast(layoutRoot.FindAnyWidget("triggers_list"));
		m_TriggersListScrollWidget		= ScrollWidget.Cast(layoutRoot.FindAnyWidget("triggers_list_scroll"));
		m_ScenePanel					= Widget.Cast(layoutRoot.FindAnyWidget("panel_scene"));

		m_TriggerRenameButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("trigger_controls_rename"));
		m_TriggerExportButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("trigger_controls_export"));
		m_TriggerDeleteButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("trigger_controls_delete"));
		m_TriggerReloadButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("trigger_controls_reload"));
		m_TriggerEditBreadcrumbsButton	= ButtonWidget.Cast(layoutRoot.FindAnyWidget("trigger_controls_edit_breadcrumbs"));

		m_TriggersSection				= Widget.Cast(layoutRoot.FindAnyWidget("triggers_section"));
		m_BreadcrumbEditPanel			= Widget.Cast(layoutRoot.FindAnyWidget("breadcrumb_edit_panel"));
		m_BreadcrumbEditHeader			= TextWidget.Cast(layoutRoot.FindAnyWidget("breadcrumb_edit_header"));
		m_BreadcrumbsListWidget			= WrapSpacerWidget.Cast(layoutRoot.FindAnyWidget("breadcrumbs_list"));
		m_BreadcrumbsListScrollWidget	= ScrollWidget.Cast(layoutRoot.FindAnyWidget("breadcrumbs_list_scroll"));
		m_BreadcrumbRenameButton		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("breadcrumb_controls_rename"));
		m_BreadcrumbDeleteButton		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("breadcrumb_controls_delete"));
		m_BreadcrumbSaveButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("breadcrumb_controls_save"));
		m_BreadcrumbBackButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("breadcrumb_controls_back"));

		m_BreadcrumbParamsSection		= Widget.Cast(layoutRoot.FindAnyWidget("breadcrumb_params_section"));
		m_BreadcrumbParamsHeader		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("breadcrumb_params_header"));
		m_BreadcrumbParamsContent		= Widget.Cast(layoutRoot.FindAnyWidget("breadcrumb_params_content"));
		m_BreadcrumbParamsEyeAcc		= EditBoxWidget.Cast(layoutRoot.FindAnyWidget("breadcrumb_params_eye_acc"));
		m_BreadcrumbParamsUseRaycast	= CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("breadcrumb_params_use_raycast"));
		m_BreadcrumbParamsRadius		= EditBoxWidget.Cast(layoutRoot.FindAnyWidget("breadcrumb_params_radius"));
		m_BreadcrumbParamsLightLerp		= CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("breadcrumb_params_light_lerp"));
		m_BreadcrumbApplyButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("breadcrumb_params_controls_apply"));

		UpdateTriggersList();

		GizmoApi gizmoApi = GetGizmoApi();
		GizmoTransformMode currentTransform = gizmoApi.GetTransformMode();
		if (currentTransform == GizmoTransformMode.NONE)
			gizmoApi.SetTransformMode(GizmoTransformMode.MOVE);

		return layoutRoot;
	}

	// --------------------------------------------------------------------------------

	override bool OnDoubleClick( Widget w, int x, int y, int button )
	{
		super.OnDoubleClick(w, x, y, button);

		if (m_UndergroundTriggerManager.IsBreadcrumbEditMode())
			return false;

		UndergroundTriggerHandle handle = m_TriggersList.Get(w);
		if (handle)
		{
			m_UndergroundTriggerManager.SelectTrigger(handle);
			m_UndergroundTriggerManager.FocusOnObject();
			return true;
		}

		return false;
	}

	// --------------------------------------------------------------------------------

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);

		if (m_ActivePopup > POPUP_ID_NONE)
		{
			if (m_Popups.Get(m_ActivePopup).OnClick(w, x, y, button))
				return true;
		}

		// --- Breadcrumb edit mode controls ---

		if (w == m_BreadcrumbSaveButton)
		{
			m_UndergroundTriggerManager.SaveBreadcrumbs();
			return true;
		}

		if (w == m_BreadcrumbBackButton)
		{
			if (m_UndergroundTriggerManager.IsBreadcrumbsDirty())
			{
				m_ActivePopup = POPUP_ID_DIALOG_DISCARD_BREADCRUMB;
				UndergroundTriggerEditorDiscardBreadcrumbsDialog discardDialog = UndergroundTriggerEditorDiscardBreadcrumbsDialog.Cast(m_Popups.Get(m_ActivePopup));
				discardDialog.SetMessage("Discard unsaved breadcrumb changes?");
				discardDialog.Open();
			}
			else
			{
				m_UndergroundTriggerManager.ExitBreadcrumbEditMode(false);
			}
			return true;
		}

		if (w == m_BreadcrumbApplyButton)
		{
			ApplyBreadcrumbParams();
			return true;
		}

		if (w == m_BreadcrumbParamsHeader)
		{
			ShowBreadcrumbParamsSection(!m_BreadcrumbParamsContent.IsVisible());
			return true;
		}

		if (w == m_BreadcrumbDeleteButton)
		{
			UndergroundBreadcrumbHandle breadcrumbHandle = m_UndergroundTriggerManager.GetSelectedBreadcrumb();
			if (breadcrumbHandle)
				m_UndergroundTriggerManager.DeleteBreadcrumb(breadcrumbHandle);
			return true;
		}

		if (w == m_BreadcrumbRenameButton)
		{
			UndergroundBreadcrumbHandle selectedBreadcrumb = m_UndergroundTriggerManager.GetSelectedBreadcrumb();
			if (selectedBreadcrumb)
			{
				m_ActivePopup = POPUP_ID_DIALOG_RENAME_BREADCRUMB;
				UndergroundTriggerEditorRenameBreadcrumbDialog renameDialogBreadcrumb = UndergroundTriggerEditorRenameBreadcrumbDialog.Cast(m_Popups.Get(m_ActivePopup));
				renameDialogBreadcrumb.SetMessage("Rename breadcrumb");
				renameDialogBreadcrumb.SetName(selectedBreadcrumb.GetHandleName());
				renameDialogBreadcrumb.Open();
			}
			return true;
		}

		UndergroundBreadcrumbHandle breadcrumbListHandle = m_BreadcrumbsList.Get(w);
		if (breadcrumbListHandle)
		{
			m_UndergroundTriggerManager.SelectBreadcrumb(breadcrumbListHandle);
			return true;
		}

		// Block trigger interaction while in breadcrumb edit mode
		if (m_UndergroundTriggerManager.IsBreadcrumbEditMode())
			return false;

		// --- Normal trigger controls ---

		UndergroundTriggerHandle triggerHandle = m_TriggersList.Get(w);
		if (w == m_TriggerDeleteButton)
		{
			triggerHandle = m_UndergroundTriggerManager.GetSelectedTrigger();
			if (triggerHandle)
				m_UndergroundTriggerManager.DeleteTrigger(triggerHandle);

			return true;
		}

		if (triggerHandle)
		{
			m_UndergroundTriggerManager.SelectTrigger(triggerHandle);
			return true;
		}

		if (w == m_TriggerEditBreadcrumbsButton)
		{
			m_UndergroundTriggerManager.EnterBreadcrumbEditMode();
			return true;
		}

		if (w == m_TriggerExportButton)
		{
			m_UndergroundTriggerManager.ExportData();
			return true;
		}

		if (w == m_TriggerReloadButton)
		{
			m_ActivePopup = POPUP_ID_DIALOG_RELOAD;
			UndergroundTriggerEditorReloadDialog reloadDialog = UndergroundTriggerEditorReloadDialog.Cast(m_Popups.Get(m_ActivePopup));
			reloadDialog.SetMessage("Unexported changes will be lost after reload from file.");
			reloadDialog.Open();
			return true;
		}

		if (w == m_TriggerRenameButton)
		{
			triggerHandle = m_UndergroundTriggerManager.GetSelectedTrigger();
			if (triggerHandle)
			{
				m_ActivePopup = POPUP_ID_DIALOG_RENAME;
				UndergroundTriggerEditorRenameDialog renameDialog = UndergroundTriggerEditorRenameDialog.Cast(m_Popups.Get(m_ActivePopup));
				renameDialog.SetMessage("Rename trigger");
				renameDialog.SetName(triggerHandle.GetHandleName());
				renameDialog.Open();
				return true;
			}
			return true;
		}

		if (w != m_ScenePanel)
		{
			if (m_UndergroundTriggerManager.IsBreadcrumbEditMode())
			{
				if (m_UndergroundTriggerManager.GetSelectedBreadcrumb())
					m_UndergroundTriggerManager.SelectBreadcrumb(null);
			}
			else
			{
				if (m_UndergroundTriggerManager.GetSelectedTrigger())
					m_UndergroundTriggerManager.SelectTrigger(null);
			}
		}

		return false;
	}

	// --------------------------------------------------------------------------------

	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_F)
		{
			UndergroundTriggerHandle handle = m_UndergroundTriggerManager.GetSelectedTrigger();
			if (handle)
			{
				m_UndergroundTriggerManager.FocusOnObject();
				return true;
			}
		}

		return false;
	}

	// --------------------------------------------------------------------------------

	override void OnShow()
	{
		if (!m_IsInputActive)
		{
			g_Game.GetMission().AddActiveInputExcludes({"menu"});
			g_Game.GetInput().ChangeGameFocus(1);
			m_IsInputActive = true;
		}
		g_Game.GetUIManager().ShowUICursor(true);
	}

	// --------------------------------------------------------------------------------

	override void OnHide()
	{
		if (m_IsInputActive)
		{
			if (g_Game.GetMission())
				g_Game.GetMission().RemoveActiveInputExcludes({"menu"}, true);

			g_Game.GetInput().ResetGameFocus(-1);
			m_IsInputActive = false;
		}
		g_Game.GetUIManager().ShowUICursor(false);
	}

	// --------------------------------------------------------------------------------

	private void OnUpdateData()
	{
		UpdateTriggersList();
		UpdateTriggersDirtyIndicator();
		if (m_UndergroundTriggerManager.IsBreadcrumbEditMode())
		{
			UpdateBreadcrumbsList();
			UpdateBreadcrumbDirtyIndicator();
		}
	}

	// --------------------------------------------------------------------------------

	private void OnSelectTrigger(UndergroundTriggerHandle handle)
	{
		UpdateTriggersList();

		foreach (Widget buttonWidget, UndergroundTriggerEditorCustomListElement element: m_TriggersListElements)
			element.Deselect();

		if (handle)
		{
			Widget key = Widget.Cast(m_TriggersList.GetKeyByValue(handle));

			array<UndergroundTriggerHandle> existingTriggerHandles = m_TriggersList.GetValueArray();
			int matchingHandleId = existingTriggerHandles.Find(handle);
			if (matchingHandleId > INDEX_NOT_FOUND)
			{
				m_TriggersListElements[key].Select();
				UpdateBreadcrumbsList();
			}

			// TODO@RR - does not work - needs to be rewritten to positional jump
			m_TriggersListScrollWidget.VScrollToWidget(key);
		}

		m_TriggersListScrollWidget.Update();

		UpdateEditBreadcrumbsButtonState();
	}

	// --------------------------------------------------------------------------------

	private void OnDeleteTrigger(UndergroundTriggerHandle handle)
	{
		Widget key = Widget.Cast(m_TriggersList.GetKeyByValue(handle));
		if (key)
		{
			auto element = UndergroundTriggerEditorCustomListElement.Cast(m_TriggersListElements[key]);
			m_TriggersListWidget.RemoveChild(element.GetRoot());
			m_TriggersListElements.Remove(key);
			m_TriggersList.Remove(key);

			m_TriggersListWidget.Update();
			m_TriggersListScrollWidget.Update();
		}

		UpdateEditBreadcrumbsButtonState();
	}

	// --------------------------------------------------------------------------------

	private void UpdateTriggersList()
	{
		array<UndergroundTriggerHandle> existingTriggerHandles = m_TriggersList.GetValueArray();
		array<UndergroundTriggerHandle> triggerHandles = m_UndergroundTriggerManager.GetTriggersList();

		foreach (auto triggerHandle : triggerHandles)
		{
			if (existingTriggerHandles.Find(triggerHandle) > INDEX_NOT_FOUND)
				continue;

			UndergroundTriggerEditorCustomListElement element = new UndergroundTriggerEditorCustomListElement(m_TriggersListWidget, triggerHandle.GetHandleName());
			ButtonWidget buttonWidget = element.GetButton();
			m_TriggersListWidget.AddChild(element.GetRoot());
			m_TriggersListElements.Insert(buttonWidget, element);
			m_TriggersList.Insert(buttonWidget, triggerHandle);
		}

		m_TriggersListWidget.Update();
		m_TriggersListScrollWidget.Update();
	}

	// --------------------------------------------------------------------------------

	private void OnSelectBreadcrumb(UndergroundBreadcrumbHandle handle)
	{
		Widget key = Widget.Cast(m_BreadcrumbsList.GetKeyByValue(handle));

		foreach (Widget buttonWidget, UndergroundTriggerEditorCustomListElement element: m_BreadcrumbsListElements)
			element.Deselect();

		if (key)
		{
			m_BreadcrumbsListElements[key].Select();
			m_BreadcrumbsListScrollWidget.VScrollToWidget(key);
			m_BreadcrumbsListScrollWidget.Update();
		}

		if (handle)
		{
			ShowBreadcrumbParamsSection(true);
		}
		else
			ShowBreadcrumbParamsSection(false);
		
		SyncBreadcrumbParamsUI();
	}

	// --------------------------------------------------------------------------------

	private void OnDeleteBreadcrumb(UndergroundBreadcrumbHandle handle)
	{
		Widget key = Widget.Cast(m_BreadcrumbsList.GetKeyByValue(handle));
		if (key)
		{
			auto element = UndergroundTriggerEditorCustomListElement.Cast(m_BreadcrumbsListElements[key]);
			m_BreadcrumbsListWidget.RemoveChild(element.GetRoot());
			m_BreadcrumbsListElements.Remove(key);
			m_BreadcrumbsList.Remove(key);
			
			ShowBreadcrumbParamsSection(false);

			m_BreadcrumbsListWidget.Update();
			m_BreadcrumbsListScrollWidget.Update();
		}
	}

	// --------------------------------------------------------------------------------

	private void UpdateBreadcrumbsList()
	{
		// Clear existing list UI
		Widget child = m_BreadcrumbsListWidget.GetChildren();
		while (child)
		{
			Widget next = child.GetSibling();
			m_BreadcrumbsListWidget.RemoveChild(child);
			child = next;
		}
		m_BreadcrumbsListElements.Clear();
		m_BreadcrumbsList.Clear();

		// Populate from manager's current breadcrumb handles
		array<UndergroundBreadcrumbHandle> breadcrumbHandles = m_UndergroundTriggerManager.GetBreadcrumbsList();
		foreach (UndergroundBreadcrumbHandle breadcrumbHandle : breadcrumbHandles)
		{
			UndergroundTriggerEditorCustomListElement element = new UndergroundTriggerEditorCustomListElement(m_BreadcrumbsListWidget, breadcrumbHandle.GetHandleName());
			ButtonWidget buttonWidget = element.GetButton();
			m_BreadcrumbsListWidget.AddChild(element.GetRoot());
			m_BreadcrumbsListElements.Insert(buttonWidget, element);
			m_BreadcrumbsList.Insert(buttonWidget, breadcrumbHandle);
		}

		// Reapply selection state for the currently selected breadcrumb
		UndergroundBreadcrumbHandle selectedBreadcrumb = m_UndergroundTriggerManager.GetSelectedBreadcrumb();
		if (selectedBreadcrumb)
		{
			Widget selectedKey = Widget.Cast(m_BreadcrumbsList.GetKeyByValue(selectedBreadcrumb));
			if (selectedKey)
				m_BreadcrumbsListElements[selectedKey].Select();
		}

		m_BreadcrumbsListWidget.Update();
		m_BreadcrumbsListScrollWidget.Update();
	}

	// --------------------------------------------------------------------------------

	private void OnBreadcrumbEditModeChanged(bool active)
	{
		m_TriggersSection.Show(!active);
		m_BreadcrumbEditPanel.Show(active);

		if (active)
		{
			UpdateBreadcrumbsList();
			UpdateBreadcrumbDirtyIndicator();
		}
		else
		{
			// Clear breadcrumb list UI on exit
			Widget child = m_BreadcrumbsListWidget.GetChildren();
			while (child)
			{
				m_BreadcrumbsListWidget.RemoveChild(child);
				child = child.GetSibling();
			}
			m_BreadcrumbsListElements.Clear();
			m_BreadcrumbsList.Clear();
		}

		m_BreadcrumbsListWidget.Update();
		m_BreadcrumbsListScrollWidget.Update();
	}

	// --------------------------------------------------------------------------------

	private void UpdateTriggersDirtyIndicator()
	{
		if (m_UndergroundTriggerManager.IsTriggersDirty())
			m_TriggerExportButton.SetText("Export *");
		else
			m_TriggerExportButton.SetText("Export");
	}

	// --------------------------------------------------------------------------------

	private void UpdateBreadcrumbDirtyIndicator()
	{
		bool dirty = m_UndergroundTriggerManager.IsBreadcrumbsDirty();

		if (dirty)
		{
			m_BreadcrumbEditHeader.SetText("Breadcrumbs *");
			m_BreadcrumbEditHeader.SetColor(ARGB(255, 255, 0, 0));
			m_BreadcrumbSaveButton.SetText("Save *");
		}
		else
		{
			m_BreadcrumbEditHeader.SetText("Breadcrumbs");
			m_BreadcrumbEditHeader.SetColor(ARGB(255, 255, 255, 0));
			m_BreadcrumbSaveButton.SetText("Save");
		}
	}

	// --------------------------------------------------------------------------------

	private void ShowBreadcrumbParamsSection(bool show)
	{
		m_BreadcrumbParamsContent.Show(show);
		if (show)
			m_BreadcrumbParamsHeader.SetText("Parameters ^");
		else
			m_BreadcrumbParamsHeader.SetText("Parameters v");
	}

	private void SyncBreadcrumbParamsUI()
	{
		UndergroundBreadcrumbHandle handle = m_UndergroundTriggerManager.GetSelectedBreadcrumb();
		if (!handle || !handle.m_Data)
			return;
		
		m_BreadcrumbParamsEyeAcc.SetText(handle.m_Data.EyeAccommodation.ToString());
		m_BreadcrumbParamsUseRaycast.SetChecked(handle.m_Data.UseRaycast);
		m_BreadcrumbParamsRadius.SetText(handle.m_Data.Radius.ToString());
		m_BreadcrumbParamsLightLerp.SetChecked(handle.m_Data.LightLerp);
	}

	private void ApplyBreadcrumbParams()
	{
		UndergroundBreadcrumbHandle handle = m_UndergroundTriggerManager.GetSelectedBreadcrumb();
		if (!handle || !handle.m_Data)
			return;

		string eyeAccText = m_BreadcrumbParamsEyeAcc.GetText();
		if (eyeAccText.Length() > 0)
		{
			handle.m_Data.EyeAccommodation = Math.Clamp(eyeAccText.ToFloat(), 0.0, 1.0);
			m_BreadcrumbParamsEyeAcc.Update();
		}

		handle.m_Data.UseRaycast = m_BreadcrumbParamsUseRaycast.IsChecked();

		string radiusText = m_BreadcrumbParamsRadius.GetText();
		if (radiusText.Length() > 0)
		{
			handle.m_Data.Radius = Math.Clamp(radiusText.ToFloat(), -1.0, 100.0);
			m_BreadcrumbParamsRadius.Update();
		}

		handle.m_Data.LightLerp = m_BreadcrumbParamsLightLerp.IsChecked();
		
		SyncBreadcrumbParamsUI();

		m_UndergroundTriggerManager.MarkBreadcrumbsDirty();
	}

	void DiscardBreadcrumbs()
	{
		m_UndergroundTriggerManager.ExitBreadcrumbEditMode(false);
	}

	// --------------------------------------------------------------------------------

	private void UpdateEditBreadcrumbsButtonState()
	{
		bool hasSelection = (m_UndergroundTriggerManager.GetSelectedTrigger() != null);
		m_TriggerEditBreadcrumbsButton.Enable(hasSelection);
	}

	// --------------------------------------------------------------------------------

	void OnPopupClose()
	{
		RemovePopup();
	}

	// --------------------------------------------------------------------------------

	void RemovePopup()
	{
		if (m_ActivePopup > POPUP_ID_NONE)
		{
			m_Popups.Get(m_ActivePopup).Close();
			m_ActivePopup = POPUP_ID_NONE;
		}
	}

	// --------------------------------------------------------------------------------

	void ReloadDataFromFile()
	{
		m_UndergroundTriggerManager.DeleteAllTrigers();
		m_UndergroundTriggerManager.DeleteAllBreadcrumbs();

		Widget child = m_TriggersListWidget.GetChildren();
		while (child)
		{
			m_TriggersListWidget.RemoveChild(child);
			child = child.GetSibling();
		}

		m_TriggersListElements.Clear();
		m_TriggersList.Clear();

		m_BreadcrumbsList.Clear();
		m_BreadcrumbsListElements.Clear();

		m_UndergroundTriggerManager.LoadDataFromFile();
		UpdateTriggersList();
		UpdateBreadcrumbsList();
	}

	void RenameTrigger(string name)
	{
		UndergroundTriggerHandle handle = m_UndergroundTriggerManager.GetSelectedTrigger();
		handle.SetHandleName(name);
		m_UndergroundTriggerManager.MarkTriggersDirty();
		OnDeleteTrigger(handle);
		UpdateTriggersList();
		UpdateBreadcrumbsList();
	}

	void RenameBreadcrumb(string name)
	{
		UndergroundBreadcrumbHandle handle = m_UndergroundTriggerManager.GetSelectedBreadcrumb();
		if (!handle)
			return;

		handle.SetHandleName(name);
		m_UndergroundTriggerManager.MarkBreadcrumbsDirty();
		UpdateBreadcrumbsList();
	}

	// --------------------------------------------------------------------------------
}


class UndergroundTriggerEditorCustomListElement : ScriptedWidgetEventHandler
{
	private bool m_IsSelected;

	protected Widget m_Root;

	protected ButtonWidget m_ContextButton;
	protected TextWidget m_ButtonText;

	void UndergroundTriggerEditorCustomListElement(Widget parent, string name)
	{
		m_Root = g_Game.GetWorkspace().CreateWidgets("gui/layouts/underground_trigger_editor/day_z_underground_trigger_editor_list_item.layout", parent);
		m_ContextButton = ButtonWidget.Cast(m_Root.FindAnyWidget("button"));
		m_ButtonText = TextWidget.Cast(m_Root.FindAnyWidget("text"));
		m_ButtonText.SetText(name);

		m_Root.SetHandler(this);
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	ButtonWidget GetButton()
	{
		return m_ContextButton;
	}

	void Select()
	{
		m_ButtonText.SetColor(ARGB(255, 255, 0, 0));
		m_IsSelected = true;
	}

	void Deselect()
	{
		m_ButtonText.SetColor(ARGB(255, 255, 255, 255));
		m_IsSelected = false;
	}
}

class UndergroundTriggerEditorPopupBase : ScriptedWidgetEventHandler
{
	protected Widget m_Root;

	protected ref UndergroundTriggerEditorMenu m_Menu;

	void UndergroundTriggerEditorPopupBase()
	{
	}

	void Open()
	{
		m_Menu = UndergroundTriggerEditorMenu.Cast(g_Game.GetUIManager().GetMenu());
		m_Root.Show(true);
	}

	void Close()
	{
		m_Root.Show(false);
	}

	protected void OnPopupClose()
	{
		UndergroundTriggerEditorMenu menu = UndergroundTriggerEditorMenu.Cast(g_Game.GetUIManager().GetMenu());
		menu.OnPopupClose();
	}
}

class UndergroundTriggerEditorDialog : UndergroundTriggerEditorPopupBase
{
	protected ButtonWidget m_OKButton;
	protected ButtonWidget m_CancelButton;
	protected RichTextWidget m_Message;

	void UndergroundTriggerEditorDialog(Widget parent)
	{
		m_Root = g_Game.GetWorkspace().CreateWidgets("gui/layouts/underground_trigger_editor/day_z_underground_trigger_editor_popup_dialog.layout", parent);
		m_Root.Show(false);

		m_OKButton 		= ButtonWidget.Cast(m_Root.FindAnyWidget("button_ok"));
		m_CancelButton 	= ButtonWidget.Cast(m_Root.FindAnyWidget("button_cancel"));
		m_Message 		= RichTextWidget.Cast(m_Root.FindAnyWidget("message"));

		m_Root.SetHandler(this);
	}

	void SetMessage(string message)
	{
		m_Message.SetText(message);
	}
}

class UndergroundTriggerEditorReloadDialog : UndergroundTriggerEditorDialog
{
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_OKButton)
		{
			Close();
			m_Menu.ReloadDataFromFile();
			OnPopupClose();
			return true;
		}
		else if (w == m_CancelButton)
		{
			Close();
			OnPopupClose();
			return true;
		}

		return super.OnClick(w, x, y, button);
	}
}

class UndergroundTriggerEditorDiscardBreadcrumbsDialog : UndergroundTriggerEditorDialog
{
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_OKButton)
		{
			Close();
			m_Menu.DiscardBreadcrumbs();
			OnPopupClose();
			return true;
		}
		else if (w == m_CancelButton)
		{
			Close();
			OnPopupClose();
			return true;
		}

		return super.OnClick(w, x, y, button);
	}
}

class UndergroundTriggerEditorRenameDialog : UndergroundTriggerEditorDialog
{
	private EditBoxWidget m_Name;

	void UndergroundTriggerEditorRenameDialog(Widget parent)
	{
		m_Root = g_Game.GetWorkspace().CreateWidgets("gui/layouts/underground_trigger_editor/day_z_underground_trigger_editor_popup_dialog_input.layout", parent);
		m_Root.Show(false);

		m_OKButton 		= ButtonWidget.Cast(m_Root.FindAnyWidget("button_ok"));
		m_CancelButton 	= ButtonWidget.Cast(m_Root.FindAnyWidget("button_cancel"));
		m_Message 		= RichTextWidget.Cast(m_Root.FindAnyWidget("message"));
		m_Name 			= EditBoxWidget.Cast(m_Root.FindAnyWidget("name"));

		m_Root.SetHandler(this);
	}

	void SetName(string name)
	{
		m_Name.SetText(name);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_OKButton)
		{
			Close();
			m_Menu.RenameTrigger(m_Name.GetText());
			OnPopupClose();
			return true;
		}
		else if (w == m_CancelButton)
		{
			Close();
			OnPopupClose();
			return true;
		}

		return super.OnClick(w, x, y, button);
	}
}

class UndergroundTriggerEditorRenameBreadcrumbDialog : UndergroundTriggerEditorDialog
{
	private EditBoxWidget m_Name;

	void UndergroundTriggerEditorRenameBreadcrumbDialog(Widget parent)
	{
		m_Root = g_Game.GetWorkspace().CreateWidgets("gui/layouts/underground_trigger_editor/day_z_underground_trigger_editor_popup_dialog_input.layout", parent);
		m_Root.Show(false);

		m_OKButton 		= ButtonWidget.Cast(m_Root.FindAnyWidget("button_ok"));
		m_CancelButton 	= ButtonWidget.Cast(m_Root.FindAnyWidget("button_cancel"));
		m_Message 		= RichTextWidget.Cast(m_Root.FindAnyWidget("message"));
		m_Name 			= EditBoxWidget.Cast(m_Root.FindAnyWidget("name"));

		m_Root.SetHandler(this);
	}

	void SetName(string name)
	{
		m_Name.SetText(name);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_OKButton)
		{
			Close();
			m_Menu.RenameBreadcrumb(m_Name.GetText());
			OnPopupClose();
			return true;
		}
		else if (w == m_CancelButton)
		{
			Close();
			OnPopupClose();
			return true;
		}

		return super.OnClick(w, x, y, button);
	}
}
#endif
