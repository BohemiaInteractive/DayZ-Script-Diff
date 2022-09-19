/*
TODO - doxygen formating
*/

//! grouped gameplay effect widgets and their handling
class GameplayEffectWidgets extends GameplayEffectWidgets_base
{
	protected ref Widget 								m_Root; //dummy parent node
	protected ref map<int,ref Widget> 					m_Layouts;
	protected ref set<ref Widget> 						m_UniqueLayouts;
	protected ref GameplayEffectDataMap 				m_WidgetDataMap;
	protected ref set<int> 								m_RunningEffects;
	protected ref set<int> 								m_RunningEffectsPrevious;
	protected ref array<int> 							m_UpdatingEffects;
	protected ref array<ref Widget> 					m_UpdatedWidgetsCheck; //to make sure widgets are not updated over and over (case of multiple IDs sharing same widget set)
	protected ref array<int> 							m_UpdatedWidgetSetsCheck; //to make sure sets are not updated over and over (case of multiple IDs sharing same widget set)
	protected ref set<int> 								m_SuspendRequests;
	protected ref map<int,typename> 					m_IDToTypeMap;
	
	protected float 									m_TimeProgBreath;
	protected float 									m_BreathMultStamina;
	protected float 									m_BreathResidue;
	
	//UserID's for widget containers that use something different from 'EffectWidgetsTypes' defaults
	protected const int 								WIDGETSET_BREATH = 100;
	
	//effect values
	protected int 										m_BreathColor;
	protected float 									m_BreathAlphaVal;
	protected float 									m_FlashbangCoverAlphaVal;
	
	void GameplayEffectWidgets()
	{
		m_Root = GetGame().GetWorkspace().CreateWidget(FrameWidgetTypeID,0,0,1.0,1.0,WidgetFlags.VISIBLE | WidgetFlags.HEXACTPOS | WidgetFlags.VEXACTPOS, 0xffffffff, 0);
		m_Layouts = new map<int,ref Widget>;
		m_UniqueLayouts = new set<ref Widget>;
		m_WidgetDataMap = new GameplayEffectDataMap;
		m_RunningEffects = new set<int>;
		m_RunningEffectsPrevious = new set<int>;
		m_UpdatingEffects = new array<int>;
		m_UpdatedWidgetsCheck = new array<ref Widget>;
		m_UpdatedWidgetSetsCheck = new array<int>;
		m_SuspendRequests = new set<int>;
		m_IDToTypeMap = new map<int,typename>;
		
		m_TimeProgBreath = 0.0;
		m_BreathMultStamina = 1.0;
		
		PairIDToTypes();
		
		RegisterLayouts("gui/layouts/gameplay/CameraEffects.layout",CompileEffectListing());
		RegisterLayouts("gui/layouts/gameplay/BleedingEffects.layout",{EffectWidgetsTypes.BLEEDING_LAYER});
		
		InitWidgetSet(EffectWidgetsTypes.MASK_BREATH,true,WIDGETSET_BREATH);
		InitWidgetSet(EffectWidgetsTypes.HELMET_BREATH,true,WIDGETSET_BREATH);
		InitWidgetSet(EffectWidgetsTypes.MOTO_BREATH,true,WIDGETSET_BREATH);
		
		InitWidgetSet(EffectWidgetsTypes.MASK_OCCLUDER,false,EffectWidgetsTypes.MASK_OCCLUDER);
		InitWidgetSet(EffectWidgetsTypes.HELMET_OCCLUDER);
		InitWidgetSet(EffectWidgetsTypes.MOTO_OCCLUDER);
		InitWidgetSet(EffectWidgetsTypes.NVG_OCCLUDER,false,EffectWidgetsTypes.NVG_OCCLUDER);
		InitWidgetSet(EffectWidgetsTypes.PUMPKIN_OCCLUDER,false,EffectWidgetsTypes.NVG_OCCLUDER);
		InitWidgetSet(EffectWidgetsTypes.EYEPATCH_OCCLUDER);
		
		InitWidgetSet(EffectWidgetsTypes.COVER_FLASHBANG);
		
		InitWidgetSet(EffectWidgetsTypes.BLEEDING_LAYER,true);
		
		UpdateVisibility();
	}
	
	void ~GameplayEffectWidgets()
	{
		for (int i = 0; i < m_Layouts.Count(); i++)
		{
			if (m_Layouts.GetElement(i))
				m_Layouts.GetElement(i).Unlink();
		}
	}
	
	/**
	\brief Registers new layout and ties effect IDs to it
	\note Order of layout creation matters, they get layered on top of each other. Within a single layout, widget priorities govern the widget order.
	*/
	protected void RegisterLayouts(string path, array<int> types)
	{
		Widget w = GetGame().GetWorkspace().CreateWidgets(path,m_Root,false);
		m_UniqueLayouts.Insert(w);
		w.Show(false);
		foreach (int i : types)
		{
			m_Layouts.Set(i,w);
		}
	}
	
	protected void PairIDToTypes()
	{
		m_IDToTypeMap.Insert(EffectWidgetsTypes.BLEEDING_LAYER,GameplayEffectsDataBleeding);
	}
	
	protected typename TranslateIDToType(int typeID)
	{
		return m_IDToTypeMap.Get(typeID);
	}
	
	override void RegisterGameplayEffectData(int id, Param p)
	{
		if (!m_WidgetDataMap.Get(id).DataInitialized())
		{
			m_WidgetDataMap.Get(id).RegisterData(p);
		}
	}
	
	/**
	\brief InitWidgetSet
		\param type \p int ID of effect widget type
		\param updating \p bool Marks widgets for 'PlayerBase.EOnFrame' updating
		\param user_id_override \p int UserID of a widget to be used instead (allows multiple types to use single widget)
		@code
			InitWidgetSet(EffectWidgetsTypes.MASK_BREATH,true,BREATH);
		@endcode
		\note All child widgets MUST be of the same type if special functionality is required (ImageWidget etc.)
	*/
	protected void InitWidgetSet(int type, bool updating = false, int user_id_override = -1)
	{
		Widget parent = null;
		if (user_id_override != -1) 
		{
			parent = m_Layouts.Get(type).FindAnyWidgetById(user_id_override);
		}
		else
		{
			parent = m_Layouts.Get(type).FindAnyWidgetById(type);
		}
		
		if (!parent)
		{
			Print("InitWidgetSet | type: " + type + " - parent not found!");
			return;
		}
		
		array<ref Widget> output;
		Widget w = parent.GetChildren();
		if (w)
		{
			output = new array<ref Widget>;
			while (w)
			{
				w.Update();
				w.Show(false,true);
				output.Insert(w);
				
				w = w.GetSibling();
			}
			
			if (parent.GetChildren())
			{
				typename handled_type = TranslateIDToType(type);
				if ( handled_type )
				{
					CreateHandledClass(handled_type,output,type,user_id_override);
				}
				else
				{
					if (ImageWidget.Cast(parent.GetChildren()))
					{
						m_WidgetDataMap.Set(type, new GameplayEffectsDataImage(output,type,user_id_override) );
					}
					else
					{
						m_WidgetDataMap.Set(type, new GameplayEffectsData(output,type,user_id_override) );
					}
				}
			}
			
			if (updating)
				m_UpdatingEffects.Insert(type);
		}
	}
	
	bool CreateHandledClass(typename handled_type, array<ref Widget> input, int type, int user_override)
	{
		if (handled_type)
		{
			GameplayEffectsData data = GameplayEffectsData.Cast(handled_type.Spawn());
			data.Init(input,type,m_Layouts.Get(type),user_override);
			m_WidgetDataMap.Set(type, data);
			return true;
		}
		return false;
	}
	
	//! returns all vanilla effects, nested in a vanilla layout. If using different layouts for custom effects, please register and link separately
	array<int> CompileEffectListing()
	{
		array<int> ret = new array<int>;
		ret.Insert(EffectWidgetsTypes.MASK_OCCLUDER);
		ret.Insert(EffectWidgetsTypes.MASK_BREATH);
		ret.Insert(EffectWidgetsTypes.HELMET_OCCLUDER);
		ret.Insert(EffectWidgetsTypes.HELMET_BREATH);
		ret.Insert(EffectWidgetsTypes.MOTO_OCCLUDER);
		ret.Insert(EffectWidgetsTypes.MOTO_BREATH);
		ret.Insert(EffectWidgetsTypes.COVER_FLASHBANG);
		ret.Insert(EffectWidgetsTypes.NVG_OCCLUDER);
		ret.Insert(EffectWidgetsTypes.PUMPKIN_OCCLUDER);
		ret.Insert(EffectWidgetsTypes.EYEPATCH_OCCLUDER);
		
		return ret;
	}
	
	protected void UpdateVisibility()
	{
		Widget w;
		//Hide diff
		int value;
		GameplayEffectsData dta;
		for (int i = 0; i < m_RunningEffectsPrevious.Count(); i++)
		{
			value = m_RunningEffectsPrevious.Get(i);
			dta = m_WidgetDataMap.Get(value);
			if (m_RunningEffects.Count() < 1 || m_RunningEffects.Find(value) == -1)
			{
				if (dta.HasDefinedHandle())
				{
					dta.UpdateVisibility(false);
				}
				else
				{
					for (int j = 0; j < m_WidgetDataMap.Get(value).GetWidgetSet().Count(); j++)
					{
						w = m_WidgetDataMap.Get(value).GetWidgetSet().Get(j);
						w.Show(false);
					}
					w.GetParent().Show(false);
				}
			}
		}
		
		//Show running effects
		if (m_RunningEffects.Count() > 0)
		{
			value = 0;
			for (i = 0; i < m_RunningEffects.Count(); i++)
			{
				value = m_RunningEffects.Get(i);
				dta = m_WidgetDataMap.Get(value);
				if (dta.HasDefinedHandle())
				{
					dta.m_LayoutRoot.Show(true);
					dta.UpdateVisibility(true);
				}
				else
				{
					for (j = 0; j < m_WidgetDataMap.Get(value).GetWidgetSet().Count(); j++)
					{
						w = m_WidgetDataMap.Get(value).GetWidgetSet().Get(j);
						w.Update();
						w.Show(true);
					}
					
					while (w) //dumb but necessary because of uncertain "visible" setting of the layout
					{
						w = w.GetParent();
						if (w)
						{
							w.Show(true);
						}
					}
				}
			}
		}
		
		m_Root.Show(m_RunningEffects.Count() > 0 && m_SuspendRequests.Count() < 1);
		m_RunningEffectsPrevious.Clear();
	}
	
	override void AddActiveEffects(array<int> effects)
	{
		if (effects && effects.Count() > 0)
		{
			m_RunningEffectsPrevious.Copy(m_RunningEffects);
			
			int value;
			for (int i = 0; i < effects.Count(); i++)
			{
				value = effects.Get(i);
				m_RunningEffects.Insert(value);
			}
			
			if (m_RunningEffectsPrevious.Count() != m_RunningEffects.Count())
			{
				UpdateVisibility();
			}
		}
	}
	
	override void RemoveActiveEffects(array<int> effects)
	{
		if (effects && effects.Count() > 0)
		{
			m_RunningEffectsPrevious.Copy(m_RunningEffects);
			
			int value;
			int idx;
			for (int i = 0; i < effects.Count(); i++)
			{
				value = effects.Get(i);
				idx = m_RunningEffects.Find(value);
				if (idx != -1)
				{
					m_RunningEffects.Remove(idx);
				}
			}
			
			if (m_RunningEffectsPrevious.Count() != m_RunningEffects.Count())
			{
				UpdateVisibility();
			}
		}
	}
	
	override void StopAllEffects()
	{
		m_Root.Show(false); //to avoid visual 'peeling'
		
		m_RunningEffectsPrevious.Copy(m_RunningEffects);
		m_RunningEffects.Clear();
		UpdateVisibility();
	}
	
	override bool IsAnyEffectRunning()
	{
		return m_RunningEffects && m_RunningEffects.Count() > 0;
	}
	
	override void AddSuspendRequest(int request_id)
	{
		m_SuspendRequests.Insert(request_id);
		UpdateVisibility();
	}
	
	override void RemoveSuspendRequest(int request_id)
	{
		int idx = m_SuspendRequests.Find(request_id);
		if (idx != -1)
		{
			m_SuspendRequests.Remove(idx);
		}
		UpdateVisibility();
	}
	
	override void ClearSuspendRequests()
	{
		m_SuspendRequests.Clear();
		UpdateVisibility();
	}
	
	override int GetSuspendRequestCount()
	{
		return m_SuspendRequests.Count();
	}
	
	//! Usually called in course of an OnFrame update, can be manually called from elsewhere with parameters
	override void UpdateWidgets(int type = -1, float timeSlice = 0, Param p = null, int handle = -1)
	{
		/*if (m_SuspendRequests.Count() > 0)
		{
			return;
		}*/
		
		GameplayEffectsData dta;
		array<ref Widget> widget_set;
		
		if (type == EffectWidgetsTypes.ROOT)
		{
			HandleWidgetRoot(timeSlice,p,handle);
		}
		else if (type == -1) //update stuff from the m_UpdatingEffects
		{
			int type_widgetset = 0;
			for (int i = 0; i < m_UpdatingEffects.Count(); i++)
			{
				if (m_RunningEffects.Find(m_UpdatingEffects.Get(i)) != -1)
				{
					type_widgetset = m_UpdatingEffects.Get(i);
					
					dta = m_WidgetDataMap.Get(type_widgetset);
					
					if (dta.HasDefinedHandle() && dta.DataInitialized())
					{
						dta.Update(timeSlice,p,handle); //calculate and apply
					}
					else
					{
						CalculateValues(type_widgetset,timeSlice,p,handle);
						widget_set = dta.GetWidgetSet();
						foreach (Widget w : widget_set)
						{
							if (w.IsVisibleHierarchy() && m_UpdatedWidgetsCheck.Find(w) == -1)
							{
								m_UpdatedWidgetsCheck.Insert(w);
								ProcessWidgetUpdate(w,type_widgetset,timeSlice,p,handle);
							}
						}
					}
				}
			}
		}
		else //update specific widget set
		{
			if (m_RunningEffects.Find(type) != -1) //only do if the effect is running (FPS stonks!)
			{
				dta = m_WidgetDataMap.Get(type);
				
				if (dta.HasDefinedHandle() && dta.DataInitialized())
				{
					dta.Update(timeSlice,p,handle); //calculate and apply
				}
				else
				{
					CalculateValues(type,timeSlice,p,handle);
					widget_set = dta.GetWidgetSet();
					foreach (Widget w2 : widget_set)
					{
						if (w2.IsVisibleHierarchy() && m_UpdatedWidgetsCheck.Find(w2) == -1)
						{
							m_UpdatedWidgetsCheck.Insert(w2);
							ProcessWidgetUpdate(w2,type,timeSlice,p,handle);
						}
					}
				}
			}
		}
		
		m_UpdatedWidgetsCheck.Clear();
		m_UpdatedWidgetSetsCheck.Clear();
	}
	
	//! Only one calculation per unique WidgetSet
	protected void CalculateValues(int type = -1, float timeSlice = 0, Param p = null, int handle = -1)
	{
		if (m_UpdatedWidgetSetsCheck.Find(m_WidgetDataMap.Get(type).GetWidgetSetID()) != -1)
		{
			//Print("skipped updating set ID " + m_WidgetDataMap.Get(type).GetWidgetSetID() + " | effect: " + type);
			return;
		}
		
		switch (type)
		{
			case EffectWidgetsTypes.MOTO_BREATH:
			case EffectWidgetsTypes.HELMET_BREATH:
			case EffectWidgetsTypes.MASK_BREATH:
			{
				CalculateBreathEffect(timeSlice);
			}
			break;
			
			case EffectWidgetsTypes.MOTO_OCCLUDER:
			case EffectWidgetsTypes.EYEPATCH_OCCLUDER:
			case EffectWidgetsTypes.HELMET_OCCLUDER:
			case EffectWidgetsTypes.MASK_OCCLUDER:
			{
				CalculateOccluderEffect(type,timeSlice,p,handle);
			}
			break;
			
			case EffectWidgetsTypes.COVER_FLASHBANG:
			{
				CalculateFlashbangEffect(type,timeSlice,p,handle);
			}
			break;
			
			default:
				return; //no need to calculate anything
			break;
		}
		
		m_UpdatedWidgetSetsCheck.Insert(m_WidgetDataMap.Get(type).GetWidgetSetID());
	}
	
	protected void ProcessWidgetUpdate(Widget w, int type, float timeSlice = 0, Param p = null, int handle = -1)
	{
		switch (type)
		{
			case EffectWidgetsTypes.MOTO_BREATH:
			case EffectWidgetsTypes.HELMET_BREATH:
			case EffectWidgetsTypes.MASK_BREATH:
			{
				UpdateBreathEffect(ImageWidget.Cast(w));
			}
			break;
			
			case EffectWidgetsTypes.MOTO_OCCLUDER:
			case EffectWidgetsTypes.EYEPATCH_OCCLUDER:
			case EffectWidgetsTypes.HELMET_OCCLUDER:
			case EffectWidgetsTypes.MASK_OCCLUDER:
			{
				UpdateOccluderEffect(ImageWidget.Cast(w),type,timeSlice,p,handle);
			}
			break;
			
			case EffectWidgetsTypes.COVER_FLASHBANG:
			{
				UpdateFlashbangEffect(ImageWidget.Cast(w));
			}
			break;
			
			default:
				//Print("---invalid widget type to update---");
			break;
		}
	}
	
	//-----------------------------------------
	//specific widget 'handlers'
	
	const float BREATH_HDR_MIN = 0.005; //dusk?
	const float BREATH_HDR_MAX = 1.0; //dark?
	const float BREATH_COLOR_MULT_MIN = 0.5;
	const float BREATH_COLOR_MULT_MAX = 0.8;
	
	//-----------------------------------------
	//Breath
	//-----------------------------------------
	protected void CalculateBreathEffect(float timeSlice = 0, int type = -1, Param p = null)
	{
		float modifier = Math.Lerp(0.25, 0.5, m_BreathResidue);
		float speed = timeSlice * modifier;
		m_BreathResidue -= speed;
		m_BreathResidue = Math.Clamp(m_BreathResidue,0,1);
		float residue_final = Math.Lerp(0, 0.7, m_BreathResidue);
		
		float hdr_mult;
		hdr_mult = GetSceneHDRMul(0);
		hdr_mult = Math.Clamp(hdr_mult,BREATH_HDR_MIN,BREATH_HDR_MAX);
		hdr_mult = Math.InverseLerp(BREATH_HDR_MIN,BREATH_HDR_MAX,hdr_mult);
		hdr_mult = Math.Lerp(BREATH_COLOR_MULT_MAX,BREATH_COLOR_MULT_MIN,hdr_mult);
		m_BreathColor = ARGBF(0.0,1.0 * hdr_mult,1.0 * hdr_mult,1.0 * hdr_mult); //grayscaling of the image
		
		
		m_BreathAlphaVal = Math.Lerp(m_BreathAlphaVal, residue_final, timeSlice);
	}
	
	protected void UpdateBreathEffect(ImageWidget w)
	{
		w.SetColor(m_BreathColor);
		w.SetAlpha(m_BreathAlphaVal);
	}
	
	//-----------------------------------------
	//Occluders
	//-----------------------------------------
	protected void CalculateOccluderEffect(int type, float timeSlice, Param p, int handle)
	{
	}
	
	protected void UpdateOccluderEffect(ImageWidget w, int type, float timeSlice, Param p, int handle)
	{
	}
	
	//-----------------------------------------
	//Flashbang
	//-----------------------------------------
	protected void CalculateFlashbangEffect(int type, float timeSlice, Param p, int handle)
	{
		Param1<float> par = Param1<float>.Cast(p);
		m_FlashbangCoverAlphaVal = par.param1;
		
		/*if (m_FlashbangCoverAlphaVal <= 0.0)
		{
			RemoveActiveEffects({EffectWidgetsTypes.COVER_FLASHBANG});
			return;
		}*/
	}
	
	protected void UpdateFlashbangEffect(ImageWidget w)
	{
		w.SetAlpha(1 - m_FlashbangCoverAlphaVal);
	}
	
	//-----------------------------------------
	//root handling (generic 'Widget', so mainly alpha and hiding?)
	//-----------------------------------------
	protected void HandleWidgetRoot(float timeSlice = 0, Param p = null, int handle = -1)
	{
		switch (handle)
		{
			default:
			{
				Param1<float> par = Param1<float>.Cast(p);
				if (par)
				{
					float alpha_mod = Math.Clamp(par.param1,0.0,1.0);
					//Print(alpha_mod);
					m_Root.SetAlpha(alpha_mod);
				}
			}
			break;
		}
	}
	
	//-----------------------------------------
	//! Generic update, called on frame from the player
	override void Update(float timeSlice)
	{
		if (m_SuspendRequests.Count() > 0)
		{
			return;
		}
		
		UpdateWidgets(-1,timeSlice);
	}
	
	/*
	override void SetBreathIntensityStamina(float stamina_cap, float stamina_current)
	{
		float stamina_normalized = Math.InverseLerp(0, stamina_cap, stamina_current);
		stamina_normalized = Math.Clamp(stamina_normalized,0,1);
		
		if ( stamina_normalized < STAMINA_SOUND_TR2 )
		{
			m_BreathMultStamina = 2.0;
		}
		else if ( stamina_normalized < STAMINA_SOUND_TR1 )
		{
			m_BreathMultStamina = 1.5;
		}
		else
		{
			m_BreathMultStamina = 1.0;
		}
	}
	*/
	override void OnVoiceEvent(float breathing_resistance01)
	{
		m_BreathResidue += Math.Lerp(0,0.35,breathing_resistance01);
		m_BreathResidue = Math.Clamp(m_BreathResidue,0,1);
	}
}
