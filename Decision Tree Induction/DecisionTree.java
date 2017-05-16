///******************************************************
//
//Author:-	Rohit Pawar
//Date:-	4/1/2017
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
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

class Decision
{
	public String attributeName;
	public String decisionName;
	public Set<Integer> rowNumbers;
}

class Attribute
{
	public String attributeName;
	public String[] attributeType;
	public List<Float> attributeValues = new ArrayList<Float>();
	public Boolean isClassAttribute = false;
	public 	void AddAttributeValues(Float value)
	{// Add next attribute value
		attributeValues.add(value);
	}
	
	public Decision countDecision(Set<Integer> choosenRows, String decision)
	{
		int rowCount = 0;
		Decision decisionData = new Decision();
		decisionData.attributeName= this.attributeName ;
		decisionData.decisionName = decision.trim();
		decisionData.rowNumbers= new HashSet<Integer>();
		for(Float value : attributeValues)
		{
			if(value == Float.parseFloat(decision))
			{
				decisionData.rowNumbers.add(rowCount);
			}
			rowCount++;
		}
		
		if(choosenRows.size() > 0) // Find Intersection of the Previous rows and current rows
			decisionData.rowNumbers.retainAll(choosenRows);
		
		return decisionData;
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
		float positiveValue = new TreeSet<>(attributeValues).first();
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
		float negativeValue = new TreeSet<>(attributeValues).last();
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
}

class NodeDecision{
	public DecisionNode decisionNode = null;
	public boolean isClassification = false;
	public float classificationValue = 0;
	public Decision decision;
	public boolean isPrintable = true;
}

class DecisionNode{
	public Attribute attribute = null;
	public List<NodeDecision> nodeDecisions;
}



public class DecisionTree {

	public List<Attribute> attributes = new ArrayList<Attribute>();
	public ClassAttribute classAttribute = null;
	public Relation relation = null;
	
	//Export Decision Tree to file
	public void ExportDecisionTree(DecisionNode rootNode, float trainingPercentage, int level, String inputFile)
	{
		String outputString = printDecisionTree(rootNode,0);
		try(  PrintWriter out = new PrintWriter( "DecisionTreeTrain"+(int)trainingPercentage+inputFile.replace(".arff", "")+".dt" )  )
		{
		    out.println( outputString );
		}
		catch (Exception ex)
		{
			
		}
	}
	
	//Export file with Predicted Class attribute And Export file with Accuracy and Confusion Matrix
	public void ExportDecisionTreeWithPredictedClassAndAccuracy(DecisionNode rootNode, float trainingPercentage, String inputFile)
	{
		StringBuilder output = new StringBuilder();
		float classPositiveValue = classAttribute.getPositiveValue();
		float classNegativeValue = classAttribute.getNegativeValue();
		output.append("@relation "+ relation.realtionName+"\n\n");
		int totalDataSetCount = attributes.get(0).attributeValues.size();
		for(Attribute attr: attributes)
		{
			output.append("@attribute "+ attr.attributeName+ " "+Arrays.toString(attr.attributeType).replace("[", "{").replace("]", "}")+"\n");
		}
		
		output.append("@attribute dt_class real\n");
		
		output.append("\n@data\n");
		
		for(int i=0;i<totalDataSetCount;i++)
		{
			for(Attribute attr: attributes)
			{
				output.append(" "+ Math.round(attr.attributeValues.get(i)));
			}
			float predictedValue = getPredicatedClassByRow(rootNode, i);
			float classValue = classAttribute.attributeValues.get(i);
			if(classValue == classPositiveValue)
			{//Original Value was positive
				if(classValue == predictedValue)
					classAttribute.positiveTrueCount++;
				else
					classAttribute.positiveFalseCount++;
			}
			else
			{// Original Value was negative
				if(classValue == predictedValue)
					classAttribute.negativeTrueCount++;
				else
					classAttribute.negativeFalseCount++;
			}
			output.append(" "+Math.round(predictedValue)+"\n");
		}
		
		try(PrintWriter out = new PrintWriter( "DecisionTreeApply"+(int)trainingPercentage+inputFile.replace(".arff", "")+".dt"))
		{
		    out.println(output.toString());
		}
		catch (Exception ex)
		{
			
		}
		DecimalFormat df = new DecimalFormat();
		df.setMaximumFractionDigits(2);
		StringBuilder accuracyOutput = new StringBuilder();
		//Confusion Matrix
		accuracyOutput.append("\n\n\t\tConfusion Matrix\n\n");
		accuracyOutput.append("\t\t |\tPredicted\n");
		accuracyOutput.append("\t\t |\t"+Math.round(classPositiveValue)+"\t\t"+Math.round(classNegativeValue));
		accuracyOutput.append("\n---------+--------------\n");
		accuracyOutput.append("Actual\t"+Math.round(classPositiveValue)+"|\t"+classAttribute.positiveTrueCount+"\t\t"+classAttribute.positiveFalseCount+"\n");
		accuracyOutput.append("\t\t"+Math.round(classNegativeValue)+"|\t"+classAttribute.negativeFalseCount+"\t\t"+classAttribute.negativeTrueCount+"\n\n\n");
		accuracyOutput.append("---------------\n");
		accuracyOutput.append("Accuracy :" + df.format(((classAttribute.positiveTrueCount+classAttribute.negativeTrueCount)*100)/(float)totalDataSetCount));
		accuracyOutput.append("\n---------------");
		try(PrintWriter out = new PrintWriter( "DecisionTreeAccuracy"+(int)trainingPercentage+inputFile.replace(".arff", "")+".dt"))
		{
		    out.println(accuracyOutput.toString());
		}
		catch (Exception ex)
		{
			
		}
		
	}
	
	//Get Predicated class value By RowId
	public float getPredicatedClassByRow(DecisionNode decisionNode, int rowNumber)
	{
		int decision = Math.round(decisionNode.attribute.attributeValues.get(rowNumber));
		for(NodeDecision nodeDecision:decisionNode.nodeDecisions)
		{
			if(nodeDecision.decision.decisionName.equals(String.valueOf(decision)))
			{
				if(nodeDecision.isClassification)
					return nodeDecision.classificationValue;
				return getPredicatedClassByRow(nodeDecision.decisionNode, rowNumber);
			}
		}
		return 0;
	}
	
	//Print Decision Tree return in form of string.
	public String printDecisionTree(DecisionNode rootNode, int level)
	{
		StringBuilder decisionTreeString = new StringBuilder();
		String tabs = ""; 
		for(int i=0;i<level;i++)
		{
			tabs += "\t";
		}
		
		for(NodeDecision nodeDecisison: rootNode.nodeDecisions)
		{
			if(!nodeDecisison.isPrintable)
				continue;
			if(!nodeDecisison.isClassification)
			{
				decisionTreeString.append(tabs + nodeDecisison.decision.attributeName + " ="+nodeDecisison.decision.decisionName+"\n");
				if(nodeDecisison.decisionNode != null)
					decisionTreeString.append(printDecisionTree(nodeDecisison.decisionNode, level + 1));
			}
			else
			{
				decisionTreeString.append(tabs +nodeDecisison.decision.attributeName + " ="+nodeDecisison.decision.decisionName+ " "+ nodeDecisison.classificationValue+"\n");
			}
		}
		return decisionTreeString.toString();
	}
	
	//Get Decision Tree
	public DecisionNode getDecisionTree(float trainingPercentage)
	{
		Set<Integer> choosenRows = new HashSet<Integer>();
		int totalDataSetCount = attributes.get(0).attributeValues.size();
		int chooseRowCount = (int) ((totalDataSetCount * trainingPercentage) / 100);
		for(int i=0;i<chooseRowCount;i++)
		{
			choosenRows.add(i);
		}
	    Set<String> usedAttributes = new HashSet<String>();
		DecisionNode decisionNode = new DecisionNode();
		usedAttributes.add(classAttribute.attributeName);
		Attribute rootAttribute = findBestSplit(choosenRows, usedAttributes);
		usedAttributes.add(rootAttribute.attributeName);
		decisionNode.attribute = rootAttribute;
		decisionNode.nodeDecisions = new ArrayList<NodeDecision>();
		for(String decision: rootAttribute.attributeType)
		{
			NodeDecision nodeDecision = new NodeDecision();
			nodeDecision.decision = rootAttribute.countDecision(choosenRows, decision);
			nodeDecision.decisionNode = BuildDecisionTree(nodeDecision.decision.rowNumbers, usedAttributes);
			if(nodeDecision.decisionNode.attribute == null )
			{
				nodeDecision.isClassification = true;
				nodeDecision.classificationValue = (classAttribute.getPositiveCount(choosenRows) >= classAttribute.getNegativeCount(choosenRows)) ? 
														classAttribute.getPositiveValue() 
													: 	classAttribute.getNegativeValue();
			}
			decisionNode.nodeDecisions.add(nodeDecision);
		}
		return decisionNode;
    }
	
	//Build Decision Tree Recursively
	public DecisionNode BuildDecisionTree(Set<Integer> choosenRows, Set<String> usedAttributes)
	{
		DecisionNode decisionNode = new DecisionNode();
		if(choosenRows.size() > 0)
		{
			Attribute bestAttribute = findBestSplit(choosenRows, usedAttributes);
			if(bestAttribute.attributeName != null)
			{
				decisionNode.attribute = bestAttribute;
				decisionNode.nodeDecisions = new ArrayList<NodeDecision>();
				usedAttributes.add(bestAttribute.attributeName);
				for(String decision: bestAttribute.attributeType)
				{
					NodeDecision nodeDecision = new NodeDecision();
					nodeDecision.decision= bestAttribute.countDecision(choosenRows, decision);
					if(nodeDecision.decision.rowNumbers.size() > 0 && getEntropy(nodeDecision.decision) != 0)
					{
						nodeDecision.decisionNode = BuildDecisionTree(nodeDecision.decision.rowNumbers, usedAttributes);
						if(nodeDecision.decisionNode.attribute == null )
						{
							nodeDecision.isClassification = true;
							nodeDecision.classificationValue = (classAttribute.getPositiveCount(choosenRows) >= classAttribute.getNegativeCount(choosenRows)) ? 
																	classAttribute.getPositiveValue() 
																: 	classAttribute.getNegativeValue();
						}
					}
					else if(nodeDecision.decision.rowNumbers.size() == 0)
					{// No Records So Choose Majority but don't Print
						nodeDecision.isPrintable = false;
						nodeDecision.isClassification = true;
						nodeDecision.classificationValue = (classAttribute.getPositiveCount(choosenRows) >= classAttribute.getNegativeCount(choosenRows)) ? 
																classAttribute.getPositiveValue() 
															: 	classAttribute.getNegativeValue();
						
					}
					else//IsClassification = true
					{
						nodeDecision.isClassification = true;
						if(classAttribute.getPositiveCount(nodeDecision.decision.rowNumbers) == nodeDecision.decision.rowNumbers.size())
							nodeDecision.classificationValue = classAttribute.getPositiveValue();
						else
							nodeDecision.classificationValue = classAttribute.getNegativeValue();
					}
					decisionNode.nodeDecisions.add(nodeDecision);
				}
			}
		}
		return decisionNode;
	}
	
	//Calculate Information Gain
	public double getInformationGain(Set<Integer> choosenRows, Attribute attribute)
	{ 
		double gain = getEntropy(choosenRows);
		float totalDataSetCount = (float)choosenRows.size();
		for(String decision : attribute.attributeType)
		{
			Decision decisionData = attribute.countDecision(choosenRows, decision);
			if(decisionData.rowNumbers.size() > 0)
			{
				double decisionEntropy = getEntropy(decisionData);
				gain += - (decisionData.rowNumbers.size()/totalDataSetCount) * decisionEntropy;
			}
		}
		return gain;
	}
	
	//Find Next Best Split Attribute
	public Attribute findBestSplit(Set<Integer> choosenRows, Set<String> usedAttributes)
	{
		double currentGain=0;
		double maxGain = 0;
		Attribute bestAttribute = new Attribute();
		for(Attribute attr: attributes)
		{
			if(!usedAttributes.contains(attr.attributeName))
			{
				currentGain = getInformationGain(choosenRows, attr);
				if(currentGain > maxGain)
				{
					maxGain = currentGain;
					bestAttribute = attr;
				}
			}
		}
		return bestAttribute;
	}
	
	//Calculate Entropy
	public double getEntropy(Set<Integer> choosenRows)
	{
		int totalDataSetCount = choosenRows.size();
		double positiveClassValuesCount = classAttribute.getPositiveCount(choosenRows);
		double negativeClassValuesCount = classAttribute.getNegativeCount(choosenRows);
		return - nlogBase2(positiveClassValuesCount/totalDataSetCount) - nlogBase2(negativeClassValuesCount/totalDataSetCount);
	}
	
	//Overloaded
	public double getEntropy(Decision decisionData)
	{
		double positiveClassValuesCount = classAttribute.getPositiveCount(decisionData.rowNumbers);
		double negativeClassValuesCount = classAttribute.getNegativeCount(decisionData.rowNumbers);
		return - nlogBase2(positiveClassValuesCount/decisionData.rowNumbers.size()) - nlogBase2(negativeClassValuesCount/decisionData.rowNumbers.size());
	}
	
	//Calculate n * log base 2
	private double nlogBase2(double value) {
	    if ( value == 0 )
	      return 0;

	    return value * Math.log(value) / Math.log(2);
	  }
	
	//Start of Code
	public static void main(String[] args) {
		DecisionTree decisionTree = new DecisionTree();
		String inputFileName;
		String classAttribute;
		float trainingPercentage;
		
		decisionTree.relation = new Relation();
		if(args.length < 6)
		{
			System.out.println("Wrong set of arguments. \nArgument format -i <InputFile> -c <ClassAttribute> -T <TrainingPercentage> PercentageRange(0-100)");
		}
		inputFileName = args[1];
		classAttribute = args[3];
		trainingPercentage = Float.parseFloat(args[5]);

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
						decisionTree.relation.realtionName = relationValues[1];
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
						attrType=attrType.replaceAll("\\{", "").replaceAll("\\}","");
						Attribute attr = new Attribute();
						attr.attributeName = attrName;
						attr.attributeType = attrType.split(",");
						if(attrName.contains("class"))
						{
							attr.isClassAttribute = true;
							decisionTree.classAttribute = new ClassAttribute(attr);
						}
						decisionTree.attributes.add(attr);
						break;
					case 3:// Add attribute values to the vector list.
						String[] values = text.trim().split(" ");
						for( String attributeValue : values ) {
							Float value = Float.parseFloat(attributeValue);
							decisionTree.attributes.get(attributeCount).AddAttributeValues(value);
							attributeCount++;
						}
						break;
					default:
						break;
				}
		    }
		    
		    DecisionNode rootNode = decisionTree.getDecisionTree(trainingPercentage);
		    decisionTree.ExportDecisionTree(rootNode,trainingPercentage,0, inputFileName);
		    decisionTree.ExportDecisionTreeWithPredictedClassAndAccuracy(rootNode,trainingPercentage, inputFileName);
		} catch (FileNotFoundException e) {
		    e.printStackTrace();
		} catch (IOException e) {
		    e.printStackTrace();
		}
		catch(Exception ex)
		{
			System.out.println("");
		}finally {
		    try {
		        if (reader != null) {
		            reader.close();
		        }
		    } catch (IOException e) {
		    }
		}
	}
	
}
