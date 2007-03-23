begin_unit
begin_include
include|#
directive|include
file|<winsock2.h>
end_include
begin_comment
comment|/*  * things that are not available in header files  */
end_comment
begin_typedef
DECL|typedef|pid_t
typedef|typedef
name|int
name|pid_t
typedef|;
end_typedef
begin_define
DECL|macro|hstrerror
define|#
directive|define
name|hstrerror
value|strerror
end_define
begin_define
DECL|macro|S_IFLNK
define|#
directive|define
name|S_IFLNK
value|0120000
end_define
begin_comment
DECL|macro|S_IFLNK
comment|/* Symbolic link */
end_comment
begin_define
DECL|macro|S_ISLNK
define|#
directive|define
name|S_ISLNK
parameter_list|(
name|x
parameter_list|)
value|(((x)& S_IFMT) == S_IFLNK)
end_define
begin_define
DECL|macro|S_ISSOCK
define|#
directive|define
name|S_ISSOCK
parameter_list|(
name|x
parameter_list|)
value|0
end_define
begin_define
DECL|macro|S_IRGRP
define|#
directive|define
name|S_IRGRP
value|0
end_define
begin_define
DECL|macro|S_IWGRP
define|#
directive|define
name|S_IWGRP
value|0
end_define
begin_define
DECL|macro|S_IXGRP
define|#
directive|define
name|S_IXGRP
value|0
end_define
begin_define
DECL|macro|S_ISGID
define|#
directive|define
name|S_ISGID
value|0
end_define
begin_define
DECL|macro|S_IROTH
define|#
directive|define
name|S_IROTH
value|0
end_define
begin_define
DECL|macro|S_IXOTH
define|#
directive|define
name|S_IXOTH
value|0
end_define
begin_define
DECL|macro|WIFEXITED
define|#
directive|define
name|WIFEXITED
parameter_list|(
name|x
parameter_list|)
value|((unsigned)(x)< 259)
end_define
begin_comment
DECL|macro|WIFEXITED
comment|/* STILL_ACTIVE */
end_comment
begin_define
DECL|macro|WEXITSTATUS
define|#
directive|define
name|WEXITSTATUS
parameter_list|(
name|x
parameter_list|)
value|((x)& 0xff)
end_define
begin_define
DECL|macro|WIFSIGNALED
define|#
directive|define
name|WIFSIGNALED
parameter_list|(
name|x
parameter_list|)
value|((unsigned)(x)> 259)
end_define
begin_define
DECL|macro|SIGKILL
define|#
directive|define
name|SIGKILL
value|0
end_define
begin_define
DECL|macro|SIGCHLD
define|#
directive|define
name|SIGCHLD
value|0
end_define
begin_define
DECL|macro|SIGPIPE
define|#
directive|define
name|SIGPIPE
value|0
end_define
begin_define
DECL|macro|SIGHUP
define|#
directive|define
name|SIGHUP
value|0
end_define
begin_define
DECL|macro|SIGQUIT
define|#
directive|define
name|SIGQUIT
value|0
end_define
begin_define
DECL|macro|SIGALRM
define|#
directive|define
name|SIGALRM
value|100
end_define
begin_define
DECL|macro|F_GETFD
define|#
directive|define
name|F_GETFD
value|1
end_define
begin_define
DECL|macro|F_SETFD
define|#
directive|define
name|F_SETFD
value|2
end_define
begin_define
DECL|macro|FD_CLOEXEC
define|#
directive|define
name|FD_CLOEXEC
value|0x1
end_define
begin_struct
DECL|struct|passwd
struct|struct
name|passwd
block|{
DECL|member|pw_name
name|char
modifier|*
name|pw_name
decl_stmt|;
DECL|member|pw_gecos
name|char
modifier|*
name|pw_gecos
decl_stmt|;
DECL|member|pw_dir
name|char
modifier|*
name|pw_dir
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|pollfd
struct|struct
name|pollfd
block|{
DECL|member|fd
name|int
name|fd
decl_stmt|;
comment|/* file descriptor */
DECL|member|events
name|short
name|events
decl_stmt|;
comment|/* requested events */
DECL|member|revents
name|short
name|revents
decl_stmt|;
comment|/* returned events */
block|}
struct|;
end_struct
begin_define
DECL|macro|POLLIN
define|#
directive|define
name|POLLIN
value|1
end_define
begin_define
DECL|macro|POLLHUP
define|#
directive|define
name|POLLHUP
value|2
end_define
begin_typedef
DECL|typedef|sig_handler_t
typedef|typedef
name|void
function_decl|(
name|__cdecl
modifier|*
name|sig_handler_t
function_decl|)
parameter_list|(
name|int
parameter_list|)
function_decl|;
end_typedef
begin_struct
DECL|struct|sigaction
struct|struct
name|sigaction
block|{
DECL|member|sa_handler
name|sig_handler_t
name|sa_handler
decl_stmt|;
DECL|member|sa_flags
name|unsigned
name|sa_flags
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|sigemptyset
define|#
directive|define
name|sigemptyset
parameter_list|(
name|x
parameter_list|)
value|(void)0
end_define
begin_define
DECL|macro|SA_RESTART
define|#
directive|define
name|SA_RESTART
value|0
end_define
begin_struct
DECL|struct|itimerval
struct|struct
name|itimerval
block|{
DECL|member|it_value
DECL|member|it_interval
name|struct
name|timeval
name|it_value
decl_stmt|,
name|it_interval
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|ITIMER_REAL
define|#
directive|define
name|ITIMER_REAL
value|0
end_define
begin_define
DECL|macro|st_blocks
define|#
directive|define
name|st_blocks
value|st_size/512
end_define
begin_comment
DECL|macro|st_blocks
comment|/* will be cleaned up later */
end_comment
begin_define
DECL|macro|lstat
define|#
directive|define
name|lstat
value|stat
end_define
begin_comment
comment|/*  * trivial stubs  */
end_comment
begin_function
DECL|function|readlink
specifier|static
specifier|inline
name|int
name|readlink
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
name|size_t
name|bufsiz
parameter_list|)
block|{
name|errno
operator|=
name|ENOSYS
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|symlink
specifier|static
specifier|inline
name|int
name|symlink
parameter_list|(
specifier|const
name|char
modifier|*
name|oldpath
parameter_list|,
specifier|const
name|char
modifier|*
name|newpath
parameter_list|)
block|{
name|errno
operator|=
name|ENOSYS
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|link
specifier|static
specifier|inline
name|int
name|link
parameter_list|(
specifier|const
name|char
modifier|*
name|oldpath
parameter_list|,
specifier|const
name|char
modifier|*
name|newpath
parameter_list|)
block|{
name|errno
operator|=
name|ENOSYS
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|fchmod
specifier|static
specifier|inline
name|int
name|fchmod
parameter_list|(
name|int
name|fildes
parameter_list|,
name|mode_t
name|mode
parameter_list|)
block|{
name|errno
operator|=
name|ENOSYS
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|fork
specifier|static
specifier|inline
name|int
name|fork
parameter_list|(
name|void
parameter_list|)
block|{
name|errno
operator|=
name|ENOSYS
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|alarm
specifier|static
specifier|inline
name|unsigned
name|int
name|alarm
parameter_list|(
name|unsigned
name|int
name|seconds
parameter_list|)
block|{
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|fsync
specifier|static
specifier|inline
name|int
name|fsync
parameter_list|(
name|int
name|fd
parameter_list|)
block|{
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|getppid
specifier|static
specifier|inline
name|int
name|getppid
parameter_list|(
name|void
parameter_list|)
block|{
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|sync
specifier|static
specifier|inline
name|void
name|sync
parameter_list|(
name|void
parameter_list|)
block|{}
end_function
begin_function
DECL|function|getuid
specifier|static
specifier|inline
name|int
name|getuid
parameter_list|()
block|{
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|getpwnam
specifier|static
specifier|inline
name|struct
name|passwd
modifier|*
name|getpwnam
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|fcntl
specifier|static
specifier|inline
name|int
name|fcntl
parameter_list|(
name|int
name|fd
parameter_list|,
name|int
name|cmd
parameter_list|,
name|long
name|arg
parameter_list|)
block|{
if|if
condition|(
name|cmd
operator|==
name|F_GETFD
operator|||
name|cmd
operator|==
name|F_SETFD
condition|)
return|return
literal|0
return|;
name|errno
operator|=
name|EINVAL
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_comment
comment|/*  * simple adaptors  */
end_comment
begin_function
DECL|function|mingw_mkdir
specifier|static
specifier|inline
name|int
name|mingw_mkdir
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
return|return
name|mkdir
argument_list|(
name|path
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|mkdir
define|#
directive|define
name|mkdir
value|mingw_mkdir
end_define
begin_function
DECL|function|mingw_unlink
specifier|static
specifier|inline
name|int
name|mingw_unlink
parameter_list|(
specifier|const
name|char
modifier|*
name|pathname
parameter_list|)
block|{
comment|/* read-only files cannot be removed */
name|chmod
argument_list|(
name|pathname
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
return|return
name|unlink
argument_list|(
name|pathname
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|unlink
define|#
directive|define
name|unlink
value|mingw_unlink
end_define
begin_function
DECL|function|waitpid
specifier|static
specifier|inline
name|int
name|waitpid
parameter_list|(
name|pid_t
name|pid
parameter_list|,
name|unsigned
modifier|*
name|status
parameter_list|,
name|unsigned
name|options
parameter_list|)
block|{
if|if
condition|(
name|options
operator|==
literal|0
condition|)
return|return
name|_cwait
argument_list|(
name|status
argument_list|,
name|pid
argument_list|,
literal|0
argument_list|)
return|;
name|errno
operator|=
name|EINVAL
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|pipe
specifier|static
specifier|inline
name|int
name|pipe
parameter_list|(
name|int
name|filedes
index|[
literal|2
index|]
parameter_list|)
block|{
return|return
name|_pipe
argument_list|(
name|filedes
argument_list|,
literal|8192
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/*  * implementations of missing functions  */
end_comment
begin_function_decl
name|unsigned
name|int
name|sleep
parameter_list|(
name|unsigned
name|int
name|seconds
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|mkstemp
parameter_list|(
name|char
modifier|*
name|template
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
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
function_decl|;
end_function_decl
begin_function_decl
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
function_decl|;
end_function_decl
begin_function_decl
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
function_decl|;
end_function_decl
begin_function_decl
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
function_decl|;
end_function_decl
begin_function_decl
name|int
name|getpagesize
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* defined in MinGW's libgcc.a */
end_comment
begin_function_decl
name|struct
name|passwd
modifier|*
name|getpwuid
parameter_list|(
name|int
name|uid
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
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
function_decl|;
end_function_decl
begin_function_decl
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
function_decl|;
end_function_decl
begin_comment
comment|/*  * replacements of existing functions  */
end_comment
begin_function_decl
name|int
name|mingw_open
parameter_list|(
specifier|const
name|char
modifier|*
name|filename
parameter_list|,
name|int
name|oflags
parameter_list|,
modifier|...
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|open
define|#
directive|define
name|open
value|mingw_open
end_define
begin_function_decl
name|char
modifier|*
name|mingw_getcwd
parameter_list|(
name|char
modifier|*
name|pointer
parameter_list|,
name|int
name|len
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|getcwd
define|#
directive|define
name|getcwd
value|mingw_getcwd
end_define
begin_function_decl
name|int
name|mingw_rename
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|rename
define|#
directive|define
name|rename
value|mingw_rename
end_define
begin_comment
comment|/*  * git specific compatibility  */
end_comment
begin_define
DECL|macro|has_dos_drive_prefix
define|#
directive|define
name|has_dos_drive_prefix
parameter_list|(
name|path
parameter_list|)
value|(isalpha(*(path))&& (path)[1] == ':')
end_define
begin_define
DECL|macro|is_dir_sep
define|#
directive|define
name|is_dir_sep
parameter_list|(
name|c
parameter_list|)
value|((c) == '/' || (c) == '\\')
end_define
begin_define
DECL|macro|PATH_SEP
define|#
directive|define
name|PATH_SEP
value|';'
end_define
begin_define
DECL|macro|PRIuMAX
define|#
directive|define
name|PRIuMAX
value|"I64u"
end_define
end_unit
