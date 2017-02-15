/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.etm.api.jni;

import com.etm.api.var.DpIdentifierVar;
import com.etm.api.var.Variable;

/**
 *
 * @author vogler
 */
public class DpIdValueList extends Malloc {
    public DpIdValueList() {
        super();
    }
    
    public DpIdValueList(long cptr) {
        super.setPointer(cptr);
    }         
    
    public native String toDebug(int level);        
    
    public native DpVCItem getFirstItem();
    public native DpVCItem getNextItem();
    public native DpVCItem cutFirstItem();
    public native int getNumberOfItems();
    public native boolean appendItem(DpIdentifierVar dpid, Variable var);
    
    @Override
    protected native long malloc();

    @Override
    protected native void free(long cptr);    
    
}
