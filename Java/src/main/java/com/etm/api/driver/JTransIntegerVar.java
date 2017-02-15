/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.etm.api.driver;

import com.etm.api.var.IntegerVar;
import com.etm.api.var.Variable;
import com.etm.api.var.VariableType;
import com.etm.api.utils.Debug;
import java.nio.ByteBuffer;
import java.util.logging.Level;

/**
 *
 * @author vogler
 */
public class JTransIntegerVar extends JTransBaseVar {            
        
    public JTransIntegerVar(String name, int type) {
        super(name, type, VariableType.IntegerVar, Integer.SIZE);
    }               
    
    public JTransIntegerVar(String name, int type, int size) {
        super(name, type, VariableType.IntegerVar, size);           
    }       

    protected byte[] toPeriph_(Integer val) { return toPeriph(val); }
    public static byte[] toPeriph(Integer val) {
        ByteBuffer buffer = ByteBuffer.allocate(Integer.SIZE);
        buffer.putInt(val);
        return buffer.array();        
    }
    
    protected Integer toVal_(byte[] data) { return toVal(data); }
    public static Integer toVal(byte[] data) {
        ByteBuffer buffer = ByteBuffer.wrap(data); // big-endian by default
        return buffer.getInt();        
    }    
    
    @Override
    public byte[] toPeriph(int dlen, Variable var, int subix) {
        try {
//            Debug.out.log(Level.INFO, "toPeriph: dlen={0} var={1} subindex={2}", new Object[]{dlen, var.formatValue(), subix});
            //Debug.sleep(100);
            if ( var.getIntegerVar() == null ) {
                Debug.out.log(Level.WARNING, "toPeriph: Variable has no {0} value!", new Object[]{getVariableType().toString()});
                return null;
            } else {
                Integer val = var.getIntegerVar().getValue();
                return toPeriph_(val);
            }
        } catch ( Exception ex) {
            Debug.StackTrace(Level.SEVERE, ex);
            return null;
        }
    }   
        
    @Override
    public Variable toVar(byte[] data, int dlen, int subix) {       
        try {                        
//            Debug.out.log(Level.INFO, "toVar: data={0} dlen={1} subindex={2}", new Object[]{data.toString(), dlen, subix});
//            Debug.sleep(100);
            Integer val = toVal_(data);
            IntegerVar var = new IntegerVar(val);
//            Debug.out.log(Level.INFO, "toVar: data={0}", val);            
            return var;
        } catch ( Exception ex) {
            Debug.StackTrace(Level.SEVERE, ex);
            return null;
        }        
    }
}
