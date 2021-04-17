#pragma once
#include <string>
#include "BinaryReader.h"
#include "BinaryWriter.h"

namespace Glitter
{
    class TextureOld
	{
	private:
		std::string name;
        std::string texture;

	public:
        TextureOld();
        TextureOld(std::string filename);

		inline void setName(std::string n)
		{
			name = n;
        }

		inline std::string getName() const
		{
			return name;
        }

        inline std::string getTexture() const
        {
            return texture;
        }

        void read(BinaryReader* reader);
	};
}
