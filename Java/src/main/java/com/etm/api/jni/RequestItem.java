/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.etm.api.jni;

import com.etm.api.var.DpIdentifierVar;

/**
 *
 * @author vogler
 */
public class RequestItem extends Malloc {
    
    public RequestItem(long cptr) {
        super.setPointer(cptr);
    }
    
    public native DpIdentifierVar getId();
    public native int getNumber();       
    public native String toDebug(int level);
    
    @Override
    protected long malloc() {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    protected native void free(long cptr);
    
}
