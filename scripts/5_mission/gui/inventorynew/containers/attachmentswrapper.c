class AttachmentsWrapper: AttachmentsGroupContainer
{
	Attachments m_Attachments;
	protected Widget m_AttachmentsFalseHeader;
	protected RichTextWidget m_AttachmentsFalseHeaderText;
	
	void AttachmentsWrapper(LayoutHolder parent) 
	{
		m_AttachmentsFalseHeader 		= m_RootWidget.FindAnyWidget( "attachmets_wrapper_header_spacer" );
		m_AttachmentsFalseHeaderText 	= RichTextWidget.Cast(m_AttachmentsFalseHeader.FindAnyWidget("TextWidget0"));
		m_AttachmentsFalseHeader.Show( false );
	}
	
	void SetParent( Attachments atts )
	{
		m_Attachments = atts;
	}
	
	override bool Select()
	{
		return m_Attachments.Select();
	}
	
	override bool SelectItem()
	{
		return m_Attachments.SelectItem();
	}
	
	override bool Combine()
	{
		return m_Attachments.Combine();
	}
	
	override bool TransferItemToVicinity()
	{
		return m_Attachments.TransferItemToVicinity();
	}
	
	override bool TransferItem()
	{
		return m_Attachments.TransferItem();
	}
	
	override bool InspectItem()
	{
		return m_Attachments.InspectItem();
	}
	
	override bool SplitItem()
	{
		return m_Attachments.SplitItem();
	}
	
	override bool EquipItem()
	{
		return m_Attachments.EquipItem();
	}
	
	override bool CanEquip()
	{
		return m_Attachments.CanEquip();
	}
	
	override bool CanCombine()
	{
		return m_Attachments.CanCombine();
	}
	
	override bool CanCombineAmmo()
	{
		return m_Attachments.CanCombineAmmo();
	}
	
	override bool IsDisplayable()
	{
		return !IsEmpty();
	}
	
	override bool IsItemActive()
	{
		return m_Attachments.IsItemActive();
	}

	override bool IsItemWithQuantityActive()
	{
		return m_Attachments.IsItemWithQuantityActive();
	}
	
	override void UpdateInterval()
	{
		super.UpdateInterval();
		m_Attachments.UpdateInterval();
	}
	
	int GetFocusedSlot()
	{
		return m_Attachments.GetFocusedSlot();
	}
	
	void ShowFalseAttachmentsHeader(bool show)
	{
		m_AttachmentsFalseHeader.Show(show);
	}
	
	void SetFalseAttachmentsHeaderText(string text)
	{
		m_AttachmentsFalseHeaderText.SetText(text);
	}
	
	TextWidget GetFalseHeaderTextWidget()
	{
		return m_AttachmentsFalseHeaderText;
	}
}
