#include "export.h"

#include <stdexcept>
#include <double-conversion.h>

namespace Utils
{
	bool exportPackedMeshToObj(ZenLoad::PackedMesh& mesh, const std::string& file,
		const int decimals, const std::string& name)
	{
		FILE* f = fopen(file.c_str(), "w");

		if (!f)
			return false;

		fputs("# File exported by ZenLib\n", f);
		fputs(("o " + name + "\n").c_str(), f);

		int flags = double_conversion::DoubleToStringConverter::NO_FLAGS;
		double_conversion::DoubleToStringConverter dConverter(flags, 0, 0, 'e', 0, 0, 0, 0);

		const int charsPerPosComponent = decimals + 10; // max buffer for chars per position component
		const int charsPerTexCoordComponent = decimals + 6; // max buffer for chars per texture coords component
		const int charsPerNormalComponent = decimals + 6; // max buffer for chars per texture coords component

		{
			// Write positions (needs biggest buffer, so check for int range)
			if (mesh.vertices.size() > (INT_MAX / 3. / charsPerPosComponent)) {
				throw std::range_error(
					"Needed buffer size exceeds MAX_INT, cannot create StringBuilder!");
			}

			int bufferSize = charsPerPosComponent * 3 * (int)mesh.vertices.size();// positions
			char* buffer = new char[bufferSize];
			double_conversion::StringBuilder builder(buffer, bufferSize);

			for (auto& v : mesh.vertices)
			{
				builder.AddCharacter('v');
				builder.AddCharacter(' ');
				dConverter.ToFixed((double)v.Position.x, decimals, &builder);
				builder.AddCharacter(' ');
				dConverter.ToFixed((double)v.Position.y, decimals, &builder);
				builder.AddCharacter(' ');
				dConverter.ToFixed((double)v.Position.z, decimals, &builder);
				builder.AddCharacter('\n');
			}
			fputs(builder.Finalize(), f);
			fflush(f);
			delete[] buffer;
		}
		{
			// Write texture coords
			int bufferSize = charsPerTexCoordComponent * 2 * (int)mesh.vertices.size();// tex coords
			char* buffer = new char[bufferSize];
			double_conversion::StringBuilder builder(buffer, bufferSize);

			for (auto& v : mesh.vertices)
			{
				builder.AddCharacter('v');
				builder.AddCharacter('t');
				builder.AddCharacter(' ');
				dConverter.ToFixed((double)v.TexCoord.x, decimals, &builder);
				builder.AddCharacter(' ');
				dConverter.ToFixed((double)v.TexCoord.y, decimals, &builder);
				builder.AddCharacter('\n');
			}
			fputs(builder.Finalize(), f);
			fflush(f);
			delete[] buffer;
		}
		{
			// Write normals
			int bufferSize = charsPerNormalComponent * 3 * (int)mesh.vertices.size();// normals
			char* buffer = new char[bufferSize];
			double_conversion::StringBuilder builder(buffer, bufferSize);

			for (auto& v : mesh.vertices)
			{
				builder.AddCharacter('v');
				builder.AddCharacter('n');
				builder.AddCharacter(' ');
				dConverter.ToFixed((double)v.Normal.x, decimals, &builder);
				builder.AddCharacter(' ');
				dConverter.ToFixed((double)v.Normal.y, decimals, &builder);
				builder.AddCharacter(' ');
				dConverter.ToFixed((double)v.Normal.z, decimals, &builder);
				builder.AddCharacter('\n');
			}
			fputs(builder.Finalize(), f);
			fflush(f);
			delete[] buffer;
		}

		fputs("s off\n", f);

		// Write faces
		for (auto& s : mesh.subMeshes)
		{
			fputs(("g " + s.material.matName + "\n").c_str(), f);
			fputs(("usemtl " + s.material.matName + "\n").c_str(), f);

			int bufferSize = 26 * (int)s.indices.size();
			char* buffer = new char[bufferSize];
			double_conversion::StringBuilder builder(buffer, bufferSize);

			for (size_t i = 0; i<s.indices.size(); i += 3)
			{
				uint32_t idx[] = { s.indices[i], s.indices[i + 1], s.indices[i + 2] };

				builder.AddCharacter('f');
				builder.AddCharacter(' ');

				std::string f1 = std::to_string(idx[0] + 1);
				builder.AddString(f1.c_str());
				builder.AddCharacter('/');
				builder.AddString(f1.c_str());
				builder.AddCharacter('/');
				builder.AddString(f1.c_str());
				builder.AddCharacter(' ');

				std::string f2 = std::to_string(idx[1] + 1);
				builder.AddString(f2.c_str());
				builder.AddCharacter('/');
				builder.AddString(f2.c_str());
				builder.AddCharacter('/');
				builder.AddString(f2.c_str());
				builder.AddCharacter(' ');

				std::string f3 = std::to_string(idx[2] + 1);
				builder.AddString(f3.c_str());
				builder.AddCharacter('/');
				builder.AddString(f3.c_str());
				builder.AddCharacter('/');
				builder.AddString(f3.c_str());
				builder.AddCharacter('\n');
			}

			fputs(builder.Finalize(), f);
			delete[] buffer;
		}

		fclose(f);

		// Write mtl
		FILE* mf = fopen((file + ".mtl").c_str(), "w");

		if (!mf)
			return false;

		for (auto& s : mesh.subMeshes)
		{
			fputs(("newmtl " + s.material.matName + "\n").c_str(), mf);
			fputs(("map_Kd " + s.material.texture + "\n").c_str(), mf);

			ZMath::float4 color; color.fromABGR8(s.material.color);

			fputs(("Kd " + std::to_string(color.x) + " " + std::to_string(color.y) + " "
				+ std::to_string(color.z) + "\n").c_str(), mf);

			fputs(("Ka " + std::to_string(color.x) + " " + std::to_string(color.y) + " "
				+ std::to_string(color.z) + "\n").c_str(), mf);

			fputs("\n", mf);
		}

		fclose(mf);

		return true;
	}
}
