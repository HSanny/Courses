
import java.util.Random;
import java.lang.management.*;
/*
import ISlinear;
import ISbsearch;
import IShash;
import ISbtree;
*/
public class IStest {

	/**
	 * @param args
	 * test selection of IntSet implementations
	 */
	public static void main	(String[] args) {
		 int trials=500000;
		 if (args.length == 1 ) {
			 try {
				trials = Integer.parseInt(args[0]);
			} catch (NumberFormatException e) {
				System.err.println("expected an integer arg: number of trials to make, you provided " + args[0]);
				System.exit(1);}
		 } else
			 trials = 500000; // default 500k
		
		 System.out.println("WARNING: performance testing using repeated iterations. Program run may take several minutes");
		ISbsearch isb = new ISbsearch(1,trials);
		System.out.println("test binary search/insertion sort");
		long isbtime = testAset(trials, isb);	
		System.out.println("total time " + isbtime / 1000000 + " millisec");
		System.out.println();

		ISlinear isl = new ISlinear(1,trials);	// note - 88.8 seconds on emperor Java 6 1/11/2012, 57 sec 12/3/13
												//       205.4 sec on cwj laptop 2/11/12
		System.out.println("test linearsearch unordered insert ");
		long isltime = testAset(trials, isl);	
		System.out.println("total time " + isltime / 1000000 + " millisec");
		System.out.println();
		
		
		System.out.println("test hashSet - small initial size");
		IShash ishSmall = new IShash();
		long ishStime = testAset(trials, ishSmall);	
		System.out.println("total time " + ishStime / 1000000 + " millisec");
		System.out.println();
		
		System.out.println("test hashSet - large initial size");
		IShash ishLarge = new IShash(trials);
		long ishLtime = testAset(trials, ishLarge);	
		System.out.println("total time " + ishLtime / 1000000 + " millisec");
        System.out.println();

        System.out.println("test binary sort tree");
		ISbtree isbt = new ISbtree(trials);
		long isbttime = testAset(trials, isbt);	
		System.out.println("total time " + isbttime / 1000000 + " millisec");
        System.out.println();

	}

	private static long testAset(int trials, IntSet theset) {
		// insert the number of trials of random numbers into theset
		// return the CPU time taken (nanoseconds)
		Random rgen = new Random();
		long startTime = getCpuTime();
		long endTime;
		for (int rcount= 1; rcount <= trials; rcount++) {
			int r=rgen.nextInt(trials)+1;
			try {
				theset.insert(r);
			} catch (IndexOutOfBoundsException e) {
				System.err.println("attempt to insert number " + r + " outside range " + 1 + ".." + trials);
			}
		}
		long midTime = getCpuTime();
		System.out.println("create and insert time= " + (midTime-startTime)/1000000 + " millisec");
		// test whether all ints in range 1..trials are in set
		int card = 0;
		for (int rc= 1; rc <= trials; rc++) {
			if (theset.isMember(rc)) {
				card++;
			}
		}
		endTime = getCpuTime();
		System.out.println("search time= " + (endTime-midTime)/1000000 + " millisec");
		if (card != theset.cardinality()) {
			System.out.println("implementation error: number of unique elements inserted does not match reported cardinality: " + card + " vs " + theset.cardinality());
		}
		return endTime-startTime;
	}
	
	 /** Get CPU time in nanoseconds. */
	/* from Nadeau Software Consulting http://nadeausoftware.com/
	 * Java tip: How to get CPU, system, and user time for benchmarking
	 * http://nadeausoftware.com/articles/2008/03/java_tip_how_get_cpu_and_user_time_benchmarking
	 */
	 private static long getCpuTime( ) {
	     ThreadMXBean bean = ManagementFactory.getThreadMXBean( );
	     return bean.isCurrentThreadCpuTimeSupported( ) ?
	         bean.getCurrentThreadCpuTime( ) : 0L;
	 }
}
