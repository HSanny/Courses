
public class ISlinear implements IntSet {
		// represent a a set of integers by a linear array of integers
		// unordered, filled from index 0 to index count-1
	private int[] myArraySet;
	private int count = 0; // index of next empty space in myset 
	// invariant: 0<= count <= myset.length
	
	private int min;
	private int max;

	public ISlinear (int min, int max) {
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
		if (n < min | min > max) {
			throw new IndexOutOfBoundsException("inserting a number outside the raneg of the set");
		}
		if (count >= myArraySet.length) {
			return;	// protective coding: set is full, no action
		}
		else if (isMember(n)) {
			return;
		}
		else {
			myArraySet[count]=n;
			count++;
		}
	}


	//@Override
	public boolean isMember(int c) {
		for (int sx=0; sx < count; sx++) {
			if(myArraySet[sx]==c) return true;
		}
		return false;
	}

	//@Override
	public void makeEmpty() {
		count=0;
	}

}
