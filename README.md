# ZenEngine
ZenEngine aims to be a simple engine. It was born out of me wanting an engine where I can use native code as well as scripting, plus me wanting to learn game engine development.
At the same time it aims to be simpler then for example Unreal Engine (where you can actually mix native code and scripts).

# Acknoledgements
Parts of the code are taken from the Hazel 2D project (even though this is a 3D engine) which you can find at: https://github.com/TheCherno/Hazel/ and is licensed under the APACHE 
license. Definitely check out the Hazel project https://hazelengine.com/ and Yan "TheCherno" Chernikov YouTube channel https://www.youtube.com/user/TheChernoProject.

# Building
Make sure to install CMake and Vulkan and add a `VULKAN_SDK` environment variable pointing to your Vulkan installation (in my case this is `C:\VulkanSDK\1.4.304.1`).

Clone the repository and the submodules using
```
git clone --recurse-submodules https://github.com/lucabtz/ZenEngine
```
this will take a while.

Create a `build` directory by using
```
cd ZenEngine
mkdir build
```

Run `cmake` using
```
cmake . -B build
```

Then build using Visual Studio opening `ZenEngineEcosystem.sln`. There are 4 possible build configurations:
- `Debug`
- `DebugEditor`
- `Release`
- `Editor`
Debug and Release are self-explanatory. Editor versions contain the game editor and currently it's the only thing that makes sense building. Ideally one would write the game logic, then build with the editor included the game code and edit the game scenes. Once the game scenes. If editing to the game logic is required one should change the code and build the editor again. Once the game is ready one builds the `Release` configuration.