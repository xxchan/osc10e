import java.util.concurrent.*;

public class QuickSortAction<T extends Comparable<T>> extends SortAction<T> 
{
    public QuickSortAction(int begin, int end, T[] array) {
        super(begin, end, array);
    }
    
    protected void sort() {
        T pivot = array[begin];
        int i,j;
        for(i=begin, j=end; i<j;)
        {
            while(i<j && array[j].compareTo(pivot) >= 0)
                --j;
            if(i<j)
                array[i] = array[j];
            while(i<j && array[i].compareTo(pivot) < 0)
                ++i;
            if(i<j)
                array[j] = array[i];
        }
        array[i] = pivot;
        
        QuickSortAction<T> leftAction = new QuickSortAction<T>(begin, i-1, array);
        QuickSortAction<T> rightAction = new QuickSortAction<T>(i+1, end, array);

        leftAction.fork();
        rightAction.fork();

        leftAction.join();
        rightAction.join();
    }
}
