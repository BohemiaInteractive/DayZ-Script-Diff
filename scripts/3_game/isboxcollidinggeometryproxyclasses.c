//! Class that holds parameters to feed into CGame.IsBoxCollidingGeometryProxy
class BoxCollidingParams
{
	/**
	\brief Set the parameters
	    \param center \p vector Center of box
		\param orientation \p vector Orientation of box
		\param edgeLength \p vector EdgeLength of box
		\param primaryType \p vector Primary geometry to check
		\param secondaryType \p vector Secondary geometry to check (if no backup is needed, fill in the same as for primaryType)
		\param fullComponentInfo \p vector When true gives information about every component and proxy in box, when false just the first component found
	*/
	proto native void SetParams(vector center, vector orientation, vector edgeLength, ObjIntersect primaryType, ObjIntersect secondaryType, bool fullComponentInfo);
}

//! ComponentInfo for BoxCollidingResult
class ComponentInfo
{
	int component;			//!< Component index
	vector componentMin; 	//!< Component min in world space
	vector componentMax; 	//!< Component max in world space
	vector componentCenter; //!< Component center in world space
	float componentRadius; 	//!< Radius of bounding box
}

//! Result for an object found in CGame.IsBoxCollidingGeometryProxy
class BoxCollidingResult
{
	Object obj;		//!< Object inside of box
	Object parent; 	//!< Parent of proxy object
	
	int hierLevel; 	//!< How deep in the proxy hierarchy are we
	int geomLevel; 	//!< What geometry was the object found in (will either be primaryType or secondaryType)
	
	ref ComponentInfoArray componentInfo; 	//!< Either the first component found or all of them depending on "fullComponentInfo"
	ref ProxyInfoArray proxyInfo; 			//!< Either the first proxy found or all of them depending on "fullComponentInfo"
}

typedef array<ref ComponentInfo> ComponentInfoArray;
typedef array<ref BoxCollidingResult> ProxyInfoArray;