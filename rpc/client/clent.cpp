void glActiveTexture(GLenum texture)
{
Buffer buf(0);
	buf.add(texture);
	invoke(buf);
}


void glAttachShader(GLuint program,GLuint shader)
{
Buffer buf(1);
	buf.add(program);
	buf.add(shader);
	invoke(buf);
}


void glBindAttribLocation(GLuint program,GLuint index,const GLchar *name)
{
Buffer buf(2);
	buf.add(program);
	buf.add(index);
	buf.add(*name);
	invoke(buf);
}


void glBindBuffer(GLenum target,GLuint buffer)
{
Buffer buf(3);
	buf.add(target);
	buf.add(buffer);
	invoke(buf);
}


void glBindFramebuffer(GLenum target,GLuint framebuffer)
{
Buffer buf(4);
	buf.add(target);
	buf.add(framebuffer);
	invoke(buf);
}


void glBindRenderbuffer(GLenum target,GLuint renderbuffer)
{
Buffer buf(5);
	buf.add(target);
	buf.add(renderbuffer);
	invoke(buf);
}


void glBindTexture(GLenum target,GLuint texture)
{
Buffer buf(6);
	buf.add(target);
	buf.add(texture);
	invoke(buf);
}


void glBlendColor(GLfloat red,GLfloat green,GLfloat blue,GLfloat alpha)
{
Buffer buf(7);
	buf.add(red);
	buf.add(green);
	buf.add(blue);
	buf.add(alpha);
	invoke(buf);
}


void glBlendEquation(GLenum mode)
{
Buffer buf(8);
	buf.add(mode);
	invoke(buf);
}


void glBlendEquationSeparate(GLenum modeRGB,GLenum modeAlpha)
{
Buffer buf(9);
	buf.add(modeRGB);
	buf.add(modeAlpha);
	invoke(buf);
}


void glBlendFunc(GLenum sfactor,GLenum dfactor)
{
Buffer buf(10);
	buf.add(sfactor);
	buf.add(dfactor);
	invoke(buf);
}


void glBlendFuncSeparate(GLenum sfactorRGB,GLenum dfactorRGB,GLenum sfactorAlpha,GLenum dfactorAlpha)
{
Buffer buf(11);
	buf.add(sfactorRGB);
	buf.add(dfactorRGB);
	buf.add(sfactorAlpha);
	buf.add(dfactorAlpha);
	invoke(buf);
}


void glBufferData(GLenum target,GLsizeiptr size,const void *data,GLenum usage)
{
Buffer buf(12);
	buf.add(target);
	buf.add(size);
	buf.add(*data);
	buf.add(usage);
	invoke(buf);
}


void glBufferSubData(GLenum target,GLintptr offset,GLsizeiptr size,const void *data)
{
Buffer buf(13);
	buf.add(target);
	buf.add(offset);
	buf.add(size);
	buf.add(*data);
	invoke(buf);
}


GLenum glCheckFramebufferStatus(GLenum target)
{
Buffer buf(14);
	buf.add(target);
	invoke(buf);
	GLenum result;
	buf.get(result);
	return result;
}


void glClear(GLbitfield mask)
{
Buffer buf(15);
	buf.add(mask);
	invoke(buf);
}


void glClearColor(GLfloat red,GLfloat green,GLfloat blue,GLfloat alpha)
{
Buffer buf(16);
	buf.add(red);
	buf.add(green);
	buf.add(blue);
	buf.add(alpha);
	invoke(buf);
}


void glClearDepthf(GLfloat d)
{
Buffer buf(17);
	buf.add(d);
	invoke(buf);
}


void glClearStencil(GLint s)
{
Buffer buf(18);
	buf.add(s);
	invoke(buf);
}


void glColorMask(GLboolean red,GLboolean green,GLboolean blue,GLboolean alpha)
{
Buffer buf(19);
	buf.add(red);
	buf.add(green);
	buf.add(blue);
	buf.add(alpha);
	invoke(buf);
}


void glCompileShader(GLuint shader)
{
Buffer buf(20);
	buf.add(shader);
	invoke(buf);
}


void glCompressedTexImage2D(GLenum target,GLint level,GLenum internalformat,GLsizei width,GLsizei height,GLint border,GLsizei imageSize,const void *data)
{
Buffer buf(21);
	buf.add(target);
	buf.add(level);
	buf.add(internalformat);
	buf.add(width);
	buf.add(height);
	buf.add(border);
	buf.add(imageSize);
	buf.add(*data);
	invoke(buf);
}


void glCompressedTexSubImage2D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLsizei width,GLsizei height,GLenum format,GLsizei imageSize,const void *data)
{
Buffer buf(22);
	buf.add(target);
	buf.add(level);
	buf.add(xoffset);
	buf.add(yoffset);
	buf.add(width);
	buf.add(height);
	buf.add(format);
	buf.add(imageSize);
	buf.add(*data);
	invoke(buf);
}


void glCopyTexImage2D(GLenum target,GLint level,GLenum internalformat,GLint x,GLint y,GLsizei width,GLsizei height,GLint border)
{
Buffer buf(23);
	buf.add(target);
	buf.add(level);
	buf.add(internalformat);
	buf.add(x);
	buf.add(y);
	buf.add(width);
	buf.add(height);
	buf.add(border);
	invoke(buf);
}


void glCopyTexSubImage2D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLint x,GLint y,GLsizei width,GLsizei height)
{
Buffer buf(24);
	buf.add(target);
	buf.add(level);
	buf.add(xoffset);
	buf.add(yoffset);
	buf.add(x);
	buf.add(y);
	buf.add(width);
	buf.add(height);
	invoke(buf);
}


GLuint glCreateProgram(void)
{
Buffer buf(25);
	invoke(buf);
	GLuint result;
	buf.get(result);
	return result;
}


GLuint glCreateShader(GLenum type)
{
Buffer buf(26);
	buf.add(type);
	invoke(buf);
	GLuint result;
	buf.get(result);
	return result;
}


void glCullFace(GLenum mode)
{
Buffer buf(27);
	buf.add(mode);
	invoke(buf);
}


void glDeleteBuffers(GLsizei n,const GLuint *buffers)
{
Buffer buf(28);
	buf.add(n);
	buf.add(*buffers);
	invoke(buf);
}


void glDeleteFramebuffers(GLsizei n,const GLuint *framebuffers)
{
Buffer buf(29);
	buf.add(n);
	buf.add(*framebuffers);
	invoke(buf);
}


void glDeleteProgram(GLuint program)
{
Buffer buf(30);
	buf.add(program);
	invoke(buf);
}


void glDeleteRenderbuffers(GLsizei n,const GLuint *renderbuffers)
{
Buffer buf(31);
	buf.add(n);
	buf.add(*renderbuffers);
	invoke(buf);
}


void glDeleteShader(GLuint shader)
{
Buffer buf(32);
	buf.add(shader);
	invoke(buf);
}


void glDeleteTextures(GLsizei n,const GLuint *textures)
{
Buffer buf(33);
	buf.add(n);
	buf.add(*textures);
	invoke(buf);
}


void glDepthFunc(GLenum func)
{
Buffer buf(34);
	buf.add(func);
	invoke(buf);
}


void glDepthMask(GLboolean flag)
{
Buffer buf(35);
	buf.add(flag);
	invoke(buf);
}


void glDepthRangef(GLfloat n,GLfloat f)
{
Buffer buf(36);
	buf.add(n);
	buf.add(f);
	invoke(buf);
}


void glDetachShader(GLuint program,GLuint shader)
{
Buffer buf(37);
	buf.add(program);
	buf.add(shader);
	invoke(buf);
}


void glDisable(GLenum cap)
{
Buffer buf(38);
	buf.add(cap);
	invoke(buf);
}


void glDisableVertexAttribArray(GLuint index)
{
Buffer buf(39);
	buf.add(index);
	invoke(buf);
}


void glDrawArrays(GLenum mode,GLint first,GLsizei count)
{
Buffer buf(40);
	buf.add(mode);
	buf.add(first);
	buf.add(count);
	invoke(buf);
}


void glDrawElements(GLenum mode,GLsizei count,GLenum type,const void *indices)
{
Buffer buf(41);
	buf.add(mode);
	buf.add(count);
	buf.add(type);
	buf.add(*indices);
	invoke(buf);
}


void glEnable(GLenum cap)
{
Buffer buf(42);
	buf.add(cap);
	invoke(buf);
}


void glEnableVertexAttribArray(GLuint index)
{
Buffer buf(43);
	buf.add(index);
	invoke(buf);
}


void glFinish(void)
{
Buffer buf(44);
	invoke(buf);
}


void glFlush(void)
{
Buffer buf(45);
	invoke(buf);
}


void glFramebufferRenderbuffer(GLenum target,GLenum attachment,GLenum renderbuffertarget,GLuint renderbuffer)
{
Buffer buf(46);
	buf.add(target);
	buf.add(attachment);
	buf.add(renderbuffertarget);
	buf.add(renderbuffer);
	invoke(buf);
}


void glFramebufferTexture2D(GLenum target,GLenum attachment,GLenum textarget,GLuint texture,GLint level)
{
Buffer buf(47);
	buf.add(target);
	buf.add(attachment);
	buf.add(textarget);
	buf.add(texture);
	buf.add(level);
	invoke(buf);
}


void glFrontFace(GLenum mode)
{
Buffer buf(48);
	buf.add(mode);
	invoke(buf);
}


void glGenBuffers(GLsizei n,GLuint *buffers)
{
Buffer buf(49);
	buf.add(n);
	buf.add(*buffers);
	invoke(buf);
}


void glGenerateMipmap(GLenum target)
{
Buffer buf(50);
	buf.add(target);
	invoke(buf);
}


void glGenFramebuffers(GLsizei n,GLuint *framebuffers)
{
Buffer buf(51);
	buf.add(n);
	buf.add(*framebuffers);
	invoke(buf);
}


void glGenRenderbuffers(GLsizei n,GLuint *renderbuffers)
{
Buffer buf(52);
	buf.add(n);
	buf.add(*renderbuffers);
	invoke(buf);
}


void glGenTextures(GLsizei n,GLuint *textures)
{
Buffer buf(53);
	buf.add(n);
	buf.add(*textures);
	invoke(buf);
}


void glGetActiveAttrib(GLuint program,GLuint index,GLsizei bufSize,GLsizei *length,GLint *size,GLenum *type,GLchar *name)
{
Buffer buf(54);
	buf.add(program);
	buf.add(index);
	buf.add(bufSize);
	buf.add(*length);
	buf.add(*size);
	buf.add(*type);
	buf.add(*name);
	invoke(buf);
}


void glGetActiveUniform(GLuint program,GLuint index,GLsizei bufSize,GLsizei *length,GLint *size,GLenum *type,GLchar *name)
{
Buffer buf(55);
	buf.add(program);
	buf.add(index);
	buf.add(bufSize);
	buf.add(*length);
	buf.add(*size);
	buf.add(*type);
	buf.add(*name);
	invoke(buf);
}


void glGetAttachedShaders(GLuint program,GLsizei maxCount,GLsizei *count,GLuint *shaders)
{
Buffer buf(56);
	buf.add(program);
	buf.add(maxCount);
	buf.add(*count);
	buf.add(*shaders);
	invoke(buf);
}


GLint glGetAttribLocation(GLuint program,const GLchar *name)
{
Buffer buf(57);
	buf.add(program);
	buf.add(*name);
	invoke(buf);
	GLint result;
	buf.get(result);
	return result;
}


void glGetBooleanv(GLenum pname,GLboolean *data)
{
Buffer buf(58);
	buf.add(pname);
	buf.add(*data);
	invoke(buf);
}


void glGetBufferParameteriv(GLenum target,GLenum pname,GLint *params)
{
Buffer buf(59);
	buf.add(target);
	buf.add(pname);
	buf.add(*params);
	invoke(buf);
}


GLenum glGetError(void)
{
Buffer buf(60);
	invoke(buf);
	GLenum result;
	buf.get(result);
	return result;
}


void glGetFloatv(GLenum pname,GLfloat *data)
{
Buffer buf(61);
	buf.add(pname);
	buf.add(*data);
	invoke(buf);
}


void glGetFramebufferAttachmentParameteriv(GLenum target,GLenum attachment,GLenum pname,GLint *params)
{
Buffer buf(62);
	buf.add(target);
	buf.add(attachment);
	buf.add(pname);
	buf.add(*params);
	invoke(buf);
}


void glGetIntegerv(GLenum pname,GLint *data)
{
Buffer buf(63);
	buf.add(pname);
	buf.add(*data);
	invoke(buf);
}


void glGetProgramiv(GLuint program,GLenum pname,GLint *params)
{
Buffer buf(64);
	buf.add(program);
	buf.add(pname);
	buf.add(*params);
	invoke(buf);
}


void glGetProgramInfoLog(GLuint program,GLsizei bufSize,GLsizei *length,GLchar *infoLog)
{
Buffer buf(65);
	buf.add(program);
	buf.add(bufSize);
	buf.add(*length);
	buf.add(*infoLog);
	invoke(buf);
}


void glGetRenderbufferParameteriv(GLenum target,GLenum pname,GLint *params)
{
Buffer buf(66);
	buf.add(target);
	buf.add(pname);
	buf.add(*params);
	invoke(buf);
}


void glGetShaderiv(GLuint shader,GLenum pname,GLint *params)
{
Buffer buf(67);
	buf.add(shader);
	buf.add(pname);
	buf.add(*params);
	invoke(buf);
}


void glGetShaderInfoLog(GLuint shader,GLsizei bufSize,GLsizei *length,GLchar *infoLog)
{
Buffer buf(68);
	buf.add(shader);
	buf.add(bufSize);
	buf.add(*length);
	buf.add(*infoLog);
	invoke(buf);
}


void glGetShaderPrecisionFormat(GLenum shadertype,GLenum precisiontype,GLint *range,GLint *precision)
{
Buffer buf(69);
	buf.add(shadertype);
	buf.add(precisiontype);
	buf.add(*range);
	buf.add(*precision);
	invoke(buf);
}


void glGetShaderSource(GLuint shader,GLsizei bufSize,GLsizei *length,GLchar *source)
{
Buffer buf(70);
	buf.add(shader);
	buf.add(bufSize);
	buf.add(*length);
	buf.add(*source);
	invoke(buf);
}


const GLubyte *glGetString(GLenum name)
{
Buffer buf(71);
	buf.add(name);
	invoke(buf);
	GLubyte result;
	buf.get(result);
	return result;
}


void glGetTexParameterfv(GLenum target,GLenum pname,GLfloat *params)
{
Buffer buf(72);
	buf.add(target);
	buf.add(pname);
	buf.add(*params);
	invoke(buf);
}


void glGetTexParameteriv(GLenum target,GLenum pname,GLint *params)
{
Buffer buf(73);
	buf.add(target);
	buf.add(pname);
	buf.add(*params);
	invoke(buf);
}


void glGetUniformfv(GLuint program,GLint location,GLfloat *params)
{
Buffer buf(74);
	buf.add(program);
	buf.add(location);
	buf.add(*params);
	invoke(buf);
}


void glGetUniformiv(GLuint program,GLint location,GLint *params)
{
Buffer buf(75);
	buf.add(program);
	buf.add(location);
	buf.add(*params);
	invoke(buf);
}


GLint glGetUniformLocation(GLuint program,const GLchar *name)
{
Buffer buf(76);
	buf.add(program);
	buf.add(*name);
	invoke(buf);
	GLint result;
	buf.get(result);
	return result;
}


void glGetVertexAttribfv(GLuint index,GLenum pname,GLfloat *params)
{
Buffer buf(77);
	buf.add(index);
	buf.add(pname);
	buf.add(*params);
	invoke(buf);
}


void glGetVertexAttribiv(GLuint index,GLenum pname,GLint *params)
{
Buffer buf(78);
	buf.add(index);
	buf.add(pname);
	buf.add(*params);
	invoke(buf);
}


void glGetVertexAttribPointerv(GLuint index,GLenum pname,void **pointer)
{
Buffer buf(79);
	buf.add(index);
	buf.add(pname);
	buf.add(**pointer);
	invoke(buf);
}


void glHint(GLenum target,GLenum mode)
{
Buffer buf(80);
	buf.add(target);
	buf.add(mode);
	invoke(buf);
}


GLboolean glIsBuffer(GLuint buffer)
{
Buffer buf(81);
	buf.add(buffer);
	invoke(buf);
	GLboolean result;
	buf.get(result);
	return result;
}


GLboolean glIsEnabled(GLenum cap)
{
Buffer buf(82);
	buf.add(cap);
	invoke(buf);
	GLboolean result;
	buf.get(result);
	return result;
}


GLboolean glIsFramebuffer(GLuint framebuffer)
{
Buffer buf(83);
	buf.add(framebuffer);
	invoke(buf);
	GLboolean result;
	buf.get(result);
	return result;
}


GLboolean glIsProgram(GLuint program)
{
Buffer buf(84);
	buf.add(program);
	invoke(buf);
	GLboolean result;
	buf.get(result);
	return result;
}


GLboolean glIsRenderbuffer(GLuint renderbuffer)
{
Buffer buf(85);
	buf.add(renderbuffer);
	invoke(buf);
	GLboolean result;
	buf.get(result);
	return result;
}


GLboolean glIsShader(GLuint shader)
{
Buffer buf(86);
	buf.add(shader);
	invoke(buf);
	GLboolean result;
	buf.get(result);
	return result;
}


GLboolean glIsTexture(GLuint texture)
{
Buffer buf(87);
	buf.add(texture);
	invoke(buf);
	GLboolean result;
	buf.get(result);
	return result;
}


void glLineWidth(GLfloat width)
{
Buffer buf(88);
	buf.add(width);
	invoke(buf);
}


void glLinkProgram(GLuint program)
{
Buffer buf(89);
	buf.add(program);
	invoke(buf);
}


void glPixelStorei(GLenum pname,GLint param)
{
Buffer buf(90);
	buf.add(pname);
	buf.add(param);
	invoke(buf);
}


void glPolygonOffset(GLfloat factor,GLfloat units)
{
Buffer buf(91);
	buf.add(factor);
	buf.add(units);
	invoke(buf);
}


void glReadPixels(GLint x,GLint y,GLsizei width,GLsizei height,GLenum format,GLenum type,void *pixels)
{
Buffer buf(92);
	buf.add(x);
	buf.add(y);
	buf.add(width);
	buf.add(height);
	buf.add(format);
	buf.add(type);
	buf.add(*pixels);
	invoke(buf);
}


void glReleaseShaderCompiler(void)
{
Buffer buf(93);
	invoke(buf);
}


void glRenderbufferStorage(GLenum target,GLenum internalformat,GLsizei width,GLsizei height)
{
Buffer buf(94);
	buf.add(target);
	buf.add(internalformat);
	buf.add(width);
	buf.add(height);
	invoke(buf);
}


void glSampleCoverage(GLfloat value,GLboolean invert)
{
Buffer buf(95);
	buf.add(value);
	buf.add(invert);
	invoke(buf);
}


void glScissor(GLint x,GLint y,GLsizei width,GLsizei height)
{
Buffer buf(96);
	buf.add(x);
	buf.add(y);
	buf.add(width);
	buf.add(height);
	invoke(buf);
}


void glShaderBinary(GLsizei count,const GLuint *shaders,GLenum binaryformat,const void *binary,GLsizei length)
{
Buffer buf(97);
	buf.add(count);
	buf.add(*shaders);
	buf.add(binaryformat);
	buf.add(*binary);
	buf.add(length);
	invoke(buf);
}


void glShaderSource(GLuint shader,GLsizei count,const GLchar *const*string,const GLint *length)
{
Buffer buf(98);
	buf.add(shader);
	buf.add(count);
	buf.add(*const*string);
	buf.add(*length);
	invoke(buf);
}


void glStencilFunc(GLenum func,GLint ref,GLuint mask)
{
Buffer buf(99);
	buf.add(func);
	buf.add(ref);
	buf.add(mask);
	invoke(buf);
}


void glStencilFuncSeparate(GLenum face,GLenum func,GLint ref,GLuint mask)
{
Buffer buf(100);
	buf.add(face);
	buf.add(func);
	buf.add(ref);
	buf.add(mask);
	invoke(buf);
}


void glStencilMask(GLuint mask)
{
Buffer buf(101);
	buf.add(mask);
	invoke(buf);
}


void glStencilMaskSeparate(GLenum face,GLuint mask)
{
Buffer buf(102);
	buf.add(face);
	buf.add(mask);
	invoke(buf);
}


void glStencilOp(GLenum fail,GLenum zfail,GLenum zpass)
{
Buffer buf(103);
	buf.add(fail);
	buf.add(zfail);
	buf.add(zpass);
	invoke(buf);
}


void glStencilOpSeparate(GLenum face,GLenum sfail,GLenum dpfail,GLenum dppass)
{
Buffer buf(104);
	buf.add(face);
	buf.add(sfail);
	buf.add(dpfail);
	buf.add(dppass);
	invoke(buf);
}


void glTexImage2D(GLenum target,GLint level,GLint internalformat,GLsizei width,GLsizei height,GLint border,GLenum format,GLenum type,const void *pixels)
{
Buffer buf(105);
	buf.add(target);
	buf.add(level);
	buf.add(internalformat);
	buf.add(width);
	buf.add(height);
	buf.add(border);
	buf.add(format);
	buf.add(type);
	buf.add(*pixels);
	invoke(buf);
}


void glTexParameterf(GLenum target,GLenum pname,GLfloat param)
{
Buffer buf(106);
	buf.add(target);
	buf.add(pname);
	buf.add(param);
	invoke(buf);
}


void glTexParameterfv(GLenum target,GLenum pname,const GLfloat *params)
{
Buffer buf(107);
	buf.add(target);
	buf.add(pname);
	buf.add(*params);
	invoke(buf);
}


void glTexParameteri(GLenum target,GLenum pname,GLint param)
{
Buffer buf(108);
	buf.add(target);
	buf.add(pname);
	buf.add(param);
	invoke(buf);
}


void glTexParameteriv(GLenum target,GLenum pname,const GLint *params)
{
Buffer buf(109);
	buf.add(target);
	buf.add(pname);
	buf.add(*params);
	invoke(buf);
}


void glTexSubImage2D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLsizei width,GLsizei height,GLenum format,GLenum type,const void *pixels)
{
Buffer buf(110);
	buf.add(target);
	buf.add(level);
	buf.add(xoffset);
	buf.add(yoffset);
	buf.add(width);
	buf.add(height);
	buf.add(format);
	buf.add(type);
	buf.add(*pixels);
	invoke(buf);
}


void glUniform1f(GLint location,GLfloat v0)
{
Buffer buf(111);
	buf.add(location);
	buf.add(v0);
	invoke(buf);
}


void glUniform1fv(GLint location,GLsizei count,const GLfloat *value)
{
Buffer buf(112);
	buf.add(location);
	buf.add(count);
	buf.add(*value);
	invoke(buf);
}


void glUniform1i(GLint location,GLint v0)
{
Buffer buf(113);
	buf.add(location);
	buf.add(v0);
	invoke(buf);
}


void glUniform1iv(GLint location,GLsizei count,const GLint *value)
{
Buffer buf(114);
	buf.add(location);
	buf.add(count);
	buf.add(*value);
	invoke(buf);
}


void glUniform2f(GLint location,GLfloat v0,GLfloat v1)
{
Buffer buf(115);
	buf.add(location);
	buf.add(v0);
	buf.add(v1);
	invoke(buf);
}


void glUniform2fv(GLint location,GLsizei count,const GLfloat *value)
{
Buffer buf(116);
	buf.add(location);
	buf.add(count);
	buf.add(*value);
	invoke(buf);
}


void glUniform2i(GLint location,GLint v0,GLint v1)
{
Buffer buf(117);
	buf.add(location);
	buf.add(v0);
	buf.add(v1);
	invoke(buf);
}


void glUniform2iv(GLint location,GLsizei count,const GLint *value)
{
Buffer buf(118);
	buf.add(location);
	buf.add(count);
	buf.add(*value);
	invoke(buf);
}


void glUniform3f(GLint location,GLfloat v0,GLfloat v1,GLfloat v2)
{
Buffer buf(119);
	buf.add(location);
	buf.add(v0);
	buf.add(v1);
	buf.add(v2);
	invoke(buf);
}


void glUniform3fv(GLint location,GLsizei count,const GLfloat *value)
{
Buffer buf(120);
	buf.add(location);
	buf.add(count);
	buf.add(*value);
	invoke(buf);
}


void glUniform3i(GLint location,GLint v0,GLint v1,GLint v2)
{
Buffer buf(121);
	buf.add(location);
	buf.add(v0);
	buf.add(v1);
	buf.add(v2);
	invoke(buf);
}


void glUniform3iv(GLint location,GLsizei count,const GLint *value)
{
Buffer buf(122);
	buf.add(location);
	buf.add(count);
	buf.add(*value);
	invoke(buf);
}


void glUniform4f(GLint location,GLfloat v0,GLfloat v1,GLfloat v2,GLfloat v3)
{
Buffer buf(123);
	buf.add(location);
	buf.add(v0);
	buf.add(v1);
	buf.add(v2);
	buf.add(v3);
	invoke(buf);
}


void glUniform4fv(GLint location,GLsizei count,const GLfloat *value)
{
Buffer buf(124);
	buf.add(location);
	buf.add(count);
	buf.add(*value);
	invoke(buf);
}


void glUniform4i(GLint location,GLint v0,GLint v1,GLint v2,GLint v3)
{
Buffer buf(125);
	buf.add(location);
	buf.add(v0);
	buf.add(v1);
	buf.add(v2);
	buf.add(v3);
	invoke(buf);
}


void glUniform4iv(GLint location,GLsizei count,const GLint *value)
{
Buffer buf(126);
	buf.add(location);
	buf.add(count);
	buf.add(*value);
	invoke(buf);
}


void glUniformMatrix2fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat *value)
{
Buffer buf(127);
	buf.add(location);
	buf.add(count);
	buf.add(transpose);
	buf.add(*value);
	invoke(buf);
}


void glUniformMatrix3fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat *value)
{
Buffer buf(128);
	buf.add(location);
	buf.add(count);
	buf.add(transpose);
	buf.add(*value);
	invoke(buf);
}


void glUniformMatrix4fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat *value)
{
Buffer buf(129);
	buf.add(location);
	buf.add(count);
	buf.add(transpose);
	buf.add(*value);
	invoke(buf);
}


void glUseProgram(GLuint program)
{
Buffer buf(130);
	buf.add(program);
	invoke(buf);
}


void glValidateProgram(GLuint program)
{
Buffer buf(131);
	buf.add(program);
	invoke(buf);
}


void glVertexAttrib1f(GLuint index,GLfloat x)
{
Buffer buf(132);
	buf.add(index);
	buf.add(x);
	invoke(buf);
}


void glVertexAttrib1fv(GLuint index,const GLfloat *v)
{
Buffer buf(133);
	buf.add(index);
	buf.add(*v);
	invoke(buf);
}


void glVertexAttrib2f(GLuint index,GLfloat x,GLfloat y)
{
Buffer buf(134);
	buf.add(index);
	buf.add(x);
	buf.add(y);
	invoke(buf);
}


void glVertexAttrib2fv(GLuint index,const GLfloat *v)
{
Buffer buf(135);
	buf.add(index);
	buf.add(*v);
	invoke(buf);
}


void glVertexAttrib3f(GLuint index,GLfloat x,GLfloat y,GLfloat z)
{
Buffer buf(136);
	buf.add(index);
	buf.add(x);
	buf.add(y);
	buf.add(z);
	invoke(buf);
}


void glVertexAttrib3fv(GLuint index,const GLfloat *v)
{
Buffer buf(137);
	buf.add(index);
	buf.add(*v);
	invoke(buf);
}


void glVertexAttrib4f(GLuint index,GLfloat x,GLfloat y,GLfloat z,GLfloat w)
{
Buffer buf(138);
	buf.add(index);
	buf.add(x);
	buf.add(y);
	buf.add(z);
	buf.add(w);
	invoke(buf);
}


void glVertexAttrib4fv(GLuint index,const GLfloat *v)
{
Buffer buf(139);
	buf.add(index);
	buf.add(*v);
	invoke(buf);
}


void glVertexAttribPointer(GLuint index,GLint size,GLenum type,GLboolean normalized,GLsizei stride,const void *pointer)
{
Buffer buf(140);
	buf.add(index);
	buf.add(size);
	buf.add(type);
	buf.add(normalized);
	buf.add(stride);
	buf.add(*pointer);
	invoke(buf);
}


void glViewport(GLint x,GLint y,GLsizei width,GLsizei height)
{
Buffer buf(141);
	buf.add(x);
	buf.add(y);
	buf.add(width);
	buf.add(height);
	invoke(buf);
}


EGLBoolean eglChooseConfig(EGLDisplay dpy,const EGLint *attrib_list,EGLConfig *configs,EGLint config_size,EGLint *num_config)
{
Buffer buf(142);
	buf.add(dpy);
	buf.add(*attrib_list);
	buf.add(*configs);
	buf.add(config_size);
	buf.add(*num_config);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglCopyBuffers(EGLDisplay dpy,EGLSurface surface,EGLNativePixmapType target)
{
Buffer buf(143);
	buf.add(dpy);
	buf.add(surface);
	buf.add(target);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLContext eglCreateContext(EGLDisplay dpy,EGLConfig config,EGLContext share_context,const EGLint *attrib_list)
{
Buffer buf(144);
	buf.add(dpy);
	buf.add(config);
	buf.add(share_context);
	buf.add(*attrib_list);
	invoke(buf);
	EGLContext result;
	buf.get(result);
	return result;
}


EGLSurface eglCreatePbufferSurface(EGLDisplay dpy,EGLConfig config,const EGLint *attrib_list)
{
Buffer buf(145);
	buf.add(dpy);
	buf.add(config);
	buf.add(*attrib_list);
	invoke(buf);
	EGLSurface result;
	buf.get(result);
	return result;
}


EGLSurface eglCreatePixmapSurface(EGLDisplay dpy,EGLConfig config,EGLNativePixmapType pixmap,const EGLint *attrib_list)
{
Buffer buf(146);
	buf.add(dpy);
	buf.add(config);
	buf.add(pixmap);
	buf.add(*attrib_list);
	invoke(buf);
	EGLSurface result;
	buf.get(result);
	return result;
}


EGLSurface eglCreateWindowSurface(EGLDisplay dpy,EGLConfig config,EGLNativeWindowType win,const EGLint *attrib_list)
{
Buffer buf(147);
	buf.add(dpy);
	buf.add(config);
	buf.add(win);
	buf.add(*attrib_list);
	invoke(buf);
	EGLSurface result;
	buf.get(result);
	return result;
}


EGLBoolean eglDestroyContext(EGLDisplay dpy,EGLContext ctx)
{
Buffer buf(148);
	buf.add(dpy);
	buf.add(ctx);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglDestroySurface(EGLDisplay dpy,EGLSurface surface)
{
Buffer buf(149);
	buf.add(dpy);
	buf.add(surface);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglGetConfigAttrib(EGLDisplay dpy,EGLConfig config,EGLint attribute,EGLint *value)
{
Buffer buf(150);
	buf.add(dpy);
	buf.add(config);
	buf.add(attribute);
	buf.add(*value);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglGetConfigs(EGLDisplay dpy,EGLConfig *configs,EGLint config_size,EGLint *num_config)
{
Buffer buf(151);
	buf.add(dpy);
	buf.add(*configs);
	buf.add(config_size);
	buf.add(*num_config);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLDisplay eglGetCurrentDisplay(void)
{
Buffer buf(152);
	invoke(buf);
	EGLDisplay result;
	buf.get(result);
	return result;
}


EGLSurface eglGetCurrentSurface(EGLint readdraw)
{
Buffer buf(153);
	buf.add(readdraw);
	invoke(buf);
	EGLSurface result;
	buf.get(result);
	return result;
}


EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id)
{
Buffer buf(154);
	buf.add(display_id);
	invoke(buf);
	EGLDisplay result;
	buf.get(result);
	return result;
}


EGLint eglGetError(void)
{
Buffer buf(155);
	invoke(buf);
	EGLint result;
	buf.get(result);
	return result;
}


void *eglGetProcAddress(const char *procname)
{
Buffer buf(156);
	buf.add(*procname);
	invoke(buf);
}


EGLBoolean eglInitialize(EGLDisplay dpy,EGLint *major,EGLint *minor)
{
Buffer buf(157);
	buf.add(dpy);
	buf.add(*major);
	buf.add(*minor);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglMakeCurrent(EGLDisplay dpy,EGLSurface draw,EGLSurface read,EGLContext ctx)
{
Buffer buf(158);
	buf.add(dpy);
	buf.add(draw);
	buf.add(read);
	buf.add(ctx);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglQueryContext(EGLDisplay dpy,EGLContext ctx,EGLint attribute,EGLint *value)
{
Buffer buf(159);
	buf.add(dpy);
	buf.add(ctx);
	buf.add(attribute);
	buf.add(*value);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


const char *eglQueryString(EGLDisplay dpy,EGLint name)
{
Buffer buf(160);
	buf.add(dpy);
	buf.add(name);
	invoke(buf);
	char result;
	buf.get(result);
	return result;
}


EGLBoolean eglQuerySurface(EGLDisplay dpy,EGLSurface surface,EGLint attribute,EGLint *value)
{
Buffer buf(161);
	buf.add(dpy);
	buf.add(surface);
	buf.add(attribute);
	buf.add(*value);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglSwapBuffers(EGLDisplay dpy,EGLSurface surface)
{
Buffer buf(162);
	buf.add(dpy);
	buf.add(surface);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglTerminate(EGLDisplay dpy)
{
Buffer buf(163);
	buf.add(dpy);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglWaitGL(void)
{
Buffer buf(164);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglWaitNative(EGLint engine)
{
Buffer buf(165);
	buf.add(engine);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglBindTexImage(EGLDisplay dpy,EGLSurface surface,EGLint buffer)
{
Buffer buf(166);
	buf.add(dpy);
	buf.add(surface);
	buf.add(buffer);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglReleaseTexImage(EGLDisplay dpy,EGLSurface surface,EGLint buffer)
{
Buffer buf(167);
	buf.add(dpy);
	buf.add(surface);
	buf.add(buffer);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglSurfaceAttrib(EGLDisplay dpy,EGLSurface surface,EGLint attribute,EGLint value)
{
Buffer buf(168);
	buf.add(dpy);
	buf.add(surface);
	buf.add(attribute);
	buf.add(value);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglSwapInterval(EGLDisplay dpy,EGLint interval)
{
Buffer buf(169);
	buf.add(dpy);
	buf.add(interval);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglBindAPI(EGLenum api)
{
Buffer buf(170);
	buf.add(api);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLenum eglQueryAPI(void)
{
Buffer buf(171);
	invoke(buf);
	EGLenum result;
	buf.get(result);
	return result;
}


EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay dpy,EGLenum buftype,EGLClientBuffer buffer,EGLConfig config,const EGLint *attrib_list)
{
Buffer buf(172);
	buf.add(dpy);
	buf.add(buftype);
	buf.add(buffer);
	buf.add(config);
	buf.add(*attrib_list);
	invoke(buf);
	EGLSurface result;
	buf.get(result);
	return result;
}


EGLBoolean eglReleaseThread(void)
{
Buffer buf(173);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLBoolean eglWaitClient(void)
{
Buffer buf(174);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLContext eglGetCurrentContext(void)
{
Buffer buf(175);
	invoke(buf);
	EGLContext result;
	buf.get(result);
	return result;
}


EGLSync eglCreateSync(EGLDisplay dpy,EGLenum type,const EGLAttrib *attrib_list)
{
Buffer buf(176);
	buf.add(dpy);
	buf.add(type);
	buf.add(*attrib_list);
	invoke(buf);
	EGLSync result;
	buf.get(result);
	return result;
}


EGLBoolean eglDestroySync(EGLDisplay dpy,EGLSync sync)
{
Buffer buf(177);
	buf.add(dpy);
	buf.add(sync);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLint eglClientWaitSync(EGLDisplay dpy,EGLSync sync,EGLint flags,EGLTime timeout)
{
Buffer buf(178);
	buf.add(dpy);
	buf.add(sync);
	buf.add(flags);
	buf.add(timeout);
	invoke(buf);
	EGLint result;
	buf.get(result);
	return result;
}


EGLBoolean eglGetSyncAttrib(EGLDisplay dpy,EGLSync sync,EGLint attribute,EGLAttrib *value)
{
Buffer buf(179);
	buf.add(dpy);
	buf.add(sync);
	buf.add(attribute);
	buf.add(*value);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLImage eglCreateImage(EGLDisplay dpy,EGLContext ctx,EGLenum target,EGLClientBuffer buffer,const EGLAttrib *attrib_list)
{
Buffer buf(180);
	buf.add(dpy);
	buf.add(ctx);
	buf.add(target);
	buf.add(buffer);
	buf.add(*attrib_list);
	invoke(buf);
	EGLImage result;
	buf.get(result);
	return result;
}


EGLBoolean eglDestroyImage(EGLDisplay dpy,EGLImage image)
{
Buffer buf(181);
	buf.add(dpy);
	buf.add(image);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


EGLDisplay eglGetPlatformDisplay(EGLenum platform,void *native_display,const EGLAttrib *attrib_list)
{
Buffer buf(182);
	buf.add(platform);
	buf.add(*native_display);
	buf.add(*attrib_list);
	invoke(buf);
	EGLDisplay result;
	buf.get(result);
	return result;
}


EGLSurface eglCreatePlatformWindowSurface(EGLDisplay dpy,EGLConfig config,void *native_window,const EGLAttrib *attrib_list)
{
Buffer buf(183);
	buf.add(dpy);
	buf.add(config);
	buf.add(*native_window);
	buf.add(*attrib_list);
	invoke(buf);
	EGLSurface result;
	buf.get(result);
	return result;
}


EGLSurface eglCreatePlatformPixmapSurface(EGLDisplay dpy,EGLConfig config,void *native_pixmap,const EGLAttrib *attrib_list)
{
Buffer buf(184);
	buf.add(dpy);
	buf.add(config);
	buf.add(*native_pixmap);
	buf.add(*attrib_list);
	invoke(buf);
	EGLSurface result;
	buf.get(result);
	return result;
}


EGLBoolean eglWaitSync(EGLDisplay dpy,EGLSync sync,EGLint flags)
{
Buffer buf(185);
	buf.add(dpy);
	buf.add(sync);
	buf.add(flags);
	invoke(buf);
	EGLBoolean result;
	buf.get(result);
	return result;
}


