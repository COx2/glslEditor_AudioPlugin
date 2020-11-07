/*
  ==============================================================================

    GLSLTypes.h
    Created: 13 Jun 2020 1:11:38pm
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Resources/WavefrontObjParser.h"

//==============================================================================
struct Vertex
{
    //==============================================================================
    float position[3];
    float normal[3];
    float colour[4];
    float texCoord[2];

    JUCE_LEAK_DETECTOR (Vertex)
};

//==============================================================================
// This class just manages the attributes that the shaders use.
struct Attributes
{
    //==============================================================================
    Attributes (OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
    {
        position.reset (createAttribute (openGLContext, shaderProgram, "position"));
        normal.reset (createAttribute (openGLContext, shaderProgram, "normal"));
        sourceColour.reset (createAttribute (openGLContext, shaderProgram, "sourceColour"));
        texureCoordIn.reset (createAttribute (openGLContext, shaderProgram, "texureCoordIn"));
    }

    //==============================================================================
    void enable (OpenGLContext& openGLContext)
    {
        if (position != nullptr)
        {
            openGLContext.extensions.glVertexAttribPointer (position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
            openGLContext.extensions.glEnableVertexAttribArray (position->attributeID);
        }

        if (normal != nullptr)
        {
            openGLContext.extensions.glVertexAttribPointer (normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
            openGLContext.extensions.glEnableVertexAttribArray (normal->attributeID);
        }

        if (sourceColour != nullptr)
        {
            openGLContext.extensions.glVertexAttribPointer (sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
            openGLContext.extensions.glEnableVertexAttribArray (sourceColour->attributeID);
        }

        if (texureCoordIn != nullptr)
        {
            openGLContext.extensions.glVertexAttribPointer (texureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
            openGLContext.extensions.glEnableVertexAttribArray (texureCoordIn->attributeID);
        }
    }

    void disable (OpenGLContext& openGLContext)
    {
        if (position.get() != nullptr)
            openGLContext.extensions.glDisableVertexAttribArray (position->attributeID);
        if (normal.get() != nullptr)
            openGLContext.extensions.glDisableVertexAttribArray (normal->attributeID);
        if (sourceColour.get() != nullptr)
            openGLContext.extensions.glDisableVertexAttribArray (sourceColour->attributeID);
        if (texureCoordIn.get() != nullptr)
            openGLContext.extensions.glDisableVertexAttribArray (texureCoordIn->attributeID);
    }

    //==============================================================================
    std::unique_ptr<OpenGLShaderProgram::Attribute> position;
    std::unique_ptr<OpenGLShaderProgram::Attribute> normal;
    std::unique_ptr<OpenGLShaderProgram::Attribute> sourceColour;
    std::unique_ptr<OpenGLShaderProgram::Attribute> texureCoordIn;

private:
    //==============================================================================
    static OpenGLShaderProgram::Attribute* createAttribute (OpenGLContext& openGLContext,
                                                            OpenGLShaderProgram& shader,
                                                            const char* attributeName)
    {
        if (openGLContext.extensions.glGetAttribLocation (shader.getProgramID(), attributeName) < 0)
            return nullptr;

        return new OpenGLShaderProgram::Attribute (shader, attributeName);
    }

    //==============================================================================
    JUCE_LEAK_DETECTOR (Attributes)
};

//==============================================================================
// This class just manages the uniform values that the demo shaders use.
struct Uniforms
{
    //==============================================================================
    Uniforms (OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
    {
        projectionMatrix.reset (createUniform (openGLContext, shaderProgram, "projectionMatrix"));
        viewMatrix.reset (createUniform (openGLContext, shaderProgram, "viewMatrix"));
        time.reset (createUniform (openGLContext, shaderProgram, "time"));
        resolution.reset (createUniform (openGLContext, shaderProgram, "resolution"));
        mouse.reset (createUniform (openGLContext, shaderProgram, "mouse"));
        midiCC.reset (createUniform (openGLContext, shaderProgram, "midiCC"));
        spectrum.reset (createUniform (openGLContext, shaderProgram, "spectrum"));
        wave.reset (createUniform (openGLContext, shaderProgram, "wave"));
    }

    //==============================================================================
    std::unique_ptr<OpenGLShaderProgram::Uniform> projectionMatrix;
    std::unique_ptr<OpenGLShaderProgram::Uniform> viewMatrix;
    std::unique_ptr<OpenGLShaderProgram::Uniform> time;
    std::unique_ptr<OpenGLShaderProgram::Uniform> resolution;
    std::unique_ptr<OpenGLShaderProgram::Uniform> mouse;
    std::unique_ptr<OpenGLShaderProgram::Uniform> midiCC;
    std::unique_ptr<OpenGLShaderProgram::Uniform> wave;
    std::unique_ptr<OpenGLShaderProgram::Uniform> spectrum;

private:
    //==============================================================================
    static OpenGLShaderProgram::Uniform* createUniform (OpenGLContext& openGLContext,
                                                        OpenGLShaderProgram& shaderProgram,
                                                        const char* uniformName)
    {
        if (openGLContext.extensions.glGetUniformLocation (shaderProgram.getProgramID(), uniformName) < 0)
            return nullptr;

        return new OpenGLShaderProgram::Uniform (shaderProgram, uniformName);
    }

    //==============================================================================
    JUCE_LEAK_DETECTOR (Uniforms)
};

//==============================================================================
/** This loads a 3D model from an OBJ file and converts it into some vertex buffers
    that we can draw.
*/
struct Shape
{
    //==============================================================================
    Shape (OpenGLContext& openGLContext)
    {
        if (shapeFile.load (BinaryData::base_obj).wasOk())
            for (int i = 0; i < shapeFile.shapes.size(); ++i)
                vertexBuffers.add (new VertexBuffer (openGLContext, *shapeFile.shapes.getUnchecked (i)));
    }

    //==============================================================================
    void draw (OpenGLContext& openGLContext, Attributes& glAttributes)
    {
        for (int i = 0; i < vertexBuffers.size(); ++i)
        {
            VertexBuffer& vertexBuffer = *vertexBuffers.getUnchecked (i);
            vertexBuffer.bind();

            glAttributes.enable (openGLContext);
            glDrawElements (GL_TRIANGLES, vertexBuffer.numIndices, GL_UNSIGNED_INT, 0);
            glAttributes.disable (openGLContext);
        }
    }

private:
    //==============================================================================
    struct VertexBuffer
    {
        //==============================================================================
        VertexBuffer (OpenGLContext& context, WavefrontObjFile::Shape& aShape) : openGLContext (context)
        {
            numIndices = aShape.mesh.indices.size();

            openGLContext.extensions.glGenBuffers (1, &vertexBuffer);
            openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);

            Array<Vertex> vertices;
            createVertexListFromMesh (aShape.mesh, vertices, Colours::green);

            openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER,
                                                   static_cast<GLsizeiptr> (static_cast<size_t> (vertices.size()) * sizeof (Vertex)),
                                                   vertices.getRawDataPointer(),
                                                   GL_STATIC_DRAW);

            openGLContext.extensions.glGenBuffers (1, &indexBuffer);
            openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            openGLContext.extensions.glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                                                   static_cast<GLsizeiptr> (static_cast<size_t> (numIndices) * sizeof (juce::uint32)),
                                                   aShape.mesh.indices.getRawDataPointer(),
                                                   GL_STATIC_DRAW);
        }

        ~VertexBuffer()
        {
            openGLContext.extensions.glDeleteBuffers (1, &vertexBuffer);
            openGLContext.extensions.glDeleteBuffers (1, &indexBuffer);
        }

        //==============================================================================
        void bind()
        {
            openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
            openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        }

        //==============================================================================
        GLuint vertexBuffer, indexBuffer;
        int numIndices;
        OpenGLContext& openGLContext;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VertexBuffer)
    };

    //==============================================================================
    static void createVertexListFromMesh (const WavefrontObjFile::Mesh& mesh, Array<Vertex>& list, Colour colour)
    {
        const float scale = 1.0f; //0.2f;
        WavefrontObjFile::TextureCoord defaultTexCoord = { 0.5f, 0.5f };
        WavefrontObjFile::Vertex defaultNormal = { 0.5f, 0.5f, 0.5f };

        for (int i = 0; i < mesh.vertices.size(); ++i)
        {
            const WavefrontObjFile::Vertex& v = mesh.vertices.getReference (i);

            const WavefrontObjFile::Vertex& n = i < mesh.normals.size() ? mesh.normals.getReference (i) : defaultNormal;

            const WavefrontObjFile::TextureCoord& tc = i < mesh.textureCoords.size() ? mesh.textureCoords.getReference (i) : defaultTexCoord;

            Vertex vert = {
                {
                    scale * v.x,
                    scale * v.y,
                    scale * v.z,
                },
                {
                    scale * n.x,
                    scale * n.y,
                    scale * n.z,
                },
                { colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha() },
                { tc.x, tc.y }
            };

            list.add (vert);
        }
    }

    //==============================================================================
    WavefrontObjFile shapeFile;
    OwnedArray<VertexBuffer> vertexBuffers;

    //==============================================================================
    JUCE_LEAK_DETECTOR (Shape)
};