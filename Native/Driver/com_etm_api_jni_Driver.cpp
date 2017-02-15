#include <WCCOAJavaDrv.hxx>
#include <WCCOAJavaResources.hxx>
#include <../LibJava/Java.hxx>
#include <com_etm_api_jni_Driver.h>

#include <DpIdentifierVar.hxx>

//------------------------------------------------------------------------------------------------
// JAVA JNI GetLogPath

JNIEXPORT jstring JNICALL Java_com_etm_api_jni_Driver_apiGetLogPath
(JNIEnv *env, jobject obj)
{
	CharString path = WCCOAJavaResources::getLogDir();
	jstring js = env->NewStringUTF(path);
	return js;
}

JNIEXPORT jstring JNICALL Java_com_etm_api_jni_Driver_apiGetDataPath
(JNIEnv *env, jobject obj)
{
	CharString path = WCCOAJavaResources::getDataDir();
	jstring js = env->NewStringUTF(path);
	return js;
}

//------------------------------------------------------------------------------------------------
// JAVA JNI startup

JNIEXPORT void JNICALL Java_com_etm_api_jni_Driver_apiStartup
(JNIEnv *env, jobject obj, jobjectArray jargv)
{
	int argc = env->GetArrayLength(jargv);
	char **argv = (char **)malloc(argc * sizeof(char *));
	for (int i = 0; i < argc; i++)
	{
		jobject jstr = env->GetObjectArrayElement(jargv, i);
		Java::copyJavaStringToString(env, (jstring)jstr, &argv[i]);
		env->DeleteLocalRef(jstr);
	}

	std::cout << "InitResources..." << std::endl;
	WCCOAJavaResources resources;
	resources.init(argc, argv);

	std::cout << "InitDriver..." << std::endl;
	WCCOAJavaDrv::thisManager = new WCCOAJavaDrv;
	WCCOAJavaDrv::thisManager->javaInitialize(env, obj);

	std::cout << "MainProcedure..." << std::endl;
	WCCOAJavaDrv::thisManager->mainProcedure(argc, argv);
}

