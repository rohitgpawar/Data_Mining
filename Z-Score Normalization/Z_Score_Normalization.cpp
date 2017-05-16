/******************************************************

******************************************************/
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

class Attribute
{
private:
	float CalculateVariance(float mean)
	{//Calculate the Variance
		float temp = 0;
		for (std::vector<float>::iterator it = attributeValues.begin(); it != attributeValues.end(); ++it)
		{
			temp += (*it - mean) * (*it - mean);
		}
		return temp / attributeValues.size();
	}

public:
	std::string attributeName;
	std::string attributeType;
	std::vector<float> attributeValues;
	float sum = 0;
	float mean = 0;
	bool isIgnored = false;
	float stdDeviation = 0;

	void AddAttributeValues(float value)
	{// Add next attribute value
		attributeValues.push_back(value);
		sum += value;
	}

	float GetMean()
	{// Cacluate mean
		if (mean == 0)
			return mean = sum / attributeValues.size();
		return mean;
	}

	float GetStandardDeviation()
	{// Calculate Standard deviation
		if (stdDeviation == 0)
			return stdDeviation = sqrt(CalculateVariance(GetMean()));

		return stdDeviation;
	}

	float GetZScoreNormalization(float value)
	{// Calculate Z-Score normalization
		float normalizedValue = 0;
		normalizedValue = (value - mean) / stdDeviation;
		return normalizedValue;
	}
};

class Relation
{
public:
	std::string realtionName;
};

int main(int argc, char *argv[])
{
	std::string inputFile;
	std::string line;
	std::string ignoreAttribute = "class";
	std::string outputMeanStdFile;
	std::string outputNormalizeFile;

	if (argc != 5)
	{
		std::cout << "Wrong set of arguments. \nArguments format: -i <InputFile> -c <classattribute>\n";
	}
	else
	{
		inputFile = argv[2];
		ignoreAttribute = argv[4];
		outputMeanStdFile = "z_nomalization_MeanStd" + inputFile;
		outputNormalizeFile = "z_nomalization_Normalize" + inputFile;
		std::ifstream infile(inputFile.c_str());
		Relation relation;
		std::vector<Attribute> attributes;
		while (std::getline(infile, line))  // getting next line.
		{
			std::istringstream iss(line);
			std::string s1;
			std::string s2;
			int flow = 0;
			int attributeCount = 0;
			iss >> s1;
			if (s1.compare("@relation") == 0)
				flow = 1;
			else if (s1.compare("@attribute") == 0)
				flow = 2;
			else if (s1.compare("@data") == 0)
				continue;
			else if (!s1.empty())
			{
				flow = 3;
				float value = std::stof(s1);
				if (!attributes.at(attributeCount).isIgnored)
					attributes.at(attributeCount).AddAttributeValues(value);
				attributeCount++;
			}

			switch (flow)
			{
			case 1:
				relation.realtionName = line;
				break;
			case 2:
				while (iss >> s1 >> s2)
				{// Add attribute MetaData @attribute
					Attribute attr;
					attr.attributeName = s1;
					attr.attributeType = s2;
					if (s1.compare(ignoreAttribute) == 0)
						attr.isIgnored = true;
					attributes.push_back(attr);
				}
				break;
			case 3:
				while (iss >> s1)
				{ // Add attribute values to the vector list.
					float value = std::stof(s1);
					if (!attributes.at(attributeCount).isIgnored)
						attributes.at(attributeCount).AddAttributeValues(value);
					attributeCount++;
				}
				break;
			default:
				break;
			}
		}

		std::string fileBaseContent;
		//Print @relation content
		fileBaseContent += relation.realtionName + "\n\n";
		//Print @attribute content
		for (std::vector<Attribute>::iterator it = attributes.begin(); it != attributes.end(); ++it)
		{
			fileBaseContent += "@attribute " + (*it).attributeName + " " + (*it).attributeType + "\n";
		}
		//Print @data content
		fileBaseContent += "\n@data\n";


		/*Writing mean and standard deviation to MeanStd file*/
		std::ofstream meanStdFile;
		meanStdFile.open(outputMeanStdFile.c_str());
		meanStdFile << fileBaseContent;
		for (std::vector<Attribute>::iterator it = attributes.begin(); it != attributes.end(); ++it)
		{
			if (!(*it).isIgnored)
				meanStdFile << std::setw(11) << std::setprecision(6) << std::fixed << (*it).GetMean();
		}
		meanStdFile << "\n";
		for (std::vector<Attribute>::iterator it = attributes.begin(); it != attributes.end(); ++it)
		{
			if (!(*it).isIgnored)
				meanStdFile << std::setw(11) << std::setprecision(6) << std::fixed << (*it).GetStandardDeviation();
		}

		meanStdFile.close();

		/*Writing normalized values to Normalize file*/
		std::ofstream normalizedfile;
		normalizedfile.open(outputNormalizeFile.c_str());
		normalizedfile << fileBaseContent;
		int attributeRowNumber = 0;
		int attributeRowCount = 0;
		if (!attributes.at(0).isIgnored)
		{
			attributeRowCount = attributes.at(0).attributeValues.size();
		}
		else
		{
			attributeRowCount = attributes.at(1).attributeValues.size();
		}
		while (attributeRowNumber < attributeRowCount)
		{

			for (std::vector<Attribute>::iterator it = attributes.begin(); it != attributes.end(); ++it)
			{
				if (!(*it).isIgnored)
				{
					normalizedfile << std::setw(11) << std::setprecision(6) << std::fixed << (*it).GetZScoreNormalization((*it).attributeValues.at(attributeRowNumber));
				}
			}
			normalizedfile << "\n";
			attributeRowNumber++;
		}
	}
	return 0;
}
