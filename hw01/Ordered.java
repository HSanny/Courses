/**
 * Created on 21 Feb, 2013
 * Edited by MacVim
 *
 * @author Jimmy Lin - u5223173@uds.anu.edu.au
 * @version 1.2 
 */
public class Ordered{
	/**
	 * determine whether an array is in non-decreasing order upwards or downwards
 	 * @param values an array of ints of any length
	 * @return true if the elements of values are in non-decreasing order
	 * that is, each element is equal to or greater than its predecessor-
	 * as the array index increases, if up is true; or as it decreases, if up is false
	 * otherwise return false
	 */
	public static boolean isOrdered (int values[], boolean up ){
		boolean result = true; // one array is default to be ordered if 0-length array is input
		// acquire the string representation of input array
		String value_str = "[";
		for(int i = 0 ; i < values.length ; i++) 
			if (i < values.length - 1) value_str += values[i] + ",";
			else if (i == values.length - 1) value_str += values[i] + "]";
		if (values.length == 0) value_str += "]";

		// statement for start required computation - comparison between elements and traverse the array
		System.out.println("\nApplying isOrdered method for array "+ value_str +"..");
		if (up)
			System.out.println("Given Parameter up is "+ up + " -- non-decreasing order as index increases.." );
		else
			System.out.println("Given Parameter up is "+ up  + " -- non-decreasing order as index decreases..");
		if (values.length <= 1) {
			System.out.println("Arrays of length 0 or 1 are defined to be ordered..");
			System.out.println("Return value for calling isOrdered: "+ result);
			return result; // no need to examine the array of length 0 and 1
		}
		for(int i = 0 ; i < values.length - 1 ; i ++){
			String str = "Comparing the "+i+"th element and the "+(i+1)+"th element.." ;
			if (up && values[i] <= values[i+1]) {
				// currently ordered, remain to be true
				System.out.println(str + " result: ordered..");
				continue;
			}
			else if (!up && values[i] >= values[i+1]) {
				// currently ordered, remain to be true
				System.out.println(str + " result: ordered..");
				continue; 	
			}
			else {
				// condition not satisfied, break and return false
				System.out.println(str + " result: un-ordered..");
				result = false;
				break;
			}
		}
		System.out.println("Return value for calling isOrdered: "+ result);
		return result;
	}

	/**
	 * test the isOrdered method
	 * @param args is ignored
	 * @return exit  value shall is 1 if any test fails, otherwise exit value is 0
	 */
	public static void main(String args[]){
		int[] test_example_0 = {}; // empty array
		int[] test_example_1 = {0};  // single element with element value 0 
		int[] test_example_2 = {-2,-1,0,1,2,3};  // array that in order upwards (negative and positive, including zero)
		int[] test_example_3 = {2,3,2,0,0,0};  // out of order at the start 
		int[] test_example_4 = {0,0,0,0,5,4};  // out of order at the end
		int[] test_example_5 = {0,0,0,1,0,0,0}; // out of order in the middle

		boolean test_0t = Ordered.isOrdered(test_example_0,true);   // expectation: true
		boolean test_0f = Ordered.isOrdered(test_example_0,false);  //  expectation:  true
		boolean test_1t = Ordered.isOrdered(test_example_1,true);   // expectation: true
		boolean test_1f = Ordered.isOrdered(test_example_1,false);  // expectation: true
		boolean test_2t = Ordered.isOrdered(test_example_2,true);   // expectation: true
		boolean test_2f = Ordered.isOrdered(test_example_2,false);  // expectation: false
		boolean test_3t = Ordered.isOrdered(test_example_3,true);   // expectation: false
		boolean test_3f = Ordered.isOrdered(test_example_3,false);  // expectation: false
		boolean test_4t = Ordered.isOrdered(test_example_4,true);   // expectation: false
		boolean test_4f = Ordered.isOrdered(test_example_4,false);  // expectation: false
		boolean test_5t = Ordered.isOrdered(test_example_5,true);  // expectation: false
		boolean test_5f = Ordered.isOrdered(test_example_5,false);  // expectation: false
		if (test_0t && test_0f && test_1t && test_1f && test_2t && !test_2f && !test_3t && !test_3f && !test_4t && !test_4f && !test_5t && ! test_5f) {
			System.out.println("Test: true");
			System.exit(0);   // all tests succeed
		}else{ 
			System.out.println("Test: false");
			System.exit(1);   // some tests failed
		}
	}
}
