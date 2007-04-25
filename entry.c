begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_function
DECL|function|create_directories
specifier|static
name|void
name|create_directories
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|struct
name|checkout
modifier|*
name|state
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|path
argument_list|)
decl_stmt|;
name|char
modifier|*
name|buf
init|=
name|xmalloc
argument_list|(
name|len
operator|+
literal|1
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|slash
init|=
name|path
decl_stmt|;
while|while
condition|(
operator|(
name|slash
operator|=
name|strchr
argument_list|(
name|slash
operator|+
literal|1
argument_list|,
literal|'/'
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|len
operator|=
name|slash
operator|-
name|path
expr_stmt|;
name|memcpy
argument_list|(
name|buf
argument_list|,
name|path
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|buf
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|mkdir
argument_list|(
name|buf
argument_list|,
literal|0777
argument_list|)
condition|)
block|{
if|if
condition|(
name|errno
operator|==
name|EEXIST
condition|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
name|len
operator|>
name|state
operator|->
name|base_dir_len
operator|&&
name|state
operator|->
name|force
operator|&&
operator|!
name|unlink
argument_list|(
name|buf
argument_list|)
operator|&&
operator|!
name|mkdir
argument_list|(
name|buf
argument_list|,
literal|0777
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|stat
argument_list|(
name|buf
argument_list|,
operator|&
name|st
argument_list|)
operator|&&
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
continue|continue;
comment|/* ok */
block|}
name|die
argument_list|(
literal|"cannot create directory at %s"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
block|}
block|}
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|remove_subtree
specifier|static
name|void
name|remove_subtree
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|DIR
modifier|*
name|dir
init|=
name|opendir
argument_list|(
name|path
argument_list|)
decl_stmt|;
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
name|char
name|pathbuf
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|char
modifier|*
name|name
decl_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
name|die
argument_list|(
literal|"cannot opendir %s (%s)"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|pathbuf
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|name
operator|=
name|pathbuf
operator|+
name|strlen
argument_list|(
name|path
argument_list|)
expr_stmt|;
operator|*
name|name
operator|++
operator|=
literal|'/'
expr_stmt|;
while|while
condition|(
operator|(
name|de
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
operator|(
name|de
operator|->
name|d_name
index|[
literal|0
index|]
operator|==
literal|'.'
operator|)
operator|&&
operator|(
operator|(
name|de
operator|->
name|d_name
index|[
literal|1
index|]
operator|==
literal|0
operator|)
operator|||
operator|(
operator|(
name|de
operator|->
name|d_name
index|[
literal|1
index|]
operator|==
literal|'.'
operator|)
operator|&&
name|de
operator|->
name|d_name
index|[
literal|2
index|]
operator|==
literal|0
operator|)
operator|)
condition|)
continue|continue;
name|strcpy
argument_list|(
name|name
argument_list|,
name|de
operator|->
name|d_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|pathbuf
argument_list|,
operator|&
name|st
argument_list|)
condition|)
name|die
argument_list|(
literal|"cannot lstat %s (%s)"
argument_list|,
name|pathbuf
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
name|remove_subtree
argument_list|(
name|pathbuf
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|unlink
argument_list|(
name|pathbuf
argument_list|)
condition|)
name|die
argument_list|(
literal|"cannot unlink %s (%s)"
argument_list|,
name|pathbuf
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
if|if
condition|(
name|rmdir
argument_list|(
name|path
argument_list|)
condition|)
name|die
argument_list|(
literal|"cannot rmdir %s (%s)"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|create_file
specifier|static
name|int
name|create_file
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|int
name|mode
parameter_list|)
block|{
name|mode
operator|=
operator|(
name|mode
operator|&
literal|0100
operator|)
condition|?
literal|0777
else|:
literal|0666
expr_stmt|;
return|return
name|open
argument_list|(
name|path
argument_list|,
name|O_WRONLY
operator||
name|O_CREAT
operator||
name|O_EXCL
argument_list|,
name|mode
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|read_blob_entry
specifier|static
name|void
modifier|*
name|read_blob_entry
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
block|{
name|enum
name|object_type
name|type
decl_stmt|;
name|void
modifier|*
name|new
init|=
name|read_sha1_file
argument_list|(
name|ce
operator|->
name|sha1
argument_list|,
operator|&
name|type
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
name|new
condition|)
block|{
if|if
condition|(
name|type
operator|==
name|OBJ_BLOB
condition|)
return|return
name|new
return|;
name|free
argument_list|(
name|new
argument_list|)
expr_stmt|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|write_entry
specifier|static
name|int
name|write_entry
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|struct
name|checkout
modifier|*
name|state
parameter_list|,
name|int
name|to_tempfile
parameter_list|)
block|{
name|int
name|fd
decl_stmt|;
name|long
name|wrote
decl_stmt|;
switch|switch
condition|(
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
operator|&
name|S_IFMT
condition|)
block|{
name|char
modifier|*
name|buf
decl_stmt|,
modifier|*
name|new
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
case|case
name|S_IFREG
case|:
name|new
operator|=
name|read_blob_entry
argument_list|(
name|ce
argument_list|,
name|path
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|new
condition|)
return|return
name|error
argument_list|(
literal|"git-checkout-index: unable to read sha1 file of %s (%s)"
argument_list|,
name|path
argument_list|,
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|to_tempfile
condition|)
block|{
name|strcpy
argument_list|(
name|path
argument_list|,
literal|".merge_file_XXXXXX"
argument_list|)
expr_stmt|;
name|fd
operator|=
name|mkstemp
argument_list|(
name|path
argument_list|)
expr_stmt|;
block|}
else|else
name|fd
operator|=
name|create_file
argument_list|(
name|path
argument_list|,
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|free
argument_list|(
name|new
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"git-checkout-index: unable to create file %s (%s)"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
block|}
comment|/* 		 * Convert from git internal format to working tree format 		 */
name|buf
operator|=
name|convert_to_working_tree
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|new
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
name|buf
condition|)
block|{
name|free
argument_list|(
name|new
argument_list|)
expr_stmt|;
name|new
operator|=
name|buf
expr_stmt|;
block|}
name|wrote
operator|=
name|write_in_full
argument_list|(
name|fd
argument_list|,
name|new
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|new
argument_list|)
expr_stmt|;
if|if
condition|(
name|wrote
operator|!=
name|size
condition|)
return|return
name|error
argument_list|(
literal|"git-checkout-index: unable to write file %s"
argument_list|,
name|path
argument_list|)
return|;
break|break;
case|case
name|S_IFLNK
case|:
name|new
operator|=
name|read_blob_entry
argument_list|(
name|ce
argument_list|,
name|path
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|new
condition|)
return|return
name|error
argument_list|(
literal|"git-checkout-index: unable to read sha1 file of %s (%s)"
argument_list|,
name|path
argument_list|,
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|to_tempfile
operator|||
operator|!
name|has_symlinks
condition|)
block|{
if|if
condition|(
name|to_tempfile
condition|)
block|{
name|strcpy
argument_list|(
name|path
argument_list|,
literal|".merge_link_XXXXXX"
argument_list|)
expr_stmt|;
name|fd
operator|=
name|mkstemp
argument_list|(
name|path
argument_list|)
expr_stmt|;
block|}
else|else
name|fd
operator|=
name|create_file
argument_list|(
name|path
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|free
argument_list|(
name|new
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"git-checkout-index: unable to create "
literal|"file %s (%s)"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
block|}
name|wrote
operator|=
name|write_in_full
argument_list|(
name|fd
argument_list|,
name|new
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|new
argument_list|)
expr_stmt|;
if|if
condition|(
name|wrote
operator|!=
name|size
condition|)
return|return
name|error
argument_list|(
literal|"git-checkout-index: unable to write file %s"
argument_list|,
name|path
argument_list|)
return|;
block|}
else|else
block|{
name|wrote
operator|=
name|symlink
argument_list|(
name|new
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|new
argument_list|)
expr_stmt|;
if|if
condition|(
name|wrote
condition|)
return|return
name|error
argument_list|(
literal|"git-checkout-index: unable to create "
literal|"symlink %s (%s)"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
block|}
break|break;
case|case
name|S_IFDIRLNK
case|:
if|if
condition|(
name|to_tempfile
condition|)
return|return
name|error
argument_list|(
literal|"git-checkout-index: cannot create temporary subproject %s"
argument_list|,
name|path
argument_list|)
return|;
if|if
condition|(
name|mkdir
argument_list|(
name|path
argument_list|,
literal|0777
argument_list|)
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"git-checkout-index: cannot create subproject directory %s"
argument_list|,
name|path
argument_list|)
return|;
break|break;
default|default:
return|return
name|error
argument_list|(
literal|"git-checkout-index: unknown file mode for %s"
argument_list|,
name|path
argument_list|)
return|;
block|}
if|if
condition|(
name|state
operator|->
name|refresh_cache
condition|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
name|lstat
argument_list|(
name|ce
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
name|fill_stat_cache_info
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|checkout_entry
name|int
name|checkout_entry
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
specifier|const
name|struct
name|checkout
modifier|*
name|state
parameter_list|,
name|char
modifier|*
name|topath
parameter_list|)
block|{
specifier|static
name|char
name|path
index|[
name|PATH_MAX
operator|+
literal|1
index|]
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|int
name|len
init|=
name|state
operator|->
name|base_dir_len
decl_stmt|;
if|if
condition|(
name|topath
condition|)
return|return
name|write_entry
argument_list|(
name|ce
argument_list|,
name|topath
argument_list|,
name|state
argument_list|,
literal|1
argument_list|)
return|;
name|memcpy
argument_list|(
name|path
argument_list|,
name|state
operator|->
name|base_dir
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|path
operator|+
name|len
argument_list|,
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|lstat
argument_list|(
name|path
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
name|unsigned
name|changed
init|=
name|ce_match_stat
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|,
literal|1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|changed
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|state
operator|->
name|force
condition|)
block|{
if|if
condition|(
operator|!
name|state
operator|->
name|quiet
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"git-checkout-index: %s already exists\n"
argument_list|,
name|path
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
comment|/* 		 * We unlink the old file, to get the new one with the 		 * right permissions (including umask, which is nasty 		 * to emulate by hand - much easier to let the system 		 * just do the right thing) 		 */
name|unlink
argument_list|(
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
comment|/* If it is a gitlink, leave it alone! */
if|if
condition|(
name|S_ISDIRLNK
argument_list|(
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|state
operator|->
name|force
condition|)
return|return
name|error
argument_list|(
literal|"%s is a directory"
argument_list|,
name|path
argument_list|)
return|;
name|remove_subtree
argument_list|(
name|path
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|state
operator|->
name|not_new
condition|)
return|return
literal|0
return|;
name|create_directories
argument_list|(
name|path
argument_list|,
name|state
argument_list|)
expr_stmt|;
return|return
name|write_entry
argument_list|(
name|ce
argument_list|,
name|path
argument_list|,
name|state
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
end_unit
