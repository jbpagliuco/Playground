Overview

Class Definitions:
- (Device) Renderer: Provides functions to actually draw things to the window. This is platform specific (Direct3D, OpenGL, etc.).
- Scene: A collection of all of the things that make the game world.
- SceneRenderer: Handles logic of how to render a scene. This class uses the device renderer to actually draw.