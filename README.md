<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/othneildrew/Best-README-Template">
    <img src="./Images/ContainerRender.PNG" width="400px">
  </a>

  <h3 align="center">OpenGL Renderer</h3>

  <p align="center">
    A simple OpenGL Renderer / Model Viewer
</div>

<!-- ABOUT THE PROJECT -->
## About The Project

This project is a basic renderer that can display simple .obj models like the backpack included in the repository. In the name of simplicity, it is a somewhat barebones renderer with several features that could be added such as vertex colors and more complex texturing/lighting. For now, it features the ability to load .obj models with options to flip the textures (must be changed before opening model), toggle backface culling, and change the camera speed.

Libraries used:
* OpenGL for rendering
* GLFW for window creation
* GLAD for OpenGL function pointers
* stb_image for texture loading
* glm for math functions
* IMGUI for UI
* ImGuiFileDialog for the file dialog
* ASSIMP for model loading
