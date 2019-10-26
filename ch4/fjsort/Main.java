import java.util.concurrent.*;

public class Main {
    static final int SIZE = 20;

    private static Integer[] array;

    public static void main(String[] args) {
        ForkJoinPool pool = new ForkJoinPool();
        Integer[] array = new Integer[SIZE];
        
        MergeSortAction<Integer> mergeSortAction = new MergeSortAction<Integer>(0, SIZE-1, array);
        QuickSortAction<Integer> quickSortAction = new QuickSortAction<Integer>(0, SIZE-1, array);
        
        java.util.Random ran = new java.util.Random();
        for(int i = 0; i < SIZE; ++i)
            array[i] = ran.nextInt(SIZE * 10);

        System.out.println("Before merge sort:");
        for(int i = 0; i < SIZE; ++i)
            System.out.print(array[i] + " ");
        pool.invoke(mergeSortAction);
        System.out.println("\nAfter merge sort:");
        for(int i = 0; i < SIZE; ++i)
            System.out.print(array[i] + " ");
        System.out.println();

        
        for(int i = 0; i < SIZE; ++i)
            array[i] = ran.nextInt(SIZE * 10);
        System.out.println("Before quick sort:");
        for(int i = 0; i < SIZE; ++i)
            System.out.print(array[i] + " ");
        pool.invoke(quickSortAction);
        System.out.println("\nAfter quick sort:");
        for(int i = 0; i < SIZE; ++i)
            System.out.print(array[i] + " ");
        System.out.println();
    }
}
