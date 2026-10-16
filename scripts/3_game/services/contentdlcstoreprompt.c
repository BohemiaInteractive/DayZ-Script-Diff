//! Informs the player that a DLC is missing and opens the platform store once the warning has been confirmed.
class ContentDLCStorePrompt : UIScriptedMenu
{
	protected static ref ContentDLCStorePrompt m_Instance;

	protected EDLCId m_DLCId;
	protected int m_DialogId;

	/*!
		Warns the player about the missing DLC, the store is opened once the warning is confirmed.
		\param dlcId ID of the DLC the player does not own
	*/
	static void WarnAndOpenStore(EDLCId dlcId)
	{
		if (!ContentDLC.IsValidDLCID(dlcId))
			return;

		// A previous warning may still be pending, in that case reuse its handler
		if (!m_Instance)
			m_Instance = new ContentDLCStorePrompt();

		m_Instance.ShowWarning(dlcId);
	}

	protected void ShowWarning(EDLCId dlcId)
	{
		m_DLCId = dlcId;
		m_DialogId = ErrorModuleHandler.CreateError(ErrorCategory.ConnectErrorClient, EConnectErrorClient.DLC_CHECK_FAILED);

		string name = g_Game.GetContentDLCService().GetLocalizedName(dlcId);
		string message = ErrorModuleHandler.GetClientMessage(ErrorCategory.ConnectErrorClient, EConnectErrorClient.DLC_CHECK_FAILED, name);
		string header = string.Format("%1 (%2)", "#server_browser_connecting_failed", ErrorModuleHandler.GetErrorHex(m_DialogId));

		g_Game.GetUIManager().ShowDialog(header, message, m_DialogId, DBT_OK, DBB_OK, DMT_EXCLAMATION, this);
	}

	//! Runs from the call queue, so the prompt is released outside of the modal callback
	protected static void OpenStore(EDLCId dlcId)
	{
		g_Game.GetContentDLCService().BuyDLC(dlcId);

		m_Instance = null;
	}

	override bool OnModalResult(Widget w, int x, int y, int code, int result)
	{
		super.OnModalResult(w, x, y, code, result);

		// Any way of closing the dialog means the player has been told about the missing DLC
		if (code == m_DialogId)
			g_Game.GetCallQueue(CALL_CATEGORY_GUI).Call(OpenStore, m_DLCId);

		return true;
	}
}
