class GameplayEffectWidgets_base extends Managed
{
	int m_MaskWidgetUpdateCount;//number of times the widget was updated through a single voice event

	void IncreaseMaskUpdateCount(){m_MaskWidgetUpdateCount++};
	void ResetMaskUpdateCount(){m_MaskWidgetUpdateCount = 0};
	
	bool IsAnyEffectRunning(){}
	bool AreEffectsSuspended(){}
	void AddActiveEffects(array<int> effects){}
	void RemoveActiveEffects(array<int> effects){}
	void StopAllEffects(){}
	void AddSuspendRequest(int request_id){}
	void RemoveSuspendRequest(int request_id){}
	void ClearSuspendRequests(){}
	int GetSuspendRequestCount(){}
	void UpdateWidgets(int type = -1, float timeSlice = 0, Param p = null, int handle = -1){}
	void Update(float timeSlice){}
	void OnVoiceEvent(float breathing_resistance01){}
	void SetBreathIntensityStamina(float stamina_cap, float stamina_current){}
	
	void RegisterGameplayEffectData(int id, Param p){}
}