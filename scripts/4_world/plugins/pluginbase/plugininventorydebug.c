// *************************************************************************************
// ! PluginInventoryDebug  
// *************************************************************************************
#ifdef DIAG_DEVELOPER
class PluginInventoryDebug extends PluginBase
{
	// Inventory movement is not send to server
	bool m_IsOnlyLocalMoveEnable	= false;
	// For hand manipulation client wait for ack from server
	bool m_IsHandAckEnable			= false;
	// Repair desync system, if disable server will not send or respond on repair request
	bool m_IsDesyncRepairEnable 	= true;
	// Pre split inventory handshake
	bool m_IsOldProcessInputDataEnable 	= false;

	//---------------------------------------------------
    // gui stuff

	void PluginInventoryDebug()
	{
		m_IsOnlyLocalMoveEnable = false;
		m_IsHandAckEnable = false;
		m_IsDesyncRepairEnable = true;
		m_IsOldProcessInputDataEnable = false;
	}

	bool IsLocalOnlyMoveEnable()
	{
		return m_IsOnlyLocalMoveEnable;
	}

	void SetLocalOnlyMoveEnable(bool enable)
	{
		m_IsOnlyLocalMoveEnable = enable;
	}
	
	void SetHandAckEnable(bool enable)
	{
		m_IsHandAckEnable = enable;
		InventoryDebug.SetHandAckEnable(enable);
	}
	
	bool IsHandAckEnable()
	{
		return m_IsHandAckEnable;
	}
	
	void SetDesyncRepairEnable(bool enable)
	{
		m_IsDesyncRepairEnable = enable;
	}
	
	bool IsDesyncRepairEnable()
	{
		return m_IsDesyncRepairEnable;
	}
	
	void SetOldProcessInputDataEnable(bool enable)
	{
		m_IsOldProcessInputDataEnable = enable;
	}
	
	bool IsOldProcessInputDataEnable()
	{
		return m_IsOldProcessInputDataEnable;
	}
}
#endif