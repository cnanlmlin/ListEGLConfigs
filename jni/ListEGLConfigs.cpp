/**
 * \file ListEGLConfigs.cpp
 * \brief A sample which prints out the list of available
 * EGL/GL configurations on the current platform.
 */

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdlib>
#include <cstdio>

#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    __FILE__

#define  LOGI(format, args...) { fprintf(stderr, format, ##args); __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##args); }
#define  LOGE(format, args...) { fprintf(stderr, format, ##args); __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##args); }
#define  LOGD(format, args...) { fprintf(stderr, format, ##args); __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, format, ##args); }

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
     if ((v < (const char*) 0) || (v > (const char*) 0x10000)){
    	 LOGI("GL %s = %s\n", name, v);
     }
     else{
    	 LOGI("GL %s = (null) 0x%08x\n", name, (unsigned int) v);
     }
}

static void checkEglError(const char* op, EGLBoolean returnVal = EGL_TRUE) {
    if (returnVal != EGL_TRUE) {
        LOGE("%s() returned %d\n", op, returnVal);
    }
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error= glGetError()) {
        LOGE("after %s() glError (0x%x)\n", op, error);
    }
}

const char* decodeCaveat( EGLint value)
{
    switch(value)
    {
        case EGL_NONE:
            return "Normal";
            break;
        case EGL_SLOW_CONFIG:
            return "Slow";
            break;
        case EGL_NON_CONFORMANT_CONFIG:
            return "Non-conformant";
            break;
        default:
            return "Unknown EGL_CONFIG_CAVEAT";
            break;
    }
}

const char* decodeSurfaceStrings[] = {
    "None!",
    "PBuffer",
    "Pixmap",
    "PBuffer+Pixmap",
    "Window",
    "Window+PBuffer",
    "Window+Pixmap",
    "Window+Pixmap+PBuffer"
};

const char* decodeSurface(EGLint value)
{
    /* TODO consider more than just bits 0-2 */
    return decodeSurfaceStrings[value&7];
}

const char* decodeColorBuffer(EGLint value)
{
    switch(value)
    {
        case EGL_RGB_BUFFER:
            return "EGL_RGB_BUFFER";
            break;
        case EGL_LUMINANCE_BUFFER:
            return "EGL_LUMINANCE_BUFFER";
            break;
        default:
            return "Unknown EGL_COLOR_BUFFER_TYPE";
            break;
    }
}

void printEGLConfiguration(EGLDisplay dpy, EGLConfig config) {

#define X(VAL) {VAL, #VAL}
    struct {EGLint attribute; const char* name;} names[] = {
    X(EGL_BUFFER_SIZE),
    X(EGL_ALPHA_SIZE),
    X(EGL_BLUE_SIZE),
    X(EGL_GREEN_SIZE),
    X(EGL_RED_SIZE),
    X(EGL_DEPTH_SIZE),
    X(EGL_STENCIL_SIZE),
    X(EGL_CONFIG_CAVEAT),
    X(EGL_CONFIG_ID),
    X(EGL_LEVEL),
    X(EGL_MAX_PBUFFER_HEIGHT),
    X(EGL_MAX_PBUFFER_PIXELS),
    X(EGL_MAX_PBUFFER_WIDTH),
    X(EGL_NATIVE_RENDERABLE),
    X(EGL_NATIVE_VISUAL_ID),
    X(EGL_NATIVE_VISUAL_TYPE),
    X(EGL_SAMPLES),
    X(EGL_SAMPLE_BUFFERS),
    X(EGL_SURFACE_TYPE),
    X(EGL_TRANSPARENT_TYPE),
    X(EGL_TRANSPARENT_RED_VALUE),
    X(EGL_TRANSPARENT_GREEN_VALUE),
    X(EGL_TRANSPARENT_BLUE_VALUE),
    X(EGL_BIND_TO_TEXTURE_RGB),
    X(EGL_BIND_TO_TEXTURE_RGBA),
    X(EGL_MIN_SWAP_INTERVAL),
    X(EGL_MAX_SWAP_INTERVAL),
    X(EGL_LUMINANCE_SIZE),
    X(EGL_ALPHA_MASK_SIZE),
    X(EGL_COLOR_BUFFER_TYPE),
    X(EGL_RENDERABLE_TYPE),
    X(EGL_CONFORMANT),
   };
#undef X

    for (size_t j = 0; j < sizeof(names) / sizeof(names[0]); j++) {
        EGLint value = -1;
        EGLint returnVal = eglGetConfigAttrib(dpy, config, names[j].attribute, &value);
        EGLint error = eglGetError();
        if (returnVal && error == EGL_SUCCESS) {
            if(0 == strcmp(names[j].name, "EGL_CONFIG_CAVEAT")){
            	LOGI("\t%-32s: %10s (0x%08x)\n", names[j].name, decodeCaveat(value), value);
            }

            if(0 == strcmp(names[j].name, "EGL_SURFACE_TYPE")){
            	LOGI("\t%-32s: %10s (0x%08x)\n", names[j].name, decodeSurface(value), value);
            }

            if(0 == strcmp(names[j].name, "EGL_COLOR_BUFFER_TYPE")){
            	LOGI("\t%-32s: %10s (0x%08x)\n", names[j].name, decodeColorBuffer(value), value);
            }
            LOGI("\t%-32s: %10d (0x%08x)\n", names[j].name, value, value);
        }
    }
    LOGI("\n");
}

int printEGLConfigurations(EGLDisplay dpy, EGLint numConfigs) {
    LOGI("Number of EGL configuration: %d\n", numConfigs);

    EGLConfig* configs = (EGLConfig*) malloc(sizeof(EGLConfig) * numConfigs);
    if (! configs) {
        LOGD("Could not allocate configs.\n");
        return false;
    }

    EGLint returnVal = eglGetConfigs(dpy, configs, numConfigs, &numConfigs);
    checkEglError("eglGetConfigs", returnVal);
    if (!returnVal) {
        free(configs);
        return false;
    }

    for(int i = 0; i < numConfigs; i++) {
    	LOGI("Configuration %d\n", i);
        printEGLConfiguration(dpy, configs[i]);
    }

    free(configs);
    return true;
}

static EGLDisplay mEglDisplay;
static EGLSurface mEglSurface;
static EGLContext mEglContext;
static EGLint mEglWidth = 0;
static EGLint mEglHeight = 0;
EGLint majorVersion;
EGLint minorVersion;

int setupGLEnv() {
    EGLBoolean returnValue;
    //EGLConfig myConfig = {0};

    EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLint s_configAttribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE };

    EGLint attribs[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE
    };

    EGLDisplay dpy;
    EGLSurface surface;
    EGLContext context;
    EGLint w = 0;
    EGLint h = 0;
    EGLint numConfigs;
    EGLConfig config;

    checkEglError("<init>");
    dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    checkEglError("eglGetDisplay");
    if (dpy == EGL_NO_DISPLAY) {
        LOGD("eglGetDisplay returned EGL_NO_DISPLAY.\n");
        return 0;
    }

    returnValue = eglInitialize(dpy, &majorVersion, &minorVersion);
    checkEglError("eglInitialize", returnValue);
    fprintf(stderr, "EGL version %d.%d\n", majorVersion, minorVersion);
    if (returnValue != EGL_TRUE) {
        LOGD("eglInitialize failed\n");
        return 0;
    }


    if (!eglGetConfigs(dpy, NULL, 0, &numConfigs)) {
         LOGD("eglGetConfig fail\n");
         return 0;
    }

    if (!printEGLConfigurations(dpy, numConfigs)) {
        LOGD("printEGLConfigurations failed\n");
        return 0;
    }
    checkEglError("printEGLConfigurations");


    if (!eglChooseConfig(dpy, s_configAttribs, &config, 1, &numConfigs)) {
    	LOGD("eglChooseConfig fail\n");
        return 0;
    }

    //create PbufferSerface in native layer
    surface = eglCreatePbufferSurface(dpy, config, attribs);
    if (surface == EGL_NO_SURFACE) {
    	LOGD("eglCreatePbufferSurface error %d\n", eglGetError());
        return false;
    }

    context = eglCreateContext(dpy, config, NULL, context_attribs);
    checkEglError("eglCreateContext");
    if (context == EGL_NO_CONTEXT) {
    	LOGD("eglCreateContext failed\n");
        return 0;
    }
    returnValue = eglMakeCurrent(dpy, surface, surface, context);
    checkEglError("eglMakeCurrent", returnValue);
    if (returnValue != EGL_TRUE) {
        return 0;
    }
    eglQuerySurface(dpy, surface, EGL_WIDTH, &w);
    checkEglError("eglQuerySurface");
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &h);
    checkEglError("eglQuerySurface");

    mEglDisplay = dpy;
    mEglSurface = surface;
    mEglContext = context;
    mEglWidth = w;
    mEglHeight = h;
    return 1;
}

void releaseGLEnv() {
    if (EGL_NO_CONTEXT != mEglContext) {
        eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(mEglDisplay, mEglSurface);
        eglDestroyContext(mEglDisplay, mEglContext);
        eglTerminate(mEglDisplay);

        mEglSurface = EGL_NO_SURFACE;
        mEglContext = EGL_NO_CONTEXT;
        mEglDisplay = EGL_NO_DISPLAY;
    }
}

bool listConfigs(void){
	setupGLEnv();
	GLint dim = mEglWidth < mEglHeight ? mEglWidth : mEglHeight;
	LOGI("\n");
	LOGI("Window dimensions: %d x %d\n", mEglWidth, mEglHeight);


	LOGI("Egl Version is:  %d.%d\n", majorVersion, minorVersion);
	LOGI("EGL vendor string:  %s\n", eglQueryString(mEglDisplay, EGL_VENDOR));
	LOGI("EGL version string:  %s\n", eglQueryString(mEglDisplay, EGL_VERSION));
	LOGI("EGL client APIs:  %s\n", eglQueryString(mEglDisplay, EGL_CLIENT_APIS));
	LOGI("EGL extensions string:\n");
	LOGI("    %s\n", eglQueryString(mEglDisplay, EGL_EXTENSIONS));

	LOGI("\n");
	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);

	releaseGLEnv();
	return true;
}

extern "C"
{
    JNIEXPORT void JNICALL Java_android_egl_listeglconfigs_ListEGLConfigs_init
    (JNIEnv *env, jclass jcls, jint width, jint height)
    {
        listConfigs();
    }

    JNIEXPORT void JNICALL Java_android_egl_listeglconfigs_ListEGLConfigs_step
    (JNIEnv *env, jclass jcls)
    {
    }

    JNIEXPORT void JNICALL Java_android_egl_listeglconfigs_ListEGLConfigs_uninit
    (JNIEnv *, jclass)
    {
    }
}
