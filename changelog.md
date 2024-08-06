Version: 06-08-2024_3 Alpha
Changes:
- Removed Plugin. 

--------------------------------------------------------------------------------------

Version: 06-08-2024_2 Alpha
Changes:
- GLTFLoader is now just a buncha functions. 

--------------------------------------------------------------------------------------

Version: 06-08-2024_2 Alpha
Changes:
- Fixed GLTFLoader not loading texture correctly from .glb files.

--------------------------------------------------------------------------------------

Version: 06-08-2024_1 Alpha
Changes:
- Updated box2d which fixed body stopping to move after velocity becomes 0
- Made so transform position changes rigidbody position 
- Updated GLTF loader and moved to assets since its not really related to rendering
- Removed timestep.
- Removed prefab.
- Removed scriptable entity.
- Created  
    - IntVector2 
    - IntVector3 
    - IntVector4 
    - UIntVector2
    - UIntVector3
    - UIntVector4
    - Vector2    
    - Vector3    
    - Vector4    
    - Matrix2    
    - Matrix3    
    - Matrix4    
    - Quaternion 
        - Which are alias of glm types


--------------------------------------------------------------------------------------

Version: 26-07-2024_1 Alpha
Changes:
- Created Random class.
- Subdata editing for vertex buffer.

--------------------------------------------------------------------------------------

Version: 22-07-2024_1 Alpha
Changes:
- OpenGL 4.6 support.

--------------------------------------------------------------------------------------

Version: 20-07-2024_5 Alpha
Changes:
- Print error when window initialization fails.

--------------------------------------------------------------------------------------

Version: 20-07-2024_4 Alpha
Changes:
- Fixed typos.

--------------------------------------------------------------------------------------

Version: 20-07-2024_3 Alpha
Changes:
- Added resizeable, vsync minwidth and minheight flag to application specs.
- By default working directory is in assets folder.

--------------------------------------------------------------------------------------

Version: 20-07-2024_2 Alpha
Changes:
- Moved examples to different repo.  

--------------------------------------------------------------------------------------

Version: 20-07-2024_1 Alpha
Changes:
- "Fixed" an oversight.  

--------------------------------------------------------------------------------------

Version: 19-07-2024_1 Alpha
Changes:
- "Fixed" OpenGL 3.0 ES Framebuffer. i'll clean it up later.  
- Tried to fix web builds. Framebuffer is broken specifically on web builds and no mesh/quad is visible.

--------------------------------------------------------------------------------------

Version: 06-07-2024_3 Alpha
Changes:
- Removed FileSystemData  

--------------------------------------------------------------------------------------

Version: 06-07-2024_2 Alpha
Changes:
- Fixed oversight where working directory in FileSystem did not get updated  

--------------------------------------------------------------------------------------

Version: 06-07-2024_1 Alpha
Changes:
- Removed ImGui Panels from engine code 

--------------------------------------------------------------------------------------

Version: 02-07-2024_1 Alpha
Changes:
- File dialog support for linux 
    - Using gtk4 to create file dialog
- Removed level-editor. Instead a new project has started to create 2D level/map standard.

--------------------------------------------------------------------------------------

Version: 20-06-2024_1 Alpha
Changes:
- Bevy like system thingy idk check World.h/cpp
    - Instead of NativeSctiptComponent now u create components and systems like in bevy. Prefab need a bit of work to fully work.

--------------------------------------------------------------------------------------

Version: 15-06-2024_1 Alpha
Changes:
- Further mesh/gltf work and bug fix.
- Framebuffer temp bug fix.
    - Now has working depth buffer

--------------------------------------------------------------------------------------

Version: 14-06-2024_1 Alpha
Changes:
- Initial mesh/gltf work. 

--------------------------------------------------------------------------------------

Version: 25-05-2024_2 Alpha
Changes:
- Fixed infinite recusion when instantiating prefab that had root entity with a child. 

--------------------------------------------------------------------------------------

Version: 25-05-2024_1 Alpha
Changes:
- Added Prefab class which serializes in to Tabby Prefab Format (tbpf)

