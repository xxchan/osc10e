import java.util.concurrent.*;
import java.util.Arrays;

public class MergeSortAction<T extends Comparable<T>> extends SortAction<T> 
{
    public MergeSortAction(int begin, int end, T[] array) {
        super(begin, end, array);
    }
    
    protected void sort() {
        int mid = begin + (end-begin)/2;
        MergeSortAction<T> leftAction = new MergeSortAction<T>(begin, mid, array);
        MergeSortAction<T> rightAction = new MergeSortAction<T>(mid+1, end, array);

        leftAction.fork();
        rightAction.fork();

        leftAction.join();
        rightAction.join();

        merge();
    }

    private void merge() {
        int mid = begin + (end-begin)/2;
        int i,j,k;
        T[] tmp = Arrays.copyOfRange(array, begin, end+1); 
        for(i=begin, j=mid+1, k=0; i<=mid && j<=end;) {
            if(array[i].compareTo(array[j]) < 0)
                tmp[k++] = array[i++];
            else
                tmp[k++] = array[j++];
        }
        while(i<=mid)
            tmp[k++] = array[i++];
        while(j<=end)
            tmp[k++] = array[j++];

        for(i=begin, k=0; i<=end; )
            array[i++] = tmp[k++];
    }
}
