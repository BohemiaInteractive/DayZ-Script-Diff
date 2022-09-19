
//typedef Param3<string, string, int> 			WritePaperParams; //message, pen, font
//typedef Param1<array<ref WritePaperParams>> 	PaperParams;
//-----------------------

class Paper extends ItemBase
{
	protected ref WrittenNoteData m_NoteContents;
	
	void Paper()
	{
		m_NoteContents = new WrittenNoteData(this);
	}
	
	void ~Paper()
	{
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{   
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		if (version >= 108 && !ctx.Read(m_NoteContents))
			return false;
				
		return true;
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_NoteContents);		
	}
	
	override WrittenNoteData GetWrittenNoteData()
	{
		return m_NoteContents;
	}
	
	//================================================================
	// IGNITION ACTION
	//================================================================
	override bool HasFlammableMaterial()
	{
		return true;
	}
	
	override bool CanBeIgnitedBy( EntityAI igniter = NULL )
	{
		if ( GetHierarchyParent() ) return false;
		
		return true;
	}
	
	override bool CanIgniteItem( EntityAI ignite_target = NULL )
	{
		return false;
	}
	
	override void OnIgnitedTarget( EntityAI ignited_item )
	{
	}
	
	override void OnIgnitedThis( EntityAI fire_source )
	{
		Fireplace.IgniteEntityAsFireplace( this, fire_source );
	}

	override bool IsThisIgnitionSuccessful( EntityAI item_source = NULL )
	{
		return Fireplace.CanIgniteEntityAsFireplace( this );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		//AddAction(ActionReadPaper);
		//AddAction(ActionWritePaper);

		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
		AddAction(ActionAttach);
		AddAction(ActionDetach);
	}
}
