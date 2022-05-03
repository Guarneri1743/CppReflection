local solution_dir = "build/" .. _ACTION
function SetupIncludeDirs()
   includedirs {
      "src"
   }
end

function SetupSlotion()
   location(solution_dir)
   solution "TinyReflection"
      configurations {
         "Debug", 
         "Release"
      }

      platforms { "Win64" }
      warnings "Extra"
      floatingpoint "Fast"
      symbols "On"
      cppdialect "C++11"
      rtti "Off"
      characterset ("MBCS")

      configuration "Debug*"
         defines { "DEBUG", "_DEBUG" }
         targetdir ( solution_dir .. "lib/Debug" )

      configuration "Release*"
         defines { "NDEBUG"}
         optimize "Speed"
         targetdir ( solution_dir .. "lib/Release"  )

      filter { "platforms:Win*", "configurations:Debug*" }
         defines { "WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS", "NOMINMAX", "_REFL_GEN_OFF_" }
         system "Windows"
         architecture "x86_64"
         staticruntime "Off"
      
      filter { "platforms:Win*", "configurations:Release*" }
         defines { "WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS", "NOMINMAX", "_REFL_GEN_OFF_"  }
         system "Windows"
         architecture "x86_64"
         staticruntime "Off"

      filter { "system:windows", "action:vs*"}
    	 flags { "MultiProcessorCompile" }
end

function SetupExample()
   project "Example"
   kind "ConsoleApp"
   language "C++"

   files { 
      "src/*.*",
      "example/*.*"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end

function SetupReflGen()
   project "MetaGen"
   kind "ConsoleApp"
   language "C++"

   local generate_reflection_code =  "..\\..\\meta_gen\\bin\\meta_gen.exe ..\\..\\example\\main.cpp -- -I. $CFLAGS"

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")
      prebuildcommands
      {
         generate_reflection_code
      }

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
      prebuildcommands
      {
         generate_reflection_code
      }
end

SetupIncludeDirs()
SetupSlotion()
SetupExample()
SetupReflGen()