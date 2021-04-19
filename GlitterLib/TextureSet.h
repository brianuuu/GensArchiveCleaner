#pragma once
#include <string>
#include "BinaryReader.h"
#include "BinaryWriter.h"

namespace Glitter
{
    class TextureSet
	{
	private:
		std::string name;
        std::vector<std::string> dotTextures;

	public:
        TextureSet();
        TextureSet(std::string filename);

		inline void setName(std::string n)
		{
			name = n;
        }

		inline std::string getName() const
		{
			return name;
        }

        inline std::vector<std::string> getDotTextures() const
        {
            return dotTextures;
        }

        void read(BinaryReader* reader);
	};
}
