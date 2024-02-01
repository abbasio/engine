NOTE: Readme is currently WIP

This repo contains the code for a 2D engine, built with C++ and SDL2. This engine uses Entity Component System (ECS) architecture. The basic implementation is as follows:
  - Entities are simply numeric IDs
  - Components are pieces of data, with no logic
  - Systems dictate the logic for how these interact

Essentially, entities will have components associated with them using a component signature. Systems will check for entities with specific component signatures, and perform logic on them.

The Registry class handles organizing these pieces together. You can see details of the implementation under src/ECS.
