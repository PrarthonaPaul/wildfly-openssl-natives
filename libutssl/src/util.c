#include "utssl.h"
#include "utssl_private.h"
#include <jni.h>

#include <unistd.h>

static JavaVM     *tcn_global_vm = NULL;

static jclass    jString_class;
static jmethodID jString_init;
static jmethodID jString_getBytes;
#define TCN_PARENT_IDE  "TCN_PARENT_ID"

int tcn_parent_pid = 0;

/* Called by the JVM when APR_JAVA is loaded */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    void   *ppe;

    if ((*vm)->GetEnv(vm, &ppe, JNI_VERSION_1_4)) {
        return JNI_ERR;
    }
    tcn_global_vm = vm;
    env           = (JNIEnv *)ppe;

    /* Initialize global java.lang.String class */
    TCN_LOAD_CLASS(env, jString_class, "java/lang/String", JNI_ERR);

    TCN_GET_METHOD(env, jString_class, jString_init,
                   "<init>", "([B)V", JNI_ERR);
    TCN_GET_METHOD(env, jString_class, jString_getBytes,
                   "getBytes", "()[B", JNI_ERR);
#ifdef WIN32
    {
        char *ppid = getenv(TCN_PARENT_IDE);
        if (ppid)
            tcn_parent_pid = atoi(ppid);
    }
#else
    tcn_parent_pid = getppid();
#endif

    return  JNI_VERSION_1_4;
}


/* Called by the JVM before the APR_JAVA is unloaded */
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    void   *ppe;

    if ((*vm)->GetEnv(vm, &ppe, JNI_VERSION_1_2)) {
        return;
    }
    if (jString_class) {
        env  = (JNIEnv *)ppe;
        TCN_UNLOAD_CLASS(env, jString_class);
    }
}


jint throwIllegalStateException( JNIEnv *env, char *message )
{
    jclass exClass;
    char *className = "java/lang/IllegalStateException";

    exClass = (*env)->FindClass( env, className);
    return (*env)->ThrowNew( env, exClass, message );
}


void tcn_Throw(JNIEnv *env, char *fmt, ...) {
    throwIllegalStateException(env, fmt);
//TODO
/*
    char msg[8124] = {'\0'};
    va_list ap;

    va_start(ap, fmt);
    snprintf(msg, 8124, fmt, ap);
    throwIllegalStateException(env, msg);
    va_end(ap);*/
}

jint tcn_get_java_env(JNIEnv **env)
{
    if ((*tcn_global_vm)->GetEnv(tcn_global_vm, (void **)env,
                                 JNI_VERSION_1_4)) {
        return JNI_ERR;
    }
    return JNI_OK;
}


JavaVM * tcn_get_java_vm()
{
    return tcn_global_vm;
}
