
//! Static part of the construction system, for global handling mostly
class ConstructionManagerStatic
{
	protected static const int UPDATE_QUEUE_BATCH_SIZE_DEFAULT = 15; //parts processed in a single simulation step
	
	protected static bool m_Initialized;
	protected static bool m_IsServerBatchProcessing; //is server?
	protected static ScriptInvoker m_UpdateQueue;
	protected static ref map<ConstructionBase, ref array<ConstructionPart>> m_QueueConstructionMap;
	protected static ref array<ConstructionBase> m_CleanupQueue;
	
	static void Init()
	{
		if (!m_Initialized)
		{
			m_UpdateQueue = g_Game.GetUpdateQueue(CALL_CATEGORY_SYSTEM);
			m_IsServerBatchProcessing = g_Game.IsServer() || !g_Game.IsMultiplayer();
			m_QueueConstructionMap = new map<ConstructionBase, ref array<ConstructionPart>>();
			m_CleanupQueue = new array<ConstructionBase>();
			m_Initialized = true;
		}
	}
	
	//============================================
	// Batching and processing
	//============================================
	
	///////////////////////
	//API
	
	static void AddPartToProcess(ConstructionBase con, ConstructionPart part)
	{
		if (!m_Initialized)
			Init();
		
		array<ConstructionPart> tmp;
		if (!m_QueueConstructionMap.Find(con, tmp))
			tmp = new array<ConstructionPart>();
		
		if (tmp.Find(part) == -1)
			tmp.Insert(part);
		
		m_QueueConstructionMap.Set(con, tmp);
		
		if (m_UpdateQueue.Count(ProcessUpdateQueues) == 0)
			m_UpdateQueue.Insert(ProcessUpdateQueues);
	}
	
	//! Unremoved objects can become NULL
	static void RemoveFromProcessing(ConstructionBase con)
	{
		if (m_QueueConstructionMap.Contains(con))
			m_QueueConstructionMap.Remove(con);
	}
	
	///////////////////////
	//Internals
	
	protected static void ProcessUpdateQueues()
	{
		int batchCounter = UPDATE_QUEUE_BATCH_SIZE_DEFAULT;
		int count;
		
		foreach (ConstructionBase con, array<ConstructionPart> updatePartsArray : m_QueueConstructionMap)
		{
			if (batchCounter == 0)
				break;
			
			count = updatePartsArray.Count();
			for (int i = count - 1; i > -1; --i)
			{
				if (batchCounter == 0)
					break;
				
				ConstructionPart part = updatePartsArray[i];
				con.UpdateSpecificPartPhysics(part);
				if (m_IsServerBatchProcessing)
					con.UpdateSpecificPartVisuals(part);
				
				updatePartsArray.Remove(i);
				--batchCounter;
				
				if (updatePartsArray.Count() == 0)
				{
					m_CleanupQueue.Insert(con);
					break;
				}
			}
		}
		
		foreach (ConstructionBase cleanupCon : m_CleanupQueue)
		{
			if (m_IsServerBatchProcessing)
				cleanupCon.UpdateNavmesh();
			m_QueueConstructionMap.Remove(cleanupCon);
		}
		
		m_CleanupQueue.Clear();
		
		if (m_QueueConstructionMap.Count() == 0 && m_UpdateQueue.Count(ProcessUpdateQueues) > 0)
		{
			m_UpdateQueue.Remove(ProcessUpdateQueues);
			m_QueueConstructionMap.Clear();
		}
	}
}
