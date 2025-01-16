class ScriptConsoleCameraValues
{
	static float m_CurrentFOV;
	static float m_CurrentFocusDistance;
	static float m_CurrentFocusLength;
	static float m_CurrentFocusLengthNear;
	static float m_CurrentBlur;
	static float m_CurrentFocusDepthOffset;
	
	static void SetDefaultValues()
	{
		m_CurrentFOV = 1.0;
		m_CurrentFocusDistance = 0.0;
		m_CurrentFocusLength = -1.0;
		m_CurrentFocusLengthNear = -1.0;
		m_CurrentBlur = 0.0;
		m_CurrentFocusDepthOffset = 1.0;
	}
}

static ref ScriptConsoleCameraValues s_ScriptConsoleValues;
static ScriptConsoleCameraValues GetScriptConsoleValues()
{
	if (!s_ScriptConsoleValues)
		s_ScriptConsoleValues = new ScriptConsoleCameraValues;
	return s_ScriptConsoleValues;
}

class ScriptConsoleCameraTab : ScriptConsoleTabBase
{	
	protected ref ScriptConsoleSelector m_FOVSelector;
	protected ref ScriptConsoleSelector m_FocusDistanceSelector;
	protected ref ScriptConsoleSelector m_FocusLengthSelector;
	protected ref ScriptConsoleSelector m_FocusLengthNearSelector;
	protected ref ScriptConsoleSelector m_BlurSelector;
	protected ref ScriptConsoleSelector m_FocusDepthOffsetSelector;
	
	protected TextWidget m_FOVValue;
	protected EditBoxWidget m_FOVEditbox;
	
	protected TextWidget m_FocusDistanceValue;
	protected EditBoxWidget m_FocusDistanceEditbox;
	
	protected TextWidget m_FocusLengthValue;
	protected EditBoxWidget m_FocusLengthEditbox;

	protected TextWidget m_FocusLengthNearValue;
	protected EditBoxWidget m_FocusLengthNearEditbox;

	protected TextWidget m_BlurValue;
	protected EditBoxWidget m_BlurEditbox;
	
	protected TextWidget m_FocusDepthOffsetValue;
	protected EditBoxWidget  m_FocusDepthOffsetEditbox;
	
	protected ButtonWidget m_ResetDOFButton;
	protected EditBoxWidget m_DOFPresetEditbox;

	protected Widget m_DOFPresetsPanel;
	protected Widget m_DOFPresetsBtns;
	protected ButtonWidget m_DOFShowPresetsBtn;
	protected TextWidget m_DOFShowPresetsBtnLabel;
	protected ButtonWidget m_DOFSavePrefabBtn;
	protected ButtonWidget m_DOFDeletePrefabBtn;
	protected ButtonWidget m_DOFLoadPrefabBtn;
	
	protected FreeDebugCamera m_FreeDebugCamera;
	protected bool m_MouseMovementEnabled;
	protected bool m_DOFPresetsVisible;
	protected bool m_IsCTRLPressed;
	protected bool m_IsCAPSPressed;
	protected bool m_IsReturnPressed;
	protected bool m_ValueEditboxMode;
	protected float m_ValueEditModeTime;
	protected bool m_FirstInit;
	protected ScriptConsoleCameraDOFPreset m_SelectedDOFPreset;
	protected ref ScriptConsolePresetsList m_DOFPresetsList;
	protected bool m_CanUseMovement;
	
	void ScriptConsoleCameraTab(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
	{
		m_FreeDebugCamera = FreeDebugCamera.GetInstance();
		
		m_FOVSelector = new ScriptConsoleSelector(root.FindAnyWidget("SliderFOVPanel"), m_FreeDebugCamera.GetCurrentFOV(), this, false, 0.001, 1.0);
		m_FocusDistanceSelector = new ScriptConsoleSelector(root.FindAnyWidget("SliderFocusDistancePanel"), GetScriptConsoleValues().m_CurrentFocusDistance, this, false, 0.0, 1000.0);
		
		m_FocusLengthSelector = new ScriptConsoleSelector(root.FindAnyWidget("SliderFocusLengthPanel"), GetScriptConsoleValues().m_CurrentFocusLength, this, false, -1.0, 10000.0);
		m_FocusLengthSelector.SetStep(0.0001);
		
		m_FocusLengthNearSelector = new ScriptConsoleSelector(root.FindAnyWidget("SliderFocusLengthNearPanel"), GetScriptConsoleValues().m_CurrentFocusLengthNear, this, false, -1.0, 10000.0);
		m_FocusLengthNearSelector.SetStep(0.0001);
	
		m_BlurSelector = new ScriptConsoleSelector(root.FindAnyWidget("SliderBlurPanel"), GetScriptConsoleValues().m_CurrentBlur, this, false, 0.0, 32.0);
		m_FocusDepthOffsetSelector = new ScriptConsoleSelector(root.FindAnyWidget("SliderFocusDepthOffsetPanel"), GetScriptConsoleValues().m_CurrentFocusDepthOffset, this, false, 0.0, 1000.0);
		
		m_FOVSelector.m_OptionChanged.Insert(UpdateFOV);
		m_FocusDistanceSelector.m_OptionChanged.Insert(UpdateFocusDistance);
		m_FocusLengthSelector.m_OptionChanged.Insert(UpdateFocusLength);
		m_FocusLengthNearSelector.m_OptionChanged.Insert(UpdateFocusLengthNear);
		m_BlurSelector.m_OptionChanged.Insert(UpdateBlur);
		m_FocusDepthOffsetSelector.m_OptionChanged.Insert(UpdateFocusDepthOffset);
		
		m_FOVValue = TextWidget.Cast(root.FindAnyWidget("SliderFOVValue"));
		m_FOVEditbox = EditBoxWidget.Cast(root.FindAnyWidget("SliderFOVEditbox"));
		
		m_FocusDistanceValue = TextWidget.Cast(root.FindAnyWidget("SliderFocusDistanceValue"));
		m_FocusDistanceEditbox = EditBoxWidget.Cast(root.FindAnyWidget("SliderFocusDistanceEditbox"));
		
		m_FocusLengthValue = TextWidget.Cast(root.FindAnyWidget("SliderFocusLengthValue"));
		m_FocusLengthEditbox = EditBoxWidget.Cast(root.FindAnyWidget("SliderFocusLengthEditbox"));
		
		m_FocusLengthNearValue = TextWidget.Cast(root.FindAnyWidget("SliderFocusLengthNearValue"));
		m_FocusLengthNearEditbox = EditBoxWidget.Cast(root.FindAnyWidget("SliderFocusLengthNearEditbox"));
		
		m_BlurValue = TextWidget.Cast(root.FindAnyWidget("SliderBlurValue"));
		m_BlurEditbox = EditBoxWidget.Cast(root.FindAnyWidget("SliderBlurEditbox"));
		
		m_FocusDepthOffsetValue = TextWidget.Cast(root.FindAnyWidget("SliderFocusDepthOffsetValue"));
		m_FocusDepthOffsetEditbox = EditBoxWidget.Cast(root.FindAnyWidget("SliderFocusDepthOffsetEditbox"));
		
		m_ResetDOFButton = ButtonWidget.Cast(root.FindAnyWidget("ResetDOFButton"));
	
		m_FOVValue.SetText(GetScriptConsoleValues().m_CurrentFOV.ToString());
		m_FocusDistanceValue.SetText(GetScriptConsoleValues().m_CurrentFocusDistance.ToString());
		m_FocusLengthValue.SetText(GetScriptConsoleValues().m_CurrentFocusLength.ToString());
		m_FocusLengthNearValue.SetText(GetScriptConsoleValues().m_CurrentFocusLengthNear.ToString());
		m_BlurValue.SetText(GetScriptConsoleValues().m_CurrentBlur.ToString());
		m_FocusDepthOffsetValue.SetText(GetScriptConsoleValues().m_CurrentFocusDepthOffset.ToString());

		m_DOFPresetsPanel = root.FindAnyWidget("DOFPresetsPanel");
		m_DOFPresetsList = new ScriptConsolePresetsList(m_DOFPresetsPanel, this);
		m_DOFPresetsList.m_ItemSelected.Insert(OnListItemSelected);
		
		m_DOFPresetsBtns = root.FindAnyWidget("DOFPresetsButtons");
		m_DOFLoadPrefabBtn = ButtonWidget.Cast(root.FindAnyWidget("DOFLoadPrefab"));
		m_DOFDeletePrefabBtn = ButtonWidget.Cast(root.FindAnyWidget("DOFDeletePrefab"));
		
		m_DOFShowPresetsBtn = ButtonWidget.Cast(root.FindAnyWidget("DOFPresetsButton"));
		m_DOFShowPresetsBtnLabel = TextWidget.Cast(root.FindAnyWidget("DOFPresetsLabel"));
		m_DOFPresetEditbox = EditBoxWidget.Cast(root.FindAnyWidget("DOFName"));
		m_DOFSavePrefabBtn = ButtonWidget.Cast(root.FindAnyWidget("SaveDOFButton"));

		m_DOFShowPresetsBtn.Show(true);
		m_DOFPresetEditbox.Show(true);
		m_DOFSavePrefabBtn.Show(true);
		
		if (!m_FirstInit)
		{
			ResetDOF();
			m_FirstInit = true;
		}
		
		ReloadDOFPresetsList();
	}
		
	protected void ReloadDOFPresetsList()
	{
		m_DOFPresetsList.ClearList();
		
		for (int i = 0; i < m_ConfigDebugProfile.GetCameraPresets().Count(); i++) 
		{
			ScriptConsoleCameraDOFPreset preset = m_ConfigDebugProfile.GetCameraPresets()[i];
			if (!preset)
			{
				ErrorEx("Could not load DOF preset entry!");
				continue;
			}

			m_DOFPresetsList.AddListItem(preset.Name, 0, i);
		}
	}
	
	protected void UpdateFOV(float value)
	{
		GetScriptConsoleValues().m_CurrentFOV = value;
		m_FOVValue.SetText(value.ToString());
		m_FOVEditbox.SetText(value.ToString());
		m_FreeDebugCamera.SetFOV(value);
	}
	
	protected void UpdateFocusDistance(float value)
	{
		GetScriptConsoleValues().m_CurrentFocusDistance = value;
		m_FocusDistanceValue.SetText(value.ToString());
		m_FocusDistanceEditbox.SetText(value.ToString());
		SetDOF();
	}
	
	protected void UpdateFocusLength(float value)
	{
		GetScriptConsoleValues().m_CurrentFocusLength = value;
		m_FocusLengthValue.SetText(value.ToString());
		m_FocusLengthEditbox.SetText(value.ToString());
		SetDOF();
	}
	
	protected void UpdateFocusLengthNear(float value)
	{
		GetScriptConsoleValues().m_CurrentFocusLengthNear = value;
		m_FocusLengthNearValue.SetText(value.ToString());
		m_FocusLengthNearEditbox.SetText(value.ToString());
		SetDOF();
	}
	
	protected void UpdateBlur(float value)
	{
		GetScriptConsoleValues().m_CurrentBlur = value;
		m_BlurValue.SetText(value.ToString());
		m_BlurEditbox.SetText(value.ToString());
		SetDOF();
	}

	protected void UpdateFocusDepthOffset(float value)
	{
		GetScriptConsoleValues().m_CurrentFocusDepthOffset = value;
		m_FocusDepthOffsetValue.SetText(value.ToString());
		m_FocusDepthOffsetEditbox.SetText(value.ToString());
		SetDOF();
	}
	
	protected void SetDOF()
	{
		GetGame().OverrideDOF(true, GetScriptConsoleValues().m_CurrentFocusDistance, GetScriptConsoleValues().m_CurrentFocusLength, GetScriptConsoleValues().m_CurrentFocusLengthNear, GetScriptConsoleValues().m_CurrentBlur, GetScriptConsoleValues().m_CurrentFocusDepthOffset);
	}
	
	protected void ResetDOF()
	{
		GetScriptConsoleValues().SetDefaultValues();
		UpdateDOFSliders();
		GetGame().OverrideDOF(true, GetScriptConsoleValues().m_CurrentFocusDistance, GetScriptConsoleValues().m_CurrentFocusLength, GetScriptConsoleValues().m_CurrentFocusLengthNear, GetScriptConsoleValues().m_CurrentBlur, GetScriptConsoleValues().m_CurrentFocusDepthOffset);
	}
	
	protected void UpdateDOFSliders()
	{
		m_FocusDistanceValue.SetText(GetScriptConsoleValues().m_CurrentFocusDistance.ToString());
		m_FocusDistanceEditbox.SetText(GetScriptConsoleValues().m_CurrentFocusDistance.ToString());
		m_FocusDistanceSelector.SetValue(GetScriptConsoleValues().m_CurrentFocusDistance, false);
		
		m_FocusLengthValue.SetText(GetScriptConsoleValues().m_CurrentFocusLength.ToString());
		m_FocusLengthEditbox.SetText(GetScriptConsoleValues().m_CurrentFocusLength.ToString());
		m_FocusLengthSelector.SetValue(GetScriptConsoleValues().m_CurrentFocusLength, false);
		
		m_FocusLengthNearValue.SetText(GetScriptConsoleValues().m_CurrentFocusLengthNear.ToString());
		m_FocusLengthNearEditbox.SetText(GetScriptConsoleValues().m_CurrentFocusLengthNear.ToString());
		m_FocusLengthNearSelector.SetValue(GetScriptConsoleValues().m_CurrentFocusLengthNear, false);
		
		m_BlurValue.SetText(GetScriptConsoleValues().m_CurrentBlur.ToString());
		m_BlurEditbox.SetText(GetScriptConsoleValues().m_CurrentBlur.ToString());
		m_BlurSelector.SetValue(GetScriptConsoleValues().m_CurrentBlur, false);
		
		m_FocusDepthOffsetValue.SetText(GetScriptConsoleValues().m_CurrentFocusDepthOffset.ToString());
		m_FocusDepthOffsetEditbox.SetText(GetScriptConsoleValues().m_CurrentFocusDepthOffset.ToString());
		m_FocusDepthOffsetSelector.SetValue(GetScriptConsoleValues().m_CurrentFocusDepthOffset, false);
	}
	
	protected void SaveDOFPreset()
	{
		m_ConfigDebugProfile.AddCameraPreset(m_DOFPresetEditbox.GetText(), GetScriptConsoleValues().m_CurrentFocusDistance, GetScriptConsoleValues().m_CurrentFocusLength, GetScriptConsoleValues().m_CurrentFocusLengthNear, GetScriptConsoleValues().m_CurrentBlur,  GetScriptConsoleValues().m_CurrentFocusDepthOffset);
		ReloadDOFPresetsList();
	}
	
	protected void DeleteDOFPreset()
	{
		if (!m_SelectedDOFPreset)
			return;
		
		m_ConfigDebugProfile.RemoveCameraPreset(m_SelectedDOFPreset.Name);
		ReloadDOFPresetsList();
	}

	protected void OnListItemSelected(int row, int column)
	{
		m_SelectedDOFPreset = m_ConfigDebugProfile.GetCameraPresets().Get(row);
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w == m_DOFPresetEditbox)
		{
			GetGame().GetMission().RemoveActiveInputExcludes({"movement"});
			if (GetFocus() == m_DOFPresetEditbox)
				SetFocus(null);
		}
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w == m_DOFPresetEditbox)
		{
			GetGame().GetMission().AddActiveInputExcludes({"movement"});
		}
		return false;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_ResetDOFButton)
		{
			ResetDOF();
			return true;
		}
		else if (w == m_DOFShowPresetsBtn)
		{
			if (!m_DOFPresetsVisible)
			{
				m_DOFShowPresetsBtnLabel.SetText("Hide DOF Presets");
				m_DOFPresetsPanel.Show(true);
				m_DOFPresetsBtns.Show(true);
				m_DOFPresetsVisible = true;
			}
			else
			{
				m_DOFShowPresetsBtnLabel.SetText("Show DOF Presets");
				m_DOFPresetsPanel.Show(false);
				m_DOFPresetsBtns.Show(false);
				m_DOFPresetsVisible = false;
			}
		}
		else if (w == m_DOFLoadPrefabBtn)
		{
			LoadDOFPreset();
		}
		else if (w == m_DOFSavePrefabBtn)
		{
			SaveDOFPreset();
		}
		else if (w == m_DOFDeletePrefabBtn)
		{
			DeleteDOFPreset();
		}
		
		return false;
	}

	protected void LoadDOFPreset()
	{
		if (!m_SelectedDOFPreset)
			return;

		GetScriptConsoleValues().m_CurrentFocusDistance = m_SelectedDOFPreset.FocusDistance;
		GetScriptConsoleValues().m_CurrentFocusLength = m_SelectedDOFPreset.FocusLength;
		GetScriptConsoleValues().m_CurrentFocusLengthNear = m_SelectedDOFPreset.FocusLengthNear;
		GetScriptConsoleValues().m_CurrentBlur = m_SelectedDOFPreset.Blur;
		GetScriptConsoleValues().m_CurrentFocusDepthOffset = m_SelectedDOFPreset.FocusDepthOffset;
		UpdateDOFSliders();
		SetDOF();
	}
	
	protected void ToggleValueEditMode(bool editBoxMode)
	{
		m_FOVValue.Show(!editBoxMode);
		m_FOVEditbox.Show(editBoxMode);
		m_FOVSelector.GetLayoutRoot().Show(!editBoxMode);

		m_FocusDistanceValue.Show(!editBoxMode);
		m_FocusDistanceEditbox.Show(editBoxMode);
		m_FocusDistanceSelector.GetLayoutRoot().Show(!editBoxMode);

		m_FocusLengthValue.Show(!editBoxMode);
		m_FocusLengthEditbox.Show(editBoxMode);
		m_FocusLengthSelector.GetLayoutRoot().Show(!editBoxMode);

		m_FocusLengthNearValue.Show(!editBoxMode);
		m_FocusLengthNearEditbox.Show(editBoxMode);
		m_FocusLengthNearSelector.GetLayoutRoot().Show(!editBoxMode);

		m_BlurValue.Show(!editBoxMode);
		m_BlurEditbox.Show(editBoxMode);
		m_BlurSelector.GetLayoutRoot().Show(!editBoxMode);

		m_FocusDepthOffsetValue.Show(!editBoxMode);
		m_FocusDepthOffsetEditbox.Show(editBoxMode);
		m_FocusDepthOffsetSelector.GetLayoutRoot().Show(!editBoxMode);
		
		if (editBoxMode)
		{
			m_FOVEditbox.SetText(GetScriptConsoleValues().m_CurrentFOV.ToString());
			m_FocusDistanceEditbox.SetText(GetScriptConsoleValues().m_CurrentFocusDistance.ToString());
			m_FocusLengthEditbox.SetText(GetScriptConsoleValues().m_CurrentFocusLength.ToString());
			m_FocusLengthNearEditbox.SetText(GetScriptConsoleValues().m_CurrentFocusLengthNear.ToString());
			m_BlurEditbox.SetText(GetScriptConsoleValues().m_CurrentBlur.ToString());
			m_FocusDepthOffsetEditbox.SetText(GetScriptConsoleValues().m_CurrentFocusDepthOffset.ToString());
		}
	}
	
	protected void UpdateValuesFromEditMode()
	{
		if (IsValidEditboxValue(m_FOVEditbox.GetText(), 0.001, 1.0))
			UpdateFOV(m_FOVEditbox.GetText().ToFloat());
			
		bool updateDOF = true;
		if (IsValidEditboxValue(m_FocusDistanceEditbox.GetText(), 0.0, 1000.0))
		{
			UpdateFocusDistance(m_FocusDistanceEditbox.GetText().ToFloat());
		}
		else
		{
			updateDOF = false;
		}
		
		if (IsValidEditboxValue(m_FocusLengthEditbox.GetText(), -1.0, 10000.0))
		{
			UpdateFocusLength(m_FocusLengthEditbox.GetText().ToFloat());
		}
		else
		{
			updateDOF = false;
		}
		
		if (IsValidEditboxValue(m_FocusLengthNearEditbox.GetText(), -1.0, 10000.0))
		{
			UpdateFocusLengthNear(m_FocusLengthNearEditbox.GetText().ToFloat());
		}
		else
		{
			updateDOF = false;
		}
		
		if (IsValidEditboxValue(m_BlurEditbox.GetText(), 0.0, 32.0))
		{
			UpdateBlur(m_BlurEditbox.GetText().ToFloat());
		}
		else
		{
			updateDOF = false;
		}
		
		if (IsValidEditboxValue(m_FocusDepthOffsetEditbox.GetText(), 0.0, 1000.0))
		{
			UpdateFocusDepthOffset(m_FocusDepthOffsetEditbox.GetText().ToFloat());
		}
		else
		{
			updateDOF = false;
		}
		
		if (updateDOF)
		{
			UpdateDOFSliders();
			SetDOF();
		}
	}
	
	bool IsValidEditboxValue(string text, float min, float max)
	{
		TStringArray allowedCharacters = {".","0","1","2","3","4","5","6","7","8","9"};
		for (int i = 0; i < text.Length(); i++)
		{
			int foundIndex = allowedCharacters.Find(text.Get(i));
			if (foundIndex == -1 || i == 0 && foundIndex == 0 || i == text.Length() && foundIndex == 0)
				return false;
		}
		
		if (text.ToFloat() < min || text.ToFloat() > max)
			return false;
		
		return true;
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		m_IsCTRLPressed = KeyState(KeyCode.KC_LCONTROL) || KeyState(KeyCode.KC_RCONTROL);
		m_IsCAPSPressed = KeyState(KeyCode.KC_CAPITAL);
		m_IsReturnPressed = KeyState(KeyCode.KC_RETURN);
		
		if (m_IsReturnPressed && m_ValueEditboxMode)
		{
			UpdateValuesFromEditMode();
			ToggleValueEditMode(false);
		}
		
		if (m_IsCTRLPressed)
		{
			m_ValueEditModeTime += timeslice;
			if (m_ValueEditModeTime >= 0.300)
			{
				if (!m_ValueEditboxMode)
				{
					ToggleValueEditMode(true);
				}
				else
				{
					UpdateValuesFromEditMode();
					ToggleValueEditMode(false);
				}
				
				m_ValueEditboxMode = !m_ValueEditboxMode;
				m_ValueEditModeTime = 0;
			}
		}
		else
		{
			m_ValueEditModeTime = 0;
		}
		
		if (m_IsCAPSPressed && !m_MouseMovementEnabled && (!GetFocus() || GetFocus() != m_DOFPresetEditbox) && GetWidgetUnderCursor() != m_DOFPresetEditbox)
		{
			if (GetGame() && GetGame().GetMission() && GetGame().GetUIManager())
			{
				m_MouseMovementEnabled = true;
				GetGame().GetMission().RemoveActiveInputExcludes({"aiming"});
				GetGame().GetUIManager().ShowUICursor(false);
			}
		}
		else if (!m_IsCAPSPressed && m_MouseMovementEnabled)
		{
			if (GetGame() && GetGame().GetMission() && GetGame().GetUIManager())
			{
				m_MouseMovementEnabled = false;
				GetGame().GetMission().AddActiveInputExcludes({"aiming"});
				GetGame().GetUIManager().ShowUICursor(true);
			}
		}

		if (GetFocus() == m_DOFPresetEditbox && !m_CanUseMovement)
		{
			m_CanUseMovement = true;
			GetGame().GetMission().RemoveActiveInputExcludes({"UAMoveForward", "UAMoveBack", "UAMoveLeft", "UAMoveRight", "UAWalkRunTemp"});
		}
		else if ((GetFocus() != m_DOFPresetEditbox || !GetFocus()) && GetWidgetUnderCursor() != m_DOFPresetEditbox && m_CanUseMovement)
		{
			m_CanUseMovement = false;
			GetGame().GetMission().AddActiveInputExcludes({"UAMoveForward", "UAMoveBack", "UAMoveLeft", "UAMoveRight", "UAWalkRunTemp"});
		}
	}
}
