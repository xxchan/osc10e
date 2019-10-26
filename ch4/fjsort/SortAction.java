import java.util.concurrent.*;

public abstract class SortAction<T extends Comparable<T>> extends RecursiveAction
{
    static final int THRESHOLD = 1;

    protected int begin;
    protected int end;
    protected T[] array;

    public SortAction(int begin, int end, T[] array) {
        this.begin = begin;
        this.end = end;
        this.array = array;
    }

    protected void compute() {
        if (end - begin < THRESHOLD) {
            naiveSort();
        }
        else {
	        sort();
        }
    }

    // naive insertion sort
    protected void naiveSort() {
    	for(int i = begin; i <= end; ++i) {
	        for(int j = i-1; j >= begin && (array[j].compareTo(array[j+1]) > 0); --j) {
                T tmp = array[j];
                array[j] = array[j+1];
		        array[j+1] = tmp;
	        }
	    }
    }

    protected abstract void sort();
}

