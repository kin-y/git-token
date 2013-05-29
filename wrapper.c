begin_unit
begin_comment
comment|/*  * Various trivial helper wrappers around standard functions  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_function
DECL|function|do_nothing
specifier|static
name|void
name|do_nothing
parameter_list|(
name|size_t
name|size
parameter_list|)
block|{ }
end_function
begin_function_decl
DECL|variable|try_to_free_routine
specifier|static
name|void
function_decl|(
modifier|*
name|try_to_free_routine
function_decl|)
parameter_list|(
name|size_t
name|size
parameter_list|)
init|=
name|do_nothing
function_decl|;
end_function_decl
begin_function
DECL|function|memory_limit_check
specifier|static
name|void
name|memory_limit_check
parameter_list|(
name|size_t
name|size
parameter_list|)
block|{
specifier|static
name|int
name|limit
init|=
operator|-
literal|1
decl_stmt|;
if|if
condition|(
name|limit
operator|==
operator|-
literal|1
condition|)
block|{
specifier|const
name|char
modifier|*
name|env
init|=
name|getenv
argument_list|(
literal|"GIT_ALLOC_LIMIT"
argument_list|)
decl_stmt|;
name|limit
operator|=
name|env
condition|?
name|atoi
argument_list|(
name|env
argument_list|)
operator|*
literal|1024
else|:
literal|0
expr_stmt|;
block|}
if|if
condition|(
name|limit
operator|&&
name|size
operator|>
name|limit
condition|)
name|die
argument_list|(
literal|"attempting to allocate %"
name|PRIuMAX
literal|" over limit %d"
argument_list|,
operator|(
name|intmax_t
operator|)
name|size
argument_list|,
name|limit
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|set_try_to_free_routine
name|try_to_free_t
name|set_try_to_free_routine
parameter_list|(
name|try_to_free_t
name|routine
parameter_list|)
block|{
name|try_to_free_t
name|old
init|=
name|try_to_free_routine
decl_stmt|;
if|if
condition|(
operator|!
name|routine
condition|)
name|routine
operator|=
name|do_nothing
expr_stmt|;
name|try_to_free_routine
operator|=
name|routine
expr_stmt|;
return|return
name|old
return|;
block|}
end_function
begin_function
DECL|function|xstrdup
name|char
modifier|*
name|xstrdup
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|)
block|{
name|char
modifier|*
name|ret
init|=
name|strdup
argument_list|(
name|str
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
block|{
name|try_to_free_routine
argument_list|(
name|strlen
argument_list|(
name|str
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
name|ret
operator|=
name|strdup
argument_list|(
name|str
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory, strdup failed"
argument_list|)
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|xmalloc
name|void
modifier|*
name|xmalloc
parameter_list|(
name|size_t
name|size
parameter_list|)
block|{
name|void
modifier|*
name|ret
decl_stmt|;
name|memory_limit_check
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|ret
operator|=
name|malloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
operator|&&
operator|!
name|size
condition|)
name|ret
operator|=
name|malloc
argument_list|(
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
block|{
name|try_to_free_routine
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|ret
operator|=
name|malloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
operator|&&
operator|!
name|size
condition|)
name|ret
operator|=
name|malloc
argument_list|(
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory, malloc failed (tried to allocate %lu bytes)"
argument_list|,
operator|(
name|unsigned
name|long
operator|)
name|size
argument_list|)
expr_stmt|;
block|}
ifdef|#
directive|ifdef
name|XMALLOC_POISON
name|memset
argument_list|(
name|ret
argument_list|,
literal|0xA5
argument_list|,
name|size
argument_list|)
expr_stmt|;
endif|#
directive|endif
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|xmallocz
name|void
modifier|*
name|xmallocz
parameter_list|(
name|size_t
name|size
parameter_list|)
block|{
name|void
modifier|*
name|ret
decl_stmt|;
if|if
condition|(
name|unsigned_add_overflows
argument_list|(
name|size
argument_list|,
literal|1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Data too large to fit into virtual memory space."
argument_list|)
expr_stmt|;
name|ret
operator|=
name|xmalloc
argument_list|(
name|size
operator|+
literal|1
argument_list|)
expr_stmt|;
operator|(
operator|(
name|char
operator|*
operator|)
name|ret
operator|)
index|[
name|size
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_comment
comment|/*  * xmemdupz() allocates (len + 1) bytes of memory, duplicates "len" bytes of  * "data" to the allocated memory, zero terminates the allocated memory,  * and returns a pointer to the allocated memory. If the allocation fails,  * the program dies.  */
end_comment
begin_function
DECL|function|xmemdupz
name|void
modifier|*
name|xmemdupz
parameter_list|(
specifier|const
name|void
modifier|*
name|data
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
return|return
name|memcpy
argument_list|(
name|xmallocz
argument_list|(
name|len
argument_list|)
argument_list|,
name|data
argument_list|,
name|len
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|xstrndup
name|char
modifier|*
name|xstrndup
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
name|char
modifier|*
name|p
init|=
name|memchr
argument_list|(
name|str
argument_list|,
literal|'\0'
argument_list|,
name|len
argument_list|)
decl_stmt|;
return|return
name|xmemdupz
argument_list|(
name|str
argument_list|,
name|p
condition|?
name|p
operator|-
name|str
else|:
name|len
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|xrealloc
name|void
modifier|*
name|xrealloc
parameter_list|(
name|void
modifier|*
name|ptr
parameter_list|,
name|size_t
name|size
parameter_list|)
block|{
name|void
modifier|*
name|ret
decl_stmt|;
name|memory_limit_check
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|ret
operator|=
name|realloc
argument_list|(
name|ptr
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
operator|&&
operator|!
name|size
condition|)
name|ret
operator|=
name|realloc
argument_list|(
name|ptr
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
block|{
name|try_to_free_routine
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|ret
operator|=
name|realloc
argument_list|(
name|ptr
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
operator|&&
operator|!
name|size
condition|)
name|ret
operator|=
name|realloc
argument_list|(
name|ptr
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory, realloc failed"
argument_list|)
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|xcalloc
name|void
modifier|*
name|xcalloc
parameter_list|(
name|size_t
name|nmemb
parameter_list|,
name|size_t
name|size
parameter_list|)
block|{
name|void
modifier|*
name|ret
decl_stmt|;
name|memory_limit_check
argument_list|(
name|size
operator|*
name|nmemb
argument_list|)
expr_stmt|;
name|ret
operator|=
name|calloc
argument_list|(
name|nmemb
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
operator|&&
operator|(
operator|!
name|nmemb
operator|||
operator|!
name|size
operator|)
condition|)
name|ret
operator|=
name|calloc
argument_list|(
literal|1
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
block|{
name|try_to_free_routine
argument_list|(
name|nmemb
operator|*
name|size
argument_list|)
expr_stmt|;
name|ret
operator|=
name|calloc
argument_list|(
name|nmemb
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
operator|&&
operator|(
operator|!
name|nmemb
operator|||
operator|!
name|size
operator|)
condition|)
name|ret
operator|=
name|calloc
argument_list|(
literal|1
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory, calloc failed"
argument_list|)
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_comment
comment|/*  * xread() is the same a read(), but it automatically restarts read()  * operations with a recoverable error (EAGAIN and EINTR). xread()  * DOES NOT GUARANTEE that "len" bytes is read even if the data is available.  */
end_comment
begin_function
DECL|function|xread
name|ssize_t
name|xread
parameter_list|(
name|int
name|fd
parameter_list|,
name|void
modifier|*
name|buf
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
name|ssize_t
name|nr
decl_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|nr
operator|=
name|read
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|nr
operator|<
literal|0
operator|)
operator|&&
operator|(
name|errno
operator|==
name|EAGAIN
operator|||
name|errno
operator|==
name|EINTR
operator|)
condition|)
continue|continue;
return|return
name|nr
return|;
block|}
block|}
end_function
begin_comment
comment|/*  * xwrite() is the same a write(), but it automatically restarts write()  * operations with a recoverable error (EAGAIN and EINTR). xwrite() DOES NOT  * GUARANTEE that "len" bytes is written even if the operation is successful.  */
end_comment
begin_function
DECL|function|xwrite
name|ssize_t
name|xwrite
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|void
modifier|*
name|buf
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
name|ssize_t
name|nr
decl_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|nr
operator|=
name|write
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|nr
operator|<
literal|0
operator|)
operator|&&
operator|(
name|errno
operator|==
name|EAGAIN
operator|||
name|errno
operator|==
name|EINTR
operator|)
condition|)
continue|continue;
return|return
name|nr
return|;
block|}
block|}
end_function
begin_function
DECL|function|read_in_full
name|ssize_t
name|read_in_full
parameter_list|(
name|int
name|fd
parameter_list|,
name|void
modifier|*
name|buf
parameter_list|,
name|size_t
name|count
parameter_list|)
block|{
name|char
modifier|*
name|p
init|=
name|buf
decl_stmt|;
name|ssize_t
name|total
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|count
operator|>
literal|0
condition|)
block|{
name|ssize_t
name|loaded
init|=
name|xread
argument_list|(
name|fd
argument_list|,
name|p
argument_list|,
name|count
argument_list|)
decl_stmt|;
if|if
condition|(
name|loaded
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|loaded
operator|==
literal|0
condition|)
return|return
name|total
return|;
name|count
operator|-=
name|loaded
expr_stmt|;
name|p
operator|+=
name|loaded
expr_stmt|;
name|total
operator|+=
name|loaded
expr_stmt|;
block|}
return|return
name|total
return|;
block|}
end_function
begin_function
DECL|function|write_in_full
name|ssize_t
name|write_in_full
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|void
modifier|*
name|buf
parameter_list|,
name|size_t
name|count
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|p
init|=
name|buf
decl_stmt|;
name|ssize_t
name|total
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|count
operator|>
literal|0
condition|)
block|{
name|ssize_t
name|written
init|=
name|xwrite
argument_list|(
name|fd
argument_list|,
name|p
argument_list|,
name|count
argument_list|)
decl_stmt|;
if|if
condition|(
name|written
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|written
condition|)
block|{
name|errno
operator|=
name|ENOSPC
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|count
operator|-=
name|written
expr_stmt|;
name|p
operator|+=
name|written
expr_stmt|;
name|total
operator|+=
name|written
expr_stmt|;
block|}
return|return
name|total
return|;
block|}
end_function
begin_function
DECL|function|xdup
name|int
name|xdup
parameter_list|(
name|int
name|fd
parameter_list|)
block|{
name|int
name|ret
init|=
name|dup
argument_list|(
name|fd
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"dup failed"
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|xfdopen
name|FILE
modifier|*
name|xfdopen
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|char
modifier|*
name|mode
parameter_list|)
block|{
name|FILE
modifier|*
name|stream
init|=
name|fdopen
argument_list|(
name|fd
argument_list|,
name|mode
argument_list|)
decl_stmt|;
if|if
condition|(
name|stream
operator|==
name|NULL
condition|)
name|die_errno
argument_list|(
literal|"Out of memory? fdopen failed"
argument_list|)
expr_stmt|;
return|return
name|stream
return|;
block|}
end_function
begin_function
DECL|function|xmkstemp
name|int
name|xmkstemp
parameter_list|(
name|char
modifier|*
name|template
parameter_list|)
block|{
name|int
name|fd
decl_stmt|;
name|char
name|origtemplate
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|strlcpy
argument_list|(
name|origtemplate
argument_list|,
name|template
argument_list|,
sizeof|sizeof
argument_list|(
name|origtemplate
argument_list|)
argument_list|)
expr_stmt|;
name|fd
operator|=
name|mkstemp
argument_list|(
name|template
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|int
name|saved_errno
init|=
name|errno
decl_stmt|;
specifier|const
name|char
modifier|*
name|nonrelative_template
decl_stmt|;
if|if
condition|(
name|strlen
argument_list|(
name|template
argument_list|)
operator|!=
name|strlen
argument_list|(
name|origtemplate
argument_list|)
condition|)
name|template
operator|=
name|origtemplate
expr_stmt|;
name|nonrelative_template
operator|=
name|absolute_path
argument_list|(
name|template
argument_list|)
expr_stmt|;
name|errno
operator|=
name|saved_errno
expr_stmt|;
name|die_errno
argument_list|(
literal|"Unable to create temporary file '%s'"
argument_list|,
name|nonrelative_template
argument_list|)
expr_stmt|;
block|}
return|return
name|fd
return|;
block|}
end_function
begin_comment
comment|/* git_mkstemp() - create tmp file honoring TMPDIR variable */
end_comment
begin_function
DECL|function|git_mkstemp
name|int
name|git_mkstemp
parameter_list|(
name|char
modifier|*
name|path
parameter_list|,
name|size_t
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|template
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|tmp
decl_stmt|;
name|size_t
name|n
decl_stmt|;
name|tmp
operator|=
name|getenv
argument_list|(
literal|"TMPDIR"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tmp
condition|)
name|tmp
operator|=
literal|"/tmp"
expr_stmt|;
name|n
operator|=
name|snprintf
argument_list|(
name|path
argument_list|,
name|len
argument_list|,
literal|"%s/%s"
argument_list|,
name|tmp
argument_list|,
name|template
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|<=
name|n
condition|)
block|{
name|errno
operator|=
name|ENAMETOOLONG
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
return|return
name|mkstemp
argument_list|(
name|path
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/* git_mkstemps() - create tmp file with suffix honoring TMPDIR variable. */
end_comment
begin_function
DECL|function|git_mkstemps
name|int
name|git_mkstemps
parameter_list|(
name|char
modifier|*
name|path
parameter_list|,
name|size_t
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|template
parameter_list|,
name|int
name|suffix_len
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|tmp
decl_stmt|;
name|size_t
name|n
decl_stmt|;
name|tmp
operator|=
name|getenv
argument_list|(
literal|"TMPDIR"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tmp
condition|)
name|tmp
operator|=
literal|"/tmp"
expr_stmt|;
name|n
operator|=
name|snprintf
argument_list|(
name|path
argument_list|,
name|len
argument_list|,
literal|"%s/%s"
argument_list|,
name|tmp
argument_list|,
name|template
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|<=
name|n
condition|)
block|{
name|errno
operator|=
name|ENAMETOOLONG
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
return|return
name|mkstemps
argument_list|(
name|path
argument_list|,
name|suffix_len
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/* Adapted from libiberty's mkstemp.c. */
end_comment
begin_undef
DECL|macro|TMP_MAX
undef|#
directive|undef
name|TMP_MAX
end_undef
begin_define
DECL|macro|TMP_MAX
define|#
directive|define
name|TMP_MAX
value|16384
end_define
begin_function
DECL|function|git_mkstemps_mode
name|int
name|git_mkstemps_mode
parameter_list|(
name|char
modifier|*
name|pattern
parameter_list|,
name|int
name|suffix_len
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
specifier|static
specifier|const
name|char
name|letters
index|[]
init|=
literal|"abcdefghijklmnopqrstuvwxyz"
literal|"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
literal|"0123456789"
decl_stmt|;
specifier|static
specifier|const
name|int
name|num_letters
init|=
literal|62
decl_stmt|;
name|uint64_t
name|value
decl_stmt|;
name|struct
name|timeval
name|tv
decl_stmt|;
name|char
modifier|*
name|template
decl_stmt|;
name|size_t
name|len
decl_stmt|;
name|int
name|fd
decl_stmt|,
name|count
decl_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|pattern
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|<
literal|6
operator|+
name|suffix_len
condition|)
block|{
name|errno
operator|=
name|EINVAL
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|strncmp
argument_list|(
operator|&
name|pattern
index|[
name|len
operator|-
literal|6
operator|-
name|suffix_len
index|]
argument_list|,
literal|"XXXXXX"
argument_list|,
literal|6
argument_list|)
condition|)
block|{
name|errno
operator|=
name|EINVAL
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
comment|/* 	 * Replace pattern's XXXXXX characters with randomness. 	 * Try TMP_MAX different filenames. 	 */
name|gettimeofday
argument_list|(
operator|&
name|tv
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|value
operator|=
operator|(
call|(
name|size_t
call|)
argument_list|(
name|tv
operator|.
name|tv_usec
operator|<<
literal|16
argument_list|)
operator|)
operator|^
name|tv
operator|.
name|tv_sec
operator|^
name|getpid
argument_list|()
expr_stmt|;
name|template
operator|=
operator|&
name|pattern
index|[
name|len
operator|-
literal|6
operator|-
name|suffix_len
index|]
expr_stmt|;
for|for
control|(
name|count
operator|=
literal|0
init|;
name|count
operator|<
name|TMP_MAX
condition|;
operator|++
name|count
control|)
block|{
name|uint64_t
name|v
init|=
name|value
decl_stmt|;
comment|/* Fill in the random bits. */
name|template
index|[
literal|0
index|]
operator|=
name|letters
index|[
name|v
operator|%
name|num_letters
index|]
expr_stmt|;
name|v
operator|/=
name|num_letters
expr_stmt|;
name|template
index|[
literal|1
index|]
operator|=
name|letters
index|[
name|v
operator|%
name|num_letters
index|]
expr_stmt|;
name|v
operator|/=
name|num_letters
expr_stmt|;
name|template
index|[
literal|2
index|]
operator|=
name|letters
index|[
name|v
operator|%
name|num_letters
index|]
expr_stmt|;
name|v
operator|/=
name|num_letters
expr_stmt|;
name|template
index|[
literal|3
index|]
operator|=
name|letters
index|[
name|v
operator|%
name|num_letters
index|]
expr_stmt|;
name|v
operator|/=
name|num_letters
expr_stmt|;
name|template
index|[
literal|4
index|]
operator|=
name|letters
index|[
name|v
operator|%
name|num_letters
index|]
expr_stmt|;
name|v
operator|/=
name|num_letters
expr_stmt|;
name|template
index|[
literal|5
index|]
operator|=
name|letters
index|[
name|v
operator|%
name|num_letters
index|]
expr_stmt|;
name|v
operator|/=
name|num_letters
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|pattern
argument_list|,
name|O_CREAT
operator||
name|O_EXCL
operator||
name|O_RDWR
argument_list|,
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|>
literal|0
condition|)
return|return
name|fd
return|;
comment|/* 		 * Fatal error (EPERM, ENOSPC etc). 		 * It doesn't make sense to loop. 		 */
if|if
condition|(
name|errno
operator|!=
name|EEXIST
condition|)
break|break;
comment|/* 		 * This is a random value.  It is only necessary that 		 * the next TMP_MAX values generated by adding 7777 to 		 * VALUE are different with (module 2^32). 		 */
name|value
operator|+=
literal|7777
expr_stmt|;
block|}
comment|/* We return the null string if we can't find a unique file name.  */
name|pattern
index|[
literal|0
index|]
operator|=
literal|'\0'
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|git_mkstemp_mode
name|int
name|git_mkstemp_mode
parameter_list|(
name|char
modifier|*
name|pattern
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
comment|/* mkstemp is just mkstemps with no suffix */
return|return
name|git_mkstemps_mode
argument_list|(
name|pattern
argument_list|,
literal|0
argument_list|,
name|mode
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|gitmkstemps
name|int
name|gitmkstemps
parameter_list|(
name|char
modifier|*
name|pattern
parameter_list|,
name|int
name|suffix_len
parameter_list|)
block|{
return|return
name|git_mkstemps_mode
argument_list|(
name|pattern
argument_list|,
name|suffix_len
argument_list|,
literal|0600
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|xmkstemp_mode
name|int
name|xmkstemp_mode
parameter_list|(
name|char
modifier|*
name|template
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
name|int
name|fd
decl_stmt|;
name|char
name|origtemplate
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|strlcpy
argument_list|(
name|origtemplate
argument_list|,
name|template
argument_list|,
sizeof|sizeof
argument_list|(
name|origtemplate
argument_list|)
argument_list|)
expr_stmt|;
name|fd
operator|=
name|git_mkstemp_mode
argument_list|(
name|template
argument_list|,
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|int
name|saved_errno
init|=
name|errno
decl_stmt|;
specifier|const
name|char
modifier|*
name|nonrelative_template
decl_stmt|;
if|if
condition|(
operator|!
name|template
index|[
literal|0
index|]
condition|)
name|template
operator|=
name|origtemplate
expr_stmt|;
name|nonrelative_template
operator|=
name|absolute_path
argument_list|(
name|template
argument_list|)
expr_stmt|;
name|errno
operator|=
name|saved_errno
expr_stmt|;
name|die_errno
argument_list|(
literal|"Unable to create temporary file '%s'"
argument_list|,
name|nonrelative_template
argument_list|)
expr_stmt|;
block|}
return|return
name|fd
return|;
block|}
end_function
begin_function
DECL|function|warn_if_unremovable
specifier|static
name|int
name|warn_if_unremovable
parameter_list|(
specifier|const
name|char
modifier|*
name|op
parameter_list|,
specifier|const
name|char
modifier|*
name|file
parameter_list|,
name|int
name|rc
parameter_list|)
block|{
if|if
condition|(
name|rc
operator|<
literal|0
condition|)
block|{
name|int
name|err
init|=
name|errno
decl_stmt|;
if|if
condition|(
name|ENOENT
operator|!=
name|err
condition|)
block|{
name|warning
argument_list|(
literal|"unable to %s %s: %s"
argument_list|,
name|op
argument_list|,
name|file
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|errno
operator|=
name|err
expr_stmt|;
block|}
block|}
return|return
name|rc
return|;
block|}
end_function
begin_function
DECL|function|unlink_or_warn
name|int
name|unlink_or_warn
parameter_list|(
specifier|const
name|char
modifier|*
name|file
parameter_list|)
block|{
return|return
name|warn_if_unremovable
argument_list|(
literal|"unlink"
argument_list|,
name|file
argument_list|,
name|unlink
argument_list|(
name|file
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|rmdir_or_warn
name|int
name|rmdir_or_warn
parameter_list|(
specifier|const
name|char
modifier|*
name|file
parameter_list|)
block|{
return|return
name|warn_if_unremovable
argument_list|(
literal|"rmdir"
argument_list|,
name|file
argument_list|,
name|rmdir
argument_list|(
name|file
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|remove_or_warn
name|int
name|remove_or_warn
parameter_list|(
name|unsigned
name|int
name|mode
parameter_list|,
specifier|const
name|char
modifier|*
name|file
parameter_list|)
block|{
return|return
name|S_ISGITLINK
argument_list|(
name|mode
argument_list|)
condition|?
name|rmdir_or_warn
argument_list|(
name|file
argument_list|)
else|:
name|unlink_or_warn
argument_list|(
name|file
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|warn_on_inaccessible
name|void
name|warn_on_inaccessible
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|warning
argument_list|(
name|_
argument_list|(
literal|"unable to access '%s': %s"
argument_list|)
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
DECL|function|access_error_is_ok
specifier|static
name|int
name|access_error_is_ok
parameter_list|(
name|int
name|err
parameter_list|,
name|unsigned
name|flag
parameter_list|)
block|{
return|return
name|err
operator|==
name|ENOENT
operator|||
name|err
operator|==
name|ENOTDIR
operator|||
operator|(
operator|(
name|flag
operator|&
name|ACCESS_EACCES_OK
operator|)
operator|&&
name|err
operator|==
name|EACCES
operator|)
return|;
block|}
end_function
begin_function
DECL|function|access_or_warn
name|int
name|access_or_warn
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|mode
parameter_list|,
name|unsigned
name|flag
parameter_list|)
block|{
name|int
name|ret
init|=
name|access
argument_list|(
name|path
argument_list|,
name|mode
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
operator|&&
operator|!
name|access_error_is_ok
argument_list|(
name|errno
argument_list|,
name|flag
argument_list|)
condition|)
name|warn_on_inaccessible
argument_list|(
name|path
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|access_or_die
name|int
name|access_or_die
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|mode
parameter_list|,
name|unsigned
name|flag
parameter_list|)
block|{
name|int
name|ret
init|=
name|access
argument_list|(
name|path
argument_list|,
name|mode
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
operator|&&
operator|!
name|access_error_is_ok
argument_list|(
name|errno
argument_list|,
name|flag
argument_list|)
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"unable to access '%s'"
argument_list|)
argument_list|,
name|path
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|xgetpwuid_self
name|struct
name|passwd
modifier|*
name|xgetpwuid_self
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|passwd
modifier|*
name|pw
decl_stmt|;
name|errno
operator|=
literal|0
expr_stmt|;
name|pw
operator|=
name|getpwuid
argument_list|(
name|getuid
argument_list|()
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pw
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"unable to look up current user in the passwd file: %s"
argument_list|)
argument_list|,
name|errno
condition|?
name|strerror
argument_list|(
name|errno
argument_list|)
else|:
name|_
argument_list|(
literal|"no such user"
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|pw
return|;
block|}
end_function
end_unit
