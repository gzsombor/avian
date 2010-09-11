/* Copyright (c) 2008-2009, Avian Contributors

   Permission to use, copy, modify, and/or distribute this software
   for any purpose with or without fee is hereby granted, provided
   that the above copyright notice and this permission notice appear
   in all copies.

   There is NO WARRANTY for this software.  See license.txt for
   details. */

#ifndef MACHINE_H
#define MACHINE_H

#include "common.h"
#include "system.h"
#include "heap.h"
#include "finder.h"
#include "processor.h"
#include "constants.h"
#include "arch.h"

#ifdef PLATFORM_WINDOWS
#  define JNICALL __stdcall
#else
#  define JNICALL
#endif

#define PROTECT(thread, name)                                   \
  Thread::SingleProtector MAKE_NAME(protector_) (thread, &name);

#define ACQUIRE(t, x) MonitorResource MAKE_NAME(monitorResource_) (t, x)

#define ACQUIRE_RAW(t, x) RawMonitorResource MAKE_NAME(monitorResource_) (t, x)

#define ENTER(t, state) StateResource MAKE_NAME(stateResource_) (t, state)

namespace vm {

const bool Verbose = false;
const bool DebugRun = false;
const bool DebugStack = false;
const bool DebugMonitors = false;
const bool DebugReferences = false;

const uintptr_t HashTakenMark = 1;
const uintptr_t ExtendedMark = 2;
const uintptr_t FixedMark = 3;

const unsigned ThreadHeapSizeInBytes = 64 * 1024;
const unsigned ThreadHeapSizeInWords = ThreadHeapSizeInBytes / BytesPerWord;

const unsigned ThreadBackupHeapSizeInBytes = 2 * 1024;
const unsigned ThreadBackupHeapSizeInWords
= ThreadBackupHeapSizeInBytes / BytesPerWord;

const unsigned ThreadHeapPoolSize = 64;

const unsigned FixedFootprintThresholdInBytes
= ThreadHeapPoolSize * ThreadHeapSizeInBytes;

enum FieldCode {
  VoidField,
  ByteField,
  CharField,
  DoubleField,
  FloatField,
  IntField,
  LongField,
  ShortField,
  BooleanField,
  ObjectField
};

enum StackTag {
  IntTag, // must be zero
  ObjectTag
};

const int NativeLine = -1;
const int UnknownLine = -2;

// class vmFlags:
const unsigned ReferenceFlag = 1 << 0;
const unsigned WeakReferenceFlag = 1 << 1;
const unsigned NeedInitFlag = 1 << 2;
const unsigned InitFlag = 1 << 3;
const unsigned InitErrorFlag = 1 << 4;
const unsigned PrimitiveFlag = 1 << 5;
const unsigned BootstrapFlag = 1 << 6;
const unsigned HasFinalizerFlag = 1 << 7;
const unsigned LinkFlag = 1 << 8;
const unsigned HasFinalMemberFlag = 1 << 9;
const unsigned SingletonFlag = 1 << 10;
const unsigned ContinuationFlag = 1 << 11;

// method vmFlags:
const unsigned ClassInitFlag = 1 << 0;
const unsigned CompiledFlag = 1 << 1;
const unsigned ConstructorFlag = 1 << 2;
const unsigned FastNative = 1 << 3;

#ifndef JNI_VERSION_1_6
#define JNI_VERSION_1_6 0x00010006
#endif

typedef Machine JavaVM;
typedef Thread JNIEnv;

typedef uint8_t jboolean;
typedef int8_t jbyte;
typedef uint16_t jchar;
typedef int16_t jshort;
typedef int32_t jint;
typedef int64_t jlong;
typedef float jfloat;
typedef double jdouble;

typedef jint jsize;

typedef object* jobject;

typedef jobject jclass;
typedef jobject jthrowable;
typedef jobject jstring;
typedef jobject jweak;

typedef jobject jarray;
typedef jarray jbooleanArray;
typedef jarray jbyteArray;
typedef jarray jcharArray;
typedef jarray jshortArray;
typedef jarray jintArray;
typedef jarray jlongArray;
typedef jarray jfloatArray;
typedef jarray jdoubleArray;
typedef jarray jobjectArray;

typedef uintptr_t jfieldID;
typedef uintptr_t jmethodID;

union jvalue {
  jboolean z;
  jbyte    b;
  jchar    c;
  jshort   s;
  jint     i;
  jlong    j;
  jfloat   f;
  jdouble  d;
  jobject  l;
};

struct JNINativeMethod {
  char* name;
  char* signature;
  void* function;
};

struct JavaVMVTable {
  void* reserved0;
  void* reserved1;
  void* reserved2;

#if (! TARGET_RT_MAC_CFM) && defined(__ppc__)
  void* cfm_vectors[4];
#endif

  jint
  (JNICALL *DestroyJavaVM)
  (JavaVM*);

  jint
  (JNICALL *AttachCurrentThread)
  (JavaVM*, JNIEnv**, void*);

  jint
  (JNICALL *DetachCurrentThread)
  (JavaVM*);

  jint
  (JNICALL *GetEnv)
  (JavaVM*, JNIEnv**, jint);

  jint
  (JNICALL *AttachCurrentThreadAsDaemon)
  (JavaVM*, JNIEnv**, void*);

#if TARGET_RT_MAC_CFM && defined(__ppc__)
    void* real_functions[5];
#endif
};

struct JNIEnvVTable {
  void* reserved0;
  void* reserved1;
  void* reserved2;
  void* reserved3;

#if (! TARGET_RT_MAC_CFM) && defined(__ppc__)
  void* cfm_vectors[225];
#endif

  jint
  (JNICALL *GetVersion)
    (JNIEnv*);

  jclass
  (JNICALL *DefineClass)
    (JNIEnv*, const char*, jobject, const jbyte*, jsize);

  jclass
  (JNICALL *FindClass)
    (JNIEnv*, const char*);

  jmethodID
  (JNICALL *FromReflectedMethod)
    (JNIEnv*, jobject);

  jfieldID
  (JNICALL *FromReflectedField)
    (JNIEnv*, jobject);

  jobject
  (JNICALL *ToReflectedMethod)
    (JNIEnv*, jclass, jmethodID, jboolean);

  jclass
  (JNICALL *GetSuperclass)
    (JNIEnv*, jclass);

  jboolean
  (JNICALL *IsAssignableFrom)
    (JNIEnv*, jclass, jclass);

  jobject
  (JNICALL *ToReflectedField)
    (JNIEnv*, jclass, jfieldID, jboolean);

  jint
  (JNICALL *Throw)
    (JNIEnv*, jthrowable);

  jint
  (JNICALL *ThrowNew)
    (JNIEnv*, jclass, const char*);

  jthrowable
  (JNICALL *ExceptionOccurred)
    (JNIEnv*);

  void
  (JNICALL *ExceptionDescribe)
  (JNIEnv*);

  void
  (JNICALL *ExceptionClear)
  (JNIEnv*);

  void
  (JNICALL *FatalError)
  (JNIEnv*, const char*);

  jint
  (JNICALL *PushLocalFrame)
    (JNIEnv*, jint);

  jobject
  (JNICALL *PopLocalFrame)
    (JNIEnv*, jobject);

  jobject
  (JNICALL *NewGlobalRef)
    (JNIEnv*, jobject);

  void
  (JNICALL *DeleteGlobalRef)
  (JNIEnv*, jobject);

  void
  (JNICALL *DeleteLocalRef)
  (JNIEnv*, jobject);

  jboolean
  (JNICALL *IsSameObject)
    (JNIEnv*, jobject, jobject);

  jobject
  (JNICALL *NewLocalRef)
    (JNIEnv*, jobject);

  jint
  (JNICALL *EnsureLocalCapacity)
    (JNIEnv*, jint);

  jobject
  (JNICALL *AllocObject)
    (JNIEnv*, jclass);

  jobject
  (JNICALL *NewObject)
    (JNIEnv*, jclass, jmethodID, ...);

  jobject
  (JNICALL *NewObjectV)
    (JNIEnv*, jclass, jmethodID, va_list);

  jobject
  (JNICALL *NewObjectA)
    (JNIEnv*, jclass, jmethodID, const jvalue*);

  jclass
  (JNICALL *GetObjectClass)
    (JNIEnv*, jobject);

  jboolean
  (JNICALL *IsInstanceOf)
    (JNIEnv*, jobject, jclass);

  jmethodID
  (JNICALL *GetMethodID)
    (JNIEnv*, jclass, const char*, const char*);

  jobject
  (JNICALL *CallObjectMethod)
    (JNIEnv*, jobject, jmethodID, ...);

  jobject
  (JNICALL *CallObjectMethodV)
    (JNIEnv*, jobject, jmethodID, va_list);

  jobject
  (JNICALL *CallObjectMethodA)
    (JNIEnv*, jobject, jmethodID, const jvalue*);

  jboolean
  (JNICALL *CallBooleanMethod)
    (JNIEnv*, jobject, jmethodID, ...);

  jboolean
  (JNICALL *CallBooleanMethodV)
    (JNIEnv*, jobject, jmethodID, va_list);

  jboolean
  (JNICALL *CallBooleanMethodA)
    (JNIEnv*, jobject, jmethodID, const jvalue*);

  jbyte
  (JNICALL *CallByteMethod)
    (JNIEnv*, jobject, jmethodID, ...);

  jbyte
  (JNICALL *CallByteMethodV)
    (JNIEnv*, jobject, jmethodID, va_list);

  jbyte
  (JNICALL *CallByteMethodA)
    (JNIEnv*, jobject, jmethodID, const jvalue*);

  jchar
  (JNICALL *CallCharMethod)
    (JNIEnv*, jobject, jmethodID, ...);

  jchar
  (JNICALL *CallCharMethodV)
    (JNIEnv*, jobject, jmethodID, va_list);

  jchar
  (JNICALL *CallCharMethodA)
    (JNIEnv*, jobject, jmethodID, const jvalue*);

  jshort
  (JNICALL *CallShortMethod)
    (JNIEnv*, jobject, jmethodID, ...);

  jshort
  (JNICALL *CallShortMethodV)
    (JNIEnv*, jobject, jmethodID, va_list);

  jshort
  (JNICALL *CallShortMethodA)
    (JNIEnv*, jobject, jmethodID, const jvalue*);

  jint
  (JNICALL *CallIntMethod)
    (JNIEnv*, jobject, jmethodID, ...);

  jint
  (JNICALL *CallIntMethodV)
    (JNIEnv*, jobject, jmethodID, va_list);

  jint
  (JNICALL *CallIntMethodA)
    (JNIEnv*, jobject, jmethodID, const jvalue*);

  jlong
  (JNICALL *CallLongMethod)
    (JNIEnv*, jobject, jmethodID, ...);

  jlong
  (JNICALL *CallLongMethodV)
    (JNIEnv*, jobject, jmethodID, va_list);

  jlong
  (JNICALL *CallLongMethodA)
    (JNIEnv*, jobject, jmethodID, const jvalue*);

  jfloat
  (JNICALL *CallFloatMethod)
  (JNIEnv*, jobject, jmethodID, ...);

  jfloat
  (JNICALL *CallFloatMethodV)
  (JNIEnv*, jobject, jmethodID, va_list);

  jfloat
  (JNICALL *CallFloatMethodA)
  (JNIEnv*, jobject, jmethodID, const jvalue*);

  jdouble
  (JNICALL *CallDoubleMethod)
  (JNIEnv*, jobject, jmethodID, ...);

  jdouble
  (JNICALL *CallDoubleMethodV)
  (JNIEnv*, jobject, jmethodID, va_list);

  jdouble
  (JNICALL *CallDoubleMethodA)
  (JNIEnv*, jobject, jmethodID, const jvalue*);

  void
  (JNICALL *CallVoidMethod)
  (JNIEnv*, jobject, jmethodID, ...);

  void
  (JNICALL *CallVoidMethodV)
  (JNIEnv*, jobject, jmethodID, va_list);

  void
  (JNICALL *CallVoidMethodA)
  (JNIEnv*, jobject, jmethodID, const jvalue*);

  jobject
  (JNICALL *CallNonvirtualObjectMethod)
    (JNIEnv*, jobject, jclass, jmethodID, ...);

  jobject
  (JNICALL *CallNonvirtualObjectMethodV)
    (JNIEnv*, jobject, jclass, jmethodID, va_list);

  jobject
  (JNICALL *CallNonvirtualObjectMethodA)
    (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);

  jboolean
  (JNICALL *CallNonvirtualBooleanMethod)
    (JNIEnv*, jobject, jclass, jmethodID, ...);

  jboolean
  (JNICALL *CallNonvirtualBooleanMethodV)
    (JNIEnv*, jobject, jclass, jmethodID, va_list);

  jboolean
  (JNICALL *CallNonvirtualBooleanMethodA)
    (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);

  jbyte
  (JNICALL *CallNonvirtualByteMethod)
    (JNIEnv*, jobject, jclass, jmethodID, ...);

  jbyte
  (JNICALL *CallNonvirtualByteMethodV)
    (JNIEnv*, jobject, jclass, jmethodID, va_list);

  jbyte
  (JNICALL *CallNonvirtualByteMethodA)
    (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);

  jchar
  (JNICALL *CallNonvirtualCharMethod)
    (JNIEnv*, jobject, jclass, jmethodID, ...);

  jchar
  (JNICALL *CallNonvirtualCharMethodV)
    (JNIEnv*, jobject, jclass, jmethodID, va_list);

  jchar
  (JNICALL *CallNonvirtualCharMethodA)
    (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);

  jshort
  (JNICALL *CallNonvirtualShortMethod)
    (JNIEnv*, jobject, jclass, jmethodID, ...);

  jshort
  (JNICALL *CallNonvirtualShortMethodV)
    (JNIEnv*, jobject, jclass, jmethodID,
     va_list);

  jshort
  (JNICALL *CallNonvirtualShortMethodA)
    (JNIEnv*, jobject, jclass, jmethodID,
     const jvalue*);

  jint
  (JNICALL *CallNonvirtualIntMethod)
    (JNIEnv*, jobject, jclass, jmethodID, ...);

  jint
  (JNICALL *CallNonvirtualIntMethodV)
    (JNIEnv*, jobject, jclass, jmethodID,
     va_list);

  jint
  (JNICALL *CallNonvirtualIntMethodA)
    (JNIEnv*, jobject, jclass, jmethodID,
     const jvalue*);

  jlong
  (JNICALL *CallNonvirtualLongMethod)
    (JNIEnv*, jobject, jclass, jmethodID, ...);

  jlong
  (JNICALL *CallNonvirtualLongMethodV)
    (JNIEnv*, jobject, jclass, jmethodID,
     va_list);
  jlong
  (JNICALL *CallNonvirtualLongMethodA)
    (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);

  jfloat
  (JNICALL *CallNonvirtualFloatMethod)
  (JNIEnv*, jobject, jclass, jmethodID, ...);

  jfloat
  (JNICALL *CallNonvirtualFloatMethodV)
  (JNIEnv*, jobject, jclass, jmethodID, va_list);

  jfloat
  (JNICALL *CallNonvirtualFloatMethodA)
  (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);

  jdouble
  (JNICALL *CallNonvirtualDoubleMethod)
  (JNIEnv*, jobject, jclass, jmethodID, ...);

  jdouble
  (JNICALL *CallNonvirtualDoubleMethodV)
  (JNIEnv*, jobject, jclass, jmethodID, va_list);

  jdouble
  (JNICALL *CallNonvirtualDoubleMethodA)
  (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);

  void
  (JNICALL *CallNonvirtualVoidMethod)
  (JNIEnv*, jobject, jclass, jmethodID, ...);

  void
  (JNICALL *CallNonvirtualVoidMethodV)
  (JNIEnv*, jobject, jclass, jmethodID, va_list);

  void
  (JNICALL *CallNonvirtualVoidMethodA)
  (JNIEnv*, jobject, jclass, jmethodID, const jvalue*);

  jfieldID
  (JNICALL *GetFieldID)
    (JNIEnv*, jclass, const char*, const char*);

  jobject
  (JNICALL *GetObjectField)
    (JNIEnv*, jobject, jfieldID);

  jboolean
  (JNICALL *GetBooleanField)
    (JNIEnv*, jobject, jfieldID);

  jbyte
  (JNICALL *GetByteField)
    (JNIEnv*, jobject, jfieldID);

  jchar
  (JNICALL *GetCharField)
    (JNIEnv*, jobject, jfieldID);

  jshort
  (JNICALL *GetShortField)
    (JNIEnv*, jobject, jfieldID);

  jint
  (JNICALL *GetIntField)
    (JNIEnv*, jobject, jfieldID);

  jlong
  (JNICALL *GetLongField)
    (JNIEnv*, jobject, jfieldID);

  jfloat
  (JNICALL *GetFloatField)
  (JNIEnv*, jobject, jfieldID);

  jdouble
  (JNICALL *GetDoubleField)
  (JNIEnv*, jobject, jfieldID);

  void
  (JNICALL *SetObjectField)
  (JNIEnv*, jobject, jfieldID, jobject);

  void
  (JNICALL *SetBooleanField)
  (JNIEnv*, jobject, jfieldID, jboolean);

  void
  (JNICALL *SetByteField)
  (JNIEnv*, jobject, jfieldID, jbyte);

  void
  (JNICALL *SetCharField)
  (JNIEnv*, jobject, jfieldID, jchar);

  void
  (JNICALL *SetShortField)
  (JNIEnv*, jobject, jfieldID, jshort);

  void
  (JNICALL *SetIntField)
  (JNIEnv*, jobject, jfieldID, jint);

  void
  (JNICALL *SetLongField)
  (JNIEnv*, jobject, jfieldID, jlong);

  void
  (JNICALL *SetFloatField)
  (JNIEnv*, jobject, jfieldID, jfloat);

  void
  (JNICALL *SetDoubleField)
  (JNIEnv*, jobject, jfieldID, jdouble);

  jmethodID
  (JNICALL *GetStaticMethodID)
    (JNIEnv*, jclass, const char*, const char*);

  jobject
  (JNICALL *CallStaticObjectMethod)
    (JNIEnv*, jclass, jmethodID, ...);

  jobject
  (JNICALL *CallStaticObjectMethodV)
    (JNIEnv*, jclass, jmethodID, va_list);

  jobject
  (JNICALL *CallStaticObjectMethodA)
    (JNIEnv*, jclass, jmethodID, const jvalue*);

  jboolean
  (JNICALL *CallStaticBooleanMethod)
    (JNIEnv*, jclass, jmethodID, ...);

  jboolean
  (JNICALL *CallStaticBooleanMethodV)
    (JNIEnv*, jclass, jmethodID, va_list);

  jboolean
  (JNICALL *CallStaticBooleanMethodA)
    (JNIEnv*, jclass, jmethodID, const jvalue*);

  jbyte
  (JNICALL *CallStaticByteMethod)
    (JNIEnv*, jclass, jmethodID, ...);

  jbyte
  (JNICALL *CallStaticByteMethodV)
    (JNIEnv*, jclass, jmethodID, va_list);

  jbyte
  (JNICALL *CallStaticByteMethodA)
    (JNIEnv*, jclass, jmethodID, const jvalue*);

  jchar
  (JNICALL *CallStaticCharMethod)
    (JNIEnv*, jclass, jmethodID, ...);

  jchar
  (JNICALL *CallStaticCharMethodV)
    (JNIEnv*, jclass, jmethodID, va_list);

  jchar
  (JNICALL *CallStaticCharMethodA)
    (JNIEnv*, jclass, jmethodID, const jvalue*);

  jshort
  (JNICALL *CallStaticShortMethod)
    (JNIEnv*, jclass, jmethodID, ...);

  jshort
  (JNICALL *CallStaticShortMethodV)
    (JNIEnv*, jclass, jmethodID, va_list);

  jshort
  (JNICALL *CallStaticShortMethodA)
    (JNIEnv*, jclass, jmethodID, const jvalue*);

  jint
  (JNICALL *CallStaticIntMethod)
    (JNIEnv*, jclass, jmethodID, ...);

  jint
  (JNICALL *CallStaticIntMethodV)
    (JNIEnv*, jclass, jmethodID, va_list);

  jint
  (JNICALL *CallStaticIntMethodA)
    (JNIEnv*, jclass, jmethodID, const jvalue*);

  jlong
  (JNICALL *CallStaticLongMethod)
    (JNIEnv*, jclass, jmethodID, ...);

  jlong
  (JNICALL *CallStaticLongMethodV)
    (JNIEnv*, jclass, jmethodID, va_list);

  jlong
  (JNICALL *CallStaticLongMethodA)
    (JNIEnv*, jclass, jmethodID, const jvalue*);

  jfloat
  (JNICALL *CallStaticFloatMethod)
  (JNIEnv*, jclass, jmethodID, ...);

  jfloat
  (JNICALL *CallStaticFloatMethodV)
  (JNIEnv*, jclass, jmethodID, va_list);

  jfloat
  (JNICALL *CallStaticFloatMethodA)
  (JNIEnv*, jclass, jmethodID, const jvalue*);

  jdouble
  (JNICALL *CallStaticDoubleMethod)
  (JNIEnv*, jclass, jmethodID, ...);

  jdouble
  (JNICALL *CallStaticDoubleMethodV)
  (JNIEnv*, jclass, jmethodID, va_list);

  jdouble
  (JNICALL *CallStaticDoubleMethodA)
  (JNIEnv*, jclass, jmethodID, const jvalue*);

  void
  (JNICALL *CallStaticVoidMethod)
  (JNIEnv*, jclass, jmethodID, ...);

  void
  (JNICALL *CallStaticVoidMethodV)
  (JNIEnv*, jclass, jmethodID, va_list);

  void
  (JNICALL *CallStaticVoidMethodA)
  (JNIEnv*, jclass, jmethodID, const jvalue*);

  jfieldID
  (JNICALL *GetStaticFieldID)
    (JNIEnv*, jclass, const char*, const char*);

  jobject
  (JNICALL *GetStaticObjectField)
    (JNIEnv*, jclass, jfieldID);

  jboolean
  (JNICALL *GetStaticBooleanField)
    (JNIEnv*, jclass, jfieldID);

  jbyte
  (JNICALL *GetStaticByteField)
    (JNIEnv*, jclass, jfieldID);

  jchar
  (JNICALL *GetStaticCharField)
    (JNIEnv*, jclass, jfieldID);

  jshort
  (JNICALL *GetStaticShortField)
    (JNIEnv*, jclass, jfieldID);

  jint
  (JNICALL *GetStaticIntField)
    (JNIEnv*, jclass, jfieldID);

  jlong
  (JNICALL *GetStaticLongField)
    (JNIEnv*, jclass, jfieldID);

  jfloat
  (JNICALL *GetStaticFloatField)
  (JNIEnv*, jclass, jfieldID);

  jdouble
  (JNICALL *GetStaticDoubleField)
  (JNIEnv*, jclass, jfieldID);

  void
  (JNICALL *SetStaticObjectField)
  (JNIEnv*, jclass, jfieldID, jobject);

  void
  (JNICALL *SetStaticBooleanField)
  (JNIEnv*, jclass, jfieldID, jboolean);

  void
  (JNICALL *SetStaticByteField)
  (JNIEnv*, jclass, jfieldID, jbyte);

  void
  (JNICALL *SetStaticCharField)
  (JNIEnv*, jclass, jfieldID, jchar);

  void
  (JNICALL *SetStaticShortField)
  (JNIEnv*, jclass, jfieldID, jshort);

  void
  (JNICALL *SetStaticIntField)
  (JNIEnv*, jclass, jfieldID, jint);

  void
  (JNICALL *SetStaticLongField)
  (JNIEnv*, jclass, jfieldID, jlong);

  void
  (JNICALL *SetStaticFloatField)
  (JNIEnv*, jclass, jfieldID, jfloat);

  void
  (JNICALL *SetStaticDoubleField)
  (JNIEnv*, jclass, jfieldID, jdouble);

  jstring
  (JNICALL *NewString)
    (JNIEnv*, const jchar*, jsize);

  jsize
  (JNICALL *GetStringLength)
    (JNIEnv*, jstring);

  const jchar*
  (JNICALL *GetStringChars)
  (JNIEnv*, jstring, jboolean*);

  void
  (JNICALL *ReleaseStringChars)
  (JNIEnv*, jstring, const jchar*);

  jstring
  (JNICALL *NewStringUTF)
    (JNIEnv*, const char*);

  jsize
  (JNICALL *GetStringUTFLength)
    (JNIEnv*, jstring);

  const char*
  (JNICALL *GetStringUTFChars)
  (JNIEnv*, jstring, jboolean*);

  void
  (JNICALL *ReleaseStringUTFChars)
  (JNIEnv*, jstring, const char*);

  jsize
  (JNICALL *GetArrayLength)
    (JNIEnv*, jarray);

  jobjectArray
  (JNICALL *NewObjectArray)
    (JNIEnv*, jsize, jclass, jobject);

  jobject
  (JNICALL *GetObjectArrayElement)
    (JNIEnv*, jobjectArray, jsize);

  void
  (JNICALL *SetObjectArrayElement)
  (JNIEnv*, jobjectArray, jsize, jobject);

  jbooleanArray
  (JNICALL *NewBooleanArray)
    (JNIEnv*, jsize);

  jbyteArray
  (JNICALL *NewByteArray)
    (JNIEnv*, jsize);

  jcharArray
  (JNICALL *NewCharArray)
    (JNIEnv*, jsize);

  jshortArray
  (JNICALL *NewShortArray)
    (JNIEnv*, jsize);

  jintArray
  (JNICALL *NewIntArray)
    (JNIEnv*, jsize);

  jlongArray
  (JNICALL *NewLongArray)
    (JNIEnv*, jsize);

  jfloatArray
  (JNICALL *NewFloatArray)
    (JNIEnv*, jsize);

  jdoubleArray
  (JNICALL *NewDoubleArray)
    (JNIEnv*, jsize);

  jboolean*
  (JNICALL *GetBooleanArrayElements)
  (JNIEnv*, jbooleanArray, jboolean*);

  jbyte*
  (JNICALL *GetByteArrayElements)
  (JNIEnv*, jbyteArray, jboolean*);

  jchar*
  (JNICALL *GetCharArrayElements)
  (JNIEnv*, jcharArray, jboolean*);

  jshort*
  (JNICALL *GetShortArrayElements)
  (JNIEnv*, jshortArray, jboolean*);

  jint*
  (JNICALL *GetIntArrayElements)
  (JNIEnv*, jintArray, jboolean*);

  jlong*
  (JNICALL *GetLongArrayElements)
  (JNIEnv*, jlongArray, jboolean*);

  jfloat*
  (JNICALL *GetFloatArrayElements)
  (JNIEnv*, jfloatArray, jboolean*);

  jdouble*
  (JNICALL *GetDoubleArrayElements)
  (JNIEnv*, jdoubleArray, jboolean*);

  void
  (JNICALL *ReleaseBooleanArrayElements)
  (JNIEnv*, jbooleanArray, jboolean*, jint);

  void
  (JNICALL *ReleaseByteArrayElements)
  (JNIEnv*, jbyteArray, jbyte*, jint);

  void
  (JNICALL *ReleaseCharArrayElements)
  (JNIEnv*, jcharArray, jchar*, jint);

  void
  (JNICALL *ReleaseShortArrayElements)
  (JNIEnv*, jshortArray, jshort*, jint);

  void
  (JNICALL *ReleaseIntArrayElements)
  (JNIEnv*, jintArray, jint*, jint);

  void
  (JNICALL *ReleaseLongArrayElements)
  (JNIEnv*, jlongArray, jlong*, jint);

  void
  (JNICALL *ReleaseFloatArrayElements)
  (JNIEnv*, jfloatArray, jfloat*, jint);

  void
  (JNICALL *ReleaseDoubleArrayElements)
  (JNIEnv*, jdoubleArray, jdouble*, jint);

  void
  (JNICALL *GetBooleanArrayRegion)
  (JNIEnv*, jbooleanArray, jsize, jsize, jboolean*);

  void
  (JNICALL *GetByteArrayRegion)
  (JNIEnv*, jbyteArray, jsize, jsize, jbyte*);

  void
  (JNICALL *GetCharArrayRegion)
  (JNIEnv*, jcharArray, jsize, jsize, jchar*);

  void
  (JNICALL *GetShortArrayRegion)
  (JNIEnv*, jshortArray, jsize, jsize, jshort*);

  void
  (JNICALL *GetIntArrayRegion)
  (JNIEnv*, jintArray, jsize, jsize, jint*);

  void
  (JNICALL *GetLongArrayRegion)
  (JNIEnv*, jlongArray, jsize, jsize, jlong*);

  void
  (JNICALL *GetFloatArrayRegion)
  (JNIEnv*, jfloatArray, jsize, jsize, jfloat*);

  void
  (JNICALL *GetDoubleArrayRegion)
  (JNIEnv*, jdoubleArray, jsize, jsize, jdouble*);

  void
  (JNICALL *SetBooleanArrayRegion)
  (JNIEnv*, jbooleanArray, jsize, jsize, const jboolean*);

  void
  (JNICALL *SetByteArrayRegion)
  (JNIEnv*, jbyteArray, jsize, jsize, const jbyte*);

  void
  (JNICALL *SetCharArrayRegion)
  (JNIEnv*, jcharArray, jsize, jsize, const jchar*);

  void
  (JNICALL *SetShortArrayRegion)
  (JNIEnv*, jshortArray, jsize, jsize, const jshort*);

  void
  (JNICALL *SetIntArrayRegion)
  (JNIEnv*, jintArray, jsize, jsize, const jint*);

  void
  (JNICALL *SetLongArrayRegion)
  (JNIEnv*, jlongArray, jsize, jsize, const jlong*);

  void
  (JNICALL *SetFloatArrayRegion)
  (JNIEnv*, jfloatArray, jsize, jsize, const jfloat*);

  void
  (JNICALL *SetDoubleArrayRegion)
  (JNIEnv*, jdoubleArray, jsize, jsize, const jdouble*);

  jint
  (JNICALL *RegisterNatives)
    (JNIEnv*, jclass, const JNINativeMethod*, jint);

  jint
  (JNICALL *UnregisterNatives)
    (JNIEnv*, jclass);

  jint
  (JNICALL *MonitorEnter)
    (JNIEnv*, jobject);

  jint
  (JNICALL *MonitorExit)
    (JNIEnv*, jobject);

  jint
  (JNICALL *GetJavaVM)
    (JNIEnv*, JavaVM**);

  void
  (JNICALL *GetStringRegion)
  (JNIEnv*, jstring, jsize, jsize, jchar*);

  void
  (JNICALL *GetStringUTFRegion)
  (JNIEnv*, jstring, jsize, jsize, char*);

  void*
  (JNICALL *GetPrimitiveArrayCritical)
  (JNIEnv*, jarray, jboolean*);

  void
  (JNICALL *ReleasePrimitiveArrayCritical)
  (JNIEnv*, jarray, void*, jint);

  const jchar*
  (JNICALL *GetStringCritical)
  (JNIEnv*, jstring, jboolean*);

  void
  (JNICALL *ReleaseStringCritical)
  (JNIEnv*, jstring, const jchar*);

  jweak
  (JNICALL *NewWeakGlobalRef)
  (JNIEnv*, jobject);

  void
  (JNICALL *DeleteWeakGlobalRef)
  (JNIEnv*, jweak);

  jboolean
  (JNICALL *ExceptionCheck)
    (JNIEnv*);

  jobject
  (JNICALL *NewDirectByteBuffer)
    (JNIEnv*, void*, jlong);

  void*
  (JNICALL *GetDirectBufferAddress)
  (JNIEnv* env, jobject);

  jlong
  (JNICALL *GetDirectBufferCapacity)
    (JNIEnv*, jobject);

#if TARGET_RT_MAC_CFM && defined(__ppc__)
  void* real_functions[228];
#endif
};

inline int
strcmp(const int8_t* a, const int8_t* b)
{
  return ::strcmp(reinterpret_cast<const char*>(a),
                  reinterpret_cast<const char*>(b));
}

void
noop();

class Reference {
 public:
  Reference(object target, Reference** handle):
    target(target),
    next(*handle),
    handle(handle),
    count(0)
  {
    if (next) {
      next->handle = &next;
    }
    *handle = this;
  }

  object target;
  Reference* next;
  Reference** handle;
  unsigned count;
};

class Machine {
 public:
  enum Type {
#include "type-enums.cpp"
  };

  enum AllocationType {
    MovableAllocation,
    FixedAllocation,
    ImmortalAllocation
  };

  Machine(System* system, Heap* heap, Finder* finder, Processor* processor,
          const char** properties, unsigned propertyCount);

  ~Machine() { 
    dispose();
  }

  void dispose();

  JavaVMVTable* vtable;
  System* system;
  Heap::Client* heapClient;
  Heap* heap;
  Finder* finder;
  Processor* processor;
  Thread* rootThread;
  Thread* exclusive;
  Thread* finalizeThread;
  Reference* jniReferences;
  const char** properties;
  unsigned propertyCount;
  unsigned activeCount;
  unsigned liveCount;
  unsigned daemonCount;
  unsigned fixedFootprint;
  System::Local* localThread;
  System::Monitor* stateLock;
  System::Monitor* heapLock;
  System::Monitor* classLock;
  System::Monitor* referenceLock;
  System::Monitor* shutdownLock;
  System::Library* libraries;
  object loader;
  object classMap;
  object loadClassMethod;
  object bootstrapClassMap;
  object monitorMap;
  object stringMap;
  object byteArrayMap;
  object types;
  object jniMethodTable;
  object finalizers;
  object tenuredFinalizers;
  object finalizeQueue;
  object weakReferences;
  object tenuredWeakReferences;
  object shutdownHooks;
  object objectsToFinalize;
  object nullPointerException;
  object arrayIndexOutOfBoundsException;
  bool unsafe;
  bool triedBuiltinOnLoad;
  JavaVMVTable javaVMVTable;
  JNIEnvVTable jniEnvVTable;
  uintptr_t* heapPool[ThreadHeapPoolSize];
  unsigned heapPoolIndex;
};

void
printTrace(Thread* t, object exception);

uint8_t&
threadInterrupted(Thread* t, object thread);

void
enterActiveState(Thread* t);

#ifdef VM_STRESS

inline void stress(Thread* t);

#else // not VM_STRESS

#define stress(t)

#endif // not VM_STRESS

void
runJavaThread(Thread* t);

void
runFinalizeThread(Thread* t);

class Thread {
 public:
  enum State {
    NoState,
    ActiveState,
    IdleState,
    ZombieState,
    JoinedState,
    ExclusiveState,
    ExitState
  };

  class Protector {
   public:
    Protector(Thread* t): t(t), next(t->protector) {
      t->protector = this;
    }

    ~Protector() {
      t->protector = next;
    }

    virtual void visit(Heap::Visitor* v) = 0;

    Thread* t;
    Protector* next;
  };

  class SingleProtector: public Protector {
   public:
    SingleProtector(Thread* t, object* p): Protector(t), p(p) { }

    virtual void visit(Heap::Visitor* v) {
      v->visit(p);
    }

    object* p;
  };

  class ClassInitStack {
   public:
    ClassInitStack(Thread* t, object class_):
      next(t->classInitStack),
      class_(class_),
      protector(t, &(this->class_))
    {
      t->classInitStack = this;
    }

    ~ClassInitStack() {
      protector.t->classInitStack = next;
    }

    ClassInitStack* next;
    object class_;
    SingleProtector protector;
  };

  class Runnable: public System::Runnable {
   public:
    Runnable(Thread* t): t(t) { }

    virtual void attach(System::Thread* st) {
      t->systemThread = st;
    }

    virtual void run() {
      enterActiveState(t);

      t->m->localThread->set(t);

      if (t == t->m->finalizeThread) {
        runFinalizeThread(t);
      } else if (t->javaThread) {
        runJavaThread(t);

        if (t->exception) {
          printTrace(t, t->exception);
        }
      }

      t->exit();
    }

    virtual bool interrupted() {
      return threadInterrupted(t, t->javaThread);
    }

    virtual void setInterrupted(bool v) {
      threadInterrupted(t, t->javaThread) = v;
    }

    Thread* t;
  };

  Thread(Machine* m, object javaThread, Thread* parent);

  void init();
  void exit();
  void dispose();
#ifdef AVIAN_THREAD_ALLOCATOR
  void setThreadAllocatorSize(int size);
#endif

  JNIEnvVTable* vtable;
  Machine* m;
  Thread* parent;
  Thread* peer;
  Thread* child;
  Thread* waitNext;
  State state;
  unsigned criticalLevel;
  System::Thread* systemThread;
  System::Monitor* lock;
  object javaThread;
  object exception;
  unsigned heapIndex;
  unsigned heapOffset;
  Protector* protector;
  ClassInitStack* classInitStack;
  Runnable runnable;
  uintptr_t* defaultHeap;
  uintptr_t* heap;
  uintptr_t backupHeap[ThreadBackupHeapSizeInWords];
  unsigned backupHeapIndex;
  bool useBackupHeap;
  bool waiting;
  bool tracing;
#ifdef AVIAN_THREAD_ALLOCATOR
  Heap::ThreadHeap* threadHeap;
#endif
#ifdef VM_STRESS
  bool stress;
#endif // VM_STRESS
};

typedef uint64_t (JNICALL *FastNativeFunction)(Thread*, object, uintptr_t*);

inline object
objectClass(Thread*, object o)
{
  return mask(cast<object>(o, 0));
}

void
enter(Thread* t, Thread::State state);

inline void
enterActiveState(Thread* t)
{
  enter(t, Thread::ActiveState);
}

class StateResource {
 public:
  StateResource(Thread* t, Thread::State state): t(t), oldState(t->state) {
    enter(t, state);
  }

  ~StateResource() { enter(t, oldState); }

 private:
  Thread* t;
  Thread::State oldState;
};

inline void
dispose(Thread* t, Reference* r)
{
  *(r->handle) = r->next;
  if (r->next) {
    r->next->handle = r->handle;
  }
  t->m->heap->free(r, sizeof(*r));
}

inline void
acquire(Thread*, Reference* r)
{
  ++ r->count;
}

inline void
release(Thread* t, Reference* r)
{
  if ((-- r->count) == 0) {
    dispose(t, r);
  }
}

void
collect(Thread* t, Heap::CollectionType type);

void
shutDown(Thread* t);

#ifdef VM_STRESS

inline void
stress(Thread* t)
{
  if ((not t->stress)
      and (not t->tracing)
      and t->state != Thread::NoState
      and t->state != Thread::IdleState)
  {
    t->stress = true;

#  ifdef VM_STRESS_MAJOR
      collect(t, Heap::MajorCollection);
#  else // not VM_STRESS_MAJOR
      collect(t, Heap::MinorCollection);
#  endif // not VM_STRESS_MAJOR

    t->stress = false;
  }
}

#endif // not VM_STRESS

inline void
acquire(Thread* t, System::Monitor* m)
{
  if (not m->tryAcquire(t->systemThread)) {
    ENTER(t, Thread::IdleState);
    m->acquire(t->systemThread);
  }

  stress(t);
}

inline void
release(Thread* t, System::Monitor* m)
{
  m->release(t->systemThread);
}

class MonitorResource {
 public:
  MonitorResource(Thread* t, System::Monitor* m): t(t), m(m) {
    acquire(t, m);
  }

  ~MonitorResource() {
    release(t, m);
  }

 private:
  Thread* t;
  System::Monitor* m;
};

class RawMonitorResource {
 public:
  RawMonitorResource(Thread* t, System::Monitor* m): t(t), m(m) {
    m->acquire(t->systemThread);
  }

  ~RawMonitorResource() {
    release(t, m);
  }

 private:
  Thread* t;
  System::Monitor* m;
};

inline void NO_RETURN
abort(Thread* t)
{
  abort(t->m->system);
}

#ifndef NDEBUG
inline void
assert(Thread* t, bool v)
{
  assert(t->m->system, v);
}
#endif // not NDEBUG

inline void
expect(Thread* t, bool v)
{
  expect(t->m->system, v);
}

class FixedAllocator: public Allocator {
 public:
  FixedAllocator(System* s, uint8_t* base, unsigned capacity):
    s(s), base(base), offset(0), capacity(capacity)
  { }

  virtual void* tryAllocate(unsigned) {
    abort(s);
  }

  virtual void* allocate(unsigned size) {
    unsigned paddedSize = pad(size);
    expect(s, offset + paddedSize < capacity);

    void* p = base + offset;
    offset += paddedSize;
    return p;
  }

  virtual void free(const void*, unsigned) {
    abort(s);
  }

  System* s;
  uint8_t* base;
  unsigned offset;
  unsigned capacity;
};

inline bool
ensure(Thread* t, unsigned sizeInBytes)
{
  if (t->heapIndex + ceiling(sizeInBytes, BytesPerWord)
      > ThreadHeapSizeInWords)
  {
    if (sizeInBytes <= ThreadBackupHeapSizeInBytes) {
      expect(t, not t->useBackupHeap);

      t->useBackupHeap = true;

      return true;
    } else {
      return false;
    }
  } else {
    return true;
  }
}

object
allocate2(Thread* t, unsigned sizeInBytes, bool objectMask);

object
allocate3(Thread* t, Allocator* allocator, Machine::AllocationType type,
          unsigned sizeInBytes, bool objectMask);

inline object
allocateSmall(Thread* t, unsigned sizeInBytes)
{
  assert(t, t->heapIndex + ceiling(sizeInBytes, BytesPerWord)
         <= ThreadHeapSizeInWords);

  object o = reinterpret_cast<object>(t->heap + t->heapIndex);
  t->heapIndex += ceiling(sizeInBytes, BytesPerWord);
  cast<object>(o, 0) = 0;
  return o;
}

inline object
allocate(Thread* t, unsigned sizeInBytes, bool objectMask)
{
  stress(t);
  if (UNLIKELY(t->heapIndex + ceiling(sizeInBytes, BytesPerWord)
               > ThreadHeapSizeInWords
               or t->m->exclusive))
  {
    return allocate2(t, sizeInBytes, objectMask);
  } else {
    return allocateSmall(t, sizeInBytes);
  }
}

inline object
allocateObject(Thread* t, unsigned sizeInBytes, bool objectMask)
{
#ifdef AVIAN_THREAD_ALLOCATOR
  if (t->threadHeap) {
#ifdef AVIAN_THREAD_ALLOCATOR_DEBUG
	  printf("allocate from thread heap %d\n", sizeInBytes);
#endif
	  object o = reinterpret_cast<object> (t->threadHeap->allocate(sizeInBytes));
	  memset(o, 0, sizeInBytes);
	  return o;
  }
#endif
  return allocate(t, sizeInBytes, objectMask);
}

inline void
mark(Thread* t, object o, unsigned offset, unsigned count)
{
  t->m->heap->mark(o, offset / BytesPerWord, count);
}

inline void
mark(Thread* t, object o, unsigned offset)
{
  t->m->heap->mark(o, offset / BytesPerWord, 1);
}

inline void
set(Thread* t, object target, unsigned offset, object value)
{
  cast<object>(target, offset) = value;
  mark(t, target, offset);
}

inline void
setObjectClass(Thread*, object o, object value)
{
  cast<object>(o, 0)
    = reinterpret_cast<object>
    (reinterpret_cast<uintptr_t>(value)
     | (reinterpret_cast<uintptr_t>(cast<object>(o, 0)) & (~PointerMask)));
}

inline Thread*
startThread(Thread* t, object javaThread)
{
  Thread* p = t->m->processor->makeThread(t->m, javaThread, t);

  if (t->m->system->success(t->m->system->start(&(p->runnable)))) {
    return p;
  } else {
    p->exit();
    return 0;
  }
}

inline const char*
findProperty(Machine* m, const char* name)
{
  for (unsigned i = 0; i < m->propertyCount; ++i) {
    const char* p = m->properties[i];
    const char* n = name;
    while (*p and *p != '=' and *n and *p == *n) {
      ++ p;
      ++ n;
    }
    if (*p == '=' and *n == 0) {
      return p + 1;
    }
  }
  return 0;
}

inline const char*
findProperty(Thread* t, const char* name)
{
  return findProperty(t->m, name);
}

object&
arrayBodyUnsafe(Thread*, object, unsigned);

bool
instanceOf(Thread* t, object class_, object o);

#include "type-declarations.cpp"

inline object
getClassLoaderMap(Thread* t, object loader)
{
  if (loader == t->m->loader) {
    return t->m->classMap;
  } else {
    return classLoaderMap(t, loader);
  }
}

inline bool
objectFixed(Thread*, object o)
{
  return (cast<uintptr_t>(o, 0) & (~PointerMask)) == FixedMark;
}

inline bool
objectExtended(Thread*, object o)
{
  return (cast<uintptr_t>(o, 0) & (~PointerMask)) == ExtendedMark;
}

inline bool
hashTaken(Thread*, object o)
{
  return (cast<uintptr_t>(o, 0) & (~PointerMask)) == HashTakenMark;
}

inline unsigned
baseSize(Thread* t, object o, object class_)
{
  return ceiling(classFixedSize(t, class_), BytesPerWord)
    + ceiling(classArrayElementSize(t, class_)
              * cast<uintptr_t>(o, classFixedSize(t, class_) - BytesPerWord),
              BytesPerWord);
}

object
makeTrace(Thread* t, Processor::StackWalker* walker);

object
makeTrace(Thread* t, Thread* target);

inline object
makeTrace(Thread* t)
{
  return makeTrace(t, t);
}

inline object
makeRuntimeException(Thread* t, object message)
{
  PROTECT(t, message);
  object trace = makeTrace(t);
  return makeRuntimeException(t, message, trace, 0);
}

inline object
makeIllegalStateException(Thread* t, object message)
{
  PROTECT(t, message);
  object trace = makeTrace(t);
  return makeIllegalStateException(t, message, trace, 0);
}

inline object
makeIllegalArgumentException(Thread* t)
{
  return makeIllegalArgumentException(t, 0, makeTrace(t), 0);
}

inline object
makeIllegalMonitorStateException(Thread* t)
{
  return makeIllegalMonitorStateException(t, 0, makeTrace(t), 0);
}

inline object
makeIndexOutOfBoundsException(Thread* t)
{
  return makeIndexOutOfBoundsException(t, 0, makeTrace(t), 0);
}

inline object
makeArrayIndexOutOfBoundsException(Thread* t, object message)
{
  PROTECT(t, message);
  object trace = makeTrace(t);
  return makeArrayIndexOutOfBoundsException(t, message, trace, 0);
}

inline object
makeArrayStoreException(Thread* t)
{
  return makeArrayStoreException(t, 0, makeTrace(t), 0);
}

inline object
makeNegativeArraySizeException(Thread* t, object message)
{
  PROTECT(t, message);
  object trace = makeTrace(t);
  return makeNegativeArraySizeException(t, message, trace, 0);
}

inline object
makeClassCastException(Thread* t, object message)
{
  PROTECT(t, message);
  object trace = makeTrace(t);
  return makeClassCastException(t, message, trace, 0);
}

inline object
makeClassNotFoundException(Thread* t, object message)
{
  PROTECT(t, message);
  object trace = makeTrace(t);
  return makeClassNotFoundException(t, message, trace, 0, 0);
}

inline object
makeNullPointerException(Thread* t)
{
  return makeNullPointerException(t, 0, makeTrace(t), 0);
}

inline object
makeInvocationTargetException(Thread* t, object targetException)
{
  PROTECT(t, targetException);
  object trace = makeTrace(t);
  return makeRuntimeException(t, 0, trace, targetException);
}

inline object
makeInterruptedException(Thread* t)
{
  return makeInterruptedException(t, 0, makeTrace(t), 0);
}

inline object
makeIncompatibleContinuationException(Thread* t)
{
  return makeIncompatibleContinuationException(t, 0, makeTrace(t), 0);
}

inline object
makeStackOverflowError(Thread* t)
{
  return makeStackOverflowError(t, 0, makeTrace(t), 0);
}

inline object
makeNoSuchFieldError(Thread* t, object message)
{
  PROTECT(t, message);
  object trace = makeTrace(t);
  return makeNoSuchFieldError(t, message, trace, 0);
}

inline object
makeNoSuchMethodError(Thread* t, object message)
{
  PROTECT(t, message);
  object trace = makeTrace(t);
  return makeNoSuchMethodError(t, message, trace, 0);
}

inline object
makeNoClassDefFoundError(Thread* t, object message)
{
  PROTECT(t, message);
  object trace = makeTrace(t);
  return makeNoClassDefFoundError(t, message, trace, 0);
}

inline object
makeUnsatisfiedLinkError(Thread* t, object message)
{
  PROTECT(t, message);
  object trace = makeTrace(t);
  return makeUnsatisfiedLinkError(t, message, trace, 0);
}

inline object
makeExceptionInInitializerError(Thread* t, object cause)
{
  PROTECT(t, cause);
  object trace = makeTrace(t);
  return makeExceptionInInitializerError(t, 0, trace, cause, cause);
}

inline object
makeIncompatibleClassChangeError(Thread* t)
{
  return makeIncompatibleClassChangeError(t, 0, makeTrace(t), 0);
}

inline object
makeNew(Thread* t, object class_)
{
  assert(t, t->state == Thread::ActiveState);

  PROTECT(t, class_);
  unsigned sizeInBytes = pad(classFixedSize(t, class_));
  assert(t, sizeInBytes);
  object instance = allocate(t, sizeInBytes, classObjectMask(t, class_));
  setObjectClass(t, instance, class_);

  return instance;
}

object
makeNewGeneral(Thread* t, object class_);

inline object
make(Thread* t, object class_)
{
  if (UNLIKELY(classVmFlags(t, class_)
               & (WeakReferenceFlag | HasFinalizerFlag)))
  {
    return makeNewGeneral(t, class_);
  } else {
    return makeNew(t, class_);
  }
}

object
makeByteArray(Thread* t, const char* format, ...);

object
makeString(Thread* t, const char* format, ...);

int
stringUTFLength(Thread* t, object string);

void
stringChars(Thread* t, object string, char* chars);

void
stringChars(Thread* t, object string, uint16_t* chars);

void
stringUTFChars(Thread* t, object string, char* chars, unsigned length);

bool
isAssignableFrom(Thread* t, object a, object b);

object
classInitializer(Thread* t, object class_);

object
frameMethod(Thread* t, int frame);

inline uintptr_t&
extendedWord(Thread* t UNUSED, object o, unsigned baseSize)
{
  assert(t, objectExtended(t, o));
  return cast<uintptr_t>(o, baseSize * BytesPerWord);
}

inline unsigned
extendedSize(Thread* t, object o, unsigned baseSize)
{
  return baseSize + objectExtended(t, o);
}

inline void
markHashTaken(Thread* t, object o)
{
  assert(t, not objectExtended(t, o));
  assert(t, not objectFixed(t, o));

  ACQUIRE_RAW(t, t->m->heapLock);

  cast<uintptr_t>(o, 0) |= HashTakenMark;
  t->m->heap->pad(o);
}

inline uint32_t
takeHash(Thread*, object o)
{
  return reinterpret_cast<uintptr_t>(o) / BytesPerWord;
}

inline uint32_t
objectHash(Thread* t, object o)
{
  if (objectExtended(t, o)) {
    return extendedWord(t, o, baseSize(t, o, objectClass(t, o)));
  } else {
    if (not objectFixed(t, o)) {
      markHashTaken(t, o);
    }
    return takeHash(t, o);
  }
}

inline bool
objectEqual(Thread*, object a, object b)
{
  return a == b;
}

inline uint32_t
byteArrayHash(Thread* t, object array)
{
  return hash(&byteArrayBody(t, array, 0), byteArrayLength(t, array));
}

inline uint32_t
charArrayHash(Thread* t, object array)
{
  return hash(&charArrayBody(t, array, 0), charArrayLength(t, array));
}

inline bool
byteArrayEqual(Thread* t, object a, object b)
{
  return a == b or
    ((byteArrayLength(t, a) == byteArrayLength(t, b)) and
     memcmp(&byteArrayBody(t, a, 0), &byteArrayBody(t, b, 0),
            byteArrayLength(t, a)) == 0);
}

inline uint32_t
stringHash(Thread* t, object s)
{
  if (stringHashCode(t, s) == 0 and stringLength(t, s)) {
    object data = stringData(t, s);
    if (objectClass(t, data)
        == arrayBody(t, t->m->types, Machine::ByteArrayType))
    {
      stringHashCode(t, s) = hash
        (&byteArrayBody(t, data, stringOffset(t, s)), stringLength(t, s));
    } else {
      stringHashCode(t, s) = hash
        (&charArrayBody(t, data, stringOffset(t, s)), stringLength(t, s));
    }
  }
  return stringHashCode(t, s);
}

inline uint16_t
stringCharAt(Thread* t, object s, int i)
{
  object data = stringData(t, s);
  if (objectClass(t, data)
      == arrayBody(t, t->m->types, Machine::ByteArrayType))
  {
    return byteArrayBody(t, data, stringOffset(t, s) + i);
  } else {
    return charArrayBody(t, data, stringOffset(t, s) + i);
  }
}

inline bool
stringEqual(Thread* t, object a, object b)
{
  if (a == b) {
    return true;
  } else if (stringLength(t, a) == stringLength(t, b)) {
    for (unsigned i = 0; i < stringLength(t, a); ++i) {
      if (stringCharAt(t, a, i) != stringCharAt(t, b, i)) {
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

inline uint32_t
methodHash(Thread* t, object method)
{
  return byteArrayHash(t, methodName(t, method))
    ^ byteArrayHash(t, methodSpec(t, method));
}

inline bool
methodEqual(Thread* t, object a, object b)
{
  return a == b or
    (byteArrayEqual(t, methodName(t, a), methodName(t, b)) and
     byteArrayEqual(t, methodSpec(t, a), methodSpec(t, b)));
}

class MethodSpecIterator {
 public:
  MethodSpecIterator(Thread* t, const char* s):
    t(t), s(s + 1)
  { }

  const char* next() {
    assert(t, *s != ')');

    const char* p = s;

    switch (*s) {
    case 'L':
      while (*s and *s != ';') ++ s;
      ++ s;
      break;

    case '[':
      while (*s == '[') ++ s;
      switch (*s) {
      case 'L':
        while (*s and *s != ';') ++ s;
        ++ s;
        break;

      default:
        ++ s;
        break;
      }
      break;
      
    default:
      ++ s;
      break;
    }
    
    return p;
  }

  bool hasNext() {
    return *s != ')';
  }

  const char* returnSpec() {
    assert(t, *s == ')');
    return s + 1;
  }

  Thread* t;
  const char* s;
};

unsigned
fieldCode(Thread* t, unsigned javaCode);

unsigned
fieldType(Thread* t, unsigned code);

unsigned
primitiveSize(Thread* t, unsigned code);

inline unsigned
fieldSize(Thread* t, unsigned code)
{
  if (code == ObjectField) {
    return BytesPerWord;
  } else {
    return primitiveSize(t, code);
  }
}

inline unsigned
fieldSize(Thread* t, object field)
{
  return fieldSize(t, fieldCode(t, field));
}

object
findLoadedSystemClass(Thread* t, object spec);

inline bool
emptyMethod(Thread* t, object method)
{
  return ((methodFlags(t, method) & ACC_NATIVE) == 0)
    and (codeLength(t, methodCode(t, method)) == 1)
    and (codeBody(t, methodCode(t, method), 0) == return_);
}

object
parseClass(Thread* t, object loader, const uint8_t* data, unsigned length);

object
resolveClass(Thread* t, object loader, object name);

inline object
resolveClass(Thread* t, object loader, const char* name)
{
  PROTECT(t, loader);
  object n = makeByteArray(t, "%s", name);
  return resolveClass(t, loader, n);
}

object
resolveSystemClass(Thread* t, object name);

inline object
resolveSystemClass(Thread* t, const char* name)
{
  return resolveSystemClass(t, makeByteArray(t, "%s", name));
}

void
linkClass(Thread* t, object loader, object class_);

object
resolveMethod(Thread* t, object class_, const char* methodName,
              const char* methodSpec);

inline object
resolveMethod(Thread* t, object loader, const char* className,
              const char* methodName, const char* methodSpec)
{
  object class_ = resolveClass(t, loader, className);
  if (LIKELY(t->exception == 0)) {
    return resolveMethod(t, class_, methodName, methodSpec);
  } else {
    return 0;
  }
}

object
resolveField(Thread* t, object class_, const char* fieldName,
             const char* fieldSpec);

inline object
resolveField(Thread* t, object loader, const char* className,
             const char* fieldName, const char* fieldSpec)
{
  object class_ = resolveClass(t, loader, className);
  if (LIKELY(t->exception == 0)) {
    return resolveField(t, class_, fieldName, fieldSpec);
  } else {
    return 0;
  }
}

object
resolveObjectArrayClass(Thread* t, object loader, object elementSpec);

bool
classNeedsInit(Thread* t, object c);

bool
preInitClass(Thread* t, object c);

void
postInitClass(Thread* t, object c);

void
initClass(Thread* t, object c);

object
makeObjectArray(Thread* t, object loader, object elementClass, unsigned count);

inline object
makeObjectArray(Thread* t, unsigned count)
{
  return makeObjectArray
    (t, t->m->loader, arrayBody(t, t->m->types, Machine::JobjectType), count);
}

object
findInTable(Thread* t, object table, object name, object spec,
            object& (*getName)(Thread*, object),
            object& (*getSpec)(Thread*, object));

inline object
findFieldInClass(Thread* t, object class_, object name, object spec)
{
  return findInTable
    (t, classFieldTable(t, class_), name, spec, fieldName, fieldSpec);
}

inline object
findMethodInClass(Thread* t, object class_, object name, object spec)
{
  return findInTable
    (t, classMethodTable(t, class_), name, spec, methodName, methodSpec);
}

object
findInHierarchy(Thread* t, object class_, object name, object spec,
                object (*find)(Thread*, object, object, object),
                object (*makeError)(Thread*, object));

inline object
findMethod(Thread* t, object class_, object name, object spec)
{
  return findInHierarchy
    (t, class_, name, spec, findMethodInClass, makeNoSuchMethodError);
}

inline object
findVirtualMethod(Thread* t, object method, object class_)
{
  return arrayBody(t, classVirtualTable(t, class_), 
                   methodOffset(t, method));
}

inline object
findInterfaceMethod(Thread* t, object method, object class_)
{
  assert(t, (classVmFlags(t, class_) & BootstrapFlag) == 0);

  object interface = methodClass(t, method);
  object itable = classInterfaceTable(t, class_);
  for (unsigned i = 0; i < arrayLength(t, itable); i += 2) {
    if (arrayBody(t, itable, i) == interface) {
      return arrayBody(t, arrayBody(t, itable, i + 1),
                       methodOffset(t, method));
    }
  }
  abort(t);
}

inline unsigned
objectArrayLength(Thread* t UNUSED, object array)
{
  assert(t, classFixedSize(t, objectClass(t, array)) == BytesPerWord * 2);
  assert(t, classArrayElementSize(t, objectClass(t, array)) == BytesPerWord);
  return cast<uintptr_t>(array, BytesPerWord);
}

inline object&
objectArrayBody(Thread* t UNUSED, object array, unsigned index)
{
  assert(t, classFixedSize(t, objectClass(t, array)) == BytesPerWord * 2);
  assert(t, classArrayElementSize(t, objectClass(t, array)) == BytesPerWord);
  assert(t, classObjectMask(t, objectClass(t, array))
         == classObjectMask(t, arrayBody
                            (t, t->m->types, Machine::ArrayType)));
  return cast<object>(array, ArrayBody + (index * BytesPerWord));
}

unsigned
parameterFootprint(Thread* t, const char* s, bool static_);

void
addFinalizer(Thread* t, object target, void (*finalize)(Thread*, object));

inline bool
atomicCompareAndSwapObject(Thread* t, object target, unsigned offset,
                           object old, object new_)
{
  if (atomicCompareAndSwap(&cast<uintptr_t>(target, offset),
                           reinterpret_cast<uintptr_t>(old),
                           reinterpret_cast<uintptr_t>(new_)))
  {
    mark(t, target, offset);
    return true;
  } else {
    return false;
  }
}

// The following two methods (monitorAtomicAppendAcquire and
// monitorAtomicPollAcquire) use the Michael and Scott Non-Blocking
// Queue Algorithm: http://www.cs.rochester.edu/u/michael/PODC96.html

inline void
monitorAtomicAppendAcquire(Thread* t, object monitor)
{
  PROTECT(t, monitor);

  object node = makeMonitorNode(t, t, 0);

  while (true) {
    object tail = monitorAcquireTail(t, monitor);
    
    loadMemoryBarrier();

    object next = monitorNodeNext(t, tail);

    loadMemoryBarrier();

    if (tail == monitorAcquireTail(t, monitor)) {
      if (next) {
        atomicCompareAndSwapObject
          (t, monitor, MonitorAcquireTail, tail, next);
      } else if (atomicCompareAndSwapObject
                 (t, tail, MonitorNodeNext, 0, node))
      {
        atomicCompareAndSwapObject
          (t, monitor, MonitorAcquireTail, tail, node);
        return;
      }
    }
  }
}

inline Thread*
monitorAtomicPollAcquire(Thread* t, object monitor, bool remove)
{
  while (true) {
    object head = monitorAcquireHead(t, monitor);

    loadMemoryBarrier();

    object tail = monitorAcquireTail(t, monitor);

    loadMemoryBarrier();

    object next = monitorNodeNext(t, head);

    loadMemoryBarrier();

    if (head == monitorAcquireHead(t, monitor)) {
      if (head == tail) {
        if (next) {
          atomicCompareAndSwapObject
            (t, monitor, MonitorAcquireTail, tail, next);
        } else {
          return 0;
        }
      } else {
        Thread* value = static_cast<Thread*>(monitorNodeValue(t, next));
        if ((not remove)
            or atomicCompareAndSwapObject
            (t, monitor, MonitorAcquireHead, head, next))
        {
          return value;
        }
      }
    }
  }
}

inline bool
monitorTryAcquire(Thread* t, object monitor)
{
  if (monitorOwner(t, monitor) == t
      or (monitorAtomicPollAcquire(t, monitor, false) == 0
          and atomicCompareAndSwap
          (reinterpret_cast<uintptr_t*>(&monitorOwner(t, monitor)), 0,
           reinterpret_cast<uintptr_t>(t))))
  {
    ++ monitorDepth(t, monitor);
    return true;
  } else {
    return false;
  }
}

inline void
monitorAcquire(Thread* t, object monitor)
{
  if (not monitorTryAcquire(t, monitor)) {
    PROTECT(t, monitor);

    ACQUIRE(t, t->lock);

    monitorAtomicAppendAcquire(t, monitor);
    
    // note that we don't try to acquire the lock until we're first in
    // line, both because it's fair and because we don't support
    // removing elements from arbitrary positions in the queue

    while (not (t == monitorAtomicPollAcquire(t, monitor, false)
                and atomicCompareAndSwap
                (reinterpret_cast<uintptr_t*>(&monitorOwner(t, monitor)), 0,
                 reinterpret_cast<uintptr_t>(t))))
    {
      ENTER(t, Thread::IdleState);
      
      t->lock->wait(t->systemThread, 0);
    }

    expect(t, t == monitorAtomicPollAcquire(t, monitor, true));
        
    ++ monitorDepth(t, monitor);
  }

  assert(t, monitorOwner(t, monitor) == t);
}

inline void
monitorRelease(Thread* t, object monitor)
{
  expect(t, monitorOwner(t, monitor) == t);

  if (-- monitorDepth(t, monitor) == 0) {
    monitorOwner(t, monitor) = 0;

    storeLoadMemoryBarrier();
    
    Thread* next = monitorAtomicPollAcquire(t, monitor, false);

    if (next) {
      ACQUIRE(t, next->lock);
       
      next->lock->notify(t->systemThread);
    }
  }
}

inline void
monitorAppendWait(Thread* t, object monitor)
{
  assert(t, monitorOwner(t, monitor) == t);

  expect(t, not t->waiting);
  expect(t, t->waitNext == 0);

  t->waiting = true;

  if (monitorWaitTail(t, monitor)) {
    static_cast<Thread*>(monitorWaitTail(t, monitor))->waitNext = t;
  } else {
    monitorWaitHead(t, monitor) = t;
  }

  monitorWaitTail(t, monitor) = t;
}

inline void
monitorRemoveWait(Thread* t, object monitor)
{
  assert(t, monitorOwner(t, monitor) == t);

  Thread* previous = 0;
  for (Thread* current = static_cast<Thread*>(monitorWaitHead(t, monitor));
       current; current = current->waitNext)
  {
    if (t == current) {
      if (t == monitorWaitHead(t, monitor)) {
        monitorWaitHead(t, monitor) = t->waitNext;
      } else {
        previous->waitNext = t->waitNext;
      }

      if (t == monitorWaitTail(t, monitor)) {
        assert(t, t->waitNext == 0);
        monitorWaitTail(t, monitor) = previous;
      }

      t->waitNext = 0;
      t->waiting = false;

      return;
    } else {
      previous = current;
    }
  }

  abort(t);
}

inline bool
monitorFindWait(Thread* t, object monitor)
{
  assert(t, monitorOwner(t, monitor) == t);

  for (Thread* current = static_cast<Thread*>(monitorWaitHead(t, monitor));
       current; current = current->waitNext)
  {
    if (t == current) {
      return true;
    }
  }

  return false;
}

inline bool
monitorWait(Thread* t, object monitor, int64_t time)
{
  expect(t, monitorOwner(t, monitor) == t);

  bool interrupted;
  unsigned depth;

  PROTECT(t, monitor);

  { ACQUIRE(t, t->lock);

    monitorAppendWait(t, monitor);

    depth = monitorDepth(t, monitor);
    monitorDepth(t, monitor) = 1;

    monitorRelease(t, monitor);

    ENTER(t, Thread::IdleState);

    interrupted = t->lock->wait(t->systemThread, time);
  }

  monitorAcquire(t, monitor);

  monitorDepth(t, monitor) = depth;

  if (t->waiting) {
    monitorRemoveWait(t, monitor);
  } else {
    expect(t, not monitorFindWait(t, monitor));
  }

  assert(t, monitorOwner(t, monitor) == t);

  return interrupted;
}

inline Thread*
monitorPollWait(Thread* t, object monitor)
{
  assert(t, monitorOwner(t, monitor) == t);

  Thread* next = static_cast<Thread*>(monitorWaitHead(t, monitor));

  if (next) {
    monitorWaitHead(t, monitor) = next->waitNext;
    next->waiting = false;
    next->waitNext = 0;
    if (next == monitorWaitTail(t, monitor)) {
      monitorWaitTail(t, monitor) = 0;
    }
  } else {
    assert(t, monitorWaitTail(t, monitor) == 0);
  }

  return next;
}

inline bool
monitorNotify(Thread* t, object monitor)
{
  expect(t, monitorOwner(t, monitor) == t);
  
  Thread* next = monitorPollWait(t, monitor);

  if (next) {
    ACQUIRE(t, next->lock);

    next->lock->notify(t->systemThread);

    return true;
  } else {
    return false;
  }
}

inline void
monitorNotifyAll(Thread* t, object monitor)
{
  PROTECT(t, monitor);

  while (monitorNotify(t, monitor)) { }
}

object
objectMonitor(Thread* t, object o, bool createNew);

inline void
acquire(Thread* t, object o)
{
  unsigned hash;
  if (DebugMonitors) {
    hash = objectHash(t, o);
  }

  object m = objectMonitor(t, o, true);

  if (DebugMonitors) {
    fprintf(stderr, "thread %p acquires %p for %x\n",
            t, m, hash);
  }

  monitorAcquire(t, m);
}

inline void
release(Thread* t, object o)
{
  unsigned hash;
  if (DebugMonitors) {
    hash = objectHash(t, o);
  }

  object m = objectMonitor(t, o, false);

  if (DebugMonitors) {
    fprintf(stderr, "thread %p releases %p for %x\n",
            t, m, hash);
  }

  monitorRelease(t, m);
}

inline void
wait(Thread* t, object o, int64_t milliseconds)
{
  unsigned hash;
  if (DebugMonitors) {
    hash = objectHash(t, o);
  }

  object m = objectMonitor(t, o, false);

  if (DebugMonitors) {
    fprintf(stderr, "thread %p waits %d millis on %p for %x\n",
            t, static_cast<int>(milliseconds), m, hash);
  }

  if (m and monitorOwner(t, m) == t) {
    PROTECT(t, m);

    bool interrupted = monitorWait(t, m, milliseconds);

    if (interrupted) {
      t->exception = makeInterruptedException(t);
    }
  } else {
    t->exception = makeIllegalMonitorStateException(t);
  }

  if (DebugMonitors) {
    fprintf(stderr, "thread %p wakes up on %p for %x\n",
            t, m, hash);
  }

  stress(t);
}

inline void
notify(Thread* t, object o)
{
  unsigned hash;
  if (DebugMonitors) {
    hash = objectHash(t, o);
  }

  object m = objectMonitor(t, o, false);

  if (DebugMonitors) {
    fprintf(stderr, "thread %p notifies on %p for %x\n",
            t, m, hash);
  }

  if (m and monitorOwner(t, m) == t) {
    monitorNotify(t, m);
  } else {
    t->exception = makeIllegalMonitorStateException(t);
  }
}

inline void
notifyAll(Thread* t, object o)
{
  object m = objectMonitor(t, o, false);

  if (DebugMonitors) {
    fprintf(stderr, "thread %p notifies all on %p for %x\n",
            t, m, objectHash(t, o));
  }

  if (m and monitorOwner(t, m) == t) {
    monitorNotifyAll(t, m);
  } else {
    t->exception = makeIllegalMonitorStateException(t);
  }
}

inline void
interrupt(Thread*, Thread* target)
{
  target->systemThread->interrupt();
}

inline void
setDaemon(Thread* t, object thread, bool daemon)
{
  ACQUIRE_RAW(t, t->m->stateLock);

  if ((threadDaemon(t, thread) != 0) != daemon) {
    threadDaemon(t, thread) = daemon;

    if (daemon) {
      ++ t->m->daemonCount;
    } else {
      expect(t, t->m->daemonCount);
      -- t->m->daemonCount;
    }
    
    t->m->stateLock->notifyAll(t->systemThread);
  }
}

object
intern(Thread* t, object s);

void
walk(Thread* t, Heap::Walker* w, object o, unsigned start);

int
walkNext(Thread* t, object o, int previous);

void
visitRoots(Machine* m, Heap::Visitor* v);

inline jobject
makeLocalReference(Thread* t, object o)
{
  return t->m->processor->makeLocalReference(t, o);
}

inline void
disposeLocalReference(Thread* t, jobject r)
{
  t->m->processor->disposeLocalReference(t, r);
}

inline bool
methodVirtual(Thread* t, object method)
{
  return (methodFlags(t, method) & (ACC_STATIC | ACC_PRIVATE)) == 0
    and byteArrayBody(t, methodName(t, method), 0) != '<';
}

inline unsigned
singletonMaskSize(unsigned count)
{
  if (count) {
    return ceiling(count + 2, BitsPerWord);
  }
  return 0;
}

inline unsigned
singletonMaskSize(Thread* t, object singleton)
{
  unsigned length = singletonLength(t, singleton);
  if (length) {
    return ceiling(length + 2, BitsPerWord + 1);
  }
  return 0;
}

inline unsigned
singletonCount(Thread* t, object singleton)
{
  return singletonLength(t, singleton) - singletonMaskSize(t, singleton);
}

inline uint32_t*
singletonMask(Thread* t, object singleton)
{
  assert(t, singletonLength(t, singleton));
  return reinterpret_cast<uint32_t*>
    (&singletonBody(t, singleton, singletonCount(t, singleton)));
}

inline void
singletonMarkObject(Thread* t, object singleton, unsigned index)
{
  singletonMask(t, singleton)[(index + 2) / 32]
    |= (static_cast<uint32_t>(1) << ((index + 2) % 32));
}

inline bool
singletonIsObject(Thread* t, object singleton, unsigned index)
{
  assert(t, index < singletonCount(t, singleton));

  return (singletonMask(t, singleton)[(index + 2) / 32]
          & (static_cast<uint32_t>(1) << ((index + 2) % 32))) != 0;
}

inline object&
singletonObject(Thread* t, object singleton, unsigned index)
{
  assert(t, singletonIsObject(t, singleton, index));
  return reinterpret_cast<object&>(singletonBody(t, singleton, index));
}

inline uintptr_t&
singletonValue(Thread* t, object singleton, unsigned index)
{
  assert(t, not singletonIsObject(t, singleton, index));
  return singletonBody(t, singleton, index);
}

inline object
makeSingletonOfSize(Thread* t, unsigned count)
{
  object o = makeSingleton(t, count + singletonMaskSize(count));
  assert(t, singletonLength(t, o) == count + singletonMaskSize(t, o));
  if (count) {
    singletonMask(t, o)[0] = 1;
  }
  return o;
}

inline void
singletonSetBit(Thread* t, object singleton, unsigned start, unsigned index)
{
  singletonValue(t, singleton, start + (index / BitsPerWord))
    |= static_cast<uintptr_t>(1) << (index % BitsPerWord);
}

inline bool
singletonBit(Thread* t, object singleton, unsigned start, unsigned index)
{
  return (singletonValue(t, singleton, start + (index / BitsPerWord))
          & (static_cast<uintptr_t>(1) << (index % BitsPerWord))) != 0;
}

inline unsigned
poolMaskSize(unsigned count)
{
  return ceiling(count, BitsPerWord);
}

inline unsigned
poolMaskSize(Thread* t, object pool)
{
  return ceiling(singletonCount(t, pool), BitsPerWord + 1);
}

inline unsigned
poolSize(Thread* t, object pool)
{
  return singletonCount(t, pool) - poolMaskSize(t, pool);
}

inline object
resolveClassInObject(Thread* t, object loader, object container,
                     unsigned classOffset)
{
  object o = cast<object>(container, classOffset);
  if (objectClass(t, o) == arrayBody(t, t->m->types, Machine::ByteArrayType)) {
    PROTECT(t, container);

    o = resolveClass(t, loader, o);
    if (UNLIKELY(t->exception)) return 0;
    
    set(t, container, classOffset, o);
  }
  return o; 
}

inline object
resolveClassInPool(Thread* t, object loader, object method, unsigned index)
{
  object o = singletonObject(t, codePool(t, methodCode(t, method)), index);
  if (objectClass(t, o) == arrayBody(t, t->m->types, Machine::ReferenceType)) {
    PROTECT(t, method);

    o = resolveClass(t, loader, referenceName(t, o));
    if (UNLIKELY(t->exception)) return 0;
    
    set(t, codePool(t, methodCode(t, method)),
        SingletonBody + (index * BytesPerWord), o);
  }
  return o; 
}

inline object
resolveClassInPool(Thread* t, object method, unsigned index)
{
  return resolveClassInPool(t, classLoader(t, methodClass(t, method)),
                            method, index);
}

inline object
resolve(Thread* t, object loader, object method, unsigned index,
        object (*find)(vm::Thread*, object, object, object),
        object (*makeError)(vm::Thread*, object))
{
  object o = singletonObject(t, codePool(t, methodCode(t, method)), index);
  if (objectClass(t, o) == arrayBody(t, t->m->types, Machine::ReferenceType))
  {
    PROTECT(t, method);

    object reference = o;
    PROTECT(t, reference);

    object class_ = resolveClassInObject(t, loader, o, ReferenceClass);
    if (UNLIKELY(t->exception)) return 0;
    
    o = findInHierarchy
      (t, class_, referenceName(t, reference), referenceSpec(t, reference),
       find, makeError);
    if (UNLIKELY(t->exception)) return 0;
    
    set(t, codePool(t, methodCode(t, method)),
        SingletonBody + (index * BytesPerWord), o);
  }

  return o;
}

inline object
resolveField(Thread* t, object loader, object method, unsigned index)
{
  return resolve(t, loader, method, index, findFieldInClass,
                 makeNoSuchFieldError);
}

inline object
resolveField(Thread* t, object method, unsigned index)
{
  return resolveField
    (t, classLoader(t, methodClass(t, method)), method, index);
}

inline object
resolveMethod(Thread* t, object loader, object method, unsigned index)
{
  return resolve(t, loader, method, index, findMethodInClass,
                 makeNoSuchMethodError);
}

inline object
resolveMethod(Thread* t, object method, unsigned index)
{
  return resolveMethod
    (t, classLoader(t, methodClass(t, method)), method, index);
}

void
dumpHeap(Thread* t, FILE* out);

} // namespace vm

void
vmPrintTrace(vm::Thread* t);

void*
vmAddressFromLine(vm::Thread* t, vm::object m, unsigned line);

#endif//MACHINE_H
