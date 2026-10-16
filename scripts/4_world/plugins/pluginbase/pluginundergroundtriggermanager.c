#ifdef DIAG_DEVELOPER
class PluginUndergroundTriggerManager : PluginBase
{
	private static PluginUndergroundTriggerManager m_Instance;

	private static const float RAYCAST_LENGTH 	= 1000.0;
	private static const string DATETIME_FORMAT = "%1%2%3T%4%5%6Z";
	private static const float SORT_RESORT_THRESHOLD = 10.0;
	private static const int MAX_DEBUG_DRAW_COUNT = 100;
	private static const float MAX_DEBUG_DRAW_DISTANCE = 300.0;

	private bool m_IsOpen;

	private vector m_LastSortCameraPosition;

	private bool m_BreadcrumbEditMode;
	private int m_BreadcrumbEditOriginalCount;
	private bool m_BreadcrumbsDirty;
	private bool m_TriggersDirty;

	private ref array<UndergroundTriggerHandle> m_TriggerHandles;
	private ref array<UndergroundBreadcrumbHandle> m_BreadcrumbHandles;

	private ref ArrayBoxDraw m_TriggerVisuals;
	private ref BoxDraw m_TriggerVisualsSelected;

	private ref ArrayBreadcrumbDraw m_BreadcrumbVisuals;
	private ref BreadcrumbDraw m_BreadcrumbVisualsSelected;

	private ref ScriptInvoker m_OnUpdateData;

	private ref ScriptInvoker m_OnSelectTrigger;
	private ref ScriptInvoker m_OnDeleteTrigger;

	private ref ScriptInvoker m_OnSelectBreadcrumb;
	private ref ScriptInvoker m_OnDeleteBreadcrumb;

	private ref ScriptInvoker m_OnBreadcrumbEditModeChanged;

	void PluginUndergroundTriggerManager()
	{
		if (m_Instance == null)
			m_Instance = this;
	}
	
	// --------------------------------------------------------------------------------

	static PluginUndergroundTriggerManager GetInstance()
	{
		return PluginUndergroundTriggerManager.Cast(GetPlugin(PluginUndergroundTriggerManager));
	}

	// --------------------------------------------------------------------------------

	ScriptInvoker Event_OnUpdateData()
	{
		if (!m_OnUpdateData)
			m_OnUpdateData = new ScriptInvoker();

		return m_OnUpdateData;
	}

	// --------------------------------------------------------------------------------

	ScriptInvoker Event_OnSelectTrigger()
	{
		if (!m_OnSelectTrigger)
			m_OnSelectTrigger = new ScriptInvoker();

		return m_OnSelectTrigger;
	}
	
	// --------------------------------------------------------------------------------

	ScriptInvoker Event_OnDeleteTrigger()
	{
		if (!m_OnDeleteTrigger)
			m_OnDeleteTrigger = new ScriptInvoker();

		return m_OnDeleteTrigger;
	}
	
	// --------------------------------------------------------------------------------

	ScriptInvoker Event_OnSelectBreadcrumb()
	{
		if (!m_OnSelectBreadcrumb)
			m_OnSelectBreadcrumb = new ScriptInvoker();

		return m_OnSelectBreadcrumb;
	}
	
	// --------------------------------------------------------------------------------

	ScriptInvoker Event_OnDeleteBreadcrumb()
	{
		if (!m_OnDeleteBreadcrumb)
			m_OnDeleteBreadcrumb = new ScriptInvoker();

		return m_OnDeleteBreadcrumb;
	}
	
	// --------------------------------------------------------------------------------

	ScriptInvoker Event_OnBreadcrumbEditModeChanged()
	{
		if (!m_OnBreadcrumbEditModeChanged)
			m_OnBreadcrumbEditModeChanged = new ScriptInvoker();

		return m_OnBreadcrumbEditModeChanged;
	}

	// --------------------------------------------------------------------------------

	override void OnUpdate(float delta_time)
	{
		if (m_IsOpen)
		{
			vector cameraPos = FreeDebugCamera.GetInstance().GetPosition();
			
			if (vector.Distance(cameraPos, m_LastSortCameraPosition) > SORT_RESORT_THRESHOLD)
			{
				m_LastSortCameraPosition = cameraPos;
				
				if (m_TriggerVisuals.Count() > 1)
					SortBoxDrawsByDistance(m_TriggerVisuals);
				
				if (m_BreadcrumbVisuals.Count() > 1)
					SortBreadcrumbDrawsByDistance(m_BreadcrumbVisuals);
			}
			
			UpdateTriggers(cameraPos);
			UpdateBreadcrumbs(cameraPos);
		}
	}
	
	private void UpdateTriggers(vector cameraPos)
	{
		int limit = Math.Min(m_TriggerVisuals.Count(), MAX_DEBUG_DRAW_COUNT);
		bool selectedWasDrawn = false;
		
		for (int i = 0; i < limit; ++i)
		{
			BoxDraw triggerVisual = m_TriggerVisuals.Get(i);
			
			if (vector.Distance(cameraPos, triggerVisual.GetCenter()) > MAX_DEBUG_DRAW_DISTANCE)
				break;
			
			if (triggerVisual == m_TriggerVisualsSelected)
				selectedWasDrawn = true;
			
			triggerVisual.SetAllowTransform(triggerVisual == m_TriggerVisualsSelected && !m_BreadcrumbEditMode);
			triggerVisual.Update();
		}
		
		if (m_TriggerVisualsSelected && !selectedWasDrawn)
		{
			m_TriggerVisualsSelected.SetAllowTransform(!m_BreadcrumbEditMode);
			m_TriggerVisualsSelected.Update();
		}
	}
	
	private void UpdateBreadcrumbs(vector cameraPos)
	{
		int limit = Math.Min(m_BreadcrumbVisuals.Count(), MAX_DEBUG_DRAW_COUNT);
		bool selectedWasDrawn = false;
		
		for (int i = 0; i < limit; ++i)
		{
			BreadcrumbDraw breadCrumbVisual = m_BreadcrumbVisuals.Get(i);
			
			if (vector.Distance(cameraPos, breadCrumbVisual.GetCenter()) > MAX_DEBUG_DRAW_DISTANCE)
				break;
			
			if (breadCrumbVisual == m_BreadcrumbVisualsSelected)
				selectedWasDrawn = true;
			
			if (m_BreadcrumbEditMode)
				breadCrumbVisual.SetAllowTransform(breadCrumbVisual == m_BreadcrumbVisualsSelected);
			else
				breadCrumbVisual.SetAllowTransform(false);
			breadCrumbVisual.Update();
		}
		
		if (m_BreadcrumbVisualsSelected && !selectedWasDrawn)
		{
			m_BreadcrumbVisualsSelected.SetAllowTransform(true);
			m_BreadcrumbVisualsSelected.Update();
		}
	}
	
	// --------------------------------------------------------------------------------

	override void OnInit()
	{
		m_TriggerHandles 	= new array<UndergroundTriggerHandle>();
		m_BreadcrumbHandles = new array<UndergroundBreadcrumbHandle>();
		m_TriggerVisuals 	= new ArrayBoxDraw();
		m_BreadcrumbVisuals = new ArrayBreadcrumbDraw();

		LoadDataFromFile();
	}
	
	// --------------------------------------------------------------------------------

	void OnMouseWheel(int wheel)
	{
		if (!IsMouseInScene())
			return;

		Event_OnUpdateData().Invoke();
	}
	
	// --------------------------------------------------------------------------------

	void OnUIEditorOpened()
	{
		m_IsOpen = true;

		//OnInit(); // leaves the data untouched during runtime
		if (!DeveloperFreeCamera.IsFreeCameraEnabled())
			DeveloperFreeCamera.EnableFreeCameraSceneEditor(PlayerBase.Cast(g_Game.GetPlayer()));

		DiagMenu.SetEngineValue(EDiagMenuGame.ENF_DIAG_GAME_FREECAM_NOCLIP, true);
		Event_OnUpdateData().Invoke();
	}
	
	// --------------------------------------------------------------------------------

	void OnUIEditorClosed()
	{
		m_IsOpen = false;

		DeveloperFreeCamera.FreeCameraToggle(PlayerBase.Cast(g_Game.GetPlayer()));
		DiagMenu.SetEngineValue(EDiagMenuGame.ENF_DIAG_GAME_FREECAM_NOCLIP, false);
		Event_OnUpdateData().Invoke();
		m_TriggerVisuals.Update();
		m_BreadcrumbVisuals.Update();
	}

	// --------------------------------------------------------------------------------

	void Event_OnClick()
	{
		if (!IsMouseInScene() || !m_IsOpen)
			return;

		vector dir = g_Game.GetPointerDirection();
		vector from = FreeDebugCamera.GetInstance().GetPosition();
		vector to = from + (dir * RAYCAST_LENGTH);

		RaycastRVParams params = new RaycastRVParams(from, to, null);
		params.type = ObjIntersectGeom;

		array<ref RaycastRVResult> results = new array<ref RaycastRVResult>();
		array<Object> excluded = new array<Object>();
		if (DayZPhysics.RaycastRVProxy(params, results, excluded))
		{
			if (results.Count() == 0)
				return;

			UndergroundTriggerHandle triggerHandle;
			UndergroundBreadcrumbHandle breadcrumbHandle;
			if (Class.CastTo(triggerHandle, results[0].obj))
			{
				if (!m_BreadcrumbEditMode)
					SelectTrigger(triggerHandle);
			}
			else if (Class.CastTo(breadcrumbHandle, results[0].obj))
			{
				if (m_BreadcrumbEditMode)
					SelectBreadcrumb(breadcrumbHandle);
			}
			else
			{
				if (!m_BreadcrumbEditMode)
				{
					SelectTrigger(null);
					SelectBreadcrumb(null);
				}
				else
				{
					SelectBreadcrumb(null);
				}
			}
		}
		else
		{
			if (!m_BreadcrumbEditMode)
			{
				SelectTrigger(null);
				SelectBreadcrumb(null);
			}
			else
			{
				SelectBreadcrumb(null);
			}
		}

		Event_OnUpdateData().Invoke();
	}

	// --------------------------------------------------------------------------------

	void Event_OnDoubleClick()
	{
		if (!IsMouseInScene() || !m_IsOpen)
			return;

		vector dir = g_Game.GetPointerDirection();
		vector from = FreeDebugCamera.GetInstance().GetPosition();
		vector to = from + (dir * RAYCAST_LENGTH);

		RaycastRVParams params = new RaycastRVParams(from, to, null);
		params.type = ObjIntersectGeom;

		array<ref RaycastRVResult> results = new array<ref RaycastRVResult>();
		array<Object> excluded = new array<Object>();
		if (DayZPhysics.RaycastRVProxy(params, results, excluded))
		{
			if (m_BreadcrumbEditMode)
			{
				AddBreadcrumb(results[0].pos);
			}
			else
			{
				BoxDraw box = new BoxDraw(results[0].pos);
				m_TriggerVisuals.Insert(box);
				UndergroundTriggerHandle handle = box.GetTriggerHandle();
				handle.SetHandleName(string.Format("trigger-%1", g_Game.GetTime()));
				box.Event_OnTransformChanged().Insert(MarkTriggersDirtyOnTransform);
				m_TriggerHandles.Insert(handle);

				MarkTriggersDirty();
				SelectTrigger(handle);
			}
		}

		Event_OnUpdateData().Invoke();
	}
	
	// --------------------------------------------------------------------------------

	array<UndergroundTriggerHandle> GetTriggersList()
	{
		return m_TriggerHandles;
	}

	// --------------------------------------------------------------------------------

	array<UndergroundBreadcrumbHandle> GetBreadcrumbsList()
	{
		return m_BreadcrumbHandles;
	}
	
	// --------------------------------------------------------------------------------

	void EditorToggle()
	{
		if (g_Game.IsMultiplayer())
			return;

		if (IsOpened())
			g_Game.GetUIManager().Back();
		else
		{
			g_Game.GetUIManager().EnterScriptedMenu(MENU_UNDERGROUND_TRIGGER_EDITOR, null);
			Event_OnUpdateData().Invoke();
		}
	}

	// --------------------------------------------------------------------------------

	bool IsOpened()
	{
		return m_IsOpen;
	}
	
	// --------------------------------------------------------------------------------

	bool IsBreadcrumbEditMode()
	{
		return m_BreadcrumbEditMode;
	}

	// --------------------------------------------------------------------------------

	bool IsBreadcrumbsDirty()
	{
		return m_BreadcrumbsDirty;
	}

	// --------------------------------------------------------------------------------
	
	bool IsTriggersDirty()
	{
		return m_TriggersDirty;
	}
	
	// --------------------------------------------------------------------------------

	// Marks unexported trigger changes; refreshes UI only on the false->true edge
	void MarkTriggersDirty()
	{
		if (m_TriggersDirty)
			return;

		m_TriggersDirty = true;
		Event_OnUpdateData().Invoke();
	}
	
	// --------------------------------------------------------------------------------

	// ScriptInvoker-compatible hook for BoxDraw.Event_OnTransformChanged.
	// Ignores no-op re-emits (gizmo selection / init) that pass an unchanged transform.
	void MarkTriggersDirtyOnTransform(vector oldTransform[4], vector newTransform[4])
	{
		if (IsTransformUnchanged(oldTransform, newTransform))
			return;

		MarkTriggersDirty();
	}
	
	// --------------------------------------------------------------------------------

	// ScriptInvoker-compatible hook for BreadcrumbDraw.Event_OnTransformChanged.
	// Ignores no-op re-emits (gizmo selection / init) that pass an unchanged transform.
	void MarkBreadcrumbsDirtyOnTransform(vector oldTransform[4], vector newTransform[4])
	{
		if (IsTransformUnchanged(oldTransform, newTransform))
			return;

		MarkBreadcrumbsDirty();
	}
	
	// --------------------------------------------------------------------------------

	// Marks unsaved breadcrumb changes; refreshes UI only on the false->true edge
	void MarkBreadcrumbsDirty()
	{
		if (!m_BreadcrumbEditMode || m_BreadcrumbsDirty)
			return;

		m_BreadcrumbsDirty = true;
		Event_OnUpdateData().Invoke();
	}
	
	// --------------------------------------------------------------------------------

	void EnterBreadcrumbEditMode()
	{
		UndergroundTriggerHandle triggerHandle = GetSelectedTrigger();
		if (!triggerHandle)
			return;

		if (!triggerHandle.m_Data.Breadcrumbs)
			triggerHandle.m_Data.Breadcrumbs = new array<ref JsonUndergroundAreaBreadcrumb>();

		m_BreadcrumbEditMode = true;
		m_BreadcrumbEditOriginalCount = triggerHandle.m_Data.Breadcrumbs.Count();
		m_BreadcrumbsDirty = false;

		Event_OnBreadcrumbEditModeChanged().Invoke(true);
		Event_OnUpdateData().Invoke();
	}
	
	// --------------------------------------------------------------------------------

	void SaveBreadcrumbs()
	{
		UndergroundTriggerHandle triggerHandle = GetSelectedTrigger();
		if (!triggerHandle || !triggerHandle.m_Data.Breadcrumbs)
			return;

		// Sync visual positions back to the data model
		foreach (int index, UndergroundBreadcrumbHandle breadcrumbHandle : m_BreadcrumbHandles)
		{
			if (index >= triggerHandle.m_Data.Breadcrumbs.Count())
				break;

			vector pos = breadcrumbHandle.m_Reference.GetCenter();
			JsonUndergroundAreaBreadcrumb breadcrumbData = triggerHandle.m_Data.Breadcrumbs[index];
			breadcrumbData.Position = new TFloatArray();
			breadcrumbData.Position.Insert(pos[0]);
			breadcrumbData.Position.Insert(pos[1]);
			breadcrumbData.Position.Insert(pos[2]);
			
			breadcrumbData.EyeAccommodation = breadcrumbHandle.m_Data.EyeAccommodation;
			breadcrumbData.Radius			= breadcrumbHandle.m_Data.Radius;
			breadcrumbData.LightLerp 		= breadcrumbHandle.m_Data.LightLerp;
			breadcrumbData.UseRaycast 		= breadcrumbHandle.m_Data.UseRaycast;
		}

		// Advance snapshot so Cancel from this point only undoes post-save changes
		m_BreadcrumbEditOriginalCount = triggerHandle.m_Data.Breadcrumbs.Count();
		m_BreadcrumbsDirty = false;
		m_TriggersDirty = true;
		Event_OnUpdateData().Invoke();
	}

	// --------------------------------------------------------------------------------

	void ExitBreadcrumbEditMode(bool save)
	{
		UndergroundTriggerHandle triggerHandle = GetSelectedTrigger();

		m_BreadcrumbEditMode = false;
		m_BreadcrumbsDirty = false;

		if (!save && triggerHandle && triggerHandle.m_Data.Breadcrumbs)
		{
			// Remove breadcrumbs added since last save from the data model
			for (int i = triggerHandle.m_Data.Breadcrumbs.Count() - 1; i >= m_BreadcrumbEditOriginalCount; i--)
				triggerHandle.m_Data.Breadcrumbs.Remove(i);

			// Re-select trigger to reload breadcrumb visuals from the unmodified m_Data
			SelectTrigger(triggerHandle);
		}

		if (m_BreadcrumbVisualsSelected)
		{
			m_BreadcrumbVisualsSelected.SetAllowTransform(false);
			m_BreadcrumbVisualsSelected = null;
		}

		Event_OnBreadcrumbEditModeChanged().Invoke(false);
		Event_OnUpdateData().Invoke();
	}
	
	// --------------------------------------------------------------------------------

	void SelectTrigger(UndergroundTriggerHandle handle)
	{
		if (m_BreadcrumbEditMode)
			return;

		if (m_TriggerVisualsSelected)
		{
			m_TriggerVisualsSelected.SetAllowTransform(false);
			m_TriggerVisualsSelected = null;
			DeleteAllBreadcrumbs();
		}

		GetGizmoApi().DeselectAll();

		if (handle)
		{
			if (m_TriggerHandles.Find(handle) > INDEX_NOT_FOUND)
			{
				m_TriggerVisualsSelected = handle.m_Reference;
				m_TriggerVisualsSelected.SetAllowTransform(true);
				m_TriggerVisualsSelected.Select();

				Event_OnSelectTrigger().Invoke(handle);
				Event_OnUpdateData().Invoke();

				if (handle.m_Data.Breadcrumbs && handle.m_Data.Breadcrumbs.Count() > 0)
				{
					m_BreadcrumbVisuals = new ArrayBreadcrumbDraw();
					foreach (int index, JsonUndergroundAreaBreadcrumb breadcrumbData : handle.m_Data.Breadcrumbs)
					{
						BreadcrumbDraw breadcrumb = new BreadcrumbDraw(breadcrumbData.GetPosition());
						UndergroundBreadcrumbHandle breadcrumbHandle = breadcrumb.GetBreadcrumbHandle();

						handle.m_Reference.Event_OnTransformChanged().Insert(breadcrumbHandle.m_Reference.ApplyTransformation);
						breadcrumb.Event_OnTransformChanged().Insert(MarkBreadcrumbsDirtyOnTransform);

						breadcrumbHandle.m_ParentTrigger = handle;
						if (!breadcrumbData.Comment)
						{
							breadcrumbHandle.SetHandleName(string.Format("Breadcrumb #%1", index));
							m_BreadcrumbHandles.Insert(breadcrumbHandle);
							
							breadcrumbHandle.m_Data.EyeAccommodation = breadcrumbData.EyeAccommodation;
							breadcrumbHandle.m_Data.Radius = breadcrumbData.Radius;
							breadcrumbHandle.m_Data.LightLerp = breadcrumbData.LightLerp;
							breadcrumbHandle.m_Data.UseRaycast = breadcrumbData.UseRaycast;
						}

						m_BreadcrumbVisuals.Insert(breadcrumb);
					}
				}
			}
		}
		else
		{
			Event_OnSelectTrigger().Invoke(null);
			Event_OnUpdateData().Invoke();
		}
	}
	
	// --------------------------------------------------------------------------------

	void SelectBreadcrumb(UndergroundBreadcrumbHandle handle)
	{
		if (m_BreadcrumbVisualsSelected)
		{
			m_BreadcrumbVisualsSelected.SetAllowTransform(false);
			m_BreadcrumbVisualsSelected = null;
		}

		GetGizmoApi().DeselectAll();

		if (handle)
		{
			if (m_BreadcrumbHandles.Find(handle) > INDEX_NOT_FOUND)
			{
				m_BreadcrumbVisualsSelected = handle.m_Reference;
				m_BreadcrumbVisualsSelected.SetAllowTransform(true);
				m_BreadcrumbVisualsSelected.Select();

				Event_OnSelectBreadcrumb().Invoke(handle);
				Event_OnUpdateData().Invoke();
			}
		}
		else
		{
			Event_OnSelectBreadcrumb().Invoke(null);
			Event_OnUpdateData().Invoke();
		}
	}
	
	// --------------------------------------------------------------------------------

	UndergroundTriggerHandle GetSelectedTrigger()
	{
		if (m_TriggerVisualsSelected)
			return m_TriggerVisualsSelected.GetTriggerHandle();

		return null;
	}
	
	// --------------------------------------------------------------------------------

	UndergroundBreadcrumbHandle GetSelectedBreadcrumb()
	{
		if (m_BreadcrumbVisualsSelected)
			return m_BreadcrumbVisualsSelected.GetBreadcrumbHandle();

		return null;
	}
	
	// --------------------------------------------------------------------------------

	void AddBreadcrumb(vector position)
	{
		UndergroundTriggerHandle triggerHandle = GetSelectedTrigger();
		if (!triggerHandle || !triggerHandle.m_Data)
			return;

		if (!triggerHandle.m_Data.Breadcrumbs)
			triggerHandle.m_Data.Breadcrumbs = new array<ref JsonUndergroundAreaBreadcrumb>();

		// Add to data model
		JsonUndergroundAreaBreadcrumb breadcrumbData = new JsonUndergroundAreaBreadcrumb();
		breadcrumbData.Position = new TFloatArray();
		breadcrumbData.Position.Insert(position[0]);
		breadcrumbData.Position.Insert(position[1]);
		breadcrumbData.Position.Insert(position[2]);
		triggerHandle.m_Data.Breadcrumbs.Insert(breadcrumbData);

		// Create visual
		BreadcrumbDraw breadcrumb = new BreadcrumbDraw(position);
		UndergroundBreadcrumbHandle breadcrumbHandle = breadcrumb.GetBreadcrumbHandle();

		triggerHandle.m_Reference.Event_OnTransformChanged().Insert(breadcrumbHandle.m_Reference.ApplyTransformation);
		breadcrumb.Event_OnTransformChanged().Insert(MarkBreadcrumbsDirtyOnTransform);

		breadcrumbHandle.m_ParentTrigger = triggerHandle;
		breadcrumbHandle.SetHandleName(string.Format("Breadcrumb #%1", m_BreadcrumbHandles.Count()));

		m_BreadcrumbHandles.Insert(breadcrumbHandle);
		m_BreadcrumbVisuals.Insert(breadcrumb);

		m_BreadcrumbsDirty = true;

		SelectBreadcrumb(breadcrumbHandle);
		Event_OnUpdateData().Invoke();
	}
	
	// --------------------------------------------------------------------------------

	void DeleteBreadcrumb(UndergroundBreadcrumbHandle handle)
	{
		int index = m_BreadcrumbHandles.Find(handle);
		if (index == INDEX_NOT_FOUND)
			return;

		// Remove from parent trigger data model at the matching index
		UndergroundTriggerHandle triggerHandle = handle.m_ParentTrigger;
		if (triggerHandle && index < triggerHandle.m_Data.Breadcrumbs.Count())
			triggerHandle.m_Data.Breadcrumbs.Remove(index);

		if (m_BreadcrumbVisualsSelected == handle.m_Reference)
		{
			m_BreadcrumbVisualsSelected.SetAllowTransform(false);
			m_BreadcrumbVisualsSelected = null;
		}

		m_BreadcrumbVisuals.RemoveItem(handle.m_Reference);
		m_BreadcrumbHandles.Remove(index);

		if (m_BreadcrumbEditMode)
			m_BreadcrumbsDirty = true;

		Event_OnDeleteBreadcrumb().Invoke(handle);
		Event_OnUpdateData().Invoke();
	}
	
	// --------------------------------------------------------------------------------

	void DeleteTrigger(UndergroundTriggerHandle handle)
	{
		GetGizmoApi().DeselectAll();

		if (handle)
		{
			if (m_TriggerHandles.Find(handle) > INDEX_NOT_FOUND)
			{
				m_TriggerVisualsSelected = null;

				m_TriggerVisuals.RemoveItem(handle.m_Reference);
				m_TriggerHandles.RemoveItem(handle);

				m_TriggersDirty = true;

				Event_OnDeleteTrigger().Invoke(handle);
				DeleteAllBreadcrumbs();
				Event_OnUpdateData().Invoke();
			}
		}
	}
	
	// --------------------------------------------------------------------------------

	void DeleteAllTrigers()
	{
		int count = m_TriggerHandles.Count();
		if (count > 0)
		{
			m_TriggerVisualsSelected = null;

			m_TriggerVisuals.Clear();
			m_TriggerHandles.Clear();

			DeleteAllBreadcrumbs();

			Event_OnUpdateData().Invoke();
		}
	}
	
	// --------------------------------------------------------------------------------

	void DeleteAllBreadcrumbs()
	{
		int count = m_BreadcrumbHandles.Count();
		if (count > 0)
		{
			m_BreadcrumbVisualsSelected = null;

			m_BreadcrumbHandles.Clear();
			m_BreadcrumbVisuals.Clear();

			Event_OnUpdateData().Invoke();
		}
	}
	
	// --------------------------------------------------------------------------------

	void ExportData()
	{
		JsonUndergroundTriggers jsonData = new JsonUndergroundTriggers();
		jsonData.Triggers = new array<ref JsonUndergroundAreaTriggerData>();

		foreach (UndergroundTriggerHandle triggerHandle : m_TriggerHandles)
		{
			vector size = triggerHandle.m_Reference.GetSize();
			triggerHandle.m_Data.Size = new TFloatArray();
			triggerHandle.m_Data.Size.Insert(size[0]);
			triggerHandle.m_Data.Size.Insert(size[1]);
			triggerHandle.m_Data.Size.Insert(size[2]);

			vector center = triggerHandle.m_Reference.GetCenter();
			triggerHandle.m_Data.Position = new TFloatArray();
			triggerHandle.m_Data.Position.Insert(center[0]);
			triggerHandle.m_Data.Position.Insert(center[1]);
			triggerHandle.m_Data.Position.Insert(center[2]);

			vector orientation = triggerHandle.m_Reference.GetOrientation();
			triggerHandle.m_Data.Orientation = new TFloatArray();
			triggerHandle.m_Data.Orientation.Insert(orientation[0]);
			triggerHandle.m_Data.Orientation.Insert(orientation[1]);
			triggerHandle.m_Data.Orientation.Insert(orientation[2]);
			
			foreach (int index, UndergroundBreadcrumbHandle breadcrumbHandle : m_BreadcrumbHandles)
			{
				if (breadcrumbHandle.m_ParentTrigger && triggerHandle != breadcrumbHandle.m_ParentTrigger)
					continue;

				vector breadcrumbPosition = breadcrumbHandle.m_Reference.GetCenter();
				breadcrumbHandle.m_Data.Position = new TFloatArray();
				breadcrumbHandle.m_Data.Position.Insert(breadcrumbPosition[0]);
				breadcrumbHandle.m_Data.Position.Insert(breadcrumbPosition[1]);
				breadcrumbHandle.m_Data.Position.Insert(breadcrumbPosition[2]);

				JsonUndergroundAreaBreadcrumb originalBreadcrumb  = JsonUndergroundAreaBreadcrumb.Cast(triggerHandle.m_Data.Breadcrumbs[index]);
				originalBreadcrumb.Position 		= breadcrumbHandle.m_Data.Position;
				originalBreadcrumb.EyeAccommodation = breadcrumbHandle.m_Data.EyeAccommodation;
				originalBreadcrumb.Radius			= breadcrumbHandle.m_Data.Radius;
				originalBreadcrumb.LightLerp 		= breadcrumbHandle.m_Data.LightLerp;
				originalBreadcrumb.UseRaycast 		= breadcrumbHandle.m_Data.UseRaycast;
			}

			JsonUndergroundAreaTriggerData triggerData = new JsonUndergroundAreaTriggerData();
			triggerData = triggerHandle.m_Data;

			jsonData.Triggers.Insert(triggerData);
		}

		string errorMessage;
		if (!JsonFileLoader<JsonUndergroundTriggers>.SaveFile("$mission:cfgundergroundtriggers.json", jsonData, errorMessage))
		{
			ErrorEx(errorMessage);
			return;
		}

		m_TriggersDirty = false;
		Event_OnUpdateData().Invoke();
		
		OnExport();
	}
	
	// --------------------------------------------------------------------------------

	void FocusOnObject()
	{
		if (m_TriggerVisualsSelected)
			FocusCameraOn(m_TriggerVisualsSelected.GetCenter(), m_TriggerVisualsSelected.GetSize());
		else if (m_BreadcrumbVisualsSelected)
			// Breadcrumbs have no real size, use an artificial 2m distance
			FocusCameraOn(m_BreadcrumbVisualsSelected.GetCenter(), Vector(1, 1, 1), 2.0);
	}
	
	// --------------------------------------------------------------------------------

	private void FocusCameraOn(vector position, vector size, float maxDistance = 0)
	{
		vector cameraPosition = position;
		cameraPosition[0] = cameraPosition[0] + size[0] * 2;
		cameraPosition[1] = cameraPosition[1] + size[1] + 0.5;
		cameraPosition[2] = cameraPosition[2] + size[2] * 2;

		// Clamp the camera offset to maxDistance while preserving the viewing angle.
		// Triggers pass the default (0) and are unaffected.
		if (maxDistance > 0)
		{
			vector offset = cameraPosition - position;
			if (offset.Length() > maxDistance)
				cameraPosition = position + (offset.Normalized() * maxDistance);
		}

		vector viewTarget = position;
		viewTarget[1] = viewTarget[1] + size[1] * 0.5;

		vector cameraDirection = viewTarget - cameraPosition;
		cameraDirection.Normalize();

		FreeDebugCamera.GetInstance().SetPosition(cameraPosition);
		FreeDebugCamera.GetInstance().SetDirection(cameraDirection);
	}
	
	// --------------------------------------------------------------------------------

	// Bound in PluginKeybings directly
	void FreeCameraControlEnable()
	{
		if (g_Game.GetUIManager().GetMenu())
			g_Game.GetUIManager().GetMenu().OnHide();
	}
	
	// --------------------------------------------------------------------------------

	// Bound in PluginKeybings directly
	void FreeCameraControlDisable()
	{
		if (g_Game.GetUIManager().GetMenu())
			g_Game.GetUIManager().GetMenu().OnShow();
	}
	
	// --------------------------------------------------------------------------------

	void LoadDataFromFile()
	{
		// preload data from cfgundergoundtriggers.json
		JsonUndergroundTriggers jsonData = UndergroundAreaLoader.GetData();
		if (jsonData && jsonData.Triggers)
		{
			foreach (int index, JsonUndergroundAreaTriggerData data : jsonData.Triggers)
			{
				BoxDraw box = new BoxDraw(data.GetPosition(), data.GetOrientation(), data.GetSize());
				m_TriggerVisuals.Insert(box);
				UndergroundTriggerHandle handle = box.GetTriggerHandle();
				handle.m_Data = data;
				if (!handle.m_Data.Comment)
					handle.m_Data.Comment = string.Format("trigger %1", index);
				box.Event_OnTransformChanged().Insert(MarkTriggersDirtyOnTransform);
				m_TriggerHandles.Insert(handle);
			}

			m_TriggersDirty = false;
			Event_OnUpdateData().Invoke();
		}
	}
	
	private void OnExport()
	{
		//backup original file
		int year, month, day, hour, minute, second;
		GetYearMonthDayUTC(year, month, day);
		GetHourMinuteSecondUTC(hour, minute, second);
		string datetimeUTC	= string.Format(DATETIME_FORMAT, year, month.ToStringLen(2), day.ToStringLen(2), hour.ToStringLen(2), minute.ToStringLen(2), second.ToStringLen(2));
		string backupFilename = string.Format("$mission:cfgundergroundtriggers.json.backup-%1", datetimeUTC);
		CopyFile("$mission:cfgundergroundtriggers.json", backupFilename);
	}

	// --------------------------------------------------------------------------------	

	private bool IsTransformUnchanged(vector oldTransform[4], vector newTransform[4])
	{
		const float eps = 0.0001;
		return vector.Distance(oldTransform[3], newTransform[3]) < eps && vector.Distance(oldTransform[0], newTransform[0]) < eps && vector.Distance(oldTransform[1], newTransform[1]) < eps && vector.Distance(oldTransform[2], newTransform[2]) < eps;
	}
	
	// --------------------------------------------------------------------------------

	private bool IsMouseInScene()
	{
		Widget widgetUnder = GetWidgetUnderCursor();

		if (widgetUnder != null && widgetUnder.GetName() == "panel_scene")
			return true;

		return false;
	}
	
	// --------------------------------------------------------------------------------
	
	private void SortBoxDrawsByDistance(ArrayBoxDraw arr)
	{
		int n = arr.Count();
		for (int i = 1; i < n; i++)
		{
			BoxDraw key = arr.Get(i);
			float keyDist = vector.Distance(m_LastSortCameraPosition, key.GetCenter());
			int j = i - 1;
			while (j >= 0 && vector.Distance(m_LastSortCameraPosition, arr.Get(j).GetCenter()) > keyDist)
			{
				arr.Set(j + 1, arr.Get(j));
				j--;
			}
			arr.Set(j + 1, key);
		}
	}
	
	// --------------------------------------------------------------------------------

	private void SortBreadcrumbDrawsByDistance(ArrayBreadcrumbDraw arr)
	{
		int n = arr.Count();
		for (int i = 1; i < n; i++)
		{
			BreadcrumbDraw key = arr.Get(i);
			float keyDist = vector.Distance(m_LastSortCameraPosition, key.GetCenter());
			int j = i - 1;
			while (j >= 0 && vector.Distance(m_LastSortCameraPosition, arr.Get(j).GetCenter()) > keyDist)
			{
				arr.Set(j + 1, arr.Get(j));
				j--;
			}
			arr.Set(j + 1, key);
		}
	}
}
#endif
