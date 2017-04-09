/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package at.rocworks.oa4j.base;

import at.rocworks.oa4j.jni.Manager;
import at.rocworks.oa4j.var.DpIdentifierVar;
import at.rocworks.oa4j.var.Variable;
import at.rocworks.oa4j.base.JSemaphore;
import java.util.logging.Level;
import at.rocworks.oa4j.base.JDebug;
import java.io.IOException;
import java.util.ArrayList;
import java.util.concurrent.Callable;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

/**
 *
 * @author vogler
 */
public class JManager extends Manager implements Runnable {
    public static final int DB_MAN = 3;        
    public static final int API_MAN = 7;
    
    public static int MAX_ENQUEUE_SIZE_HIGH = 10000;
    public static int MAX_ENQUEUE_SIZE_LOW = 5000;
    public static int MAX_DEQUEUE_SIZE = 1000; // used in JHotLinkWaitForAnswer
    
    private int maxEnqueueSizeReached=0;
    
    private static JManager instance = null; // Singleton
    
    protected JSemaphore loopPaused = new JSemaphore(true);
    protected volatile boolean apiEnabled = false;
    protected volatile boolean apiConnected = false;    
    protected volatile boolean loopBreak = false;
    
    private final ConcurrentHashMap<Integer, JHotLinkWaitForAnswer> hotlinkList = new ConcurrentHashMap<>();    
    private final ConcurrentLinkedQueue<JHotLinkWaitForAnswer> hotlinkQueue = new ConcurrentLinkedQueue<>();
    private final ConcurrentLinkedQueue<Callable> taskQueue = new ConcurrentLinkedQueue<>();
    
    private int loopWaitUSec=10000;
    
    private String projName="<unknown>";
    private String projDir=".";        
    private String confDir="."; 
    
    private int manType=API_MAN;
    private int manNum=1;  

    public JManager setMaxEnqueueSize(int high, int low) {
        this.MAX_ENQUEUE_SIZE_HIGH = high;
        this.MAX_ENQUEUE_SIZE_LOW = low;
        return this;
    }

    public JManager setMaxDequeueSize(int size) {
        this.MAX_DEQUEUE_SIZE = size;
        return this;
    }

    public int getEnqueueSize() {
        return hotlinkQueue.size();
    }

    public static JManager getInstance() {
        return JManager.instance;
    }
    
    public String getProjPath() { return projDir; }
    private JManager setProjPath(String projDir) { 
        this.projDir=projDir; 
        this.confDir=this.projDir+"/config";                 
        return this; 
    }        
    
    public JManager setProjName(String projName) {
        this.projName=projName;
        return this;
    }
    
    public String getConfigDir() { return confDir; }
    public String getLogDir() { return apiGetLogPath(); }
    public String getLogFile() { return getLogDir()+getManName(); }
    public String getConfigValue(String key) { return apiGetConfigValue(key); }
    
    public boolean isEnabled() { return apiEnabled; }
    public boolean isConnected() { return apiConnected; }
    
    public int getManType() { return manType; }   
    private JManager setManType(int manType) { this.manType=manType; return this; }
    
    public int getManNum() { return manNum; }
    public JManager setManNum(int manNum) { this.manNum=manNum; return this; }
    
    public JManager setLoopWaitUSec(int usec) {
        this.loopWaitUSec=usec;
        return this;
    }
    
    public int getLoopWaitUSec() {
        return this.loopWaitUSec;
    }
        
    public JManager init(String args[]) throws Exception {
        for ( int i=0; i<args.length; i++ ) {
            // projDir & configDir
            if ( args[i].equals("-path") && args.length>i+1 ) {
                setProjPath(args[i+1]);        
            }
            
            if ( args[i].equals("-proj") && args.length>i+1 ) {
                setProjName(args[i+1]);
            }            
                        
            // managerNum
            if ( args[i].equals("-num") && args.length>i+1 ) {
                setManNum(Integer.valueOf(args[i+1]));
            }
            
            // managerType
            if ( args[i].equals("-db") ) {
                setManType(DB_MAN);
            }

        }        
        return init();
    }
    
    public JManager init(String projName, int manType, int manNum) throws Exception {
        setProjName(projName);        
        setManType(manType);
        setManNum(manNum);    
        return init();
    }    

    private JManager init() throws Exception {
        if (JManager.instance == null) {
            JManager.instance = this;
        } else {
            throw new IllegalStateException("There can only be one manager!");
        }   
        
        apiEnabled=false;        
        String errmsg="";        
        try {   
            System.loadLibrary("WCCOAjava");
            apiEnabled=true;
        } catch ( java.lang.UnsatisfiedLinkError ex ) {
            errmsg=ex.getMessage();
        }

        if ( !apiEnabled ) {
            JDebug.out.warning(errmsg);
        } else {
            // Set log file settings
            //JDebug.setOutput(getLogDir(), getManName());

        }
        return this;
    }

    public void setDebugOutput() {
        JDebug.setOutput(getLogDir(), getManName());
    }

    public void setDebugConsole() {
        JDebug.setConsole();
    }

    public String getManName() {
        return "WCCOAjava"+manNum;
    }
    
    public void start() {
        start(true, true);
    }
    
    public void start(boolean connectToData, boolean connectToEvent) {   
        if ( apiEnabled ) {
            apiStartup(manType, 
                    new String[]{"WCCOAjava", "-proj", projName, "-num", Integer.toString(manNum)}, 
                    connectToData, connectToEvent);
            loopPaused.sendFalse();
            apiConnected=true;
            new Thread(this).start();
        }
    }
    
    public void stop() {
        if ( apiEnabled ) {
            JDebug.out.log(Level.INFO, "api manager stop.");
            apiConnected = false; // stop run loop
            pause();
            apiShutdown();
        }
    }        

    @Override
    public void run() {
        while (apiConnected) {
            JDebug.out.log(Level.INFO, "api manager loop waiting.");
            loopPaused.awaitFalse();
            JDebug.out.log(Level.INFO, "api manager loop started.");
            while ( !loopBreak ) {                
                synchronized ( this ) {                                    
                    apiDispatch(0, loopWaitUSec); 
                    queueWorker();
                }
            }
            loopPaused.sendTrue();
            JDebug.out.log(Level.INFO, "api manager loop stopped.");
        }
    }   

    public void pause() {
        //JDebug.out.finest("api manager pause ....");
        loopBreak=true;
        loopPaused.awaitTrue();
        //JDebug.out.finest("api manager pause done");
    }

    public void resume() {
        //JDebug.out.finest("api manager resume ....");
        loopBreak=false;
        loopPaused.sendFalse();
        //JDebug.out.finest("api manager resume done");
    }    
    
    protected void enqueueHotlink(JHotLinkWaitForAnswer hl) {
        if ( hotlinkQueue.size() >= MAX_ENQUEUE_SIZE_HIGH )  {
            maxEnqueueSizeReached++;
            if (maxEnqueueSizeReached % 100==1)
                JDebug.out.log(Level.WARNING, "max enqueue size reached {0} discarding hotlink...", hotlinkQueue.size());
//            try {
//                Thread.sleep(100);
//            } catch (InterruptedException ex) {
//                JDebug.StackTrace(Level.SEVERE, ex);
//            }
        } else if ( maxEnqueueSizeReached == 0 ) {
            hotlinkQueue.add(hl);
        } else if ( hotlinkQueue.size() <= MAX_ENQUEUE_SIZE_LOW ) {
            hotlinkQueue.add(hl);            
            maxEnqueueSizeReached=0;        
            JDebug.out.log(Level.WARNING, "enqueue below threshold size {0} processing hotlink...", hotlinkQueue.size());
        }
    }   
    
    protected void register(JHotLinkWaitForAnswer hl) {
        hotlinkList.put(hl.getHdlId(), hl);
    }
    
    protected void deregister(JHotLinkWaitForAnswer hl) {
        hotlinkList.remove(hl.getHdlId());
    }
    
    private void queueWorker() {        
        int k;
        try {
            // Maximum of loop, otherwise it could happen that we are in this 
            // loop forever, if concurrently hotlink-requests are added...
            JHotLinkWaitForAnswer hotlink;        
            for ( k=0; k<=100 && (hotlink=hotlinkQueue.poll()) != null; k++ ) {
                hotlink.call();
            }

            // pop tasks and execute it 
            Callable task;
            for ( k=0; k<=100 && (task=taskQueue.poll()) != null; k++ ) {
                task.call();
            }
        } catch (Exception ex) {
            JDebug.StackTrace(Level.SEVERE, ex);
        }                        
    }
    
    public boolean enqueueTask(Callable task) {
        return taskQueue.add(task);
    }
    
    public Object executeTask(Callable task) {
        ArrayList<Object> res = new ArrayList<>();
        JSemaphore sem = new JSemaphore(false);
        enqueueTask(()->{
            res.add(task.call());
            sem.sendTrue();
            return true;
        });        
        sem.awaitTrue();
        return ( res.size() > 0 ? res.get(0) : null);        
    }
    
    @Override
    public int callbackAnswer(int id, int idx, DpIdentifierVar dpid, Variable var) {
        //JDebug.out.log(Level.INFO, "java got answer id={0} idx={1} dpid={2} var={3}", new Object[]{id, idx, dpid, var});
        JHotLinkWaitForAnswer hdl;
        hdl = hotlinkList.get(id);
 
        //JDebug.out.log(Level.INFO, "java found answer id={0} idx={1} dpid={2} var={3} hdl={4}", new Object[]{id, idx, dpid, var, hdl});

        if (hdl != null) {                      
            //threadPool.execute(ForkJoinTask.adapt(()->{
                switch (idx) {
                    case -1:
                        hdl.answerInit();
                        break;
                    case -2:
                        hdl.callbackDone();
                        break;
                    default:
                        hdl.callbackItem(new JDpVCItem(dpid, var)); 
                        break;
                }                
            //}));
            return 0;
        } else 
            return -1;
    }           
    
    @Override
    public int callbackHotlink(int id, int idx, DpIdentifierVar dpid, Variable var) {
        //JDebug.out.log(Level.INFO, "java got hotlink id={0} idx={1} dpid={2} var={3}", new Object[]{id, idx, dpid, var});
        JHotLinkWaitForAnswer hdl;
        hdl = hotlinkList.get(id);
        
        //JDebug.out.log(Level.INFO, "java found hotlink id={0} idx={1} dpid={2} var={3} hdl={4}", new Object[]{id, idx, dpid, var, hdl});

        if (hdl != null) {
            //threadPool.execute(ForkJoinTask.adapt(()->{
                switch (idx) {
                    case -1:
                        hdl.hotlinkInit();
                        break;
                    case -2:
                        hdl.callbackDone();
                        //threadPool.execute(()-> { hdl.callbackDone(); });
                        break;
                    default:
                        hdl.callbackItem(new JDpVCItem(dpid, var));
                        break;
                }
            //}));
            return 0;
        } else {
            return -1;
        }
    }           
           
    @Override
    public boolean doReceiveSysMsg(long cPtrSysMsg) {
//        SysMsg msg = new SysMsg(cPtrSysMsg);
//        JDebug.out.info("------------SYSMSG DEBUG BEGIN-------------------------");
//        JDebug.out.log(Level.INFO, "isA => {0}", msg.isA());
//        JDebug.out.info(msg.toDebug(99));
//        JDebug.out.info("------------SYSMSG DEBUG END  -------------------------");
        this.apiDoReceiveSysMsg(cPtrSysMsg);
        return true;
    }

    @Override
    public boolean doReceiveDpMsg(long cPtrDpMsg) {
//        DpMsg msg = new DpMsg(cPtrDpMsg);
//        JDebug.out.info("------------DPMSG DEBUG BEGIN-------------------------");
//        JDebug.out.log(Level.INFO, "isA => {0}", msg.isA());
//        JDebug.out.info(msg.toDebug(99));
//        JDebug.out.info("------------DPMSG DEBUG END  -------------------------");
        this.apiDoReceiveDpMsg(cPtrDpMsg);
        return true;
    }
    
    // NOT USED
    
    @Override
    public int callbackHotlinkGroup(int id, long ptrDpHlGroup) {
        //threadPool.execute(()->{ // didn't work
            apiProcessHotlinkGroup(id, ptrDpHlGroup);
        //});
        return 0;
    }
    
}
