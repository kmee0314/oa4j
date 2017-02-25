package at.rocworks.oa4j.jni;

import at.rocworks.oa4j.base.JDebug;
import at.rocworks.oa4j.var.DynVar;
import at.rocworks.oa4j.var.TextVar;
import at.rocworks.oa4j.var.Variable;

import java.util.logging.Level;

/**
 * Created by vogler on 2/19/2017.
 */
abstract public class ExternHdlFunction implements Runnable {
    private volatile boolean done = false;

    private final long waitCondPtr; // c++ pointer to waitCond object

    private TextVar function;
    private DynVar parameter;

    public ExternHdlFunction(long waitCondPtr) {
        this.waitCondPtr = waitCondPtr;
    }

    /**
     * Internal, is called from the JNI in the case of an async function
     * @param function Function name which will be passed to execute function
     * @param parameter Parameters which will be passed to execute function
     */
    public void start(TextVar function, DynVar parameter) {
        //JDebug.out.log(Level.INFO, "start function={0} parameter={1}", new Object[] { function, parameter.formatValue() });

        this.function=function;
        this.parameter=parameter;

        new Thread(this).start();
    }

    /**
     * Function which will be executed from WinCC OA Control script
     * @param function Name of a (sub)program unit, must be handled by yourself (e.g. case statement)
     * @param parameter List of parameter values
     * @return List of values
     */
    abstract public DynVar execute(TextVar function, DynVar parameter);

    /**
     * Internal, called from JNI in the case of threaded function
     * @return true when thread is finished
     */
    public boolean checkDone() {
        return done;
    }

    /**
     * Add a result values in the case when the function is called async
     * @param var Variable which should be passed out
     */
    public void addResult(Variable var) throws IllegalStateException {
        if ( waitCondPtr > 0 )
            ExternHdl.apiAddResult(waitCondPtr, var);
        else
            throw new IllegalStateException("no waitCondPtr available!");
    }

    /**
     * Start Control Function / Callback to Control
     * @param name Functionname
     * @param args Arguments
     * @return EXEC_OK=0, EXEC_ERROR=1, EXEC_DONE=2, -99..not an async call
     */
    public int callback(String name, Variable args) throws IllegalStateException {
        if ( waitCondPtr > 0 )
            return ExternHdl.apiStartFunc(waitCondPtr, name, args);
        else
            throw new IllegalStateException("no waitCondPtr available!");
    }

    /**
     * Internal, called when function is called async
     */
    @Override
    public void run() {
        try {
            addResult(execute(function, parameter));
        } catch (Exception ex) {
            JDebug.StackTrace(Level.SEVERE, ex);
        }
        done = true;
    }
}
