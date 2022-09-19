//! Static component of PPE manager, used to hold the instance.
class PPEManagerStatic
{
	static ref PPEManager m_Manager;
	
	static void CreateManagerStatic()
	{
		if (m_Manager)
		{
			Debug.Log("PPEManagerStatic | CreateManagerStatic - PPEManager already exists");
			return;
		}
		
		m_Manager = new PPEManager;
	}
	
	static void DestroyManagerStatic()
	{
		if (m_Manager)
		{
			m_Manager.Cleanup();
			delete m_Manager;
		}
	}
	
	//! Returns the manager instance singleton
	static PPEManager GetPPEManager()
	{
		return m_Manager;
	}
}

/**
/brief Postprocess manager, responsible for updates, receiving, and re-distributing requester data to their respective destinations.
	/par Basic post process flow outline:
	Getting a registered 'PPERequester' instance from the 'PPERequesterBank'
	
	/par
	Launching the requester, either through an overriden 'Start' method, or custom method with some setters (both flag it as active and to be processed)
	
	/par On render update, PPEManager:
	Handles queued requester changes, re-distributes individual commands to material structure
		
		/par
		Updates the material/parameter structure and calculates the blend values
		
		/par
		Sets the final values via native functions (only called once per changed parameter - optimization stonks)
	
	/note Requester serves as a centralized platform for specific effec/group of effects. Although technically the direct commands to mat/param would be feasible, this allows for easier control of effect groups,
	/note and clearer command hierarchy (no value setters without clear parentage).
*/
class PPEManager extends Managed
{
	const int CAMERA_ID = 0;
	
	protected bool 									m_ManagerInitialized;
	protected ref map<int, ref PPEClassBase> 		m_PPEClassMap; //contains sorted postprocess classes, IDs in 'PostProcessEffectType' // <MaterialID,<material_class>>
	protected ref map<int, ref array<int>> 			m_PPEMaterialUpdateQueueMap; //multiple levels of update queues, to allow for multiple dependent updates during same frame (greedy?)
	protected ref array<int> 						m_UpdatedMaterials;
	protected ref array<ref PPERequesterBase> 		m_ExistingPostprocessRequests; //which requests are active overall. Does not have to be updating ATM!
	protected ref array<ref PPERequesterBase> 		m_UpdatingRequests; //which requests are currently updating and processing
	
	void PPEManager()
	{
		m_ManagerInitialized = false;
		PPERequesterBank.Init();
	}
	
	void Cleanup()
	{
		PPERequesterBank.Cleanup();
		
		if (m_ManagerInitialized)
		{
			m_PPEMaterialUpdateQueueMap.Clear();
			m_ExistingPostprocessRequests.Clear();
			m_UpdatingRequests.Clear();
			m_PPEClassMap.Clear();
		}
	}
	
	//! Launched from 'DayZGame.DeferredInit' to make earlier access, use, and updates impossible (downside of a non-static system)
	void Init()
	{
		//DbgPrnt("PPEDebug | PPEManager | m_ManagerInitialized: " + m_ManagerInitialized);
		if (!m_ManagerInitialized)
		{
			m_PPEMaterialUpdateQueueMap = new map<int, ref array<int>>;
			m_UpdatedMaterials = new array<int>;
			m_ExistingPostprocessRequests = new array<ref PPERequesterBase>;
			m_UpdatingRequests = new array<ref PPERequesterBase>;
			InitPPEManagerClassMap();
			
			GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Insert(this.Update); //can be safely and easily 'disabled' here
			m_ManagerInitialized = true;
		}
	}
	
	//! Ordered by 'PostProcessEffectType' for easy access through the same enum; ID saved all the same
	protected void InitPPEManagerClassMap()
	{
		if (m_PPEClassMap)
		{
			delete m_PPEClassMap;
		}
		m_PPEClassMap = new map<int, ref PPEClassBase>;
		
		RegisterPPEClass(new PPENone()); //dummy
		RegisterPPEClass(new PPEUnderWater());
		RegisterPPEClass(new PPESSAO());
		RegisterPPEClass(new PPEDepthOfField());
		RegisterPPEClass(new PPEHBAO());
		RegisterPPEClass(new PPERotBlur());
		RegisterPPEClass(new PPEGodRays());
		RegisterPPEClass(new PPERain());
		RegisterPPEClass(new PPEFilmGrain());
		RegisterPPEClass(new PPERadialBlur());
		RegisterPPEClass(new PPEChromAber());
		RegisterPPEClass(new PPEWetDistort());
		RegisterPPEClass(new PPEDynamicBlur());
		RegisterPPEClass(new PPEColorGrading());
		RegisterPPEClass(new PPEColors());
		RegisterPPEClass(new PPEGlow());
		RegisterPPEClass(new PPESMAA());
		RegisterPPEClass(new PPEFXAA());
		RegisterPPEClass(new PPEMedian());
		RegisterPPEClass(new PPESunMask());
		RegisterPPEClass(new PPEGaussFilter());
		RegisterPPEClass(new PPEExposureNative());
		RegisterPPEClass(new PPEEyeAccomodationNative());
		RegisterPPEClass(new PPEDOF());
		RegisterPPEClass(new PPELightIntensityParamsNative());
	}
	
	//! Registeres material class and creates data structure within
	protected void RegisterPPEClass(PPEClassBase material_class)
	{
		m_PPEClassMap.Set(material_class.GetPostProcessEffectID(), material_class);
	}
	
	void SendMaterialValueData(PPERequestParamDataBase data)
	{
		//DbgPrnt("DataVerification | m_ColorTarget | SendMaterialValueData: " + PPERequestParamDataColor.Cast(data).m_ColorTarget[0] + "/" + PPERequestParamDataColor.Cast(data).m_ColorTarget[1] + "/" + PPERequestParamDataColor.Cast(data).m_ColorTarget[2] + "/" + PPERequestParamDataColor.Cast(data).m_ColorTarget[3]);
		PPEClassBase mat_class = m_PPEClassMap.Get(data.GetMaterialID());
		mat_class.InsertParamValueData(data);
		SetMaterialParamUpdating(data.GetMaterialID(),data.GetParameterID(),PPEConstants.DEPENDENCY_ORDER_BASE);
	}
	
	//! Queues material/parameter to update (once)
	void SetMaterialParamUpdating(int material_id, int parameter_id, int order)
	{
		if ( order > PPEConstants.DEPENDENCY_ORDER_HIGHEST )
		{
			//DbgPrnt("PPEDebug | PPEManager - SetMaterialParamUpdating | Order higher than max, ignoring! | mat/par/ord: " + material_id + "/" + parameter_id + "/" + order);
			return;
		}
		
		PPEClassBase mat_class = m_PPEClassMap.Get(material_id);
		
		//DbgPrnt("PPEDebug | PPEManager - SetMaterialParamUpdating | mat/par: " + material_id + "/" + parameter_id);
		//insert material into queue
		if ( !m_PPEMaterialUpdateQueueMap.Contains(order) )
			m_PPEMaterialUpdateQueueMap.Set(order,new array<int>);
		
		int found = m_PPEMaterialUpdateQueueMap.Get(order).Find(material_id);
		if ( found == -1 )
		{
			m_PPEMaterialUpdateQueueMap.Get(order).Insert(material_id);
		}
		
		mat_class.SetParameterUpdating(order,parameter_id);
	}
	
	//! Currently unused, requests remain in the hierarchy and are used when needed (slightly faster than constantly re-shuffilng the arrays)
	void RemoveMaterialUpdating(int material_id, int order = 0)
	{
		if ( m_PPEMaterialUpdateQueueMap.Contains(order) )
		{
			m_PPEMaterialUpdateQueueMap.Get(order).RemoveItem(material_id);
			
			if ( m_PPEMaterialUpdateQueueMap.Get(order).Count() == 0)
				m_PPEMaterialUpdateQueueMap.Remove(order);
		}
	}
	
	protected void ClearMaterialUpdating()
	{
		m_PPEMaterialUpdateQueueMap.Clear();
	}
	
	//! Marks requester as 'active'. Currently indistinguiishable from 'updating' requester, can potentially be used for intermittently updated requesters
	void SetRequestActive(PPERequesterBase request, bool active)
	{
		int found = m_ExistingPostprocessRequests.Find(request);
		if ( active && found == -1 )
		{
			m_ExistingPostprocessRequests.Insert(request);
		}
		else if ( !active && found > -1 ) //should always be found in this case, redundant?
		{
			//RemoveActiveRequestFromMaterials(request);
			
			m_ExistingPostprocessRequests.Remove(found);
		}
	}
	
	//! Marks requester as 'updating' and to be processed on manager update
	void SetRequestUpdating(PPERequesterBase request, bool active)
	{
		if (!m_UpdatingRequests)
		{
			Debug.Log("PPEManager | SetRequestUpdating | !m_UpdatingRequests");
			return;
		}
		
		int idx = m_UpdatingRequests.Find(request);
		
		if ( active && idx == -1 )
		{
			m_UpdatingRequests.Insert(request);
		}
		else if ( !active && idx > -1 )
		{
			m_UpdatingRequests.Remove(idx);
		}
	}
	
	// Just a getter
	bool GetExistingRequester(typename req, out PPERequesterBase ret)
	{
		int idx = m_ExistingPostprocessRequests.Find(PPERequesterBank.GetRequester(req));
		if (idx > -1)
		{
			ret = m_ExistingPostprocessRequests.Get(idx);
			return true;
		}
		return false;
	}
	
	/**
	/brief Originally designed to rip the requester data from all relevant mat/params, but that proved too costly and volatile.
	/note Still, it is here, use at your own peril.
	*/
	protected void RemoveActiveRequestFromMaterials(PPERequesterBase req)
	{
		int count = req.GetActiveRequestStructure().Count();
		int mat_id;
		for (int i = 0; i < count; i++)
		{
			mat_id = req.GetActiveRequestStructure().GetKey(i);
			PPEClassBase mat_class = m_PPEClassMap.Get(mat_id);
			mat_class.RemoveRequest(req.GetRequesterIDX());
		}
	}
	
	//! Unused cleanup method, should it be ever needed
	protected void RequestsCleanup()
	{
	}
	
	//! Marks material class as updated and values to be set in the course of update - 'ProcessApplyValueChanges'
	void InsertUpdatedMaterial(int mat_id)
	{
		if ( m_UpdatedMaterials.Find(mat_id) == -1 )
			m_UpdatedMaterials.Insert(mat_id);
	}
	
	//---------//
	//PROCESSING
	//---------//
	protected void ProcessRequesterUpdates(float timeslice)
	{
		PPERequesterBase req;
		for (int i = 0; i < m_UpdatingRequests.Count(); i++)
		{
			//DbgPrnt("PPEDebug | ProcessRequesterUpdates | m_UpdatingRequests[i]: " + m_UpdatingRequests[i]);
			req = m_UpdatingRequests.Get(i);
			if (req)
				req.OnUpdate(timeslice);
		}
	}
	
	protected void ProcessMaterialUpdates(float timeslice)
	{
		for (int i = 0; i < m_PPEMaterialUpdateQueueMap.Count(); i++) //orders (levels?)
		{
			//DbgPrnt("PPEDebug | ProcessMaterialUpdates | GetKey " + i + ": " + m_PPEMaterialUpdateQueueMap.GetKey(i));
			//DbgPrnt("PPEDebug | ProcessMaterialUpdates | GetElement - count " + i + ": " + m_PPEMaterialUpdateQueueMap.GetElement(i).Count());
			
			for (int j = 0; j < m_PPEMaterialUpdateQueueMap.GetElement(i).Count(); j++)
			{
				PPEClassBase mat_class = m_PPEClassMap.Get(m_PPEMaterialUpdateQueueMap.GetElement(i).Get(j));
				mat_class.OnUpdate(timeslice,i);
			}
		}
	}
	
	protected void ProcessApplyValueChanges()
	{
		int material_id;
		for (int i = 0; i < m_UpdatedMaterials.Count(); i++)
		{
			material_id = m_UpdatedMaterials.Get(i);
			PPEClassBase mat_class = m_PPEClassMap.Get(material_id);
			mat_class.ApplyValueChanges();
		}
		
		m_UpdatedMaterials.Clear();
		ClearMaterialUpdating();
	}
	
	void Update(float timeslice)
	{
		if (!m_ManagerInitialized)
			return;
		
		ProcessRequesterUpdates(timeslice);
		ProcessMaterialUpdates(timeslice);
		ProcessApplyValueChanges();
		RequestsCleanup(); //unused
	}
	
	//! Returns default values as Param. See 'PPEConstants' file for various typedefs used
	Param GetPostProcessDefaultValues(int material, int parameter)
	{
		PPEClassBase mat_class = m_PPEClassMap.Get(material);
		return mat_class.GetParameterCommandData(parameter).GetDefaultValues();
	}
	
	//! Returns current values as Param. See 'PPEConstants' file for various typedefs used
	Param GetPostProcessCurrentValues(int material, int parameter)
	{
		PPEClassBase mat_class = m_PPEClassMap.Get(material);
		return mat_class.GetParameterCommandData(parameter).GetCurrentValues();
	}
	
	//TODO - certain C++ events may change the actual material path with a graphics option changes. Reflect this on script-side!
	//Currently only SSAY/HBAO affected...welp.
	//! Changes material file associated with the script material class. Will be used very rarely, mostly set in C++ anyway.
	void ChangePPEMaterial(PostProcessPrioritiesCamera priority, PostProcessEffectType type, string path, bool scriptside_only)
	{
		if (m_PPEClassMap.Contains(type))
		{
			PPEClassBase mat_class = m_PPEClassMap.Get(type);
			typename name = mat_class.Type();
			PPEClassBase postprocess_capsule = PPEClassBase.Cast(name.Spawn());
			postprocess_capsule.ChangeMaterialPathUsed(path);
			
			if (postprocess_capsule.GetMaterial() == 0x0)
			{
				Debug.Log("PPEManager | Invalid material path " + path + " used for " + name );
				return;
			}
			
			//m_PPEClassMap.Remove(type);
			m_PPEClassMap.Set(type,postprocess_capsule);
		}
		
		//can be sent script-side only to adapt to c++ options changes
		if (!scriptside_only)
			SetCameraPostProcessEffect(CAMERA_ID,priority,type,path);
	}
	
	//! stops all effects of a certain kind
	void StopAllEffects(int mask = 0)
	{
		if (m_ExistingPostprocessRequests)
		{		
			foreach (PPERequesterBase requester : m_ExistingPostprocessRequests)
			{
				if (requester.GetCategoryMask() & mask)
				{
					requester.Stop();
				}
			}
		}
	}
	
	void DbgPrnt(string text)
	{
		//Debug.Log(""+text);
	}
};
