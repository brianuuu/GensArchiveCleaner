#pragma once
#include <string>
#include "BinaryReader.h"
#include "BinaryWriter.h"

namespace Glitter
{
    class UVAnim
	{
	private:
		std::string name;
        std::string material;

	public:
        UVAnim();
        UVAnim(std::string filename);

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

        void read(BinaryReader* reader);
	};
}
