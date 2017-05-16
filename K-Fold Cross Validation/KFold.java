///******************************************************
//
//Author:-	Rohit Pawar
//Date:-	4/18/2017
//
//******************************************************/

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

class AttributeTypeProbability
{
	public String attributeName;
	public float decision;
	public float classValue;
	public Integer count = 0;
}

class FoldInfo
{
	public int foldNumber;
	public Set<Integer> rowNumbers;
	public double accuracy;
}
class Attribute
{
	private int foldNumber = 1;
	public String attributeName;
	public String[] attributeType;
	public List<Float> attributeValues = new ArrayList<Float>();
	public Boolean isClassAttribute = false;
	public 	void AddAttributeValues(Float value)
	{// Add next attribute value
		attributeValues.add(value);
	}
	
	//Calculate Probability for each attribute value in an attribute according to the training KFolds
	public ArrayList<AttributeTypeProbability> attributeProbabilityByClassValue(Set<Integer> testingRows,Attribute classAttribute)
	{
		int rowCount = 0;
		ArrayList<AttributeTypeProbability> attrTypeProbabilityList = new ArrayList<AttributeTypeProbability>();
		
		for(Float value: attributeValues)
		{
			if(!testingRows.contains(rowCount))
			{
				float classRowValue = classAttribute.attributeValues.get(rowCount);
				boolean attrTypeProbabilityFound = false;
				for(AttributeTypeProbability attrTypeProbability : attrTypeProbabilityList)
				{
					if(attrTypeProbability.decision == value && attrTypeProbability.classValue == classRowValue)
					{
						attrTypeProbability.count++;
						attrTypeProbabilityFound = true;
					}
				}
				if(!attrTypeProbabilityFound)
				{
					AttributeTypeProbability attrTypeProbability = new AttributeTypeProbability();
					attrTypeProbability.attributeName = attributeName;
					attrTypeProbability.decision = value;
					attrTypeProbability.classValue = classRowValue;
					attrTypeProbability.count++;
					attrTypeProbabilityList.add(attrTypeProbability);
				}
			}
			rowCount++;
		}
		return attrTypeProbabilityList;
	}
}

class Relation
{
public String realtionName;
};

class ClassAttribute extends Attribute
{
	//Constructor
	public ClassAttribute(Attribute attr)
	{
		this.attributeName= attr.attributeName;
		this.attributeType = attr.attributeType;
		this.attributeValues = attr.attributeValues;
	}
	
	public float getPositiveCount()
	{
		return this.getPositiveCount(null);
	}
	
	public float getPositiveValue()
	{
		return new TreeSet<>(attributeValues).first();
	}
	
	public float getNegativeValue()
	{
		return new TreeSet<>(attributeValues).last();
	}
	
	public float getPositiveCount(Set<Integer> rowNumbers)
	{
		float countPositive = 0;
		float positiveValue = getPositiveValue();
		if(rowNumbers != null)
		{
			for(int rowNumber : rowNumbers)
			{
				if(attributeValues.get(rowNumber) == positiveValue)
					countPositive++;
			}
		}
		else
		{
			for(Float attrValue : attributeValues)
			{
				if(attrValue == positiveValue)
					countPositive++;
			}
		}
		
		return countPositive;
	}
	
	public float getNegativeCount()
	{
		return this.getNegativeCount(null);
	}
	
	public float getNegativeCount(Set<Integer> rowNumbers)
	{
		float countNegative = 0;
		float negativeValue = getNegativeValue();
		if(rowNumbers != null)
		{
			for(int rowNumber : rowNumbers)
			{
				if(attributeValues.get(rowNumber) == negativeValue)
					countNegative++;
			}
		}
		else
		{
			for(Float attrValue : attributeValues)
			{
				if(attrValue == negativeValue)
					countNegative++;
			}
		}
		
		return countNegative;
	}

	public int positiveTrueCount = 0;
	
	public int positiveFalseCount = 0;
	
	public int negativeTrueCount = 0;
	
	public int negativeFalseCount = 0;
	
	private int positiveFoldCount = 1;
	
	private int negativeFoldCount = 1;
	
	//PositivePrior Probability
	public double getPriorPositiveProbability()
	{
		double priorPositiveProbability = 0;
		
		priorPositiveProbability= Math.round(getPositiveCount()/attributeValues.size()*100.0)/100.0;
		
		return priorPositiveProbability;
	}

	//NegativePrior Probability
	public double getPriorNegativeProbability()
	{
		double priorNegativeProbability = 0;
		
		priorNegativeProbability= Math.round(getNegativeCount()/attributeValues.size()*100.0)/100.0;
		
		return priorNegativeProbability;
	}

	//Get Folds for the given Dataset
	public List<FoldInfo> getFolds(int kFolds)
	{
		List<FoldInfo> foldInfoList = new ArrayList<FoldInfo>();
		for(int i=1; i<=kFolds; i++)
		{
			FoldInfo foldInfo = new FoldInfo();
			foldInfo.foldNumber = i;
			foldInfo.rowNumbers = new HashSet<Integer>();
			foldInfoList.add(foldInfo);
		}
		int rowCount = 0;
		float positiveValue = getPositiveValue();
		for(Float attrValue : attributeValues)
		{
			if(attrValue == positiveValue)
			{
				foldInfoList.get(positiveFoldCount - 1).rowNumbers.add(rowCount);
				positiveFoldCount++;
				if(positiveFoldCount > kFolds)
					positiveFoldCount = 1;
			}
			else
			{
				foldInfoList.get(negativeFoldCount - 1).rowNumbers.add(rowCount);
				negativeFoldCount++;
				if(negativeFoldCount > kFolds)
					negativeFoldCount = 1;
			}
				rowCount++;
		}
		return foldInfoList;
	}
}

public class KFold {

	public List<Attribute> attributes = new ArrayList<Attribute>();
	public ClassAttribute classAttribute = null;
	public Relation relation = null;
	public List<FoldInfo> foldInfoList= new ArrayList<FoldInfo>();

	//Export file with Predicted Class attribute And Export file with Overall Accuracy and Confusion Matrix
	public void ExportNaiveBayesClassificationPredictedClass(Integer kFolds,String inputFile)
	{
		StringBuilder output = new StringBuilder();
		float classPositiveValue = classAttribute.getPositiveValue();
		float classNegativeValue = classAttribute.getNegativeValue();
		output.append("@relation "+ relation.realtionName+"\n\n");
		int totalDataSetCount = attributes.get(0).attributeValues.size();
		for(Attribute attr: attributes)
		{
			if(attr.attributeName != "bayesClass")
				output.append("@attribute "+ attr.attributeName+ " "+Arrays.toString(attr.attributeType).replace("[", "{").replace("]", "}")+"\n");
			else
				output.append("@attribute bayesClass real\n");
		}
		
		output.append("\n@data\n");
		
		for(int i=0;i<totalDataSetCount;i++)
		{
			for(Attribute attr: attributes)
			{
				output.append(" "+ Math.round(attr.attributeValues.get(i)));			
			}
			output.append("\n");
		}
		
		try(PrintWriter out = new PrintWriter( "KFold_"+(int)kFolds+"FoldClassification"+inputFile))
		{
		    out.println(output.toString());
		}
		catch (Exception ex)
		{
			
		}
		DecimalFormat df = new DecimalFormat();
		df.setMaximumFractionDigits(2);
		StringBuilder foldAccuracyOutput = new StringBuilder();
		float foldAccuracySum = 0.0f;
		for(FoldInfo foldinfo: foldInfoList)
		{
			foldAccuracyOutput.append("\nFold "+foldinfo.foldNumber+" Accuracy :" + df.format(foldinfo.accuracy));
			foldAccuracySum += foldinfo.accuracy;
		}
		StringBuilder accuracyOutput = new StringBuilder();
		//Confusion Matrix
		accuracyOutput.append("\n\n\t\tConfusion Matrix\n\n");
		accuracyOutput.append("\t\t |\tPredicted\n");
		accuracyOutput.append("\t\t |\t"+Math.round(classPositiveValue)+"\t\t"+Math.round(classNegativeValue));
		accuracyOutput.append("\n---------+--------------\n");
		accuracyOutput.append("Actual\t"+Math.round(classPositiveValue)+"|\t"+classAttribute.positiveTrueCount+"\t\t"+classAttribute.positiveFalseCount+"\n");
		accuracyOutput.append("\t\t"+Math.round(classNegativeValue)+"|\t"+classAttribute.negativeFalseCount+"\t\t"+classAttribute.negativeTrueCount+"\n\n\n");
		accuracyOutput.append("-----------------------\n");
		accuracyOutput.append("Overall Accuracy :" + df.format((foldAccuracySum)/(float)foldInfoList.size()));
		accuracyOutput.append("\n-----------------------");
		accuracyOutput.append("\n");
		accuracyOutput.append(foldAccuracyOutput);
		try(PrintWriter out = new PrintWriter( "KFold_"+(int)kFolds+"FoldConfusion"+inputFile.replace(".arff", "")+".txt"))
		{
		    out.println(accuracyOutput.toString());
		}
		catch (Exception ex)
		{
			
		}
		
	}
	
	//Product of probability of each feature from an tuple/row
	public float featuresProbabilityProduct(Integer rowNumber,Set<Integer> testingRows,float classValue)
	{
		float product = 1.0f;
		HashMap<String, ArrayList<AttributeTypeProbability>> attributeProbability = new HashMap<String,ArrayList<AttributeTypeProbability>>();
		for(Attribute attr: attributes)
		{//Set All Probabilities for Attribute ValueTypes
			if(!attr.isClassAttribute)
			{
				attributeProbability.put(attr.attributeName, attr.attributeProbabilityByClassValue(testingRows,classAttribute));
			}
		}
		double classCount = 1.0;
		if(classAttribute.getPositiveValue() == classValue)
			classCount = classAttribute.getPositiveCount();
		else
			classCount = classAttribute.getNegativeCount();
		
		//Use AttributeProbilities for getting Probility
		for(Attribute attr:attributes)
		{
			if(!attr.isClassAttribute)
			{
				float testValue = attr.attributeValues.get(rowNumber);
				ArrayList<AttributeTypeProbability> attrTypeProbabilityList= attributeProbability.get(attr.attributeName);
				Integer attrDecisionClassValueCount = 0;
				for(AttributeTypeProbability attrTypeProbability: attrTypeProbabilityList)
				{
					if(attrTypeProbability.classValue == classValue && attrTypeProbability.decision == testValue)
					{
						attrDecisionClassValueCount = attrTypeProbability.count;
						break;
					}
				}
				if(attrDecisionClassValueCount!=0)
				{
					product*=(attrDecisionClassValueCount/classCount);
				}
				else
				{
					return 0;
				}
			}
		}
		
		return product;
	}
	
	//Start training using Bayes Classifiers on the testing Rows divided by Kfolds.
	public void trainBayesClassifier(Set<Integer> testingRows,Attribute attr)
	{
		
		
		float category = 0.0f;
		for(Integer row: testingRows)
		{
			float featurePositiveProbabilityProduct = featuresProbabilityProduct(row,testingRows,classAttribute.getPositiveValue());
			float featureNegativeProbabilityProduct = featuresProbabilityProduct(row,testingRows,classAttribute.getNegativeValue());
			if(featurePositiveProbabilityProduct > featureNegativeProbabilityProduct)
				category = classAttribute.getPositiveValue();
			else
				category = classAttribute.getNegativeValue();
			
			attr.attributeValues.set(row, category);
		}
		
	}
	
	//Divide input into KFolds and get the classifiers for each tuple in test fold.
	public void trainBayesClassifierWithKFolds(Integer kFolds)
	{
		float classPositiveValue = classAttribute.getPositiveValue();
		float classNegativeValue = classAttribute.getNegativeValue();
		
		Attribute attr = new Attribute();
		attr.attributeName = "bayesClass";
		attr.attributeType = "real".split(",");
		attr.isClassAttribute = true;
		for (int i = 0; i < classAttribute.attributeValues.size(); i++) 
		{
			attr.attributeValues.add(0.0f);
		}
		foldInfoList = classAttribute.getFolds(kFolds);
		for(FoldInfo foldInfo : foldInfoList)
		{
			int foldpositiveTrueCount = 0;
			int foldpositiveFalseCount = 0;
			int foldnegativeTrueCount = 0;
			int foldnegativeFalseCount = 0;
			trainBayesClassifier(foldInfo.rowNumbers,attr);
			//Calculate Accuracy for this fold
			for(int row:foldInfo.rowNumbers)
			{
				float predictedValue = attr.attributeValues.get(row);
				float classValue = classAttribute.attributeValues.get(row);
				if(classValue == classPositiveValue)
				{//Original Value was positive
					if(classValue == predictedValue)
					{
						classAttribute.positiveTrueCount++;
						foldpositiveTrueCount++;
					}
					else
					{
						classAttribute.positiveFalseCount++;
						foldpositiveFalseCount++;
					}
				}
				else
				{// Original Value was negative
					if(classValue == predictedValue)
					{
						classAttribute.negativeTrueCount++;
						foldnegativeTrueCount++;
					}
					else
					{
						classAttribute.negativeFalseCount++;
						foldnegativeFalseCount++;
					}
				}
			}
			foldInfo.accuracy = Math.round((((foldpositiveTrueCount+foldnegativeTrueCount)*100)/(float)foldInfo.rowNumbers.size())*100.0)/100.0;
		}
		attributes.add(attr);
	}
	
	//Start of Code
	public static void main(String[] args) {
		KFold naiveBayesClassification = new KFold();
		String inputFileName;
		String classAttribute;
		Integer kFolds;
		
		naiveBayesClassification.relation = new Relation();
		if(args.length < 6)
		{
			System.out.println("Wrong set of arguments. \nArgument format -i <InputFile> -c <ClassAttribute> -T <TrainingPercentage> PercentageRange(0-100)");
		}
		inputFileName = args[1];
		classAttribute = args[3];
		kFolds = Integer.parseInt(args[5]);

		File file = new File(inputFileName);
		BufferedReader reader = null;

		try {
		    reader = new BufferedReader(new FileReader(file));
		    String text = null;

		    while ((text = reader.readLine()) != null) {
		    	text.toString();
				String s2;
				int flow = 0;
				int attributeCount = 0;
				if (text.contains("@relation"))
					flow = 1;
				else if (text.contains("@attribute"))
					flow = 2;
				else if (text.contains("@data"))
					continue;
				else if (!text.trim().isEmpty())
				{
					flow = 3;
				}
				Pattern pattern = null;
				Matcher matcher;
				switch (flow)
				{
					case 1:
						String[] relationValues = text.split(" ");
						naiveBayesClassification.relation.realtionName = relationValues[1];
						break;
					case 2:// Add attribute MetaData @attribute
						String attrName = new String();
						String attrType = new String();
						pattern = Pattern.compile("@attribute *");
						matcher = pattern.matcher(text);
						if (matcher.find()) {
							text = text.substring(matcher.end());
						}
						pattern = Pattern.compile(" ");
						matcher = pattern.matcher(text);
						if (matcher.find()) {
							attrName = text.substring(0, matcher.start());
							attrType = text.substring(matcher.end());
						}
						attrType=attrType.replaceAll("\\{", "").replaceAll("\\}","").replaceAll(" ", "");
						Attribute attr = new Attribute();
						attr.attributeName = attrName;
						attr.attributeType = attrType.split(",");
						if(attrName.contains("class"))
						{
							attr.isClassAttribute = true;
							naiveBayesClassification.classAttribute = new ClassAttribute(attr);
						}
						naiveBayesClassification.attributes.add(attr);
						break;
					case 3:// Add attribute values to the vector list.
						String[] values = text.trim().split(" ");
						for( String attributeValue : values ) {
							Float value = Float.parseFloat(attributeValue);
							naiveBayesClassification.attributes.get(attributeCount).AddAttributeValues(value);
							attributeCount++;
						}
						break;
					default:
						break;
				}
		    }
		    
		    naiveBayesClassification.trainBayesClassifierWithKFolds(kFolds);
		    naiveBayesClassification.ExportNaiveBayesClassificationPredictedClass(kFolds, inputFileName);
		} catch (FileNotFoundException e) {
		    e.printStackTrace();
		} catch (IOException e) {
		    e.printStackTrace();
		}
		catch(Exception ex)
		{
			System.out.println("");
		}
		finally 
		{
		    try {
		        if (reader != null) {
		            reader.close();
		        }
		    } 
		    catch (IOException e) {
		    }
		}
	}
	
}
