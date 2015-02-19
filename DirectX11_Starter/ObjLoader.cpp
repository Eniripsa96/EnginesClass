#include "ObjLoader.h"

ObjLoader::ObjLoader()
{
}

ObjLoader::~ObjLoader()
{
}

// Loads an OBJ model from a file
// Loosely based off the tutorial here: http://www.braynzarsoft.net/index.php?p=D3D11OBJMODEL
// Returns number of indices which is needed by the mesh
UINT ObjLoader::Load(char* fileName, ID3D11Device* device, ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer)
{
	// Initialize data
	char c;
	vector<XMFLOAT3> positions;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT2> uvs;
	vector<UINT> posIndices;
	vector<UINT> normalIndices;
	vector<UINT> uvIndices;

	// Read the file
	wifstream read(fileName);
	while (read)
	{
		c = (char)read.get();	//Get next char

		switch (c)
		{

		// If it's the end of a line, go to the next character
		case '\n':
			break;

		// Vertex data
		case 'v':	
			c = (char)read.get();

			// Position
			if (c == ' ')
			{
				float x, y, z;
				read >> x >> y >> z;
				positions.push_back(XMFLOAT3(x, y, z));
			}

			// UV Coordinates
			if (c == 't')
			{
				float u, v;
				read >> u >> v;
				uvs.push_back(XMFLOAT2(u, v));
			}

			// Normals
			if (c == 'n')
			{
				float x, y, z;
				read >> x >> y >> z;
				normals.push_back(XMFLOAT3(x, y, z));
			}
			break;

		// Faces
		case 'f':
			c = (char)read.get();
			if (c == ' ')
			{
				wstring faceData = L"";
				std::wstring vertexData;
				int sets = 1;

				// Load the face data
				do 
				{
					c = (char)read.get();
					faceData += c;
					if (c == ' ')
					{
						sets++;
					}
				} 
				while (c != '\n' && read);
				
				// If it ended with a space, there's one less tri
				if (faceData[faceData.length() - 1] == ' ')
				{
					sets--;
				}

				// Start parsing the line
				wstringstream stream(faceData);
				if (faceData.length() > 0)
				{
					int vIndex1, vIndex2;
					int nIndex1, nIndex2;
					int uvIndex1, uvIndex2;
					for (int i = 0; i < sets; ++i)
					{
						stream >> vertexData;	

						std::wstring part;
						int vIndex, nIndex, uvIndex;
						int type = 0;		

						//Parse this set
						for (UINT j = 0; j < vertexData.length(); ++j)
						{
							if (vertexData[j] != '/')
							{
								part += vertexData[j];
							}

							// Parse to an int when it the piece ends
							if (vertexData[j] == '/' || j == vertexData.length() - 1)
							{
								std::wistringstream intStream(part);

								// Position index
								if (type == 0)	
								{
									intStream >> vIndex;
									vIndex -= 1;
								}

								// UV index
								else if (type == 1)
								{
									intStream >> uvIndex;
									uvIndex -= 1;	
								}

								// Normal
								else if (type == 2)
								{
									intStream >> nIndex;
									nIndex -= 1;
								}

								part = L"";
								type++;				
							}
						}

						// Indices shared by all tris
						if (i == 0)
						{
							vIndex1 = vIndex;
							nIndex1 = nIndex;
							uvIndex1 = uvIndex;
						}

						// Second index to start the chain
						else if (i == 1)
						{
							vIndex2 = vIndex;
							nIndex2 = nIndex;
							uvIndex2 = uvIndex;
						}

						// Add a new tri at 3 and beyond
						else
						{
							// Add the tri positions to the index list
							posIndices.push_back(vIndex1);
							posIndices.push_back(vIndex2);
							posIndices.push_back(vIndex);

							// Add the ti normals to the index list
							normalIndices.push_back(nIndex1);
							normalIndices.push_back(nIndex2);
							normalIndices.push_back(nIndex);

							// Add the tri uvs to the index list
							uvIndices.push_back(uvIndex1);
							uvIndices.push_back(uvIndex2);
							uvIndices.push_back(uvIndex);
							
							// Get ready for the next tri
							vIndex2 = vIndex;
							nIndex2 = nIndex;
							uvIndex2 = uvIndex;
						}
					}
				}
			}
			break;

		// Ignore other types of lines
		default:
			do
			{
				c = (char)read.get();
			} 
			while (c != '\n' && read);
			break;
		}
	}
	
	// Translate data to vertexes
	// Currently doesn't share vertices, could improve this
	// Need to watch for dissimilar indexes between normals/uvs/positions though
	vector<Vertex> vertices;
	vector<UINT> indices;
	Vertex temp;
	for (UINT i = 0; i < posIndices.size(); i++)
	{
		temp.Position = positions[posIndices[i]];
		temp.Normal = normals[normalIndices[i]];
		temp.UV = uvs[uvIndices[i]];

		vertices.push_back(temp);
		indices.push_back(i);
	}

	// Create the vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer));

	// Create the index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = &indices[0];
	HR(device->CreateBuffer(&ibd, &initialIndexData, indexBuffer));

	return indices.size();
}
