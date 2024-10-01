/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2015 by ROLI Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#pragma once

#include <map>

//==============================================================================
/**
    This is a quick-and-dirty parser for the 3D OBJ file format.

    Just call load() and if there aren't any errors, the 'shapes' array should
    be filled with all the shape objects that were loaded from the file.
*/
class WavefrontObjFile
{
public:
    WavefrontObjFile() {}

    juce::Result load (const juce::String& objFileContent)
    {
        shapes.clear();
        return parseObjFile (juce::StringArray::fromLines (objFileContent));
    }

    juce::Result load (const juce::File& file)
    {
        sourceFile = file;
        return load (file.loadFileAsString());
    }

    //==============================================================================
    typedef juce::uint32 Index;

    struct Vertex
    {
        float x, y, z;
    };
    struct TextureCoord
    {
        float x, y;
    };

    struct Mesh
    {
        juce::Array<Vertex> vertices, normals;
        juce::Array<TextureCoord> textureCoords;
        juce::Array<Index> indices;

        JUCE_LEAK_DETECTOR (Mesh)
    };

    struct Material
    {
        Material() noexcept : shininess (1.0f), refractiveIndex (0.0f)
        {
            juce::zerostruct (ambient);
            juce::zerostruct (diffuse);
            juce::zerostruct (specular);
            juce::zerostruct (transmittance);
            juce::zerostruct (emission);
        }

        juce::String name;

        Vertex ambient, diffuse, specular, transmittance, emission;
        float shininess, refractiveIndex;

        juce::String ambientTextureName, diffuseTextureName,
            specularTextureName, normalTextureName;

        juce::StringPairArray parameters;

        JUCE_LEAK_DETECTOR (Material)
    };

    struct Shape
    {
        juce::String name;
        Mesh mesh;
        Material material;

        JUCE_LEAK_DETECTOR (Shape)
    };

    juce::OwnedArray<Shape> shapes;

private:
    //==============================================================================
    juce::File sourceFile;

    struct TripleIndex
    {
        TripleIndex() noexcept : vertexIndex (-1), textureIndex (-1), normalIndex (-1) {}

        bool operator< (const TripleIndex& other) const noexcept
        {
            if (this == &other)
                return false;

            if (vertexIndex != other.vertexIndex)
                return vertexIndex < other.vertexIndex;

            if (textureIndex != other.textureIndex)
                return textureIndex < other.textureIndex;

            return normalIndex < other.normalIndex;
        }

        int vertexIndex, textureIndex, normalIndex;

        JUCE_LEAK_DETECTOR (TripleIndex)
    };

    struct IndexMap
    {
        std::map<TripleIndex, Index> map;

        Index getIndexFor (TripleIndex i, Mesh& newMesh, const Mesh& srcMesh)
        {
            const std::map<TripleIndex, Index>::iterator it (map.find (i));

            if (it != map.end())
                return it->second;

            const Index index = (Index) newMesh.vertices.size();

            if (juce::isPositiveAndBelow (i.vertexIndex, srcMesh.vertices.size()))
                newMesh.vertices.add (srcMesh.vertices.getReference (i.vertexIndex));

            if (juce::isPositiveAndBelow (i.normalIndex, srcMesh.normals.size()))
                newMesh.normals.add (srcMesh.normals.getReference (i.normalIndex));

            if (juce::isPositiveAndBelow (i.textureIndex, srcMesh.textureCoords.size()))
                newMesh.textureCoords.add (srcMesh.textureCoords.getReference (i.textureIndex));

            map[i] = index;
            return index;
        }

        JUCE_LEAK_DETECTOR (IndexMap)
    };

    static float parseFloat (juce::String::CharPointerType& t)
    {
        t = t.findEndOfWhitespace();
        return (float)juce::CharacterFunctions::readDoubleValue (t);
    }

    static Vertex parseVertex (juce::String::CharPointerType t)
    {
        Vertex v;
        v.x = parseFloat (t);
        v.y = parseFloat (t);
        v.z = parseFloat (t);
        return v;
    }

    static TextureCoord parseTextureCoord (juce::String::CharPointerType t)
    {
        TextureCoord tc;
        tc.x = parseFloat (t);
        tc.y = parseFloat (t);
        return tc;
    }

    static bool matchToken (juce::String::CharPointerType& t, const char* token)
    {
        const int len = (int) strlen (token);

        if (juce::CharacterFunctions::compareUpTo (juce::CharPointer_ASCII (token), t, len) == 0)
        {
            juce::String::CharPointerType end = t + len;

            if (end.isEmpty() || end.isWhitespace())
            {
                t = end.findEndOfWhitespace();
                return true;
            }
        }

        return false;
    }

    struct Face
    {
        Face (juce::String::CharPointerType t)
        {
            while (! t.isEmpty())
                triples.add (parseTriple (t));
        }

        juce::Array<TripleIndex> triples;

        void addIndices (Mesh& newMesh, const Mesh& srcMesh, IndexMap& indexMap)
        {
            TripleIndex i0 (triples[0]), i1, i2 (triples[1]);

            for (int i = 2; i < triples.size(); ++i)
            {
                i1 = i2;
                i2 = triples.getReference (i);

                newMesh.indices.add (indexMap.getIndexFor (i0, newMesh, srcMesh));
                newMesh.indices.add (indexMap.getIndexFor (i1, newMesh, srcMesh));
                newMesh.indices.add (indexMap.getIndexFor (i2, newMesh, srcMesh));
            }
        }

        static TripleIndex parseTriple (juce::String::CharPointerType& t)
        {
            TripleIndex i;

            t = t.findEndOfWhitespace();
            i.vertexIndex = t.getIntValue32() - 1;
            t = findEndOfFaceToken (t);

            if (t.isEmpty() || t.getAndAdvance() != '/')
                return i;

            if (*t == '/')
            {
                ++t;
            }
            else
            {
                i.textureIndex = t.getIntValue32() - 1;
                t = findEndOfFaceToken (t);

                if (t.isEmpty() || t.getAndAdvance() != '/')
                    return i;
            }

            i.normalIndex = t.getIntValue32() - 1;
            t = findEndOfFaceToken (t);
            return i;
        }

        static juce::String::CharPointerType findEndOfFaceToken (juce::String::CharPointerType t) noexcept
        {
            return juce::CharacterFunctions::findEndOfToken (t, juce::CharPointer_ASCII ("/ \t"), juce::String().getCharPointer());
        }

        JUCE_LEAK_DETECTOR (Face)
    };

    static Shape* parseFaceGroup (const Mesh& srcMesh,
                                  const juce::Array<Face>& faceGroup,
                                  const Material& material,
                                  const juce::String& name)
    {
        if (faceGroup.size() == 0)
            return nullptr;

        juce::ScopedPointer<Shape> shape (new Shape());
        shape->name = name;
        shape->material = material;

        IndexMap indexMap;

        for (int i = 0; i < faceGroup.size(); ++i)
            faceGroup.getUnchecked (i).addIndices (shape->mesh, srcMesh, indexMap);

        return shape.release();
    }

    juce::Result parseObjFile (const juce::StringArray& lines)
    {
        Mesh mesh;
        juce::Array<Face> faceGroup;

        juce::Array<Material> knownMaterials;
        Material lastMaterial;
        juce::String lastName;

        for (int lineNum = 0; lineNum < lines.size(); ++lineNum)
        {
            juce::String::CharPointerType l = lines[lineNum].getCharPointer().findEndOfWhitespace();

            if (matchToken (l, "v"))
            {
                mesh.vertices.add (parseVertex (l));
                continue;
            }
            if (matchToken (l, "vn"))
            {
                mesh.normals.add (parseVertex (l));
                continue;
            }
            if (matchToken (l, "vt"))
            {
                mesh.textureCoords.add (parseTextureCoord (l));
                continue;
            }
            if (matchToken (l, "f"))
            {
                faceGroup.add (Face (l));
                continue;
            }

            if (matchToken (l, "usemtl"))
            {
                const juce::String name (juce::String (l).trim());

                for (int i = knownMaterials.size(); --i >= 0;)
                {
                    if (knownMaterials.getReference (i).name == name)
                    {
                        lastMaterial = knownMaterials.getReference (i);
                        break;
                    }
                }

                continue;
            }

            if (matchToken (l, "mtllib"))
            {
                juce::Result r = parseMaterial (knownMaterials, juce::String (l).trim());
                continue;
            }

            if (matchToken (l, "g") || matchToken (l, "o"))
            {
                if (Shape* shape = parseFaceGroup (mesh, faceGroup, lastMaterial, lastName))
                    shapes.add (shape);

                faceGroup.clear();
                lastName = juce::StringArray::fromTokens (l, " \t", "")[0];
                continue;
            }
        }

        if (Shape* shape = parseFaceGroup (mesh, faceGroup, lastMaterial, lastName))
            shapes.add (shape);

        return juce::Result::ok();
    }

    juce::Result parseMaterial (juce::Array<Material>& materials, const juce::String& filename)
    {
        jassert (sourceFile.exists());
        juce::File f (sourceFile.getSiblingFile (filename));

        if (! f.exists())
            return juce::Result::fail ("Cannot open file: " + filename);

        juce::StringArray lines;
        lines.addLines (f.loadFileAsString());

        materials.clear();
        Material material;

        for (int i = 0; i < lines.size(); ++i)
        {
            juce::String::CharPointerType l (lines[i].getCharPointer().findEndOfWhitespace());

            if (matchToken (l, "newmtl"))
            {
                materials.add (material);
                material.name = juce::String (l).trim();
                continue;
            }

            if (matchToken (l, "Ka"))
            {
                material.ambient = parseVertex (l);
                continue;
            }
            if (matchToken (l, "Kd"))
            {
                material.diffuse = parseVertex (l);
                continue;
            }
            if (matchToken (l, "Ks"))
            {
                material.specular = parseVertex (l);
                continue;
            }
            if (matchToken (l, "Kt"))
            {
                material.transmittance = parseVertex (l);
                continue;
            }
            if (matchToken (l, "Ke"))
            {
                material.emission = parseVertex (l);
                continue;
            }
            if (matchToken (l, "Ni"))
            {
                material.refractiveIndex = parseFloat (l);
                continue;
            }
            if (matchToken (l, "Ns"))
            {
                material.shininess = parseFloat (l);
                continue;
            }

            if (matchToken (l, "map_Ka"))
            {
                material.ambientTextureName = juce::String (l).trim();
                continue;
            }
            if (matchToken (l, "map_Kd"))
            {
                material.diffuseTextureName = juce::String (l).trim();
                continue;
            }
            if (matchToken (l, "map_Ks"))
            {
                material.specularTextureName = juce::String (l).trim();
                continue;
            }
            if (matchToken (l, "map_Ns"))
            {
                material.normalTextureName = juce::String (l).trim();
                continue;
            }

            juce::StringArray tokens;
            tokens.addTokens (l, " \t", "");

            if (tokens.size() >= 2)
                material.parameters.set (tokens[0].trim(), tokens[1].trim());
        }

        materials.add (material);
        return juce::Result::ok();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavefrontObjFile)
};
