#include "MeshAsset.h"

#include <sstream>
#include <vector>

#include "Core/Debug/Log.h"
#include "Core/File/File.h"
#include "Core/Streaming/Stream.h"
#include "Core/Util/String.h"
#include "Core/Util/Util.h"
#include "Renderer/Mesh.h"


// MeshX File NGAFormat - Version 2
// Header:
//		Version - 4 bytes
//		Num Vertex Attributes - 4 bytes
//		List of Vertex Attributes - 6 * Num Vertex Attributes bytes
//			NGAFormat - 1 byte
//			Semantic Type - 1 byte
//			Semantic Index - 1 byte
//		Num Vertices - 8 bytes
//		Num Indices - 8 bytes
// Data:
//		Vertices - Vertex Stride * Num Vertices bytes
//		Indices - sizeof(IndexType) * Num Indices bytes

// Changelog:
// 2 - Add vertex format description to header.

#include "Core/Math/Vector.h"

namespace playground
{
#if CORE_BUILD_TYPE(TOOLS)
	struct VertexType {
		Vector3f mPosition;
		Vector3f mNormal;
		Vector3f mBinormal;
		Vector3f mTangent;
		Vector2f mTexCoord;
	};

	struct VertexArray {
		std::vector<VertexType> mVertices;

		bool mHasNormal = false;
		bool mHasTexCoord = false;
		bool mHasBinormal = false;
		bool mHasTangent = false;

#define HAS_ATTR(b, length) ((b) ? length : 0)
		size_t CalculateStride()const { 
			return sizeof(float) * 
				(3 + HAS_ATTR(mHasNormal, 3) + HAS_ATTR(mHasTexCoord, 2) + HAS_ATTR(mHasBinormal, 3) + HAS_ATTR(mHasTangent, 3));
		}
#undef HAS_ATTR

#define HAS_ATTR(b) ((b) ? 1 : 0)
		int GetAttrCount()const {
			return 1 + HAS_ATTR(mHasNormal) + HAS_ATTR(mHasTexCoord) + HAS_ATTR(mHasBinormal) + HAS_ATTR(mHasTangent);
		}
#undef HAS_ATTR

		void* CreateVertexData()
		{
			size_t stride = CalculateStride();
			size_t numVertices = mVertices.size();
			float* vertexData = (float*)CORE_ALLOC(sizeof(float) * numVertices * stride);

			for (int i = 0; i < numVertices; ++i) {
				VertexType v = mVertices[i];
				size_t offset = i * (stride / sizeof(float));

				size_t o = 0;
#define WRITE(value) vertexData[offset + o] = value; ++o

				WRITE(v.mPosition.x);
				WRITE(v.mPosition.y);
				WRITE(v.mPosition.z);

				if (mHasNormal) {
					WRITE(v.mNormal.x);
					WRITE(v.mNormal.y);
					WRITE(v.mNormal.z);
				}

				if (mHasTexCoord) {
					WRITE(v.mTexCoord.x);
					WRITE(v.mTexCoord.y);
				}

				if (mHasBinormal) {
					WRITE(v.mBinormal.x);
					WRITE(v.mBinormal.y);
					WRITE(v.mBinormal.z);
				}

				if (mHasTangent) {
					WRITE(v.mTangent.x);
					WRITE(v.mTangent.y);
					WRITE(v.mTangent.z);
				}

#undef WRITE
			}

			return vertexData;
		}
	};

	static bool LoadMeshFromOBJ(VertexArray &vertices, std::vector<IndexType> &indices, const std::string &filename);
#endif

	static bool OnMeshxLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header);
	static void OnMeshUnload(const AssetID &id);

	static bool CreateMeshFromMeshx(Mesh *mesh, const std::string &filename);

	bool MeshSystemInit()
	{
		AssetType meshxMeshType;
		meshxMeshType.mExt = "meshx";
		meshxMeshType.mOnLoadRaw = OnMeshxLoad;
		meshxMeshType.mOnUnload = OnMeshUnload;
		RegisterAssetType(meshxMeshType);

		return true;
	}

	void MeshSystemShutdown()
	{
		CORE_ASSERT(Mesh::ReportEmpty(), "There were still meshes allocated during shutdown!");
		Mesh::ReleaseAll();
	}
	
	static bool OnMeshxLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header)
	{
		Mesh *pMesh = Mesh::Create(id);
		CORE_ASSERT_RETURN_VALUE(pMesh != nullptr, false, "Failed to allocate mesh.");

		return CreateMeshFromMeshx(pMesh, filename);
	}
	
	static void OnMeshUnload(const AssetID &id)
	{
		Mesh::Release(id);
	}


	static bool CreateMeshFromMeshx(Mesh *mesh, const std::string &filename)
	{
		File file(filename, std::ios::in | std::ios::binary);

		uint32_t version;
		file.Read<uint32_t>(version);
		CORE_ASSERT_RETURN_VALUE(version >= MESH_ASSET_MIN_VERSION && version <= MESH_ASSET_MAX_VERSION, 
			false, 
			"Trying to load mesh with invalid version number (%d). File: %s", version, filename.c_str());
		
		uint32_t numVertexAttributes;
		bool success = file.Read<uint32_t>(numVertexAttributes);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to read numVertexAttributes from mesh file: %s", filename.c_str());

		NGAVertexFormatDesc vDesc;
		int vertexStride = 0;
		for (int i = 0; i < (int)numVertexAttributes; ++i) {
			uint8_t format;
			success = file.Read<uint8_t>(format);
			CORE_ASSERT_RETURN_VALUE(success, false, "Failed to read vertex attribute format at index %d from mesh file: %s", i, filename.c_str());

			uint8_t semanticType;
			success = file.Read<uint8_t>(semanticType);
			CORE_ASSERT_RETURN_VALUE(success, false, "Failed to read vertex attribute semanticType at index %d from mesh file: %s", i, filename.c_str());

			uint8_t semanticIndex;
			success = file.Read<uint8_t>(semanticIndex);
			CORE_ASSERT_RETURN_VALUE(success, false, "Failed to read vertex attribute semanticIndex at index %d from mesh file: %s", i, filename.c_str());

			NGAVertexAttribute attr;
			attr.mFormat = (NGAFormat)format;
			attr.mSemanticType = (NGAVertexSemanticType)semanticType;
			attr.mSemanticIndex = semanticIndex;
			attr.mOffset = vertexStride;
			vDesc.mAttributes.push_back(attr);

			vertexStride += (int)GetFormatByteSize(attr.mFormat);
		}

		uint64_t numVertices;
		success = file.Read<uint64_t>(numVertices);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to read numVertices from mesh file: %s", filename.c_str());

		uint64_t numIndices;
		success = file.Read<uint64_t>(numIndices);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to read numIndices from mesh file: %s", filename.c_str());

		void *vertexData = CORE_ALLOC(vertexStride * numVertices);
		success = file.ReadBytes((char*)vertexData, vertexStride * numVertices);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to read vertexData from mesh file: %s", filename.c_str());

		void *indexData = CORE_ALLOC(sizeof(IndexType) * numIndices);
		success = file.ReadBytes((char*)indexData, sizeof(IndexType) * numIndices);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to read indexData from mesh file: %s", filename.c_str());

		MeshData meshData;
		meshData.vertices = vertexData;
		meshData.numVertices = numVertices;
		meshData.vertexStride = vertexStride;
		meshData.indices = (IndexType*)indexData;
		meshData.numIndices = numIndices;
		meshData.mVertexFormat = vDesc;
		
		const bool initialized = mesh->Initialize(meshData);

		CORE_FREE(vertexData);
		CORE_FREE(indexData);

		return initialized;
	}


#if CORE_BUILD_TYPE(TOOLS)
	static bool LoadMeshFromOBJ(VertexArray &vertices, std::vector<IndexType> &indices, const std::string &filename)
	{
		struct FaceIndexData
		{
			uint32_t pos;
			uint32_t norm;
			uint32_t tex;
		};

		File file(filename, std::ios::in);

		typedef Vector3f Position;
		typedef Vector3f Normal;
		typedef Vector2f TexCoord;

		std::vector<Position> positions;
		std::vector<Normal> normals;
		std::vector<TexCoord> texCoords;
		std::vector<FaceIndexData> faces;

		/////////////////////////////////////////////////
		std::string line;
		while (file.ReadLine(line)) {
			Trim(line);

			std::stringstream ss(line);

			// Strip the prefix
			std::string garbage;
			ss >> garbage;

			switch (line[0]) {
			case 'v':
				// Vertex data
				switch (line[1]) {
				case ' ':
				{
					Position p;
					ss >> p.x >> p.y >> p.z;
					positions.push_back(p);
					break;
				}

				case 'n':
				{
					Normal n;
					ss >> n.x >> n.y >> n.z;
					normals.push_back(n);
					break;
				}

				case 't':
				{
					TexCoord texCoord;
					ss >> texCoord.x >> texCoord.y;
					texCoords.push_back(texCoord);
					break;
				}
				}

				break;

			case 'f':
			{
				// TODO
				CORE_ASSERT_RETURN_VALUE(normals.size() > 0 && texCoords.size() > 0, false, "Meshes must have both normal and tex coord data.");

				const int numVerticesInFace = 3;
				FaceIndexData indices[numVerticesInFace];

				for (int i = 0; i < numVerticesInFace; ++i) {
					std::string faceString;
					ss >> faceString;
					std::stringstream faceSS(faceString);

					std::string indexString;
					std::getline(faceSS, indexString, '/');
					indices[i].pos = std::stoul(indexString) - 1;

					std::getline(faceSS, indexString, '/');
					indices[i].tex = std::stoul(indexString) - 1;

					std::getline(faceSS, indexString, '/');
					indices[i].norm = std::stoul(indexString) - 1;

					faces.push_back(indices[i]);
				}
				break;
			}
			}
		}

		CORE_ASSERT_RETURN_VALUE(positions.size() > 0, false);
		CORE_ASSERT_RETURN_VALUE(faces.size() > 0, false);

		// Remove duplicates and create mesh
		std::map<std::string, uint32_t> vertexIds;

		for (auto &face : faces) {
			std::string faceString = std::to_string(face.pos) + "/" + std::to_string(face.norm) + "/" + std::to_string(face.tex);

			vertices.mHasNormal = true;
			vertices.mHasTexCoord = true;

			if (vertexIds.find(faceString) == vertexIds.end()) {
				VertexType vertex;

				vertex.mPosition = positions[face.pos];
				vertex.mNormal = normals[face.norm];
				vertex.mTexCoord = texCoords[face.tex];

				vertices.mVertices.push_back(vertex);

				IndexType index = (IndexType)(vertices.mVertices.size() - 1);
				vertexIds[faceString] = index;
				indices.push_back(index);
			} else {
				IndexType index = vertexIds[faceString];
				indices.push_back(index);
			}
		}

		return true;
	}

	
	static void CalculateTangentSpaceTVector(VertexArray &vertices, const std::vector<IndexType> &indices)
	{
		vertices.mHasTangent = true;

		for (auto &it : vertices.mVertices) {
			it.mTangent = Vector3f(0.0f, 0.0f, 0.0f);
		}

		for (int i = 0; i < indices.size(); i += 3) {
			IndexType i1 = indices[i];
			IndexType i2 = indices[i + 1];
			IndexType i3 = indices[i + 2];

			VertexType &vertex1 = vertices.mVertices[i1];
			VertexType &vertex2 = vertices.mVertices[i2];
			VertexType &vertex3 = vertices.mVertices[i3];

			Vector3f v1 = vertex1.mPosition;
			Vector3f v2 = vertex2.mPosition;
			Vector3f v3 = vertex3.mPosition;

			Vector2f w1 = vertex1.mTexCoord;
			Vector2f w2 = vertex2.mTexCoord;
			Vector2f w3 = vertex3.mTexCoord;

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;

			float r = 1.0F / (s1 * t2 - s2 * t1);
			Vector3f sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
			Vector3f tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);

#define FLOAT3_ADD(f1, f2) f1.x += f2.x; f1.y += f2.y; f1.z += f2.z
			FLOAT3_ADD(vertex1.mTangent, sdir);
			FLOAT3_ADD(vertex2.mTangent, sdir);
			FLOAT3_ADD(vertex3.mTangent, sdir);
#undef FLOAT3_ADD
		}

		for (auto &vertex : vertices.mVertices) {
			Vector n(vertex.mNormal);
			Vector t(vertex.mTangent);
			Vector b(vertex.mBinormal);

			t = t - (n * n.V3Dot(t));
			t.V3Normalize();
			vertex.mTangent = t.AsVector3();
		}
	}

	bool ConvertMeshObjToMeshx(const std::string &filename, bool calculateTangentSpaceBasicVectors)
	{
		VertexArray vertices;
		std::vector<IndexType> indices;
		const bool success = LoadMeshFromOBJ(vertices, indices, filename);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to create mesh from %s", filename.c_str());

		if (calculateTangentSpaceBasicVectors) {
			CalculateTangentSpaceTVector(vertices, indices);
		}

		const std::string outFilename = DropFileExt(filename) + ".meshx";
		File file(outFilename, std::ios::out | std::ios::binary);

#define WRITE(cat, x) if (!(x)) { CORE_ASSERT_RETURN_VALUE(false, false, "Failed to write %s", cat); }
		WRITE("Version", file.Write<uint32_t>(MESH_ASSET_VERSION));

		const int numAttributes = vertices.GetAttrCount();
		WRITE("Num Vertex Attributes", file.Write<uint32_t>(numAttributes));

		// Always write position
		WRITE("POSITION NGAFormat", file.Write<uint8_t>((uint8_t)NGAFormat::R32G32B32_FLOAT));
		WRITE("POSITION Semantic", file.Write<uint8_t>((uint8_t)NGAVertexSemanticType::POSITION));
		WRITE("POSITION Semantic Index", file.Write<uint8_t>(0));

		if (vertices.mHasNormal) {
			WRITE("NORMAL NGAFormat", file.Write<uint8_t>((uint8_t)NGAFormat::R32G32B32_FLOAT));
			WRITE("NORMAL Semantic", file.Write<uint8_t>((uint8_t)NGAVertexSemanticType::NORMAL));
			WRITE("NORMAL Semantic Index", file.Write<uint8_t>(0));
		}

		if (vertices.mHasTexCoord) {
			WRITE("TEXCOORD NGAFormat", file.Write<uint8_t>((uint8_t)NGAFormat::R32G32_FLOAT));
			WRITE("TEXCOORD Semantic", file.Write<uint8_t>((uint8_t)NGAVertexSemanticType::TEXCOORD));
			WRITE("TEXCOORD Semantic Index", file.Write<uint8_t>(0));
		}

		if (vertices.mHasTangent) {
			WRITE("TANGENT NGAFormat", file.Write<uint8_t>((uint8_t)NGAFormat::R32G32B32_FLOAT));
			WRITE("TANGENT Semantic", file.Write<uint8_t>((uint8_t)NGAVertexSemanticType::TANGENT));
			WRITE("TANGENT Semantic Index", file.Write<uint8_t>(0));
		}

		if (vertices.mHasBinormal) {
			WRITE("BINORMAL NGAFormat", file.Write<uint8_t>((uint8_t)NGAFormat::R32G32B32_FLOAT));
			WRITE("BINORMAL Semantic", file.Write<uint8_t>((uint8_t)NGAVertexSemanticType::BINORMAL));
			WRITE("BINORMAL Semantic Index", file.Write<uint8_t>(0));
		}

		WRITE("Num Vertices", file.Write<uint64_t>(vertices.mVertices.size()));
		WRITE("Num Indices", file.Write<uint64_t>(indices.size()));

		void *vertexData = vertices.CreateVertexData();
		WRITE("Vertices", file.WriteBytes((char*)vertexData, vertices.mVertices.size() * vertices.CalculateStride()));
		WRITE("Indices", file.WriteBytes((char*)indices.data(), indices.size() * sizeof(IndexType)));
#undef WRITE

		CORE_FREE(vertexData);

		LogInfo("Mesh", "Successfully converted mesh to meshx. Output file: %s", outFilename.c_str());

		return true;
	}
#endif
}