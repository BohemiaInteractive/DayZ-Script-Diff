class BloodContainerBase extends ItemBase
{
	private bool m_IsBloodTypeVisible = false;
	
	void BloodContainerBase()
	{
		RegisterNetSyncVariableBool("m_IsBloodTypeVisible");
	}
	
	override bool IsBloodContainer()
	{
		return true;
	}
	
	void SetBloodTypeVisible( bool visible )
	{
		m_IsBloodTypeVisible = visible;
		SetSynchDirty();
	}
	
	bool GetBloodTypeVisible()
	{
		return m_IsBloodTypeVisible;
	}	
	
	override void OnStoreSave( ParamsWriteContext ctx )
	{
		super.OnStoreSave(ctx);

		ctx.Write( m_IsBloodTypeVisible );
	}

	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad(ctx, version) )
			return false;

		bool is_blood_type_visible = false;
		ctx.Read( is_blood_type_visible );
		SetBloodTypeVisible( is_blood_type_visible );
		
		return true;
	}
}