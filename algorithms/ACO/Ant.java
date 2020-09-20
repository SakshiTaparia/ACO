import java.util.ArrayList; 
import java.util.Arrays; 


public class Ant{
    public ArrayList<Integer> Current_CP_list;
    protected boolean current_CP_array[]; // make it boolean
    protected boolean Current_Coverage[];
    public float Current_Budget;
    public boolean freeze;
    public int Coverage_value;
    // new variable (Current coverage value)
    // make Current_coverage[][] as boolean array[] of road cells only

    
    public Ant(int number_of_CP, int number_of_RSeg)
    {
        this.Current_CP_list = new ArrayList<Integer>();
        this.Current_Budget = 0;
        this.current_CP_array = new boolean[number_of_CP];
        this.Current_Coverage = new boolean[number_of_RSeg];
        for(int i = 0;i<this.Current_Coverage.length;i++)
            this.Current_Coverage[i] =  false;
    }

    // update current coverage value
    public void Add_CP(int index, float[][] junction, boolean[][] coverage)
    {
        this.Current_CP_list.add((Integer)index);
        this.current_CP_array[index] = true;

        for(int i = 0;i<this.Current_Coverage.length;i++)
        {
            if(!this.Current_Coverage[i] && coverage[i][index])
                this.Coverage_value++;
            this.Current_Coverage[i] =  this.Current_Coverage[i]||coverage[i][index];
        }
        this.Current_Budget += junction[index][2] + 3900 + 121.7;  //cost of RSU and installment cost
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
    public float Coverage_Addition(int index, boolean[][] coverage)
    {
        // float temp1[]=new float[coverage.length];
        int Old_Rseg_Covered = this.Coverage_value;
        // Arrays.fill(temp1, 0.);
        // for(int i=0;i<coverage.length;i++)
        //     temp1[i]=(float)0.;
        
        // for(int i=0;i<this.current_CP_array.length;i++)
        // {
        //     if(this.current_CP_array[i])
        //         for(int j=0;j<temp1.length;j++)
        //             temp1[j]+=this.Current_Coverage[j][i];
        // }

        // float temp2;;
        // for(int i=0;i<temp1.length;i++)
        //     temp2[i]=temp1[i]+coverage[i][index];
        
        // int Old_Rseg_Covered=0;
        // for(int i=0;i<temp1.length;i++)
        //     if(temp1[i]>0)
        //         Old_Rseg_Covered ++;

        int New_Rseg_Covered = 0+this.Coverage_value;
        for(int i = 0;i<this.Current_Coverage.length;i++)
        {
            if(!this.Current_Coverage[i] && coverage[i][index])
                New_Rseg_Covered++;
        }
        // for(int i=0;i<temp2.length;i++)
        //     if(temp2[i]>0)
        //         New_Rseg_Covered ++;

        // int normalize = 0;
        // for(int i=0;i<this.Current_Coverage.length;i++)
        //     if(coverage[i][index])
        //         normalize ++;
        
        return (float)(New_Rseg_Covered - Old_Rseg_Covered)/200;
    }

    public void Freeze_Ant()
    {
        this.freeze = true;
    }
}