

projects = {"hello_opengl" , "hello_triangle"}

workspace "graphics_workspace"
   configurations { "Debug", "Release" }

for i=1 , #projects do 

project(projects[i])
   
   print("building project : " , projects[i])

   kind "ConsoleApp"
   language "C++"
   
   files { 
	projects[i] .. "/**.h", 
	projects[i] .. "/**.hpp" , 
	projects[i] .. "/**.cpp"
   }

   filter { "configurations:Debug" }
      defines { "DEBUG" }
      symbols "On"

   filter { "configurations:Release" }
      defines { "NDEBUG" }
      optimize "On"

end