import java.util.*;
import java.lang.Math;
import java.lang.*; 
import java.io.*;

class AntColonyOptimization{
    public float alpha = 2;
    public float beta = 8;

    public float evaporation = 0.5f;
    public float antFactor = 0.5f;

    public float Max_Budget = 12000;

    public int Max_Iterations = (int)(Max_Budget/(3900+140+121.7)) + 5;

    public float Max_Coverage = 0;
    public int Current_Index = 0;
    
    public ArrayList<Integer> Nodes_Added;
    public float junction[][];
    public float covergae[][];

    public int CP_Number;
    public int Rseg_Number;

    public float probabilities[];
    public float trails[][];

    public Ant ants[];
    public float coverage[][];
    public int TotalAnts;
    private Random rand = new Random();
    

    //Constructor
    public AntColonyOptimization(float[][] junction, float[][] coverage)
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
        
        this.TotalAnts = (int)(this.antFactor * junction.length);
        this.ants = new Ant[this.TotalAnts];
    }


    //  Prepare ants

    public void setupAnts()
    {
        for(int i=0;i<this.TotalAnts;i++)
            this.ants[i] = new Ant(this.CP_Number, this.Rseg_Number);
        
        for(int i=0;i<this.TotalAnts;i++)
        {
            int random_index = rand.nextInt(junction.length);
            
            this.ants[i].Add_CP(random_index, this.junction, this.coverage);
        }

        this.Current_Index = 0;
        System.out.println("Setup ants done");
    }

    // Calculate probabilities

    public void calculateProbabilities(Ant ant)
    {
        int i = ant.Current_CP_list.get(this.Current_Index);

        this.probabilities = new float[this.junction.length];
        for(int t=0; t<this.junction.length;t++)
            this.probabilities[t] = (float)0.;
        
        for(int j=0; j < junction.length; j++)
        {
            if (ant.current_CP_array[j] == 0)
            {
                float factor1 = (float)(Math.pow(this.trails[i][j],  this.alpha));
                float factor2  =(float)(Math.pow(ant.Coverage_Addition(j, this.coverage)/this.junction[j][2]/190.0, this.beta));

                this.probabilities[j] += factor1*factor2;
            }
        }
        float sum = (float)0.;
        for(int t=0; t<this.probabilities.length; t++)
            sum += probabilities[t];
        
        for(int t=0; t<this.probabilities.length; t++)
            probabilities[t] /= sum;

    }


    // Select next candidate position to drop for each ant

    public int selectNextCandidate(Ant ant)
    {
        this.calculateProbabilities(ant);

        float r = (float)rand.nextDouble();
        
        float total = (float)0.;

        for(int i=0; i<this.junction.length; i++)
        {
            total += this.probabilities[i];

            if (total >= r)
                return i;
        }

        System.out.println("ERROR :: Nothing returned as all nodes are visited!");
        return -1;
    }

    // At each iteration, move ants

    public int moveAnts()
    {
        int stopped_ants = 0;

        for(int i=0; i<this.TotalAnts; i++)
        {
            Ant ant = this.ants[i];
            if (ant.freeze)
            {
                stopped_ants ++;
                continue;
            }

            int next_Cd = this.selectNextCandidate(ant);

            if(next_Cd == -1)
                ant.Freeze_Ant();
            
            else
            {
                if ( (ant.Current_Budget + this.junction[next_Cd][2]) > this.Max_Budget)
                {
                    System.out.println("ERROR :: Budget Exceeded!");
                    ant.Freeze_Ant();
                }

                else
                {
                    ant.Add_CP(next_Cd, this.junction, this.coverage);
                }

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

        for(int i=0; i < this.TotalAnts; i++)
        {
            Ant a = this.ants[i];
            
            if (a.freeze)
                continue;
            
            // Trail values should be less than but close to 1

            float contribution = a.Coverage_value() / this.Rseg_Number;

            for(int j = 0; j <a.Current_CP_list.size()-1; j++ )
                this.trails[a.Current_CP_list.get(j)][a.Current_CP_list.get(j + 1)] += contribution;
        }
    }

    // Update the best solution

    public void updateBest()
    {
        for(int i=0; i < this.TotalAnts; i++)
        {
            Ant a = this.ants[i];
            float ant_cover = a.Coverage_value();

            if(ant_cover > this.Max_Coverage)
            {
                this.Max_Coverage = ant_cover;
                this.Nodes_Added = new ArrayList<Integer>();
                for(int j=0; j<a.Current_CP_list.size(); j++)
                    this.Nodes_Added.add(a.Current_CP_list.get(j));
            }

        }
        System.out.println("Coverage - " + this.Max_Coverage);
        for(int i =0; i<Nodes_Added.size(); i++)
            System.out.print(this.Nodes_Added.get(i).intValue()+" ");
        System.out.println("\n");
    }

    // Use this method to run the main logic
    
    public void solve()
    {
        this.setupAnts();

        for(int i = 0; i<this.Max_Iterations; i++)
        {
            System.out.println("Iteration %d  ----------"+ i);

            int stopped_ant_count = this.moveAnts();

            System.out.println("Stopped ants after move ants step = "+ stopped_ant_count);

            this.updateTrails();
            this.updateBest();

            if (stopped_ant_count == this.TotalAnts)
            {
                System.out.println("Solve ended at iteration = " + i);
            }
        }

        System.out.println("---------- Iterations complete -----------");

        System.out.println("Best Coverage:  "+ this.Max_Coverage);
        for(int i = 0; i<this.Nodes_Added.size(); i++)
            System.out.print( this.Nodes_Added.get(i).intValue() + " ");
        System.out.println("\n");
    }
}