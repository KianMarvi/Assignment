#ifndef RENDER_HELPER_H
#define RENDER_HELPER_H

#include "Vertex.h"

class Mesh;

class RenderHelper
{
private:
	static bool bLightEnable;
	static bool bColorTextureEnabled;
	static bool bColorTexture;
	static bool bTextEnabled;

public:
	// Pre Render Mesh to setup the shaders before rendering a mesh without light
	static void PreRenderMesh(const bool bLightEnable = false, const bool bColorTextureEnabled = true, const bool bColorTexture = false, const bool bTextEnabled = false);
	// Render Mesh to render a mesh without light
	static void RenderMesh(Mesh* _mesh);
	// Post Render Mesh to setup the shaders after rendering a mesh without light
	static void PostRenderMesh(const bool bLightEnable = true, const bool bColorTextureEnabled = false, const bool bColorTexture = true, const bool bTextEnabled = false);

	// Pre Render Mesh to setup the shaders before rendering a mesh with light
	static void PreRenderMeshWithLight(const bool bLightEnable = true, const bool bColorTextureEnabled = true, const bool bColorTexture = false, const bool bTextEnabled = false);
	// Render Mesh to render a mesh with light
	static void RenderMeshWithLight(Mesh* _mesh);
	// Post Render Mesh to setup the shaders after rendering a mesh with light
	static void PostRenderMeshWithLight(const bool bLightEnable = false, const bool bColorTextureEnabled = false, const bool bColorTexture = true, const bool bTextEnabled = false);

	// Pre Render Text to setup the shaders before rendering text
	static void PreRenderText(const bool bLightEnable = false, const bool bColorTextureEnabled = true, const bool bColorTexture = false, const bool bTextEnabled = true);
	// Setup the shaders before rendering a text
	static void RenderText(Mesh* _mesh, const std::string& _text, Color _color);
	// Post Render Text to setup the shaders before rendering text
	static void PostRenderText(const bool bLightEnable = true, const bool bColorTextureEnabled = false, const bool bColorTexture = true, const bool bTextEnabled = false);
};

#endif // RENDER_HELPER_H