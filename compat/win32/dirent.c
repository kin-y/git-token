begin_unit
begin_include
include|#
directive|include
file|"../../git-compat-util.h"
end_include
begin_struct
DECL|struct|DIR
struct|struct
name|DIR
block|{
DECL|member|dd_dir
name|struct
name|dirent
name|dd_dir
decl_stmt|;
comment|/* includes d_type */
DECL|member|dd_handle
name|HANDLE
name|dd_handle
decl_stmt|;
comment|/* FindFirstFile handle */
DECL|member|dd_stat
name|int
name|dd_stat
decl_stmt|;
comment|/* 0-based index */
block|}
struct|;
end_struct
begin_function
DECL|function|finddata2dirent
specifier|static
specifier|inline
name|void
name|finddata2dirent
parameter_list|(
name|struct
name|dirent
modifier|*
name|ent
parameter_list|,
name|WIN32_FIND_DATAA
modifier|*
name|fdata
parameter_list|)
block|{
comment|/* copy file name from WIN32_FIND_DATA to dirent */
name|memcpy
argument_list|(
name|ent
operator|->
name|d_name
argument_list|,
name|fdata
operator|->
name|cFileName
argument_list|,
sizeof|sizeof
argument_list|(
name|ent
operator|->
name|d_name
argument_list|)
argument_list|)
expr_stmt|;
comment|/* Set file type, based on WIN32_FIND_DATA */
if|if
condition|(
name|fdata
operator|->
name|dwFileAttributes
operator|&
name|FILE_ATTRIBUTE_DIRECTORY
condition|)
name|ent
operator|->
name|d_type
operator|=
name|DT_DIR
expr_stmt|;
else|else
name|ent
operator|->
name|d_type
operator|=
name|DT_REG
expr_stmt|;
block|}
end_function
begin_function
DECL|function|opendir
name|DIR
modifier|*
name|opendir
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|char
name|pattern
index|[
name|MAX_PATH
index|]
decl_stmt|;
name|WIN32_FIND_DATAA
name|fdata
decl_stmt|;
name|HANDLE
name|h
decl_stmt|;
name|int
name|len
decl_stmt|;
name|DIR
modifier|*
name|dir
decl_stmt|;
comment|/* check that name is not NULL */
if|if
condition|(
operator|!
name|name
condition|)
block|{
name|errno
operator|=
name|EINVAL
expr_stmt|;
return|return
name|NULL
return|;
block|}
comment|/* check that the pattern won't be too long for FindFirstFileA */
name|len
operator|=
name|strlen
argument_list|(
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|+
literal|2
operator|>=
name|MAX_PATH
condition|)
block|{
name|errno
operator|=
name|ENAMETOOLONG
expr_stmt|;
return|return
name|NULL
return|;
block|}
comment|/* copy name to temp buffer */
name|memcpy
argument_list|(
name|pattern
argument_list|,
name|name
argument_list|,
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
comment|/* append optional '/' and wildcard '*' */
if|if
condition|(
name|len
operator|&&
operator|!
name|is_dir_sep
argument_list|(
name|pattern
index|[
name|len
operator|-
literal|1
index|]
argument_list|)
condition|)
name|pattern
index|[
name|len
operator|++
index|]
operator|=
literal|'/'
expr_stmt|;
name|pattern
index|[
name|len
operator|++
index|]
operator|=
literal|'*'
expr_stmt|;
name|pattern
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
comment|/* open find handle */
name|h
operator|=
name|FindFirstFileA
argument_list|(
name|pattern
argument_list|,
operator|&
name|fdata
argument_list|)
expr_stmt|;
if|if
condition|(
name|h
operator|==
name|INVALID_HANDLE_VALUE
condition|)
block|{
name|DWORD
name|err
init|=
name|GetLastError
argument_list|()
decl_stmt|;
name|errno
operator|=
operator|(
name|err
operator|==
name|ERROR_DIRECTORY
operator|)
condition|?
name|ENOTDIR
else|:
name|err_win_to_posix
argument_list|(
name|err
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
comment|/* initialize DIR structure and copy first dir entry */
name|dir
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|DIR
argument_list|)
argument_list|)
expr_stmt|;
name|dir
operator|->
name|dd_handle
operator|=
name|h
expr_stmt|;
name|dir
operator|->
name|dd_stat
operator|=
literal|0
expr_stmt|;
name|finddata2dirent
argument_list|(
operator|&
name|dir
operator|->
name|dd_dir
argument_list|,
operator|&
name|fdata
argument_list|)
expr_stmt|;
return|return
name|dir
return|;
block|}
end_function
begin_function
DECL|function|readdir
name|struct
name|dirent
modifier|*
name|readdir
parameter_list|(
name|DIR
modifier|*
name|dir
parameter_list|)
block|{
if|if
condition|(
operator|!
name|dir
condition|)
block|{
name|errno
operator|=
name|EBADF
expr_stmt|;
comment|/* No set_errno for mingw */
return|return
name|NULL
return|;
block|}
comment|/* if first entry, dirent has already been set up by opendir */
if|if
condition|(
name|dir
operator|->
name|dd_stat
condition|)
block|{
comment|/* get next entry and convert from WIN32_FIND_DATA to dirent */
name|WIN32_FIND_DATAA
name|fdata
decl_stmt|;
if|if
condition|(
name|FindNextFileA
argument_list|(
name|dir
operator|->
name|dd_handle
argument_list|,
operator|&
name|fdata
argument_list|)
condition|)
block|{
name|finddata2dirent
argument_list|(
operator|&
name|dir
operator|->
name|dd_dir
argument_list|,
operator|&
name|fdata
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|DWORD
name|lasterr
init|=
name|GetLastError
argument_list|()
decl_stmt|;
comment|/* POSIX says you shouldn't set errno when readdir can't 			   find any more files; so, if another error we leave it set. */
if|if
condition|(
name|lasterr
operator|!=
name|ERROR_NO_MORE_FILES
condition|)
name|errno
operator|=
name|err_win_to_posix
argument_list|(
name|lasterr
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
block|}
operator|++
name|dir
operator|->
name|dd_stat
expr_stmt|;
return|return
operator|&
name|dir
operator|->
name|dd_dir
return|;
block|}
end_function
begin_function
DECL|function|closedir
name|int
name|closedir
parameter_list|(
name|DIR
modifier|*
name|dir
parameter_list|)
block|{
if|if
condition|(
operator|!
name|dir
condition|)
block|{
name|errno
operator|=
name|EBADF
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|FindClose
argument_list|(
name|dir
operator|->
name|dd_handle
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|dir
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
