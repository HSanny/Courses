
public class ISbsearch implements IntSet {
	// binary search over ordered array
	// order by insertion sort
	
	private int[] myArraySet;
	private int count = 0;	// number of elements in set
	
	// invariant: 0 <= count <= myset.length
	
	private int min;
	private int max;

	public ISbsearch (int min, int max) {
		if (max<min) max=min;  // error protection, minimum size 1
		myArraySet = new int[max-min+1];	
		this.min=min;
		this.max=max;
		this.makeEmpty();
	}
	
	//@Override
	public int cardinality() {
		return count;
	}

	//@Override
	public void insert(int n) throws IndexOutOfBoundsException {
		// insert into set, use insertion sort to maintain in increasing order
		if (isMember(n)) return;
		// adapted from Koffman and Wolfgang p.434
		int nextPos;
		for(nextPos=count;
			nextPos > 0 && n < myArraySet[nextPos-1];
			nextPos--)
		{
				myArraySet[nextPos] = myArraySet[nextPos-1];
		}
		myArraySet[nextPos] = n;
		count++;
	}

	//@Override
	public boolean isMember(int c) {
		return binarySearch(myArraySet, c, 0, count-1);
	}

	private static boolean binarySearch(int[] items, int target, int first, int last) {
	// binary search - specialised slightly from Koffman and Wolfgang p.265
		if (first > last) 
			return false;	// base case for unsuccessful search
		else {
			int middle = (first + last) / 2; // next probe index
			if (target == items[middle]) {
				return true;
			}
			if (target < items[middle]) {
				return binarySearch(items,target, first, middle-1);
			}
			else {
				return binarySearch(items,target, middle+1, last);
			}	
		}
	}

	//@Override
	public void makeEmpty() {
		count = 0;
	}

}
