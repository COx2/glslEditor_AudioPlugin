/*
  ==============================================================================

    GLSLComponent.cpp
    Created: 9 Feb 2017 1:46:02am
    Author:  COx2

  ==============================================================================
*/
#include "GLSLComponent.h"
#include "StaticValues.h"

//==============================================================================
static const char* defaultVertexShader =
"attribute vec3 position;\n"
"attribute vec3 normal;\n"
"attribute vec4 sourceColour;\n"
"attribute vec2 texureCoordIn;\n"
"attribute vec2 surfacePosAttrib;\n"
"\n"
"uniform mat4 projectionMatrix;\n"
"uniform mat4 viewMatrix;\n"
"uniform vec2 resolution;\n"
"\n"
"varying vec4 destinationColour;\n"
"varying vec2 textureCoordOut;\n"
"varying vec2 surfacePosition;\n"
"\n"
"void main()\n"
"{\n"
"    destinationColour = sourceColour;\n"
"    textureCoordOut = texureCoordIn;\n"
"    mat4 trnsmat = mat4(1., 0., 0., 0., 0., resolution.y / resolution.x, 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.);\n"
"    trnsmat = trnsmat*0.5;\n"
"    surfacePosition = (vec4(position, 1.0) * trnsmat).xy;\n"
"    gl_Position = vec4(position, 1.0);\n"
"}\n";

static const char* defaultFragmentShader =
#if JUCE_OPENGL_ES
"varying lowp vec4 destinationColour;\n"
"varying lowp vec2 textureCoordOut;\n"
#else
"varying vec4 destinationColour;\n"
"varying vec2 textureCoordOut;\n"
#endif
"\n"
"#extension GL_OES_standard_derivatives : enable\n"
"\n"
"uniform float time;\n"  /**/
"uniform vec2 mouse;\n" /**/
"uniform vec2 resolution;\n" /**/
"uniform float midiCC[128];\n"  /**/
"uniform float wave[256];\n" /**/
"uniform float spectrum[256];\n" /**/
"\n"
"#define pi 3.1415\n"
"\n"
"void main()\n"
"{\n"
"    vec2 position = (gl_FragCoord.xy / resolution.xy);\n"
"    float r = abs(cos(position.x + time*position.y * spectrum[64]));\n"
"    float g = abs(sin(position.x - position.y + time + mouse.x));\n"
"    float b = abs(tan(position.y + time + mouse.y * wave[64]));\n"
"    gl_FragColor = vec4(r, g, b, 1.0);\n"
"}\n";

//==============================================================================
GLSLComponent::GLSLComponent()
{
	//setSize(800, 600);
}

GLSLComponent::~GLSLComponent()
{
	shutdownOpenGL();
}

void GLSLComponent::initialise()
{
	vertexShader = defaultVertexShader;
	fragmentShader = defaultFragmentShader;

	if (StaticValues::getShaderCacheReady())
	{
		setShaderProgramFragment(StaticValues::getShaderCache());
		updateShader();
	}
	else
	{
		if (fragmentDoc != nullptr)
			fragmentDoc->replaceAllContent(defaultFragmentShader);

		createShaders();
	}
	isInitialised = true;
}

void GLSLComponent::shutdown()
{
	shader = nullptr;
	shape = nullptr;
	attributes = nullptr;
	uniforms = nullptr;
}

void GLSLComponent::render()
{
	if (!openGLContext.isAttached())
		return;

	if (!openGLContext.isActive())
		return;

	jassert(OpenGLHelpers::isContextActive());

	if (isShaderCompileReady)
		updateShader();

	const float desktopScale = (float)openGLContext.getRenderingScale();
	OpenGLHelpers::clear(Colour::greyLevel(0.1f));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, roundToInt(desktopScale * getWidth()), roundToInt(desktopScale * getHeight()));

	shader->use();

	//////////////////////  VertextShader   ////////////////

	if (uniforms->projectionMatrix != nullptr)
		uniforms->projectionMatrix->setMatrix4(getProjectionMatrix().mat, 1, false);

	if (uniforms->viewMatrix != nullptr)
		uniforms->viewMatrix->setMatrix4(getViewMatrix().mat, 1, false);

	//////////////////////  FragmentShader   ////////////////

	if (uniforms->time != nullptr) {
		uniforms->time->set(timeCounter);
	}

	if (uniforms->resolution != nullptr) {
		uniforms->resolution->set(getWidth(), getHeight());
	}

	if (uniforms->mouse != nullptr) {
		if (isMouseButtonDown())
			uniforms->mouse->set(mouseX, mouseY);
	}

	if (uniforms->midiCC != nullptr) {
		uniforms->midiCC->set(m_midiCC, 128);
	}

	if (uniforms->spectrum != nullptr) {
		uniforms->spectrum->set(m_spectrum, 256);
	}

	if (uniforms->wave != nullptr) {
		uniforms->wave->set(m_wave, 256);
	}

	//////////////////////////////////////

	shape->draw(openGLContext, *attributes);

	// Reset the element buffers so child Components draw correctly
	openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
	openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/**/
	timeCounter += 0.02f;
	/**/
}

void GLSLComponent::paint(Graphics& g)
{
	// You can add your component specific drawing code here!
	// This will draw over the top of the openGL background.
}

void GLSLComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
}

void GLSLComponent::setStatusLabelPtr(Label* _statusLabel)
{
	statusLabel = _statusLabel;
}

void GLSLComponent::setFragmentDocPtr(CodeDocument* _fragmentDoc)
{
	fragmentDoc = _fragmentDoc;
}

void GLSLComponent::setEditorPtr(AudioProcessorEditor* _editor)
{
	editor = _editor;
}

void GLSLComponent::setShaderProgram(const String& vertexShader, const String& fragmentShader)
{
	newVertexShader = vertexShader;
	newFragmentShader = fragmentShader;
	isShaderCompileReady = true;
}

void GLSLComponent::setShaderProgramFragment(const String& _fragmentShader)
{
	newVertexShader = vertexShader;
	newFragmentShader = _fragmentShader;
	isShaderCompileReady = true;
}

void GLSLComponent::setShaderProgramVertex(const String& _vertexShader)
{
	newVertexShader = _vertexShader;
	newFragmentShader = fragmentShader;
	isShaderCompileReady = true;
}

void GLSLComponent::setMidiCCValue(int ccNumber, float value)
{
	if (ccNumber < 128) {
		m_midiCC[ccNumber] = value;

		if (statusLabel != nullptr) 
		{
			auto cText = statusLabel->getText();
			cText += " /" + String(ccNumber) + "-" + String(value, 1);
			statusLabel->setText(cText, dontSendNotification);
		}

	}
}

void GLSLComponent::setSpectrumValue(int spectrumNumber, float value)
{
	if (spectrumNumber < 256) {
		m_spectrum[spectrumNumber] = value;
	}
}

void GLSLComponent::setWaveValue(int waveNumber, float value)
{
	if (waveNumber < 256) {
		m_wave[waveNumber] = value;
	}
}

//==============================================================================
void GLSLComponent::createShaders()
{
	if (!openGLContext.isAttached())
		return;

	if (!openGLContext.isActive())
		return;

	ScopedPointer<OpenGLShaderProgram> newShader(new OpenGLShaderProgram(openGLContext));
	String statusText;

	if (newShader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(vertexShader))
		&& newShader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(fragmentShader))
		&& newShader->link())
	{
		shape = nullptr;
		attributes = nullptr;
		uniforms = nullptr;

		shader = newShader;
		shader->use();

		shape = new Shape(openGLContext);
		attributes = new Attributes(openGLContext, *shader);
		uniforms = new Uniforms(openGLContext, *shader);

		statusText = "GLSL: v" + String(OpenGLShaderProgram::getLanguageVersion(), 2);
	}
	else
	{
		statusText = newShader->getLastError();
	}

	if (statusLabel != nullptr)
		statusLabel->setText(statusText, dontSendNotification);

	isShaderCompileReady = false;
}

void GLSLComponent::updateShader()
{
	if (!openGLContext.isAttached())
		return;

	if (!openGLContext.isActive())
		return;

	if (newVertexShader.isNotEmpty() || newFragmentShader.isNotEmpty())
	{
		ScopedPointer<OpenGLShaderProgram> newShader(new OpenGLShaderProgram(openGLContext));
		String statusText;

		if (newShader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(newVertexShader))
			&& newShader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(newFragmentShader))
			&& newShader->link())
		{
			shape = nullptr;
			attributes = nullptr;
			uniforms = nullptr;

			shader = newShader;
			shader->use();

			shape = new Shape(openGLContext);
			attributes = new Attributes(openGLContext, *shader);
			uniforms = new Uniforms(openGLContext, *shader);

			statusText = "GLSL: v" + String(OpenGLShaderProgram::getLanguageVersion(), 2);
		}
		else
		{
			statusText = newShader->getLastError();
		}

		if (statusLabel != nullptr)
			statusLabel->setText(statusText, dontSendNotification);

		newVertexShader = String();
		newFragmentShader = String();

		isShaderCompileReady = false;
	}
}

void GLSLComponent::mouseDrag(const MouseEvent& event)
{
	mouseX = float(event.getPosition().getX()) / getWidth();
	mouseY = 1.0f - float(event.getPosition().getY()) / getHeight();

	mouseX = std::min(std::max(0.0f, mouseX), 1.0f);
	mouseY = std::min(std::max(0.0f, mouseY), 1.0f);
}

Matrix3D<float> GLSLComponent::getProjectionMatrix() const
{
	float w = 1.0f / (0.5f + 0.1f);
	float h = w * getLocalBounds().toFloat().getAspectRatio(false);
	return Matrix3D<float>::fromFrustum(-w, w, -h, h, 4.0f, 30.0f);
}

Matrix3D<float> GLSLComponent::getViewMatrix() const
{
	Matrix3D<float> viewMatrix(Vector3D<float>(0.0f, 0.0f, -5.0f /*-10.0f*/));
	Matrix3D<float> rotationMatrix = viewMatrix.rotated(Vector3D<float>(-0.3f, 5.0f * std::sin(getFrameCounter() * 0.01f), 0.0f));

	return /*rotationMatrix * */viewMatrix;
}
