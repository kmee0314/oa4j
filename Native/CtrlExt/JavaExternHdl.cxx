#include <../LibJava/Java.hxx>

#include <JavaExternHdl.hxx>
#include <JavaResources.hxx>
#include <Manager.hxx>     
#include <WaitCond.hxx>

#include <DpIdentifierVar.hxx>
#include <DpVCItem.hxx>               
#include <Variable.hxx>
#include <FloatVar.hxx>
#include <IntegerVar.hxx>
#include <CharVar.hxx>
#include <ULongVar.hxx>
#include <TimeVar.hxx>
#include <AnyTypeVar.hxx>
#include <DynVar.hxx>
#include <UIntegerVar.hxx>

const bool JavaExternHdl::DEBUG = false;

const char *JavaExternHdl::ManagerName = "JavaCtrlExt";

const char *JavaExternHdl::ExternHdlClassName = "at/rocworks/oc4j/jni/ExternHdl";

const char *JavaExternHdl::JavaCallClassName = "at/rocworks/oc4j/jni/ExternHdlFunction";

jclass JavaExternHdl::clsJavaCall;

//------------------------------------------------------------------------------

static FunctionListRec fnList[] =
{
	// TODO add for every new function an entry
	{ INTEGER_VAR, "startJVM", "()", false },
	{ INTEGER_VAR, "stopJVM", "()", false },
	{ INTEGER_VAR, "javaCall", "(string class, string function, dyn_anytype input, dyn_anytype &output)", false },
	{ INTEGER_VAR, "javaCallAsync", "(string class, string function, dyn_anytype input, dyn_anytype &output)", false }
};

CTRL_EXTENSION(JavaExternHdl, fnList)

//------------------------------------------------------------------------------

const Variable *JavaExternHdl::execute(ExecuteParamRec &param)
{
  enum
  {
    F_startJVM = 0,
	F_stopJVM = 1,
	F_javaCall = 2,
	F_javaCallAsync = 3
  };

  switch ( param.funcNum )
  {
	case F_startJVM:		return startVM(param); 
	case F_stopJVM:			return stopVM(param);
	case F_javaCall:	    return javaCall(param);
	case F_javaCallAsync:	return javaCallAsync(param);  
    default:
      return &errorIntVar;
  }
}

// ---------------------------------------------------------------------
// WaitCond
class JavaCallWaitCond : public WaitCond
{
public:
	JavaCallWaitCond(JNIEnv *p_env, CtrlThread *thread, ExprList *args);
	~JavaCallWaitCond();

	virtual const TimeVar &nextCheck() const;
	virtual int checkDone();

	IntegerVar result;

	CtrlThread *thread;
	ExprList *args;
	DynVar *out;

private:
	JNIEnv *env;
	jobject jThread;

};

//----------------------------------------------------------------------
JavaCallWaitCond::JavaCallWaitCond(JNIEnv *p_env, CtrlThread *p_thread, ExprList *p_args)
{
	env = p_env;
	thread = p_thread;
	args = p_args;

	result.setValue(-99);

	const Variable *FunctionClass = (args->getFirst()->evaluate(thread)); // Class(Name) must be a subtype of ExternHdlFunction (T <: ExternHdlFunction)

	jclass clsFunction = Java::FindClass(env, ((TextVar*)FunctionClass)->getString());
	if (clsFunction == nil) {
		std::string msg = "function class not found";
		ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
			JavaExternHdl::ManagerName, ((TextVar*)FunctionClass)->getString(), msg.c_str());
		result.setValue(-1);
	}
	else {
		// public ExternHdlFunction(long waitCondPtr, boolean asThread) {
		jmethodID jMethodInit = env->GetMethodID(clsFunction, "<init>", "(J)V");
		if (jMethodInit == nil) {
			std::string msg = "function constructor not found";
			ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
				JavaExternHdl::ManagerName, "JavaCallWaitCond", msg.c_str());
			result.setValue(-2);
		}
		else {
			const Variable *fun = (args->getNext()->evaluate(thread));
			const Variable *par = (args->getNext()->evaluate(thread));

			out = (DynVar*)args->getNext()->getTarget(thread); // TODO: check if it is a DynVar!
			DynVar tmp(ANYTYPE_VAR);
			*out = tmp;

			this->out = out;

			jobject jfun = Java::convertToJava(env, (VariablePtr)fun);
			jobject jpar = Java::convertToJava(env, (VariablePtr)par);

			jThread = env->NewObject(clsFunction, jMethodInit, (jlong)this); 

			if (env->ExceptionCheck()) {
				std::string msg = "constructor java exception!";
				ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
					JavaExternHdl::ManagerName, "JavaCallWaitCond", msg.c_str());
				env->ExceptionDescribe();
				result.setValue(-3);
			}
			else {
				// public Variable execute(String function, Variable parameter);
				jmethodID jMethodExecute = env->GetMethodID(clsFunction, "start", "(Ljava/lang/String;Lat/rocworks/oc4j/var/DynVar;)V");
				if (jMethodExecute == nil) {
					std::string msg = "method execute not found";
					ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
						JavaExternHdl::ManagerName, "JavaCallWaitCond", msg.c_str());
					result.setValue(-4);
				}
				else {
					env->CallVoidMethod(jThread, jMethodExecute, jfun, jpar);
					if (env->ExceptionCheck()) {
						std::string msg = "start function java exception!";
						ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
							JavaExternHdl::ManagerName, "JavaCallWaitCond", msg.c_str());
						env->ExceptionDescribe();
						result.setValue(-99);
					}
					else {
						result.setValue(0);
					}
				}
			}
			env->DeleteLocalRef(jfun);
			env->DeleteLocalRef(jpar);
		}
		env->DeleteLocalRef(clsFunction);
	}	
}

//----------------------------------------------------------------------
JavaCallWaitCond::~JavaCallWaitCond()
{
	env->DeleteLocalRef(jThread);
}

//----------------------------------------------------------------------
const TimeVar& JavaCallWaitCond::nextCheck() const
{
	static TimeVar timeOfNextCall;
	timeOfNextCall.setCurrentTime();
	timeOfNextCall += TimeVar(0, (PVSSshort)100);
	return timeOfNextCall;
}

//----------------------------------------------------------------------
int JavaCallWaitCond::checkDone()
{
	jmethodID jMethod = Java::GetMethodID(env, JavaExternHdl::clsJavaCall, "checkDone", "()Z");
	if (jMethod == nil) { result.setValue(-98); return true; }
	jboolean jDone = env->CallBooleanMethod(jThread, jMethod);
	return jDone;
}

// ---------------------------------------------------------------------
// startJVM
const Variable* JavaExternHdl::startVM(ExecuteParamRec &param)
{
	static IntegerVar result(-99);

	param.thread->clearLastError();

	if (jvmState == JNI_OK)  {
		ErrHdl::error(ErrClass::PRIO_INFO, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
			ManagerName, "java", CharString("jvm already started"));
		result.setValue(0);
	}
	else {
		JavaResources::init();

		//================== prepare loading of Java VM ============================
		JavaVMInitArgs vm_args;                        // Initialization arguments
		JavaVMOption* options = new JavaVMOption[99];   // JVM invocation options		

		int idx = -1;

		// 0) jvmOption e.g. -Xmx512m
		if (strlen(JavaResources::getJvmOption().c_str()) > 0)
		{
			options[++idx].optionString = JavaResources::getJvmOption();
			std::cout << "jvmOption='" << options[0].optionString << "'" << std::endl;
		}

		// 1) jvmClassPath
		if (strlen(JavaResources::getJvmClassPath().c_str()) > 0)
		{
			std::string cp = "-Djava.class.path=" + (const char&)JavaResources::getJvmClassPath();
			options[++idx].optionString = (char*)cp.c_str();
			std::cout << "jvmClassPath=" << options[idx].optionString << std::endl;
		}

		// 2) jvmLibraryPath
		if (strlen(JavaResources::getJvmLibraryPath().c_str()) > 0)
		{
			std::string lp = "-Djava.library.path=" + (const char&)JavaResources::getJvmLibraryPath();
			options[++idx].optionString = (char*)lp.c_str();
			std::cout << "jvmLibraryPath=" << options[idx].optionString << std::endl;
		}

		// config.java
		std::ifstream t(Resources::getConfigDir() + "config.java");
		std::string line;
		while (std::getline(t, line) && ++idx<99)
		{
			std::cout << "config.java: " << line << std::endl;
			char * cstr = new char[line.length() + 1];
			std::strcpy(cstr, line.c_str());
			options[idx].optionString = cstr;
		}

		vm_args.version = JNI_VERSION_1_8;             // minimum Java version
		vm_args.nOptions = idx + 1;                          // number of options
		vm_args.options = options;
		vm_args.ignoreUnrecognized = true;     // invalid options make the JVM init fail 

		//=============== load and initialize Java VM and JNI interface =============
		jvmState = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
		delete options;    // we then no longer need the initialisation options. 
		if (jvmState != JNI_OK) {
			ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
				ManagerName, "java", CharString("error creating jvm"));
			result.setValue(-1);
		}
		else {
			//=============== Display JVM version =======================================
			std::cout << "JVM load succeeded: Version ";
			jint ver = env->GetVersion();
			std::cout << ((ver >> 16) & 0x0f) << "." << (ver & 0x0f) << std::endl;

			// register native callbacks from java to c++
			const JNINativeMethod methods[] = { 
				{ "apiGetManType", "()I", (void*)&Java_at_rocworks_oc4j_jni_ExternHdl_apiGetManType },
				{ "apiGetManNum", "()I", (void*)&Java_at_rocworks_oc4j_jni_ExternHdl_apiGetManNum },
				{ "apiGetLogDir", "()Ljava/lang/String;", (void*)&Java_at_rocworks_oc4j_jni_ExternHdl_apiGetLogDir },
				{ "apiAddResult", "(JLat/rocworks/oc4j/var/Variable;)I", (void*)&Java_at_rocworks_oc4j_jni_ExternHdl_apiAddResult }
			};


			const int methods_size = sizeof(methods) / sizeof(methods[0]);
			jclass jExternHdlClass = env->FindClass(ExternHdlClassName);
			env->RegisterNatives(jExternHdlClass, methods, methods_size);
			env->DeleteLocalRef(jExternHdlClass);

			// find java extern hdl class
			jclass jClass = Java::FindClass(env, JavaExternHdl::ExternHdlClassName);
			if (jClass == nil) result.setValue(-2);
			else {
				// Initalized ExternHdl
				// TODO pass log path
				jmethodID jMethodInit = Java::GetStaticMethodID(env, jClass, "init", "()V");
				if (jMethodInit == nil) result.setValue(-3);
				else {
					env->CallStaticVoidMethod(jClass, jMethodInit);
					//std::cout << "JVM init done" << std::endl;
					result.setValue(0);
				}
				env->DeleteLocalRef(jClass);

				// Get/Cache Java Classes
				clsJavaCall = Java::FindClass(env, JavaExternHdl::JavaCallClassName);
				if (jClass == nil) result.setValue(-4);
			}
		}
	}
	return &result;
}

// ---------------------------------------------------------------------
// stopJVM
const Variable* JavaExternHdl::stopVM(ExecuteParamRec &param)
{
	static IntegerVar result(-99);

	if (jvmState == JNI_OK && jvm != NULL) {
		jint ret;
		jvmState = ((ret = jvm->DestroyJavaVM()) == JNI_OK) ? JNI_ABORT : JNI_ERR;
		result.setValue(ret);
	}
	else {
		ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
			ManagerName, "java", CharString("jvm not started"));
		result.setValue(jvmState);
	}

	return &result;
}

// ---------------------------------------------------------------------
// javaCallAsync
const Variable* JavaExternHdl::javaCallAsync(ExecuteParamRec &param)
{
	static IntegerVar result(-99);

	param.thread->clearLastError();

	if (jvmState != JNI_OK) {
		ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
			ManagerName, "javaCallAsync", CharString("jvm not started"));
		result.setValue(-1);
		return &result;
	}	

	JavaCallWaitCond *cond = new JavaCallWaitCond(env, param.thread, param.args);
	param.thread->setWaitCond(cond);	
	
	return &cond->result;
}

// ---------------------------------------------------------------------
// javaCall
const Variable* JavaExternHdl::javaCall(ExecuteParamRec &param)
{
	static IntegerVar result(-99);

	if (jvmState != JNI_OK) {
		ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
			ManagerName, "javaCallAsync", CharString("jvm not started"));
		result.setValue(-2);
		return &result;
	}

	ExprList *args = param.args;
	CtrlThread *thread = param.thread;

	const Variable *FunctionClass = (args->getFirst()->evaluate(thread)); // Class(Name) must be a subtype of ExternHdlFunction (T <: ExternHdlFunction)

	jclass clsFunction = Java::FindClass(env, ((TextVar*)FunctionClass)->getString());
	if (clsFunction == nil) {
		std::string msg = "function class not found";
		ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
			JavaExternHdl::ManagerName, ((TextVar*)FunctionClass)->getString().c_str(), msg.c_str());
		result.setValue(-1);
	}
	else {
		// public ExternHdlFunction(long waitCondPtr, boolean asThread) {
		jmethodID jMethodInit = env->GetMethodID(clsFunction, "<init>", "(J)V");
		if (jMethodInit == nil) {
			std::string msg = "constructor not found";
			ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
				JavaExternHdl::ManagerName, "javaCall", msg.c_str());
			result.setValue(-2);
		} 
		else {
			const Variable *fun = (args->getNext()->evaluate(thread));
			const Variable *par = (args->getNext()->evaluate(thread));

			jobject jfun = Java::convertToJava(env, (VariablePtr)fun);
			jobject jpar = Java::convertToJava(env, (VariablePtr)par);

			jobject jobj = env->NewObject(clsFunction, jMethodInit, 0L); // 0...not as thread
			if (env->ExceptionCheck()) {
				std::string msg = "constructor java exception";
				ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
					JavaExternHdl::ManagerName, "javaCall", msg.c_str());
				env->ExceptionDescribe();
				result.setValue(-3);
			}
			else {
				// public Variable execute(String function, Variable parameter);
				jmethodID jMethodExecute = env->GetMethodID(clsFunction, "execute", "(Ljava/lang/String;Lat/rocworks/oc4j/var/DynVar;)Lat/rocworks/oc4j/var/DynVar;");
				if (jMethodExecute == nil) {
					std::string msg = "method execute not found";
					ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
						JavaExternHdl::ManagerName, "javaCall", msg.c_str());
					result.setValue(-4);
				}
				else {
					jobject jvar = env->CallObjectMethod(jobj, jMethodExecute, jfun, jpar);
					if (env->ExceptionCheck()) {
						std::string msg = "execute java exception!";
						ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
							JavaExternHdl::ManagerName, "javaCall", msg.c_str());
						env->ExceptionDescribe();
						result.setValue(-5);
					}
					else {
						DynVar *out = (DynVar*)args->getNext()->getTarget(thread);
						DynVar tmp(ANYTYPE_VAR);
						*out = tmp;

						Variable *var;
						Variable *item;

						if ((var = Java::convertJVariable(env, jvar)) != NULL)
						{
							//std::cout << var->isA(DYN_VAR) << " - " << var->isA() << std::endl;
							if (var->isA(DYN_VAR) == DYN_VAR)
							{
								DynVar *dvar = (DynVar*)var;
								while (item = dvar->cutFirstVar())
								{
									AnyTypeVar *avar = new AnyTypeVar(item);
									if (!out->append(avar))
									{
										ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
											JavaExternHdl::ManagerName, "javaCallAsync", CharString("error adding value to dyn"));
										delete avar;
									}
								}
							}
							delete var;
						}
						result.setValue(0);
					}
					env->DeleteLocalRef(jvar);
				}				
			}	
			env->DeleteLocalRef(jobj);
			env->DeleteLocalRef(jfun);
			env->DeleteLocalRef(jpar);
		}
		env->DeleteLocalRef(clsFunction);
	}
	return &result;
}


JNIEXPORT jstring JNICALL Java_at_rocworks_oc4j_jni_ExternHdl_apiGetLogDir
(JNIEnv *env, jclass)
{
	return env->NewStringUTF(Resources::getLogDir());
}

JNIEXPORT jint JNICALL Java_at_rocworks_oc4j_jni_ExternHdl_apiGetManType
(JNIEnv *env, jclass)
{
	return (jint)Resources::getManType();
}

JNIEXPORT jint JNICALL Java_at_rocworks_oc4j_jni_ExternHdl_apiGetManNum
(JNIEnv *env, jclass)
{
	return (jint)Resources::getManNum();
}

JNIEXPORT jint JNICALL Java_at_rocworks_oc4j_jni_ExternHdl_apiAddResult
(JNIEnv *env, jclass, jlong jWaitCondPtr, jobject jvar)
{
	// get pointer to waitCond object
	JavaCallWaitCond *waitCond = (JavaCallWaitCond*)jWaitCondPtr;

	DynVar *out = (DynVar*)((JavaCallWaitCond*)jWaitCondPtr)->out;

	//DynVar tmp(ANYTYPE_VAR);
	//*out = tmp;

	Variable *var;
	Variable *item;

	if ((var = Java::convertJVariable(env, jvar)) != NULL)
	{
		//std::cout << var->isA(DYN_VAR) << " - " << var->isA() << std::endl;
		if (var->isA(DYN_VAR) == DYN_VAR)
		{
			DynVar *dvar = (DynVar*)var;
			while (item = dvar->cutFirstVar())
			{
				AnyTypeVar *avar = new AnyTypeVar(item);
				if (!out->append(avar))
				{
					ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
						JavaExternHdl::ManagerName, "javaCallAsync", CharString("error adding value to dyn"));
					delete avar;
				}
			}
		}
		delete var;
	}
	return 0;
}