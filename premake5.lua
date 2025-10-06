workspace "RavaFramework"
	architecture "x64"
	startproject "RavaMain"

	configurations{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}"
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"]		= "Externals/GLFW/include"
IncludeDir["glm"]		= "Externals/glm"

LibDir = {}
LibDir["Vulkan"]	= "Externals/Vulkan/lib"

project "RavaFramework"
	location "RavaFramework"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "off"

	debugdir "$(SolutionDir)"

	pchheader "RavaFramework.h"
	pchsource "%{prj.name}/src/RavaFramework.cpp"

	targetdir ("bin/" ..outputdir.. "/%{prj.name}")
	objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c",
	}
	
	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
	}

	includedirs {
		"%{prj.name}/src",
		"%{VULKAN_SDK}/Include",
		"Externals",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
	}
	
	libdirs {
		"%{VULKAN_SDK}/Lib",
	}

	links {
		"vulkan-1.lib",
		"GLFW",
	}

	postbuildcommands {
		-- "call $(SolutionDir)CompileShaders.bat",
	}

	buildoptions {
		"/utf-8",
	}

	filter"system:windows"
		systemversion "latest"
		
	filter "configurations:Debug"
		defines "RV_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines {"RV_RELEASE", "NDEBUG"}
		runtime "Release"
		optimize "on"
		
project "RavaMain"
	location "RavaMain"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++latest"
	staticruntime "off"

	targetdir ("bin/" ..outputdir.. "/%{prj.name}")
	objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c",
		"%{prj.name}/**.rc",
	}

	includedirs {
		"%{prj.name}/src",
		"RavaFramework/src",
		"%{VULKAN_SDK}/Include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
	}

	libdirs {
		"%{VULKAN_SDK}/Lib",
	}

	links {
		"vulkan-1.lib",
		"GLFW",
	}


	links{
		"RavaFramework",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "RV_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RV_RELEASE"
		runtime "Release"
		optimize "on"

group "Externals"
		include "Externals/GLFW.lua"