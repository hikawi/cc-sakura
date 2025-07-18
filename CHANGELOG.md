## v0.3 Changelog

### Additions

- Added an engine-wide collision checking system, using quadtrees and approximated collision checks.
- Added partial collision checks for approximating.
- Added collision signals.
- Added mouse-scene collision.
- Added animation curves (linear, ease-in, ease-out, ease-in-out). Well, technically linear has always been there.
- Added option to render a sprite scaled and rotated.
- Added a loading scene.
- Added a simple threading helper functions file.
- Added a logging to file mechanism.

### Removals

- Removed scene's blocking flags.

### Changes

- Fixed `hash_map_iterate` and `hash_set_iterate` iterating wrong.
- Fixed **capsule colliders** using nonstandard endpoints. Now they use the common standard endpoint definitions.
- Added `moved_colliders` into a scene for the engine to handle collisions with colliders that move during a physics tick.
- Changed `scene_destroy(Scene *)` to also **destroy all the colliders, sprites, moved_colliders, and the scene's data** if available. This is to simplify each scene's `ondestroy` callback.
