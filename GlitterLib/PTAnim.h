#pragma once
#include <string>
#include "BinaryReader.h"
#include "BinaryWriter.h"

namespace Glitter
{
    class PTAnim
	{
	private:
		std::string name;
        std::string material;
        std::vector<std::string> textures;

	public:
        PTAnim();
        PTAnim(std::string filename);

		inline void setName(std::string n)
		{
			name = n;
        }

		inline std::string getName() const
		{
			return name;
        }

        inline std::string getMaterial() const
        {
            return material;
        }

        inline std::vector<std::string> getTextures() const
        {
            return textures;
        }

        void read(BinaryReader* reader);
	};
}
