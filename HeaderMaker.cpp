#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

struct sSection
{
	unsigned int nFirstLineNum;
	std::vector<std::string> vLines;
};

struct sParsedFile
{
	std::vector<std::string> vHeaders;
	std::unordered_map<std::string, sSection> mapSections;
};


bool GrabFile(const std::string& sFileName, std::vector<std::string>& vFile)
{
	std::ifstream ifs(sFileName);
	if (ifs.is_open())
	{
		while (!ifs.eof())
		{
			std::string s;
			std::getline(ifs, s);
			vFile.emplace_back(s);
		}

		return true;
	}

	return false;
}

struct sInstruction
{
	std::string line;
	std::vector<std::string> commands;
};

void CreateInstructions(const std::vector<std::string>& vSourceFile, std::vector<sInstruction>& vInstructions)
{
	for (auto& line : vSourceFile)
	{
		sInstruction inst;
		inst.line = line;

		// Does line contain an '///[OLC_HM]' directive?
		if (line.find("///[OLC_HM]") != std::string::npos)
		{
			// ...it does! So parse the tokens
			std::stringstream ss(line);
			std::string token;
			ss >> token; // First token is just the directive
			while (!ss.eof())
			{
				ss >> token;
				inst.commands.push_back(token);
			}			
		}

		vInstructions.push_back(inst);
	}
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << "FAIL: No template header specified\n";
		return 0;
	}

	// Read in template file, and parse it

	std::vector<std::string> vTemplateFile;
	if (!GrabFile(argv[1], vTemplateFile))
	{
		std::cout << "FAIL: Could not open template header file: " << argv[1];
		return 0;
	}

	std::vector<sInstruction> vInstructions;
	CreateInstructions(vTemplateFile, vInstructions);
	


	std::unordered_map<std::string, sParsedFile> mapFiles;

	// Pass 1: Pull in files
	for (auto& inst : vInstructions)
	{
		if (!inst.commands.empty())
		{
			if (inst.commands[0] == "INSERT")
			{
				std::string sFilename = inst.commands[1];
				if (mapFiles.count(sFilename) == 0)
				{
					// We've not read this file before so read it in and parse it
					std::vector<std::string> vSourceFile;
					if (!GrabFile(sFilename, vSourceFile))
					{
						std::cout << "FAIL INSERT: Could not open file: " << sFilename;
						return 0;
					}

					std::vector<sInstruction> vSourceInstructions;
					CreateInstructions(vSourceFile, vSourceInstructions);

					sParsedFile source;
					std::string sCurrentSection = "";

					for (unsigned int i = 0; i < vSourceInstructions.size(); i++)
					{
						auto& line = vSourceInstructions[i];

						if (!line.commands.empty())
						{
							// Line in source file is a directive
							if (line.commands.size() == 2)
							{
								if (line.commands[0] == "START")
								{
									sCurrentSection = line.commands[1];
								}
								else if (line.commands[0] == "END")
								{
									sCurrentSection = "";
								}
							}
							else
							{
								std::cout << "FAIL BAD DIRECTIVE: \n" << line.line << "\nin " << sFilename << "\n";
								return 0;
							}
						}
						else
						{
							if (line.line.find("#include") != std::string::npos)
							{
								if (line.line.find("<") != std::string::npos)
								{
									source.vHeaders.push_back(line.line);
								}
							}

							if (sCurrentSection != "")
							{
								if (source.mapSections.find(sCurrentSection) == source.mapSections.end())
									source.mapSections[sCurrentSection].nFirstLineNum = i + 1;

								source.mapSections[sCurrentSection].vLines.push_back(line.line);
							}
						}
					}

					mapFiles[sFilename] = source;

				}
			}
		}
	}

	

	// Pool Headers
	std::unordered_set<std::string> setHeaders;
	for (auto& file : mapFiles)
	{
		for (auto& header : file.second.vHeaders)
		{
			setHeaders.insert(header);
		}
	}

	// Pass 2: Construct Source
	std::vector<std::string> vOutput;

	for (unsigned int i = 0; i < vInstructions.size(); i++)
	{
		auto& inst = vInstructions[i];
		if (!inst.commands.empty())
		{
			if (inst.commands[0] == "INSERT")
			{
				std::string sFilename = inst.commands[1];
				std::string sSection = inst.commands[2];

				if (mapFiles[sFilename].mapSections.count(sSection) > 0)
				{
					std::string directive = std::string{"#line "} + std::to_string(mapFiles[sFilename].mapSections[sSection].nFirstLineNum) + " \"" + sFilename + "\"";
					vOutput.push_back(directive);

					for (auto& line : mapFiles[sFilename].mapSections[sSection].vLines)
						vOutput.push_back(line);

					// Count the lines from 1 and account for the directive itself
					directive = std::string{"#line "} + std::to_string(i + 2) + " \"" + argv[1] + "\"";
					vOutput.push_back(directive);
				}
				else
				{
					std::cout << "FAIL: NO SECTION FOUND: " << sSection << "\n";
					return 0;
				}				
			}
			else if (inst.commands[0] == "HEADERS")
			{
				for (auto& line : setHeaders)
				{
					vOutput.push_back(line);
				}
			}
		}
		else
		{
			vOutput.push_back(inst.line);
		}
	}

	std::ofstream ofs(argv[2]);
	for (auto& line : vOutput)
		ofs << line << "\n";
	ofs.close();

	std::cout << "olc::HeaderMaker Exit\n";

	return 0;
}
