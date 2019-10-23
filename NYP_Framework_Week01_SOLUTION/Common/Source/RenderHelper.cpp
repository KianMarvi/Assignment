#include "RenderHelper.h"
#include "Mesh.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "MatrixStack.h"

bool RenderHelper::bLightEnable = false;
bool RenderHelper::bColorTextureEnabled = false;
bool RenderHelper::bColorTexture = false;
bool RenderHelper::bTextEnabled = false;

/**
* Pre Render Mesh to setup the shaders before rendering a mesh without light
*/
void RenderHelper::PreRenderMesh(const bool bLightEnable, const bool bColorTextureEnabled, const bool bColorTexture, const bool bTextEnabled)
{
	// Get current shader program
	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();

	// Enable / Disable lighting stuff
	RenderHelper::bLightEnable = bLightEnable;
	if (bLightEnable)
		currProg->UpdateInt("lightEnabled", 1);
	else
		currProg->UpdateInt("lightEnabled", 0);

	// Enable / Disable colour texture enabled
	RenderHelper::bColorTextureEnabled = bColorTextureEnabled;
	if (bColorTextureEnabled)
		currProg->UpdateInt("colorTextureEnabled", 1);
	else
		currProg->UpdateInt("colorTextureEnabled", 0);

	// Enable / Disable colour texture
	RenderHelper::bColorTexture = bColorTexture;
	if (bColorTexture)
		currProg->UpdateInt("colorTexture", 1);
	else
		currProg->UpdateInt("colorTexture", 0);

	// Enable / Disable text display
	RenderHelper::bTextEnabled = bTextEnabled;
	if (bColorTexture)
		currProg->UpdateInt("textEnabled", 1);
	else
		currProg->UpdateInt("textEnabled", 0);
}

/**
* Render Mesh to render a mesh without light
*/
void RenderHelper::RenderMesh(Mesh* _mesh)
{
	// Get all our transform matrices & update shader
	Mtx44 MVP;
	MVP = GraphicsManager::GetInstance()->GetProjectionMatrix() * GraphicsManager::GetInstance()->GetViewMatrix() * GraphicsManager::GetInstance()->GetModelStack().Top();
	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();
	currProg->UpdateMatrix44("MVP", &MVP.a[0]);

	// Update textures first if available
	if (_mesh->textureID > 0)
	{
		//		if (RenderHelper::bColorTextureEnabled)
		//			currProg->UpdateInt("colorTextureEnabled", 1);

		GraphicsManager::GetInstance()->UpdateTexture(0, _mesh->textureID);

		//		if (RenderHelper::bColorTexture)
		//			currProg->UpdateInt("colorTexture", 0);
	}
	else
	{
		if (bColorTextureEnabled)
		{
			currProg->UpdateInt("colorTextureEnabled", 0);
		}
	}

	// Do actual rendering
	_mesh->Render();

	// Unbind texture for safety (in case next render call uses it by accident)
	if (_mesh->textureID > 0)
	{
		GraphicsManager::GetInstance()->UnbindTexture(0);
	}
	else
	{
		if (bColorTextureEnabled)
		{
			currProg->UpdateInt("colorTextureEnabled", 1);
		}
	}
}

/**
* Post Render Mesh to setup the shaders after rendering a mesh without light
*/
void RenderHelper::PostRenderMesh(const bool bLightEnable, const bool bColorTextureEnabled, const bool bColorTexture, const bool bTextEnabled)
{
	// Get current shader program
	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();

	// Enable / Disable lighting stuff
	RenderHelper::bLightEnable = bLightEnable;
	if (bLightEnable)
		currProg->UpdateInt("lightEnabled", 1);
	else
		currProg->UpdateInt("lightEnabled", 0);

	// Enable / Disable colour texture
	RenderHelper::bColorTextureEnabled = bColorTextureEnabled;
	if (bColorTextureEnabled)
		currProg->UpdateInt("colorTextureEnabled", 1);
	else
		currProg->UpdateInt("colorTextureEnabled", 0);

	// Enable / Disable colour texture
	RenderHelper::bColorTexture = bColorTexture;
	if (bColorTexture)
		currProg->UpdateInt("colorTexture", 1);
	else
		currProg->UpdateInt("colorTexture", 0);

	// Enable / Disable text display
	RenderHelper::bTextEnabled = bTextEnabled;
	if (bColorTexture)
		currProg->UpdateInt("textEnabled", 1);
	else
		currProg->UpdateInt("textEnabled", 0);
}

/**
* Pre Render Mesh to setup the shaders before rendering a mesh with light
*/
void RenderHelper::PreRenderMeshWithLight(const bool bLightEnable, const bool bColorTextureEnabled, const bool bColorTexture, const bool bTextEnabled)
{
	// Get current shader program
	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();

	// Enable / Disable lighting stuff
	if (bLightEnable)
		currProg->UpdateInt("lightEnabled", 1);
	else
		currProg->UpdateInt("lightEnabled", 0);

	// Enable / Disable colour texture
	if (bColorTextureEnabled)
		currProg->UpdateInt("colorTextureEnabled", 1);
	else
		currProg->UpdateInt("colorTextureEnabled", 0);

	// Enable / Disable colour texture
	RenderHelper::bColorTexture = bColorTexture;
	if (bColorTexture)
		currProg->UpdateInt("colorTexture", 1);
	else
		currProg->UpdateInt("colorTexture", 0);

	// Enable / Disable text display
	RenderHelper::bTextEnabled = bTextEnabled;
	if (bColorTexture)
		currProg->UpdateInt("textEnabled", 1);
	else
		currProg->UpdateInt("textEnabled", 0);
}

/**
* Render Mesh to render a mesh with light
*/
void RenderHelper::RenderMeshWithLight(Mesh* _mesh)
{
	// Get all our transform matrices & update shader
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	MVP = GraphicsManager::GetInstance()->GetProjectionMatrix() * GraphicsManager::GetInstance()->GetViewMatrix() * GraphicsManager::GetInstance()->GetModelStack().Top();
	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();
	currProg->UpdateMatrix44("MVP", &MVP.a[0]);

	// Update light stuff
	//currProg->UpdateInt("lightEnabled", 1);
	modelView = GraphicsManager::GetInstance()->GetViewMatrix() * GraphicsManager::GetInstance()->GetModelStack().Top();
	currProg->UpdateMatrix44("MV", &modelView.a[0]);
	modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
	currProg->UpdateMatrix44("MV_inverse_transpose", &modelView.a[0]);

	//load material
	currProg->UpdateVector3("material.kAmbient", &_mesh->material.kAmbient.r);
	currProg->UpdateVector3("material.kDiffuse", &_mesh->material.kDiffuse.r);
	currProg->UpdateVector3("material.kSpecular", &_mesh->material.kSpecular.r);
	currProg->UpdateFloat("material.kShininess", _mesh->material.kShininess);

	// Update textures first if available
	if (_mesh->textureID > 0)
	{
		//		currProg->UpdateInt("colorTextureEnabled", 1);
		GraphicsManager::GetInstance()->UpdateTexture(0, _mesh->textureID);
		//		currProg->UpdateInt("colorTexture", 0);
	}
	else
	{
		if (bColorTextureEnabled)
		{
			currProg->UpdateInt("colorTextureEnabled", 0);
		}
	}

	// Do actual rendering
	_mesh->Render();

	// Unbind texture for safety (in case next render call uses it by accident)
	if (_mesh->textureID > 0)
	{
		GraphicsManager::GetInstance()->UnbindTexture(0);
	}
	else
	{
		if (bColorTextureEnabled)
		{
			currProg->UpdateInt("colorTextureEnabled", 1);
		}
	}
}

/**
* Post Render Mesh to setup the shaders after rendering a mesh with light
*/
void RenderHelper::PostRenderMeshWithLight(const bool bLightEnable, const bool bColorTextureEnabled, const bool bColorTexture, const bool bTextEnabled)
{
	// Get current shader program
	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();

	// Enable / Disable lighting stuff
	if (bLightEnable)
		currProg->UpdateInt("lightEnabled", 1);
	else
		currProg->UpdateInt("lightEnabled", 0);

	// Enable / Disable colour texture
	if (bColorTextureEnabled)
		currProg->UpdateInt("colorTextureEnabled", 1);
	else
		currProg->UpdateInt("colorTextureEnabled", 0);

	// Enable / Disable colour texture
	RenderHelper::bColorTexture = bColorTexture;
	if (bColorTexture)
		currProg->UpdateInt("colorTexture", 1);
	else
		currProg->UpdateInt("colorTexture", 0);

	// Enable / Disable text display
	RenderHelper::bTextEnabled = bTextEnabled;
	if (bColorTexture)
		currProg->UpdateInt("textEnabled", 1);
	else
		currProg->UpdateInt("textEnabled", 0);
}


// Pre Render Text to setup the shaders before rendering text
void RenderHelper::PreRenderText(const bool bLightEnable, const bool bColorTextureEnabled, const bool bColorTexture, const bool bTextEnabled)
{
	// Get current shader program
	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();

	// Enable / Disable lighting stuff
	RenderHelper::bLightEnable = bLightEnable;
	if (bLightEnable)
		currProg->UpdateInt("lightEnabled", 1);
	else
		currProg->UpdateInt("lightEnabled", 0);

	// Enable / Disable colour texture enabled
	RenderHelper::bColorTextureEnabled = bColorTextureEnabled;
	if (bColorTextureEnabled)
		currProg->UpdateInt("colorTextureEnabled", 1);
	else
		currProg->UpdateInt("colorTextureEnabled", 0);

	// Enable / Disable colour texture
	RenderHelper::bColorTexture = bColorTexture;
	if (bColorTexture)
		currProg->UpdateInt("colorTexture", 1);
	else
		currProg->UpdateInt("colorTexture", 0);

	// Enable / Disable text display
	RenderHelper::bTextEnabled = bTextEnabled;
	if (bColorTexture)
		currProg->UpdateInt("textEnabled", 1);
	else
		currProg->UpdateInt("textEnabled", 0);
}

/**
* Setup the shaders before rendering a text
*/
void RenderHelper::RenderText(Mesh* _mesh, const std::string& _text, Color _color)
{
	// Trivial Rejection : Unable to render without mesh or texture
	if (!_mesh || _mesh->textureID <= 0)
		return;

	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();

	currProg->UpdateInt("textEnabled", 1);
	currProg->UpdateVector3("textColor", &_color.r);
	//currProg->UpdateInt("lightEnabled", 0);

	if (_mesh->textureID > 0)
	{
		//currProg->UpdateInt("colorTextureEnabled", 1);
		GraphicsManager::GetInstance()->UpdateTexture(0, _mesh->textureID);
		//currProg->UpdateInt("colorTexture", 0);
	}
	else
	{
		if (bColorTextureEnabled)
		{
			currProg->UpdateInt("colorTextureEnabled", 1);
		}
	}

	for (unsigned i = 0; i < _text.length(); ++i)
	{
		Mtx44 characterSpacing, MVP;
		//characterSpacing.SetToTranslation((i+0.5f) * 1.0f, 0, 0); // 1.0f is the spacing of each character, you may change this value
		characterSpacing.SetToTranslation((float)(1 + (int)i), 0.0f, 0.0f); // 1.0f is the spacing of each character, you may change this value
		MVP = GraphicsManager::GetInstance()->GetProjectionMatrix() * GraphicsManager::GetInstance()->GetViewMatrix() * GraphicsManager::GetInstance()->GetModelStack().Top() * characterSpacing;
		currProg->UpdateMatrix44("MVP", &MVP.a[0]);

		_mesh->Render((unsigned)_text[i] * 6, 6);
	}

	if (_mesh->textureID > 0)
	{
		GraphicsManager::GetInstance()->UnbindTexture(0);
	}
	else
	{
		if (bColorTextureEnabled)
		{
			currProg->UpdateInt("colorTextureEnabled", 0);
		}
	}
	currProg->UpdateInt("textEnabled", 0);
}

// Post Render Text to setup the shaders before rendering text
void RenderHelper::PostRenderText(const bool bLightEnable, const bool bColorTextureEnabled, const bool bColorTexture, const bool bTextEnabled)
{
	// Get current shader program
	ShaderProgram* currProg = GraphicsManager::GetInstance()->GetActiveShader();

	// Enable / Disable lighting stuff
	RenderHelper::bLightEnable = bLightEnable;
	if (bLightEnable)
		currProg->UpdateInt("lightEnabled", 1);
	else
		currProg->UpdateInt("lightEnabled", 0);

	// Enable / Disable colour texture
	RenderHelper::bColorTextureEnabled = bColorTextureEnabled;
	if (bColorTextureEnabled)
		currProg->UpdateInt("colorTextureEnabled", 1);
	else
		currProg->UpdateInt("colorTextureEnabled", 0);

	// Enable / Disable colour texture
	RenderHelper::bColorTexture = bColorTexture;
	if (bColorTexture)
		currProg->UpdateInt("colorTexture", 1);
	else
		currProg->UpdateInt("colorTexture", 0);

	// Enable / Disable text display
	RenderHelper::bTextEnabled = bTextEnabled;
	if (bColorTexture)
		currProg->UpdateInt("textEnabled", 1);
	else
		currProg->UpdateInt("textEnabled", 0);
}
