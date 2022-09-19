class CreditsDepartmentElement extends CreditsElement
{
	protected TextWidget								m_DepartmentTitle;
	protected GridSpacerWidget							m_DepartmentSections;
	protected ref array<ref CreditsDepartmentSection>	m_DepartmentSectionEntries = new array<ref CreditsDepartmentSection>;
	
	void CreditsDepartmentElement( int index, Widget parent, JsonDataCreditsDepartment department_data )
	{
		m_Root					= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/credits/department_element/department_element.layout", parent );
		m_DepartmentTitle		= TextWidget.Cast( m_Root.FindAnyWidget( "department_title" ) );
		m_DepartmentSections	= GridSpacerWidget.Cast( m_Root.FindAnyWidget( "department_sections" ) );
		
		if( department_data.DepartmentName != "" )
		{
			m_DepartmentTitle.SetText( department_data.DepartmentName );
		}
		else
		{
			m_DepartmentTitle.Show( false );
			m_Root.FindAnyWidget( "SeparatorPanel" ).Show( false );
		}
		
		m_Root.SetSort( index );
		LoadDataAsync( department_data.Sections );
	}
	
	void LoadDataAsync( array<ref JsonDataCreditsSection> department_data )
	{
		for( int i = 1; i <= department_data.Count(); i++ )
		{
			ref CreditsDepartmentSection e = new CreditsDepartmentSection( i, m_DepartmentSections, department_data.Get( i - 1 ) );
			m_DepartmentSectionEntries.Insert( e );
		}
		
		m_DepartmentSections.Update();
	}
}

class CreditsDepartmentSection extends CreditsElement
{
	protected TextWidget			m_SectionTitle;
	protected WrapSpacerWidget		m_SectionSections;
	protected ref array<Widget>		m_SectionEntries = new array<Widget>;
	
	void CreditsDepartmentSection( int index, Widget parent, JsonDataCreditsSection section_data )
	{
		m_Root				= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/credits/department_element/department_section.layout", parent );
		m_SectionTitle		= TextWidget.Cast( m_Root.FindAnyWidget( "section_title" ) );
		m_SectionSections	= WrapSpacerWidget.Cast( m_Root.FindAnyWidget( "section_elements" ) );
		
		if( section_data.SectionName != "" )
		{
			m_SectionTitle.SetText( section_data.SectionName );
		}
		else
		{
			m_SectionTitle.Show( false );
			m_Root.FindAnyWidget( "SeparatorPanel" ).Show( false );
		}	
		
		LoadDataAsync( section_data.SectionLines );
		m_Root.SetSort( index );
	}
	
	void LoadDataAsync( array<string> section_data )
	{
		for( int i = 1; i <= section_data.Count(); i++ )
		{
			TextWidget w = TextWidget.Cast( GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/credits/department_element/department_section_element.layout", m_SectionSections ) );
			w.SetText( section_data.Get( i - 1 ) + " \n" );
			w.Update();
			m_Root.SetSort( i );
			m_SectionEntries.Insert( w );
		}
		
		m_SectionSections.Update();
	}
}