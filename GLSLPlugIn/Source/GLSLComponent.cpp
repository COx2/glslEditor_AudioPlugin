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
	// Shaderがnullにならないように代入しておく
	vertexShader = defaultVertexShader;
	fragmentShader = defaultFragmentShader;

	if (isShaderCacheReady)
	{
		setShaderProgramFragment(ShaderCache);
		updateShader();
	}
	else
	{
		if (fragmentDoc != nullptr)
			fragmentDoc->replaceAllContent(defaultFragmentShader);

		createShaders();
	}
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
	if (isShaderCompileReady)
		updateShader();

	jassert(OpenGLHelpers::isContextActive());

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
		auto s = sinf(timeCounter);
		uniforms->time->set(timeCounter);
	}

	if (uniforms->resolution != nullptr) {
		uniforms->resolution->set(800, 600);
	}

	if (uniforms->mouse != nullptr) {
		if (isMouseButtonDown())
			uniforms->mouse->set(mouseX * 0.1f, mouseY * 0.1f);
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
	mouseX = event.getPosition().getX();
	mouseY = event.getPosition().getY();
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