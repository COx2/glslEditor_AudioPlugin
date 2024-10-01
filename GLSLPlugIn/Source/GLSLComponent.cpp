/*
  ==============================================================================

    GLSLComponent.cpp
    Created: 9 Feb 2017 1:46:02am
    Author:  COx2

  ==============================================================================
*/
#include "GLSLComponent.h"
#include "StaticValues.h"

// short hand.
using namespace juce;

//==============================================================================
const juce::String GLSLComponent::defaultVertexShader = juce::String (std::string (R"(
attribute vec3 position;
attribute vec3 normal;
attribute vec4 sourceColour;
attribute vec2 texureCoordIn;
attribute vec2 surfacePosAttrib;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec2 resolution;

varying vec4 destinationColour;
varying vec2 textureCoordOut;
varying vec2 surfacePosition;

void main()
{
    destinationColour = sourceColour;
    textureCoordOut = texureCoordIn;
    mat4 trnsmat = mat4(1., 0., 0., 0., 0., resolution.y / resolution.x, 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.);
    trnsmat = trnsmat*0.5;
    surfacePosition = (vec4(position, 1.0) * trnsmat).xy;
    gl_Position = vec4(position, 1.0);
})"));

const juce::String GLSLComponent::defaultFragmentShader = String (std::string (R"(
#if JUCE_OPENGL_ES
varying lowp vec4 destinationColour;
varying lowp vec2 textureCoordOut;
#else
varying vec4 destinationColour;
varying vec2 textureCoordOut;
#endif

#extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
uniform float midiCC[128];
uniform float wave[256];
uniform float spectrum[256];

#define pi 3.1415

void main()
{
    vec2 position = (gl_FragCoord.xy / resolution.xy);
    float r = abs(cos(position.x + time*position.y * spectrum[64]));
    float g = abs(sin(position.x - position.y + time + mouse.x));
    float b = abs(tan(position.y + time + mouse.y * wave[64]));
    gl_FragColor = vec4(r, g, b, 1.0);
})"));

juce::Array<GLSLComponent::ShaderPreset> GLSLComponent::getShaderPresets()
{
#define SHADER_2DDEMO_HEADER \
            "/*  This demo shows the use of the OpenGLGraphicsContextCustomShader,\n" \
            "    which allows a 2D area to be filled using a GL shader program.\n" \
            "\n" \
            "    Edit the shader program below and it will be \n" \
            "    recompiled in real-time!\n" \
            "*/\n\n"

    GLSLComponent::ShaderPreset presets[] =
    {
        {
            "Simple Gradient",

            SHADER_2DDEMO_HEADER
            "void main()\n"
            "{\n"
            "    " JUCE_MEDIUMP " vec4 colour1 = vec4 (1.0, 0.4, 0.6, 1.0);\n"
            "    " JUCE_MEDIUMP " vec4 colour2 = vec4 (0.0, 0.8, 0.6, 1.0);\n"
            "    " JUCE_MEDIUMP " float alpha = pixelPos.x / 1000.0;\n"
            "    gl_FragColor = pixelAlpha * mix (colour1, colour2, alpha);\n"
            "}\n"
        },

        {
            "Circular Gradient",

            SHADER_2DDEMO_HEADER
            "void main()\n"
            "{\n"
            "    " JUCE_MEDIUMP " vec4 colour1 = vec4 (1.0, 0.4, 0.6, 1.0);\n"
            "    " JUCE_MEDIUMP " vec4 colour2 = vec4 (0.3, 0.4, 0.4, 1.0);\n"
            "    " JUCE_MEDIUMP " float alpha = distance (pixelPos, vec2 (600.0, 500.0)) / 400.0;\n"
            "    gl_FragColor = pixelAlpha * mix (colour1, colour2, alpha);\n"
            "}\n"
        },

        {
            "Circle",

            SHADER_2DDEMO_HEADER
            "void main()\n"
            "{\n"
            "    " JUCE_MEDIUMP " vec4 colour1 = vec4 (0.1, 0.1, 0.9, 1.0);\n"
            "    " JUCE_MEDIUMP " vec4 colour2 = vec4 (0.0, 0.8, 0.6, 1.0);\n"
            "    " JUCE_MEDIUMP " float distance = distance (pixelPos, vec2 (600.0, 500.0));\n"
            "\n"
            "    " JUCE_MEDIUMP " float innerRadius = 200.0;\n"
            "    " JUCE_MEDIUMP " float outerRadius = 210.0;\n"
            "\n"
            "    if (distance < innerRadius)\n"
            "        gl_FragColor = colour1;\n"
            "    else if (distance > outerRadius)\n"
            "        gl_FragColor = colour2;\n"
            "    else\n"
            "        gl_FragColor = mix (colour1, colour2, (distance - innerRadius) / (outerRadius - innerRadius));\n"
            "\n"
            "    gl_FragColor *= pixelAlpha;\n"
            "}\n"
        },

        {
            "Solid Colour",

            SHADER_2DDEMO_HEADER
            "void main()\n"
            "{\n"
            "    gl_FragColor = vec4 (1.0, 0.6, 0.1, 1.0);\n"
            "}\n"
        }
    };

    return Array<ShaderPreset>(presets, numElementsInArray(presets));
}

//==============================================================================
GLSLComponent::GLSLComponent()
{
}

GLSLComponent::~GLSLComponent()
{
    shutdownOpenGL();
}

void GLSLComponent::initialise()
{
    vertexShader = defaultVertexShader;
    fragmentShader = getShaderPresets().getReference(3).fragmentShader;

    if (StaticValues::getShaderCacheReady())
    {
        setShaderProgramFragment (StaticValues::getShaderCacheVerified());
        updateShader();
    }
    else
    {
        createShaders();
    }
    isInitialised = true;

    triggerAsyncUpdate();
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
    if (! openGLContext.isAttached())
        return;

    if (! openGLContext.isActive())
        return;

    jassert (OpenGLHelpers::isContextActive());

    if (shader == nullptr)
        return;

    if (isShaderCompileReady)
        updateShader();

    const float desktopScale = (float) openGLContext.getRenderingScale();
    OpenGLHelpers::clear (Colour::greyLevel (0.1f));

    juce::gl::glEnable (juce::gl::GL_BLEND);
    juce::gl::glBlendFunc (juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);

    juce::gl::glViewport (0, 0, roundToInt (desktopScale * getWidth()), roundToInt (desktopScale * getHeight()));

    shader->use();

    //////////////////////  VertextShader   ////////////////

    if (uniforms->projectionMatrix != nullptr)
        uniforms->projectionMatrix->setMatrix4 (getProjectionMatrix().mat, 1, false);

    if (uniforms->viewMatrix != nullptr)
        uniforms->viewMatrix->setMatrix4 (getViewMatrix().mat, 1, false);

    //////////////////////  FragmentShader   ////////////////

    if (uniforms->time != nullptr)
    {
        uniforms->time->set (timeCounter);
    }

    if (uniforms->resolution != nullptr)
    {
        uniforms->resolution->set (getWidth(), getHeight());
    }

    if (uniforms->mouse != nullptr)
    {
        if (isMouseButtonDown())
            uniforms->mouse->set (mouseX, mouseY);
    }

    if (uniforms->midiCC != nullptr)
    {
        uniforms->midiCC->set (m_midiCC, 128);
    }

    if (uniforms->spectrum != nullptr)
    {
        uniforms->spectrum->set (m_spectrum, 256);
    }

    if (uniforms->wave != nullptr)
    {
        uniforms->wave->set (m_wave, 256);
    }

    //////////////////////////////////////

    shape->draw (openGLContext, *attributes);

    // Reset the element buffers so child Components draw correctly
    openGLContext.extensions.glBindBuffer (juce::gl::GL_ARRAY_BUFFER, 0);
    openGLContext.extensions.glBindBuffer (juce::gl::GL_ELEMENT_ARRAY_BUFFER, 0);

    /**/
    timeCounter += 0.02f;
    /**/
}

void GLSLComponent::paint (Graphics& g)
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

void GLSLComponent::setStatusLabelPtr (Label* _statusLabel)
{
    statusLabel = _statusLabel;
}

void GLSLComponent::setFragmentDocPtr (CodeDocument* _fragmentDoc)
{
    fragmentDoc = _fragmentDoc;
}

//==============================================================================
void GLSLComponent::setShaderProgram (const String& vertexShader, const String& fragmentShader)
{
    newVertexShader = vertexShader;
    newFragmentShader = fragmentShader;
    isShaderCompileReady = true;
}

void GLSLComponent::setShaderProgramFragment (const String& _fragmentShader)
{
    newVertexShader = vertexShader;
    newFragmentShader = _fragmentShader;
    isShaderCompileReady = true;
}

void GLSLComponent::setShaderProgramVertex (const String& _vertexShader)
{
    newVertexShader = _vertexShader;
    newFragmentShader = fragmentShader;
    isShaderCompileReady = true;
}

//==============================================================================
void GLSLComponent::setMidiCCValue (int ccNumber, float value)
{
    if (ccNumber < 128)
    {
        m_midiCC[ccNumber] = value;
#ifdef DEBUG
        if (statusLabel != nullptr)
        {
            auto cText = statusLabel->getText();
            cText += " /" + String (ccNumber) + "-" + String (value, 1);
            statusLabel->setText (cText, dontSendNotification);
        }
#endif // DEBUG
    }
}

void GLSLComponent::setSpectrumValue (int spectrumNumber, float value)
{
    if (spectrumNumber < 256)
    {
        m_spectrum[spectrumNumber] = value;
    }
}

void GLSLComponent::setWaveValue (int waveNumber, float value)
{
    if (waveNumber < 256)
    {
        m_wave[waveNumber] = value;
    }
}

//==============================================================================
void GLSLComponent::handleAsyncUpdate()
{
    if (statusLabel != nullptr)
        statusLabel->setText (statusText, dontSendNotification);
}

//==============================================================================
void GLSLComponent::createShaders()
{
    if (! openGLContext.isAttached())
        return;

    if (! openGLContext.isActive())
        return;

    std::unique_ptr<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (openGLContext));

    if (newShader->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (vertexShader))
        && newShader->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
        && newShader->link())
    {
        shader = std::move (newShader);
        shader->use();

        shape.reset (new Shape (openGLContext));
        attributes.reset (new Attributes (openGLContext, *shader));
        uniforms.reset (new Uniforms (openGLContext, *shader));

        statusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
        isShaderCompileSuccess = true;

        StaticValues::setShaderCacheVerified (newFragmentShader);
    }
    else
    {
        statusText = newShader->getLastError();
        isShaderCompileSuccess = false;
    }

    isShaderCompileReady = false;

    triggerAsyncUpdate();
}

void GLSLComponent::updateShader()
{
    if (! openGLContext.isAttached())
        return;

    if (! openGLContext.isActive())
        return;

    if (newVertexShader.isNotEmpty() || newFragmentShader.isNotEmpty())
    {
        std::unique_ptr<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (openGLContext));

        if (newShader->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (newVertexShader))
            && newShader->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (newFragmentShader))
            && newShader->link())
        {
            shader = std::move (newShader);
            shader->use();

            shape.reset (new Shape (openGLContext));
            attributes.reset (new Attributes (openGLContext, *shader));
            uniforms.reset (new Uniforms (openGLContext, *shader));

            statusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
            isShaderCompileSuccess = true;

            StaticValues::setShaderCacheVerified (newFragmentShader);
        }
        else
        {
            statusText = "GLSL: v" + String(OpenGLShaderProgram::getLanguageVersion(), 2)
                + juce::newLine 
                + newShader->getLastError();
            isShaderCompileSuccess = false;
        }

        newVertexShader = String();
        newFragmentShader = String();

        isShaderCompileReady = false;
    }

    triggerAsyncUpdate();
}

void GLSLComponent::mouseDrag (const MouseEvent& event)
{
    mouseX = float (event.getPosition().getX()) / getWidth();
    mouseY = 1.0f - float (event.getPosition().getY()) / getHeight();

    mouseX = std::min (std::max (0.0f, mouseX), 1.0f);
    mouseY = std::min (std::max (0.0f, mouseY), 1.0f);
}

Matrix3D<float> GLSLComponent::getProjectionMatrix() const
{
    float w = 1.0f / (0.5f + 0.1f);
    float h = w * getLocalBounds().toFloat().getAspectRatio (false);
    return Matrix3D<float>::fromFrustum (-w, w, -h, h, 4.0f, 30.0f);
}

Matrix3D<float> GLSLComponent::getViewMatrix() const
{
    Matrix3D<float> viewMatrix (Matrix3D<float>::fromTranslation(Vector3D<float> (0.0f, 0.0f, -5.0f /*-10.0f*/)));
    Matrix3D<float> rotationMatrix = viewMatrix.rotation (Vector3D<float> (-0.3f, 5.0f * std::sin (getFrameCounter() * 0.01f), 0.0f));

    return /*rotationMatrix * */ viewMatrix;
}
