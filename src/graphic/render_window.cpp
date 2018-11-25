#include <GL/gl.h>
#include "log.h"
#include "core.h"
#include "graphic/render_system.h"

namespace dk
{

ret_code render_window::create()
{
	m_display = XOpenDisplay(nullptr);
	if (m_display == nullptr) {
		DK_LOG_ERROR("Failed to open display connection");
		return ret_code::ERROR;
	}

	m_egl_display = eglGetDisplay(m_display);
	if (m_egl_display == nullptr) {
		DK_LOG_ERROR("Failed to get EGL display");
		return ret_code::ERROR;
	}

	EGLint egl_major_ver;
	EGLint egl_minor_ver;
	if (eglInitialize(m_egl_display, &egl_major_ver, &egl_minor_ver) != EGL_TRUE) {
		DK_LOG_ERROR("Failed to initialize EGL");
		return ret_code::ERROR;
	}

	DK_LOG_OK("EGL v", egl_major_ver, '.', egl_minor_ver, " initialized");
	if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE) {
		DK_LOG_ERROR("EGL failed to bind OpenGL API");
		return ret_code::ERROR;
	}

	static EGLint const config_attrib_list[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		//EGL_BUFFER_SIZE,     32,
		//EGL_RED_SIZE,        8,
		//EGL_GREEN_SIZE,      8,
		//EGL_BLUE_SIZE,       8,
		//EGL_ALPHA_SIZE,      8,
		//EGL_SAMPLES,         8,
		EGL_NONE
	};

	EGLint configs_count;
	if (eglChooseConfig(m_egl_display, config_attrib_list, &m_config, 1, &configs_count) != EGL_TRUE) {
		DK_LOG_ERROR("Failed to choose EGL config");
		return ret_code::ERROR;
	}

	if (configs_count == 0) {
		DK_LOG_ERROR("No matching EGL configs found");
		return ret_code::ERROR;
	}

	EGLint vi_id;
	if (eglGetConfigAttrib(m_egl_display, m_config, EGL_NATIVE_VISUAL_ID, &vi_id) != EGL_TRUE) {
		DK_LOG_ERROR("Failed to get visual info ID from EGL");
		return ret_code::ERROR;
	}

	int vi_count;
	XVisualInfo vi_tmpl { .visualid = (VisualID)vi_id, .screen = DefaultScreen(m_display) };
	XVisualInfo* vi = XGetVisualInfo(m_display, VisualIDMask | VisualScreenMask, &vi_tmpl, &vi_count);
	if (vi == nullptr || vi_count == 0) {
		DK_LOG_ERROR("Failed to get visual info");
		return ret_code::ERROR;
	}

	m_window = XCreateWindow(m_display, RootWindow(m_display, vi->screen),
		0, 0, 800, 600, 0, vi->depth, InputOutput, vi->visual, 0, nullptr);
	XFree(vi);

	if (m_window == 0) {
		DK_LOG_ERROR("Failed to create window");
		return ret_code::ERROR;
	}

	DK_LOG_OK("Window created");
	XStoreName(m_display, m_window, "tst | v0.0.1");
	XMapWindow(m_display, m_window);

	m_surface = eglCreateWindowSurface(m_egl_display, m_config, m_window, nullptr);
	if (m_surface == nullptr) {
		DK_LOG_ERROR("Failed to create window surface");
		return ret_code::ERROR;
	}

	static EGLint const context_attrib_list[] = {
		EGL_CONTEXT_MAJOR_VERSION, 3,
		EGL_CONTEXT_MINOR_VERSION, 3,
		EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
		EGL_NONE
	};

	m_context = eglCreateContext(m_egl_display, m_config, EGL_NO_CONTEXT, context_attrib_list);
	if (m_context == EGL_NO_CONTEXT) {
		DK_LOG_ERROR("Failed to create OpenGL context");
		return ret_code::ERROR;
	}

	eglMakeCurrent(m_egl_display, m_surface, m_surface, m_context);

	auto glsl_ver = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	DK_LOG_OK("OpenGL context created");
	DK_LOG("OpenGL vendor: ", glGetString(GL_VENDOR));
	DK_LOG("OpenGL renderer: ", glGetString(GL_RENDERER));
	DK_LOG("OpenGL version: ", glGetString(GL_VERSION));
	DK_LOG("GLSL version: ", (glsl_ver != nullptr ? glsl_ver : "(none)"));

	auto render_sys = core::get_render_sys();
	if (render_sys->init() != ret_code::OK)
		return ret_code::ERROR;

	return ret_code::OK;
}

}
