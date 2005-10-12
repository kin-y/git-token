begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_comment
comment|/*  * Default to not allowing changes to the list of files. The  * tool doesn't actually care, but this makes it harder to add  * files to the revision control by mistake by doing something  * like "git-update-index *" and suddenly having all the object  * files be revision controlled.  */
end_comment
begin_decl_stmt
DECL|variable|allow_add
DECL|variable|allow_remove
DECL|variable|allow_replace
DECL|variable|allow_unmerged
DECL|variable|not_new
DECL|variable|quiet
DECL|variable|info_only
specifier|static
name|int
name|allow_add
init|=
literal|0
decl_stmt|,
name|allow_remove
init|=
literal|0
decl_stmt|,
name|allow_replace
init|=
literal|0
decl_stmt|,
name|allow_unmerged
init|=
literal|0
decl_stmt|,
name|not_new
init|=
literal|0
decl_stmt|,
name|quiet
init|=
literal|0
decl_stmt|,
name|info_only
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|force_remove
specifier|static
name|int
name|force_remove
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* Three functions to allow overloaded pointer return; see linux/err.h */
end_comment
begin_function
DECL|function|ERR_PTR
specifier|static
specifier|inline
name|void
modifier|*
name|ERR_PTR
parameter_list|(
name|long
name|error
parameter_list|)
block|{
return|return
operator|(
name|void
operator|*
operator|)
name|error
return|;
block|}
end_function
begin_function
DECL|function|PTR_ERR
specifier|static
specifier|inline
name|long
name|PTR_ERR
parameter_list|(
specifier|const
name|void
modifier|*
name|ptr
parameter_list|)
block|{
return|return
operator|(
name|long
operator|)
name|ptr
return|;
block|}
end_function
begin_function
DECL|function|IS_ERR
specifier|static
specifier|inline
name|long
name|IS_ERR
parameter_list|(
specifier|const
name|void
modifier|*
name|ptr
parameter_list|)
block|{
return|return
operator|(
name|unsigned
name|long
operator|)
name|ptr
operator|>
operator|(
name|unsigned
name|long
operator|)
operator|-
literal|1000L
return|;
block|}
end_function
begin_function
DECL|function|add_file_to_cache
specifier|static
name|int
name|add_file_to_cache
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|size
decl_stmt|,
name|namelen
decl_stmt|,
name|option
decl_stmt|,
name|status
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|status
operator|=
name|lstat
argument_list|(
name|path
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
if|if
condition|(
name|status
operator|<
literal|0
operator|||
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
comment|/* When we used to have "path" and now we want to add 		 * "path/file", we need a way to remove "path" before 		 * being able to add "path/file".  However, 		 * "git-update-index --remove path" would not work. 		 * --force-remove can be used but this is more user 		 * friendly, especially since we can do the opposite 		 * case just fine without --force-remove. 		 */
if|if
condition|(
name|status
operator|==
literal|0
operator|||
operator|(
name|errno
operator|==
name|ENOENT
operator|||
name|errno
operator|==
name|ENOTDIR
operator|)
condition|)
block|{
if|if
condition|(
name|allow_remove
condition|)
block|{
if|if
condition|(
name|remove_file_from_cache
argument_list|(
name|path
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"%s: cannot remove from the index"
argument_list|,
name|path
argument_list|)
return|;
else|else
return|return
literal|0
return|;
block|}
elseif|else
if|if
condition|(
name|status
operator|<
literal|0
condition|)
block|{
return|return
name|error
argument_list|(
literal|"%s: does not exist and --remove not passed"
argument_list|,
name|path
argument_list|)
return|;
block|}
block|}
if|if
condition|(
literal|0
operator|==
name|status
condition|)
return|return
name|error
argument_list|(
literal|"%s: is a directory - add files inside instead"
argument_list|,
name|path
argument_list|)
return|;
else|else
return|return
name|error
argument_list|(
literal|"lstat(\"%s\"): %s"
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
name|namelen
operator|=
name|strlen
argument_list|(
name|path
argument_list|)
expr_stmt|;
name|size
operator|=
name|cache_entry_size
argument_list|(
name|namelen
argument_list|)
expr_stmt|;
name|ce
operator|=
name|xmalloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|ce
argument_list|,
literal|0
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|path
argument_list|,
name|namelen
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
name|ce
operator|->
name|ce_mode
operator|=
name|create_ce_mode
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|trust_executable_bit
condition|)
block|{
comment|/* If there is an existing entry, pick the mode bits 		 * from it. 		 */
name|int
name|pos
init|=
name|cache_name_pos
argument_list|(
name|path
argument_list|,
name|namelen
argument_list|)
decl_stmt|;
if|if
condition|(
literal|0
operator|<=
name|pos
condition|)
name|ce
operator|->
name|ce_mode
operator|=
name|active_cache
index|[
name|pos
index|]
operator|->
name|ce_mode
expr_stmt|;
block|}
name|ce
operator|->
name|ce_flags
operator|=
name|htons
argument_list|(
name|namelen
argument_list|)
expr_stmt|;
if|if
condition|(
name|index_path
argument_list|(
name|ce
operator|->
name|sha1
argument_list|,
name|path
argument_list|,
operator|&
name|st
argument_list|,
operator|!
name|info_only
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|option
operator|=
name|allow_add
condition|?
name|ADD_CACHE_OK_TO_ADD
else|:
literal|0
expr_stmt|;
name|option
operator||=
name|allow_replace
condition|?
name|ADD_CACHE_OK_TO_REPLACE
else|:
literal|0
expr_stmt|;
if|if
condition|(
name|add_cache_entry
argument_list|(
name|ce
argument_list|,
name|option
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"%s: cannot add to the index - missing --add option?"
argument_list|,
name|path
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * "refresh" does not calculate a new sha1 file or bring the  * cache up-to-date for mode/content changes. But what it  * _does_ do is to "re-match" the stat information of a file  * with the cache, so that you can refresh the cache for a  * file that hasn't been changed but where the stat entry is  * out of date.  *  * For example, you'd want to do this after doing a "git-read-tree",  * to link up the stat cache details with the proper files.  */
end_comment
begin_function
DECL|function|refresh_entry
specifier|static
name|struct
name|cache_entry
modifier|*
name|refresh_entry
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|updated
decl_stmt|;
name|int
name|changed
decl_stmt|,
name|size
decl_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|ce
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
return|return
name|ERR_PTR
argument_list|(
operator|-
name|errno
argument_list|)
return|;
name|changed
operator|=
name|ce_match_stat
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|changed
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|ce_modified
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|)
condition|)
return|return
name|ERR_PTR
argument_list|(
operator|-
name|EINVAL
argument_list|)
return|;
name|size
operator|=
name|ce_size
argument_list|(
name|ce
argument_list|)
expr_stmt|;
name|updated
operator|=
name|xmalloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|updated
argument_list|,
name|ce
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|fill_stat_cache_info
argument_list|(
name|updated
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
return|return
name|updated
return|;
block|}
end_function
begin_function
DECL|function|refresh_cache
specifier|static
name|int
name|refresh_cache
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|has_errors
init|=
literal|0
decl_stmt|;
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
decl_stmt|,
modifier|*
name|new
decl_stmt|;
name|ce
operator|=
name|active_cache
index|[
name|i
index|]
expr_stmt|;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
block|{
while|while
condition|(
operator|(
name|i
operator|<
name|active_nr
operator|)
operator|&&
operator|!
name|strcmp
argument_list|(
name|active_cache
index|[
name|i
index|]
operator|->
name|name
argument_list|,
name|ce
operator|->
name|name
argument_list|)
condition|)
name|i
operator|++
expr_stmt|;
name|i
operator|--
expr_stmt|;
if|if
condition|(
name|allow_unmerged
condition|)
continue|continue;
name|printf
argument_list|(
literal|"%s: needs merge\n"
argument_list|,
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
name|has_errors
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|new
operator|=
name|refresh_entry
argument_list|(
name|ce
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|new
condition|)
continue|continue;
if|if
condition|(
name|IS_ERR
argument_list|(
name|new
argument_list|)
condition|)
block|{
if|if
condition|(
name|not_new
operator|&&
name|PTR_ERR
argument_list|(
name|new
argument_list|)
operator|==
operator|-
name|ENOENT
condition|)
continue|continue;
if|if
condition|(
name|quiet
condition|)
continue|continue;
name|printf
argument_list|(
literal|"%s: needs update\n"
argument_list|,
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
name|has_errors
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|active_cache_changed
operator|=
literal|1
expr_stmt|;
comment|/* You can NOT just free active_cache[i] here, since it 		 * might not be necessarily malloc()ed but can also come 		 * from mmap(). */
name|active_cache
index|[
name|i
index|]
operator|=
name|new
expr_stmt|;
block|}
return|return
name|has_errors
return|;
block|}
end_function
begin_comment
comment|/*  * We fundamentally don't like some paths: we don't want  * dot or dot-dot anywhere, and for obvious reasons don't  * want to recurse into ".git" either.  *  * Also, we don't want double slashes or slashes at the  * end that can make pathnames ambiguous.  */
end_comment
begin_function
DECL|function|verify_dotfile
specifier|static
name|int
name|verify_dotfile
parameter_list|(
specifier|const
name|char
modifier|*
name|rest
parameter_list|)
block|{
comment|/* 	 * The first character was '.', but that 	 * has already been discarded, we now test 	 * the rest. 	 */
switch|switch
condition|(
operator|*
name|rest
condition|)
block|{
comment|/* "." is not allowed */
case|case
literal|'\0'
case|:
case|case
literal|'/'
case|:
return|return
literal|0
return|;
comment|/* 	 * ".git" followed by  NUL or slash is bad. This 	 * shares the path end test with the ".." case. 	 */
case|case
literal|'g'
case|:
if|if
condition|(
name|rest
index|[
literal|1
index|]
operator|!=
literal|'i'
condition|)
break|break;
if|if
condition|(
name|rest
index|[
literal|2
index|]
operator|!=
literal|'t'
condition|)
break|break;
name|rest
operator|+=
literal|2
expr_stmt|;
comment|/* fallthrough */
case|case
literal|'.'
case|:
if|if
condition|(
name|rest
index|[
literal|1
index|]
operator|==
literal|'\0'
operator|||
name|rest
index|[
literal|1
index|]
operator|==
literal|'/'
condition|)
return|return
literal|0
return|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|verify_path
specifier|static
name|int
name|verify_path
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|char
name|c
decl_stmt|;
goto|goto
name|inside
goto|;
for|for
control|(
init|;
condition|;
control|)
block|{
if|if
condition|(
operator|!
name|c
condition|)
return|return
literal|1
return|;
if|if
condition|(
name|c
operator|==
literal|'/'
condition|)
block|{
name|inside
label|:
name|c
operator|=
operator|*
name|path
operator|++
expr_stmt|;
switch|switch
condition|(
name|c
condition|)
block|{
default|default:
continue|continue;
case|case
literal|'/'
case|:
case|case
literal|'\0'
case|:
break|break;
case|case
literal|'.'
case|:
if|if
condition|(
name|verify_dotfile
argument_list|(
name|path
argument_list|)
condition|)
continue|continue;
block|}
return|return
literal|0
return|;
block|}
name|c
operator|=
operator|*
name|path
operator|++
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|add_cacheinfo
specifier|static
name|int
name|add_cacheinfo
parameter_list|(
specifier|const
name|char
modifier|*
name|arg1
parameter_list|,
specifier|const
name|char
modifier|*
name|arg2
parameter_list|,
specifier|const
name|char
modifier|*
name|arg3
parameter_list|)
block|{
name|int
name|size
decl_stmt|,
name|len
decl_stmt|,
name|option
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
if|if
condition|(
name|sscanf
argument_list|(
name|arg1
argument_list|,
literal|"%o"
argument_list|,
operator|&
name|mode
argument_list|)
operator|!=
literal|1
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|arg2
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|verify_path
argument_list|(
name|arg3
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|len
operator|=
name|strlen
argument_list|(
name|arg3
argument_list|)
expr_stmt|;
name|size
operator|=
name|cache_entry_size
argument_list|(
name|len
argument_list|)
expr_stmt|;
name|ce
operator|=
name|xmalloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|ce
argument_list|,
literal|0
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|arg3
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_flags
operator|=
name|htons
argument_list|(
name|len
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_mode
operator|=
name|create_ce_mode
argument_list|(
name|mode
argument_list|)
expr_stmt|;
name|option
operator|=
name|allow_add
condition|?
name|ADD_CACHE_OK_TO_ADD
else|:
literal|0
expr_stmt|;
name|option
operator||=
name|allow_replace
condition|?
name|ADD_CACHE_OK_TO_REPLACE
else|:
literal|0
expr_stmt|;
return|return
name|add_cache_entry
argument_list|(
name|ce
argument_list|,
name|option
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|chmod_path
specifier|static
name|int
name|chmod_path
parameter_list|(
name|int
name|flip
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|pos
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|path
argument_list|,
name|strlen
argument_list|(
name|path
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|ce
operator|=
name|active_cache
index|[
name|pos
index|]
expr_stmt|;
name|mode
operator|=
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|S_ISREG
argument_list|(
name|mode
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
switch|switch
condition|(
name|flip
condition|)
block|{
case|case
literal|'+'
case|:
name|ce
operator|->
name|ce_mode
operator||=
name|htonl
argument_list|(
literal|0111
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'-'
case|:
name|ce
operator|->
name|ce_mode
operator|&=
name|htonl
argument_list|(
operator|~
literal|0111
argument_list|)
expr_stmt|;
break|break;
default|default:
return|return
operator|-
literal|1
return|;
block|}
name|active_cache_changed
operator|=
literal|1
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|cache_file
specifier|static
name|struct
name|cache_file
name|cache_file
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|update_one
specifier|static
name|void
name|update_one
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|int
name|prefix_length
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|p
init|=
name|prefix_path
argument_list|(
name|prefix
argument_list|,
name|prefix_length
argument_list|,
name|path
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|verify_path
argument_list|(
name|p
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Ignoring path %s\n"
argument_list|,
name|path
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|force_remove
condition|)
block|{
if|if
condition|(
name|remove_file_from_cache
argument_list|(
name|p
argument_list|)
condition|)
name|die
argument_list|(
literal|"git-update-index: unable to remove %s"
argument_list|,
name|path
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|add_file_to_cache
argument_list|(
name|p
argument_list|)
condition|)
name|die
argument_list|(
literal|"Unable to process file %s"
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|read_index_info
specifier|static
name|void
name|read_index_info
parameter_list|(
name|int
name|line_termination
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
decl_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|char
modifier|*
name|ptr
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
name|read_line
argument_list|(
operator|&
name|buf
argument_list|,
name|stdin
argument_list|,
name|line_termination
argument_list|)
expr_stmt|;
if|if
condition|(
name|buf
operator|.
name|eof
condition|)
break|break;
name|mode
operator|=
name|strtoul
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
operator|&
name|ptr
argument_list|,
literal|8
argument_list|)
expr_stmt|;
if|if
condition|(
name|ptr
operator|==
name|buf
operator|.
name|buf
operator|||
operator|*
name|ptr
operator|!=
literal|' '
operator|||
name|get_sha1_hex
argument_list|(
name|ptr
operator|+
literal|1
argument_list|,
name|sha1
argument_list|)
operator|||
name|ptr
index|[
literal|41
index|]
operator|!=
literal|'\t'
condition|)
goto|goto
name|bad_line
goto|;
name|ptr
operator|+=
literal|42
expr_stmt|;
if|if
condition|(
operator|!
name|verify_path
argument_list|(
name|ptr
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Ignoring path %s\n"
argument_list|,
name|ptr
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|mode
condition|)
block|{
comment|/* mode == 0 means there is no such path -- remove */
if|if
condition|(
name|remove_file_from_cache
argument_list|(
name|ptr
argument_list|)
condition|)
name|die
argument_list|(
literal|"git-update-index: unable to remove %s"
argument_list|,
name|ptr
argument_list|)
expr_stmt|;
block|}
else|else
block|{
comment|/* mode ' ' sha1 '\t' name 			 * ptr[-1] points at tab, 			 * ptr[-41] is at the beginning of sha1 			 */
name|ptr
index|[
operator|-
literal|42
index|]
operator|=
name|ptr
index|[
operator|-
literal|1
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|add_cacheinfo
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
name|ptr
operator|-
literal|41
argument_list|,
name|ptr
argument_list|)
condition|)
name|die
argument_list|(
literal|"git-update-index: unable to update %s"
argument_list|,
name|ptr
argument_list|)
expr_stmt|;
block|}
continue|continue;
name|bad_line
label|:
name|die
argument_list|(
literal|"malformed index info %s"
argument_list|,
name|buf
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|newfd
decl_stmt|,
name|entries
decl_stmt|,
name|has_errors
init|=
literal|0
decl_stmt|,
name|line_termination
init|=
literal|'\n'
decl_stmt|;
name|int
name|allow_options
init|=
literal|1
decl_stmt|;
name|int
name|read_from_stdin
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|prefix
init|=
name|setup_git_directory
argument_list|()
decl_stmt|;
name|int
name|prefix_length
init|=
name|prefix
condition|?
name|strlen
argument_list|(
name|prefix
argument_list|)
else|:
literal|0
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|)
expr_stmt|;
name|newfd
operator|=
name|hold_index_file_for_update
argument_list|(
operator|&
name|cache_file
argument_list|,
name|get_index_file
argument_list|()
argument_list|)
expr_stmt|;
if|if
condition|(
name|newfd
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to create new cachefile"
argument_list|)
expr_stmt|;
name|entries
operator|=
name|read_cache
argument_list|()
expr_stmt|;
if|if
condition|(
name|entries
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"cache corrupted"
argument_list|)
expr_stmt|;
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
specifier|const
name|char
modifier|*
name|path
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|allow_options
operator|&&
operator|*
name|path
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|allow_options
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"-q"
argument_list|)
condition|)
block|{
name|quiet
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--add"
argument_list|)
condition|)
block|{
name|allow_add
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--replace"
argument_list|)
condition|)
block|{
name|allow_replace
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--remove"
argument_list|)
condition|)
block|{
name|allow_remove
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--unmerged"
argument_list|)
condition|)
block|{
name|allow_unmerged
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--refresh"
argument_list|)
condition|)
block|{
name|has_errors
operator||=
name|refresh_cache
argument_list|()
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--cacheinfo"
argument_list|)
condition|)
block|{
if|if
condition|(
name|i
operator|+
literal|3
operator|>=
name|argc
condition|)
name|die
argument_list|(
literal|"git-update-index: --cacheinfo<mode><sha1><path>"
argument_list|)
expr_stmt|;
if|if
condition|(
name|add_cacheinfo
argument_list|(
name|argv
index|[
name|i
operator|+
literal|1
index|]
argument_list|,
name|argv
index|[
name|i
operator|+
literal|2
index|]
argument_list|,
name|argv
index|[
name|i
operator|+
literal|3
index|]
argument_list|)
condition|)
name|die
argument_list|(
literal|"git-update-index: --cacheinfo cannot add %s"
argument_list|,
name|argv
index|[
name|i
operator|+
literal|3
index|]
argument_list|)
expr_stmt|;
name|i
operator|+=
literal|3
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--chmod=-x"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--chmod=+x"
argument_list|)
condition|)
block|{
if|if
condition|(
name|argc
operator|<=
name|i
operator|+
literal|1
condition|)
name|die
argument_list|(
literal|"git-update-index: %s<path>"
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|chmod_path
argument_list|(
name|path
index|[
literal|8
index|]
argument_list|,
name|argv
index|[
operator|++
name|i
index|]
argument_list|)
condition|)
name|die
argument_list|(
literal|"git-update-index: %s cannot chmod %s"
argument_list|,
name|path
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--info-only"
argument_list|)
condition|)
block|{
name|info_only
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--force-remove"
argument_list|)
condition|)
block|{
name|force_remove
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"-z"
argument_list|)
condition|)
block|{
name|line_termination
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--stdin"
argument_list|)
condition|)
block|{
if|if
condition|(
name|i
operator|!=
name|argc
operator|-
literal|1
condition|)
name|die
argument_list|(
literal|"--stdin must be at the end"
argument_list|)
expr_stmt|;
name|read_from_stdin
operator|=
literal|1
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--index-info"
argument_list|)
condition|)
block|{
name|allow_add
operator|=
name|allow_replace
operator|=
name|allow_remove
operator|=
literal|1
expr_stmt|;
name|read_index_info
argument_list|(
name|line_termination
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--ignore-missing"
argument_list|)
condition|)
block|{
name|not_new
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|die
argument_list|(
literal|"unknown option %s"
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
name|update_one
argument_list|(
name|path
argument_list|,
name|prefix
argument_list|,
name|prefix_length
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|read_from_stdin
condition|)
block|{
name|struct
name|strbuf
name|buf
decl_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|read_line
argument_list|(
operator|&
name|buf
argument_list|,
name|stdin
argument_list|,
name|line_termination
argument_list|)
expr_stmt|;
if|if
condition|(
name|buf
operator|.
name|eof
condition|)
break|break;
name|update_one
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
name|prefix
argument_list|,
name|prefix_length
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|active_cache_changed
condition|)
block|{
if|if
condition|(
name|write_cache
argument_list|(
name|newfd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
operator|||
name|commit_index_file
argument_list|(
operator|&
name|cache_file
argument_list|)
condition|)
name|die
argument_list|(
literal|"Unable to write new cachefile"
argument_list|)
expr_stmt|;
block|}
return|return
name|has_errors
condition|?
literal|1
else|:
literal|0
return|;
block|}
end_function
end_unit
