=======
Modules
=======

Leggiero provides engine functions by modules.
Each modules are basically independent. Dependency should be declared explicitly by common interface.


Component and Module Interface
==============================

Leggiero formalizes the interface for engine modules as **Engine Component** and **Module Interface** .

Engine Component
  Engine component can have their own logical states. It is a component of the game program object entity.

Module Interface
  Module interface does not have a conceptual state. It is an interface that represents the system of APIs the module providing.

Each engine module can define zero, one, or more engine components and module interfaces.
