import java.util.Objects;
public class Pair implements Comparable<Pair>  {
    int key;
    float priority;
    
    public Pair (int k, float p) {
      key = k;
      priority = p;
    }
    
    @Override
    public int compareTo(Pair p) {
        float f1 = this.priority;
        float f2 = p.priority;
        if(f1>f2)
            return -1;
        else if(f2>f1)
            return 1;
        else
            return 0;
        // return second.priority - first.priority;
    }


    @Override
    public String toString() {
      return key + "," + priority ;
    }
    
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Pair p = (Pair) o;
        if(p.key == this.key)
            return true;
        else
            return false;
    }
  }
