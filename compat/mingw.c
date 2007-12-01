begin_unit
begin_include
include|#
directive|include
file|"../git-compat-util.h"
end_include
begin_decl_stmt
DECL|variable|_CRT_fmode
name|unsigned
name|int
name|_CRT_fmode
init|=
name|_O_BINARY
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|sleep
name|unsigned
name|int
name|sleep
parameter_list|(
name|unsigned
name|int
name|seconds
parameter_list|)
block|{
name|Sleep
argument_list|(
name|seconds
operator|*
literal|1000
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|mkstemp
name|int
name|mkstemp
parameter_list|(
name|char
modifier|*
name|template
parameter_list|)
block|{
name|char
modifier|*
name|filename
init|=
name|mktemp
argument_list|(
name|template
argument_list|)
decl_stmt|;
if|if
condition|(
name|filename
operator|==
name|NULL
condition|)
return|return
operator|-
literal|1
return|;
return|return
name|open
argument_list|(
name|filename
argument_list|,
name|O_RDWR
operator||
name|O_CREAT
argument_list|,
literal|0600
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|gettimeofday
name|int
name|gettimeofday
parameter_list|(
name|struct
name|timeval
modifier|*
name|tv
parameter_list|,
name|void
modifier|*
name|tz
parameter_list|)
block|{
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|poll
name|int
name|poll
parameter_list|(
name|struct
name|pollfd
modifier|*
name|ufds
parameter_list|,
name|unsigned
name|int
name|nfds
parameter_list|,
name|int
name|timeout
parameter_list|)
block|{
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|gmtime_r
name|struct
name|tm
modifier|*
name|gmtime_r
parameter_list|(
specifier|const
name|time_t
modifier|*
name|timep
parameter_list|,
name|struct
name|tm
modifier|*
name|result
parameter_list|)
block|{
comment|/* gmtime() in MSVCRT.DLL is thread-safe, but not reentrant */
name|memcpy
argument_list|(
name|result
argument_list|,
name|gmtime
argument_list|(
name|timep
argument_list|)
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|tm
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|localtime_r
name|struct
name|tm
modifier|*
name|localtime_r
parameter_list|(
specifier|const
name|time_t
modifier|*
name|timep
parameter_list|,
name|struct
name|tm
modifier|*
name|result
parameter_list|)
block|{
comment|/* localtime() in MSVCRT.DLL is thread-safe, but not reentrant */
name|memcpy
argument_list|(
name|result
argument_list|,
name|localtime
argument_list|(
name|timep
argument_list|)
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|tm
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|getpwuid
name|struct
name|passwd
modifier|*
name|getpwuid
parameter_list|(
name|int
name|uid
parameter_list|)
block|{
specifier|static
name|struct
name|passwd
name|p
decl_stmt|;
return|return
operator|&
name|p
return|;
block|}
end_function
begin_function
DECL|function|setitimer
name|int
name|setitimer
parameter_list|(
name|int
name|type
parameter_list|,
name|struct
name|itimerval
modifier|*
name|in
parameter_list|,
name|struct
name|itimerval
modifier|*
name|out
parameter_list|)
block|{
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|sigaction
name|int
name|sigaction
parameter_list|(
name|int
name|sig
parameter_list|,
name|struct
name|sigaction
modifier|*
name|in
parameter_list|,
name|struct
name|sigaction
modifier|*
name|out
parameter_list|)
block|{
return|return
operator|-
literal|1
return|;
block|}
end_function
end_unit
