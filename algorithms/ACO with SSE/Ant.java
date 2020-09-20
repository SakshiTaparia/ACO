import java.util.ArrayList; 
import java.util.Arrays; 
import java.lang.Math;
import java.util.Comparator;
import java.util.*;

public class Ant{
    public ArrayList<Integer> Current_CP_list;
    protected boolean current_CP_array[]; // make it boolean
    // protected boolean Current_Coverage[];
    // protected float Current_Coverage[];
    public ArrayList<PriorityQueue<Pair>> Current_Coverage;
    public float Current_Budget;
    public boolean freeze;
    public float Coverage_value;
    private Random rand = new Random();
    // new variable (Current coverage value)
    // make Current_coverage[][] as boolean array[] of road cells only

    
    public Ant(int number_of_CP, int number_of_RSeg)
    {
        this.Current_CP_list = new ArrayList<Integer>();
        this.Current_Budget = 0;
        this.current_CP_array = new boolean[number_of_CP];
        this.Coverage_value = 0f;
        this.Current_Coverage = new ArrayList<PriorityQueue<Pair>>();
        // this.Current_Coverage = new boolean[number_of_RSeg];
        // this.Current_Coverage = new float[number_of_RSeg];
        for(int i = 0;i<number_of_RSeg; i++)
        {
            PriorityQueue<Pair> pq = new PriorityQueue<Pair>(30);
            this.Current_Coverage.add(pq);
        }
    }

    // update current coverage value
    public void Add_CP(int index, float[][] junction, float[][] coverage)
    {
        this.Current_CP_list.add((Integer)index);
        this.current_CP_array[index] = true;
        float f = 0f;
        for(int i = 0;i<this.Current_Coverage.size();i++)
        {
            if(!this.Current_Coverage.get(i).isEmpty())
            {
                // f = Float.parseFloat(this.Current_Coverage.get(i).peek().split(",")[1]);
                f = this.Current_Coverage.get(i).peek().priority;
                this.Coverage_value += Math.max(0,coverage[i][index]-f);
                this.Current_Coverage.get(i).add(new Pair(index, coverage[i][index]));
            }
            else
            {
                this.Current_Coverage.get(i).add(new Pair(index, coverage[i][index]));
            }
            // this.Current_Coverage[i] =  Math.max(this.Current_Coverage[i],coverage[i][index]);
        }
        this.Current_Budget += junction[index][2] + 3900 + 121.7;  //cost of RSU and installment cost
    }

    public float effective_coverage(int n)
    {
        int index = this.Current_CP_list.get(n).intValue();
        float effect = 0f;
        float temp = 0f;
        for(int i = 0;i<this.Current_Coverage.size();i++)
        {
            if(!this.Current_Coverage.get(i).isEmpty())
            {
                int topi = this.Current_Coverage.get(i).peek().key;
                if(topi == index)
                {
                    temp = this.Current_Coverage.get(i).poll().priority;
                    effect += temp - this.Current_Coverage.get(i).peek().priority;
                    this.Current_Coverage.get(i).add(new Pair(index, temp));
                }
            }
        }
        return effect/this.Coverage_value;
    }

    public void remove(int n, float[][] junction)
    {
        int index = this.Current_CP_list.get(n).intValue();
        float temp = 0f;
        for(int i = 0;i<this.Current_Coverage.size();i++)
        {
            if(!this.Current_Coverage.get(i).isEmpty())
            {
                int topi = this.Current_Coverage.get(i).peek().key;
                if(topi == index)
                {
                    temp = this.Current_Coverage.get(i).poll().priority;
                    this.Coverage_value -= (temp - this.Current_Coverage.get(i).peek().priority);
                }
                else
                    this.Current_Coverage.get(i).remove(new Pair(index, 0f));
            }
        }
        this.Current_CP_list.remove(n);
        this.Current_Budget -= junction[index][2] + 3900 + 121.7;
        this.freeze = false;
    }
    //find n worst indexes and remove one of them randomly
    public void remove(float threshold, int n, float[][] junction)
    {
        if(!this.freeze)
            return;
            
        if(this.Current_CP_list.size()<5)
            return;
        // float f = (float)rand.nextDouble();
        // if(f<0.5 && !this.freeze)
        //     return;
        PriorityQueue<Pair> pq = new PriorityQueue<Pair>(30);
        for(int i = 0; i<this.Current_CP_list.size(); i++)
        {
            pq.add(new Pair(i, 1f-this.effective_coverage(i)));
        }
        int r = rand.nextInt(n);
        int index = 0;
        int temp = 0;
        float eff = 0f;
        if((1f-pq.peek().priority)<threshold)
        {
            while(r>0)
            {
                index = pq.poll().key;
                r--;
            }
            index = pq.peek().key;
            this.remove(index, junction);
        }
    }

    // // no need
    // public float Coverage_value()
    // {
    //     int count=0;
    //     int temp=0;
    //     for(int i=0;i<this.Current_Coverage.length;i++)
    //     {
    //         temp=0;
    //         for (int j=0;j<this.Current_Coverage[i].length;j++)
    //             temp+=Current_Coverage[i][j];
    //         if(temp>0)
    //             count++;
    //     }
    //     return count;
    // }
    
    // make co
    public float Coverage_Addition(int index, float[][] coverage)
    {
        // float temp1[]=new float[coverage.length];
        float Old_Rseg_Covered = this.Coverage_value;
        float New_Rseg_Covered = 0+this.Coverage_value;
        for(int i = 0;i<this.Current_Coverage.size(); i++)
        {
            New_Rseg_Covered += Math.max(0,coverage[i][index]-this.Current_Coverage.get(i).peek().priority);
        }
        // for(int i=0;i<temp2.length;i++)
        //     if(temp2[i]>0)
        //         New_Rseg_Covered ++;

        // int normalize = 0;
        // for(int i=0;i<this.Current_Coverage.length;i++)
        //     if(coverage[i][index])
        //         normalize ++;

        // System.out.println("Coverage addition : "+ (New_Rseg_Covered - Old_Rseg_Covered)/100);
        
        return (float)(New_Rseg_Covered - Old_Rseg_Covered)/100;
    }

    public void Freeze_Ant()
    {
        this.freeze = true;
    }
}
