/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>

/** This is the C++ implementation of the Java native method
which returns a new instance of JVM String.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT jstring JNICALL
Java_com_leggiero_${{ProgramName}}_Android_stringFromJNI( JNIEnv* env, jobject thiz )
{
    return env->NewStringUTF("Hello, HAPPY World!");
}
