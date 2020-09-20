import java.util.*;
import java.lang.Math;
import java.lang.*; 
import java.io.*;


class MoveAnts implements Runnable
{
    public ArrayList<Ant> ants;
    public ACO_parallel aco;
    public MoveAnts(ArrayList<Ant> antList, ACO_parallel acot)
    {
        this.ants = antList;
        this.aco = acot;
    }

    public void run()
    {
        try
        {   
            for(int i=0; i<ants.size(); i++)
            {
                Ant ant = this.ants.get(i);
                if (ant.freeze)
                {
                    continue;
                }

                int next_Cd = this.aco.selectNextCandidate(ant);

                if(next_Cd == -1)
                    ant.Freeze_Ant();
                
                else
                {
                    if ( (ant.Current_Budget + this.aco.junction[next_Cd][2]) > this.aco.Max_Budget)
                    {
                        // System.out.println("ERROR :: Budget Exceeded!");
                        ant.Freeze_Ant();
                    }

                    else
                    {
                        ant.Add_CP(next_Cd, this.aco.junction, this.aco.coverage);
                    }

                }
            }
        }
        catch (Exception e) 
        { 
            // Throwing an exception 
            System.out.println ("Exception is caught while moving ants."); 
        }
    }
}
class ACO_parallel{
    public float alpha = 1f;
    public float beta = 5f;

    public float evaporation = 0.5f;

    public float setupAntFactor = 0.7f;
    public float addAntFactor = 0.07f;
    public int stepsBeforeAdd = 3;

    // parameters for removal of nodes from ants trail
    public int remove_After = 10;
    public float remove_threshold = 0.05f;
    public int topn = 2;

    public int noOfNewAnts;
    public float currentBudget;
    public float antFactor = 2f;

    public float Max_Budget = 100000;

    // public int Max_Iterations = (int)(Max_Budget/(3900+140+121.7)) + 30;
    public int Max_Iterations = 150;

    // trails vary between 0 and 1
    public float max_trail = 0.7f;
    public float min_trail = 0.3f;
    public float Max_Coverage = 0;
    public int Current_Index = 0;
    
    public ArrayList<Integer> Nodes_Added;
    public float junction[][];

    public int CP_Number;
    public int Rseg_Number;

    public float probabilities[];
    public float trails[][];

    // public Ant ants[];
    public ArrayList<Ant> ants;
    // public boolean coverage[][];
    public float coverage[][];
    public int TotalAnts;
    public int initialAnts;

    private Random rand = new Random();

    public int threadCount = 12;
    

    //Constructor
    public ACO_parallel(float[][] junction, float[][] coverage)
    {
        this.junction = junction;
        this.coverage = coverage;

        this.CP_Number = junction.length;
        this.Rseg_Number = coverage.length;

        this.probabilities = new float[junction.length];
        for(int i=0;i<junction.length;i++)
            probabilities[i] = (float)0.;
        
        this.trails = new float[junction.length][junction.length];
        for(int i=0;i<junction.length;i++)
            for(int j=0;j<junction.length;j++)
                this.trails[i][j] = 1.f;
        
        this.initialAnts = (int)(this.setupAntFactor * junction.length);
        // this.ants = new Ant[this.TotalAnts];
        this.noOfNewAnts = (int)(this.addAntFactor * junction.length);
        this.ants = new ArrayList<Ant>();
        // System.out.println("ant-factor: "+ this.antFactor);
        System.out.println("alpha: "+this.alpha+"    beta: "+this.beta + "    Evaporation: "+ this.evaporation);
        System.out.println("Max iterations: " + this.Max_Iterations );
        System.out.println("roadCells : " + this.coverage.length);
        System.out.println("CPs : "+ this.coverage[0].length);
    }

    // add ants
    public void addAnts(int n)
    {
        
        for(int i=0;i<n;i++)
        {
            Ant a = new Ant(this.CP_Number, this.Rseg_Number);
            int random_index = rand.nextInt(junction.length);
            
            a.Add_CP(random_index, this.junction, this.coverage);
            this.ants.add(a);
        }
        
        this.Current_Index = 0;
        System.out.println("New " + n + " ants added.");
    }


    //  Prepare ants

    // public void setupAnts()
    // {   
    //     for(int i=0;i<this.TotalAnts;i++)
    //         this.ants.get(i) = new Ant(this.CP_Number, this.Rseg_Number);
        
    //     for(int i=0;i<this.TotalAnts;i++)
    //     {
    //         int random_index = rand.nextInt(junction.length);
            
    //         this.ants.get(i).Add_CP(random_index, this.junction, this.coverage);
    //     }

    //     this.Current_Index = 0;
    //     System.out.println("Setup ants done");
    // }

    // Calculate probabilities

    public float[] calculateProbabilities(Ant ant)
    {
        int i = ant.Current_CP_list.get(ant.Current_CP_list.size()-1);

        this.probabilities = new float[this.junction.length];
        for(int t=0; t<this.junction.length;t++)
            this.probabilities[t] = (float)0.;
        
        for(int j=0; j < junction.length; j++)
        {
            if (!ant.current_CP_array[j])
            {
                float factor1 = (float)(Math.pow(this.trails[i][j],  this.alpha));
                // float factor2  =(float)(Math.pow(ant.Coverage_Addition(j, this.coverage)*190.0/this.junction[j][2], this.beta));
                float factor2  =(float)(Math.pow(ant.Coverage_Addition(j, this.coverage), this.beta));
                // System.out.println("factor 1 : "+factor1);
                // System.out.println("factor 2 : "+factor2);
                this.probabilities[j] += factor1*factor2;
            }
        }
        float sum = (float)0.;
        for(int t=0; t<this.probabilities.length; t++)
        {
            sum += this.probabilities[t];
            // System.out.println("sum : "+sum);
        }
        if(sum == 0f)
            return this.probabilities;
        for(int t=0; t<this.probabilities.length; t++)
            this.probabilities[t] = this.probabilities[t]/sum;

        // sum = (float)0.;
        // for(int t=0; t<this.probabilities.length; t++)
        //     sum += this.probabilities[t];

        // System.out.println("sum : " +sum);
        return this.probabilities;
    }


    // Select next candidate position to drop for each ant
    public int[] topK(float[] arr, int k)
    {
        int temp=0;
        int[] result = new int[k];
        boolean flag=false;
        for(int t=0;t<k;t++)
        {   
            temp=0;
            for(int j=0;j<t;j++)
                if(result[j]==temp)
                {
                    j=0;
                    temp++;
                }
            for(int i = 0; i<arr.length; i++)
            {
                if(arr[i]>arr[temp])
                {
                    for(int j=0;j<t;j++)
                        if(result[j]==i)
                        {
                            flag = true;
                            break;
                        }
                    if(flag)
                    {
                        flag = false;
                        continue;
                    }
                    temp = i;
                }
            }
            result[t] = temp;
        }
        return result;
    }
    public int selectNextCandidate(Ant ant)
    {
        this.probabilities = this.calculateProbabilities(ant);

        float r = (float)rand.nextDouble();
        float total = (float)0.;
        
        for(int i=0; i<this.probabilities.length; i++)
        {
                total += this.probabilities[i];

                if (total >= r)
                    return i;
        }
        System.out.println("total : "+total);
        System.out.println("random number :"+r);
            
        // int[] topK = topK(this.probabilities, 3);
        // int random_index = rand.nextInt(3);
        // return topK[random_index];
        

        System.out.println("ERROR :: Nothing returned as all nodes are visited!");
        return -1;
    }

    // At each iteration, move ants

    public int moveAnts()
    {
        ArrayList<Thread> threadList = new ArrayList<Thread>();
        int antsPerThread = this.ants.size()/this.threadCount;
        int counter = 0;
        int t;
        for(int i=0; i<this.threadCount; i++)
        {
            t=0;
            ArrayList<Ant> antlist = new ArrayList<Ant>();
            for(int j=counter; j+t<this.ants.size(); j++)
            {
                if(j == counter + antsPerThread)
                {
                    counter += antsPerThread;
                    break;
                }
                if(this.ants.get(j+t).freeze)
                {
                    t++;
                    j--;
                    continue;
                }
                else
                {
                    antlist.add(this.ants.get(j+t));
                }
            }
            counter = counter+t;
            Thread object = new Thread(new MoveAnts(antlist, this)); 
            object.start();
            threadList.add(object);
        }


        try
        {
            for(int i=0; i<this.threadCount;i++)
                threadList.get(i).join();
        }
        catch(Exception ex) 
        { 
            System.out.println("Exception has been caught" + ex); 
        } 
        
        int stopped_ants = 0;

        for(int i=0; i<this.ants.size(); i++)
        {
            Ant ant = this.ants.get(i);
            if (ant.freeze)
            {
                stopped_ants ++;
            }
        }

        this.Current_Index += 1;

        return stopped_ants;
    }

    // Update trails that ants used

    public void updateTrails()
    {
        for(int i = 0;i< this.trails.length; i++)
            for(int j = 0; j<this.trails[0].length; j++)
                this.trails[i][j] *= this.evaporation;
        float maxCoverage = 0;
        float minCoverage = 10000000;
        for(int i=0; i < this.ants.size(); i++)
        {
            Ant a = this.ants.get(i);
            // System.out.println("Index " + i); 
            if(a.Coverage_value>maxCoverage)
                maxCoverage = a.Coverage_value;
            if(a.Coverage_value < minCoverage)
                minCoverage = a.Coverage_value;
        }

        for(int i=0; i < this.ants.size(); i++)
        {
            Ant a = this.ants.get(i);
            
            if (a.freeze)
                continue;
            
            // Trail values should be less than but close to 1

            // float contribution = a.Coverage_value / this.Rseg_Number;
            float contribution = (a.Coverage_value - minCoverage)/(maxCoverage - minCoverage);

            for(int j = 0; j <a.Current_CP_list.size()-1; j++ )
                this.trails[a.Current_CP_list.get(j)][a.Current_CP_list.get(j + 1)] += contribution * 0.5;
        }
        float maxTrail = 0f;
        for(int i = 0;i< this.trails.length; i++)
            for(int j = 0; j<this.trails[0].length; j++)
            {    
                this.trails[i][j] = Math.min(this.max_trail, this.trails[i][j]);
                this.trails[i][j] = Math.max(this.trails[i][j], this.min_trail);
                if(this.trails[i][j] > maxTrail)
                    maxTrail = this.trails[i][j] ;
            }
            // if(maxTrail<this.trails[i][j])
        for(int i = 0;i< this.trails.length; i++)
        for(int j = 0; j<this.trails[0].length; j++)
            this.trails[i][j] /= maxTrail ;
        
    }

    // Update the best solution

    public void updateBest()
    {
        for(int i=0; i < this.ants.size(); i++)
        {
            Ant a = this.ants.get(i);
            float ant_cover = a.Coverage_value;

            if(ant_cover > this.Max_Coverage)
            {
                this.currentBudget = a.Current_Budget;
                this.Max_Coverage = ant_cover;
                this.Nodes_Added = new ArrayList<Integer>();
                for(int j=0; j<a.Current_CP_list.size(); j++)
                    this.Nodes_Added.add(a.Current_CP_list.get(j));
            }

        }
        System.out.println("Coverage - " + this.Max_Coverage/this.coverage.length + "  Budget :" + this.currentBudget);
        // System.out.println("Coverage - " + this.Max_Coverage + "  Budget :" + this.currentBudget);


        for(int i =0; i<Nodes_Added.size(); i++)
            System.out.print(this.Nodes_Added.get(i).intValue()+" ");
        System.out.println("\n");
        try{    
            try{
            FileWriter fw=new FileWriter("results/RSUs and Coverage(Mumbai)(antFactor:" +this.antFactor + " alpha:"+this.alpha+" beta:" +this.beta  +" Budget:"+ this.Max_Budget+").txt",true);
            fw.write("Max Budget : " + this.currentBudget + "\n"); 
            fw.write("roadCells : " + this.coverage.length + "\n"); 
            fw.write("Candidate Positions : "+ this.coverage[0].length + "\n"); 
            fw.write("Coverage - " + this.Max_Coverage + "\n"); 
            float percent_coverage = (float)this.Max_Coverage/(float)this.coverage.length*100.f;
            fw.write("Coverage Percent - " + percent_coverage + "\n"); 
            fw.write("ant-factor: "+ this.antFactor + " alpha: "+this.alpha+"    beta: "+this.beta + "\n"); 

            for(int i =0; i<Nodes_Added.size(); i++)
            {
                System.out.print(this.Nodes_Added.get(i).intValue()+" ");
                fw.write(this.Nodes_Added.get(i).intValue() + "\n");    
            }
            System.out.println("\n");    
            fw.close(); 
            }
            catch(Exception e){
                FileWriter fw=new FileWriter("RSUs and Coverage(Mumbai)(antFactor:" +this.antFactor + " alpha:"+this.alpha+" beta:" +this.beta  +" Budget:"+ this.Max_Budget+").txt",true);
            
            fw.write("Max Budget : " + this.currentBudget + "\n"); 
            fw.write("roadCells : " + this.coverage.length + "\n"); 
            fw.write("Candidate Positions : "+ this.coverage[0].length + "\n"); 
            fw.write("Coverage - " + this.Max_Coverage + "\n"); 
            float percent_coverage = (float)this.Max_Coverage/(float)this.coverage.length*100.f;
            fw.write("Coverage Percent - " + percent_coverage + "\n"); 
            fw.write("ant-factor: "+ this.antFactor + " alpha: "+this.alpha+"    beta: "+this.beta + "\n"); 

            for(int i =0; i<Nodes_Added.size(); i++)
            {
                System.out.print(this.Nodes_Added.get(i).intValue()+" ");
                fw.write(this.Nodes_Added.get(i).intValue() + "\n");    
            }
            System.out.println("\n");    
            fw.close();    
        }
        }
        catch(Exception e){System.out.println(e);}  
    }

    // Use this method to run the main logic
    public void remove_Nodes()
    {
        for(int i=0; i<this.ants.size(); i++)
            this.ants.get(i).remove(this.remove_threshold, this.topn, this.coverage);
            
        System.out.println("1 node removed from trails !!!");
    }
    public void solve()
    {
        this.addAnts(this.initialAnts);

        for(int i = 0; i<this.Max_Iterations; i++)
        {
            System.out.println("Iteration "+ i+"  ----------------");
            if(i % this.stepsBeforeAdd == 0)
                this.addAnts(this.noOfNewAnts);
                
            System.out.println("No of Ants = "+ this.ants.size());

            if(i>10 && i%this.remove_After == 0)
                this.remove_Nodes();

            int stopped_ant_count = this.moveAnts();

            System.out.println("Stopped ants after move ants step = "+ stopped_ant_count);

            this.updateTrails();
            this.updateBest();

            // if (stopped_ant_count == this.ants.size())
            // {
            //     System.out.println("Solve ended at iteration = " + i);
            // }
        }

        System.out.println("---------- Iterations complete -----------");

        System.out.println("Best Coverage:  "+ this.Max_Coverage);
        for(int i = 0; i<this.Nodes_Added.size(); i++)
            System.out.print( this.Nodes_Added.get(i).intValue() + " ");
        System.out.println("\n");

    }
}
