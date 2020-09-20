import java.util.*;
import java.lang.Math;
import java.lang.*; 
import java.io.*;

public class aco_generative
{
    public static float [][] FloatList_to_floatArray(ArrayList<ArrayList<Float>> FloatListList)
    {
        float [][] floatArr = new float[FloatListList.size()][FloatListList.get(0).size()];
        for(int j=0; j< FloatListList.size();j++)
        {
            List<Float> FloatList = new ArrayList<Float>();
            FloatList = FloatListList.get(j);
            float[] floatArray = new float[FloatList.size()];
            int i = 0;

            for (Float f : FloatList) 
            {
                floatArray[i++] = (f != null ? f.floatValue() : Float.NaN); 
            }
            
            floatArr[j] = floatArray;
        }

        return floatArr;
    }

    public static void main(String[] args)
    {
        try{
        try{
        // read CSV Density.csv
        
        ArrayList<ArrayList<Float>> Density = new ArrayList<ArrayList<Float>>();
        BufferedReader csvReader = new BufferedReader(new FileReader("Density.csv"));
        String row;
        while ((row = csvReader.readLine()) != null) 
        {
            String[] data = row.split(",");
            ArrayList<Float> temp = new ArrayList<>();
            for(int i=0; i<data.length;i++)
            {
                float f = Float.parseFloat(data[i]);
                Float fObj = new Float(f);    
                temp.add(fObj);
            }
            Density.add(temp);
        }
        csvReader.close();

        // read CSV Junction.csv

        int counter = 0;
        ArrayList<ArrayList<Float>> Junction = new ArrayList<ArrayList<Float>>();
        csvReader = new BufferedReader(new FileReader("Junction.csv"));
        while ((row = csvReader.readLine()) != null) 
        {
            if(counter == 0)
            {
                counter++;
                continue;
            }
            String[] data = row.split(",");
            ArrayList<Float> temp = new ArrayList<>();
            for(int i=0; i<data.length;i++)
            {
                float f = Float.parseFloat(data[i]);
                Float fObj = new Float(f);    
                temp.add(fObj);
            }
            Junction.add(temp);
        }
        csvReader.close();

        // read CSV Distance.csv

        counter = 0;
        ArrayList<ArrayList<Float>> Distance = new ArrayList<ArrayList<Float>>();
        csvReader = new BufferedReader(new FileReader("Distance.csv"));
        while ((row = csvReader.readLine()) != null) 
        {
            if(counter == 0)
            {
                counter++;
                continue;
            }
            String[] data = row.split(",");
            ArrayList<Float> temp = new ArrayList<>();
            for(int i=1; i<data.length;i++)
            {
                float f = Float.parseFloat(data[i]);
                Float fObj = new Float(f);    
                temp.add(fObj);
            }
            Distance.add(temp);
        }
        csvReader.close();

        float [][] density = FloatList_to_floatArray(Density);
        float [][] junction = FloatList_to_floatArray(Junction);
        float [][] distance = FloatList_to_floatArray(Distance);
        
        // Consider single RSU of Trange 243 metres

        float[][] coverage = new float[distance.length][distance[0].length];

        for(int i=0; i<coverage.length; i++){
            for(int j=0; j<coverage[0].length; j++){
                if(distance[i][j] < (float)243.)
                    coverage[i][j] = (float)1.;
                else
                    coverage[i][j] = (float)0.;
            }
        }


        System.out.println("Starting!");

        AntColonyOptimization ACO = new AntColonyOptimization(junction, coverage);
        ACO.solve();
        }
        catch (FileNotFoundException ex)  {
        // insert code to run when exception occurs
        }
        }
        catch(IOException e) {
        }
    }
}