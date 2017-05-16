///******************************************************

//Author:-	Rohit Pawar
//Date:-		2/28/2017
//
//******************************************************/


#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <exception>
#include <cstdlib>
using namespace std;

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

	float GetInverseZScoreNormalization(float value)
	{// Calculate Inverse Z-Score normalization
		if (mean == 0)
			GetMean();
		if (stdDeviation == 0)
			GetStandardDeviation();
		float reversenormalizedValue = 0;
		reversenormalizedValue = (value * stdDeviation) + mean;
		return reversenormalizedValue;
	}

	void SetClusterIndex(int clusterIndex)
	{
		
	}

};

class Relation
{
public:
	std::string realtionName;
};

class Cluster
{
private:
	vector<float> attributeSummation;
	vector<Attribute> attributes;
public:
	vector<int> rowNumbers;
	vector<float> central_values;
	int clusterId;
	Cluster()
	{
	
	}

	Cluster(int clusterId, vector<float> centralValues, vector<Attribute> attributes)
	{
		this->clusterId = clusterId;
		this->central_values = centralValues;
		this->attributes = attributes;
		attributeSummation = vector<float>((attributes).size(), 0);
	}

	void addPoint(int rowNumber)
	{
		rowNumbers.push_back(rowNumber);
		for (int i = 0; i<attributes.size(); i++)
		{
			if(!attributes.at(i).isIgnored)
				attributeSummation[i] += attributes.at(i).attributeValues[rowNumber];
		}
	}

	void removePoint(int rowNumber)
	{
		rowNumbers.erase(remove(rowNumbers.begin(), rowNumbers.end(), rowNumber), rowNumbers.end());
		for (int i = 0; i<attributes.size(); i++)
		{
			if (!attributes.at(i).isIgnored)
			{
				if (attributeSummation[i] >= attributes.at(i).attributeValues[rowNumber])
					attributeSummation[i] -= attributes.at(i).attributeValues[rowNumber];
				else
				{
					attributeSummation[i] -= attributes.at(i).attributeValues[rowNumber];
				}
			}
		}
	}

	float getTotalSSE()
	{
		//rowNumbers
		float totalSSE = 0;
		for (int rowNo = 0; rowNo < rowNumbers.size(); rowNo++)
		{
			for (int i = 0; i < (attributes).size(); i++)
			{
				if (!attributes.at(i).isIgnored)
				{
					double centroidValue = getCentralValue(i);
					double  dataPoint = (attributes).at(i).attributeValues[rowNumbers[rowNo]];
					totalSSE += pow(centroidValue - dataPoint, 2.0);
				}
			}
		}
		return totalSSE;
	}

	double getCentralValue(int index)
	{
		return central_values[index];
	}

	void setCentralValue(int index, double value)
	{
		central_values[index] = value;
	}

	int getTotalDataPoints()
	{
		return rowNumbers.size();
	}

	float getSum(int attributeNumber)
	{
		return attributeSummation[attributeNumber];
	}

	int getClusterID()
	{
		return clusterId;
	}

	void setClusterID(int value)
	{
		clusterId = value;
	}
};

class KMeans
{
private:
	int K; // number of clusters
	int totalDataPoints;
	vector<Attribute> attributes;

	// return ID of nearest center (uses euclidean distance)
	int getIDNearestCenter(int dataPointRowNumber)
	{
		double sum = 0.0, min_dist;
		int id_cluster_center = 0;

		for (int i = 0; i < (attributes).size(); i++)
		{
			if (!(attributes).at(i).isIgnored)
			{
				double centroidValue = clusters[0].getCentralValue(i);
				double  dataPoint = (attributes).at(i).attributeValues[dataPointRowNumber];
				sum += pow(centroidValue - dataPoint, 2.0);
			}
		}

		min_dist = sqrt(sum);

		for (int i = 1; i < K; i++)
		{
			double dist;
			sum = 0.0;

			for (int j = 0; j < (attributes).size(); j++)
			{
				if (!(attributes).at(j).isIgnored)
				{
					double centroidValue = clusters[i].getCentralValue(j);
					double  dataPoint = (attributes).at(j).attributeValues[dataPointRowNumber];
					sum += pow(centroidValue - dataPoint, 2.0);
				}
			}

			dist = sqrt(sum);

			if (dist < min_dist)
			{
				min_dist = dist;
				id_cluster_center = i;
			}
		}

		return id_cluster_center + 1;
	}

public:
	vector<Cluster> clusters;
	vector<int> rowClusterIds;
	KMeans(int K, vector<Attribute> & attributes)
	{
		this->K = K;
		this->totalDataPoints = (attributes).at(0).attributeValues.size();
		this->rowClusterIds = vector<int>(totalDataPoints, 0);
		this->attributes = attributes;
	}

	KMeans(int K, vector<Attribute> & attributes, vector<int> rowNumbers)
	{
		vector<Attribute> attributecopy = vector<Attribute>(attributes.begin(), attributes.end());

		for (std::vector<Attribute>::iterator it = attributecopy.begin(); it != attributecopy.end(); ++it)
		{
			vector<float> attributeValues;
			for (int rowNo = 0; rowNo < rowNumbers.size(); rowNo++)
			{
				if(!(*it).isIgnored)
					attributeValues.push_back((*it).attributeValues.at(rowNumbers[rowNo]));
			}
			(*it).attributeValues = attributeValues;
		}
		this->K = K;
		this->totalDataPoints = (attributecopy).at(0).attributeValues.size();
		this->rowClusterIds = vector<int>(totalDataPoints, 0);
		this->attributes = attributecopy;
	}

	void run()
	{
		if (K > totalDataPoints)
			return;

		vector<int> prohibited_indexes;

		// choose K distinct values for the centers of the clusters
		for (int i = 0; i < K; i++)
		{
			while (true)
			{
				srand(time(NULL));
				int index_point = rand() % totalDataPoints; // Pick random DataPoint
				if (find(prohibited_indexes.begin(), prohibited_indexes.end(),index_point) == prohibited_indexes.end())
				{
					prohibited_indexes.push_back(index_point);
					vector<float> centralValues = vector<float>((attributes).size(), 0);
					for (int j = 0; j<(attributes).size(); j++)
					{
						if(!(attributes).at(j).isIgnored)
							centralValues[j] = (attributes).at(j).attributeValues[index_point];
					}
					Cluster cluster(i+1, centralValues, attributes);
					cluster.addPoint(index_point);
					clusters.push_back(cluster);
					rowClusterIds[index_point] = i + 1;
					break;
				}
			}
		}

		int iter = 1;

		while (true)
		{
			bool done = true;
			// associates each point to the nearest center
			for (int i = 0; i < totalDataPoints; i++)
			{
				int id_old_cluster = rowClusterIds[i];
				int id_nearest_center = getIDNearestCenter(i);

				if (id_old_cluster != id_nearest_center)
				{
					if (id_old_cluster != 0)
						clusters[id_old_cluster - 1].removePoint(i);

					rowClusterIds[i] = id_nearest_center;
					clusters[id_nearest_center - 1].addPoint(i);
					done = false;
				}
			}

			// recalculating the center of each cluster
			for (int i = 0; i < K; i++)
			{
				for (int j = 0; j < (attributes).size(); j++)
				{
					float total_points_cluster = clusters[i].getTotalDataPoints();
					double sum = 0.0;

					if (total_points_cluster > 0)
					{
						clusters[i].setCentralValue(j, clusters[i].getSum(j) / total_points_cluster);
					}
				}
			}

			if (done == true)
			{
				//Break iteration
				break;
			}

			iter++;
		}
	}
};

class BisectingKMeans
{
private:
	int K; // number of clusters
	int totalDataPoints;
	vector<Attribute> *attributes;
public:
	vector<Cluster> clusters;
	vector<int> rowClusterIds;
	BisectingKMeans(int K, vector<Attribute> & attributes)
	{
		this->K = K;
		this->totalDataPoints = (attributes).at(0).attributeValues.size();
		this->rowClusterIds = vector<int>(totalDataPoints, 0);
		this->attributes = &attributes;
	}
	void run()
	{
		KMeans kmeansBisect((int)2, *attributes);
		kmeansBisect.run();
		vector<Cluster> bisectedCusters = vector<Cluster>(kmeansBisect.clusters);
		int totalClusters = 2;
		while (totalClusters < K)
		{// Divide Clusters further comparing their SSE
			int maxtotalSSECluster = 0;
			int maxtotalSSEClusterID = 0;
			Cluster maxCluster;
			for(std::vector<Cluster>::iterator it = bisectedCusters.begin(); it != bisectedCusters.end(); ++it)
			{
				int currentTotalSSE = (*it).getTotalSSE();
				if (currentTotalSSE > maxtotalSSECluster)
				{
					maxtotalSSECluster = currentTotalSSE;
					maxtotalSSEClusterID = (*it).clusterId;
					maxCluster = (*it);
				}
			}
			//Remove Cluster from Bisected clusterList
			for (std::vector<Cluster>::iterator iter = bisectedCusters.begin(); iter != bisectedCusters.end(); ++iter)
			{
				if ((*iter).clusterId == maxtotalSSEClusterID)
				{
					bisectedCusters.erase(iter);
					break;
				}
			}

			int clusterId = 1; // Set updated ClusterId's
			for (std::vector<Cluster>::iterator it = bisectedCusters.begin(); it != bisectedCusters.end(); ++it)
			{
				try
				{
					(*it).setClusterID(clusterId);
					for (int rowNo = 0; rowNo < (*it).rowNumbers.size(); ++rowNo)
					{
					
							rowClusterIds[(*it).rowNumbers[rowNo]] = clusterId;
					
					}
				}
				catch (exception& e)
				{
					//throw e;
				}
				clusterId++;
			}
			
			KMeans kmeansBisect((int)2, *attributes, maxCluster.rowNumbers);
			kmeansBisect.run();
			//Set Correct clusterId's to corresponding rows.
			for (vector<Cluster>::iterator clus = kmeansBisect.clusters.begin(); clus != kmeansBisect.clusters.end(); ++clus)
			{
				(*clus).setClusterID(clusterId);
				for (vector<int>::iterator rowNo = (*clus).rowNumbers.begin(); rowNo != (*clus).rowNumbers.end(); ++rowNo)
				{
					rowClusterIds[maxCluster.rowNumbers[*rowNo]]= clusterId;
				}
				bisectedCusters.push_back((*clus));
				clusterId++;
			}
			totalClusters++;
		}
		clusters = bisectedCusters;
	}
};

class FileOperations
{
private:
	ofstream fileStream;
public: 
	FileOperations(string fileName)
	{
		this->fileStream.open(fileName);
	}
	void WriteMetaDataToFile(vector<Attribute> attributes)
	{
		Relation relation;

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
		
		fileStream << fileBaseContent;
	}

	void CloseFile()
	{
		fileStream.close();
	}

	void WriteKmeanBasicCluster(KMeans kmeans)
	{
		for (std::vector<Cluster>::iterator it = kmeans.clusters.begin(); it != kmeans.clusters.end(); ++it)
		{
			for (std::vector<float>::iterator itCentralValues = (*it).central_values.begin(); itCentralValues != (*it).central_values.end(); ++itCentralValues)
			{
				fileStream << std::setw(11) << std::setprecision(6) << std::fixed << (*itCentralValues);
			}
			fileStream << std::setw(11) << std::setprecision(6) << std::fixed << (*it).clusterId;
			fileStream << "\n";
		}
	}

	void WriteKmeanBasicCluster(BisectingKMeans kmeans)
	{
		for (std::vector<Cluster>::iterator it = kmeans.clusters.begin(); it != kmeans.clusters.end(); ++it)
		{
			for (std::vector<float>::iterator itCentralValues = (*it).central_values.begin(); itCentralValues != (*it).central_values.end(); ++itCentralValues)
			{
				fileStream << std::setw(11) << std::setprecision(6) << std::fixed << (*itCentralValues);
			}
			fileStream << std::setw(11) << std::setprecision(6) << std::fixed << (*it).clusterId;
			fileStream << "\n";
		}
	}

	void WriteAttributesWithClusterId(vector<Attribute> attributes, KMeans kmeans)
	{
		int attributeRowCount = 1;
		if (!attributes.at(0).isIgnored)
		{
			attributeRowCount = attributes.at(0).attributeValues.size();
		}
		else
		{
			attributeRowCount = attributes.at(1).attributeValues.size();
		}

		int attributeRowNumber = 0;

		while (attributeRowNumber < attributeRowCount)
		{
			//if (!(*it).isIgnored)
			for (std::vector<Attribute>::iterator it = attributes.begin(); it != attributes.end(); ++it)
			{
				if (!(*it).isIgnored)
				{
					fileStream << std::setw(11) << std::setprecision(6) << std::fixed << (*it).attributeValues.at(attributeRowNumber);
				}
				else if ((*it).attributeName != "cluster")
				{
					fileStream << std::setw(11) << std::setprecision(6) << std::fixed << float(0);
				}
			}
			fileStream << std::setw(11) << std::setprecision(6) << std::fixed << kmeans.rowClusterIds.at(attributeRowNumber);
			fileStream << "\n";
			attributeRowNumber++;
		}
	}

	void WriteAttributesWithClusterId(vector<Attribute> attributes, BisectingKMeans kmeans)
	{
		int attributeRowCount = 1;
		if (!attributes.at(0).isIgnored)
		{
			attributeRowCount = attributes.at(0).attributeValues.size();
		}
		else
		{
			attributeRowCount = attributes.at(1).attributeValues.size();
		}

		int attributeRowNumber = 0;

		while (attributeRowNumber < attributeRowCount)
		{

			for (std::vector<Attribute>::iterator it = attributes.begin(); it != attributes.end(); ++it)
			{
				if (!(*it).isIgnored)
				{
					fileStream << std::setw(11) << std::setprecision(6) << std::fixed << (*it).attributeValues.at(attributeRowNumber);
				}
				else if((*it).attributeName!="cluster")
				{
					fileStream << std::setw(11) << std::setprecision(6) << std::fixed << float(0);
				}
			}
			fileStream << std::setw(11) << std::setprecision(6) << std::fixed << kmeans.rowClusterIds.at(attributeRowNumber);
			fileStream << "\n";
			attributeRowNumber++;
		}
	}

	void WriteUnnormalizedKmeanBasicCluster(KMeans kmeans, vector<Attribute> *attributes)
	{
		for (std::vector<Cluster>::iterator it = kmeans.clusters.begin(); it != kmeans.clusters.end(); ++it)
		{
			int attributeNo = 0;
			for (std::vector<float>::iterator itCentralValues = (*it).central_values.begin(); itCentralValues != (*it).central_values.end(); ++itCentralValues)
			{
				Attribute * currentAttr = &(*attributes).at(attributeNo);
				if ((*currentAttr).isIgnored && attributeNo < ((*attributes).size()-1))
				{
					attributeNo++;
					currentAttr = &(*attributes).at(attributeNo);
				}
				fileStream << std::setw(11) << std::setprecision(6) << std::fixed << (*currentAttr).GetInverseZScoreNormalization((*itCentralValues));
				attributeNo++;
			}
			fileStream << std::setw(11) << std::setprecision(6) << std::fixed << (*it).clusterId;
			fileStream << "\n";
		}
	}
	
	void WriteUnnormalizedKmeanBasicCluster(BisectingKMeans kmeans, vector<Attribute> *attributes)
	{
		for (std::vector<Cluster>::iterator it = kmeans.clusters.begin(); it != kmeans.clusters.end(); ++it)
		{
			int attributeNo = 0;
			for (std::vector<float>::iterator itCentralValues = (*it).central_values.begin(); itCentralValues != (*it).central_values.end(); ++itCentralValues)
			{
				Attribute * currentAttr = &(*attributes).at(attributeNo);
				if ((*currentAttr).isIgnored && attributeNo < ((*attributes).size() - 1))
				{
					attributeNo++;
					currentAttr = &(*attributes).at(attributeNo);
				}
				fileStream << std::setw(11) << std::setprecision(6) << std::fixed << (*currentAttr).GetInverseZScoreNormalization((*itCentralValues));
				attributeNo++;
			}
			fileStream << std::setw(11) << std::setprecision(6) << std::fixed << (*it).clusterId;
			fileStream << "\n";
		}
	}
};

int main(int argc, char *argv[])
{
	std::string inputFile;
	std::string line;
	std::string ignoreAttribute = "class";
	int NumberOfClusters = 1;
	bool applyNormalization = false;
	std::string outputClusterCenterBasic, outputClusteringUnnormalizedBisecting, outputClusteringUnnormalizedBasic, outputClusterCenterUnnormalizedBisecting, outputClusterCenterUnnormalizedBasic, outputClusteringBasic, outputClusterCenterNormalizedBasic, outputClusteringNormalizedBasic, outputClusterCenterBisecting, outputClusteringBisecting, outputClusterCenterNormalizedBisecting, outputClusteringNormalizedBisecting;
	Relation relation;

	if (argc < 5)
	{//Kmeans_ClusterBasic –i kmtest.arff –K 5 –c class –normalize
		//YourLoginNormalize –i InputFile –c classattribute
		std::cout << "argc "<<argc<< "\n";
		std::cout << "Wrong set of arguments. \nArguments format: -i <InputFile> –K <NumberOfClusters> -c <classattribute>{Optional} -normalize {Optional}\n";
	}
	else
	{
		inputFile = argv[2];
		NumberOfClusters = atoi(argv[4]);
		if (argc > 7 && argv[7] != NULL)
		{
			applyNormalization = true;
			if ((string)argv[5] == "-c")
				ignoreAttribute = argv[6];
		}
		else if (argc > 6 && argv[6] != NULL)
		{
			if ((string)argv[5] == "-c")
				ignoreAttribute = argv[6];
		}
		else if (argc > 5)
		{
			if ((string)argv[5] == "-normalize")
				applyNormalization = true;
		}

		outputClusterCenterBasic = "Kmeans_ClusterCenterBasic" + inputFile; 
		outputClusterCenterBisecting = "Kmeans_ClusterCenterBisecting" + inputFile; 
		outputClusteringBisecting = "Kmeans_ClusteringBisecting" + inputFile; 
		outputClusteringBasic = "Kmeans_ClusteringBasic" + inputFile; 
		outputClusterCenterNormalizedBasic = "Kmeans_ClusterCenterNormalizedBasic" + inputFile; 
		outputClusterCenterNormalizedBisecting = "Kmeans_ClusterCenterNormalizedBisecting" + inputFile; 
		outputClusteringNormalizedBasic = "Kmeans_ClusteringNormalizedBasic" + inputFile; 
		outputClusteringNormalizedBisecting = "Kmeans_ClusteringNormalizedBisecting" + inputFile; 
		outputClusterCenterUnnormalizedBasic = "Kmeans_ClusterCenterUnnormalizedBasic" + inputFile; 
		outputClusterCenterUnnormalizedBisecting = "Kmeans_ClusterCenterUnnormalizedBisecting" + inputFile; 
		outputClusteringUnnormalizedBasic = "Kmeans_ClusteringUnnormalizedBasic" + inputFile; 
		outputClusteringUnnormalizedBisecting = "Kmeans_ClusteringUnnormalizedBisecting" + inputFile; 

		std::ifstream infile(inputFile.c_str());
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

		//Apply KMeans for all attributes
		KMeans kmeans(NumberOfClusters, attributes);
		kmeans.run();

		//Apply BisectingKMeans for all attributes
		BisectingKMeans bisectingKMeans(NumberOfClusters, attributes);
		bisectingKMeans.run();

		Attribute clusterAttribute;
		clusterAttribute.attributeName = "cluster";
		clusterAttribute.attributeType = "real";
		clusterAttribute.isIgnored = true;
		attributes.push_back(clusterAttribute);


		int attributeRowCount = 1;
		if (!attributes.at(0).isIgnored)
		{
			attributeRowCount = attributes.at(0).attributeValues.size();
		}
		else
		{
			attributeRowCount = attributes.at(1).attributeValues.size();
		}		

		if (!applyNormalization)
		{
			//Basic K-Means Cluster Centers: A text file (“YourLoginClusterCenterBasicKInputFile”) containing the each of the cluster centers (means), one cluster per line
			FileOperations clusterCenterBasicFile(outputClusterCenterBasic.c_str());
			clusterCenterBasicFile.WriteMetaDataToFile(attributes);
			clusterCenterBasicFile.WriteKmeanBasicCluster(kmeans);
			clusterCenterBasicFile.CloseFile();
			
			// Basic K-Means Clustering: A text file (“YourLoginClusteringBasicKInputFile”) containing cluster ids for each pattern vector in the source file
			FileOperations clusterBasic(outputClusteringBasic.c_str());
			clusterBasic.WriteMetaDataToFile(attributes);
			clusterBasic.WriteAttributesWithClusterId(attributes, kmeans);
			clusterBasic.CloseFile();

			// Bisecting K-means Cluster Centers: 
			FileOperations clusterCenterBisectingBasicFile(outputClusterCenterBisecting.c_str());
			clusterCenterBisectingBasicFile.WriteMetaDataToFile(attributes);
			clusterCenterBisectingBasicFile.WriteKmeanBasicCluster(bisectingKMeans);
			clusterCenterBisectingBasicFile.CloseFile();

			// Bisecting K-means Clustering:
			FileOperations clusteringBisecting(outputClusteringBisecting.c_str());
			clusteringBisecting.WriteMetaDataToFile(attributes);
			clusteringBisecting.WriteAttributesWithClusterId(attributes, bisectingKMeans);
			clusteringBisecting.CloseFile();
		}
		else
		{
			/* NORMALIZED OUTPUT FILES */
			std::vector<Attribute> normalizedAttributes = vector<Attribute>(attributes.begin(), attributes.end() - 1);
			for (std::vector<Attribute>::iterator it = normalizedAttributes.begin(); it != normalizedAttributes.end(); ++it)
			{
				if (!(*it).isIgnored)
					(*it).GetStandardDeviation();
			}
			int normalizedAttributeRowNumber = 0;
			while (normalizedAttributeRowNumber < attributeRowCount)
			{
				for (std::vector<Attribute>::iterator it = normalizedAttributes.begin(); it != normalizedAttributes.end(); ++it)
				{//Set normalized values to each DataPoint
					if (!(*it).isIgnored)
					{
						(*it).attributeValues.at(normalizedAttributeRowNumber) = (*it).GetZScoreNormalization((*it).attributeValues.at(normalizedAttributeRowNumber));
					}
				}
				normalizedAttributeRowNumber++;
			}

			//Apply KMeans on Normalized Data
			KMeans kmeansOnNormalizedAttributes(NumberOfClusters, normalizedAttributes);
			kmeansOnNormalizedAttributes.run();

			//Apply Bisecting KMeans on Normalized Data
			BisectingKMeans bisectingKMeansOnNormalizedAttributes(NumberOfClusters, normalizedAttributes);
			bisectingKMeansOnNormalizedAttributes.run();
			normalizedAttributes.push_back(clusterAttribute);

			// YourLoginClusterCenterNormalizedBasicKInputFile : (normalized) cluster centers using basic K-means
			FileOperations clusterCenterNormalizedBasicFile(outputClusterCenterNormalizedBasic.c_str());
			clusterCenterNormalizedBasicFile.WriteMetaDataToFile(normalizedAttributes);
			clusterCenterNormalizedBasicFile.WriteKmeanBasicCluster(kmeansOnNormalizedAttributes);
			clusterCenterNormalizedBasicFile.CloseFile();

			// YourLoginClusteringNormalizedBasicKInputFile : (normalized) clustering results for basic K-means
			FileOperations clusterNormalizedBasic(outputClusteringNormalizedBasic.c_str());
			clusterNormalizedBasic.WriteMetaDataToFile(normalizedAttributes);
			clusterNormalizedBasic.WriteAttributesWithClusterId(normalizedAttributes, kmeansOnNormalizedAttributes);
			clusterNormalizedBasic.CloseFile();

			//YourLoginClusterCenterNormalizedBisectingKInputFile : (normalized) cluster centers using bisecting K-means
			FileOperations clusterCenterNormalizedBisecting(outputClusterCenterNormalizedBisecting.c_str());
			clusterCenterNormalizedBisecting.WriteMetaDataToFile(normalizedAttributes);
			clusterCenterNormalizedBisecting.WriteKmeanBasicCluster(bisectingKMeansOnNormalizedAttributes);
			clusterCenterNormalizedBisecting.CloseFile();

			// YourLoginClusteringNormalizedBisectingKInputFile: : (normalized) clustering results for basic K-means
			FileOperations clusteringNormalizedBisecting(outputClusteringNormalizedBisecting.c_str());
			clusteringNormalizedBisecting.WriteMetaDataToFile(normalizedAttributes);
			clusteringNormalizedBisecting.WriteAttributesWithClusterId(normalizedAttributes, bisectingKMeansOnNormalizedAttributes);
			clusteringNormalizedBisecting.CloseFile();

			//YourLoginClusterCenterUnnormalizedBasicKInputFile : cluster centers in the original domain using basic K-means
			FileOperations clusterCenterUnnormalizedBasic(outputClusterCenterUnnormalizedBasic.c_str());
			clusterCenterUnnormalizedBasic.WriteMetaDataToFile(normalizedAttributes);
			clusterCenterUnnormalizedBasic.WriteUnnormalizedKmeanBasicCluster(kmeansOnNormalizedAttributes,&attributes);
			clusterCenterUnnormalizedBasic.CloseFile();

			//YourLoginClusterCenterUnnormalizedBisectingKInputFile : cluster centers in the original domain using bisecting K-means
			FileOperations clusterCenterUnnormalizedBisecting(outputClusterCenterUnnormalizedBisecting.c_str());
			clusterCenterUnnormalizedBisecting.WriteMetaDataToFile(normalizedAttributes);
			clusterCenterUnnormalizedBisecting.WriteUnnormalizedKmeanBasicCluster(bisectingKMeansOnNormalizedAttributes, &attributes);
			clusterCenterUnnormalizedBisecting.CloseFile();

			//YourLoginClusteringUnnormalizedBasicKInputFile : clustering results in the original domain for basic K-means
			FileOperations clusteringUnnormalizedBasic(outputClusteringUnnormalizedBasic.c_str());
			clusteringUnnormalizedBasic.WriteMetaDataToFile(attributes);
			clusteringUnnormalizedBasic.WriteAttributesWithClusterId(attributes, kmeans);
			clusteringUnnormalizedBasic.CloseFile();

			// YourLoginClusteringUnnormalizedBisectingKInputFile: clustering results in the original domain for bisecting K-means
			FileOperations clusteringUnnormalizedBisecting(outputClusteringUnnormalizedBisecting.c_str());
			clusteringUnnormalizedBisecting.WriteMetaDataToFile(attributes);
			clusteringUnnormalizedBisecting.WriteAttributesWithClusterId(attributes, bisectingKMeans);
			clusteringUnnormalizedBisecting.CloseFile();
		}
	}
	return 0;
}
