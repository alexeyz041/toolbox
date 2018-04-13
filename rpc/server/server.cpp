
#include "buffer.h"
#include <GLES2/gl2.h>
#include <EGL/egl.h>


void glActiveTexture_stub(Buffer &args, Buffer &res)
{
GLenum texture;
	EXT(args,texture);
	glActiveTexture(texture);
}


void glAttachShader_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLuint shader;
	EXT(args,program);
	EXT(args,shader);
	glAttachShader(program,shader);
}


void glBindAttribLocation_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLuint index;
GLchar *name;
	EXT(args,program);
	EXT(args,index);
	EXTSTR(args,name);
	glBindAttribLocation(program,index,name);
}


void glBindBuffer_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLuint buffer;
	EXT(args,target);
	EXT(args,buffer);
	glBindBuffer(target,buffer);
}


void glBindFramebuffer_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLuint framebuffer;
	EXT(args,target);
	EXT(args,framebuffer);
	glBindFramebuffer(target,framebuffer);
}


void glBindRenderbuffer_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLuint renderbuffer;
	EXT(args,target);
	EXT(args,renderbuffer);
	glBindRenderbuffer(target,renderbuffer);
}


void glBindTexture_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLuint texture;
	EXT(args,target);
	EXT(args,texture);
	glBindTexture(target,texture);
}


void glBlendColor_stub(Buffer &args, Buffer &res)
{
GLfloat red;
GLfloat green;
GLfloat blue;
GLfloat alpha;
	EXT(args,red);
	EXT(args,green);
	EXT(args,blue);
	EXT(args,alpha);
	glBlendColor(red,green,blue,alpha);
}


void glBlendEquation_stub(Buffer &args, Buffer &res)
{
GLenum mode;
	EXT(args,mode);
	glBlendEquation(mode);
}


void glBlendEquationSeparate_stub(Buffer &args, Buffer &res)
{
GLenum modeRGB;
GLenum modeAlpha;
	EXT(args,modeRGB);
	EXT(args,modeAlpha);
	glBlendEquationSeparate(modeRGB,modeAlpha);
}


void glBlendFunc_stub(Buffer &args, Buffer &res)
{
GLenum sfactor;
GLenum dfactor;
	EXT(args,sfactor);
	EXT(args,dfactor);
	glBlendFunc(sfactor,dfactor);
}


void glBlendFuncSeparate_stub(Buffer &args, Buffer &res)
{
GLenum sfactorRGB;
GLenum dfactorRGB;
GLenum sfactorAlpha;
GLenum dfactorAlpha;
	EXT(args,sfactorRGB);
	EXT(args,dfactorRGB);
	EXT(args,sfactorAlpha);
	EXT(args,dfactorAlpha);
	glBlendFuncSeparate(sfactorRGB,dfactorRGB,sfactorAlpha,dfactorAlpha);
}


void glBufferData_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLsizeiptr size;
void *data;
GLenum usage;
	EXT(args,target);
	EXT(args,size);
	EXT(args,*data);
	EXT(args,usage);
	glBufferData(target,size,*data,usage);
}


void glBufferSubData_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLintptr offset;
GLsizeiptr size;
void *data;
	EXT(args,target);
	EXT(args,offset);
	EXT(args,size);
	EXT(args,*data);
	glBufferSubData(target,offset,size,*data);
}


void glCheckFramebufferStatus_stub(Buffer &args, Buffer &res)
{
GLenum target;
	EXT(args,target);
GLenum result = glCheckFramebufferStatus(target);
	ADD(res,result);
}


void glClear_stub(Buffer &args, Buffer &res)
{
GLbitfield mask;
	EXT(args,mask);
	glClear(mask);
}


void glClearColor_stub(Buffer &args, Buffer &res)
{
GLfloat red;
GLfloat green;
GLfloat blue;
GLfloat alpha;
	EXT(args,red);
	EXT(args,green);
	EXT(args,blue);
	EXT(args,alpha);
	glClearColor(red,green,blue,alpha);
}


void glClearDepthf_stub(Buffer &args, Buffer &res)
{
GLfloat d;
	EXT(args,d);
	glClearDepthf(d);
}


void glClearStencil_stub(Buffer &args, Buffer &res)
{
GLint s;
	EXT(args,s);
	glClearStencil(s);
}


void glColorMask_stub(Buffer &args, Buffer &res)
{
GLboolean red;
GLboolean green;
GLboolean blue;
GLboolean alpha;
	EXT(args,red);
	EXT(args,green);
	EXT(args,blue);
	EXT(args,alpha);
	glColorMask(red,green,blue,alpha);
}


void glCompileShader_stub(Buffer &args, Buffer &res)
{
GLuint shader;
	EXT(args,shader);
	glCompileShader(shader);
}


void glCompressedTexImage2D_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLint level;
GLenum internalformat;
GLsizei width;
GLsizei height;
GLint border;
GLsizei imageSize;
void *data;
	EXT(args,target);
	EXT(args,level);
	EXT(args,internalformat);
	EXT(args,width);
	EXT(args,height);
	EXT(args,border);
	EXT(args,imageSize);
	EXT(args,*data);
	glCompressedTexImage2D(target,level,internalformat,width,height,border,imageSize,*data);
}


void glCompressedTexSubImage2D_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLint level;
GLint xoffset;
GLint yoffset;
GLsizei width;
GLsizei height;
GLenum format;
GLsizei imageSize;
void *data;
	EXT(args,target);
	EXT(args,level);
	EXT(args,xoffset);
	EXT(args,yoffset);
	EXT(args,width);
	EXT(args,height);
	EXT(args,format);
	EXT(args,imageSize);
	EXT(args,*data);
	glCompressedTexSubImage2D(target,level,xoffset,yoffset,width,height,format,imageSize,*data);
}


void glCopyTexImage2D_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLint level;
GLenum internalformat;
GLint x;
GLint y;
GLsizei width;
GLsizei height;
GLint border;
	EXT(args,target);
	EXT(args,level);
	EXT(args,internalformat);
	EXT(args,x);
	EXT(args,y);
	EXT(args,width);
	EXT(args,height);
	EXT(args,border);
	glCopyTexImage2D(target,level,internalformat,x,y,width,height,border);
}


void glCopyTexSubImage2D_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLint level;
GLint xoffset;
GLint yoffset;
GLint x;
GLint y;
GLsizei width;
GLsizei height;
	EXT(args,target);
	EXT(args,level);
	EXT(args,xoffset);
	EXT(args,yoffset);
	EXT(args,x);
	EXT(args,y);
	EXT(args,width);
	EXT(args,height);
	glCopyTexSubImage2D(target,level,xoffset,yoffset,x,y,width,height);
}


void glCreateProgram_stub(Buffer &args, Buffer &res)
{
GLuint result = glCreateProgram();
	ADD(res,result);
}


void glCreateShader_stub(Buffer &args, Buffer &res)
{
GLenum type;
	EXT(args,type);
GLuint result = glCreateShader(type);
	ADD(res,result);
}


void glCullFace_stub(Buffer &args, Buffer &res)
{
GLenum mode;
	EXT(args,mode);
	glCullFace(mode);
}


void glDeleteBuffers_stub(Buffer &args, Buffer &res)
{
GLsizei n;
GLuint *buffers;
	EXT(args,n);
	EXT(args,*buffers);
	glDeleteBuffers(n,*buffers);
}


void glDeleteFramebuffers_stub(Buffer &args, Buffer &res)
{
GLsizei n;
GLuint *framebuffers;
	EXT(args,n);
	EXT(args,*framebuffers);
	glDeleteFramebuffers(n,*framebuffers);
}


void glDeleteProgram_stub(Buffer &args, Buffer &res)
{
GLuint program;
	EXT(args,program);
	glDeleteProgram(program);
}


void glDeleteRenderbuffers_stub(Buffer &args, Buffer &res)
{
GLsizei n;
GLuint *renderbuffers;
	EXT(args,n);
	EXT(args,*renderbuffers);
	glDeleteRenderbuffers(n,*renderbuffers);
}


void glDeleteShader_stub(Buffer &args, Buffer &res)
{
GLuint shader;
	EXT(args,shader);
	glDeleteShader(shader);
}


void glDeleteTextures_stub(Buffer &args, Buffer &res)
{
GLsizei n;
GLuint *textures;
	EXT(args,n);
	EXT(args,*textures);
	glDeleteTextures(n,*textures);
}


void glDepthFunc_stub(Buffer &args, Buffer &res)
{
GLenum func;
	EXT(args,func);
	glDepthFunc(func);
}


void glDepthMask_stub(Buffer &args, Buffer &res)
{
GLboolean flag;
	EXT(args,flag);
	glDepthMask(flag);
}


void glDepthRangef_stub(Buffer &args, Buffer &res)
{
GLfloat n;
GLfloat f;
	EXT(args,n);
	EXT(args,f);
	glDepthRangef(n,f);
}


void glDetachShader_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLuint shader;
	EXT(args,program);
	EXT(args,shader);
	glDetachShader(program,shader);
}


void glDisable_stub(Buffer &args, Buffer &res)
{
GLenum cap;
	EXT(args,cap);
	glDisable(cap);
}


void glDisableVertexAttribArray_stub(Buffer &args, Buffer &res)
{
GLuint index;
	EXT(args,index);
	glDisableVertexAttribArray(index);
}


void glDrawArrays_stub(Buffer &args, Buffer &res)
{
GLenum mode;
GLint first;
GLsizei count;
	EXT(args,mode);
	EXT(args,first);
	EXT(args,count);
	glDrawArrays(mode,first,count);
}


void glDrawElements_stub(Buffer &args, Buffer &res)
{
GLenum mode;
GLsizei count;
GLenum type;
void *indices;
	EXT(args,mode);
	EXT(args,count);
	EXT(args,type);
	EXT(args,*indices);
	glDrawElements(mode,count,type,*indices);
}


void glEnable_stub(Buffer &args, Buffer &res)
{
GLenum cap;
	EXT(args,cap);
	glEnable(cap);
}


void glEnableVertexAttribArray_stub(Buffer &args, Buffer &res)
{
GLuint index;
	EXT(args,index);
	glEnableVertexAttribArray(index);
}


void glFinish_stub(Buffer &args, Buffer &res)
{
	glFinish();
}


void glFlush_stub(Buffer &args, Buffer &res)
{
	glFlush();
}


void glFramebufferRenderbuffer_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum attachment;
GLenum renderbuffertarget;
GLuint renderbuffer;
	EXT(args,target);
	EXT(args,attachment);
	EXT(args,renderbuffertarget);
	EXT(args,renderbuffer);
	glFramebufferRenderbuffer(target,attachment,renderbuffertarget,renderbuffer);
}


void glFramebufferTexture2D_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum attachment;
GLenum textarget;
GLuint texture;
GLint level;
	EXT(args,target);
	EXT(args,attachment);
	EXT(args,textarget);
	EXT(args,texture);
	EXT(args,level);
	glFramebufferTexture2D(target,attachment,textarget,texture,level);
}


void glFrontFace_stub(Buffer &args, Buffer &res)
{
GLenum mode;
	EXT(args,mode);
	glFrontFace(mode);
}


void glGenBuffers_stub(Buffer &args, Buffer &res)
{
GLsizei n;
GLuint *buffers;
	EXT(args,n);
	EXT(args,*buffers);
	glGenBuffers(n,*buffers);
}


void glGenerateMipmap_stub(Buffer &args, Buffer &res)
{
GLenum target;
	EXT(args,target);
	glGenerateMipmap(target);
}


void glGenFramebuffers_stub(Buffer &args, Buffer &res)
{
GLsizei n;
GLuint *framebuffers;
	EXT(args,n);
	EXT(args,*framebuffers);
	glGenFramebuffers(n,*framebuffers);
}


void glGenRenderbuffers_stub(Buffer &args, Buffer &res)
{
GLsizei n;
GLuint *renderbuffers;
	EXT(args,n);
	EXT(args,*renderbuffers);
	glGenRenderbuffers(n,*renderbuffers);
}


void glGenTextures_stub(Buffer &args, Buffer &res)
{
GLsizei n;
GLuint *textures;
	EXT(args,n);
	EXT(args,*textures);
	glGenTextures(n,*textures);
}


void glGetActiveAttrib_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLuint index;
GLsizei bufSize;
GLsizei *length;
GLint *size;
GLenum *type;
GLchar *name;
	EXT(args,program);
	EXT(args,index);
	EXT(args,bufSize);
	EXT(args,*length);
	EXT(args,*size);
	EXT(args,*type);
	EXT(args,*name);
	glGetActiveAttrib(program,index,bufSize,*length,*size,*type,*name);
}


void glGetActiveUniform_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLuint index;
GLsizei bufSize;
GLsizei *length;
GLint *size;
GLenum *type;
GLchar *name;
	EXT(args,program);
	EXT(args,index);
	EXT(args,bufSize);
	EXT(args,*length);
	EXT(args,*size);
	EXT(args,*type);
	EXT(args,*name);
	glGetActiveUniform(program,index,bufSize,*length,*size,*type,*name);
}


void glGetAttachedShaders_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLsizei maxCount;
GLsizei *count;
GLuint *shaders;
	EXT(args,program);
	EXT(args,maxCount);
	EXT(args,*count);
	EXT(args,*shaders);
	glGetAttachedShaders(program,maxCount,*count,*shaders);
}


void glGetAttribLocation_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLchar *name;
	EXT(args,program);
	EXT(args,*name);
GLint result = glGetAttribLocation(program,*name);
	ADD(res,result);
}


void glGetBooleanv_stub(Buffer &args, Buffer &res)
{
GLenum pname;
GLboolean *data;
	EXT(args,pname);
	EXT(args,*data);
	glGetBooleanv(pname,*data);
}


void glGetBufferParameteriv_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum pname;
GLint *params;
	EXT(args,target);
	EXT(args,pname);
	EXT(args,*params);
	glGetBufferParameteriv(target,pname,*params);
}


void glGetError_stub(Buffer &args, Buffer &res)
{
GLenum result = glGetError();
	ADD(res,result);
}


void glGetFloatv_stub(Buffer &args, Buffer &res)
{
GLenum pname;
GLfloat *data;
	EXT(args,pname);
	EXT(args,*data);
	glGetFloatv(pname,*data);
}


void glGetFramebufferAttachmentParameteriv_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum attachment;
GLenum pname;
GLint *params;
	EXT(args,target);
	EXT(args,attachment);
	EXT(args,pname);
	EXT(args,*params);
	glGetFramebufferAttachmentParameteriv(target,attachment,pname,*params);
}


void glGetIntegerv_stub(Buffer &args, Buffer &res)
{
GLenum pname;
GLint *data;
	EXT(args,pname);
	EXT(args,*data);
	glGetIntegerv(pname,*data);
}


void glGetProgramiv_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLenum pname;
GLint *params;
	EXT(args,program);
	EXT(args,pname);
	EXT(args,*params);
	glGetProgramiv(program,pname,*params);
}


void glGetProgramInfoLog_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLsizei bufSize;
GLsizei *length;
GLchar *infoLog;
	EXT(args,program);
	EXT(args,bufSize);
	EXT(args,*length);
	EXT(args,*infoLog);
	glGetProgramInfoLog(program,bufSize,*length,*infoLog);
}


void glGetRenderbufferParameteriv_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum pname;
GLint *params;
	EXT(args,target);
	EXT(args,pname);
	EXT(args,*params);
	glGetRenderbufferParameteriv(target,pname,*params);
}


void glGetShaderiv_stub(Buffer &args, Buffer &res)
{
GLuint shader;
GLenum pname;
GLint *params;
	EXT(args,shader);
	EXT(args,pname);
	EXT(args,*params);
	glGetShaderiv(shader,pname,*params);
}


void glGetShaderInfoLog_stub(Buffer &args, Buffer &res)
{
GLuint shader;
GLsizei bufSize;
GLsizei *length;
GLchar *infoLog;
	EXT(args,shader);
	EXT(args,bufSize);
	EXT(args,*length);
	EXT(args,*infoLog);
	glGetShaderInfoLog(shader,bufSize,*length,*infoLog);
}


void glGetShaderPrecisionFormat_stub(Buffer &args, Buffer &res)
{
GLenum shadertype;
GLenum precisiontype;
GLint *range;
GLint *precision;
	EXT(args,shadertype);
	EXT(args,precisiontype);
	EXT(args,*range);
	EXT(args,*precision);
	glGetShaderPrecisionFormat(shadertype,precisiontype,*range,*precision);
}


void glGetShaderSource_stub(Buffer &args, Buffer &res)
{
GLuint shader;
GLsizei bufSize;
GLsizei *length;
GLchar *source;
	EXT(args,shader);
	EXT(args,bufSize);
	EXT(args,*length);
	EXT(args,*source);
	glGetShaderSource(shader,bufSize,*length,*source);
}


void *glGetString_stub(Buffer &args, Buffer &res)
{
GLenum name;
	EXT(args,name);
GLubyte result = *glGetString(name);
	ADD(res,result);
}


void glGetTexParameterfv_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum pname;
GLfloat *params;
	EXT(args,target);
	EXT(args,pname);
	EXT(args,*params);
	glGetTexParameterfv(target,pname,*params);
}


void glGetTexParameteriv_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum pname;
GLint *params;
	EXT(args,target);
	EXT(args,pname);
	EXT(args,*params);
	glGetTexParameteriv(target,pname,*params);
}


void glGetUniformfv_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLint location;
GLfloat *params;
	EXT(args,program);
	EXT(args,location);
	EXT(args,*params);
	glGetUniformfv(program,location,*params);
}


void glGetUniformiv_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLint location;
GLint *params;
	EXT(args,program);
	EXT(args,location);
	EXT(args,*params);
	glGetUniformiv(program,location,*params);
}


void glGetUniformLocation_stub(Buffer &args, Buffer &res)
{
GLuint program;
GLchar *name;
	EXT(args,program);
	EXT(args,*name);
GLint result = glGetUniformLocation(program,*name);
	ADD(res,result);
}


void glGetVertexAttribfv_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLenum pname;
GLfloat *params;
	EXT(args,index);
	EXT(args,pname);
	EXT(args,*params);
	glGetVertexAttribfv(index,pname,*params);
}


void glGetVertexAttribiv_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLenum pname;
GLint *params;
	EXT(args,index);
	EXT(args,pname);
	EXT(args,*params);
	glGetVertexAttribiv(index,pname,*params);
}


void glGetVertexAttribPointerv_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLenum pname;
void **pointer;
	EXT(args,index);
	EXT(args,pname);
	EXT(args,**pointer);
	glGetVertexAttribPointerv(index,pname,**pointer);
}


void glHint_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum mode;
	EXT(args,target);
	EXT(args,mode);
	glHint(target,mode);
}


void glIsBuffer_stub(Buffer &args, Buffer &res)
{
GLuint buffer;
	EXT(args,buffer);
GLboolean result = glIsBuffer(buffer);
	ADD(res,result);
}


void glIsEnabled_stub(Buffer &args, Buffer &res)
{
GLenum cap;
	EXT(args,cap);
GLboolean result = glIsEnabled(cap);
	ADD(res,result);
}


void glIsFramebuffer_stub(Buffer &args, Buffer &res)
{
GLuint framebuffer;
	EXT(args,framebuffer);
GLboolean result = glIsFramebuffer(framebuffer);
	ADD(res,result);
}


void glIsProgram_stub(Buffer &args, Buffer &res)
{
GLuint program;
	EXT(args,program);
GLboolean result = glIsProgram(program);
	ADD(res,result);
}


void glIsRenderbuffer_stub(Buffer &args, Buffer &res)
{
GLuint renderbuffer;
	EXT(args,renderbuffer);
GLboolean result = glIsRenderbuffer(renderbuffer);
	ADD(res,result);
}


void glIsShader_stub(Buffer &args, Buffer &res)
{
GLuint shader;
	EXT(args,shader);
GLboolean result = glIsShader(shader);
	ADD(res,result);
}


void glIsTexture_stub(Buffer &args, Buffer &res)
{
GLuint texture;
	EXT(args,texture);
GLboolean result = glIsTexture(texture);
	ADD(res,result);
}


void glLineWidth_stub(Buffer &args, Buffer &res)
{
GLfloat width;
	EXT(args,width);
	glLineWidth(width);
}


void glLinkProgram_stub(Buffer &args, Buffer &res)
{
GLuint program;
	EXT(args,program);
	glLinkProgram(program);
}


void glPixelStorei_stub(Buffer &args, Buffer &res)
{
GLenum pname;
GLint param;
	EXT(args,pname);
	EXT(args,param);
	glPixelStorei(pname,param);
}


void glPolygonOffset_stub(Buffer &args, Buffer &res)
{
GLfloat factor;
GLfloat units;
	EXT(args,factor);
	EXT(args,units);
	glPolygonOffset(factor,units);
}


void glReadPixels_stub(Buffer &args, Buffer &res)
{
GLint x;
GLint y;
GLsizei width;
GLsizei height;
GLenum format;
GLenum type;
void *pixels;
	EXT(args,x);
	EXT(args,y);
	EXT(args,width);
	EXT(args,height);
	EXT(args,format);
	EXT(args,type);
	EXT(args,*pixels);
	glReadPixels(x,y,width,height,format,type,*pixels);
}


void glReleaseShaderCompiler_stub(Buffer &args, Buffer &res)
{
	glReleaseShaderCompiler();
}


void glRenderbufferStorage_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum internalformat;
GLsizei width;
GLsizei height;
	EXT(args,target);
	EXT(args,internalformat);
	EXT(args,width);
	EXT(args,height);
	glRenderbufferStorage(target,internalformat,width,height);
}


void glSampleCoverage_stub(Buffer &args, Buffer &res)
{
GLfloat value;
GLboolean invert;
	EXT(args,value);
	EXT(args,invert);
	glSampleCoverage(value,invert);
}


void glScissor_stub(Buffer &args, Buffer &res)
{
GLint x;
GLint y;
GLsizei width;
GLsizei height;
	EXT(args,x);
	EXT(args,y);
	EXT(args,width);
	EXT(args,height);
	glScissor(x,y,width,height);
}


void glShaderBinary_stub(Buffer &args, Buffer &res)
{
GLsizei count;
GLuint *shaders;
GLenum binaryformat;
void *binary;
GLsizei length;
	EXT(args,count);
	EXT(args,*shaders);
	EXT(args,binaryformat);
	EXT(args,*binary);
	EXT(args,length);
	glShaderBinary(count,*shaders,binaryformat,*binary,length);
}


void glShaderSource_stub(Buffer &args, Buffer &res)
{
GLuint shader;
GLsizei count;
GLchar *const*string;
GLint *length;
	EXT(args,shader);
	EXT(args,count);
	EXT(args,*const*string);
	EXT(args,*length);
	glShaderSource(shader,count,*const*string,*length);
}


void glStencilFunc_stub(Buffer &args, Buffer &res)
{
GLenum func;
GLint ref;
GLuint mask;
	EXT(args,func);
	EXT(args,ref);
	EXT(args,mask);
	glStencilFunc(func,ref,mask);
}


void glStencilFuncSeparate_stub(Buffer &args, Buffer &res)
{
GLenum face;
GLenum func;
GLint ref;
GLuint mask;
	EXT(args,face);
	EXT(args,func);
	EXT(args,ref);
	EXT(args,mask);
	glStencilFuncSeparate(face,func,ref,mask);
}


void glStencilMask_stub(Buffer &args, Buffer &res)
{
GLuint mask;
	EXT(args,mask);
	glStencilMask(mask);
}


void glStencilMaskSeparate_stub(Buffer &args, Buffer &res)
{
GLenum face;
GLuint mask;
	EXT(args,face);
	EXT(args,mask);
	glStencilMaskSeparate(face,mask);
}


void glStencilOp_stub(Buffer &args, Buffer &res)
{
GLenum fail;
GLenum zfail;
GLenum zpass;
	EXT(args,fail);
	EXT(args,zfail);
	EXT(args,zpass);
	glStencilOp(fail,zfail,zpass);
}


void glStencilOpSeparate_stub(Buffer &args, Buffer &res)
{
GLenum face;
GLenum sfail;
GLenum dpfail;
GLenum dppass;
	EXT(args,face);
	EXT(args,sfail);
	EXT(args,dpfail);
	EXT(args,dppass);
	glStencilOpSeparate(face,sfail,dpfail,dppass);
}


void glTexImage2D_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLint level;
GLint internalformat;
GLsizei width;
GLsizei height;
GLint border;
GLenum format;
GLenum type;
void *pixels;
	EXT(args,target);
	EXT(args,level);
	EXT(args,internalformat);
	EXT(args,width);
	EXT(args,height);
	EXT(args,border);
	EXT(args,format);
	EXT(args,type);
	EXT(args,*pixels);
	glTexImage2D(target,level,internalformat,width,height,border,format,type,*pixels);
}


void glTexParameterf_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum pname;
GLfloat param;
	EXT(args,target);
	EXT(args,pname);
	EXT(args,param);
	glTexParameterf(target,pname,param);
}


void glTexParameterfv_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum pname;
GLfloat *params;
	EXT(args,target);
	EXT(args,pname);
	EXT(args,*params);
	glTexParameterfv(target,pname,*params);
}


void glTexParameteri_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum pname;
GLint param;
	EXT(args,target);
	EXT(args,pname);
	EXT(args,param);
	glTexParameteri(target,pname,param);
}


void glTexParameteriv_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLenum pname;
GLint *params;
	EXT(args,target);
	EXT(args,pname);
	EXT(args,*params);
	glTexParameteriv(target,pname,*params);
}


void glTexSubImage2D_stub(Buffer &args, Buffer &res)
{
GLenum target;
GLint level;
GLint xoffset;
GLint yoffset;
GLsizei width;
GLsizei height;
GLenum format;
GLenum type;
void *pixels;
	EXT(args,target);
	EXT(args,level);
	EXT(args,xoffset);
	EXT(args,yoffset);
	EXT(args,width);
	EXT(args,height);
	EXT(args,format);
	EXT(args,type);
	EXT(args,*pixels);
	glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,*pixels);
}


void glUniform1f_stub(Buffer &args, Buffer &res)
{
GLint location;
GLfloat v0;
	EXT(args,location);
	EXT(args,v0);
	glUniform1f(location,v0);
}


void glUniform1fv_stub(Buffer &args, Buffer &res)
{
GLint location;
GLsizei count;
GLfloat *value;
	EXT(args,location);
	EXT(args,count);
	EXT(args,*value);
	glUniform1fv(location,count,*value);
}


void glUniform1i_stub(Buffer &args, Buffer &res)
{
GLint location;
GLint v0;
	EXT(args,location);
	EXT(args,v0);
	glUniform1i(location,v0);
}


void glUniform1iv_stub(Buffer &args, Buffer &res)
{
GLint location;
GLsizei count;
GLint *value;
	EXT(args,location);
	EXT(args,count);
	EXT(args,*value);
	glUniform1iv(location,count,*value);
}


void glUniform2f_stub(Buffer &args, Buffer &res)
{
GLint location;
GLfloat v0;
GLfloat v1;
	EXT(args,location);
	EXT(args,v0);
	EXT(args,v1);
	glUniform2f(location,v0,v1);
}


void glUniform2fv_stub(Buffer &args, Buffer &res)
{
GLint location;
GLsizei count;
GLfloat *value;
	EXT(args,location);
	EXT(args,count);
	EXT(args,*value);
	glUniform2fv(location,count,*value);
}


void glUniform2i_stub(Buffer &args, Buffer &res)
{
GLint location;
GLint v0;
GLint v1;
	EXT(args,location);
	EXT(args,v0);
	EXT(args,v1);
	glUniform2i(location,v0,v1);
}


void glUniform2iv_stub(Buffer &args, Buffer &res)
{
GLint location;
GLsizei count;
GLint *value;
	EXT(args,location);
	EXT(args,count);
	EXT(args,*value);
	glUniform2iv(location,count,*value);
}


void glUniform3f_stub(Buffer &args, Buffer &res)
{
GLint location;
GLfloat v0;
GLfloat v1;
GLfloat v2;
	EXT(args,location);
	EXT(args,v0);
	EXT(args,v1);
	EXT(args,v2);
	glUniform3f(location,v0,v1,v2);
}


void glUniform3fv_stub(Buffer &args, Buffer &res)
{
GLint location;
GLsizei count;
GLfloat *value;
	EXT(args,location);
	EXT(args,count);
	EXT(args,*value);
	glUniform3fv(location,count,*value);
}


void glUniform3i_stub(Buffer &args, Buffer &res)
{
GLint location;
GLint v0;
GLint v1;
GLint v2;
	EXT(args,location);
	EXT(args,v0);
	EXT(args,v1);
	EXT(args,v2);
	glUniform3i(location,v0,v1,v2);
}


void glUniform3iv_stub(Buffer &args, Buffer &res)
{
GLint location;
GLsizei count;
GLint *value;
	EXT(args,location);
	EXT(args,count);
	EXT(args,*value);
	glUniform3iv(location,count,*value);
}


void glUniform4f_stub(Buffer &args, Buffer &res)
{
GLint location;
GLfloat v0;
GLfloat v1;
GLfloat v2;
GLfloat v3;
	EXT(args,location);
	EXT(args,v0);
	EXT(args,v1);
	EXT(args,v2);
	EXT(args,v3);
	glUniform4f(location,v0,v1,v2,v3);
}


void glUniform4fv_stub(Buffer &args, Buffer &res)
{
GLint location;
GLsizei count;
GLfloat *value;
	EXT(args,location);
	EXT(args,count);
	EXT(args,*value);
	glUniform4fv(location,count,*value);
}


void glUniform4i_stub(Buffer &args, Buffer &res)
{
GLint location;
GLint v0;
GLint v1;
GLint v2;
GLint v3;
	EXT(args,location);
	EXT(args,v0);
	EXT(args,v1);
	EXT(args,v2);
	EXT(args,v3);
	glUniform4i(location,v0,v1,v2,v3);
}


void glUniform4iv_stub(Buffer &args, Buffer &res)
{
GLint location;
GLsizei count;
GLint *value;
	EXT(args,location);
	EXT(args,count);
	EXT(args,*value);
	glUniform4iv(location,count,*value);
}


void glUniformMatrix2fv_stub(Buffer &args, Buffer &res)
{
GLint location;
GLsizei count;
GLboolean transpose;
GLfloat *value;
	EXT(args,location);
	EXT(args,count);
	EXT(args,transpose);
	EXT(args,*value);
	glUniformMatrix2fv(location,count,transpose,*value);
}


void glUniformMatrix3fv_stub(Buffer &args, Buffer &res)
{
GLint location;
GLsizei count;
GLboolean transpose;
GLfloat *value;
	EXT(args,location);
	EXT(args,count);
	EXT(args,transpose);
	EXT(args,*value);
	glUniformMatrix3fv(location,count,transpose,*value);
}


void glUniformMatrix4fv_stub(Buffer &args, Buffer &res)
{
GLint location;
GLsizei count;
GLboolean transpose;
GLfloat *value;
	EXT(args,location);
	EXT(args,count);
	EXT(args,transpose);
	EXT(args,*value);
	glUniformMatrix4fv(location,count,transpose,*value);
}


void glUseProgram_stub(Buffer &args, Buffer &res)
{
GLuint program;
	EXT(args,program);
	glUseProgram(program);
}


void glValidateProgram_stub(Buffer &args, Buffer &res)
{
GLuint program;
	EXT(args,program);
	glValidateProgram(program);
}


void glVertexAttrib1f_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLfloat x;
	EXT(args,index);
	EXT(args,x);
	glVertexAttrib1f(index,x);
}


void glVertexAttrib1fv_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLfloat *v;
	EXT(args,index);
	EXT(args,*v);
	glVertexAttrib1fv(index,*v);
}


void glVertexAttrib2f_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLfloat x;
GLfloat y;
	EXT(args,index);
	EXT(args,x);
	EXT(args,y);
	glVertexAttrib2f(index,x,y);
}


void glVertexAttrib2fv_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLfloat *v;
	EXT(args,index);
	EXT(args,*v);
	glVertexAttrib2fv(index,*v);
}


void glVertexAttrib3f_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLfloat x;
GLfloat y;
GLfloat z;
	EXT(args,index);
	EXT(args,x);
	EXT(args,y);
	EXT(args,z);
	glVertexAttrib3f(index,x,y,z);
}


void glVertexAttrib3fv_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLfloat *v;
	EXT(args,index);
	EXT(args,*v);
	glVertexAttrib3fv(index,*v);
}


void glVertexAttrib4f_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLfloat x;
GLfloat y;
GLfloat z;
GLfloat w;
	EXT(args,index);
	EXT(args,x);
	EXT(args,y);
	EXT(args,z);
	EXT(args,w);
	glVertexAttrib4f(index,x,y,z,w);
}


void glVertexAttrib4fv_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLfloat *v;
	EXT(args,index);
	EXT(args,*v);
	glVertexAttrib4fv(index,*v);
}


void glVertexAttribPointer_stub(Buffer &args, Buffer &res)
{
GLuint index;
GLint size;
GLenum type;
GLboolean normalized;
GLsizei stride;
void *pointer;
	EXT(args,index);
	EXT(args,size);
	EXT(args,type);
	EXT(args,normalized);
	EXT(args,stride);
	EXT(args,*pointer);
	glVertexAttribPointer(index,size,type,normalized,stride,*pointer);
}


void glViewport_stub(Buffer &args, Buffer &res)
{
GLint x;
GLint y;
GLsizei width;
GLsizei height;
	EXT(args,x);
	EXT(args,y);
	EXT(args,width);
	EXT(args,height);
	glViewport(x,y,width,height);
}


void eglChooseConfig_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLint *attrib_list;
EGLConfig *configs;
EGLint config_size;
EGLint *num_config;
	EXT(args,dpy);
	EXT(args,*attrib_list);
	EXT(args,*configs);
	EXT(args,config_size);
	EXT(args,*num_config);
EGLBoolean result = eglChooseConfig(dpy,*attrib_list,*configs,config_size,*num_config);
	ADD(res,result);
}


void eglCopyBuffers_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSurface surface;
EGLNativePixmapType target;
	EXT(args,dpy);
	EXT(args,surface);
	EXT(args,target);
EGLBoolean result = eglCopyBuffers(dpy,surface,target);
	ADD(res,result);
}


void eglCreateContext_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLConfig config;
EGLContext share_context;
EGLint *attrib_list;
	EXT(args,dpy);
	EXT(args,config);
	EXT(args,share_context);
	EXT(args,*attrib_list);
EGLContext result = eglCreateContext(dpy,config,share_context,*attrib_list);
	ADD(res,result);
}


void eglCreatePbufferSurface_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLConfig config;
EGLint *attrib_list;
	EXT(args,dpy);
	EXT(args,config);
	EXT(args,*attrib_list);
EGLSurface result = eglCreatePbufferSurface(dpy,config,*attrib_list);
	ADD(res,result);
}


void eglCreatePixmapSurface_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLConfig config;
EGLNativePixmapType pixmap;
EGLint *attrib_list;
	EXT(args,dpy);
	EXT(args,config);
	EXT(args,pixmap);
	EXT(args,*attrib_list);
EGLSurface result = eglCreatePixmapSurface(dpy,config,pixmap,*attrib_list);
	ADD(res,result);
}


void eglCreateWindowSurface_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLConfig config;
EGLNativeWindowType win;
EGLint *attrib_list;
	EXT(args,dpy);
	EXT(args,config);
	EXT(args,win);
	EXT(args,*attrib_list);
EGLSurface result = eglCreateWindowSurface(dpy,config,win,*attrib_list);
	ADD(res,result);
}


void eglDestroyContext_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLContext ctx;
	EXT(args,dpy);
	EXT(args,ctx);
EGLBoolean result = eglDestroyContext(dpy,ctx);
	ADD(res,result);
}


void eglDestroySurface_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSurface surface;
	EXT(args,dpy);
	EXT(args,surface);
EGLBoolean result = eglDestroySurface(dpy,surface);
	ADD(res,result);
}


void eglGetConfigAttrib_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLConfig config;
EGLint attribute;
EGLint *value;
	EXT(args,dpy);
	EXT(args,config);
	EXT(args,attribute);
	EXT(args,*value);
EGLBoolean result = eglGetConfigAttrib(dpy,config,attribute,*value);
	ADD(res,result);
}


void eglGetConfigs_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLConfig *configs;
EGLint config_size;
EGLint *num_config;
	EXT(args,dpy);
	EXT(args,*configs);
	EXT(args,config_size);
	EXT(args,*num_config);
EGLBoolean result = eglGetConfigs(dpy,*configs,config_size,*num_config);
	ADD(res,result);
}


void eglGetCurrentDisplay_stub(Buffer &args, Buffer &res)
{
EGLDisplay result = eglGetCurrentDisplay();
	ADD(res,result);
}


void eglGetCurrentSurface_stub(Buffer &args, Buffer &res)
{
EGLint readdraw;
	EXT(args,readdraw);
EGLSurface result = eglGetCurrentSurface(readdraw);
	ADD(res,result);
}


void eglGetDisplay_stub(Buffer &args, Buffer &res)
{
EGLNativeDisplayType display_id;
	EXT(args,display_id);
EGLDisplay result = eglGetDisplay(display_id);
	ADD(res,result);
}


void eglGetError_stub(Buffer &args, Buffer &res)
{
EGLint result = eglGetError();
	ADD(res,result);
}


void *eglGetProcAddress_stub(Buffer &args, Buffer &res)
{
char *procname;
	EXT(args,*procname);
	*eglGetProcAddress(*procname);
}


void eglInitialize_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLint *major;
EGLint *minor;
	EXT(args,dpy);
	EXT(args,*major);
	EXT(args,*minor);
EGLBoolean result = eglInitialize(dpy,*major,*minor);
	ADD(res,result);
}


void eglMakeCurrent_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSurface draw;
EGLSurface read;
EGLContext ctx;
	EXT(args,dpy);
	EXT(args,draw);
	EXT(args,read);
	EXT(args,ctx);
EGLBoolean result = eglMakeCurrent(dpy,draw,read,ctx);
	ADD(res,result);
}


void eglQueryContext_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLContext ctx;
EGLint attribute;
EGLint *value;
	EXT(args,dpy);
	EXT(args,ctx);
	EXT(args,attribute);
	EXT(args,*value);
EGLBoolean result = eglQueryContext(dpy,ctx,attribute,*value);
	ADD(res,result);
}


void *eglQueryString_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLint name;
	EXT(args,dpy);
	EXT(args,name);
char result = *eglQueryString(dpy,name);
	ADD(res,result);
}


void eglQuerySurface_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSurface surface;
EGLint attribute;
EGLint *value;
	EXT(args,dpy);
	EXT(args,surface);
	EXT(args,attribute);
	EXT(args,*value);
EGLBoolean result = eglQuerySurface(dpy,surface,attribute,*value);
	ADD(res,result);
}


void eglSwapBuffers_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSurface surface;
	EXT(args,dpy);
	EXT(args,surface);
EGLBoolean result = eglSwapBuffers(dpy,surface);
	ADD(res,result);
}


void eglTerminate_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
	EXT(args,dpy);
EGLBoolean result = eglTerminate(dpy);
	ADD(res,result);
}


void eglWaitGL_stub(Buffer &args, Buffer &res)
{
EGLBoolean result = eglWaitGL();
	ADD(res,result);
}


void eglWaitNative_stub(Buffer &args, Buffer &res)
{
EGLint engine;
	EXT(args,engine);
EGLBoolean result = eglWaitNative(engine);
	ADD(res,result);
}


void eglBindTexImage_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSurface surface;
EGLint buffer;
	EXT(args,dpy);
	EXT(args,surface);
	EXT(args,buffer);
EGLBoolean result = eglBindTexImage(dpy,surface,buffer);
	ADD(res,result);
}


void eglReleaseTexImage_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSurface surface;
EGLint buffer;
	EXT(args,dpy);
	EXT(args,surface);
	EXT(args,buffer);
EGLBoolean result = eglReleaseTexImage(dpy,surface,buffer);
	ADD(res,result);
}


void eglSurfaceAttrib_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSurface surface;
EGLint attribute;
EGLint value;
	EXT(args,dpy);
	EXT(args,surface);
	EXT(args,attribute);
	EXT(args,value);
EGLBoolean result = eglSurfaceAttrib(dpy,surface,attribute,value);
	ADD(res,result);
}


void eglSwapInterval_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLint interval;
	EXT(args,dpy);
	EXT(args,interval);
EGLBoolean result = eglSwapInterval(dpy,interval);
	ADD(res,result);
}


void eglBindAPI_stub(Buffer &args, Buffer &res)
{
EGLenum api;
	EXT(args,api);
EGLBoolean result = eglBindAPI(api);
	ADD(res,result);
}


void eglQueryAPI_stub(Buffer &args, Buffer &res)
{
EGLenum result = eglQueryAPI();
	ADD(res,result);
}


void eglCreatePbufferFromClientBuffer_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLenum buftype;
EGLClientBuffer buffer;
EGLConfig config;
EGLint *attrib_list;
	EXT(args,dpy);
	EXT(args,buftype);
	EXT(args,buffer);
	EXT(args,config);
	EXT(args,*attrib_list);
EGLSurface result = eglCreatePbufferFromClientBuffer(dpy,buftype,buffer,config,*attrib_list);
	ADD(res,result);
}


void eglReleaseThread_stub(Buffer &args, Buffer &res)
{
EGLBoolean result = eglReleaseThread();
	ADD(res,result);
}


void eglWaitClient_stub(Buffer &args, Buffer &res)
{
EGLBoolean result = eglWaitClient();
	ADD(res,result);
}


void eglGetCurrentContext_stub(Buffer &args, Buffer &res)
{
EGLContext result = eglGetCurrentContext();
	ADD(res,result);
}


void eglCreateSync_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLenum type;
EGLAttrib *attrib_list;
	EXT(args,dpy);
	EXT(args,type);
	EXT(args,*attrib_list);
EGLSync result = eglCreateSync(dpy,type,*attrib_list);
	ADD(res,result);
}


void eglDestroySync_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSync sync;
	EXT(args,dpy);
	EXT(args,sync);
EGLBoolean result = eglDestroySync(dpy,sync);
	ADD(res,result);
}


void eglClientWaitSync_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSync sync;
EGLint flags;
EGLTime timeout;
	EXT(args,dpy);
	EXT(args,sync);
	EXT(args,flags);
	EXT(args,timeout);
EGLint result = eglClientWaitSync(dpy,sync,flags,timeout);
	ADD(res,result);
}


void eglGetSyncAttrib_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSync sync;
EGLint attribute;
EGLAttrib *value;
	EXT(args,dpy);
	EXT(args,sync);
	EXT(args,attribute);
	EXT(args,*value);
EGLBoolean result = eglGetSyncAttrib(dpy,sync,attribute,*value);
	ADD(res,result);
}


void eglCreateImage_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLContext ctx;
EGLenum target;
EGLClientBuffer buffer;
EGLAttrib *attrib_list;
	EXT(args,dpy);
	EXT(args,ctx);
	EXT(args,target);
	EXT(args,buffer);
	EXT(args,*attrib_list);
EGLImage result = eglCreateImage(dpy,ctx,target,buffer,*attrib_list);
	ADD(res,result);
}


void eglDestroyImage_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLImage image;
	EXT(args,dpy);
	EXT(args,image);
EGLBoolean result = eglDestroyImage(dpy,image);
	ADD(res,result);
}


void eglGetPlatformDisplay_stub(Buffer &args, Buffer &res)
{
EGLenum platform;
void *native_display;
EGLAttrib *attrib_list;
	EXT(args,platform);
	EXT(args,*native_display);
	EXT(args,*attrib_list);
EGLDisplay result = eglGetPlatformDisplay(platform,*native_display,*attrib_list);
	ADD(res,result);
}


void eglCreatePlatformWindowSurface_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLConfig config;
void *native_window;
EGLAttrib *attrib_list;
	EXT(args,dpy);
	EXT(args,config);
	EXT(args,*native_window);
	EXT(args,*attrib_list);
EGLSurface result = eglCreatePlatformWindowSurface(dpy,config,*native_window,*attrib_list);
	ADD(res,result);
}


void eglCreatePlatformPixmapSurface_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLConfig config;
void *native_pixmap;
EGLAttrib *attrib_list;
	EXT(args,dpy);
	EXT(args,config);
	EXT(args,*native_pixmap);
	EXT(args,*attrib_list);
EGLSurface result = eglCreatePlatformPixmapSurface(dpy,config,*native_pixmap,*attrib_list);
	ADD(res,result);
}


void eglWaitSync_stub(Buffer &args, Buffer &res)
{
EGLDisplay dpy;
EGLSync sync;
EGLint flags;
	EXT(args,dpy);
	EXT(args,sync);
	EXT(args,flags);
EGLBoolean result = eglWaitSync(dpy,sync,flags);
	ADD(res,result);
}


void invoke(Buffer &args,Buffer &res)
{
uint32_t id = 0;
	EXT(args,id);
	switch(id) {
		case 0: glActiveTexture_stub(args,res);	break;
		case 1: glAttachShader_stub(args,res);	break;
		case 2: glBindAttribLocation_stub(args,res);	break;
		case 3: glBindBuffer_stub(args,res);	break;
		case 4: glBindFramebuffer_stub(args,res);	break;
		case 5: glBindRenderbuffer_stub(args,res);	break;
		case 6: glBindTexture_stub(args,res);	break;
		case 7: glBlendColor_stub(args,res);	break;
		case 8: glBlendEquation_stub(args,res);	break;
		case 9: glBlendEquationSeparate_stub(args,res);	break;
		case 10: glBlendFunc_stub(args,res);	break;
		case 11: glBlendFuncSeparate_stub(args,res);	break;
		case 12: glBufferData_stub(args,res);	break;
		case 13: glBufferSubData_stub(args,res);	break;
		case 14: glCheckFramebufferStatus_stub(args,res);	break;
		case 15: glClear_stub(args,res);	break;
		case 16: glClearColor_stub(args,res);	break;
		case 17: glClearDepthf_stub(args,res);	break;
		case 18: glClearStencil_stub(args,res);	break;
		case 19: glColorMask_stub(args,res);	break;
		case 20: glCompileShader_stub(args,res);	break;
		case 21: glCompressedTexImage2D_stub(args,res);	break;
		case 22: glCompressedTexSubImage2D_stub(args,res);	break;
		case 23: glCopyTexImage2D_stub(args,res);	break;
		case 24: glCopyTexSubImage2D_stub(args,res);	break;
		case 25: glCreateProgram_stub(args,res);	break;
		case 26: glCreateShader_stub(args,res);	break;
		case 27: glCullFace_stub(args,res);	break;
		case 28: glDeleteBuffers_stub(args,res);	break;
		case 29: glDeleteFramebuffers_stub(args,res);	break;
		case 30: glDeleteProgram_stub(args,res);	break;
		case 31: glDeleteRenderbuffers_stub(args,res);	break;
		case 32: glDeleteShader_stub(args,res);	break;
		case 33: glDeleteTextures_stub(args,res);	break;
		case 34: glDepthFunc_stub(args,res);	break;
		case 35: glDepthMask_stub(args,res);	break;
		case 36: glDepthRangef_stub(args,res);	break;
		case 37: glDetachShader_stub(args,res);	break;
		case 38: glDisable_stub(args,res);	break;
		case 39: glDisableVertexAttribArray_stub(args,res);	break;
		case 40: glDrawArrays_stub(args,res);	break;
		case 41: glDrawElements_stub(args,res);	break;
		case 42: glEnable_stub(args,res);	break;
		case 43: glEnableVertexAttribArray_stub(args,res);	break;
		case 44: glFinish_stub(args,res);	break;
		case 45: glFlush_stub(args,res);	break;
		case 46: glFramebufferRenderbuffer_stub(args,res);	break;
		case 47: glFramebufferTexture2D_stub(args,res);	break;
		case 48: glFrontFace_stub(args,res);	break;
		case 49: glGenBuffers_stub(args,res);	break;
		case 50: glGenerateMipmap_stub(args,res);	break;
		case 51: glGenFramebuffers_stub(args,res);	break;
		case 52: glGenRenderbuffers_stub(args,res);	break;
		case 53: glGenTextures_stub(args,res);	break;
		case 54: glGetActiveAttrib_stub(args,res);	break;
		case 55: glGetActiveUniform_stub(args,res);	break;
		case 56: glGetAttachedShaders_stub(args,res);	break;
		case 57: glGetAttribLocation_stub(args,res);	break;
		case 58: glGetBooleanv_stub(args,res);	break;
		case 59: glGetBufferParameteriv_stub(args,res);	break;
		case 60: glGetError_stub(args,res);	break;
		case 61: glGetFloatv_stub(args,res);	break;
		case 62: glGetFramebufferAttachmentParameteriv_stub(args,res);	break;
		case 63: glGetIntegerv_stub(args,res);	break;
		case 64: glGetProgramiv_stub(args,res);	break;
		case 65: glGetProgramInfoLog_stub(args,res);	break;
		case 66: glGetRenderbufferParameteriv_stub(args,res);	break;
		case 67: glGetShaderiv_stub(args,res);	break;
		case 68: glGetShaderInfoLog_stub(args,res);	break;
		case 69: glGetShaderPrecisionFormat_stub(args,res);	break;
		case 70: glGetShaderSource_stub(args,res);	break;
		case 71: glGetString_stub(args,res);	break;
		case 72: glGetTexParameterfv_stub(args,res);	break;
		case 73: glGetTexParameteriv_stub(args,res);	break;
		case 74: glGetUniformfv_stub(args,res);	break;
		case 75: glGetUniformiv_stub(args,res);	break;
		case 76: glGetUniformLocation_stub(args,res);	break;
		case 77: glGetVertexAttribfv_stub(args,res);	break;
		case 78: glGetVertexAttribiv_stub(args,res);	break;
		case 79: glGetVertexAttribPointerv_stub(args,res);	break;
		case 80: glHint_stub(args,res);	break;
		case 81: glIsBuffer_stub(args,res);	break;
		case 82: glIsEnabled_stub(args,res);	break;
		case 83: glIsFramebuffer_stub(args,res);	break;
		case 84: glIsProgram_stub(args,res);	break;
		case 85: glIsRenderbuffer_stub(args,res);	break;
		case 86: glIsShader_stub(args,res);	break;
		case 87: glIsTexture_stub(args,res);	break;
		case 88: glLineWidth_stub(args,res);	break;
		case 89: glLinkProgram_stub(args,res);	break;
		case 90: glPixelStorei_stub(args,res);	break;
		case 91: glPolygonOffset_stub(args,res);	break;
		case 92: glReadPixels_stub(args,res);	break;
		case 93: glReleaseShaderCompiler_stub(args,res);	break;
		case 94: glRenderbufferStorage_stub(args,res);	break;
		case 95: glSampleCoverage_stub(args,res);	break;
		case 96: glScissor_stub(args,res);	break;
		case 97: glShaderBinary_stub(args,res);	break;
		case 98: glShaderSource_stub(args,res);	break;
		case 99: glStencilFunc_stub(args,res);	break;
		case 100: glStencilFuncSeparate_stub(args,res);	break;
		case 101: glStencilMask_stub(args,res);	break;
		case 102: glStencilMaskSeparate_stub(args,res);	break;
		case 103: glStencilOp_stub(args,res);	break;
		case 104: glStencilOpSeparate_stub(args,res);	break;
		case 105: glTexImage2D_stub(args,res);	break;
		case 106: glTexParameterf_stub(args,res);	break;
		case 107: glTexParameterfv_stub(args,res);	break;
		case 108: glTexParameteri_stub(args,res);	break;
		case 109: glTexParameteriv_stub(args,res);	break;
		case 110: glTexSubImage2D_stub(args,res);	break;
		case 111: glUniform1f_stub(args,res);	break;
		case 112: glUniform1fv_stub(args,res);	break;
		case 113: glUniform1i_stub(args,res);	break;
		case 114: glUniform1iv_stub(args,res);	break;
		case 115: glUniform2f_stub(args,res);	break;
		case 116: glUniform2fv_stub(args,res);	break;
		case 117: glUniform2i_stub(args,res);	break;
		case 118: glUniform2iv_stub(args,res);	break;
		case 119: glUniform3f_stub(args,res);	break;
		case 120: glUniform3fv_stub(args,res);	break;
		case 121: glUniform3i_stub(args,res);	break;
		case 122: glUniform3iv_stub(args,res);	break;
		case 123: glUniform4f_stub(args,res);	break;
		case 124: glUniform4fv_stub(args,res);	break;
		case 125: glUniform4i_stub(args,res);	break;
		case 126: glUniform4iv_stub(args,res);	break;
		case 127: glUniformMatrix2fv_stub(args,res);	break;
		case 128: glUniformMatrix3fv_stub(args,res);	break;
		case 129: glUniformMatrix4fv_stub(args,res);	break;
		case 130: glUseProgram_stub(args,res);	break;
		case 131: glValidateProgram_stub(args,res);	break;
		case 132: glVertexAttrib1f_stub(args,res);	break;
		case 133: glVertexAttrib1fv_stub(args,res);	break;
		case 134: glVertexAttrib2f_stub(args,res);	break;
		case 135: glVertexAttrib2fv_stub(args,res);	break;
		case 136: glVertexAttrib3f_stub(args,res);	break;
		case 137: glVertexAttrib3fv_stub(args,res);	break;
		case 138: glVertexAttrib4f_stub(args,res);	break;
		case 139: glVertexAttrib4fv_stub(args,res);	break;
		case 140: glVertexAttribPointer_stub(args,res);	break;
		case 141: glViewport_stub(args,res);	break;
		case 142: eglChooseConfig_stub(args,res);	break;
		case 143: eglCopyBuffers_stub(args,res);	break;
		case 144: eglCreateContext_stub(args,res);	break;
		case 145: eglCreatePbufferSurface_stub(args,res);	break;
		case 146: eglCreatePixmapSurface_stub(args,res);	break;
		case 147: eglCreateWindowSurface_stub(args,res);	break;
		case 148: eglDestroyContext_stub(args,res);	break;
		case 149: eglDestroySurface_stub(args,res);	break;
		case 150: eglGetConfigAttrib_stub(args,res);	break;
		case 151: eglGetConfigs_stub(args,res);	break;
		case 152: eglGetCurrentDisplay_stub(args,res);	break;
		case 153: eglGetCurrentSurface_stub(args,res);	break;
		case 154: eglGetDisplay_stub(args,res);	break;
		case 155: eglGetError_stub(args,res);	break;
		case 156: eglGetProcAddress_stub(args,res);	break;
		case 157: eglInitialize_stub(args,res);	break;
		case 158: eglMakeCurrent_stub(args,res);	break;
		case 159: eglQueryContext_stub(args,res);	break;
		case 160: eglQueryString_stub(args,res);	break;
		case 161: eglQuerySurface_stub(args,res);	break;
		case 162: eglSwapBuffers_stub(args,res);	break;
		case 163: eglTerminate_stub(args,res);	break;
		case 164: eglWaitGL_stub(args,res);	break;
		case 165: eglWaitNative_stub(args,res);	break;
		case 166: eglBindTexImage_stub(args,res);	break;
		case 167: eglReleaseTexImage_stub(args,res);	break;
		case 168: eglSurfaceAttrib_stub(args,res);	break;
		case 169: eglSwapInterval_stub(args,res);	break;
		case 170: eglBindAPI_stub(args,res);	break;
		case 171: eglQueryAPI_stub(args,res);	break;
		case 172: eglCreatePbufferFromClientBuffer_stub(args,res);	break;
		case 173: eglReleaseThread_stub(args,res);	break;
		case 174: eglWaitClient_stub(args,res);	break;
		case 175: eglGetCurrentContext_stub(args,res);	break;
		case 176: eglCreateSync_stub(args,res);	break;
		case 177: eglDestroySync_stub(args,res);	break;
		case 178: eglClientWaitSync_stub(args,res);	break;
		case 179: eglGetSyncAttrib_stub(args,res);	break;
		case 180: eglCreateImage_stub(args,res);	break;
		case 181: eglDestroyImage_stub(args,res);	break;
		case 182: eglGetPlatformDisplay_stub(args,res);	break;
		case 183: eglCreatePlatformWindowSurface_stub(args,res);	break;
		case 184: eglCreatePlatformPixmapSurface_stub(args,res);	break;
		case 185: eglWaitSync_stub(args,res);	break;
		default:;
	}
}

