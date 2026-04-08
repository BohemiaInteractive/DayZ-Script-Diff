/*!
\addtogroup Physics
\{
*/

/*!
Wrapper over a physics simulation of an entity. The entity is capable of the physics simulation only when such object
exists (see IEntity.GetPhysics). Two main types of physics bodies are static and dynamic. Static bodies are represented
by a collision object and dynamic bodies by a rigid body. This means certain methods make sense only when dealing with
a dynamic physics body, e.g., Physics.GetVelocity, Physics.ApplyImpulse.

Physics bodies rely on physics geometries to support collisions or casting. In general a physics body can be made out
of multiply physics geometries. Certain methods allow to work with a specific physics geometry (via geometry index).
*/
typedef int[] Physics;
class Physics
{
	static const float KMH2MS = 1 / 3.6; //<! Constant for converting km/h to m/s
	static const float MS2KMH = 3.6; //<! Constant for converting m/s to km/h
	static const float STANDARD_GRAVITY = 9.81;
	static const vector VGravity = "0 -9.81 0";

	//! Destroys the physics body
	proto external void Destroy();
	/*!
	Sets interaction layer mask of the physics body.
	\param mask Bit mask of layers
	*/
	proto external void SetInteractionLayer(int mask);
	//! Returns world space transformation of the physics body (including scale).
	proto external void GetWorldTransform(out vector mat[4]);
	//! Returns internal world space transformation of the physics body (excluding scale).
	proto external void GetDirectWorldTransform(out vector mat[4]);
	//! Sets interaction layer mask of the geometry element.
	proto external void SetGeomInteractionLayer(int index, int mask);
	//! Returns interaction layer mask of the physics body.
	proto external int GetInteractionLayer();
	//! Returns true if the physics body has enabled interaction with other body's layer mask
	proto external bool HasInteractionEnabled(int mask);
	//! Returns interaction layer mask of the geometry element.
	proto external int GetGeomInteractionLayer(int index);
	/*!
	Changes state of the continuous collision detection (CCD). If both maxMotion and
	shapeCastRadius is >=0 then the CCD is enabled. If you want to disable it, use -1.
	\param maxMotion Max motion threshold when sphere-cast is performed, to find time of impact. It should be
	little bit less than size of the geometry to catch the situation when tunneling can happen.
	\param sphereCastRadius The radius of the largest possible sphere, that is completely inside the body geometry.
	*/
	proto external void EnableCCD(float maxMotion, float sphereCastRadius);
	//! Changes simulation state of the physics body.
	proto external void ChangeSimulationState(SimulationState simState);
	//! Returns simulation state of the physics body.
	proto external SimulationState GetSimulationState();
	//! Returns center of mass offset of the physics body.
	proto external vector GetCenterOfMass();
	//! Changes activation state of the physics body.
	proto external void SetActive(ActiveState act);
	//! Returns whether the physics body is being simulated (awake).
	proto external bool IsActive();
	//! Returns whether the physics body is dynamic (a rigid body).
	proto external bool IsDynamic();
	//! Returns whether the physics body is kinematic (animated)
	proto external bool IsKinematic();
	//! Enables gravity on the physics body.
	proto external bool EnableGravity(bool enable);
	//! Changes damping parameters of the physics body.
	proto external void SetDamping(float linearDamping, float angularDamping);
	//! Returns mass of the physics body.
	proto external float GetMass();
	//! Changes mass of the physics body.
	proto external void SetMass(float mass);
	/*!
	Changes inertia tensor of the physics body.
	\param v Diagonal elements of the inertia tensor (a rigid body is aligned with the principle axes of inertia)
	*/
	proto external void SetInertiaTensorV(vector v);
	//! Returns angular velocity of the physics body.
	proto external vector GetAngularVelocity();
	//! Returns linear velocity of the physics body at a position (world space coordinates).
	proto external vector GetVelocityAt(vector pos);
	//! Changes thresholds used to determine whether the physics body can stop simulation.
	proto external void SetSleepingTreshold(float linearTreshold, float angularTreshold);
	//! Sets scale factor for all impulses/velocities/forces. Default is <1,1,1>. Zero any axis if you want to do 2D physics.
	proto external void SetLinearFactor(vector linearFactor);
	//! Returns linear velocity of the physics body.
	proto external vector GetVelocity();
	//! Sets linear velocity of the physics body.
	proto external void SetVelocity(vector velocity);
	//! Sets angular velocity of the physics body.
	proto external void SetAngularVelocity(vector velocity);
	/*!
	Sets target transformation. If timeslice == dt (simulation step delta time), it will happen in next step, otherwise in time = timeslice.
	*/
	proto external void SetTargetMatrix(vector matrix[4], float timeslice);
	//! Applies impulse on the physics body.
	proto external void ApplyImpulse(vector impulse);
	//! Applies impulse on the physics body at a position (world space coordinates).
	proto external void ApplyImpulseAt(vector pos, vector impulse);
	//! Applies constant force on the physics body (at origin).
	proto external void ApplyForce(vector force);
	//! Applies constant force on the physics body at a position (world space coordinates).
	proto external void ApplyForceAt(vector pos, vector force);
	//! Applies constant torque on the physics body.
	proto external void ApplyTorque(vector torque);
	//! Clear forces and torques on the physics body.
	proto external void ClearForces();
	//! Returns total force currently applied to the physics body.
	proto external vector GetTotalForce();
	//! Returns total torque currently applied to the physics body.
	proto external vector GetTotalTorque();
	/*!
	Changes index to response matrix, see "project settings/physics/interactions/response matrix".
	Usually this index is set by physics component property, but can be overriden at runtime.
	*/
	proto external void SetResponseIndex(int responseIndex);
	//! Returns actual index to response matrix.
	proto external int GetResponseIndex();
	/*!
	Adds new geometry and returns an index of the geometry or -1 when an error occurred
	\code
		PhysicsGeom geom = PhysicsGeom.CreateBox(Vector(1, 1, 1));
		vector frame[4];
		Math3D.MatrixIdentity4(frame);
		frame[3] = vector.Up;
		string material = "{D745FD8FC67DB26A}Common/Materials/Game/stone.gamemat";
		GetPhysics().AddGeom("Box", geom, frame, material, 0xffffffff);
	\endcode
	\param name Name of the element
	\param geom Physics geometry shape
	\param frame Local transformation
	\param material Material resource name
	\param interactionLayer Bit mask of layers geometry belongs to
	*/
	proto external int AddGeom(string name, PhysicsGeom geom, vector frame[4], string material, int interactionLayer);
	//! Finds a geometry by its name and returns its index or -1 if the geometry wasn't found
	proto external int GetGeom(string name);
	//! Returns number of geometries of the entity
	proto external int GetNumGeoms();
	//! Returns world space transformation of a geometry element
	proto external bool UpdateGeometries(int interactionLayerAnd = 0xffffffff, int interactionLayerOr = 0);
	//! Returns name of a geometry element
	proto external string GetGeomName(int index);
	//! Returns world space transformation of a geometry element
	proto external void GetGeomWorldTransform(int index, out vector mat[4]);
	//! Returns entity space transformation of a geometry element
	proto external void GetGeomTransform(int index, out vector mat[4]);
	//! Returns internal physics space transformation of a geometry element
	proto external void GetGeomDirectTransform(int index, out vector mat[4]);
	//! Returns world space position of a geometry element
	proto external vector GetGeomWorldPosition(int index);
	//! Returns entity space position of a geometry element
	proto external vector GetGeomPosition(int index);
	//! Returns internal physics space position of a geometry element
	proto external vector GetGeomDirectPosition(int index);
	//! Returns world bounds of a geometry element
	proto external void GetGeomWorldBounds(int index, out vector min, out vector max);
	//! Returns internal physics bounds of a geometry element (at scale 1)
	proto external void GetGeomDirectBounds(int index, out vector min, out vector max);
	//! Returns all surface properties of a geometry element
	proto external void GetGeomSurfaces(int index, notnull out array<SurfaceProperties> surfaces);
	//! Returns the number of bones
	proto external int GetNumBones();
	//! Returns the bone at the index
	proto external Physics GetBone(int index);
	/*!
	Creates a static physics body from geometry embedded in attached VObject. If there is not any, false is returned.
	\param ent Entity that will be associated with the physics body
	\param layerMask Bit mask of layers that is ANDed with layers in object geometry. Use 0xffffffff if you want to keep it unmodified
	*/
	static proto Physics CreateStatic(notnull IEntity ent, int layerMask = 0xffffffff);
	/*!
	Creates a static physics body from custom made geometries. The geometries are deleted when rigid body is destroyed.
	\code
		PhysicsGeom geom = PhysicsGeom.CreateBox(Vector(1, 1, 1));
		string material = "{D745FD8FC67DB26A}Common/Materials/Game/stone.gamemat";
		PhysicsGeomDef geoms[] = { PhysisGeomDef("Box", geom, material, 0xffffffff) };
		Physics.CreateStaticEx(this, geoms);
	\endcode
	\param ent Entity that will be associated with the physics body
	\param geoms Array of custom made geometries
	*/
	static proto Physics CreateStaticEx(notnull IEntity ent, PhysicsGeomDef geoms[]);
	/*!
	Creates a dynamic physics body from geometry embedded in attached VObject. If there is not any, false is returned.
	\param ent Entity that will be associated with the physics body
	\param mass Body mass
	\param layerMask Bit mask of layers that is ANDed with layers in object geometry. Use 0xffffffff if you want to keep it unmodified
	*/
	static proto Physics CreateDynamic(notnull IEntity ent, float mass, int layerMask = 0xffffffff);
	/*!
	Creates a dynamic physics body from custom made geometries. The geometries are deleted when rigid body is destroyed.
	\code
		PhysicsGeom geom = PhysicsGeom.CreateBox(Vector(1, 1, 1));
		string material = "{D745FD8FC67DB26A}Common/Materials/Game/stone.gamemat";
		PhysicsGeomDef geoms[] = { PhysicsGeomDef("", geom, material, 0xffffffff) };
		vector center = 2 * vector.Up;
		Physics.CreateDynamicEx(this, center, 1.0, geoms);
	\endcode
	\param ent Entity that will be associated with the physics body
	\param centerOfMass Offset from object-pivot to center of mass
	\param mass Body mass
	\param geoms Array of custom made geometries
	*/
	static proto Physics CreateDynamicEx(notnull IEntity ent, vector centerOfMass, float mass, PhysicsGeomDef geoms[]);
	/*!
	Creates a ghost physics body from custom made geometries. The geometries are deleted when rigid body is destroyed.
	\param ent Entity that will be associated with the physics body
	\param geoms Array of custom made geometries
	*/
	static proto Physics CreateGhostEx(notnull IEntity ent, PhysicsGeomDef geoms[]);
}

/*!
\}
*/
