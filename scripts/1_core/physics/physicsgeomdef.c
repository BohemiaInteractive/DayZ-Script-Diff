/*!
\addtogroup Physics
\{
*/

/*!
Geometry element properties
*/
class PhysicsGeomDef: Managed
{
	string		Name; //!< Name of the element
	dGeom	Geometry; //!< Physics geometry shape
	vector		Frame[4] = {Vector(1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1), Vector(0, 0, 0)}; //!< Local transformation
	int			ParentNode = -1; //!< Bone the element can be attached to
	string		MaterialName; //!< Material resource name
	int			LayerMask; //!< Bit mask of layers geometry belongs to

	void PhysicsGeomDef(string name, dGeom geom, string materialName, int layerMask)
	{
		Name = name;
		Geometry = geom;
		MaterialName = materialName;
		LayerMask = layerMask;
	}

}

/*!
\}
*/
