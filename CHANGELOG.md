### Additions

- Added a generic hash map for generic keys and values (`misc/genhashmap.h`).
- Added a settings struct.
- Added debug rendering for colliders.
- Added a window icon and executable icon for MacOS. Windows and Linux still researching.

### Removals

- Removed the file logging service. Console is enough.

### Changes

- Fixed the font path not using the correct base path.
- Sprite rendering system was rewritten to be more verbose with errors and more portable.
- Converted normal file system into proper SDL storage.
- Added a minimum time duration for a loading scene to prevent it snapping too hard.
- Changed the name for `font_engine_*` functions to `text_*`, since it is shorter and we don't even use TTF's font engine.
