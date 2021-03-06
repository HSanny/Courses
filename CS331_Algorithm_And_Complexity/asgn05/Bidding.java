/*##############################################################
## MODULE: Bidding.java
## VERSION: 1.0 
## SINCE: 2014-01-14
## AUTHOR: 
##         JIMMY LIN (xl5224) - JimmyLin@utexas.edu  
##
## DESCRIPTION: 
##    Algorithm Assignment #4: BIDDING SYSTEM by Algorithm A
##    We incrementally implement the bidding-item stable matching.
## 
#################################################################
## Edited by MacVim
## Class Info auto-generated by Snippet 
################################################################*/

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.PriorityQueue;
import java.util.Comparator;
import java.util.Collections;
import java.util.HashSet;

// ****************************************************************
// Item data structure
class Item implements Comparable<Item> {
    // fields characterizing item
    int id;  
    int quality;  
    int reservePrice;  // unit: cents

    int startPrice;
    int toBid;

    static int id_count;  // as id assigner

    // constructor for item class
    Item (int quality, int reservePrice, int startPrice) {
        this.id = id_count ++;
        this.quality = quality;
        this.reservePrice = reservePrice;  
        this.startPrice = startPrice;
    }
    public String toString () {
        String str = "Item: " + this.quality + "," + this.reservePrice;
        return str;
    }
    public int compareTo (Item item) {
        return item.quality - this.quality;  // descending order
    }
}

// ****************************************************************
// Bid data structure
class Bid {
    int type;
    int id;

    static int id_count;

    Bid (int type) {
        this.type = type;
        this.id = Bid.id_count++;
    }
}

class SingleItemBid extends Bid implements Comparable<SingleItemBid> {
    Item toItem;
    int offer;

    SingleItemBid (int offer, Item toItem) {
        super(1);
        this.offer = offer;
        this.toItem = toItem;
    }
    int getWeight () {
        if (this.offer > this.toItem.reservePrice) {
            return this.offer;
        } else {
            return this.toItem.reservePrice;
        }
    }

    // compare single-item bid based on the item it offers to
    public int compareTo (SingleItemBid sib) {
        int qualityDiff = sib.toItem.quality - this.toItem.quality;
        if (qualityDiff != 0)
            return qualityDiff;
        else {
            return this.toItem.id - sib.toItem.id;
        }
    }
}
class LinearBid extends Bid implements Comparable<LinearBid> {
    int slope;
    int intercept;
    int toItemIndex;

    LinearBid (int intercept, int slope) {
        super(2);
        this.intercept = intercept;
        this.slope = slope;
        this.toItemIndex = -1;
    }
    int getWeight (Item item) {
        int weight = item.quality * this.slope + this.intercept;
        return weight;
    }
    public int compareTo (LinearBid lb) {
        return lb.slope - this.slope;
    }
}

// ****************************************************************
class Bidding {
    /* for debugging */
    public static void print (int violation, int amountToIncrement, int itemIdx) {
        /*
        System.out.println("Vio: " + violation + ", Item_idx: " + itemIdx + 
                ", amount: " + amountToIncrement);
        */
    }

    public static void main (String [] args) throws IOException {
        BufferedReader reader = new BufferedReader(new
                InputStreamReader (System.in));
        /* Parse the number of items */
        int nItems = Integer.parseInt(reader.readLine());
        if (nItems <= 0) return ;
        assert (nItems > 0);
        // System.out.println("number of items: " + n);

        /* Use hash map to restore items */
        HashMap<Integer,Item> hashItems = new HashMap<Integer,Item> (nItems);
        String line = null;
        for (int i = 0; i < nItems; i ++) {
            line = reader.readLine();
            String [] item_infos = line.split(" ");
            int tmp_quality = Integer.parseInt(item_infos[0]);
            int tmp_reservePrice = Integer.parseInt(item_infos[1]);
            int tmp_startPrice = Integer.parseInt(item_infos[2]);
            assert (tmp_startPrice <= tmp_reservePrice): 
                "startPrice is greater than reservePrice.";
            Item newitem = new Item (tmp_quality, tmp_reservePrice,tmp_startPrice);
            hashItems.put(i, newitem);
            // System.out.println(items[i].toString());
        }
        /* Create arraylist and hashmap for single-item bids */
        HashMap<Integer,SingleItemBid> hashSingleBids = new HashMap<Integer,SingleItemBid> ();
        /* Create arraylist and hashmap for linear bids */
        HashMap<Integer,LinearBid> hashLinearBids = new HashMap<Integer,LinearBid> ();

        /* Construct dummies */
        int dummy_id = -1;
        for (int i = 0; i < nItems; i ++, dummy_id--) {
            Item tmp_item = hashItems.get(i);
            SingleItemBid tmp_dummy = new SingleItemBid(tmp_item.reservePrice, tmp_item);
            tmp_dummy.id = dummy_id;
            hashSingleBids.put(dummy_id, tmp_dummy);
        }
        Bid.id_count = 0;

        /* Initialize the priceVector */
        int [] priceVector = new int [nItems];
        for (int i = 0; i < nItems; i ++) {
            priceVector[i] = hashItems.get(i).startPrice;
        }
        int [] MWMCM = new int [nItems];
        while ((line = reader.readLine()) != null) {
            String [] bidding_infos = line.split(" ");
            int type = Integer.parseInt(bidding_infos[0]);
            if (type == 1 || type == 2) { 
                for (int i = 0; i < nItems; i ++) {
                    priceVector[i] = hashItems.get(i).startPrice;
                    MWMCM[i] = Integer.parseInt(bidding_infos[i+3]);
                }
                if (type == 1) {
                    /* Single item bid insertion */
                    int tmp_reservePrice = Integer.parseInt(bidding_infos[1]);
                    int tmp_id = Integer.parseInt(bidding_infos[2]);
                    SingleItemBid newBid = new SingleItemBid(tmp_reservePrice,
                            hashItems.get(tmp_id));
                    hashSingleBids.put (newBid.id, newBid);
                    // System.out.println("1 " + tmp_id + ", " + tmp_reservePrice);
                } else if (type == 2) { 
                    /* Linear bid insertion */
                    int intercept = Integer.parseInt(bidding_infos[1]);
                    int slope = Integer.parseInt(bidding_infos[2]);
                    LinearBid newbid = new LinearBid(intercept, slope);
                    hashLinearBids.put (newbid.id, newbid);
                    // System.out.println("2 " + intercept + ", " + slope);
                }
                /* Create a hashset to restore index of all matched bids */
                HashSet<Integer> allIndexMatchedBids = new HashSet<Integer> ();
                for (int i = 0; i < nItems; i ++) {
                    if (MWMCM[i] < 0) continue;
                    else allIndexMatchedBids.add(MWMCM[i]);
                }

                /* start Algorithm A */
                while (true) {
                    int itemToincrement = -1;
                    int vioType = -1;
                    int incrementAmount = Integer.MAX_VALUE;
                    int tmp_incrementAmount;
                    // STEP ONE: check if violate case 1
                    boolean isViolate = false;
                    for (int itemIdx = 0; itemIdx < nItems; itemIdx++) {
                        int matchedBidIdx = MWMCM[itemIdx];
                        if (matchedBidIdx < 0) { 
                            // win by dummy bid
                            // Since dummy bid does not offer price to
                            // other item, impossible to violate case 1
                            continue;
                        } else if (hashSingleBids.containsKey(matchedBidIdx)) {
                            // win by a non-dummy single-item bid
                            // Since single-item bid does not offer price to
                            // other item, impossible to violate case 1
                            continue;
                        } else if (hashLinearBids.containsKey(matchedBidIdx)) {
                            // win by a linear bid
                            LinearBid u = hashLinearBids.get(matchedBidIdx);
                            int u_slope = u.slope;
                            int u_intercept = u.intercept;
                            Item v_zero = hashItems.get(itemIdx);
                            int v_zero_quality = v_zero.quality;
                            int wuv_zero = u_slope * v_zero_quality + u_intercept;
                            for (int vone = 0; vone < nItems; vone ++) {
                                if (vone == itemIdx) continue;
                                Item v_one = hashItems.get(vone);
                                int v_one_quality = v_one.quality;
                                int wuv_one = u_slope * v_one_quality + u_intercept;
                                if (wuv_zero - priceVector[itemIdx] < wuv_one - priceVector[vone]) {
                                    // it is a violation!
                                    isViolate = true;
                                    itemToincrement = vone;
                                    incrementAmount = wuv_one - wuv_zero +
                                        priceVector[itemIdx] - priceVector[vone];
                                    break;
                                }
                            }
                            if (isViolate) break;
                            else continue;
                        }
                    }
                    // STEP TWO: update the priceVector if yes
                    if (isViolate) {
                        // update the price vector
                        assert (itemToincrement >= 0) : "cannot increment idx < 0";
                        assert (incrementAmount > 0) : "increment a negative number";
                        priceVector[itemToincrement] += incrementAmount;
                        print (1, incrementAmount, itemToincrement);
                        continue;
                    }
                    // STEP THREE: check if violate case 2
                    // We do not need to care about start-price dummy bids, it
                    // can be guaranteed to be no violation for them
                    // (a) check single-item bid first
                    for (int sibIdx: hashSingleBids.keySet()) {
                        SingleItemBid sib = hashSingleBids.get(sibIdx);
                        int offeredItemIdx = sib.toItem.id;
                        int wuv_zero = sib.offer; // offered price
                        // for dummy bid
                        if (sib.id < 0 && MWMCM[sib.toItem.id] < 0) {
                            // this dummy bid matched
                            continue;
                        }
                        if (allIndexMatchedBids.contains(sib.id)) {
                            // matched
                            continue;
                        }
                        if (priceVector[offeredItemIdx] < wuv_zero) {
                            // it is a violation of case 2
                            isViolate = true;
                            itemToincrement = offeredItemIdx;
                            incrementAmount = wuv_zero - priceVector[offeredItemIdx];
                            break;
                        }
                    }
                    // (a) update the price vector for the found violation
                    if (isViolate) {
                        assert (itemToincrement >= 0) : "cannot increment idx < 0";
                        assert (incrementAmount > 0) : "increment a negative number";
                        priceVector[itemToincrement] += incrementAmount;
                        print (2, incrementAmount, itemToincrement);
                        continue;
                    } 
                    // (b) check linear bid then
                    for (int lbIdx: hashLinearBids.keySet()) {
                        LinearBid lb = hashLinearBids.get(lbIdx);
                        int u_slope = lb.slope;
                        int u_intercept = lb.intercept;
                        // if it is matched in MWMCM M?
                        if (allIndexMatchedBids.contains(lb.id)) {
                            // matched, not to check for violation
                            continue;
                        } else { 
                            // unmatched, check for violation
                            for (int itemIdx = 0; itemIdx < nItems; itemIdx ++) {
                                Item v_zero = hashItems.get(itemIdx);
                                int v_zero_quality = v_zero.quality;
                                int wuv_zero = u_slope * v_zero_quality + u_intercept;
                                if (priceVector[itemIdx] < wuv_zero) {
                                    // violation for case 2 detected!
                                    isViolate = true;
                                    itemToincrement = itemIdx;
                                    incrementAmount = wuv_zero - priceVector[itemIdx];
                                    break;
                                }
                            }
                            if (isViolate) break;
                            else continue;
                        }

                    }
                    // (b) update the priceVector if yes
                    if (isViolate) {
                        // update the price vector
                        assert (itemToincrement >= 0) : "cannot increment idx < 0";
                        assert (incrementAmount > 0) : "increment a negative number";
                        priceVector[itemToincrement] += incrementAmount;
                        print (vioType, incrementAmount, itemToincrement);
                        continue;
                    } 
                    // STEP FIVE: break the infinite loop if nothing is found
                    break;
                }
            } else if (type == 3) {
                /* In summary */
                String summary = Integer.toString (priceVector[0]);
                for (int i = 1; i < nItems; i ++) {
                    summary += " " + Integer.toString (priceVector[i]);
                }
                System.out.println(summary);
            } 
        }
    }
}
