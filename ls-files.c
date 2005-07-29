begin_unit
begin_comment
comment|/*  * This merges the file listing in the directory cache index  * with the actual working directory list, and shows different  * combinations of the two.  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|<dirent.h>
end_include
begin_include
include|#
directive|include
file|<fnmatch.h>
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|show_deleted
specifier|static
name|int
name|show_deleted
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_cached
specifier|static
name|int
name|show_cached
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_others
specifier|static
name|int
name|show_others
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_ignored
specifier|static
name|int
name|show_ignored
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_stage
specifier|static
name|int
name|show_stage
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_unmerged
specifier|static
name|int
name|show_unmerged
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_killed
specifier|static
name|int
name|show_killed
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|line_terminator
specifier|static
name|int
name|line_terminator
init|=
literal|'\n'
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|tag_cached
specifier|static
specifier|const
name|char
modifier|*
name|tag_cached
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|tag_unmerged
specifier|static
specifier|const
name|char
modifier|*
name|tag_unmerged
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|tag_removed
specifier|static
specifier|const
name|char
modifier|*
name|tag_removed
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|tag_other
specifier|static
specifier|const
name|char
modifier|*
name|tag_other
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|tag_killed
specifier|static
specifier|const
name|char
modifier|*
name|tag_killed
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|exclude_per_dir
specifier|static
name|char
modifier|*
name|exclude_per_dir
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* We maintain three exclude pattern lists:  * EXC_CMDL lists patterns explicitly given on the command line.  * EXC_DIRS lists patterns obtained from per-directory ignore files.  * EXC_FILE lists patterns from fallback ignore files.  */
end_comment
begin_define
DECL|macro|EXC_CMDL
define|#
directive|define
name|EXC_CMDL
value|0
end_define
begin_define
DECL|macro|EXC_DIRS
define|#
directive|define
name|EXC_DIRS
value|1
end_define
begin_define
DECL|macro|EXC_FILE
define|#
directive|define
name|EXC_FILE
value|2
end_define
begin_struct
DECL|struct|exclude_list
specifier|static
struct|struct
name|exclude_list
block|{
DECL|member|nr
name|int
name|nr
decl_stmt|;
DECL|member|alloc
name|int
name|alloc
decl_stmt|;
DECL|struct|exclude
struct|struct
name|exclude
block|{
DECL|member|pattern
specifier|const
name|char
modifier|*
name|pattern
decl_stmt|;
DECL|member|base
specifier|const
name|char
modifier|*
name|base
decl_stmt|;
DECL|member|baselen
name|int
name|baselen
decl_stmt|;
block|}
DECL|member|excludes
modifier|*
modifier|*
name|excludes
struct|;
block|}
DECL|variable|exclude_list
name|exclude_list
index|[
literal|3
index|]
struct|;
end_struct
begin_function
DECL|function|add_exclude
specifier|static
name|void
name|add_exclude
parameter_list|(
specifier|const
name|char
modifier|*
name|string
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
name|struct
name|exclude_list
modifier|*
name|which
parameter_list|)
block|{
name|struct
name|exclude
modifier|*
name|x
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|x
argument_list|)
argument_list|)
decl_stmt|;
name|x
operator|->
name|pattern
operator|=
name|string
expr_stmt|;
name|x
operator|->
name|base
operator|=
name|base
expr_stmt|;
name|x
operator|->
name|baselen
operator|=
name|baselen
expr_stmt|;
if|if
condition|(
name|which
operator|->
name|nr
operator|==
name|which
operator|->
name|alloc
condition|)
block|{
name|which
operator|->
name|alloc
operator|=
name|alloc_nr
argument_list|(
name|which
operator|->
name|alloc
argument_list|)
expr_stmt|;
name|which
operator|->
name|excludes
operator|=
name|realloc
argument_list|(
name|which
operator|->
name|excludes
argument_list|,
name|which
operator|->
name|alloc
operator|*
sizeof|sizeof
argument_list|(
name|x
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|which
operator|->
name|excludes
index|[
name|which
operator|->
name|nr
operator|++
index|]
operator|=
name|x
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_excludes_from_file_1
specifier|static
name|int
name|add_excludes_from_file_1
parameter_list|(
specifier|const
name|char
modifier|*
name|fname
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
name|struct
name|exclude_list
modifier|*
name|which
parameter_list|)
block|{
name|int
name|fd
decl_stmt|,
name|i
decl_stmt|;
name|long
name|size
decl_stmt|;
name|char
modifier|*
name|buf
decl_stmt|,
modifier|*
name|entry
decl_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|fname
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
goto|goto
name|err
goto|;
name|size
operator|=
name|lseek
argument_list|(
name|fd
argument_list|,
literal|0
argument_list|,
name|SEEK_END
argument_list|)
expr_stmt|;
if|if
condition|(
name|size
operator|<
literal|0
condition|)
goto|goto
name|err
goto|;
name|lseek
argument_list|(
name|fd
argument_list|,
literal|0
argument_list|,
name|SEEK_SET
argument_list|)
expr_stmt|;
if|if
condition|(
name|size
operator|==
literal|0
condition|)
block|{
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|buf
operator|=
name|xmalloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
name|read
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
operator|!=
name|size
condition|)
goto|goto
name|err
goto|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|entry
operator|=
name|buf
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|size
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|buf
index|[
name|i
index|]
operator|==
literal|'\n'
condition|)
block|{
if|if
condition|(
name|entry
operator|!=
name|buf
operator|+
name|i
operator|&&
name|entry
index|[
literal|0
index|]
operator|!=
literal|'#'
condition|)
block|{
name|buf
index|[
name|i
index|]
operator|=
literal|0
expr_stmt|;
name|add_exclude
argument_list|(
name|entry
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|which
argument_list|)
expr_stmt|;
block|}
name|entry
operator|=
name|buf
operator|+
name|i
operator|+
literal|1
expr_stmt|;
block|}
block|}
return|return
literal|0
return|;
name|err
label|:
if|if
condition|(
literal|0
operator|<=
name|fd
condition|)
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|add_excludes_from_file
specifier|static
name|void
name|add_excludes_from_file
parameter_list|(
specifier|const
name|char
modifier|*
name|fname
parameter_list|)
block|{
if|if
condition|(
name|add_excludes_from_file_1
argument_list|(
name|fname
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
operator|&
name|exclude_list
index|[
name|EXC_FILE
index|]
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"cannot use %s as an exclude file"
argument_list|,
name|fname
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|push_exclude_per_directory
specifier|static
name|int
name|push_exclude_per_directory
parameter_list|(
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|)
block|{
name|char
name|exclude_file
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|struct
name|exclude_list
modifier|*
name|el
init|=
operator|&
name|exclude_list
index|[
name|EXC_DIRS
index|]
decl_stmt|;
name|int
name|current_nr
init|=
name|el
operator|->
name|nr
decl_stmt|;
if|if
condition|(
name|exclude_per_dir
condition|)
block|{
name|memcpy
argument_list|(
name|exclude_file
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|exclude_file
operator|+
name|baselen
argument_list|,
name|exclude_per_dir
argument_list|)
expr_stmt|;
name|add_excludes_from_file_1
argument_list|(
name|exclude_file
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|el
argument_list|)
expr_stmt|;
block|}
return|return
name|current_nr
return|;
block|}
end_function
begin_function
DECL|function|pop_exclude_per_directory
specifier|static
name|void
name|pop_exclude_per_directory
parameter_list|(
name|int
name|stk
parameter_list|)
block|{
name|struct
name|exclude_list
modifier|*
name|el
init|=
operator|&
name|exclude_list
index|[
name|EXC_DIRS
index|]
decl_stmt|;
while|while
condition|(
name|stk
operator|<
name|el
operator|->
name|nr
condition|)
name|free
argument_list|(
name|el
operator|->
name|excludes
index|[
operator|--
name|el
operator|->
name|nr
index|]
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Scan the list and let the last match determines the fate.  * Return 1 for exclude, 0 for include and -1 for undecided.  */
end_comment
begin_function
DECL|function|excluded_1
specifier|static
name|int
name|excluded_1
parameter_list|(
specifier|const
name|char
modifier|*
name|pathname
parameter_list|,
name|int
name|pathlen
parameter_list|,
name|struct
name|exclude_list
modifier|*
name|el
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
if|if
condition|(
name|el
operator|->
name|nr
condition|)
block|{
for|for
control|(
name|i
operator|=
name|el
operator|->
name|nr
operator|-
literal|1
init|;
literal|0
operator|<=
name|i
condition|;
name|i
operator|--
control|)
block|{
name|struct
name|exclude
modifier|*
name|x
init|=
name|el
operator|->
name|excludes
index|[
name|i
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|exclude
init|=
name|x
operator|->
name|pattern
decl_stmt|;
name|int
name|to_exclude
init|=
literal|1
decl_stmt|;
if|if
condition|(
operator|*
name|exclude
operator|==
literal|'!'
condition|)
block|{
name|to_exclude
operator|=
literal|0
expr_stmt|;
name|exclude
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|strchr
argument_list|(
name|exclude
argument_list|,
literal|'/'
argument_list|)
condition|)
block|{
comment|/* match basename */
specifier|const
name|char
modifier|*
name|basename
init|=
name|strrchr
argument_list|(
name|pathname
argument_list|,
literal|'/'
argument_list|)
decl_stmt|;
name|basename
operator|=
operator|(
name|basename
operator|)
condition|?
name|basename
operator|+
literal|1
else|:
name|pathname
expr_stmt|;
if|if
condition|(
name|fnmatch
argument_list|(
name|exclude
argument_list|,
name|basename
argument_list|,
literal|0
argument_list|)
operator|==
literal|0
condition|)
return|return
name|to_exclude
return|;
block|}
else|else
block|{
comment|/* match with FNM_PATHNAME: 				 * exclude has base (baselen long) inplicitly 				 * in front of it. 				 */
name|int
name|baselen
init|=
name|x
operator|->
name|baselen
decl_stmt|;
if|if
condition|(
operator|*
name|exclude
operator|==
literal|'/'
condition|)
name|exclude
operator|++
expr_stmt|;
if|if
condition|(
name|pathlen
operator|<
name|baselen
operator|||
operator|(
name|baselen
operator|&&
name|pathname
index|[
name|baselen
operator|-
literal|1
index|]
operator|!=
literal|'/'
operator|)
operator|||
name|strncmp
argument_list|(
name|pathname
argument_list|,
name|x
operator|->
name|base
argument_list|,
name|baselen
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|fnmatch
argument_list|(
name|exclude
argument_list|,
name|pathname
operator|+
name|baselen
argument_list|,
name|FNM_PATHNAME
argument_list|)
operator|==
literal|0
condition|)
return|return
name|to_exclude
return|;
block|}
block|}
block|}
return|return
operator|-
literal|1
return|;
comment|/* undecided */
block|}
end_function
begin_function
DECL|function|excluded
specifier|static
name|int
name|excluded
parameter_list|(
specifier|const
name|char
modifier|*
name|pathname
parameter_list|)
block|{
name|int
name|pathlen
init|=
name|strlen
argument_list|(
name|pathname
argument_list|)
decl_stmt|;
name|int
name|st
decl_stmt|;
for|for
control|(
name|st
operator|=
name|EXC_CMDL
init|;
name|st
operator|<=
name|EXC_FILE
condition|;
name|st
operator|++
control|)
block|{
switch|switch
condition|(
name|excluded_1
argument_list|(
name|pathname
argument_list|,
name|pathlen
argument_list|,
operator|&
name|exclude_list
index|[
name|st
index|]
argument_list|)
condition|)
block|{
case|case
literal|0
case|:
return|return
literal|0
return|;
case|case
literal|1
case|:
return|return
literal|1
return|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
begin_struct
DECL|struct|nond_on_fs
struct|struct
name|nond_on_fs
block|{
DECL|member|len
name|int
name|len
decl_stmt|;
DECL|member|name
name|char
name|name
index|[
literal|0
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|dir
specifier|static
name|struct
name|nond_on_fs
modifier|*
modifier|*
name|dir
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|nr_dir
specifier|static
name|int
name|nr_dir
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|dir_alloc
specifier|static
name|int
name|dir_alloc
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_name
specifier|static
name|void
name|add_name
parameter_list|(
specifier|const
name|char
modifier|*
name|pathname
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|struct
name|nond_on_fs
modifier|*
name|ent
decl_stmt|;
if|if
condition|(
name|cache_name_pos
argument_list|(
name|pathname
argument_list|,
name|len
argument_list|)
operator|>=
literal|0
condition|)
return|return;
if|if
condition|(
name|nr_dir
operator|==
name|dir_alloc
condition|)
block|{
name|dir_alloc
operator|=
name|alloc_nr
argument_list|(
name|dir_alloc
argument_list|)
expr_stmt|;
name|dir
operator|=
name|xrealloc
argument_list|(
name|dir
argument_list|,
name|dir_alloc
operator|*
sizeof|sizeof
argument_list|(
name|ent
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|ent
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|ent
argument_list|)
operator|+
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
name|ent
operator|->
name|len
operator|=
name|len
expr_stmt|;
name|memcpy
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|pathname
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|dir
index|[
name|nr_dir
operator|++
index|]
operator|=
name|ent
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Read a directory tree. We currently ignore anything but  * directories, regular files and symlinks. That's because git  * doesn't handle them at all yet. Maybe that will change some  * day.  *  * Also, we ignore the name ".git" (even if it is not a directory).  * That likely will not change.  */
end_comment
begin_function
DECL|function|read_directory
specifier|static
name|void
name|read_directory
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
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
if|if
condition|(
name|dir
condition|)
block|{
name|int
name|exclude_stk
decl_stmt|;
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
name|char
name|fullname
index|[
name|MAXPATHLEN
operator|+
literal|1
index|]
decl_stmt|;
name|memcpy
argument_list|(
name|fullname
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|exclude_stk
operator|=
name|push_exclude_per_directory
argument_list|(
name|base
argument_list|,
name|baselen
argument_list|)
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
name|int
name|len
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
name|de
operator|->
name|d_name
index|[
literal|1
index|]
operator|==
literal|0
operator|||
operator|!
name|strcmp
argument_list|(
name|de
operator|->
name|d_name
operator|+
literal|1
argument_list|,
literal|"."
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|de
operator|->
name|d_name
operator|+
literal|1
argument_list|,
literal|"git"
argument_list|)
operator|)
condition|)
continue|continue;
name|len
operator|=
name|strlen
argument_list|(
name|de
operator|->
name|d_name
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|fullname
operator|+
name|baselen
argument_list|,
name|de
operator|->
name|d_name
argument_list|,
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|excluded
argument_list|(
name|fullname
argument_list|)
operator|!=
name|show_ignored
condition|)
continue|continue;
switch|switch
condition|(
name|DTYPE
argument_list|(
name|de
argument_list|)
condition|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
default|default:
continue|continue;
case|case
name|DT_UNKNOWN
case|:
if|if
condition|(
name|lstat
argument_list|(
name|fullname
argument_list|,
operator|&
name|st
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
operator|||
name|S_ISLNK
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
break|break;
if|if
condition|(
operator|!
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
continue|continue;
comment|/* fallthrough */
case|case
name|DT_DIR
case|:
name|memcpy
argument_list|(
name|fullname
operator|+
name|baselen
operator|+
name|len
argument_list|,
literal|"/"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|read_directory
argument_list|(
name|fullname
argument_list|,
name|fullname
argument_list|,
name|baselen
operator|+
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
continue|continue;
case|case
name|DT_REG
case|:
case|case
name|DT_LNK
case|:
break|break;
block|}
name|add_name
argument_list|(
name|fullname
argument_list|,
name|baselen
operator|+
name|len
argument_list|)
expr_stmt|;
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
name|pop_exclude_per_directory
argument_list|(
name|exclude_stk
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|cmp_name
specifier|static
name|int
name|cmp_name
parameter_list|(
specifier|const
name|void
modifier|*
name|p1
parameter_list|,
specifier|const
name|void
modifier|*
name|p2
parameter_list|)
block|{
specifier|const
name|struct
name|nond_on_fs
modifier|*
name|e1
init|=
operator|*
operator|(
specifier|const
expr|struct
name|nond_on_fs
operator|*
operator|*
operator|)
name|p1
decl_stmt|;
specifier|const
name|struct
name|nond_on_fs
modifier|*
name|e2
init|=
operator|*
operator|(
specifier|const
expr|struct
name|nond_on_fs
operator|*
operator|*
operator|)
name|p2
decl_stmt|;
return|return
name|cache_name_compare
argument_list|(
name|e1
operator|->
name|name
argument_list|,
name|e1
operator|->
name|len
argument_list|,
name|e2
operator|->
name|name
argument_list|,
name|e2
operator|->
name|len
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|show_killed_files
specifier|static
name|void
name|show_killed_files
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_dir
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|nond_on_fs
modifier|*
name|ent
init|=
name|dir
index|[
name|i
index|]
decl_stmt|;
name|char
modifier|*
name|cp
decl_stmt|,
modifier|*
name|sp
decl_stmt|;
name|int
name|pos
decl_stmt|,
name|len
decl_stmt|,
name|killed
init|=
literal|0
decl_stmt|;
for|for
control|(
name|cp
operator|=
name|ent
operator|->
name|name
init|;
name|cp
operator|-
name|ent
operator|->
name|name
operator|<
name|ent
operator|->
name|len
condition|;
name|cp
operator|=
name|sp
operator|+
literal|1
control|)
block|{
name|sp
operator|=
name|strchr
argument_list|(
name|cp
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|sp
condition|)
block|{
comment|/* If ent->name is prefix of an entry in the 				 * cache, it will be killed. 				 */
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|ent
operator|->
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|pos
condition|)
name|die
argument_list|(
literal|"bug in show-killed-files"
argument_list|)
expr_stmt|;
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
while|while
condition|(
name|pos
operator|<
name|active_nr
operator|&&
name|ce_stage
argument_list|(
name|active_cache
index|[
name|pos
index|]
argument_list|)
condition|)
name|pos
operator|++
expr_stmt|;
comment|/* skip unmerged */
if|if
condition|(
name|active_nr
operator|<=
name|pos
condition|)
break|break;
comment|/* pos points at a name immediately after 				 * ent->name in the cache.  Does it expect 				 * ent->name to be a directory? 				 */
name|len
operator|=
name|ce_namelen
argument_list|(
name|active_cache
index|[
name|pos
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|ent
operator|->
name|len
operator|<
name|len
operator|)
operator|&&
operator|!
name|strncmp
argument_list|(
name|active_cache
index|[
name|pos
index|]
operator|->
name|name
argument_list|,
name|ent
operator|->
name|name
argument_list|,
name|ent
operator|->
name|len
argument_list|)
operator|&&
name|active_cache
index|[
name|pos
index|]
operator|->
name|name
index|[
name|ent
operator|->
name|len
index|]
operator|==
literal|'/'
condition|)
name|killed
operator|=
literal|1
expr_stmt|;
break|break;
block|}
if|if
condition|(
literal|0
operator|<=
name|cache_name_pos
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|sp
operator|-
name|ent
operator|->
name|name
argument_list|)
condition|)
block|{
comment|/* If any of the leading directories in 				 * ent->name is registered in the cache, 				 * ent->name will be killed. 				 */
name|killed
operator|=
literal|1
expr_stmt|;
break|break;
block|}
block|}
if|if
condition|(
name|killed
condition|)
name|printf
argument_list|(
literal|"%s%.*s%c"
argument_list|,
name|tag_killed
argument_list|,
name|dir
index|[
name|i
index|]
operator|->
name|len
argument_list|,
name|dir
index|[
name|i
index|]
operator|->
name|name
argument_list|,
name|line_terminator
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|show_files
specifier|static
name|void
name|show_files
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
comment|/* For cached/deleted files we don't need to even do the readdir */
if|if
condition|(
name|show_others
operator|||
name|show_killed
condition|)
block|{
name|read_directory
argument_list|(
literal|"."
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|qsort
argument_list|(
name|dir
argument_list|,
name|nr_dir
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|nond_on_fs
operator|*
argument_list|)
argument_list|,
name|cmp_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_others
condition|)
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_dir
condition|;
name|i
operator|++
control|)
name|printf
argument_list|(
literal|"%s%.*s%c"
argument_list|,
name|tag_other
argument_list|,
name|dir
index|[
name|i
index|]
operator|->
name|len
argument_list|,
name|dir
index|[
name|i
index|]
operator|->
name|name
argument_list|,
name|line_terminator
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_killed
condition|)
name|show_killed_files
argument_list|()
expr_stmt|;
block|}
if|if
condition|(
name|show_cached
operator||
name|show_stage
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|excluded
argument_list|(
name|ce
operator|->
name|name
argument_list|)
operator|!=
name|show_ignored
condition|)
continue|continue;
if|if
condition|(
name|show_unmerged
operator|&&
operator|!
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|show_stage
condition|)
name|printf
argument_list|(
literal|"%s%s%c"
argument_list|,
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|?
name|tag_unmerged
else|:
name|tag_cached
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|line_terminator
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"%s%06o %s %d\t%s%c"
argument_list|,
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|?
name|tag_unmerged
else|:
name|tag_cached
argument_list|,
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
argument_list|,
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|,
name|ce_stage
argument_list|(
name|ce
argument_list|)
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|line_terminator
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|show_deleted
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
name|excluded
argument_list|(
name|ce
operator|->
name|name
argument_list|)
operator|!=
name|show_ignored
condition|)
continue|continue;
if|if
condition|(
operator|!
name|lstat
argument_list|(
name|ce
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
condition|)
continue|continue;
name|printf
argument_list|(
literal|"%s%s%c"
argument_list|,
name|tag_removed
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|line_terminator
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_decl_stmt
DECL|variable|ls_files_usage
specifier|static
specifier|const
name|char
modifier|*
name|ls_files_usage
init|=
literal|"git-ls-files [-z] [-t] (--[cached|deleted|others|stage|unmerged|killed])* "
literal|"[ --ignored ] [--exclude=<pattern>] [--exclude-from=<file>] "
literal|"[ --exclude-per-directory=<filename> ]"
decl_stmt|;
end_decl_stmt
begin_empty_stmt
empty_stmt|;
end_empty_stmt
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|exc_given
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-z"
argument_list|)
condition|)
block|{
name|line_terminator
operator|=
literal|0
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-t"
argument_list|)
condition|)
block|{
name|tag_cached
operator|=
literal|"H "
expr_stmt|;
name|tag_unmerged
operator|=
literal|"M "
expr_stmt|;
name|tag_removed
operator|=
literal|"R "
expr_stmt|;
name|tag_other
operator|=
literal|"? "
expr_stmt|;
name|tag_killed
operator|=
literal|"K "
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-c"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--cached"
argument_list|)
condition|)
block|{
name|show_cached
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-d"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--deleted"
argument_list|)
condition|)
block|{
name|show_deleted
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-o"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--others"
argument_list|)
condition|)
block|{
name|show_others
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-i"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--ignored"
argument_list|)
condition|)
block|{
name|show_ignored
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-s"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--stage"
argument_list|)
condition|)
block|{
name|show_stage
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-k"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--killed"
argument_list|)
condition|)
block|{
name|show_killed
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-u"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--unmerged"
argument_list|)
condition|)
block|{
comment|/* There's no point in showing unmerged unless 			 * you also show the stage information. 			 */
name|show_stage
operator|=
literal|1
expr_stmt|;
name|show_unmerged
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-x"
argument_list|)
operator|&&
name|i
operator|+
literal|1
operator|<
name|argc
condition|)
block|{
name|exc_given
operator|=
literal|1
expr_stmt|;
name|add_exclude
argument_list|(
name|argv
index|[
operator|++
name|i
index|]
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
operator|&
name|exclude_list
index|[
name|EXC_CMDL
index|]
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--exclude="
argument_list|,
literal|10
argument_list|)
condition|)
block|{
name|exc_given
operator|=
literal|1
expr_stmt|;
name|add_exclude
argument_list|(
name|arg
operator|+
literal|10
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
operator|&
name|exclude_list
index|[
name|EXC_CMDL
index|]
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-X"
argument_list|)
operator|&&
name|i
operator|+
literal|1
operator|<
name|argc
condition|)
block|{
name|exc_given
operator|=
literal|1
expr_stmt|;
name|add_excludes_from_file
argument_list|(
name|argv
index|[
operator|++
name|i
index|]
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--exclude-from="
argument_list|,
literal|15
argument_list|)
condition|)
block|{
name|exc_given
operator|=
literal|1
expr_stmt|;
name|add_excludes_from_file
argument_list|(
name|arg
operator|+
literal|15
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--exclude-per-directory="
argument_list|,
literal|24
argument_list|)
condition|)
block|{
name|exc_given
operator|=
literal|1
expr_stmt|;
name|exclude_per_dir
operator|=
name|arg
operator|+
literal|24
expr_stmt|;
block|}
else|else
name|usage
argument_list|(
name|ls_files_usage
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|show_ignored
operator|&&
operator|!
name|exc_given
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s: --ignored needs some exclude pattern\n"
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
comment|/* With no flags, we default to showing the cached files */
if|if
condition|(
operator|!
operator|(
name|show_stage
operator||
name|show_deleted
operator||
name|show_others
operator||
name|show_unmerged
operator||
name|show_killed
operator|)
condition|)
name|show_cached
operator|=
literal|1
expr_stmt|;
name|read_cache
argument_list|()
expr_stmt|;
name|show_files
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
