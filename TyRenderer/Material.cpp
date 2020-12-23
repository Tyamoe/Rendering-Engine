#include "Material.h"

#include <SOIL2.h>

#include "Utils.h"
#include "GLUtils.h"
#include "AssimpUtils.h"

TYvoid Material::LoadMaterial(aiScene* mScene, TYstring& path)
{
	for (TYuint i = 0; i < mScene->mNumMaterials; i++)
	{
		const aiMaterial* pMaterial = mScene->mMaterials[i];

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString afilename;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &afilename, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				TYstring filename(afilename.data);

				if (filename.substr(0, 2) == ".\\")
				{
					filename = filename.substr(2, filename.size() - 2);
				}

				TYstring fullPath = path + "/" + filename;

				if (filename.find(":") != TYstring::npos || filename.find("..") != TYstring::npos)
				{
					fullPath = filename;
				}

				TYuint texture = Material::CreateTexture(fullPath);
				if (texture == 0)
				{
					color = { GetRand(0.0f, 1.0f), GetRand(0.0f, 1.0f), GetRand(0.0f, 1.0f), 1.0f };
					texture = Material::CreateTexture(TYvec4(1.0f));
				}

				AddTexture(texture);
			}
		}
	}
}

TYuint Material::CreateTexture(TYstring& filename)
{
	TYuint textureId = 0;
	TYint width, height, nrChannels;

	TYubyte* data = SOIL_load_image(filename.c_str(), &width, &height, &nrChannels, 4);

	if (data)
	{
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		SOIL_free_image_data(data);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureId;
	}
	else
	{
		return 0;
	}
}

TYuint Material::CreateTexture(TYvec4 color)
{
	//if (TextureMap.find(color) == TextureMap.end())
	{
		TYuint textureId = 0;

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, &color.r);

		glBindTexture(GL_TEXTURE_2D, 0);

		//TextureMap.insert({ color, textureId });

		return textureId;
	}
	/*else
	{
		return TextureMap[color];
	}*/
}