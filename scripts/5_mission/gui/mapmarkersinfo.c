class MapMarkerTypes
{
	protected static ref map<int,string> m_MarkerTypes;
	
	static void Init()
	{
		m_MarkerTypes = new ref map<int,string>;
		
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_BORDER_CROSS,"\\DZ\\gear\\navigation\\data\\map_border_cross_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_BROADLEAF,"\\DZ\\gear\\navigation\\data\\map_broadleaf_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_CAMP,"\\DZ\\gear\\navigation\\data\\map_camp_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_FACTORY,"\\DZ\\gear\\navigation\\data\\map_factory_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_FIR,"\\DZ\\gear\\navigation\\data\\map_fir_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_FIREDEP,"\\DZ\\gear\\navigation\\data\\map_firedep_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_GOVOFFICE,"\\DZ\\gear\\navigation\\data\\map_govoffice_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_HILL,"\\DZ\\gear\\navigation\\data\\map_hill_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_MONUMENT,"\\DZ\\gear\\navigation\\data\\map_monument_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_PALM,"\\DZ\\gear\\navigation\\data\\map_palm_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_POLICE,"\\DZ\\gear\\navigation\\data\\map_police_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_STATION,"\\DZ\\gear\\navigation\\data\\map_station_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_STORE,"\\DZ\\gear\\navigation\\data\\map_store_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_TOURISM,"\\DZ\\gear\\navigation\\data\\map_tourism_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_TRANSMITTER,"\\DZ\\gear\\navigation\\data\\map_transmitter_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_TSHELTER,"\\DZ\\gear\\navigation\\data\\map_tshelter_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_TSIGN,"\\DZ\\gear\\navigation\\data\\map_tsign_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_VIEWPOINT,"\\DZ\\gear\\navigation\\data\\map_viewpoint_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_VINEYARD,"\\DZ\\gear\\navigation\\data\\map_vineyard_ca.paa");
		RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAP_WATERPUMP,"\\DZ\\gear\\navigation\\data\\map_waterpump_ca.paa");
	}
	
	static void RegisterMarkerType(int id, string path)
	{
		m_MarkerTypes.Set(id,path);
	}
	
	static string GetMarkerTypeFromID(int id)
	{
		return m_MarkerTypes.Get(id);
	}
}

enum eMapMarkerTypes
{
	MARKERTYPE_MAP_BORDER_CROSS = 0,
	MARKERTYPE_MAP_BROADLEAF,
	MARKERTYPE_MAP_CAMP,
	MARKERTYPE_MAP_FACTORY,
	MARKERTYPE_MAP_FIR,
	MARKERTYPE_MAP_FIREDEP,
	MARKERTYPE_MAP_GOVOFFICE,
	MARKERTYPE_MAP_HILL,
	MARKERTYPE_MAP_MONUMENT,
	MARKERTYPE_MAP_PALM,
	MARKERTYPE_MAP_POLICE,
	MARKERTYPE_MAP_STATION,
	MARKERTYPE_MAP_STORE,
	MARKERTYPE_MAP_TOURISM,
	MARKERTYPE_MAP_TRANSMITTER,
	MARKERTYPE_MAP_TSHELTER,
	MARKERTYPE_MAP_TSIGN,
	MARKERTYPE_MAP_VIEWPOINT,
	MARKERTYPE_MAP_VINEYARD,
	MARKERTYPE_MAP_WATERPUMP,
	MARKERTYPE_MAX
}