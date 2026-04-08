class InventoryJunctureSwapValidation
{
	bool m_Success;
	bool m_SkippedSwap;
	
	ref InventoryLocation m_Src1; 
	ref InventoryLocation m_Src2;
	
	EntityAI m_SrcItem1;
	EntityAI m_SrcItem2;
	
	ref InventoryLocation m_Dst1;
	ref InventoryLocation m_Dst2;
	
	EntityAI m_DstItem1;
	EntityAI m_DstItem2;
	
	void InventoryJunctureSwapValidation(Serializer ctx)
	{		
		m_Success = true;
		m_SkippedSwap = false;
		
		m_Src1 = new InventoryLocation;
		m_Src2 = new InventoryLocation;
		
		m_Dst1 = new InventoryLocation;
		m_Dst2 = new InventoryLocation;
		
		m_Src1.ReadFromContext(ctx);
		m_Src2.ReadFromContext(ctx);
		m_Dst1.ReadFromContext(ctx);
		m_Dst2.ReadFromContext(ctx);
		ctx.Read(m_SkippedSwap);
		
		m_SrcItem1 = m_Src1.GetItem();
		m_SrcItem2 = m_Src2.GetItem();
		
		m_DstItem1 = m_Dst1.GetItem();
		m_DstItem2 = m_Dst2.GetItem();
	}
}