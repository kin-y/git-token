begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|WIN32_H
end_ifndef
begin_define
DECL|macro|WIN32_H
define|#
directive|define
name|WIN32_H
end_define
begin_comment
comment|/* common Win32 functions for MinGW and Cygwin */
end_comment
begin_ifndef
ifndef|#
directive|ifndef
name|GIT_WINDOWS_NATIVE
end_ifndef
begin_comment
comment|/* Not defined for Cygwin */
end_comment
begin_include
include|#
directive|include
file|<windows.h>
end_include
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|file_attr_to_st_mode
specifier|static
specifier|inline
name|int
name|file_attr_to_st_mode
parameter_list|(
name|DWORD
name|attr
parameter_list|)
block|{
name|int
name|fMode
init|=
name|S_IREAD
decl_stmt|;
if|if
condition|(
name|attr
operator|&
name|FILE_ATTRIBUTE_DIRECTORY
condition|)
name|fMode
operator||=
name|S_IFDIR
expr_stmt|;
else|else
name|fMode
operator||=
name|S_IFREG
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|attr
operator|&
name|FILE_ATTRIBUTE_READONLY
operator|)
condition|)
name|fMode
operator||=
name|S_IWRITE
expr_stmt|;
return|return
name|fMode
return|;
block|}
end_function
begin_function
DECL|function|get_file_attr
specifier|static
specifier|inline
name|int
name|get_file_attr
parameter_list|(
specifier|const
name|char
modifier|*
name|fname
parameter_list|,
name|WIN32_FILE_ATTRIBUTE_DATA
modifier|*
name|fdata
parameter_list|)
block|{
if|if
condition|(
name|GetFileAttributesExA
argument_list|(
name|fname
argument_list|,
name|GetFileExInfoStandard
argument_list|,
name|fdata
argument_list|)
condition|)
return|return
literal|0
return|;
switch|switch
condition|(
name|GetLastError
argument_list|()
condition|)
block|{
case|case
name|ERROR_ACCESS_DENIED
case|:
case|case
name|ERROR_SHARING_VIOLATION
case|:
case|case
name|ERROR_LOCK_VIOLATION
case|:
case|case
name|ERROR_SHARING_BUFFER_EXCEEDED
case|:
return|return
name|EACCES
return|;
case|case
name|ERROR_BUFFER_OVERFLOW
case|:
return|return
name|ENAMETOOLONG
return|;
case|case
name|ERROR_NOT_ENOUGH_MEMORY
case|:
return|return
name|ENOMEM
return|;
default|default:
return|return
name|ENOENT
return|;
block|}
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
end_unit
